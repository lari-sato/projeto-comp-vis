#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "processamento.h"

SDL_Surface *carregarImagem(const char *caminho) {
    SDL_Surface *surface = IMG_Load(caminho);

    if (surface == NULL) {
        fprintf(stderr, "Erro ao carregar imagem '%s'. \nMotivo: %s\n", caminho, SDL_GetError());
        return NULL;
    }
    return surface;
}

bool verificarEscalaCinza(SDL_Surface *surface) {
    if (surface == NULL) return false;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                fprintf(stderr, "Erro ao ler pixel (%d, %d). Motivo: %s\n", x, y, SDL_GetError());
                return false;
            }

            if (!(r == g && g == b)) {
                return false;
            }
        }
    }

    return true;
}

SDL_Surface *conversaoEscalaCinza(SDL_Surface *surface) {
    if (surface == NULL) return NULL;

    SDL_Surface *cinza = SDL_CreateSurface(surface->w, surface->h, surface->format);

    if (cinza == NULL) {
        fprintf(stderr, "Erro ao criar surface em cinza. Motivo: %s\n", SDL_GetError());
        return NULL;
    }

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                fprintf(stderr, "Erro ao ler pixel (%d, %d). Motivo: %s\n", x, y, SDL_GetError());
                SDL_DestroySurface(cinza);
                return NULL;
            }

            float yFloat = 0.2125f * r + 0.7154f * g + 0.0721f * b;

            if (yFloat < 0.0f) yFloat = 0.0f;
            if (yFloat > 255.0f) yFloat = 255.0f;

            Uint8 valorCinza = (Uint8)(yFloat + 0.5f);

            if (!SDL_WriteSurfacePixel(cinza, x, y, valorCinza, valorCinza, valorCinza, a)) {
                fprintf(stderr, "Erro ao escrever pixel (%d, %d). Motivo: %s\n", x, y, SDL_GetError());
                SDL_DestroySurface(cinza);
                return NULL;
            }
        }
    }

    return cinza;
}

bool prepararImagem(const char *caminho, DadosImagem *img) {
    if (img == NULL) return false;

    img->imagemOriginal = carregarImagem(caminho);

    if (img->imagemOriginal == NULL) {
        return false;
    }

    img->ehEscalaCinza = verificarEscalaCinza(img->imagemOriginal);

    if (img->ehEscalaCinza) {
        img->imagemCinza = SDL_CreateSurface(
            img->imagemOriginal->w,
            img->imagemOriginal->h,
            img->imagemOriginal->format
        );

        if (img->imagemCinza == NULL) {
            fprintf(stderr, "Erro ao criar cópia da imagem. Motivo: %s\n", SDL_GetError());
            SDL_DestroySurface(img->imagemOriginal);
            return false;
        }

        if (!SDL_BlitSurface(img->imagemOriginal, NULL, img->imagemCinza, NULL)) {
            fprintf(stderr, "Erro ao copiar imagem. Motivo: %s\n", SDL_GetError());
            SDL_DestroySurface(img->imagemCinza);
            SDL_DestroySurface(img->imagemOriginal);
            return false;
        }
    } else {
        img->imagemCinza = conversaoEscalaCinza(img->imagemOriginal);
        if (img->imagemCinza == NULL) {
            SDL_DestroySurface(img->imagemOriginal);
            return false;
        }
    }

    return true;
}

int rodarGui(int img_width, int img_height, const int *histograma, DadosImagem *img) {
    GUI gui = {0};

    if (!SDL_CreateWindowAndRenderer("Processamento de Imagem", img_width, img_height, 0, 
                                   &gui.janelaPrincipal, &gui.rendererPrincipal)) {
        SDL_Log("Erro ao criar janela: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Texture *img_texture = SDL_CreateTextureFromSurface(gui.rendererPrincipal, img->imagemCinza);
    if (!img_texture) {
        SDL_Log("Erro ao criar textura: %s", SDL_GetError());
        SDL_DestroyRenderer(gui.rendererPrincipal);
        SDL_DestroyWindow(gui.janelaPrincipal);
        return SDL_APP_FAILURE;
    }

    int hist_width = 650;
    int hist_height = img_height;
    int offset_x = img_width + 50;
    int offset_y = 0;

    gui.janelaHistograma = SDL_CreatePopupWindow(
        gui.janelaPrincipal,
        offset_x,
        offset_y,
        hist_width,
        hist_height,
        SDL_WINDOW_POPUP_MENU
    );

    if (!gui.janelaHistograma) {
        SDL_Log("Erro ao criar janela de histograma: %s", SDL_GetError());
        SDL_DestroyRenderer(gui.rendererPrincipal);
        SDL_DestroyWindow(gui.janelaPrincipal);
        return SDL_APP_FAILURE;
    }

    gui.rendererHistograma = SDL_CreateRenderer(gui.janelaHistograma, NULL);
    if (!gui.rendererHistograma) {
        SDL_Log("Erro ao criar renderer de histograma: %s", SDL_GetError());
        SDL_DestroyRenderer(gui.rendererPrincipal);
        SDL_DestroyWindow(gui.janelaHistograma);
        SDL_DestroyWindow(gui.janelaPrincipal);
        return SDL_APP_FAILURE;
    }

    SDL_Event event;
    bool isRunning = true;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                isRunning = false;
            }
        }

        SDL_SetRenderDrawColor(gui.rendererPrincipal, 0, 0, 0, 255);
        SDL_RenderClear(gui.rendererPrincipal);

        SDL_FRect dst = { 0, 0, (float)img_width, (float)img_height };
        SDL_RenderTexture(gui.rendererPrincipal, img_texture, NULL, &dst);

        SDL_RenderPresent(gui.rendererPrincipal);

        SDL_SetRenderDrawColor(gui.rendererHistograma, 50, 50, 50, 255);
        SDL_RenderClear(gui.rendererHistograma);

        int bar_x = 10;
        int bar_y = 20;
        int bar_w = 4;
        int bar_h_max = hist_height - 100;
        int num_barras = 64;

        int max_val = 0;
        for (int i = 0; i < 256; i++) {
            if (histograma[i] > max_val) max_val = histograma[i];
        }

        if (max_val > 0) {
            for (int i = 0; i < num_barras; i++) {
                int index = (i * 256) / num_barras;
                float ratio = (float)histograma[index] / (float)max_val;
                int bar_h = (int)(ratio * bar_h_max);
                
                SDL_SetRenderDrawColor(gui.rendererHistograma, 255, 255, 255, 255);
                SDL_FRect bar = {
                    (float)(bar_x + i * (bar_w + 1)),
                    (float)(bar_y + bar_h_max - bar_h),
                    (float)bar_w,
                    (float)bar_h
                };
                SDL_RenderFillRect(gui.rendererHistograma, &bar);
            }
        }
        SDL_RenderPresent(gui.rendererHistograma);
    }

    SDL_DestroyRenderer(gui.rendererHistograma);
    SDL_DestroyRenderer(gui.rendererPrincipal);
    SDL_DestroyWindow(gui.janelaHistograma);
    SDL_DestroyWindow(gui.janelaPrincipal);

    return SDL_APP_SUCCESS;
}

void liberarImagem(DadosImagem *img) {
    if (img == NULL) return;

    if (img->imagemOriginal != NULL) {
        SDL_DestroySurface(img->imagemOriginal);
        img->imagemOriginal = NULL;
    }

    if (img->imagemCinza != NULL) {
        SDL_DestroySurface(img->imagemCinza);
        img->imagemCinza = NULL;
    }
}