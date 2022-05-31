#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif /* __PROGTEST__ */

/* A program that calculates the amount of energy consumed 
 * by the 7-segment display between two given dates.
 * */

#define D_CONS 292886LL // daily consumption
#define H_CONS 12200LL  // hourly consumption
#define I_CONS 200LL    // minute consumption

// Determine whether the year is leap
int is_leap_year ( const int y )
{
    return ( y % 4 == 0 && 
             (y % 100 != 0 || y % 400 == 0 ) &&
             y % 4000 != 0 );
}

// Check for valid date format
int valid_date ( int y, int m, int d, int h, int i )
{
    if ( y < 1600 ||
         m < 1 || m > 12 ||
         d < 1 || d > 31 ||
         h < 0 || h > 23 ||
         i < 0 || i > 59 )
        return 0;
    
    if ( ( m < 8 && !( m % 2 ) && d > 30 ) ||
         ( m > 7 && ( m % 2 ) && d > 30 ) )
        return 0;
    
    if ( ( m == 2 && is_leap_year ( y ) && d > 29 ) ||
         ( m == 2 && !is_leap_year ( y ) && d > 28 ) )
            return 0;
    return 1;
}

// Calculate the amount of dates in a given month
long long int days_in_months ( const int y, const int m )
{
    long long days = 0;

    for ( int j = 1; j < m; j++ )
    {
        if ( j < 8 ) 
        {
            if ( j == 2)
            {
                if ( is_leap_year ( y ) )
                    days += 29;
                else
                    days += 28;
                continue;
            }
            if ( ( j % 2 ) ) 
                days += 31;
            else
                days += 30;
        }
        else
        {
            if ( j % 2 )
                days += 30;
            else
                days += 31;
        }
    }
    return days;
}

// Check for valid input
int valid_interval ( int y1, int m1, int d1, int h1, int i1,
                     int y2, int m2, int d2, int h2, int i2, long long int * consumption  )
{
    if ( y1 > y2 
    || ( y1 == y2 && m1 > m2 )
    || ( y1 == y2 && m1 == m2 && d1 > d2 ) 
    || ( y1 == y2 && m1 == m2 && d1 == d2 && h1 > h2 )
    || ( y1 == y2 && m1 == m2 && d1 == d2 && h1 == h2 && i1 > i2 ) )
        return 0;
    if ( !valid_date ( y2, m2, d2, h2, i2 ) 
      || !valid_date ( y1, m1, d1, h1, i1 ) ) 
        return 0;
    return 1;
}

/* Calculate changes of a 7-segment display for each number,
 * 0 to 1 requires 4 changes, 1 to 2 - 5, etc. 
 * */
long long int count_display_changes ( int time_unit, char op_type )
{
    long long dc = 0;
    const int cl_constants[] = { 4, 5, 2, 3, 3, 1, 5, 4, 1 };
    int iter;

    if ( op_type == '/' )
        iter = time_unit / 10;
    else
        iter = time_unit % 10;
     
    for (int i = 0; i < iter; i++ )
    {
        dc += cl_constants[i];
    }
    return dc;
}

// Caclculate consumption for a given date since the beginning of the corresponding year
long long int date_consumption ( int y, int m, int d, int h, int i )
{
    long long date_cons, h_cons, i_cons, s_cons, time_cons;
    
    h_cons = count_display_changes ( h, '/' ) 
           + ( h / 10 ) * 30
           + count_display_changes ( h, '%' )
           + h * H_CONS;
    i_cons = count_display_changes ( i, '/' )
           + ( i / 10 ) * 30
           + count_display_changes ( i, '%' );
    s_cons =  i * I_CONS;

    time_cons = h_cons + i_cons + s_cons;
    date_cons = ( days_in_months ( y, m ) + ( d - 1 ) ) * D_CONS + time_cons;
    return date_cons;
}

