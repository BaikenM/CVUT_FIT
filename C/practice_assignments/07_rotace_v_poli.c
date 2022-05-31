#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif /* __PROGTEST__ */

/* A program that swaps (mirror-like) the contents of an array */

void rotateArray ( int array [], int arrayLen, int rotateBy )
{
    int current, next, tmp1, tmp2, rotateCount = 0;

	rotateBy %= arrayLen;
    if(!rotateBy || arrayLen == 1)
		return;

    if(rotateBy < 0)
		rotateBy = arrayLen + rotateBy;

    for(int i = 0; i < rotateBy; i++){
        current = i;
        tmp1 = array[i];
        do {
            next = (current + rotateBy) % arrayLen;
            tmp2 = array[next];
            array[next] = tmp1;
            tmp1 = tmp2;
            current = next;
            rotateCount++;
        }
        while(current != i);
        if(rotateCount == arrayLen)
            break;
    }
}

#ifndef __PROGTEST__
int identicalArrays ( const int a[], const int b[], int cnt )
{
    while(cnt){
        if(a[cnt] != b[cnt])
            return 0;
        cnt--;
    }
    return 1;
}
int main (int argc, char * argv [])
{
  int in0 []  = { 1, 2, 3, 4, 5 };
  int out0 [] = { 5, 1, 2, 3, 4 };

  int in1 []  = { 1, 2, 3, 4, 5 };
  int out1 [] = { 2, 3, 4, 5, 1 };

  int in2 []  = { 1, 2, 3, 4, 5, 6 };
  int out2 [] = { 4, 5, 6, 1, 2, 3 };

  int in3 []  = { 1, 2, 3, 4, 5, 6, 7 };
  int out3 [] = { 3, 4, 5, 6, 7, 1, 2 };

  int in4 []  = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  int out4 [] = { 2, 3, 4, 5, 6, 7, 8, 9, 1 };

  rotateArray ( in0, 5, 1 );
  assert ( identicalArrays ( in0, out0, 5 ) );
  rotateArray ( in1, 5, -1 );
  assert ( identicalArrays ( in1, out1, 5 ) );
  rotateArray ( in2, 6, -3 );
  assert ( identicalArrays ( in2, out2, 6 ) );
  rotateArray ( in3, 7, 12 );
  assert ( identicalArrays ( in3, out3, 7 ) );
  rotateArray ( in4, 9, -100 );
  assert ( identicalArrays ( in4, out4, 9 ) );
  return 0;
}
#endif /* __PROGTEST__ */

