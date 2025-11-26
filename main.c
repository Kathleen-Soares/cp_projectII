#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum { // tipos de células
    CELL_EMPTY = 0,
    CELL_ROCK,
    CELL_RABBIT,
    CELL_FOX
} CellType; 

typedef struct {
    CellType type;
    int proc_age;   // idade de procriação (para rabbits e foxes)
    int food_age;   // só usado para foxes (fome)
} Cell;

// Variáveis globais
int GEN_PROC_RABBITS;
int GEN_PROC_FOXES;
int GEN_FOOD_FOXES;
int N_GEN;
int R, C;
int N_objects;

Cell *world_curr = NULL; // mundo atual
Cell *world_next = NULL; // próximo mundo

#define IDX(x, y) ((x) * C + (y)) // índice linear para (x,y)

// Inicializa todas as células como vazias
void init_world(Cell *w, int R, int C) {
    for (int i = 0; i < R * C; i++) {
        w[i].type = CELL_EMPTY;
        w[i].proc_age = 0;
        w[i].food_age = 0;
    }
}

// Só para DEBUG: imprime o mundo em formato simples
// * = ROCK, R = RABBIT, F = FOX, . = vazio
void print_world(Cell *w, int R, int C) {
    printf("Mundo (%d x %d):\n", R, C);
    for (int x = 0; x < R; x++) {
        for (int y = 0; y < C; y++) {
            Cell *cell = &w[IDX(x, y)];
            char ch = '.';
            if (cell->type == CELL_ROCK)   ch = '*';
            else if (cell->type == CELL_RABBIT) ch = 'R';
            else if (cell->type == CELL_FOX)    ch = 'F';
            printf("%c ", ch); // ch representa a célula
        }
        printf("\n");
    }
    printf("\n");
}

// Lê o input inicial e preenche world_curr
void read_input() {
    if (scanf("%d %d %d %d %d %d %d",
              &GEN_PROC_RABBITS,
              &GEN_PROC_FOXES,
              &GEN_FOOD_FOXES,
              &N_GEN,
              &R, &C,
              &N_objects) != 7) {
        fprintf(stderr, "Erro ao ler parâmetros iniciais.\n");
        exit(EXIT_FAILURE);
    }

    // Alocar mundos dinamicamente com base em R e C lidos 
    world_curr = (Cell *) malloc(R * C * sizeof(Cell));
    world_next = (Cell *) malloc(R * C * sizeof(Cell));
    if (!world_curr || !world_next) {
        fprintf(stderr, "Erro ao alocar memória para o mundo.\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar tudo como vazio no começo do programa (antes de ler objetos)
    init_world(world_curr, R, C);
    init_world(world_next, R, C);

    // Ler objetos iniciais e preencher world_curr 
    for (int i = 0; i < N_objects; i++) {
        char obj[16];
        int x, y;
        if (scanf("%15s %d %d", obj, &x, &y) != 3) {
            fprintf(stderr, "Erro ao ler objeto %d.\n", i);
            exit(EXIT_FAILURE);
        }

        if (x < 0 || x >= R || y < 0 || y >= C) {
            fprintf(stderr, "Coordenadas fora dos limites: %d %d\n", x, y);
            exit(EXIT_FAILURE);
        }

        Cell *cell = &world_curr[IDX(x, y)]; // célula a preencher com o objeto lido 

        // Definir o tipo de célula 
        if (strcmp(obj, "ROCK") == 0) {
            cell->type = CELL_ROCK;
        } else if (strcmp(obj, "RABBIT") == 0) {
            cell->type = CELL_RABBIT;
        } else if (strcmp(obj, "FOX") == 0) {
            cell->type = CELL_FOX;
        } else {
            fprintf(stderr, "Objeto desconhecido: %s\n", obj);
            exit(EXIT_FAILURE);
        }

        // Todos começam com idade 0 de procriação e fome 0
        cell->proc_age = 0;
        cell->food_age = 0;
    }
}


int main(void) {
    // 1. Ler input e montar o mundo inicial
    read_input();

    // 2. (Opcional) imprimir mundo para debug
    print_world(world_curr, R, C);

    // 3. Aqui depois virá o loop de gerações e a simulação

    // 4. Liberar memória e sair
    free(world_curr);
    free(world_next);

    return 0;
}