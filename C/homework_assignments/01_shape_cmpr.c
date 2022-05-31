#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* A program that compares the perimeter and the volume of two geometric 
 * shapes (a square (S), a rectangle (R) or a triangle (T)) 
 */

// Determine if the given parameters form a valid triangle
int is_triangle(const double *x, const double *y, const double *z){
    return ((*x + *y - *z > DBL_EPSILON * fmax(fmax(*x, *y), *z) ) && (*x + *z - *y > DBL_EPSILON * fmax(fmax(*x, *y), *z) ) && (*y + *z - *x > DBL_EPSILON * fmax(fmax(*x, *y), *z) ));
}

// Calculate the perimeter of a given shape
void get_perimeter(double *p, const char id, const double x, const double y, const double z){
    switch (id) {
        case 'S':
            *p = 4 * x;
            break;
        case 'R':
            *p = 2 * (x + y);
            break;
        case 'T':
            *p = x + y + z;
            break;
    }
}

// Calculate the area of a given shape
void get_area(double *a, const char id, const double x, const double y, const double z){
    double s = (x + y + z) / 2;
    switch (id) {
        case 'S':
            *a = x * x;
            break;
        case 'R':
            *a = x * y;
            break;
        case 'T':
            *a = sqrt(s * (s - x) * (s - y) * (s - z));
            break;
    }
}

// Compare parameters (perimeter or area) and return the appropriate comparison sign (=, < or >)
char compare_sign(const double x, const double y){
    if(fabs (x - y) <= 100 * DBL_EPSILON * fmax(fabs(x), fabs(y)))
        return '=';
    else
        return x < y ? '<' : '>';
}

// Print out result in human-readable form
void print_result(const char id1, const char id2, const double p1, const double p2, const double a1, const double a2){
    const char *sq = "ctverec";
    const char *rect = "obdelnik";
    const char *tr = "trojuhelnik";

    switch (id1) {
        case 'S':
            switch (id2) {
                case 'S':
                    printf("Obvod: %s #1 %c %s #2\n", sq, compare_sign(p1, p2), sq);
                    printf("Obsah: %s #1 %c %s #2\n", sq, compare_sign(a1, a2), sq);
                    break;
                case 'R':
                    printf("Obvod: %s #1 %c %s #2\n", sq, compare_sign(p1, p2), rect);
                    printf("Obsah: %s #1 %c %s #2\n", sq, compare_sign(a1, a2), rect);
                    break;
                case 'T':
                    printf("Obvod: %s #1 %c %s #2\n", sq, compare_sign(p1, p2), tr);
                    printf("Obsah: %s #1 %c %s #2\n", sq, compare_sign(a1, a2), tr);
                    break;
            }
            break;
        case 'R':
            switch (id2) {
                case 'S':
                    printf("Obvod: %s #1 %c %s #2\n", rect, compare_sign(p1, p2), sq);
                    printf("Obsah: %s #1 %c %s #2\n", rect, compare_sign(a1, a2), sq);
                    break;
                case 'R':
                    printf("Obvod: %s #1 %c %s #2\n", rect, compare_sign(p1, p2), rect);
                    printf("Obsah: %s #1 %c %s #2\n", rect, compare_sign(a1, a2), rect);
                    break;
                case 'T':
                    printf("Obvod: %s #1 %c %s #2\n", rect, compare_sign(p1, p2), tr);
                    printf("Obsah: %s #1 %c %s #2\n", rect, compare_sign(a1, a2), tr);
                    break;
            }
            break;
        case 'T':
            switch (id2) {
                case 'S':
                    printf("Obvod: %s #1 %c %s #2\n", tr, compare_sign(p1, p2), sq);
                    printf("Obsah: %s #1 %c %s #2\n", tr, compare_sign(a1, a2), sq);
                    break;
                case 'R':
                    printf("Obvod: %s #1 %c %s #2\n", tr, compare_sign(p1, p2), rect);
                    printf("Obsah: %s #1 %c %s #2\n", tr, compare_sign(a1, a2), rect);
                    break;
                case 'T':
                    printf("Obvod: %s #1 %c %s #2\n", tr, compare_sign(p1, p2), tr);
                    printf("Obsah: %s #1 %c %s #2\n", tr, compare_sign(a1, a2), tr);
                    break;
            }
            break;
    }
}

void determine_shape(char *id, double *x, double *y, double *z){
    if(scanf(" %c", id) != 1 ){
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
    switch(*id){
        case 'S':
            if(scanf(" %lf", x) != 1 || *x <= 0) {
                printf("Nespravny vstup.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'R':
            if(scanf(" %lf %lf", x, y) != 2 || *x <= 0 || *y <= 0 || *x == *y){
                printf("Nespravny vstup.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'T':
            if(scanf(" %lf %lf %lf", x, y, z) != 3 || *x <= 0 || *y <= 0 || *z <= 0 || !is_triangle(x, y, z)) {
                printf("Nespravny vstup.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            printf("Nespravny vstup.\n");
            exit(EXIT_FAILURE);
    }
}

int main(void){
    double side1_1, side1_2, side1_3;
    double side2_1, side2_2, side2_3;
    double perimeter1 = 0, perimeter2 = 0, area1, area2;
    char id1, id2;

    // Get first shape
    printf("Obrazec #1\n");
    determine_shape(&id1, &side1_1, &side1_2, &side1_3);

    // Get second shape
    printf("Obrazec #2\n");
    determine_shape(&id2, &side2_1, &side2_2, &side2_3);

    // Calculate parameters of the first shape
    get_perimeter(&perimeter1, id1, side1_1, side1_2, side1_3);
    get_area(&area1, id1, side1_1, side1_2, side1_3);

    // Calculate parameters of the second shape
    get_perimeter(&perimeter2, id2, side2_1, side2_2, side2_3);
    get_area(&area2, id2, side2_1, side2_2, side2_3);

    print_result(id1, id2, perimeter1, perimeter2, area1, area2);
    return 0;
}