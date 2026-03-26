// Beatriz Lima de Moura 10416616
// Giovana Simões Franco 10417646
// Julia Santos Oliveira 10417672
// Larissa Yuri Sato 10418318

#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
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

int rodarGui(int img_width, int img_height, const int *histogramaInicial, DadosImagem *img) {
    GUI gui = {0};

    if (!SDL_CreateWindowAndRenderer("Processamento de Imagem", img_width, img_height, 0, 
                                   &gui.janelaPrincipal, &gui.rendererPrincipal)) {
        return SDL_APP_FAILURE;
    }

    int hist_width = 700;
    int hist_height = 500;
    gui.janelaHistograma = SDL_CreatePopupWindow(gui.janelaPrincipal, img_width + 50, 0, hist_width, hist_height, SDL_WINDOW_POPUP_MENU);
    gui.rendererHistograma = SDL_CreateRenderer(gui.janelaHistograma, NULL);

    TTF_Font *fonte = TTF_OpenFont("arial.ttf", 20);
    SDL_Color corTexto = {255, 255, 255, 255};

    bool isRunning = true;
    bool isEqualizada = false;
    bool mouseEmCima = false;
    bool botaoClicado = false;

    SDL_FRect botaoRect = { 250.0f, 400.0f, 200.0f, 50.0f };
    SDL_Event event;

    SDL_Surface* surfaceAtual = img->imagemCinza;
    SDL_Texture* img_texture = SDL_CreateTextureFromSurface(gui.rendererPrincipal, surfaceAtual);
    
    int histAtual[256];
    for(int i=0; i<256; i++) histAtual[i] = histogramaInicial[i];

    SDL_Surface* surfaceEqualizada = equalizarImagem(img->imagemCinza, histAtual);

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                isRunning = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_S) {
                    if (IMG_SavePNG(surfaceAtual, "output_image.png") == 0) {
                        printf("Sucesso! Imagem guardada como 'output_image.png'\n");
                    }
                }
            }

            if (event.window.windowID == SDL_GetWindowID(gui.janelaHistograma)) {
                if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    float mx = event.motion.x;
                    float my = event.motion.y;
                    if (mx >= botaoRect.x && mx <= (botaoRect.x + botaoRect.w) &&
                        my >= botaoRect.y && my <= (botaoRect.y + botaoRect.h)) {
                        mouseEmCima = true;
                    } else {
                        mouseEmCima = false;
                    }
                }

                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                    if (mouseEmCima) {
                        botaoClicado = true;
                        isEqualizada = !isEqualizada;

                        surfaceAtual = isEqualizada ? surfaceEqualizada : img->imagemCinza;
                        
                        SDL_DestroyTexture(img_texture);
                        img_texture = SDL_CreateTextureFromSurface(gui.rendererPrincipal, surfaceAtual);

                        calcularHistograma(surfaceAtual, histAtual);
                    }
                }

                if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
                    botaoClicado = false;
                }
            }
        }

        SDL_SetRenderDrawColor(gui.rendererPrincipal, 0, 0, 0, 255);
        SDL_RenderClear(gui.rendererPrincipal);
        SDL_FRect dst = { 0, 0, (float)img_width, (float)img_height };
        SDL_RenderTexture(gui.rendererPrincipal, img_texture, NULL, &dst);
        SDL_RenderPresent(gui.rendererPrincipal);

        SDL_SetRenderDrawColor(gui.rendererHistograma, 50, 50, 50, 255);
        SDL_RenderClear(gui.rendererHistograma);

        int bar_x = 10, bar_y = 50, bar_w = 4, bar_h_max = 300, num_barras = 64;
        int max_val = 0;
        for (int i = 0; i < 256; i++) { if (histAtual[i] > max_val) max_val = histAtual[i]; }

        if (max_val > 0) {
            for (int i = 0; i < num_barras; i++) {
                int index = (i * 256) / num_barras;
                float ratio = (float)histAtual[index] / (float)max_val;
                int bar_h = (int)(ratio * bar_h_max);
                SDL_SetRenderDrawColor(gui.rendererHistograma, 255, 255, 255, 255);
                SDL_FRect bar = { (float)(bar_x + i * (bar_w + 1)), (float)(bar_y + bar_h_max - bar_h), (float)bar_w, (float)bar_h };
                SDL_RenderFillRect(gui.rendererHistograma, &bar);
            }
        }

        if (botaoClicado) {
            SDL_SetRenderDrawColor(gui.rendererHistograma, 0, 0, 150, 255);
        } else if (mouseEmCima) {
            SDL_SetRenderDrawColor(gui.rendererHistograma, 100, 100, 255, 255);
        } else {
            SDL_SetRenderDrawColor(gui.rendererHistograma, 0, 0, 255, 255);
        }
        SDL_RenderFillRect(gui.rendererHistograma, &botaoRect);

        if (fonte) {
            const char* textoBotao = isEqualizada ? "Ver original" : "Equalizar";
            SDL_Surface* surfTexto = TTF_RenderText_Solid(fonte, textoBotao, 0, corTexto);
            SDL_Texture* texTexto = SDL_CreateTextureFromSurface(gui.rendererHistograma, surfTexto);
            
            float tw = (float)surfTexto->w;
            float th = (float)surfTexto->h;
            SDL_FRect rectTexto = { botaoRect.x + (botaoRect.w - tw) / 2.0f, botaoRect.y + (botaoRect.h - th) / 2.0f, tw, th };
            
            SDL_RenderTexture(gui.rendererHistograma, texTexto, NULL, &rectTexto);
            SDL_DestroyTexture(texTexto);
            SDL_DestroySurface(surfTexto);
        }

        if (fonte) {
            double soma = 0;
            int total_pixels = img_width * img_height;
            for (int i = 0; i < 256; i++) {
                soma += (i * histAtual[i]);
            }
            double media = soma / total_pixels;

            double soma_var = 0;
            for (int i = 0; i < 256; i++) {
                soma_var += pow(i - media, 2) * histAtual[i];
            }
            double desvio = sqrt(soma_var / total_pixels);

            char texto_media[100];
            if (media < 85) sprintf(texto_media, "Intensidade: Escura (Media: %.2f)", media);
            else if (media < 170) sprintf(texto_media, "Intensidade: Media (Media: %.2f)", media);
            else sprintf(texto_media, "Intensidade: Clara (Media: %.2f)", media);

            char texto_contraste[100];
            if (desvio < 40) sprintf(texto_contraste, "Contraste: Baixo (Desvio: %.2f)", desvio);
            else if (desvio < 80) sprintf(texto_contraste, "Contraste: Medio (Desvio: %.2f)", desvio);
            else sprintf(texto_contraste, "Contraste: Alto (Desvio: %.2f)", desvio);

            SDL_Surface* surf_media = TTF_RenderText_Solid(fonte, texto_media, 0, corTexto);
            SDL_Texture* tex_media = SDL_CreateTextureFromSurface(gui.rendererHistograma, surf_media);
            SDL_FRect rect_media = {10.0f, 10.0f, (float)surf_media->w, (float)surf_media->h};
            SDL_RenderTexture(gui.rendererHistograma, tex_media, NULL, &rect_media);
            SDL_DestroyTexture(tex_media);
            SDL_DestroySurface(surf_media);

            SDL_Surface* surf_contraste = TTF_RenderText_Solid(fonte, texto_contraste, 0, corTexto);
            SDL_Texture* tex_contraste = SDL_CreateTextureFromSurface(gui.rendererHistograma, surf_contraste);
            SDL_FRect rect_contraste = {10.0f, 40.0f, (float)surf_contraste->w, (float)surf_contraste->h};
            SDL_RenderTexture(gui.rendererHistograma, tex_contraste, NULL, &rect_contraste);
            SDL_DestroyTexture(tex_contraste);
            SDL_DestroySurface(surf_contraste);
        }

        SDL_RenderPresent(gui.rendererHistograma);
    }

    if (fonte) TTF_CloseFont(fonte);
    SDL_DestroyTexture(img_texture);
    SDL_DestroyRenderer(gui.rendererHistograma);
    SDL_DestroyRenderer(gui.rendererPrincipal);
    SDL_DestroyWindow(gui.janelaHistograma);
    SDL_DestroyWindow(gui.janelaPrincipal);
    if (surfaceEqualizada) SDL_DestroySurface(surfaceEqualizada);

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

