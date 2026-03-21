#include <stdio.h>
#include <locale.h>

int menu();

int main() {
    setlocale(LC_ALL, "Portuguese");

    printf("========= PROGRAMA DE PROCESSAMENTO DE IMAGENS ========\n");

    int opcao;

    while (opcao != 7) {
        opcao = menu();
        
        switch (opcao) {
            case 1:
                printf("Carregando imagem...\n");
                // Função 1
                break;
            case 2:
                printf("Convertendo para escala de cinza...\n");
                // Função 2
                break;
            case 3:
                printf("Exibindo GUI...\n");
                // Função 3
                break;
            case 4:
                printf("Exibindo histograma...\n");
                // Função 4
                break;
            case 5:
                printf("Equalizando o histograma...\n");
                // Função 5
                break;
            case 6:
                printf("Salvando imagem...\n");
                // Função 6
                break;
            case 7:
                printf("Obrigada por utilizar o programa!\n");
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    }

    return 0;
}

int menu() {
    int opcao;

    printf("\n\n============== MENU ==============\n");
    printf("1. Carregar imagem\n");
    printf("2. Conversão para escala de cinza\n");
    printf("3. Exibir GUI\n");
    printf("4. Exibir histograma\n");
    printf("5. Equalização do histograma\n");
    printf("6. Salvar imagem\n");
    printf("7. Sair\n");
    printf("==================================\n");

    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    while (opcao < 1 || opcao > 7) {
        printf("Opção inválida! Tente novamente: ");
        scanf("%d", &opcao);
    }

    return opcao;
}