#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Another log system implementation using structs and qsort, needs proper documentation */

#define NAME_MAX 101

typedef struct{
    char name[NAME_MAX];
    int  amount;
} product_t;

product_t *add_list(char *name, product_t **list, int *alloc_len, int *list_len){
    int found = 0;
    product_t *tmp;

    // handle memory
    if(*list_len == *alloc_len){
        *alloc_len *= 2;
        tmp = (product_t *)realloc(*list, *alloc_len * sizeof(*tmp));
        if(!tmp)
            return NULL;
        else
            *list = tmp;
    }
     
    for(int i = 0; i < *list_len; i++){
        if(!strcmp((*list)[i].name, name)){
            (*list)[i].amount++;
            found = 1;
            break;
        }
    }
    if(!found){
        strcpy((*list)[*list_len].name, name);
        (*list)[*list_len].amount = 1;
        ++*list_len;
    }
    return *list;
}

// qsort utility function
int cmp_products(const void *a, const void *b){
    product_t * prodA = (product_t *)a;
    product_t * prodB = (product_t *)b;
    return (prodB->amount - prodA->amount);
}

void query_products(product_t **list, int list_len, int q, char cmd){
    int top_total = 0;
    int place = 0; 

    qsort(*list, list_len, sizeof(product_t), cmp_products);
    if(list_len == 1){
        if(cmd == '#')
            printf("1. %s, %dx\n", (*list)[0].name, (*list)[0].amount);
        top_total += (*list)[0].amount;
    }
    else{
        for(int pos = 1; pos <= list_len && pos <= q; pos++){
            if((*list)[pos - 1].amount == (*list)[pos].amount){
                place = pos;
                while(place < list_len 
                        && ((*list)[place].amount == (*list)[place - 1].amount)){
                    place++;
                }
                for(int i = pos; i <= place; i++){
                    if(cmd == '#')
                        printf("%d.-%d. %s, %dx\n", pos, place, 
                                (*list)[i - 1].name, (*list)[i - 1].amount);
                    top_total += (*list)[i - 1].amount;
                }
                pos = place;
            }
            else{
                if(cmd == '#')
                    printf("%d. %s, %dx\n", pos, 
                            (*list)[pos - 1].name, (*list)[pos - 1].amount);
                top_total += (*list)[pos - 1].amount;
            }
        }
    }
    printf("Nejprodavanejsi zbozi: prodano %d kusu\n", top_total);
}

// Query products, check for valid input
int scan_input(product_t **list, int q, int alloc_len){
    size_t  buffer_len = 0;
    int     list_len = 0;
    char    cmd;
    char    *product_name = NULL;
    int     line_len;

    printf("Pozadavky:\n");
    getchar();
    
    while(scanf("%c", &cmd) == 1){
        switch(cmd){
            case '+':
                if(getchar() != ' ')
                    return 0;
                line_len = getline(&product_name, &buffer_len, stdin);
                line_len--;
                product_name[line_len] = '\0';
                if(line_len < 1 || line_len > NAME_MAX - 2){
                    free(product_name);
                    return 0;
                }
                else{
                    for(int i = 0; i < line_len; i++)
                        if(isspace(product_name[i])){
                            free(product_name);
                            return 0;
                        }
                    *list = add_list(product_name, list, &alloc_len, &list_len);
                    if(!list){
                        free(product_name);
                        return 0;
                    }
                }
                free(product_name);
                product_name = NULL;
                break;
            case '#':
                query_products(list, list_len, q, cmd);
                if(getchar() != '\n')
                    return 0;
                break;
            case '?':
                query_products(list, list_len, q, cmd);
                if(getchar() != '\n')
                    return 0;
                break;
            default:
                return 0;
        }
    }
    if(!feof(stdin))
        return 0;
    return 1;
}

int main(void){
    int alloc_len = 10;
    int quantity;
    product_t *list;

    list = (product_t *)malloc(alloc_len * sizeof(*list));

    printf("Pocet sledovanych:\n");
    if(scanf("%d", &quantity) != 1 || quantity <= 0){ 
        printf("Nespravny vstup.\n");
        free(list);
        return 1;
    }

    if(!scan_input(&list, quantity, alloc_len)){
        printf("Nespravny vstup.\n");
        free(list);
        return 1;
    }
    free(list);
    return 0;
}
