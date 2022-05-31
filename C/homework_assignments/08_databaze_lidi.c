#ifndef __PROGTEST__
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* INFO: program is complete, passed 4.9992/5.0000 on ProgTest, but seems unnecessarily 
 * bloated for its purposes and desperately needs proper documentation.
 * 
 * An implementation of a linked list with a more complex set of defined operations. 
 * */

typedef struct TResult
{
  struct TResult * m_Next;
  int              m_ID;
  char           * m_Name;
} TRESULT;
#endif /* __PROGTEST__ */

typedef struct TDatabase
{
  int                 m_ID;
  char              * m_Name;
  int                 m_Depth;
  struct TDatabase  * m_Root;
  struct TDatabase  * m_Left;
  struct TDatabase  * m_Right;
  struct TDatabase  * m_Id1;
  struct TDatabase  * m_Id2;

} TDATABASE;

// DEBUG
void print_db(TDATABASE * db){
  if(db -> m_Left)
    print_db(db -> m_Left);
  if(db -> m_Right)
    print_db(db -> m_Right);

  printf("m_ID: %d,    name: %s (depth: %d)\n", db -> m_ID, db -> m_Name, db -> m_Depth);
  if(db -> m_Left)
    printf("m_Left: %d, name: %s, (depth: %d)\n", db -> m_Left -> m_ID, db -> m_Left -> m_Name, db -> m_Left -> m_Depth);
  if(db -> m_Right)
    printf("m_Right: %d, name: %s, (depth: %d)\n", db -> m_Right -> m_ID, db -> m_Right -> m_Name, db -> m_Right -> m_Depth);
  if(db -> m_Id1)
    printf("m_Id1: %d,   name: %s\n", db -> m_Id1 -> m_ID, db -> m_Id1 -> m_Name);
  if(db -> m_Id2)
    printf("m_Id2: %d,   name: %s\n", db -> m_Id2 -> m_ID, db -> m_Id2 -> m_Name);
}

void      initAll          ( TDATABASE       * db )
{
  db -> m_ID      = 0;
  db -> m_Name    = NULL;
  db -> m_Depth   = 0;
  db -> m_Root    = NULL;
  db -> m_Left    = NULL;
  db -> m_Right   = NULL;
  db -> m_Id1     = NULL;
  db -> m_Id2     = NULL;
}

void delete_parents(TDATABASE * db){
  if(!db)
    return;
  delete_parents(db -> m_Left);
  delete_parents(db -> m_Right);

  db -> m_Id1 = NULL;
  db -> m_Id2 = NULL;
}

void      doneAll          ( TDATABASE       * db )
{
  if(!db)
    return;
  /* unlink all parent nodes */
  delete_parents(db -> m_Root);
  /* if db is part of the tree */
  if(db -> m_ID){
    doneAll(db -> m_Left);
    doneAll(db -> m_Right);
    
    free(db -> m_Name);
    db -> m_Name  = NULL;
    db -> m_Left  = NULL;
    db -> m_Right = NULL;
    free(db);
    db            = NULL;
    return;
  }
  /* otherwise erase the tree (deinitialize the starting point) */
  else{
    doneAll(db -> m_Root);
    db -> m_Root = NULL;
  }
}

TDATABASE * add_parent(TDATABASE * db, int id){
  if(!id)
    return NULL;
  
  if(id == db -> m_ID)
    return db;
  else if(id < db -> m_ID){
    return add_parent(db -> m_Left, id);
  }
  else{
    return add_parent(db -> m_Right, id);
  }
}

int calc_depth(TDATABASE * db){
  if(!db)
    return 0;
  return db -> m_Depth;
}

int calc_balance(TDATABASE * db){
  if(!db)
    return 0;
  return calc_depth(db -> m_Left) - calc_depth(db -> m_Right);
}

int max(int x, int y){ return (x < y) ? y : x; }

TDATABASE * rotate_left(TDATABASE * db){
  TDATABASE * tmp1 = db -> m_Right;
  TDATABASE * tmp2 = tmp1 -> m_Left;

  tmp1 -> m_Left = db;
  db -> m_Right = tmp2;

  db -> m_Depth = max(calc_depth(db -> m_Left), calc_depth(db -> m_Right)) + 1;
  tmp1 -> m_Depth = max(calc_depth(tmp1 -> m_Left), calc_depth(tmp1 -> m_Right)) + 1;
  return tmp1;
}

