// Beatriz Lima de Moura 10416616
// Giovana Simões Franco 10417646
// Julia Santos Oliveira 10417672
// Larissa Yuri Sato 10418318

#include <stdio.h>
#include <locale.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "processamento.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Portuguese");

    printf("========= PROGRAMA DE PROCESSAMENTO DE IMAGENS ========\n");

    if (argc < 2) {
        printf("Uso: %s caminho_da_imagem\n", argv[0]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Erro ao inicializar SDL. Motivo: %s\n", SDL_GetError());
        return 1;
    }

    DadosImagem img = {0};

    if (!prepararImagem(argv[1], &img)) {
        printf("Erro ao preparar a imagem.\n");
        SDL_Quit();
        return 1;
    }

    printf("Imagem carregada com sucesso!\n");
    printf("Dimensões: %d x %d\n", img.imagemOriginal->w, img.imagemOriginal->h);
    printf("Já estava em escala de cinza? %s\n", img.ehEscalaCinza ? "Sim" : "Não");

    // Pra testar conversão (dps vamos tirar)
    IMG_SavePNG(img.imagemCinza, "saida.png");
    printf("Imagem em escala de cinza salva como saida.png\n");

    int histograma[256] = {0};

    calcularHistograma(img.imagemCinza, histograma);
    
    int total_pixels = img.imagemCinza->w * img.imagemCinza->h;
    analisarHistograma(histograma, total_pixels);

    rodarGui(img.imagemCinza->w, img.imagemCinza->h, histograma, &img);
    equalizarImagem(img.imagemCinza, histograma);
    
    IMG_SavePNG(img.imagemCinza, "equalizada.png");
    printf("Sucesso! A imagem com super contraste foi salva como equalizada.png\n");

    liberarImagem(&img);
    SDL_Quit();

    return 0;
}