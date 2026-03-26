// Beatriz Lima de Moura 10416616
// Giovana Simões Franco 10417646
// Julia Santos Oliveira 10417672
// Larissa Yuri Sato 10418318

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
void calcularHistograma(SDL_Surface *surface, int *histograma);
void analisarHistograma(int *histograma, int total_pixels);

SDL_Surface* equalizarImagem(SDL_Surface *surface, int *histograma);

#endif