TDATABASE * rotate_right(TDATABASE * db){
  TDATABASE * tmp1 = db -> m_Left;
  TDATABASE * tmp2 = tmp1 -> m_Right;

  tmp1 -> m_Right = db;
  db -> m_Left = tmp2;

  db -> m_Depth = max(calc_depth(db -> m_Left), calc_depth(db -> m_Right)) + 1;
  tmp1 -> m_Depth = max(calc_depth(tmp1 -> m_Left), calc_depth(tmp1 -> m_Right)) + 1;
  return tmp1;
}

TDATABASE * balance_tree(TDATABASE * db){
  int balance = calc_balance(db);

  //printf("balance: %d\n", balance);
  if(balance < -1 || balance > 1)
    //printf("tree is unbalanced!\n");
  /* Righ Right case */
  if(balance < -1 && db -> m_ID < db -> m_Right -> m_ID){
    //printf("performing left rotate on %s...\n", db -> m_Name);
    return rotate_left(db);
  }
  /* Left Left case */
  if(balance > 1 && db -> m_ID > db -> m_Left -> m_ID){
    //printf("performing right rotate...\n");
    return rotate_right(db);
  }
  /* Right Left case */
  if(balance < -1 && db -> m_ID > db -> m_Right -> m_ID){
    //printf("performing right rotate then left rotate on %s...\n", db -> m_Name);
    db -> m_Right = rotate_right(db -> m_Right);
    return rotate_left(db);
  }
  /* Left Right case */
  if(balance > 1 && db -> m_ID < db -> m_Left -> m_ID){
    //printf("performing left rotate then right rotate...\n");
    db -> m_Left = rotate_left(db -> m_Left);
    return rotate_right(db);
  }
  //printf("tree/subtree is balanced!\n");
  return db;
}

int id_valid(TDATABASE * db, int id, int parent){
  if(!id)   /* has no parent, valid */
    return parent ? 1 : 0;
  if(!db)  /* empty entry: searching for parent id - failure, otherwise success */
    return parent ? 0 : 1;
  if(id == db -> m_ID) /* found match: searching for parent id - success, otherwise failure */
    return parent ? 1 : 0;
  if(id < db -> m_ID)   /* keep searching in subtrees */
    return id_valid(db -> m_Left, id, parent);
  else
    return id_valid(db -> m_Right, id, parent);
}

TDATABASE * init_entry(TDATABASE * root, TDATABASE * db, int id, const char * name, int id1, int id2){
  TDATABASE * entry = (TDATABASE *)malloc(sizeof(*entry));

  entry -> m_ID = id;
  entry -> m_Name = (char *)malloc((strlen(name) + 1) * sizeof(char));
  strcpy(entry -> m_Name, name);
  entry -> m_Depth = 1;
  entry -> m_Root  = NULL;
  entry -> m_Left  = NULL;
  entry -> m_Right = NULL;
  /* setting parents */
  entry -> m_Id1 = add_parent(root, id1);
  entry -> m_Id2 = add_parent(root, id2);
  return entry;
}

TDATABASE * new_entry(TDATABASE * root, TDATABASE * db, int id, const char * name, int id1, int id2){
  /* base case, initialize a new entry in the database */
  if(!db)
    return init_entry(root, db, id, name, id1, id2);
  /* recursively add entry in the right subtree */
  if(id > db -> m_ID){
    db -> m_Right = new_entry(root, db -> m_Right, id, name, id1, id2);
  }
  /* otherwise recursively add entry in the left subtree */
  else{
    db -> m_Left = new_entry(root, db -> m_Left, id, name, id1, id2);
  }
  /* update depth of every ancestor node */
  db -> m_Depth = max(calc_depth(db -> m_Left), calc_depth(db -> m_Right)) + 1;
  /* check and balance an unbalanced tree */
  return balance_tree(db);
}

int       addPerson        ( TDATABASE       * db,
                             int               id,
                             const char      * name,
                             int               id1,
                             int               id2 )
{
  /* might be useful if db is a pointer passed by value rather than by reference */
  if(!db){
    db = new_entry(db -> m_Root, db, id, name, id1, id2);
    return 1;
  }
  /* check if IDs are valid */
  if(!id_valid(db -> m_Root, id, 0) 
      || !id_valid(db -> m_Root, id1, 1)
      || !id_valid(db -> m_Root, id2, 1)
      || ((id1 == id2) && id1))
    return 0;
  /* create new entry and update db -> root (actual database start) */
  db -> m_Root = new_entry(db -> m_Root, db -> m_Root, id, name, id1, id2);
  return 1;
}

