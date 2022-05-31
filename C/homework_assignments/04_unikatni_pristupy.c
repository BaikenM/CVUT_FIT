#include <stdio.h>
#include <stdlib.h>

/* A program that simulates the behaviour of a simple logging system. 
 * Based on the input, it either determines how many times has a given user already logged in
 * or how many unique users have logged in a given range of logins.  
 */

#define MAX_USERS 100000
#define CAPACITY  1000000

/********************************* FUNCTION DECLARATIONS *********************************/

int process_query(int *u_db, int *access_log);

void query_unique_user(int *u_db, int id, int *unique_u);

int query_unique_access(int *log, int from, int to);

void quick_sort(int *arr, int start, int end);

int partition(int *arr, int start, int end);
    
void swap_elements(int *arr, int elem1, int elem2);

/********************************* DRIVER FUNCTION *********************************/

int main(void){
    int user_database[MAX_USERS] = { 0 };
    int access_log[CAPACITY];
    
    if(!process_query(user_database, access_log)){
        printf("Nespravny vstup.\n");
        return 1;
    }
    return 0;
}

/********************************* FUNCTION DEFINITIONS *********************************/

// Keep processing input logs until EOF or an invalid input (+ or ? accepted only)
int process_query(int *u_db, int *log){
    char cmd;
    int id, from, to;
    int unique_users = 0, record_count = 0;

    printf("Pozadavky:\n");
    while(scanf("%c", &cmd) != -1){
        if(record_count > CAPACITY)
            return 0;
        
        switch(cmd){
            // Log a user
            case '+':
                if(scanf(" %d", &id) != 1 
                        || id < 0 
                        || id >= MAX_USERS){
                    return 0;
                }
                log[record_count] = id;
                record_count++;
                
                query_unique_user(u_db, id, &unique_users);
                
                getchar();
                break;
            // Query unique users in range [from .. to]
            case '?':
                if(scanf(" %d %d", &from, &to) != 2 
                        || from < 0 
                        || to >= record_count 
                        || from > to){
                    return 0;
                }
                
                if(!query_unique_access(log, from, to))
                    return 0;
                
                getchar();
                break;
            default:
                return 0;
        }
    }
    if(!feof(stdin))
        return 0;
    return 1;
}

// Log the user to the system
void query_unique_user(int *u_db, int id, int *unique_u){
    u_db[id]++;

    if(u_db[id] == 1){
        printf("> prvni navsteva\n");
        (*unique_u)++;
    }
    else
        printf("> navsteva #%d\n", u_db[id]);
}

// Count the amount of unique users (first login) in range [from .. to]
int query_unique_access(int *log, int from, int to){
    int unique_u = 1, range = to - from + 1;
    int *query_log;

    // try to allocate memory for queried log copy of range from -> to
    query_log = (int *)malloc(range * sizeof(int));
    if(!query_log)
        return 0;

    // copy queried log
    for(int i = 0; i < range; i++)
        query_log[i] = log[from + i];
    
    // sort copy, select sort
    quick_sort(query_log, 0, range - 1); 

    // count unique users
    for(int i = 0; i < range - 1; i++)
        if(query_log[i] != query_log[i + 1])
                unique_u++;
        
    printf("> %d / %d\n", unique_u, range);
    free(query_log);
    return 1;
}

// Sort the array (custom quick sort implementation)
void quick_sort(int *arr, int start, int end){
    int pivot_idx;
    
    if(start < end){
        pivot_idx = partition(arr, start, end);
        quick_sort(arr, start, pivot_idx - 1);
        quick_sort(arr, pivot_idx + 1, end);
    }
}

// Quick-sort utility function
int partition(int *arr, int start, int end){
    int pivot_idx = start;
    int pivot_value = arr[end];

    for(int i = start; i < end; i++){
        if(arr[i] < pivot_value){
            swap_elements(arr, i, pivot_idx);
            pivot_idx++;
        }
    }
    swap_elements(arr, pivot_idx, end);
    return pivot_idx;
}

// Quick-sort utility function
void swap_elements(int *arr, int elem1, int elem2){
    int tmp;

    tmp = arr[elem1];
    arr[elem1] = arr[elem2];
    arr[elem2] = tmp;
}
