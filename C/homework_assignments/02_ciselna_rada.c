#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Program takes two parameters as input. 
 * Parameter #1 indicates the position of the number.
 * Parameter #2 indicates the base of a numeral system.
 * Based on the input, determine which number is located at a given position in a given numeral system.
 * */

// TODO: document

int number_length(const int x){
    return (!x) ? 1 : (int)floor(log10(x) + 1);
}

char convert_digits(long remainder){
    if(remainder <= 9)
        return '0' + remainder;
    else
        return 'a' + remainder - 10;
}

void reverse_print(long num, int base){
    if(num >= base)
        reverse_print(num / base, base);
    printf("%c", convert_digits(num % base));
}

// Calculate the number sequence and print the number residing at the position
void sequence_num(long pos, int base){
    long res = 0;
    int length = 1;
    int padding = 1;
    long long n_digit_numbers = base;
    long previous_numbers = 0;

    while(pos){
        if(pos <= length * n_digit_numbers) {
            res = pos / length + previous_numbers;
            padding += (int)(pos % length);
            pos = 0;
            break;
        }
        pos -= n_digit_numbers * length;
        length++;
        previous_numbers += n_digit_numbers;
        n_digit_numbers = (long)(pow(base, length) - pow(base, length - 1));
    }
    if(base != 10) {
        reverse_print(res, base);
        printf("\n");
    } else
        printf("%ld\n", res);
    printf("%*c\n", padding, '^');
}

// Get valid input
int get_input(long *pos_decimal, int *base){
    int err;
    printf("Pozice a soustava:\n");
    while((err = scanf("%ld %d", pos_decimal, base)) == 2){
        if(*pos_decimal < 0 || *base < 2 || *base > 36){
            return 1;
        }
        sequence_num(*pos_decimal, *base);
    }
    if(err != EOF)
        return 1;
    return 0;
}

int main(void){
    long pos_decimal;
    int base;

    if(get_input(&pos_decimal, &base)){
        printf("Nespravny vstup.\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}