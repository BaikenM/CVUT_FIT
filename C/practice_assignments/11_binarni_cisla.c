#include <stdio.h>
#include <stdlib.h>

/* Program takes two sequences as input. 
 * Sequence #1 indicates max number of digits in a binary number.
 * Sequence #2 indicates max amount of same digit (1 or 0) in a row.
 * Based on this rules, print out every possible binary number.
 */

#define INIT_COUNT 0

// Check for valid input
int get_numbers(int *k){
	int n;

	printf("Pocet cifer:\n");
	if(scanf("%d", &n) != 1 || n < 1 || n > 64){
		return -1;
	}
	printf("Max. stejnych po sobe:\n");
	if(scanf("%d", k) != 1 || *k < 1){
		return -1;
	}
	return n;
}

// Print out the resulting sequence
void print_sequences(int n, int k, int c_one, int c_zero, int *calls, int *arr, int depth){
	if(!n)
		return;
	++*calls;

	for(int i = 0; i < depth; i++)
		printf("%d", arr[i]);
	printf("\n");

	if(c_one < k && n > 1){
		arr[depth] = 1;
		print_sequences(n - 1, k, c_one + 1, INIT_COUNT, calls, arr, depth + 1); // append 1 to seq
	}

	if(c_zero < k && n > 1){
		arr[depth] = 0;
		print_sequences(n - 1, k, INIT_COUNT, c_zero + 1, calls, arr, depth + 1); // append 0 to seq
	}
}

// Driver function
int main(void){
	int n, k, calls = 0, depth = 1;
	int *number = NULL;

	n = get_numbers(&k);
	if(n == -1){
		printf("Nespravny vstup.\n");
		return 1;
	}

	number = (int *)malloc(n * sizeof(*number));
	if(!number){
		printf("Nespravny vstup.\n");
		return 1;	
	}

	number[0] = 1;
	print_sequences(n, k, INIT_COUNT + 1, INIT_COUNT, &calls, number, depth);
	printf("Celkem: %d\n", calls);
	free(number);
	return 0;
}