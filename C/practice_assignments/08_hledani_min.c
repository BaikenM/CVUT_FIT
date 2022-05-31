#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Minesweeper solver. Based on the input, prints out the matrix with the number of mines in adjacent cells */

// boards to operate on
typedef struct board{
    char    **content;
    int     height;
    int     width;
}BOARD;

/********************************* FUNCTION DECLARATIONS *********************************/

// scan through input, get board line by line until EOF
int get_board(BOARD *b);

// allocate space for a board
int alloc_mem(BOARD *b);

// calculate the resulting board
int calculate_mines(BOARD *in, BOARD *out);

// print resulting board
void print_board(BOARD *b);

// free space allocated for a board
void free_mem(BOARD *b);

/********************************* DRIVER FUNCTION *********************************/

int main(void){
    BOARD in_board = { NULL, 10, 10};
    BOARD out_board;

    if(!get_board(&in_board)){
        printf("Nespravny vstup.\n");
        free_mem(&in_board);
        return 1;
    }
   
    out_board = { NULL, in_board.height, in_board.width };
    calculate_mines(&in_board, &out_board);
    print_board(&out_board);
    free_mem(&in_board);
    free_mem(&out_board);
    return 0;
}

/********************************* FUNCTION DEFINITIONS *********************************/

void free_mem(BOARD *b){
    for(int i = 0; i < b->height; i++)
        free(b->content[i]);
    free(b->content);
}

int alloc_mem(BOARD *b){

    b->content = (char **)malloc(b->height * sizeof(char *));
    if(!b->content)
        return 0;
    for(int i = 0; i < b->height; i++){
        b->content[i] = (char *)malloc(b->width * sizeof(char));
        if(!b->content[i]){
            b->height = i;
            return 0;
        }
    }
    return 1;
}

void print_board(BOARD *b){
    printf("Vyplnena hraci plocha:\n");
    for(int i = 0; i < b->height; i++){
        for(int j = 0; j < b->width; j++){
            printf("%c", b->content[i][j]);
        }
        printf("\n");
    }
}

int get_board(BOARD *b){
    size_t buffer_len = 0;
    char *line = NULL;
    int h_current = 0;
    int line_len;

    printf("Zadejte hraci plochu:\n");
    if(!alloc_mem(b))
        return 0;
    while((line_len = getline(&line, &buffer_len, stdin)) != -1){
        if(line_len == 1){
            free(line);
            return 0;
        }
        h_current++;
        line_len--;
        if(h_current == 1){
            b->width = line_len;
            for(int i = 0; i < b->height; i++)
                b->content[i] = (char *)realloc(b->content[i], b->width * sizeof(char));
        }
        else if(line_len != b->width){
            free(line);
            return 0;
        }
        if(h_current == b->height - 1){
            b->height *= 2;
            b->content = (char **)realloc(b->content, b->height * sizeof(char *));
            if(!b->content){
                b->height /= 2;
                free(line);
                return 0;
            }
            for(int i = (b->height / 2); i < b->height; i++)
                b->content[i] = (char *)malloc(b->width * sizeof(char));
        }
        strncpy(b->content[h_current - 1], line, b->width);
        for(int i = 0; i < b->width; i++){
            if(b->content[h_current - 1][i] != '.' && b->content[h_current - 1][i] != '*'){
                free(line);
                return 0;
            }
        }
    }
    buffer_len = 0;
    free(line);
    line = NULL;
    if(feof(stdin)){
        if(!h_current)
            return 0;
        for(int i = h_current; i < b->height; i++)
            free(b->content[i]);
        b->height = h_current;
        b->content = (char **)realloc(b->content, b->height * sizeof(char *));
        
    } 
    else 
        return 0;
    return 1;
}

int has_mine(BOARD *b, int i, int j, int x, int y){
    return (b->content[i + x][j + y] == '*') ? 1 : 0;
}

int is_valid_cell(BOARD *b, int i, int j, int x, int y){
    return ((i + x >= 0) && (j + y >= 0) && (i + x <= b->height - 1) && (j + y <= b->width - 1));
}

int calculate_mines(BOARD *in, BOARD *out){
    int mine_count;

    if(!alloc_mem(out))
        return 0;
    for(int i = 0; i < out->height; i++){
        for(int j = 0; j < out->width; j++){
            mine_count = 0;

            if(is_valid_cell(in, i, j, -1, -1)){
                if(has_mine(in, i, j, -1, -1)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, -1, 0)){
                if(has_mine(in, i, j, -1, 0)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, -1, 1)){
                if(has_mine(in, i, j, -1, 1)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, 0, -1)){
                if(has_mine(in, i, j, 0, -1)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, 0, 1)){
                if(has_mine(in, i, j, 0, 1)){ 
                    mine_count++; 
                }
            }
            if(is_valid_cell(in, i, j, 1, -1)){
                if(has_mine(in, i, j, 1, -1)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, 1, 0)){
                if(has_mine(in, i, j, 1, 0)){ 
                    mine_count++;
                }
            }
            if(is_valid_cell(in, i, j, 1, 1)){
                if(has_mine(in, i, j, 1, 1)){ 
                    mine_count++;
                }
            }
            if(in->content[i][j] == '*')
                out->content[i][j] = '*';
            else if(mine_count)
                out->content[i][j] = mine_count + '0';
            else
                out->content[i][j] = '.';
        }
    }
    return 1;
}
