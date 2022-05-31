#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* This program finds all occurences of a given acronym that could be found 
 * across all the words in a given line (given that next letter of an acronym 
 * is being searched from the pos of the previous one).
 *
 */

/*---------------------------------------------------------------------------*/

char *get_acronym();

char *process_line(char *line, int len, int *new_len);

int next_query(char *cmd, int *n_max);

void search_text(char *line, char *w_pos, char cmd, char *acr, int found, int n_max, int *lim_arr, int lim_t);
    
void print_limit(char cmd, int n_max, int *limit_arr);
    
int find_acronyms(char *acr);
    
/*---------------------------------------------------------------------------*/
int main(void){
    char *acr = NULL;
    
    /* trying to get an acronym to search for */
    acr = get_acronym();
    if(!acr){
        printf("Nespravny vstup.\n");
        return 1;
    }
    
    /* search for an acronym */
    if(!find_acronyms(acr)){
        printf("Nespravny vstup.\n");
        free(acr);
        return 1;
    } 
    free(acr);
    return 0;
}

/*---------------------------------------------------------------------------*/

/** Reading input to get an acronym, store it as char array, process it, 
 * ensuring it meets ProgTest conditions.
 *
 * @return a pointer to the acronym, NULL if an error occured while reading 
 * input or acronym conditions were not met
 * */
char *get_acronym(){
    char *acr = NULL;
    size_t len;
    ssize_t n_read;
    
    printf("Zkratka:\n");
    /* check for read error and acronym length,
     * using getline to automatically determine/increase the length */
    if((n_read = getline(&acr, &len, stdin)) == -1 || n_read <= 1){
        free(acr);
        return NULL;
    }
    
    /* check whether acronym consists of uppercase letters only,
     * then convert acronym to lowercase for searching purposes */
    for(int i = 0; i < n_read - 1; i++){
        if(!isalpha(acr[i]) || islower(acr[i])){
            free(acr);
            return NULL;
        }
        else
            acr[i] = tolower(acr[i]);
    }
    /* convert acr to string */
    acr[n_read - 1] = '\0';
    return acr;
}

/*---------------------------------------------------------------------------*/

/** Check for the presence of double quotes on either sides of the line.
 * Convert all uppercase character to lowercase.
 *
 * @param[in] line the line to process
 * @param[in] l length of processed line
 * @param[out] new_l length of new_line (including '\0')
 * @return new line copied from line, NULL if length of line is less 
 * than 2 or either one of the double quotes have not been found
 * */
char *process_line(char *line, int l, int *new_l){
    int start = 0, end = l - 1, quote_start, quote_end;
    char *new_line = NULL;
    
    /* line cannot be shorter than 2 characters (pair of double quotes) */
    if(l < 2) 
        return NULL;
    
    /* skip leading spaces, set the actual start of the line */
    while(line[start] == ' ') start++; 
        quote_start = start;

    /* skip trailing spaces, set the actual end of the line */
    while(line[end] == ' ') end--;
        quote_end = end;

    /* NULL if either one of the double quotes is missing */
    if(line[start] != '"' || line[end] != '"')
        return NULL;
    
    /* allocate enough space for new_line (including double quotes and '\0'), 
     * copy the contents of line between double quotes to avoid working with 
     * whitespaces on either sides of line */
    *new_l = quote_end - quote_start + 2;
    new_line = (char *)malloc(*new_l * sizeof(*new_line));
    memcpy(new_line, line + quote_start, *new_l);
    new_line[*new_l - 1] = '\0';
    
    /* convert all uppercase characters */
    for(int i = 1; i < *new_l - 1; i++)
        if(isalpha(new_line[i]) && isupper(new_line[i]))
                new_line[i] = tolower(new_line[i]);
    return new_line;
}

/*---------------------------------------------------------------------------*/

/** Get a query type and max number of characters that an acronym can be read
 * from a single word.
 *
 * @param[in] cmd query type
 * @param[out] n_max number of characters for a single word 
 * @return 1 on success, 0 if ProgTest conditions were not met or an error
 * occured while reading input
 * */
int next_query(char *cmd, int *n_max){
    if(scanf("%c", cmd) == 1 
            && (*cmd == '?' || *cmd == '#') 
            && (getchar() == ' ') /* a space that separates cmd and n_max */
            && (scanf("%d", n_max) == 1) 
            && *n_max > 0)
        return 1;
    else if(feof(stdin)) /* scanf ran into an error */
        return 1;
    return 0;
}

/*---------------------------------------------------------------------------*/

