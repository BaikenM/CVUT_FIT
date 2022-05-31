#include <stdio.h>
#include <stdlib.h>

/* A program that for a given sequence of numbers decides whether a certain 
 * number can be a sum of two different numbers from the sequnce, O(n^2)
 * */

// Query each input number
int check_sum(int **sequence, int seq_len){
    int sum;
    int found;
    int i, j;

    printf("Testovane hodnoty:\n");
    while(scanf("%d", &sum) == 1){
        found = 0;
        for(i = 0; i < seq_len - 1; i++){
            if(found)
                break;
            for(j = i + 1; j < seq_len; j++){
                if((*sequence)[i] + (*sequence)[j] == sum){
                    found = 1;
                    break;
                }
            }
        }

        if(found)
            printf("> Soucet %d lze dosahnout.\n", sum);
        else
            printf("> Soucet %d nelze dosahnout.\n", sum);
    }

	if(ferror(stdin) || !feof(stdin))
		return 0;
	return 1;
}

int *get_sequence(int **sequence, int *seq_len, int alloc_size){
    char end_mark;
	int  end_reached = 0;
    int  *tmp = NULL;
    int  next_elem;
    int  err;

    while((err = scanf(" %d %c", &next_elem, &end_mark)) == 2){
        // Try to get a sequence
        switch(end_mark){
            case ',':
                break;
            case '?':
                end_reached = 1;
                break;
            default:
                return NULL;
        }
        // Record next number into the array
        (*sequence)[*seq_len] = next_elem;
        ++*seq_len;
        if(end_reached)
            break;

        // realloc if more space is needed
        if(*seq_len == alloc_size){
            tmp = (int *)realloc(*sequence, alloc_size * 2 * sizeof(int));
            if(!tmp)
                return NULL;
            *sequence = tmp;
            alloc_size *= 2;
        }
    }
    if(*seq_len < 2 || err != 2)
        return NULL;

    tmp = (int *)realloc(*sequence, *seq_len * sizeof(int));
    // realloc failed
    if(!tmp)
        return NULL;

    *sequence = tmp;
    return *sequence;
}

int main(void){
	int alloc_size = 10;
	int seq_len = 0;
	int *sequence = (int *)malloc(alloc_size * sizeof(*sequence));;
	int *tmp = NULL;
	
    // Get a sequence of numbers
    printf("Vstupni posloupnost:\n");
    tmp = get_sequence(&sequence, &seq_len, alloc_size);
	if(!tmp){
        free(sequence);
		printf("Nespravny vstup.\n");
        return 1;
    }
	sequence = tmp;

    // Query numbers using the input sequence
	if(!check_sum(&sequence, seq_len)){
        free(sequence);
		printf("Nespravny vstup.\n");
        return 1;
	}	
	free(sequence);
    return 0;
}
