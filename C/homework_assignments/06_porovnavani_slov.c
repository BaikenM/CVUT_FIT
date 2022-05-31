#ifndef __PROGTEST__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#endif /* __PROGTEST__ */
 
/* An implementation of a simple function that determines 
 * whether two strings are comprised of the same words. 
 * */

int same_letters(char a, char b){
    return ((a == b) 
            || (a == (islower(b) ? toupper(b) : tolower(b))) 
            || (b == (islower(a) ? toupper(a) : tolower(b))));
}

int sameWords ( const char * a, const char * b )
{
    const char *a_next = a, *curr;
    const char *b_next = b;
    int         a_wc = 0;
    
    while(1){
        while(*a_next && !isalpha(*a_next)) // search for next word in a
            a_next++;
        curr = a_next;
        
        if(!(*a_next)) // SUCCESS
            return 1;

        while(*curr && *b_next 
                && isalpha(*curr) 
                && isalpha(*b_next) 
                && same_letters(*curr, *b_next)){ // check if letters in a and b are same
            curr++;
            b_next++;
        }
        if(!(*b_next)){ 
            if(!(*curr))
                return 1;
			else if(isalpha(*curr))
				return 0;
        }

        if(!isalpha(*curr) && !isalpha(*b_next)){ // word in a found in b
            a_next = curr;
            b_next = b;
            a_wc++;
        }
        else{ // search for next word in b
            curr = a_next;
            while(*b_next && isalpha(*b_next))
                b_next++;
            while(*b_next && !isalpha(*b_next))
                b_next++;
        }
    }
    return 1;
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
  assert ( sameWords ( "thisisalongwordaverylongwordbutthelengthisstillsmallerthanthelengthofthelongestwordintheworld thisisalongwordaverylongwordbutthelengthisstillsmallerthanthelengthofthelongestwordintheworld", "thisisalongwordaverylongwordbutthelengthisstillsmallerthanthelengthofthelongestwordintheworld" )
	== 1); 
  assert ( sameWords ( "one two three", "one two five" ) == 0 );
  assert ( sameWords ( "", "asd asd asd" ) == 1 );
  assert ( sameWords ( "a", "a" ) == 1 );
  assert ( sameWords ( "IT said: 'He would do it.'", "He said he would do it." ) == 1 );
  assert ( sameWords ( "Hello students.", "HELLO studEnts!" ) == 1 );
  assert ( sameWords ( " He said 'hello!'", "'Hello.' he   said." ) == 1 );
  assert ( sameWords ( "He said he would do it.", "IT said: 'He would do it.'" ) == 1 );
  return 0;
}
#endif /* __PROGTEST__ */