void calcularHistograma(SDL_Surface *surface, int *histograma) {
    for (int i = 0; i < 256; i++) {
        histograma[i] = 0;
    }

    if (surface == NULL) return;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint8 r, g, b, a;

            if (SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                histograma[r]++; 
            }
        }
    }
}

void analisarHistograma(int *histograma, int total_pixels) {
    double soma = 0;
    for (int i = 0; i < 256; i++) {
        soma += (i * histograma[i]);
    }
    double media = soma / total_pixels;

    double soma_variancia = 0;
    for (int i = 0; i < 256; i++) {
        soma_variancia += pow(i - media, 2) * histograma[i];
    }
    double desvio_padrao = sqrt(soma_variancia / total_pixels);

    printf("\nANÁLISE DO HISTOGRAMA\n");
    
    printf("Média de Intensidade: %.2f -> ", media);
    if (media < 85) printf("Imagem ESCURA\n");
    else if (media < 170) printf("Imagem MÉDIA\n");
    else printf("Imagem CLARA\n");

    printf("Desvio Padrão (Contraste): %.2f -> ", desvio_padrao);
    if (desvio_padrao < 40) printf("Contraste BAIXO\n");
    else if (desvio_padrao < 80) printf("Contraste MÉDIO\n");
    else printf("Contraste ALTO\n");
    
    printf("\n");
}

SDL_Surface* equalizarImagem(SDL_Surface *surface, int *histograma) {
    if (surface == NULL) return NULL;

    SDL_Surface *novaImagem = SDL_CreateSurface(surface->w, surface->h, surface->format);

    int total_pixels = surface->w * surface->h;
    int cdf[256] = {0}; 
    cdf[0] = histograma[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histograma[i];
    }

    int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    int nova_cor[256] = {0};
    for (int i = 0; i < 256; i++) {
        float calculo = ((float)(cdf[i] - cdf_min) / (float)(total_pixels - cdf_min)) * 255.0f;
        if (calculo < 0) calculo = 0;
        if (calculo > 255) calculo = 255;
        nova_cor[i] = (int)round(calculo); 
    }

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint8 r, g, b, a;
            if (SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                Uint8 cor_equalizada = (Uint8)nova_cor[r];
                SDL_WriteSurfacePixel(novaImagem, x, y, cor_equalizada, cor_equalizada, cor_equalizada, a);
            }
        }
    }
    printf("A imagem foi equalizada com sucesso numa nova surface!\n");
    
    return novaImagem; 
}