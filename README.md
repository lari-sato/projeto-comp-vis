<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/71/UPM.2_horizontal_vermelho.png/960px-UPM.2_horizontal_vermelho.png">

# 💻 Computação Visual | Prof. André Kishimoto

# 🏞️ Projeto 1 - Processamento de imagens

## 🎯 Objetivo

Este projeto consiste no desenvolvimento de um software de processamento de imagens em linguagem C, utilizando a biblioteca SDL (Simple DirectMedia Layer) e SDL_image.

O programa é executado via linha de comando, recebendo como argumento o caminho de uma imagem, e realiza uma sequência de operações de análise e processamento sobre essa imagem.

## 🔧 Funcionamento

### 1. Carregamento de imagem

#### 🔹 Arquivos: `processamento.c` + `processamento.h` 

O programa inicia carregando a imagem fornecida como argumento utilizando a função `IMG_Load()` da biblioteca **SDL_image**.

Essa função:
- realiza a leitura do arquivo;
- identifica automaticamente o formato da imagem (PNG, JPG, BMP);
- cria uma estrutura `SDL_Surface` contendo os dados da imagem, como:
    - largura (`w`);
    - altura (`h`);
    - formato de pixel.

Caso o arquivo não exista ou esteja em um formato inválido:
- Uma mensagem de erro é exibida ao usuário, juntamente da causa obtida através de `SDL_GetError()`;
- O programa é finalizado.
---

### 2. Análise e conversão para escala de cinza

#### 🔹 Arquivos: `processamento.c` + `processamento.h` 

Após o carregamento, o programa apresenta as dimensões da imagem como evidência de que foi carregada corretamente.

Em seguida, verifica se a imagem já está em escala de cinza, percorrendo todos os seus pixels:

- Para cada um, verifica se `R == G == B`:
    - Se for verdadeiro para todos os pixels, imagem já está em escala de cinza;
    - Caso contrário, a imagem é considerada colorida;
    - Nos dois cenários, o usuário é notificado.

Se a imagem for colorida, ela é convertida para escala de cinza utilizando a fórmula apresentada em aula:

`Y = 0.2125 * R + 0.7154 * G + 0.0721 * B`

- Uma nova imagem (`SDL_Surface`) é criada contendo os valores convertidos.  
- Essa imagem em escala de cinza passa a ser a base para todas as operações seguintes do sistema.

---
### 3. Interface gráfica de usuário (GUI) com duas janelas

---
### 4. Análise e exibição do histograma

---
### 5. Equalização do histograma

---
### 6. Salvar imagem

---

## ⚙️ Compilação

Primeiramente, abra um terminal dentro da pasta do projeto.

### ✔️ Usando pkg-config (recomendado)
`gcc main.c processamento.c -o projeto $(pkg-config --cflags --libs sdl3 sdl3-image)`

### ✔️ Sem pkg-config
`gcc main.c processamento.c -o projeto -lSDL3 -lSDL3_image`

OBS.: No executável, é possível compilar usando `.exe` ou não.
```
-o projeto
-o projeto.exe
```

---
## ▶️ Execução

O programa deve ser executado passando o caminho da imagem como argumento:

`./projeto <caminho da imagem.extensao>`

- Por exemplo: `./projeto imagem.jpg`

---
## Organização e qualidade do código

O projeto foi estruturado de forma modular:

- Separação em múltiplos arquivos `.c` e `.h`;
- Funções com responsabilidades bem definidas;
- Uso de estruturas para encapsular dados da imagem;
- Gerenciamento correto de memória (alocação e liberação).

Além disso:
- O código é compatível com compilação via `gcc`;
- O projeto segue os requisitos técnicos definidos no enunciado.

---

## 🤝 Contribuições

- Beatriz Lima de Moura
- Giovana Simões Franco
- Julia Santos Oliveira
- Larissa Yuri Sato
