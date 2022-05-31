#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

/* A program that simulates company log management system and audit. */

struct Company
{
    string                m_Name;
    string                m_Addr;
    string                m_TaxID;
    unsigned int          m_Income;

    Company(const string& m_Name,
            const string& m_Addr,
            const string& m_TaxID);

    Company(const string& m_Name,
            const string& m_Addr);

    Company(const string& m_TaxID);
};

Company::Company ( const string& name,
                   const string& addr,
                   const string& taxID )
        : m_Name(name),
          m_Addr(addr),
          m_TaxID(taxID),
          m_Income(0){}

Company::Company(const string& name,
                 const string& addr)
        : m_Name(name),
          m_Addr(addr),
          m_TaxID(""),
          m_Income(0){}

Company::Company(const string& taxID)
        : m_Name(""),
          m_Addr(""),
          m_TaxID(taxID),
          m_Income(0){}

struct Invoice
{
    unsigned int m_Amount;

    explicit Invoice(unsigned int amount)
            : m_Amount(amount){}

    bool operator<(Invoice& rhs)
    { return this->m_Amount < rhs.m_Amount; }
};

class CVATRegister
{
public:
    CVATRegister   ( void );
    ~CVATRegister  ( void );
    bool          newCompany     ( const string    & name,
                                   const string    & addr,
                                   const string    & taxID );
    bool          cancelCompany  ( const string    & name,
                                   const string    & addr );
    bool          cancelCompany  ( const string    & taxID );
    bool          invoice        ( const string    & taxID,
                                   unsigned int      amount );
    bool          invoice        ( const string    & name,
                                   const string    & addr,
                                   unsigned int      amount );
    bool          audit          ( const string    & name,
                                   const string    & addr,
                                   unsigned int    & sumIncome ) const;
    bool          audit          ( const string    & taxID,
                                   unsigned int    & sumIncome ) const;
    bool          firstCompany   ( string          & name,
                                   string          & addr ) const;
    bool          nextCompany    ( string          & name,
                                   string          & addr ) const;
    unsigned int  medianInvoice  ( void ) const;


    bool found_comp_name_addr(const string& name,
                              const string& addr ) const;
    bool found_comp_taxID(const string& taxID ) const;
    
    // TODO: remove (debug)
    void print_companies() const;
    void print_invoices() const;
    bool company_already_exists() const;
private:
    vector<Company>  companies;
    vector<shared_ptr<Company>> companies_sorted_name;
    vector<shared_ptr<Company>> companies_sorted_ID;
    vector<Invoice>  invoices;
};

CVATRegister::CVATRegister ( void ){}
CVATRegister::~CVATRegister ( void ){
  // clear pointers before destructing vector of objects pointed to, dangling pointers prevention
  companies_sorted_name.clear();
  companies_sorted_ID.clear();
}

// CHeck if 'name' and 'addr' have valid format
bool valid_data_name_addr(const string& name, const string& addr)
{
  bool correct_format = false;

  // empty string - wrong
  if(name.length() == 0 || addr.length() == 0) return false;

  // at least one char is name whitespace
  for(size_t sz = 0; sz < name.length(); sz++)
    if(!isspace(name[sz])) correct_format = true;

  if(!correct_format) return false;

  // reset flag for addr check
  correct_format = false;

  // at least one char in addr is whitespace
  for(size_t sz = 0; sz < addr.length(); sz++)
    if(!isspace(addr[sz])) correct_format = true;

  if(!correct_format) return false;
  return true;
}

// CHeck if 'addtaxID' has valid format
bool valid_data_taxID(const string& taxID)
{
  size_t len = taxID.length();

  if(!len) return false;
  return true;
}

// TODO: two similar functions, see are_same_strings()
int cmp_case_ins(const string& lhs, const string& rhs, unsigned int len)
{
  for(unsigned int i = 0; i < len; i++)
  {
    if(tolower(lhs[i]) < tolower(rhs[i]))
      return 1;
    if(tolower(lhs[i]) > tolower(rhs[i]))
      return 0;
  }
  return -1;
}

