#include <stdio.h>
#include <math.h>

/* A simple calculator */

int main ( void ){
    double x, y, division;
    char op, eq;
    
    printf("Zadejte vzorec:\n");
    if (scanf("%lf %c %lf %c", &x, &op, &y, &eq) != 4 ||
        eq != '=' ||
        (op != '+' && op != '-' && op != '*' && op != '/')){
        printf("Nespravny vstup.\n");
        return 1;
    }
    switch (op) {
        case '+':
            printf("%g\n", x + y);
            break;
        case '-':
            printf("%g\n", x - y);
            break;
        case '*':
            printf("%g\n", x * y);
            break;
        case '/':
            if(!y){
                printf("Nespravny vstup.\n");
                return 1;
            }
            division = x / y;
            if (division < 0) {
                printf("%g\n", ceil(x / y));
                break;
            }
            printf("%g\n", floor(x / y));
            break;
        default:
            printf("Nespravny vstup.\n");
            return 1;
    }
    return 0;
}