/** Recursively search for acronyms. If current acronym letter is found, the 
 * search for the next letter continues from the current letter position.
 * 
 * @param[in] line target line to search acronym upon
 * @param[in] w_pos current word position, if acronym letter is found,
 * continue the search from here
 * @param[in] cmd query type to determine whether to print out resulting line
 * @param[in] acr acronym to find
 * @param[in] found number of acronym letters found in current word
 * @param[in] n_max number of characters that are allowed to be found in a
 * single word 
 * @param[in] lim_arr array of limits 
 * @param[in] lim_t type of limit (number varies from 1 to n_max)
 * */
void search_text(char *line, char *w_pos, char cmd, char *acr, int found, int n_max, int *lim_arr, int lim_t){
    bool incremented = false; /* variable to help with control flow */
    
    /* go through each letter in line */
    for(char *pos = w_pos; *pos != '\0'; pos++){
        if(*pos == ' '){    /* word end reached, reset found for next word */
            found = 0;
            continue;
        }
        /* current acronym letter found */
        if(*pos == *acr){
            /* set next limit type */
            if(lim_t < found){
                incremented = true;
                lim_t++;
            }
            
            /*convert current letter to print as required in ProgTest*/
            *pos = toupper(*pos);

            /* keep searching for letters if any left, otherwise finish current search */
            if(*(acr + 1) != '\0'){
                if(found + 1 == n_max){
                    char *pos_alpha = pos;
                    while(isalpha(*pos_alpha)) pos_alpha++; /* jump to the next wozrd */
                    search_text(line, pos_alpha, cmd, acr + 1, 0, n_max, lim_arr, lim_t);
                }
                else
                    search_text(line, pos + 1, cmd, acr + 1, found + 1, n_max, lim_arr, lim_t);
            }
            else{
                lim_arr[lim_t]++;
                if(cmd == '?')
                    printf("%s\n", line);
            }
            
            /* convert current letter back to lowercase before proceeding to search next */
            *pos = tolower(*pos);
            if(incremented)
                lim_t--;
            incremented = false;
        }
    }
}

/*---------------------------------------------------------------------------*/

/** Count the amount of acronym occurences and print out in a way determined by
 * the type of query (cmd).
 *
 * @param[in] cmd query type, count total limit for '?', count separate limits
 * individually for '#'
 * @param[in] n_max
 * @param[in] limit_arr
 * */
void print_limit(char cmd, int n_max, int *limit_arr){
    int limit = 0;
    
    switch(cmd){
        case '?':
            for(int i = 0; i < n_max; i++)
                limit += limit_arr[i];
            printf("> %d\n", limit);
            break;
        case '#':
            for(int i = 1; i < n_max; i++)
                limit_arr[i] += limit_arr[i - 1];
            for(int i = 0; i < n_max; i++)
                printf("> limit %d: %d\n", i + 1, limit_arr[i]);
    }
}

/*---------------------------------------------------------------------------*/

/** Read input until an error occures while getting an acronym, a target line
 * or proccessing the target line (indicated by n_read = -1).
 *
 * @param[in] acr acronym to search
 * @return 1, after successfully operating on the line, 0 if errors occured
 * */
int find_acronyms(char *acr){
    char   *target = NULL, *new_target = NULL;
    char    cmd;
    int    *limit_arr = NULL;
    int     new_len, n_max;
    size_t  n;
    ssize_t n_read;
    
    printf("Problemy:\n");
    while(1){
        /* get details on the next query */
        if(!next_query(&cmd, &n_max)){
            n_read = -1;
            break;
        }
        
        /* get target line to search for acronyms */
        if((n_read = getline(&target, &n, stdin)) == -1
                || !isspace(target[0])){
            break;
        }
        target[n_read - 1] = '\0';
        
        /* process the target line before searching */
        if((new_target = process_line(target, n_read - 1, &new_len)) == NULL){
            n_read = -1;
            break;
        }
        
        /* number of acronym occurences are later stored in an array */
        limit_arr = (int *)calloc(n_max, sizeof(*limit_arr));
       
        /* search new_target for acronyms */
        search_text(new_target, new_target, cmd, acr, 0, n_max, limit_arr, 0);
      
        /* print the number of occurences */
        print_limit(cmd, n_max, limit_arr);
        
        /* clean up */
        free(limit_arr);
        free(new_target);
    }
    free(target);
    if(n_read == -1 && !feof(stdin))
        return 0;
    return 1;
}

/*---------------------------------------------------------------------------*/
