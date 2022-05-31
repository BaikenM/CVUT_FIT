#include <stdio.h>

/* A program that calculates a time interval between two timestamps */

int main( void ){
    int h, m, s, ms;
    int h1, m1, s1, ms1, h2, m2, s2, ms2;
    int t, t1, t2, n1, n2;

    printf("Zadejte cas t1:\n");
    if(scanf(" %d : %d : %d , %n%d%n", &h1, &m1, &s1, &n1, &ms1, &n2) != 4
       || h1 < 0 || h1 > 23
       || m1 < 0 || m1 > 59
       || s1 < 0 || s1 > 59
       || ms1 < 0 || ms1 > 999)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    if ((n2 - n1) == 1)
        ms1 *= 100;
    if ((n2 - n1) == 2)
        ms1 *= 10;

    printf("Zadejte cas t2:\n");
    if(scanf(" %d : %d : %d , %n%d%n", &h2, &m2, &s2, &n1, &ms2, &n2) != 4
       || h2 < 0 || h2 > 23
       || m2 < 0 || m2 > 59
       || s2 < 0 || s2 > 59
       || ms2 < 0 || ms2 > 999)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    if ((n2 - n1) == 1)
        ms2 *= 100;
    if ((n2 - n1) == 2)
        ms2 *= 10;
    t1 = ms1 + s1 * 1000 + m1 * 60 * 1000 + h1 * 60 * 60 * 1000;
    t2 = ms2 + s2 * 1000 + m2 * 60 * 1000 + h2 * 60 * 60 * 1000;
    t = t2 - t1;
    if (t < 0)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    int sec, min;
    ms = t % 1000;
    sec = ((t - ms) / 1000);
    s = sec % 60;
    min = ((sec - s) /60);
    m = min % 60;
    h = (min - m) / 60;
    
    printf("Doba: %2d:%02d:%02d,%03d\n", h, m, s, ms);
    return 0;
}