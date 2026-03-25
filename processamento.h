#ifndef PROCESSAMENTO_H
#define PROCESSAMENTO_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct {
    SDL_Surface *imagemOriginal;
    SDL_Surface *imagemCinza;
    bool ehEscalaCinza;
} DadosImagem;

typedef struct {
    SDL_Window *janelaPrincipal;
    SDL_Renderer *rendererPrincipal;

    SDL_Window *janelaHistograma;
    SDL_Renderer *rendererHistograma;
} GUI;



bool prepararImagem(const char *caminho, DadosImagem *img);
void liberarImagem(DadosImagem *img);
int rodarGui(int img_width, int img_height, const int *histograma, DadosImagem *img);

#endif