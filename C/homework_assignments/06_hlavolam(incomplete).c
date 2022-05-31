#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* INFO: An incomplete program, might finish some time. 
 * 
 * A program takes a matrix of characters and decides which words are repeated the most 
 * (in any direction - vertically, horizontally, diagonally, left to right and right to left).
 * */

#define GET_LINE (getline(&next_line, &line_buffer, stdin))
#define RETURN_NULL   { free(next_line); \
                        free_matrix(puzzle, *max_len, *max_len); \
                        return NULL; }
#define EXIT_NO_ALLOC { free(next_line); \
                        printf("Nespravny vstup.\n"); \
                        exit(EXIT_FAILURE); }

#define ASCII_OFFSET 32 // 32 - first printable ASCII character
#define MAX_NODES 127 // all printable ASCII characters
#define INIT_DEPTH 0 // initial depth while searching for words

#define INT_TO_CHAR(i) ((char)(i + ASCII_OFFSET))

typedef struct word_t{
    word_t  *next[MAX_NODES];
    int     count;
}word_t;

word_t *new_word(void){
    word_t *tmp = (word_t *)malloc(sizeof(word_t));

    if(tmp){
        for(int i = 0; i < MAX_NODES; i++)
            tmp->next[i] = NULL;
    }
    tmp->count = 1;
    return tmp;
}

void free_trie(word_t *node){
    if(!node)
        return;
    //printf("next node: freeing...\n");
    for(int i = 0; i < MAX_NODES; i++){
        if(node->next[i] != NULL){
        //    printf("freeing %c...\n", (char)(i + ASCII_OFFSET));
            free_trie(node->next[i]);
        }
    }
    //printf("node free!\n");
    free(node);
}

void free_matrix(char **m, int rows, int len){
    for(int i = 0; i < rows; i++){
      //  printf("matrix: %.*s freed!\n", len, *(m + i));
        free(*(m + i));
    }
    //printf("matrix freed!\n");
    free(m);
}

void print_matrix(char **m, int rows, int cols){
    printf("\n");
    //for(int i = 0; i < rows; i++){
    //    printf("%.*s\n", len, *(m + i));
    //}
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++)
            printf("%c", m[i][j]);
        printf("\n");
    }
}

char **alloc_memory(int rows, int cols){
    char **puzzle = (char **)malloc(rows * sizeof(*puzzle)); 
    
    if(!puzzle)
        return NULL;
    
    for(int i = 0; i < rows; i++){
        puzzle[i] = (char *)malloc(cols * sizeof(**puzzle));
        if(!puzzle[i]){
            free(puzzle);
            return NULL;
        }
    } 
    return puzzle;
}

char **get_puzzle(int *max_len){
    size_t  line_buffer = 0;
    int     line_len, rows = 1, cols;
    char    **puzzle = NULL;
    char    *next_line = NULL;

    // get first line
    if((*max_len = GET_LINE - 1) < 1) EXIT_NO_ALLOC
     
    // initial memory allocation for puzzle, rows = 10
    puzzle = alloc_memory(*max_len, *max_len);
    if(!puzzle) EXIT_NO_ALLOC

    // fill the first row in a puzzle
    strncpy(puzzle[0], next_line, *max_len);
    
    cols = *max_len;
    
    // get remaining rows if given
    while(rows <= cols && (line_len = GET_LINE - 1) > 0){
        if(line_len != *max_len || (rows == cols && !feof(stdin)))
            RETURN_NULL
    
        strncpy(*(puzzle + rows), next_line, line_len);
        rows++;
    }
    if(rows != cols) RETURN_NULL
    free(next_line);
    return puzzle;
}

void add_words(word_t *words, char *word, int len, int *max){
    int idx;

    word_t *tmp = words;
    for(int i = 0; i < len; i++){
        idx = (int)(word[i]) - ASCII_OFFSET;
        //printf("current idx: %d\n", idx);
        if(tmp->next[idx]){
            if(i == len - 1){
                tmp->next[idx]->count++;
      //          printf("count increased!\n");
                if(tmp->next[idx]->count > *max){
                    *max = tmp->next[idx]->count;
    //                printf("NEW MAX: %d\n", *max);
                }
            }
        }
        else{
            tmp->next[idx] = new_word();
        //    printf("add_words: %c added!\n", word[i]);
        }
        tmp = tmp->next[idx];
    }
    tmp = NULL;
}