TDATABASE * init_list(TDATABASE * db, int id){
  if(!db)
    return NULL;
  
  if(id == db -> m_ID)      /* node found */
    return db;
  else if(id < db -> m_ID)  /* search left subtree */
    return init_list(db -> m_Left, id);
  else                      /* search right subtree */
    return init_list(db -> m_Right, id);
}

void print_ancestors(TRESULT * db){
  if(!db)
    return;
  
  TRESULT * tmp = db;
  while(tmp){
    //printf("next ancestors: %d, %s\n", tmp -> m_ID, tmp -> m_Name);
    tmp = tmp -> m_Next;
  }
}

int in_list(TRESULT * list, int id){
  TRESULT * tmp = list;
  
  while(tmp){
    if(id == tmp -> m_ID){
      //printf("%d is already in list!\n", id);
      tmp = NULL;
      return 1;
    }
    tmp = tmp -> m_Next;
  }
  return 0;
}

TRESULT * find_ancestors(TDATABASE * db, TRESULT * ancestors){
  TRESULT * next = NULL;
  
  if(!db)
    return NULL;
   
  if(db -> m_Id1)
    ancestors = find_ancestors(db -> m_Id1, ancestors);
  if(db -> m_Id2)
    ancestors = find_ancestors(db -> m_Id2, ancestors);
      
  if(!in_list(ancestors, db -> m_ID)){
    /* add this ancestors to the list */
    next = (TRESULT *)malloc(sizeof(*next));
    next -> m_ID = db -> m_ID;
    next -> m_Name = (char*)malloc((strlen(db -> m_Name) + 1) * sizeof(char));
    strcpy(next -> m_Name, db -> m_Name);
    next -> m_Next = ancestors;
    return next;
  }
  return ancestors;
}

TRESULT * ancestors        ( TDATABASE       * db,
                             int               id )
{
  if(!db || !id)
    return NULL;

  TDATABASE * list_start = init_list(db -> m_Root, id);;
  TRESULT * anc_list = NULL;
  
  if(!list_start) /* node with such id does not exist */
    return NULL;
  
  anc_list = find_ancestors(list_start -> m_Id1, anc_list); 
  //print_ancestors(anc_list);
  anc_list = find_ancestors(list_start -> m_Id2, anc_list);
  //print_ancestors(anc_list);
  return anc_list;

}

TRESULT * compare_lists(TRESULT * id1, TRESULT * id2){
  TRESULT * res       = NULL;
  TRESULT * res_next  = NULL;
  
  for(TRESULT * i = id1; i; i = i -> m_Next){ /* traverse list */
    for(TRESULT * j = id2; j; j = j -> m_Next){
      if(i -> m_ID == j -> m_ID){
        res_next = res;
        res = (TRESULT *)malloc(sizeof(*res));
        res -> m_ID = j -> m_ID;
        res -> m_Name = (char *)malloc((strlen(j -> m_Name) + 1) * sizeof(char));
        strcpy(res -> m_Name, j -> m_Name);
        res -> m_Next = res_next;
        break;
      }
    }
  }
  return res;
}

void      freeResult       ( TRESULT         * res )
{
  if(!res)
    return;
  freeResult(res -> m_Next);
  free(res -> m_Name);
  free(res);
}

TRESULT * commonAncestors  ( TDATABASE       * db,
                             int               id1,
                             int               id2 )
{
  if(!db || !id1 || ! id2)
    return NULL;

  TDATABASE * id1_anc     = init_list(db -> m_Root, id1);
  TDATABASE * id2_anc     = init_list(db -> m_Root, id2);
  TRESULT   * id1_list    = NULL;
  TRESULT   * id2_list    = NULL;
  TRESULT   * common_list = NULL;

  if(!id1_anc || ! id2_anc)
    return NULL;

  id1_list = find_ancestors(id1_anc -> m_Id1, id1_list);
  id1_list = find_ancestors(id1_anc -> m_Id2, id1_list);
  
  id2_list = find_ancestors(id2_anc -> m_Id1, id2_list);
  id2_list = find_ancestors(id2_anc -> m_Id2, id2_list);

  common_list = compare_lists(id1_list, id2_list);
  freeResult(id1_list);
  freeResult(id2_list);
  //printf("---------------common ancestors-----------------\n");
  //print_ancestors(common_list);
  return common_list;
}