int energyConsumption ( int y1, int m1, int d1, int h1, int i1,
                        int y2, int m2, int d2, int h2, int i2, long long int * consumption )
{
    long long int date1_cons, date2_cons, dates_cons, years_cons = 0; 
    long long int leap_years = 0;

    // Check for valid input
    if ( !valid_interval ( y1, m1, d1, h1, i1, y2, m2, d2, h2, i2, consumption ) )
        return 0;
    
    // Count leap years
    for ( int iter = y1 + 1; iter < y2; iter++ )
    {
        if (iter % 4)
            continue;
        if ( is_leap_year ( iter ) )
            leap_years++;
    }
    
    // Calculate total energy consumption for each date
    date1_cons = date_consumption ( y1, m1, d1, h1, i1 );
    date2_cons = date_consumption ( y2, m2, d2, h2, i2 );
    
    // If the same year, then simply substract
    if ( y1 == y2 )
        dates_cons = date2_cons - date1_cons;
    else
    {
        if ( is_leap_year ( y1 ) )
            dates_cons = ( 366 * D_CONS - date1_cons ) + date2_cons; 
        else
            dates_cons = ( 365 * D_CONS - date1_cons ) + date2_cons; 
        years_cons = ( ( y2 - ( y1 + 1 ) ) * 365 + leap_years ) * D_CONS;
    }
    *consumption = dates_cons + years_cons;
    return 1;
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
    long long int consumption;
    
    assert ( energyConsumption ( 1600, 5, 17, 14, 22, 2147483647, 7, 2, 12, 45, &consumption ) == 1);

    assert ( energyConsumption ( 2076, 5, 17, 14, 22, 2081, 7, 2, 12, 45, &consumption ) == 1
             && consumption == 548262860LL );
    assert ( energyConsumption ( 2021, 10,  1, 13, 15,
                                 2021, 10,  1, 18, 45, &consumption ) == 1
             && consumption == 67116LL );
    assert ( energyConsumption ( 2021, 10,  1, 13, 15,
                                 2021, 10,  2, 11, 20, &consumption ) == 1
             && consumption == 269497LL );
    assert ( energyConsumption ( 2021,  1,  1, 13, 15,
                                 2021, 10,  5, 11, 20, &consumption ) == 1
             && consumption == 81106033LL );
    assert ( energyConsumption ( 2024,  1,  1, 13, 15,
                                 2024, 10,  5, 11, 20, &consumption ) == 1
             && consumption == 81398919LL );
    assert ( energyConsumption ( 1900,  1,  1, 13, 15,
                                 1900, 10,  5, 11, 20, &consumption ) == 1
             && consumption == 81106033LL );
    assert ( energyConsumption ( 2021, 10,  1,  0,  0,
                                 2021, 10,  1, 12,  0, &consumption ) == 1
             && consumption == 146443LL );
    assert ( energyConsumption ( 2021, 10,  1,  0, 15,
                                 2021, 10,  1,  0, 25, &consumption ) == 1
             && consumption == 2035LL );
   
    assert ( energyConsumption ( 2021, 10,  1, 12,  0,
                                 2021, 10,  1, 12,  0, &consumption ) == 1
             && consumption == 0LL );
    assert ( energyConsumption ( 2021, 10,  1, 12,  0,
                                 2021, 10,  1, 12,  1, &consumption ) == 1
             && consumption == 204LL );
    assert ( energyConsumption ( 2021, 11,  1, 12,  0,
                                 2021, 10,  1, 12,  0, &consumption ) == 0 );
    assert ( energyConsumption ( 2021, 10, 32, 12,  0,
                                 2021, 11, 10, 12,  0, &consumption ) == 0 );
    assert ( energyConsumption ( 2100,  2, 29, 12,  0,
                                 2100,  2, 29, 12,  0, &consumption ) == 0 );
    assert ( energyConsumption ( 2400,  2, 29, 12,  0,
                                 2400,  2, 29, 12,  0, &consumption ) == 1
             && consumption == 0LL );
    return 0;
}
#endif /* __PROGTEST__ */
