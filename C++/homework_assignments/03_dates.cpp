#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

/* INFO: manipulators are not implemented (no bonus)
 *
 * An implementation of a class that uses the operator overloading techinque to work with dates.
 * */

const int MIN_YEAR = 2000;
const int MAX_YEAR = 2030;

// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
  public:
    InvalidDateException ( )
      : invalid_argument ( "invalid date or format" )
    {
    }
};

// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & ( * date_format ( const char * fmt ) ) ( ios_base & x )
{
  return [] ( ios_base & ios ) -> ios_base & { return ios; };
}

//Time representation and manipulation
class CDate
{
public:
  CDate(){}
  CDate(int year, int month, int day);
  CDate& operator + (int total_day_count);
  CDate& operator - (int total_day_count);
  CDate  operator ++();         // prefix increment
  CDate  operator ++(int);      // postfix increment
  CDate  operator --();         // prefix decrement
  CDate  operator --(int);      // postfix decrement

  // ADDITIONAL METHODS
  int  count_days_in_months(int m) const;
  bool is_leap_year() const;
  void convert_days_to_months(int& total_days_count, int m) const;
  int  convert_date_to_days() const;
  void set_new_date(int total_day_count);
  bool month_day_mismatch() const;
private:
  
  int m_Year;
  int m_Month;
  int m_Day;

  friend ostream& operator << (ostream& oss,     const CDate& date);
  friend istream& operator >> (istream& is,      CDate& date);
  friend bool     operator == (const CDate& lhs, const CDate& rhs);
  friend bool     operator != (const CDate& lhs, const CDate& rhs);
  friend bool     operator >= (const CDate& lhs, const CDate& rhs);
  friend bool     operator >  (const CDate& lhs, const CDate& rhs);
  friend bool     operator <= (const CDate& lhs, const CDate& rhs);
  friend bool     operator <  (const CDate& lhs, const CDate& rhs);
};

//================================ HELPING FUNCTIONS ================================

// count number of leap years in range (MIN_YEAR, year) 
int count_leap_years(int y)
{
  int leap_count = 0;
  int diff = y - MIN_YEAR;

  leap_count += diff / 4;
  if(diff % 4)
    leap_count += 1;
  return leap_count;
}
// true for April, June, September, November
bool has_thirty_days(int m){ return (m == 4 || m == 6 || m == 9 || m == 11); }

//================================ METHOD DEFINITIONS ================================

CDate::CDate(int year, int month, int day)
: m_Year(year),
  m_Month(month),
  m_Day(day)
{
  if(year < MIN_YEAR || year > MAX_YEAR
  || month < 1 || month > 12
  || day < 1 || month_day_mismatch())
    throw InvalidDateException();
}
// range of years is 2000 - 2030, check only m_Year (mod 4). (Mod 100) or (mod 400) is not needed
bool CDate::is_leap_year() const { return (m_Year % 4) ? false : true; }
int  CDate::count_days_in_months(int m) const
{
  if(m == 2) return is_leap_year() ? 29 : 28;
  else if(has_thirty_days(m)) return 30;
  else return 31;
}
void CDate::convert_days_to_months(int& total_days_count, int m) const
{
  if(m == 2)                  total_days_count -= 
                              is_leap_year() ? 29 : 28;
  else if(has_thirty_days(m)) total_days_count -= 30;
  else                        total_days_count -= 31;
}
int  CDate::convert_date_to_days() const
{
  int res;
  int years_to_days;
  int months_to_days = 0;
  int days_in_date;
  int i;

  years_to_days = (m_Year - MIN_YEAR) * 365;
  for(i = 1; i < m_Month; i++)
    months_to_days += count_days_in_months(i);
  days_in_date = years_to_days + months_to_days + m_Day;

  res = days_in_date + count_leap_years(m_Year);
  // cout << "RES: " << res << endl;
  return res;
}
void CDate::set_new_date(int total_day_count)
{
  m_Year = MIN_YEAR + total_day_count / 365;
  total_day_count -= (m_Year - MIN_YEAR) * 365 + count_leap_years(m_Year);

  for(m_Month = 1; total_day_count > 31; m_Month++)
    convert_days_to_months(total_day_count, m_Month);

  // TODO: test this
  if(m_Month == 2 && total_day_count > 28)
  {
    if(!is_leap_year())
    {
      total_day_count -= 28;
      m_Month++;
    }
    else if(total_day_count != 29)
    {
      total_day_count -= 29;
      m_Month++;
    }
  }

  m_Day = total_day_count;
}
bool CDate::month_day_mismatch() const
{
  if(m_Month == 2)
  {
    if(is_leap_year())
      return (m_Day > 29) ? true : false ;
    return (m_Day > 28) ? true : false;
  }
  else if(has_thirty_days(m_Month) 
       && m_Day > 30)                      return true;
  else if(m_Day > 31)                      return true;
  return false;
}
//================================ OVERLOADED OPERATORS ================================

