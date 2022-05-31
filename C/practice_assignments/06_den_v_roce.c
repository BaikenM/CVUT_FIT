#ifndef __PROGTEST__
#include <stdio.h>
#include <assert.h>
#endif /* __PROGTEST__ */

/* A program that determines which day of the year is the given input date */

// Check if the year is leap
int leapYear(int y){
    return ( y % 4 == 0 &&
            (y % 100 != 0 || y % 400 == 0 ) &&
             y % 4000 != 0 );
 }

// Check for valid date format
int validDate(int d, int m, int y)
{
    if(y < 2000 ||
    m < 1 || m > 12 ||
    d < 1 || d > 31)
        return 0;

    if((m < 8 && !(m % 2) && d > 30)||
    (m > 7 && (m % 2) && d > 30))
        return 0;

    if(m == 2)
    {
        if((!leapYear(y) && d > 28))
            return 0;
    }

    return 1;
}

// Convert date to a corresponding day of the year
int dateToIndex(int d, int m, int y, int *idx)
{
    int days = 0;

    if(!validDate(d, m, y))
        return 0;

    for(int j = 1; j < m; j++)
    {
        if(j < 8)
        {
            if(j == 2)
            {
                if(leapYear(y))
                    days += 29;
                else
                    days += 28;
                continue;
            }
            if((j % 2))
                days += 31;
            else
                days += 30;
        }
        else
        {
            if(j % 2)
                days += 30;
            else
                days += 31;
        }
    }
    *idx = days + d;
    return 1;
}

#ifndef __PROGTEST__
int main (int argc, char * argv []) {
    int idx;
    assert(dateToIndex( 1,  1, 2000, &idx) == 1 && idx == 1);
    assert(dateToIndex( 1,  2, 2000, &idx) == 1 && idx == 32);
    assert(dateToIndex(29,  2, 2000, &idx) == 1 && idx == 60);
    assert(dateToIndex(29,  2, 2001, &idx) == 0);
    assert(dateToIndex( 1, 12, 2000, &idx) == 1 && idx == 336);
    assert(dateToIndex(31, 12, 2000, &idx) == 1 && idx == 366);
    assert(dateToIndex( 1,  1, 1999, &idx) == 0);
    assert(dateToIndex( 6,  7, 3600, &idx) == 1 && idx == 188);
    assert(dateToIndex(29,  2, 3600, &idx) == 1 && idx == 60);
    assert(dateToIndex(29,  2, 4000, &idx) == 0);
    return 0;
}
#endif /* __PROGTEST__ */