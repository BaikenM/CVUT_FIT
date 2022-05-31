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
#endif // __PROGTEST__ 

enum INFO { NAME, SURNAME, NUMBER };

class Contact{
public:
  void         set_name     (const string& name){ m_Name = name; }
  void         set_surname  (const string& surname){ m_Surname = surname; }
  void         set_number   (const string& number){ m_Number = number; }
  const string get_name()    const { return m_Name; }
  const string get_surname() const { return m_Surname; }
  const string get_number()  const { return m_Number; }
private:
  string m_Name;
  string m_Surname;
  string m_Number;
};

string extract_contact_info(string& contact, size_t& w_pos, size_t& nw_pos){
  string info;
  size_t cont_length = contact.length();

  // reset w_pos 
  w_pos = nw_pos;

  // find first name character 
  for(size_t i = w_pos; i < cont_length; i++){
    if(isspace(contact[w_pos]))
      w_pos++;
    else
     break;
  }

  // find last name character 
  nw_pos = w_pos;
  for(size_t i = w_pos; i < cont_length; i++){
    if(!isspace(contact[nw_pos]))
      nw_pos++;
    else
      break;
  }
  return contact.substr(w_pos, nw_pos - w_pos);
}

bool contains_wrong_character(const string & info){
  // numbers cannot start with 0
  if(info[0] == '0')
    return false;

  // numbers must contain digits only
  for(size_t i = 0; i < info.length(); i++)
    if(!isdigit(info[i]))
      return false;
  return true;
}

bool check_contact_info(const string& info, INFO type){
  switch (type)
  {
    // name and surname cannot be empty or their length be less than 1
    case NAME:
    case SURNAME:
      if(info.empty() || info.length() < 1)
        return false;
      break;

    // number cannot be empty
    case NUMBER:
      if(info.length() != 9 || !contains_wrong_character(info))
        return false;
      break;
  }
  return true;
}

bool add_contact(string& contact, vector<Contact>& contact_list){
  string  name;
  string  surname;
  string  number;
  size_t  w_pos  = 0;
  size_t  nw_pos = 0;
  Contact new_contact;
  
  // extract name
  new_contact.set_name(extract_contact_info(contact, w_pos, nw_pos)); 
  if(!check_contact_info(new_contact.get_name(), NAME))
    return false;

  // extract surname 
  new_contact.set_surname(extract_contact_info(contact, w_pos, nw_pos)); 
  if(!check_contact_info(new_contact.get_surname(), SURNAME))
    return false;
  
  // extract number 
  new_contact.set_number(extract_contact_info(contact, w_pos, nw_pos)); 
  if(!check_contact_info(new_contact.get_number(), NUMBER))
    return false;

  // check if line contains redundant attributes
  for(size_t i = nw_pos; i < contact.length(); i++)
    if(!isspace(contact[i]))
      return false;

  // add new contact to the vector 
  contact_list.push_back(new_contact);
  return true;
}

void add_contact_to_output_stream(ostream& out, Contact contact_to_add){
  out << contact_to_add.get_name()    << " ";
  out << contact_to_add.get_surname() << " ";
  out << contact_to_add.get_number()  << endl;
}

bool process_queries(const vector<Contact>& c_list, ifstream& c_stream, ostream& out){
  int    number_of_contacts_found = 0;
  string name_to_search;

  while(getline(c_stream, name_to_search)){
    // reset number of contacts found 
    number_of_contacts_found = 0;
    for(auto const& contact : c_list){
      // check if name (or surname) is found 
      if(name_to_search == contact.get_name()
      || name_to_search == contact.get_surname()){
        add_contact_to_output_stream(out, contact);
        number_of_contacts_found++;
      }
    }
    out << "-> " << number_of_contacts_found << endl;
  }
  return (number_of_contacts_found) ? true : false;
}

bool report ( const string & fileName, ostream & out )
{
  string contact_info;
  ifstream contact_stream(fileName);
  vector<Contact> contact_list;

  // check if file exists
  if(!contact_stream.good())
    return false;

  // get/check contact list first
  while(getline(contact_stream, contact_info)
    && !contact_info.empty()){

    // extract contact info -> create an object
    if(!add_contact(contact_info, contact_list))
      return false;
  }

  // check for newline between list and queries 
  if(contact_info.length() != 0)
    return false;

  // process contact queries 
  process_queries(contact_list, contact_stream, out);
  return true;
}

#ifndef __PROGTEST__
int main ()
{
  ostringstream oss;
  oss . str ( "" );
  assert ( report( "tests/test0_in.txt", oss ) == true );
  assert ( oss . str () ==
    "John Christescu 258452362\n"
    "John Harmson 861647702\n"
    "-> 2\n"
    "-> 0\n"
    "Josh Dakhov 264112084\n"
    "Dakhov Speechley 865216101\n"
    "-> 2\n"
    "John Harmson 861647702\n"
    "-> 1\n" );
  oss . str ( "" );
  assert ( report( "tests/test2_in.txt", oss ) == false );
  cout << oss.str();
  oss . str ( "" );
  assert ( report( "tests/test1_in.txt", oss ) == false );
  oss . str ( "" );
  assert ( report( "tests/test3_in.txt", oss ) == true );
  oss . str ( "" );
  assert ( report( "tests/test4_in.txt", oss ) == false );
  oss . str ( "" );
  assert ( report( "tests/test5_in.txt", oss ) == false );

  return 0;
}
#endif // __PROGTEST__ 
