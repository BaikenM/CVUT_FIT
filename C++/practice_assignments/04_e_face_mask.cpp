#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

class CTimeStamp
{
public:
  CTimeStamp(const int y, const int m, const int d, const int h, const int min, const int s)
  : year(y)
  , month(m)
  , day(d)
  , hour(h)
  , minute(min)
  , second(s)
  {}

  void get_time_stamp() const
  {
    cout << "TimeStamp: " 
         << year << ", " 
         << month << ", " 
         << day << ", " 
         << hour << ", " 
         << minute << ", " 
         << second << endl;
  }
  int get_year()   const { return year; }
  int get_month()  const { return month; }
  int get_day()    const { return day; }
  int get_hour()   const { return hour; }
  int get_minute() const { return minute; }
  int get_second() const { return second; }
private:
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

class CContact
{
public:
  CContact(const CTimeStamp& c_time, const int number1, const int number2) : contact_time(c_time) {
    id_person1 = number1;
    id_person2 = number2;
  }

  CTimeStamp get_timestamp() const { return contact_time; }
  int        get_id_1()      const { return id_person1; }
  int        get_id_2()      const { return id_person2; }
private:
  CTimeStamp contact_time;
  int        id_person1;
  int        id_person2;
};

bool same_timestamp(CTimeStamp ts1, CTimeStamp ts2)
{
  if(ts1.get_year() == ts2.get_year()
     && ts1.get_month() == ts2.get_month()
       && ts1.get_day() == ts2.get_day()
          && ts1.get_hour() == ts2.get_hour()
             && ts1.get_minute() == ts2.get_minute()
                && ts1.get_second() == ts2.get_second() )
    return true;
  return false;
}

bool compare_dates(const CTimeStamp& ts1, const CTimeStamp& ts2)
{
  if(ts1.get_year() < ts2.get_year())
    return true;
  if(ts1.get_year() == ts2.get_year()
    && ts1.get_month() < ts2.get_month())
    return true;
  if(ts1.get_year() == ts2.get_year()
    && ts1.get_month() == ts2.get_month()
    && ts1.get_day() < ts2.get_day())
    return true;
  if(ts1.get_year() == ts2.get_year()
    && ts1.get_month() == ts2.get_month()
    && ts1.get_day() == ts2.get_day()
    && ts1.get_hour() < ts2.get_hour())
    return true;
  if(ts1.get_year() == ts2.get_year()
    && ts1.get_month() == ts2.get_month()
    && ts1.get_day() == ts2.get_day()
    && ts1.get_hour() == ts2.get_hour()
    && ts1.get_minute() < ts2.get_minute())
    return true;
  if(ts1.get_year() == ts2.get_year()
    && ts1.get_month() == ts2.get_month()
    && ts1.get_day() == ts2.get_day()
    && ts1.get_hour() == ts2.get_hour()
    && ts1.get_minute() == ts2.get_minute()
    && ts1.get_second() <= ts2.get_second())
    return true;
  return false;
}

void print_logs(vector<CContact>& contact_records)
{
  cout << "PRINTING OUT LOGS:" << endl;
  for(auto & cr : contact_records)
  {
    cout << "  Contact time stamp: " << cr.get_timestamp().get_year()   << ", "
         << cr.get_timestamp().get_month()  << ", "
         << cr.get_timestamp().get_day()    << ", "
         << cr.get_timestamp().get_hour()   << ", "
         << cr.get_timestamp().get_minute() << ", "
         << cr.get_timestamp().get_second() << endl;
    cout << "     |ID_1: " << cr.get_id_1() << "|     |ID_2: " << cr.get_id_2() << "|" << endl;
  }
}

bool date_within_bounds(const CTimeStamp& date, const CTimeStamp& date_begin, const CTimeStamp& date_end)
{
  if(compare_dates(date_begin, date) && compare_dates(date, date_end))
    return true;
  return false;
}

class CEFaceMask
{
public:
  CEFaceMask& addContact(const CContact& contact)
  {
    contact_records.push_back(contact);
    cout << "ADDED: " << contact.get_id_1() << ", " << contact.get_id_2() << endl;
    return *this;
  }

  vector<int> listContacts(const int id_contact) const
  {
    vector<int> result_log;
    int         id_1, id_2;
    int         number_in_log;
    bool        found;

    for(auto const& cr : contact_records)
    {
      id_1 = cr.get_id_1();
      id_2 = cr.get_id_2();
      found = false;

      // exclude self-contact
      if(id_1 == id_2)
        continue;

      // no match
      if(id_contact != id_1 && id_contact != id_2)
        continue;

      number_in_log = (id_contact == id_1) ? id_2 : id_1;
      for(auto const& next : result_log)
      {
        if(next == number_in_log)
        {
          found = true;
          break;
        }
      }
      if(!found)
        result_log.push_back(number_in_log);
    }
    return result_log;
  }

  vector<int> listContacts(const int id_contact, const CTimeStamp& ts1, const CTimeStamp& ts2) const {
    vector<int>      result_log;
    int              id_1, id_2;
    int              number_in_log;
    bool             found;

    for(auto const& cr : contact_records)
    {
      id_1 = cr.get_id_1();
      id_2 = cr.get_id_2();
      found  = false;

      // exclude self-contact
      if(id_1 == id_2)
        continue;

      // no match
      if(id_contact != id_1 && id_contact != id_2)
        continue;
      
      number_in_log = (id_contact == id_1) ? id_2 : id_1;
      for(auto const& next : result_log)
      {
        if(next == number_in_log)
        {
          found = true;
          break;
        }
      }
      if(!found && date_within_bounds(cr.get_timestamp(), ts1, ts2))
        result_log.push_back(number_in_log);
    }
    return result_log;
  }
  vector<CContact> get_contact_records() const { return contact_records; }
private:
  vector<CContact> contact_records;
};

void print_result(vector<int>& result_log)
  {
    cout << "PRINTING OUT RESULT:" << endl;
    for(auto const& x: result_log)
      cout << x << endl;
    cout << "DONE" << endl;
  }

#ifndef __PROGTEST__
int main ()
{
  CEFaceMask test;

  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 10, 12, 40, 10 ), 123456789, 999888777 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 12, 12, 40, 10 ), 123456789, 111222333 ) )
       . addContact ( CContact ( CTimeStamp ( 2021, 2, 5, 15, 30, 28 ), 999888777, 555000222 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 2, 21, 18, 0, 0 ), 123456789, 999888777 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 5, 18, 0, 0 ), 123456789, 456456456 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 2, 1, 0, 0, 0 ), 123456789, 123456789 ) );
  assert ( test . listContacts ( 123456789 ) == (vector<int> {999888777, 111222333, 456456456}) );
  assert ( test . listContacts ( 999888777 ) == (vector<int> {123456789, 555000222}) );
  assert ( test . listContacts ( 191919191 ) == (vector<int> {}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 5, 18, 0, 0 ), CTimeStamp ( 2021, 2, 21, 18, 0, 0 ) ) == (vector<int> {999888777, 111222333, 456456456}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 5, 18, 0, 1 ), CTimeStamp ( 2021, 2, 21, 17, 59, 59 ) ) == (vector<int> {999888777, 111222333}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 10, 12, 41, 9 ), CTimeStamp ( 2021, 2, 21, 17, 59, 59 ) ) == (vector<int> {111222333}) );
  return 0;
}
#endif /* __PROGTEST__ */