// TODO: check if out of bounds [2000 - 2030]?
// TODO: addition of negative values, already in check? 
CDate & CDate::operator+(int total_day_count)
{
  total_day_count += convert_date_to_days();
  set_new_date(total_day_count);
  return *this;
}
// TODO: check if out of bounds [2000 - 2030]?
// TODO: subtraction of negative values, already in check? 
CDate & CDate::operator-(int days_to_subtract)
{
  int total_day_count = convert_date_to_days() 
                      - days_to_subtract;
  // cout << "TOTAL: " << total_day_count << endl;
  set_new_date(total_day_count);
  return *this;
}
bool operator==(const CDate& lhs, const CDate& rhs)
{
  return(lhs.m_Year  == rhs.m_Year
      && lhs.m_Month == rhs.m_Month
      && lhs.m_Day   == rhs.m_Day);
}
bool operator!=(const CDate& lhs, const CDate& rhs)
{
  return !(lhs == rhs);
}
bool operator>=(const CDate& lhs, const CDate& rhs)
{
  if(lhs.m_Year >= rhs.m_Year) return true;
  else return false;

  if(lhs.m_Month >= rhs.m_Month) return true;
  else return false;

  if(lhs.m_Day >= rhs.m_Day) return true;
  return false;
}
bool operator>(const CDate& lhs, const CDate& rhs)
{
  if(lhs.m_Year > rhs.m_Year) return true;
  if(lhs.m_Year < rhs.m_Year) return false;
  
  if(lhs.m_Month > rhs.m_Month) return true;
  if(lhs.m_Month < rhs.m_Month) return false;
  
  if(lhs.m_Day > rhs.m_Day) return true;
  return false;
}
bool operator<=(const CDate& lhs, const CDate& rhs)
{
  return !(lhs > rhs);
}
bool operator<(const CDate& lhs, const CDate& rhs)
{
  return !(lhs >= rhs);
}
// TODO: only difference is required, check if out of bounds?
int operator-(const CDate& lhs, const CDate& rhs)
{
  return lhs.convert_date_to_days() - rhs.convert_date_to_days();
}
ostream& operator<<(ostream& os, const CDate& date)
{
  os << date.m_Year << "-" ;
  
  if(date.m_Month < 10)
    os << "0";
  
  os << date.m_Month << "-";
  
  if(date.m_Day < 10)
    os << "0";
   
  os << date.m_Day;
  return os;
}
istream& operator>>(istream& is, CDate& date)
{
  CDate new_date;
  char delim, next_delim;

  // YEAR OUT OF BOUNDS
  if(!(is >> new_date.m_Year) || new_date.m_Year < MIN_YEAR || new_date.m_Year > MAX_YEAR)
  {
    is.setstate(ios::failbit);
    return is;
  }
  if(!is.get(delim))
  {
    is.setstate(ios::failbit);
    return is;
  }

  // MONTH OUT OF BOUNDS
  if(!(is >> new_date.m_Month) || new_date.m_Month < 1 || new_date.m_Month > 12)
  {
    is.setstate(ios::failbit);
    return is;
  }
  if(!is.get(next_delim) || next_delim != delim)
  {
    is.setstate(ios::failbit);
    return is;
  }
  
  // DAY OUT OF BOUNDS
  if(!(is >> new_date.m_Day) || new_date.m_Day < 1 || new_date.month_day_mismatch())
  {
    is.setstate(ios::failbit);
    return is;
  }
  // SET DATE
  date = new_date;
  return is;
}
CDate CDate::operator++()
{
  return *this + 1;
} 
CDate CDate::operator++(int)
{
  CDate tmp(*this);
  ++*this;
  return tmp;
} 
CDate CDate::operator--()
{
  return *this - 1;
} 
CDate CDate::operator--(int)
{
  CDate tmp(*this);
  --*this;
  return tmp;
} 
//======================================================================================

