#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* A program that prints out a mu;tiplication table */

// Check for valid input
void get_input(int *n){
    if(scanf("%d", n) != 1 || *n <= 0){
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
}

// Print the table based on the Cartesian product of numbers in range 1 to n,
// where n is input 
void print_table(const int n){
    int padding = floor(log10(n * n)) + 2;
    int m;
    printf("%*c", padding, '|');
    for(int i = n; i; i--){
        printf("%*d", padding, i);
    }
    printf("\n");
    for(int i = padding - 1; i; i--)
        printf("-");
    printf("+");
    for(int i = n * padding; i; i--){
        printf("-");
    }
    for(int i = 1; i <= n; i++){
        printf("\n%*d|", padding - 1, i);
        m = n;
        for(int j = (n - i) + 1; j; j--){
            printf("%*d", padding, m * i);
            m--;
        }
    }
    printf("\n");
}

// Driver function
int main(void){
    int n;

    printf("Rozsah:\n");
    get_input(&n);
    print_table(n);
    return 0;
}