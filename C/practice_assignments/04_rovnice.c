#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

/* A program that check the correctness of an equation */

// Function prototypes
int float_compare (double x, double y);
void get_input(double *a, double *b, double *c, char *op, char *eq);
void set_lhs(const double *a, const double *b, const char* op, double *sum, double *sub, double *prod, double *div);
void compare_lhs_to_rhs(const double *a, const double *b, const double *c, const char *op, const double *sum, const double *sub, const double *prod, double *div);

// Driver function
int main ( void ){
    double a, b, c, sum, sub, prod, div;
    char op, eq;

    get_input(&a, &b, &c, &op, &eq);
    set_lhs(&a, &b, &op, &sum, &sub, &prod, &div);
    compare_lhs_to_rhs(&a, &b, &c, &op, &sum, &sub, &prod, &div);
    return 0;
}

// Compare floating point numbers
int float_compare (double x, double y){
    return fabs (x - y) <= DBL_EPSILON * fmax(fabs(x), fabs(y));
}

// Scan input, look for errors
void get_input(double *a, double *b, double *c, char *op, char *eq){

    printf("Zadejte rovnici:\n");
    if (scanf("%lf %c %lf %c %lf", a, op, b, eq, c) != 5 ||
        *eq != '=' ||
        (*op != '+' && *op != '-' && *op != '*' && *op != '/')){
        printf("Nespravny vstup.\n");
        exit (EXIT_FAILURE);
    }
}

// Calculate the left-hand side of the equation
void set_lhs(const double *a, const double *b, const char *op, double *sum, double *sub, double *prod, double *div){
    *sum = *a + *b;
    *sub = *a - *b;
    *prod = *a * *b;
    if(!*b && *op == '/'){
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
    *div = *a / *b;
}

// Compare the left-hand side of the equation to the right-hand side
void compare_lhs_to_rhs(const double *a, const double *b, const double *c, const char *op, const double *sum, const double *sub, const double *prod, double *div){
    switch (*op) {
        case '+':
            if (float_compare(*sum, *c)) {
                printf("Rovnice je spravne.\n");
                break;
            }
            printf("%g != %g\n", *sum, *c);
            break;
        case '-':
            if (float_compare(*sub, *c)) {
                printf("Rovnice je spravne.\n");
                break;
            }
            printf("%g != %g\n", *sub, *c);
            break;
        case '*':
            if (float_compare(*prod, *c)) {
                printf("Rovnice je spravne.\n");
                break;
            }
            printf("%g != %g\n", *prod, *c);
            break;
        case '/':
            if (*div < 0)
                *div = ceil(*a / *b);
            else
                *div = floor(*a / *b);
            if (float_compare(*div, *c)){
                printf("Rovnice je spravne.\n");
                break;
            }
            printf("%g != %g\n", *div, *c);
            break;
        default:
            printf("Nespravny vstup.\n");
            exit(EXIT_FAILURE);
    }
}
