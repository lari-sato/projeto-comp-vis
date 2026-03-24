#ifndef PROCESSAMENTO_H
#define PROCESSAMENTO_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct {
    SDL_Surface *imagemOriginal;
    SDL_Surface *imagemCinza;
    bool ehEscalaCinza;
} DadosImagem;

bool prepararImagem(const char *caminho, DadosImagem *img);
void liberarImagem(DadosImagem *img);

#endif