#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* A program that prints out a checkerboard depending on the dimensions received as input */

// Check for valid inout
void check_input(int *x, int *y){
    printf("Zadejte pocet poli:\n");
    if(scanf("%d", x) != 1 || *x <= 0) {
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
    printf("Zadejte velikost pole:\n");
    if(scanf("%d", y) != 1 || *y <= 0){
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
}

// Print out board
void print_board(const int x, const int y){
    printf("+");
    for(int i = 1; i <= x * y; i++)
        printf("-");
    printf("+\n");

    for(int j = 0; j < x * y; j++){
        printf("|");
        for(int k = 0; k < x * y; k++){
            switch (((k + 2 * y) / y) % 2) {
                case 0:
                    if(!((j + 2 * y) / y % 2)) printf(" ");
                    else printf("X");
                    break;
                case 1:
                    if(!((j + 2 * y) / y % 2)) printf("X");
                    else printf(" ");
                    break;
            }
        }
        printf("|\n");
    }

    printf("+");
    for(int i = 0; i < x * y; i++)
        printf("-");
    printf("+\n");
}

// Driver function
int main(void){
    int board_scale, tile_scale;
    check_input(&board_scale, &tile_scale);
    print_board(board_scale, tile_scale);
    return 0;
}