// comparison function, sort by company names and addresses in alphabetical order
bool cmp_comp_name_address(shared_ptr<Company>& next, const Company& target)
{
  int  pos_found;
  // get name lengths
  unsigned int min_len_name = min(next->m_Name.length(), target.m_Name.length());
  unsigned int max_len_name = max(next->m_Name.length(), target.m_Name.length());

  // compare names
  pos_found = cmp_case_ins(next->m_Name, target.m_Name, min_len_name);
  if(pos_found == 1)       return true;
  else if(pos_found == 0) return false;

  if(min_len_name != max_len_name)
  {
    if(min_len_name == next->m_Name.length()) return true;
    else return false;
  }

  // get length of shortest name
  unsigned int min_len_addr = min(next->m_Addr.length(), target.m_Addr.length());
  unsigned int max_len_addr = max(next->m_Addr.length(), target.m_Addr.length());

  // compare addresses
  pos_found = cmp_case_ins(next->m_Addr, target.m_Addr, min_len_addr);
  if(pos_found == 1)       return true;
  else if(pos_found == 0) return false;

  if(min_len_addr != max_len_addr)
  {
    if(min_len_addr == next->m_Addr.length()) return true;
    else return false;
  }
  return false;
}

// comparison function, sort by company taxIDs in alphabetical order
bool cmp_comp_id(shared_ptr<Company>& p1, const Company& p2)
{
  return p1->m_TaxID < p2.m_TaxID;
}

// TODO: almost identical with cmp_case_ins()
bool are_same_strings(const string& lhs, const string& rhs)
{
  if((lhs.length() != rhs.length())
     || (lhs.length() != rhs.length())) return false;

  for(size_t sz = 0; sz < lhs.length(); sz++)
    if(tolower(lhs[sz]) != tolower(rhs[sz])) return false;

  for(size_t sz = 0; sz < rhs.length(); sz++)
    if(tolower(rhs[sz]) != tolower(rhs[sz])) return false;
  return true;
}

// TODO: use of found_taxID is not efficient, need to optimize
bool CVATRegister::newCompany ( const string    & name,
                                const string    & addr,
                                const string    & taxID )
{
  // check company details format and search for ID match
  if(!valid_data_name_addr(name, addr) || found_comp_name_addr(name, addr)
  || !valid_data_taxID(taxID) || found_comp_taxID(taxID))
  {
    return false;
  }

  // new_company will be pointed to by p1 (to sort by name and addr) and p2 (to sort by taxID)
  Company   new_company = { name, addr, taxID };
  shared_ptr<Company> p1 = make_shared<Company>(new_company);
  shared_ptr<Company> p2 = p1;

  // deteermine positions for pointers in vectors
  auto low_name_addr = lower_bound(companies_sorted_name.begin(), companies_sorted_name.end(),
                                   new_company, cmp_comp_name_address);
  auto low_id        = lower_bound(companies_sorted_ID.begin(), companies_sorted_ID.end(),
                                   new_company, cmp_comp_id);

  // vector offset for pointer insertion
  ptrdiff_t comp_name_addr_offset = low_name_addr - companies_sorted_name.begin();
  ptrdiff_t comp_id_offset        = low_id - companies_sorted_ID.begin();

  // insert pointers maintaining order
  companies_sorted_name.insert(companies_sorted_name.begin() + comp_name_addr_offset, p1);
  companies_sorted_ID.insert(companies_sorted_ID.begin() + comp_id_offset, p2);
  companies.push_back(new_company);
  return true;
}

bool CVATRegister::cancelCompany ( const string    & name,
                                   const string    & addr )
{
  for(size_t i = 0; i < companies.size(); i++)
  {
    if(are_same_strings(companies_sorted_name[i]->m_Name, name)
       && are_same_strings(companies_sorted_name[i]->m_Addr, addr))
    {
      for(size_t j = 0; j < companies.size(); j++)
      {
        if(are_same_strings(companies_sorted_ID[j]->m_Name, name)
           && are_same_strings(companies_sorted_ID[j]->m_Addr, addr))
        {
          // unset posize_ter (taxID)
          companies_sorted_ID[j] = nullptr;
          companies_sorted_ID.erase(companies_sorted_ID.begin() + j);
          break;
        }
      }
      // unset posize_ter (name, addr)
      companies_sorted_name[i] = nullptr;
      companies_sorted_name.erase(companies_sorted_name.begin() + i);
      for(size_t j = 0; j < companies.size(); j++)
      {
        if(are_same_strings(companies[j].m_Name, name)
           && are_same_strings(companies[j].m_Addr, addr))
        {
          companies.erase(companies.begin() + j);
          break;
        }
      }
      return true;
    }
  }
  return false;
}