#ifndef __PROGTEST__
int main                   ( int               argc,
                             char            * argv [] )
{
  char      name[100];
  TDATABASE a, b;
  TRESULT * l;

  initAll ( &a );
  assert ( addPerson ( &a, 1, "John", 0, 0 ) == 1 );
  strncpy ( name, "Jane", sizeof ( name ) );
  assert ( addPerson ( &a, 2, name, 0, 0 ) == 1 );
  assert ( addPerson ( &a, 3, "Caroline", 0, 0 ) == 1 );
  assert ( addPerson ( &a, 4, "Peter", 0, 0 ) == 1 );
  assert ( addPerson ( &a, 5, "George", 1, 2 ) == 1 );
  assert ( addPerson ( &a, 6, "Martin", 1, 2 ) == 1 );
  assert ( addPerson ( &a, 7, "John", 3, 4 ) == 1 );
  assert ( addPerson ( &a, 8, "Sandra", 3, 4 ) == 1 );
  assert ( addPerson ( &a, 9, "Eve", 1, 2 ) == 1 );
  assert ( addPerson ( &a, 10, "Douglas", 1, 4 ) == 1 );
  strncpy ( name, "Phillipe", sizeof ( name ) );
  assert ( addPerson ( &a, 11, name, 6, 8 ) == 1 );
  strncpy ( name, "Maria", sizeof ( name ) );
  assert ( addPerson ( &a, 12, name, 5, 8 ) == 1 );
  //print_db(a.m_Root);
  l = ancestors ( &a, 11 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 2 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_ID == 3 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Name, "Caroline" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_ID == 4 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "Peter" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 6 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Martin" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 8 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Sandra" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  freeResult ( l );
  assert ( ancestors ( &a, 3 ) == NULL );
  assert ( ancestors ( &a, 13 ) == NULL );
  l = commonAncestors ( &a, 11, 12 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 2 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_ID == 3 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Name, "Caroline" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_ID == 4 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "Peter" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 8 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Sandra" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  freeResult ( l );
  l = commonAncestors ( &a, 10, 9 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next == NULL );
  freeResult ( l );
  assert ( commonAncestors ( &a, 7, 6 ) == NULL );
  l = commonAncestors ( &a, 7, 10 );
  //assert ( l );
  //assert ( l -> m_ID == 4 );
  //assert ( ! strcmp ( l -> m_Name, "Peter" ) );
  //assert ( l -> m_Next == NULL );
  freeResult ( l );
  assert ( addPerson ( &a, 13, "Quido", 12, 11 ) == 1 );
  l = ancestors ( &a, 13 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 2 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_ID == 3 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Name, "Caroline" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_ID == 4 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "Peter" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 5 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "George" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 6 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Martin" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 8 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Sandra" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 11 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Phillipe" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 12 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Maria" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  freeResult ( l );
  l = commonAncestors ( &a, 9, 12 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 2 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next == NULL );
  freeResult ( l );
  assert ( addPerson ( &a, 1, "Francois", 0, 0 ) == 0 );
  initAll ( &b );
  assert ( addPerson ( &b, 10000, "John", 0, 0 ) == 1 );
  assert ( addPerson ( &b, 10000, "Peter", 0, 0 ) == 0 );
  assert ( addPerson ( &b, 20000, "Jane", 10000, 0 ) == 1 );
  assert ( addPerson ( &b, 30000, "Maria", 10000, 10000 ) == 0 );
  assert ( addPerson ( &b, 40000, "Joe", 10000, 30000 ) == 0 );
  assert ( addPerson ( &b, 50000, "Carol", 50000, 20000 ) == 0 );
  assert ( addPerson ( &b, 12, "Maria", 20000, 10000 ) == 1 );
  l = ancestors ( &a, 12 );
  //assert ( l );
  //assert ( l -> m_ID == 1 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 2 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_ID == 3 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Name, "Caroline" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_ID == 4 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "Peter" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 5 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "George" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_ID == 8 );
  //assert ( ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "Sandra" ) );
  //assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  freeResult ( l );
  l = ancestors ( &b, 12 );
  //assert ( l );
  //assert ( l -> m_ID == 10000 );
  //assert ( ! strcmp ( l -> m_Name, "John" ) );
  //assert ( l -> m_Next );
  //assert ( l -> m_Next -> m_ID == 20000 );
  //assert ( ! strcmp ( l -> m_Next -> m_Name, "Jane" ) );
  //assert ( l -> m_Next -> m_Next == NULL );
  freeResult ( l );
  assert ( ancestors ( &a, 20000 ) == NULL );
  assert ( commonAncestors ( &b, 7, 10 ) == NULL );
  doneAll ( &b );

  doneAll ( &a );

  return 0;
}
#endif /* __PROGTEST__ */
