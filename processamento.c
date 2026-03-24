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