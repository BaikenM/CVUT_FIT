#include <stdio.h>
#include <stdlib.h>

/* Program takes two parameters.
 * First parameter determines max amount of dice (or dominos). 
 * Second parameter determines the values on each piece.
 * Based on the input, recursively print out the possible combinations of towers that
 * can be formed by stacking the pieces on top of each other. The only restriction is that all 
 * the pieces must be stacked in a descending order.
 * 
 * For example: for 7 pieces of values 3 8 23 5 2 1 8 possible (but not all) combinations are
 * 23, 8, 5, 3, 2, 1
 * 23, 8, 8, 5, 3, 2, 1
 * 8, 8, 2, 1
 * 2
 * */

#define INIT_START 0 // just use 0?

// get input, scan for errors
int *get_dice(int *count){
	int *dice = NULL; // store input dice
	char ch;

	printf("Pocet kostek:\n");
	if(scanf("%d", count) != 1 || *count < 1 
		|| (ch = getchar()) != '\n')
		return NULL;
	dice = (int *)malloc(*count * sizeof *dice);
	printf("Velikosti:\n");
	for (int i = 0; i < *count; ++i){
		if((scanf("%d", &dice[i]) != 1 || dice[i] < 1)){
			free(dice);
			return NULL;
		}
	}
	return dice;
}

// qsort utility function, descending order
int dice_cmp(const int *a, const int *b){ return (*b > *a) - (*a > *b); }

// sort with the help of dice_cmp function above
void sort_dice(int *dice, int count){
	qsort(dice, count, sizeof(*dice), (int (*)(const void *, const void *))dice_cmp);
}

// print previously generated stack
void print_stack(int *stack, int len){
	for (int i = 0; i < len - 1; ++i) 
		printf("%d, ", stack[i]);
    printf("%d\n", stack[len - 1]);
}

// count all subsets of a multiset, shoutout to BI-ZDM
int get_cardinality(int *dice, int n){
	int card = 1, dup = 1;

	for (int i = 0; i < n - 1; ++i){
		if(dice[i] == dice[i + 1])
			dup++;
		else{
			card *= dup + 1;
			dup = 1;
		}

	}
	card *= dup + 1;
	return card - 1; // empty set excluded
}

// recursive function to generate next stack
void stack_dice(int *dice, int *next_stack, int pos, int count, int counter){
    if(pos == count)
        return;

    for(int i = pos; i < count; i++){		// iterate from next element
        next_stack[counter] = dice[i];		// actuall stack generation
        stack_dice(dice, next_stack, i+ 1, count, counter + 1); // rinse and repeat
        if(i < count - 1 && dice[i] == dice[i+ 1])
            while(dice[i] == dice[i + 1])
                i++;
        print_stack(next_stack, counter + 1);
	}
}

int main(void){
	int dice_count, counter = 0;
	int *dice, *next_stack;

	if((dice = get_dice(&dice_count)) == NULL){
		printf("Nespravny vstup.\n");
		return 1;
	}
	next_stack = (int *)malloc(dice_count * sizeof(*next_stack));
	
	sort_dice(dice, dice_count);
	stack_dice(dice, next_stack, INIT_START, dice_count, counter);
	
	printf("Celkem: %d\n", get_cardinality(dice, dice_count));
	free(next_stack);
	free(dice);
	return 0;
}