word_t *parse_puzzle(char **puzzle, int p_len, int *max, int *w_len){
    int     max_mat = 1, next_mat = 0, total_matrices = 0;
    int     i, j, offset_i, offset_j, w_count, tmp = p_len;
    char    **w_list = NULL;
    word_t  *words = NULL;
        
    while(true){
        words = new_word(); //doesn't work, but should, see valgrind

        if(tmp == 1){
            for(i = 0; i < p_len; i++)
                for(j = 0; j < p_len; j++)                            //TODO
                      add_words(words, (*(puzzle + i) + j), tmp, max);
            break;
        }
        w_count = 4 * tmp + 4;
        w_list = alloc_memory(w_count, tmp);
        
        next_mat = offset_i = offset_j = 0;
        while(next_mat < max_mat * max_mat){
            for(i = 0; i < tmp; i++){
                for(j = 0; j < tmp; j++){
                    w_list[i][j]           = puzzle[i + offset_i][j + offset_j];
                    w_list[i + tmp][j]     = puzzle[j + offset_i][i + offset_j];
                    w_list[i + tmp * 2][j] = puzzle[i + offset_i][tmp - 1 - j + offset_j];
                    w_list[i + tmp * 3][j] = puzzle[tmp - 1 - j + offset_i][i + offset_j];
                }
                w_list[tmp * 4][i]     = puzzle[i + offset_i][i + offset_j];
                w_list[tmp * 4 + 1][i] = puzzle[tmp - 1 - i + offset_i][tmp- 1 - i + offset_j];
                w_list[tmp * 4 + 2][i] = puzzle[tmp - 1 - i + offset_i][i + offset_j];
                w_list[tmp * 4 + 3][i] = puzzle[i + offset_i][tmp - 1 - i + offset_j];
            }
            total_matrices++, next_mat++, offset_j++;
            if(offset_j + tmp > p_len){
                offset_j = 0;
                offset_i++;
            }
         //   printf("total matrices: %d\n", total_matrices); 
            for(i = 0; i < w_count; i++){                      //TODO
                if(next_mat > max_mat){
                    if(i == 0 || i == 2 * tmp){
                        j = tmp - 1;
                        while(j != 1){
                            //printf("skipped words : %.*s\n", tmp, *(w_list + i));
                            i++, j--;
                        }
                        continue;
                    }
                }
                if(((next_mat - 1) % (p_len - tmp + 1)) && next_mat > 1){
                    if(i == tmp || i == 3 * tmp){
                        j = tmp - 1;
                        while(j != 1){
                            //printf("skipped words : %.*s\n", tmp, *(w_list + i));
                            i++, j--;
                        }
                        continue;
                    }
                }
                add_words(words, *(w_list + i), tmp, max);          
            }
        }
        if(*max > 1){
            free_matrix(w_list, w_count, tmp);
            break;
        }
        free_trie(words);
        free_matrix(w_list, w_count, tmp);
        tmp--, max_mat++; // set amount of sub-matrices (max_mat^2)
    }
    *w_len = tmp;
    words->count = 1;
    return words;
}

void print_words(word_t *words, char *word, int count, int w_len, int depth){
    for(int i = 0; i < MAX_NODES; i++){
        if(words->next[i]){
            word[depth] = INT_TO_CHAR(i);
            if(depth + 1 == w_len){
                if(words->next[i]->count > 1){
                    printf("%.*s\n", w_len, word);
                    continue;
                }
            }
            print_words(words->next[i], word, count, w_len, depth + 1);
        }
    }
}

int main(void){
    word_t  *longest_words = NULL;
    char    **puzzle = NULL;
    int     max_count = 1;
    int     p_len;
    int     w_len;

    printf("Hlavolam:\n");
    puzzle = get_puzzle(&p_len);
    if(!puzzle){
        printf("Nespravny vstup.\n");
        return 1;
    }
    longest_words = parse_puzzle(puzzle, p_len, &max_count, &w_len);
    if(!longest_words){
        printf("Nespravny vstup.\n");
        free_matrix(puzzle, p_len, p_len);
        return 1;
    }
    
    char *next_word = (char *)malloc(w_len * sizeof(*next_word));
    if(!next_word){
        printf("Nespravny vstup.\n");
        free_matrix(puzzle, p_len, p_len);
        free_trie(longest_words);
        return 1;
    }
    if(max_count == 1)
        printf("Zadne opakujici se slovo.\n");
    else{
        printf("Nejdelsi opakujici se slova:\n");
        print_words(longest_words, next_word, max_count, w_len, INIT_DEPTH);
    }
    free_matrix(puzzle, p_len, p_len);
    free_trie(longest_words);
    free(next_word);
    return 0;
}