bool CVATRegister::cancelCompany ( const string    & taxID )
{
  for(size_t i = 0; i < companies.size(); i++)
  {
    if(companies_sorted_name[i]->m_TaxID == taxID)
    {
      for(size_t j = 0; j < companies.size(); j++)
      {
        if(companies_sorted_ID[j]->m_TaxID == taxID)
        {
          // unset posize_ter (taxID)
          companies_sorted_ID[j] = nullptr;
          companies_sorted_ID.erase(companies_sorted_ID.begin() + j);
          break;
        }
      }
      // unset posize_ter (name, addr)
      companies_sorted_name[i] = nullptr;
      companies_sorted_name.erase(companies_sorted_name.begin() + i);
      for(size_t j = 0; j < companies.size(); j++)
      {
        if(companies[j].m_TaxID == taxID)
        {
          companies.erase(companies.begin() + j);
          break;
        }
      }
      return true;
    }
  }
  return false;
}

bool CVATRegister::invoice ( const string    & taxID,
                             unsigned int      amount )
{
  // temporary object to make use of compare function
  Company temp = { taxID };

  // check if there already exists a company with temp's name and addr
  auto low_inv = lower_bound(companies_sorted_ID.begin(), companies_sorted_ID.end(),
                             temp, cmp_comp_id);

  if(low_inv == companies_sorted_name.end()) return false;

  // check for exact match
  if(companies_sorted_ID[low_inv - companies_sorted_ID.begin()]->m_TaxID == taxID)
  {
    companies_sorted_ID[low_inv - companies_sorted_ID.begin()]->m_Income += amount;
    Invoice new_invoice(amount);
    invoices.push_back(new_invoice);
    return true;
  }
  return false;
}

bool CVATRegister::invoice ( const string    & name,
                             const string    & addr,
                             unsigned int      amount )
{
  // temporary object to make use of compare function
  Company temp = { name, addr };

  // check if there already exists a company with temp's name and addr
  auto low_inv = lower_bound(companies_sorted_name.begin(), companies_sorted_name.end(),
                             temp, cmp_comp_name_address);


  if(low_inv == companies_sorted_name.end()) return false;

  // check for exact match
  if(are_same_strings(companies_sorted_name[low_inv - companies_sorted_name.begin()]->m_Name, name)
     && are_same_strings(companies_sorted_name[low_inv - companies_sorted_name.begin()]->m_Addr, addr))
  {
    companies_sorted_name[low_inv - companies_sorted_name.begin()]->m_Income += amount;
    Invoice new_invoice(amount);
    invoices.push_back(new_invoice);
    return true;
  }
  return false;
}

bool CVATRegister::audit ( const string    & name,
                           const string    & addr,
                           unsigned int    & sumIncome ) const
{
  // temporary object to make use of compare function
  Company temp = { name, addr };
  vector<shared_ptr<Company>>*temp_vect = (vector<shared_ptr<Company>>*)&companies_sorted_name;

  // check if there already exists a company with temp's name and addr
  auto low_inv = lower_bound(temp_vect->begin(), temp_vect->end(),
                             temp, cmp_comp_name_address);

  if(low_inv == temp_vect->end()) return false;

  // check for exact match
  if(are_same_strings(companies_sorted_name[low_inv - temp_vect->begin()]->m_Name, name)
     && are_same_strings(companies_sorted_name[low_inv - temp_vect->begin()]->m_Addr, addr))
  {
    sumIncome = companies_sorted_name[low_inv - temp_vect->begin()]->m_Income;
    return true;
  }
  return false;
}

bool CVATRegister::audit ( const string    & taxID,
                           unsigned int    & sumIncome ) const
{
  // temporary object to make use of compare function
  Company temp = { taxID };
  vector<shared_ptr<Company>> * temp_vect = (vector<shared_ptr<Company>>*)&companies_sorted_ID;

  // check if there already exists a company with temp's name and addr
  auto low_inv = lower_bound(temp_vect->begin(), temp_vect->end(),
                             temp, cmp_comp_id);

  if(low_inv == temp_vect->end()) return false;

  // check for exact match
  if(companies_sorted_ID[low_inv - temp_vect->begin()]->m_TaxID == taxID)
  {
    sumIncome = companies_sorted_ID[low_inv - temp_vect->begin()]->m_Income;
    return true;
  }
  return false;
}

