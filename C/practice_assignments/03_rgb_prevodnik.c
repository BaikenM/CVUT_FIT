#include <stdio.h>

/* A program that converts a color's RGB value to its HEX representation */

int main() {
    int r, g, b;
    char c;

    printf("Zadejte barvu v RGB formatu:\n");

    if (scanf(" rgb ( %d , %d , %d %c", &r, &g, &b, &c) != 4
        || r < 0 || r > 255
        || g < 0 || g > 255
        || b < 0 || b > 255
                || c != ')')
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("#%02X%02X%02X\n", r, g, b);
    return 0;
}
