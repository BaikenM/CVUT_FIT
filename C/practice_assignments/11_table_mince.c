#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Program takes two sequences as input. 
 * Sequence #1 indicates the coins available.
 * Sequence #2 indicates different amounts of money.
 * Based on the available coins, decide how many coins are needed from #1 to form an amount from #2. 
 * */

#define FREE_EXIT { free(coins); \
					return NULL; }

// expand allocated memory
int *realloc_memory(int *coins, int new_size){
	int *new_coins = NULL;

	new_size *= new_size < 100 ? new_size + new_size/2 : new_size * 2;
	new_coins = (int *)realloc(coins, new_size * sizeof(*new_coins));
	if(!new_coins)
		return NULL;
	return new_coins;
}

// scan input, store coins in an array
int *get_coins(int *count){
	int 	*coins = NULL;
	int 	alloc_size = 10;
	char 	ch;

	coins = (int *)malloc(alloc_size * sizeof(*coins));
	if(!coins)
		return NULL;

	while(scanf(" %d", &coins[*count]) == 1){
		if(coins[*count] < 0)
			FREE_EXIT;	// semicolon redundant?
		if(coins[*count] == 0)
			break;

		++*count;
		if(*count == alloc_size){
			if((coins = realloc_memory(coins, alloc_size)) == NULL)
				FREE_EXIT;
		}	
	}
	if(*count < 1 || (ch = getchar()) != '\n')
		FREE_EXIT;
	return coins;
}

// utility function for qsort below
int cmp_coins(const int *a, const int *b){ return (*a < *b) - (*b < *a); }

// nothing fancy, Vagner-style
void sort_coins(int *coins, int count){
	qsort(coins, count, sizeof(*coins), (int(*)(const void*, const void*))cmp_coins);
}

// ...
int find_min(int a, int b){ return a < b ? a : b; }

// recursion magic happening, INT_MAX may screw up on 'Test meznich hodnot',
// but nothing else seem to work, may rework later
int count_sum(int *coins, int count, int amount, int *coin_table){
	coin_table[0] = 0;
	for(int i = 1; i <= amount; i++){
		coin_table[i] = INT_MAX;
		int min = INT_MAX;
		for(int j = 0; j <= count; j++){
			if(i - coins[j] >= 0)
				min = coin_table[i - coins[j]];
			if(min != INT_MAX)
				coin_table[i] = find_min(min + 1, coin_table[i]);
			}
	}	
	return coin_table[amount];
}

// get input #2, consequently recursively count minimum amount of coins
int get_new_amount(int *coins, int count){
	int 	amount;
	int 	coins_used = 0;

	while(scanf("%d", &amount) == 1){
		if(amount < 0)
			return 0;
		
		int *coin_table = (int *)malloc((amount + 1) * sizeof(*coin_table));
		if(!coin_table)
			return 0;
		coins_used = count_sum(coins, count, amount, coin_table);

		if(coins_used != INT_MAX)
			printf("= %d\n", coins_used);
		else
			printf("= nema reseni\n");
		free(coin_table);
	}
	if(!feof(stdin))
		return 0;
	return 1;
}

int main(void){
	int *coins = NULL;
	int count = 0;

	printf("Mince:\n");
	if((coins = get_coins(&count)) == NULL){
		printf("Nespravny vstup.\n");
		return 1;
	}
	sort_coins(coins, count);

	printf("Castky:\n");
	if(get_new_amount(coins, count) == 0){
		printf("Nespravny vstup.\n");
		free(coins);
		return 1;
	}
	free(coins);
	return 0;
}