bool CVATRegister::firstCompany ( string          & name,
                                  string          & addr ) const
{
  if(companies.empty()) return false;

  name = companies_sorted_name[0]->m_Name;
  addr = companies_sorted_name[0]->m_Addr;
  return true;
}

bool CVATRegister::nextCompany ( string          & name,
                                 string          & addr ) const
{
  if(companies.empty()) return false;

  Company temp(name, addr);

  vector<shared_ptr<Company>> * temp_vect = (vector<shared_ptr<Company>>*)&companies_sorted_name;

  auto low_comp = lower_bound(temp_vect->begin(), temp_vect->end(),
                              temp, cmp_comp_name_address);

  if(low_comp == temp_vect->end()) return false;
  if(low_comp + 1 == temp_vect->end()) return false;

  if(cmp_case_ins((*temp_vect)[low_comp - temp_vect->begin()]->m_Name, name, name.length()) == -1
     && cmp_case_ins((*temp_vect)[low_comp - temp_vect->begin()]->m_Addr, addr, addr.length()) == -1)
  {
    if((*temp_vect)[low_comp - temp_vect->begin()]->m_Name.length() == name.length()
       && (*temp_vect)[low_comp - temp_vect->begin()]->m_Addr.length() == addr.length())
    {
      name = (*temp_vect)[low_comp + 1 - temp_vect->begin()]->m_Name;
      addr = (*temp_vect)[low_comp + 1 - temp_vect->begin()]->m_Addr;
      return true;
    }
  }
  name = (*temp_vect)[low_comp - temp_vect->begin()]->m_Name;
  addr = (*temp_vect)[low_comp - temp_vect->begin()]->m_Addr;
  return true;
}

unsigned int CVATRegister::medianInvoice  ( void ) const
{
  if(invoices.empty()) return 0;

  vector<Invoice>*temp = (vector<Invoice>*)&invoices;
  sort(temp->begin(), temp->end());
  return invoices[ceil(invoices.size() / 2)].m_Amount;
}

// additional methods and functions
bool CVATRegister::found_comp_name_addr(const string& name,
                                        const string& addr ) const 
{
  for(auto const& it : companies)
    if(are_same_strings(it.m_Name, name) && are_same_strings(it.m_Addr, addr))
      return true;
  return false;
}

bool CVATRegister::found_comp_taxID(const string& taxID) const
{
  for(auto const& it : companies)
    if(it.m_TaxID == taxID)
      return true;
  return false;
}

// TODO: remove these functions (debug)
void CVATRegister::print_companies() const
{
  int i = 0;

  cout << "===================" << endl;
  cout << " LISTING companies..." << endl;
  for(auto const& c : companies_sorted_ID)
  {
    i++;
    cout << setw(2) << i << ". "
         << c->m_Name << ", "
         << c->m_Addr << ", "
         << c->m_TaxID << ", "
         << c->m_Income << endl;
  }

  cout << "  COMPANIES SORTED BY NAME" << endl;
  for(auto const& it : companies_sorted_name)
    cout << it->m_Name << ", " << it->m_Addr << ", " << it->m_TaxID << endl;
  cout << "  COMPANIES SORTED BY TAXID" << endl;
  for(auto const& it : companies_sorted_ID)
    cout << it->m_Name << ", " << it->m_Addr << ", " << it->m_TaxID << endl;
  cout << "===================" << endl << endl;
}

void CVATRegister::print_invoices() const
{
  cout << endl << "PRINTING INVOICES" << endl;
  for(auto const& it : invoices)
  {
    cout << "-> " << it.m_Amount << endl;;
  }
  cout << "DONE PRINTING INVOICES" << endl << endl;
}

