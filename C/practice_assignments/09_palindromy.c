#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* A program that indicates if an input sequence is a palindrome */

int check_palindrome(char **pal){
    int is_palindrome = 1, case_sens = 1;
    int l_spaces, r_spaces;
    int len = strlen(*pal);
    int half = len / 2;

    if(len == 1 && isspace((*pal)[0]))
        return 0;

    for(int i = 0, j = i; i < half; i++, j++){
        l_spaces = 0, r_spaces = 0;
        while(isspace((*pal)[i])) {
            i++;
            l_spaces++;
            if(l_spaces == len)
                return 0;
        }
        while(isspace((*pal)[len - j - 1])){
            j++;
            r_spaces++;
        }
        
        if((*pal)[i] == (*pal)[len - j - 1] - ((int)'a' - (int)'A')
              ||(*pal)[len - j - 1] == (*pal)[i] - ((int)'a' - (int)'A')){
            case_sens = 0;
        }
        else if((*pal)[i] != (*pal)[len - j - 1]){
            is_palindrome = 0;
            //printf("pal[%d] = %c, pal[%d - %d - 1] = %c\n", i, (*pal)[i],
            //        len, j, (*pal)[len - j -1]);
            break;
        }
    }
    

    if(is_palindrome)
        printf("Retezec je palindrom (case-%s).\n", 
                case_sens ? "sensitive" : "insensitive");
    else
        printf("Retezec neni palindrom.\n");
    return 1;
}

int get_palindrome(){
    char *line = NULL;
    size_t len;
    int line_len;

    printf("Zadejte retezec:\n");
    while((line_len = getline(&line, &len, stdin)) > 1){
        if(line[line_len - 1] != '\n'){
            free(line);
            return 0;
        }

        line[line_len - 1] = '\0';
        if(!check_palindrome(&line)){
            free(line);
            return 0;  
        }
    }
    if(!feof(stdin) && line_len <= 1){
        free(line);
        return 0;
    }
    free(line);
    return 1;
}

int main(){    
    if(!get_palindrome()){
        printf("Nespravny vstup.\n");
        return 1;
    }
    return 0;
}
