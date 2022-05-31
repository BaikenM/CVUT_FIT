#include <stdio.h>

/* A warm-up assignment
 */

int main() {
    int input;

    printf("ml' nob:\n");
    
    if (scanf("%d", &input) != 1 || input < 1 || input > 5) {
        printf("luj\n");
        return 1;
    }
    
    switch (input) {
        case (1):
            printf("Qapla'\n");
            printf("noH QapmeH wo' Qaw'lu'chugh yay chavbe'lu' 'ej wo' choqmeH may' DoHlu'chugh lujbe'lu'.\n");
            break;
        case (2):
            printf("Qapla'\n");
            printf("Qu' buSHa'chugh SuvwI', batlhHa' vangchugh, qoj matlhHa'chugh, pagh ghaH SuvwI''e'.\n");
            break;
        case (3):
            printf("Qapla'\n");
            printf("qaStaHvIS wa' ram loS SaD Hugh SIjlaH qetbogh loD.\n");
            break;
        case (4):
            printf("Qapla'\n");
            printf("Ha'DIbaH DaSop 'e' DaHechbe'chugh yIHoHQo'.\n");
            break;
        case (5):
            printf("Qapla'\n");
            printf("leghlaHchu'be'chugh mIn lo'laHbe' taj jej.\n");
            break;
        default:
            printf("luj\n");
            return 1;
    }
    return 0;
}