#ifndef __PROGTEST__
int               main           ( void )
{
  string name, addr;
  unsigned int sumIncome;

  // CVATRegister b1;

  // assert ( b1 . newCompany ( "ACME", "Thakurova", "666/666" ) );
  // assert ( b1 . newCompany ( "BCME", "Thakurova", "/666" ) );
  // assert ( b1 . newCompany ( "CCME", "Thakurova", "6/666" ) );
  // assert ( b1 . newCompany ( "DCME", "Thakurova", "66/666" ) );

  // assert ( ! b1 . audit ( "", "", sumIncome ) );

  // assert ( b1 . medianInvoice() == 0 );
  // assert ( b1 . invoice ( "666/666", 2000 ) );
  // assert ( b1 . medianInvoice() == 2000 );
  
  // assert ( b1 . audit ( "acme", "thakurova", sumIncome ) && sumIncome == 2000 );

  // assert ( b1 . cancelCompany ( "ACME", "thakurova" ) );
  // assert ( ! b1 . audit ( "acme", "thakurova", sumIncome ) );
  // assert ( ! b1 . audit ( "666/666", sumIncome ) );
  // assert ( b1 . newCompany ( "ACME", "Thakurova", "666/666" ) );
  // assert ( b1 . audit ( "acme", "thakurova", sumIncome ) && sumIncome == 0 );
  
  // b1.print_invoices();
  // b1.print_companies();
  
  // assert ( b1 . audit ( "666/666", sumIncome ) && sumIncome == 0 );
  // assert ( b1 . cancelCompany ( "ACME", "thakurova" ) );

  CVATRegister b1;
  assert ( b1 . newCompany ( "ACME", "Thakurova", "666/666" ) );
  assert ( b1 . newCompany ( "ACME", "Kolejni", "666/666/666" ) );
  assert ( b1 . newCompany ( "Dummy", "Thakurova", "123456" ) );
  assert ( b1 . invoice ( "666/666", 2000 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "666/666/666", 3000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 4000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "aCmE", "Kolejni", 5000 ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . audit ( "ACME", "Kolejni", sumIncome ) && sumIncome == 8000 );
  assert ( b1 . audit ( "123456", sumIncome ) && sumIncome == 4000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "ACME" && addr == "Kolejni" );
  assert ( b1 . nextCompany ( name, addr ) && name == "ACME" && addr == "Thakurova" );
  assert ( b1 . nextCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "ACME", "KoLeJnI" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . cancelCompany ( "666/666" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . invoice ( "123456", 100 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 300 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 230 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 830 ) );
  assert ( b1 . medianInvoice () == 830 );
  assert ( b1 . invoice ( "123456", 1830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 3200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "123456" ) );
  assert ( ! b1 . firstCompany ( name, addr ) );

  CVATRegister b2;
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . newCompany ( "Dummy", "Kolejni", "123456" ) );
  assert ( ! b2 . newCompany ( "AcMe", "kOlEjNi", "1234" ) );
  assert ( b2 . newCompany ( "Dummy", "Thakurova", "ABCDEF" ) );
  assert ( b2 . medianInvoice () == 0 );
  assert ( b2 . invoice ( "ABCDEF", 1000 ) );
  assert ( b2 . medianInvoice () == 1000 );
  assert ( b2 . invoice ( "abcdef", 2000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( b2 . invoice ( "aCMe", "kOlEjNi", 3000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . invoice ( "1234567", 100 ) );
  assert ( ! b2 . invoice ( "ACE", "Kolejni", 100 ) );
  assert ( ! b2 . invoice ( "ACME", "Thakurova", 100 ) );
  assert ( ! b2 . audit ( "1234567", sumIncome ) );
  assert ( ! b2 . audit ( "ACE", "Kolejni", sumIncome ) );
  assert ( ! b2 . audit ( "ACME", "Thakurova", sumIncome ) );
  assert ( ! b2 . cancelCompany ( "1234567" ) );
  assert ( ! b2 . cancelCompany ( "ACE", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Thakurova" ) );
  assert ( b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . cancelCompany ( "ACME", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Kolejni" ) );

  // CVATRegister b3;
  // assert ( b3 . newCompany ( "Name9", "Address9", "9" ) );
  // assert ( b3 . newCompany ( "Name7", "Address7", "7" ) );
  // assert ( b3 . newCompany ( "Name1", "Address1", "1" ) );
  // assert ( b3 . newCompany ( "Name3", "Address3", "3" ) );
  // assert ( b3 . newCompany ( "Name5", "Address5", "5" ) );

  // b3.print_companies();

  // assert ( b3 . firstCompany ( name, addr ) && name == "Name1" && addr == "Address1" );

  // name = "Name4";
  // addr = "Address4";
  // assert ( b3 . nextCompany ( name, addr ) && name == "Name5" && addr == "Address5" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