#ifndef __PROGTEST__
int main ( void )
{
  ostringstream oss;
  istringstream iss;

  CDate a ( 2000, 1, 2 );
  CDate b ( 2010, 2, 3 );
  CDate c ( 2004, 2, 10 );
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2000-01-02" );
  oss . str ("");
  oss << b;
  assert ( oss . str () == "2010-02-03" );
  oss . str ("");
  oss << c;
  assert ( oss . str () == "2004-02-10" );
  a = a + 1500;
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2004-02-10" );
  b = b - 2000;
  oss . str ("");
  oss << b;
  assert ( oss . str () == "2004-08-13" );
  assert ( b - a == 185 );
  assert ( ( b == a ) == false );
  assert ( ( b != a ) == true );
  assert ( ( b <= a ) == false );
  assert ( ( b < a ) == false );
  assert ( ( b >= a ) == true );
  assert ( ( b > a ) == true );
  assert ( ( c == a ) == true );
  assert ( ( c != a ) == false );
  assert ( ( c <= a ) == true );
  assert ( ( c < a ) == false );
  assert ( ( c >= a ) == true );
  assert ( ( c > a ) == false );
  a = ++c;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-11 2004-02-11" );
  a = --c;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-10 2004-02-10" );
  a = c++;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-10 2004-02-11" );
  a = c--;
  oss . str ( "" );
  oss << a << " " << c;
  assert ( oss . str () == "2004-02-11 2004-02-10" );
  iss . clear ();
  iss . str ( "2015-09-03" );
  assert ( ( iss >> a ) );
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2015-09-03" );
  a = a + 70;
  oss . str ("");
  oss << a;
  assert ( oss . str () == "2015-11-12" );

  CDate d ( 2000, 1, 1 );
  try
  {
    CDate e ( 2000, 32, 1 );
    assert ( "No exception thrown!" == nullptr );
  }
  catch ( ... )
  {
  }
  iss . clear ();
  iss . str ( "2000-12-33" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-01-01" );
  iss . clear ();
  iss . str ( "2000-11-31" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-01-01" );
  iss . clear ();
  iss . str ( "2000-02-29" );
  assert ( ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-02-29" );
  iss . clear ();
  iss . str ( "2001-02-29" );
  assert ( ! ( iss >> d ) );
  oss . str ("");
  oss << d;
  assert ( oss . str () == "2000-02-29" );

  try
  {
    CDate e ( 2000, 2, 30 );
    assert ( "No exception thrown!" == nullptr );
  }
  catch ( ... )
  {
  }


  // //-----------------------------------------------------------------------------
  // // bonus test examples
  // //-----------------------------------------------------------------------------
  CDate f ( 2000, 5, 12 );
  oss . str ("");
  oss << f;
  assert ( oss . str () == "2000-05-12" );
  // oss . str ("");
  // oss << date_format ( "%Y/%m/%d" ) << f;
  // assert ( oss . str () == "2000/05/12" );
  // oss . str ("");
  // oss << date_format ( "%d.%m.%Y" ) << f;
  // assert ( oss . str () == "12.05.2000" );
  // oss . str ("");
  // oss << date_format ( "%m/%d/%Y" ) << f;
  // assert ( oss . str () == "05/12/2000" );
  // oss . str ("");
  // oss << date_format ( "%Y%m%d" ) << f;
  // assert ( oss . str () == "20000512" );
  // oss . str ("");
  // oss << date_format ( "hello kitty" ) << f;
  // assert ( oss . str () == "hello kitty" );
  // oss . str ("");
  // oss << date_format ( "%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%" ) << f;
  // assert ( oss . str () == "121212121212050505200020002000%%%%%" );
  // oss . str ("");
  // oss << date_format ( "%Y-%m-%d" ) << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-1" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-1-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-001-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-02" );
  // assert ( ( iss >> date_format ( "%Y-%m-%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2001-01-02" );
  // iss . clear ();
  // iss . str ( "05.06.2003" );
  // assert ( ( iss >> date_format ( "%d.%m.%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2003-06-05" );
  // iss . clear ();
  // iss . str ( "07/08/2004" );
  // assert ( ( iss >> date_format ( "%m/%d/%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2004-07-08" );
  // iss . clear ();
  // iss . str ( "2002*03*04" );
  // assert ( ( iss >> date_format ( "%Y*%m*%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2002-03-04" );
  // iss . clear ();
  // iss . str ( "C++09format10PA22006rulez" );
  // assert ( ( iss >> date_format ( "C++%mformat%dPA2%Yrulez" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2006-09-10" );
  // iss . clear ();
  // iss . str ( "%12%13%2010%" );
  // assert ( ( iss >> date_format ( "%%%m%%%d%%%Y%%" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2010-12-13" );

  // CDate g ( 2000, 6, 8 );
  // iss . clear ();
  // iss . str ( "2001-11-33" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "29.02.2003" );
  // assert ( ! ( iss >> date_format ( "%d.%m.%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "14/02/2004" );
  // assert ( ! ( iss >> date_format ( "%m/%d/%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2002-03" );
  // assert ( ! ( iss >> date_format ( "%Y-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "hello kitty" );
  // assert ( ! ( iss >> date_format ( "hello kitty" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2005-07-12-07" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "20000101" );
  // assert ( ( iss >> date_format ( "%Y%m%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-01-01" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
