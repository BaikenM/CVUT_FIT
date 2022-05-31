#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;

/* INFO: both bonuses received (speed1 + speed2). A very effective usage of maps demonstrated.
 * 
 * A program that uses STL contatiners and algorithms (hashmaps, map, unordered map...
 * */

bool are_same_words(const string& lhs, const string& rhs);
void print_list(list<pair<string, int>>& prod);

class CProd;
//================================== CLASS/STRUCT DECLARATIONS ==================================

class CDate
{
	public:
		CDate(int year, int month, int day);
		CDate(const CDate& rhs);
		bool operator==(const CDate& rhs) const;
		bool operator<(const CDate& rhs) const;

		int get_year() const;
		int get_month() const;
		int get_day() const;
	private:
		int m_Year;
		int m_Month;
		int m_Day;
};

struct P_Date_Hash
{
	P_Date_Hash(){}
	size_t operator()(const CDate& key) const;
	bool operator()(const CDate& lhs, const CDate& rhs) const;
};

struct P_Name_Hash
{
	size_t operator()(const string& key) const;
	bool operator()(const string& lhs, const string& rhs) const;
};

typedef map<CDate, int>   												map_date_quantity;											 // map CDate to Quantity
typedef unordered_map<string, CProd, P_Name_Hash> u_map_name_prod;	                       // map Name to CDate

class CProd
{
	public:
		CProd(const string& name, const CDate& date, int q);
		bool operator<(const CProd& rhs) const;

	private:
		string m_Name;
		CDate  m_BestBefore;
		int m_Quantity;
		int m_TotalQ;
		map_date_quantity date_quantity;
		friend class CSupermarket;
};

class CSupermarket
{
	public:
		CSupermarket& 						 store(const  string& name,                                  // add products to stock
															  		 const  CDate& date, 
															  		 int quant);
		CSupermarket&					  	 sell(list<pair<string, int>>& groceries);									 // try to sell products from input list
		void											 hash_name_variants(const string& name);										 // hash variations of product name (1 misspell)
		list<pair<string, int>> 	 expired(const CDate& date) const;													 // get the list of products to be expired by given date 
		vector<pair<bool, string>> search_occurences(list<pair<string, int>> groceries) const; // check if product name (correct/misspelled) is in stock
		bool 										   reduce_quantity(pair<string, int>& pr_needed,							 // reduce amount of given product in stock after selling it
															    						 CProd& prod_available);
		void 										   erase_name_variants(const string& name);												 // remove misspelled variations of product name, if no left

	private:
		u_map_name_prod 								name_prod;																						 // map Name to Product
		unordered_map<string, string> name_variant;      																			 // map Name variants to itself
};

//================================== CLASS/STRUCT DEFINITIONS ==================================

CDate::CDate(int year, int month, int day)
: m_Year(year),
	m_Month(month),
	m_Day(day) {}
CDate::CDate(const CDate& rhs)
:	m_Year(rhs.m_Year),
	m_Month(rhs.m_Month),
	m_Day(rhs.m_Day) {}
bool CDate::operator==(const CDate& rhs) const
{
	return m_Year == rhs.m_Year
			&& m_Month == rhs.m_Month
			&& m_Day == rhs.m_Day;
}
bool CDate::operator<(const CDate& rhs) const
{
	if(get_year() < rhs.get_year()) return true;
	if(get_year() > rhs.get_year()) return false;
	if(get_month() < rhs.get_month()) return true;
	if(get_month() > rhs.get_month()) return false;
	if(get_day() < rhs.get_day()) return true;
	else return false;
}
int CDate::get_year() const { return m_Year; }
int CDate::get_month() const { return m_Month; }
int CDate::get_day() const { return m_Day; }

CProd::CProd(const string& name, const CDate& date, int q)
: m_Name(name),
	m_BestBefore(date),
	m_Quantity(q),
	m_TotalQ(q){}
bool CProd::operator<(const CProd& rhs) const
{	return m_BestBefore < rhs.m_BestBefore; }

CSupermarket& CSupermarket::store(const  string& name, 
																	const  CDate& date, 
																	int quant)
{
	auto it_name = name_prod.find(name);

	if(it_name != name_prod.end())
	{
		auto it_date = it_name->second.date_quantity.find(date);
		if(it_date != it_name->second.date_quantity.end())
		{
			it_date->second += quant;
			it_name->second.m_TotalQ += quant;
		}
		else
		{
			it_name->second.date_quantity.insert(make_pair(date, quant));
			it_name->second.m_TotalQ += quant;
		}
	}
	else // add name variants
	{
		hash_name_variants(name);

		CProd new_prod(name, date, quant);
		new_prod.date_quantity.insert(make_pair(date, quant));
		name_prod.insert(make_pair(name, new_prod));
	}
	return *this;
}

vector<pair<bool, string>> CSupermarket::search_occurences(list<pair<string, int>> groceries) const
{
	vector<pair<bool, string>> found_prod;
	string misspelled_word;
	int similar_words;

	for(const auto& it_gr : groceries)
	{
		// search for exact match
		auto exact_match = name_prod.find(it_gr.first); 
		if(exact_match != name_prod.end())
		{
			found_prod.push_back(make_pair(true, exact_match->first));
			continue;
		}
		// search for words with misspelles
		similar_words = 0;
		for(size_t i = 0; i < it_gr.first.length(); i++)
		{
			string name_copy(it_gr.first);

			name_copy[i] = ' ';			
			auto missmatch = name_variant.find(name_copy);
			if(missmatch != name_variant.end())
			{
				misspelled_word = missmatch->second;
				if(++similar_words > 1) // more than 1 match, quit
					break;
			}
		}
		similar_words == 1 
			? found_prod.push_back(make_pair(true, misspelled_word))
			:	found_prod.push_back(make_pair(false, ""));
	}
	return found_prod;
}

CSupermarket& CSupermarket::sell(list<pair<string, int>>& groceries)
{
	// check what products are available for selling
	vector<pair<bool, string>> available_in_stock = search_occurences(groceries);
	auto it_gr = groceries.begin();
	auto it_av = available_in_stock.begin();
	int  it_av_idx = 0;

	while(it_gr != groceries.end())
	{
		if((it_av + it_av_idx)->first == false)  // no name match for current product in list
		{ 
			it_av_idx++;
			it_gr++; 
			continue;
		}

		auto it_prod = name_prod.find((it_av + it_av_idx)->second);
		reduce_quantity(*it_gr, it_prod->second) 
			? it_gr++
			: it_gr = groceries.erase(it_gr);
		it_av_idx++;
	}
	return *this;
}

list<pair<string, int>> CSupermarket::expired(const CDate& date) const
{
	vector<CProd>							 date_sorted;
	vector<pair<string, int>>  quant_sorted;
	unordered_map<string, int> name_quant;

	// for each product name count total amount of quantity, add as pair to vector
	for(const auto& it_name : name_prod)
		for(const auto& it_date : it_name.second.date_quantity)
			date_sorted.push_back(CProd(it_name.first, it_date.first, it_date.second));

	// sort by CDate
	sort(date_sorted.begin(), date_sorted.end());

	// find pos of CDate indicating all expired products
	auto low_date = lower_bound(date_sorted.begin(), date_sorted.end(), date,
						[](const CProd& lhs, const CDate& rhs)
						{ return lhs.m_BestBefore < rhs; });

	// map unqiue name to quantities
	int iter_pos = low_date - date_sorted.begin();
	for(auto iter = date_sorted.begin(); iter != date_sorted.begin() + iter_pos; iter++)
		name_quant[iter->m_Name] += iter->m_Quantity; 

	// map to vector (to be able to sort)
	for(const auto& iter : name_quant)
		quant_sorted.push_back(make_pair(iter.first, iter.second));

	// now sort by quantity (descending)
	sort(quant_sorted.begin(), quant_sorted.end(),
							[](const pair<string, int>& p1, const pair<string, int>& p2)
							{ return p1.second > p2.second; });

	list<pair<string, int>> res(quant_sorted.begin(), quant_sorted.end());
	return res;
}
bool CSupermarket::reduce_quantity(pair<string, int>& prod_needed,
																	CProd& prod_available)
{
	// reduce groceries quantity by the amount of last quantity in vector 
	// (closest to expire)
	int rem;

	while(!prod_available.date_quantity.empty())
	{
		rem = prod_needed.second - prod_available.date_quantity.begin()->second;
		if(rem >= 0)
		{	
			prod_needed.second = rem;
			prod_available.date_quantity.erase(prod_available.date_quantity.begin());
			if(prod_needed.second == 0) return false;
			continue; // check next
		}
		prod_available.date_quantity.begin()->second = abs(rem);
		return false;
	}
	erase_name_variants(prod_needed.first);
	return true;
}

// a super-duper date hashing algorithm
size_t P_Date_Hash::operator()(const CDate& key) const
{
	return key.get_year() * 366
			 + key.get_month() * 31 
			 + key.get_day();
}
bool P_Date_Hash::operator()(const CDate& lhs, const CDate& rhs) const
{	return lhs == rhs; }
// a super-duper string hashing algorithm
size_t P_Name_Hash::operator()(const string& key) const
{
	size_t h_val_str = 1;

	for(const auto& iter : key)
		h_val_str *= (size_t)iter;
	return h_val_str *= key.length();
}
bool P_Name_Hash::operator()(const string& lhs, const string& rhs) const
{	return lhs == rhs; }
bool are_same_words(const string& lhs, const string& rhs)
{
	int missmatches = 0;
	if(lhs.length() != rhs.length()) return false;
	for(size_t i = 0; i < lhs.length(); i++)
	{
		if(missmatches > 1) return false;
		if(lhs[i] != rhs[i]) missmatches++;
	}
	return true;
}
void CSupermarket::hash_name_variants(const string& name)
{
	for(size_t i = 0; i < name.length(); ++i)
		{
			string name_modified(name);

			name_modified[i] = ' ';
			name_variant[name_modified] = name;
		}
}
void CSupermarket::erase_name_variants(const string& name)
{
	for(size_t i = 0; i < name.length(); i++)
	{
		string name_var(name);
		name_var[i] = ' ';
		name_variant.erase(name_var);
	}
	return;
}

int main()
{
	CSupermarket s;
  s . store ( "bread", CDate ( 2016, 4, 30 ), 100 )
    . store ( "butter", CDate ( 2016, 5, 10 ), 10 )
    . store ( "beer", CDate ( 2016, 8, 10 ), 50 )
    . store ( "bread", CDate ( 2016, 4, 25 ), 100 )
    . store ( "okey", CDate ( 2016, 7, 18 ), 5 );

	list<pair<string,int> > l0 = s . expired ( CDate ( 2018, 4, 30 ) );
  assert ( l0 . size () == 4 );
  assert ( ( l0 == list<pair<string,int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } } ) );

  list<pair<string,int> > l1 { { "bread", 2 }, { "Coke", 5 }, { "butter", 20 } };
  s . sell ( l1 );
  assert ( l1 . size () == 2 );
  assert ( ( l1 == list<pair<string,int> > { { "Coke", 5 }, { "butter", 10 } } ) );

  list<pair<string,int> > l2 = s . expired ( CDate ( 2016, 4, 30 ) );
  assert ( l2 . size () == 1 );
  assert ( ( l2 == list<pair<string,int> > { { "bread", 98 } } ) );

  list<pair<string,int> > l3 = s . expired ( CDate ( 2016, 5, 20 ) );
  assert ( l3 . size () == 1 );
  assert ( ( l3 == list<pair<string,int> > { { "bread", 198 } } ) );

  list<pair<string,int> > l4 { { "bread", 105 } };
  s . sell ( l4 );
  assert ( l4 . size () == 0 );
  assert ( ( l4 == list<pair<string,int> > {  } ) );

  list<pair<string,int> > l5 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l5 . size () == 3 );
  assert ( ( l5 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } } ) );

  s . store ( "Coke", CDate ( 2016, 12, 31 ), 10 );

  list<pair<string,int> > l6 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 }, { "Cokes", 1 } };
  s . sell ( l6 );
  assert ( l6 . size () == 3 );
  assert ( ( l6 == list<pair<string,int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } } ) );

  list<pair<string,int> > l7 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l7 . size () == 4 );
  assert ( ( l7 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } } ) );

  s . store ( "cake", CDate ( 2016, 11, 1 ), 5 );

  list<pair<string,int> > l8 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 } };
  s . sell ( l8 );
  assert ( l8 . size () == 2 );
  assert ( ( l8 == list<pair<string,int> > { { "Cake", 1 }, { "coke", 1 } } ) );

  list<pair<string,int> > l9 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l9 . size () == 5 );
  assert ( ( l9 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } } ) );

  list<pair<string,int> > l10 { { "cake", 15 }, { "Cake", 2 } };

	s . sell ( l10 );
	assert ( l10 . size () == 2 );
  assert ( ( l10 == list<pair<string,int> > { { "cake", 12 }, { "Cake", 2 } } ) );

  list<pair<string,int> > l11 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l11 . size () == 4 );
  assert ( ( l11 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } } ) );

  list<pair<string,int> > l12 { { "Cake", 4 } };
  s . sell ( l12 );
  assert ( l12 . size () == 0 );
  assert ( ( l12 == list<pair<string,int> > {  } ) );

  list<pair<string,int> > l13 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l13 . size () == 4 );
  assert ( ( l13 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } } ) );

  list<pair<string,int> > l14 { { "Beer", 20 }, { "Coke", 1 }, { "bear", 25 }, { "beer", 10 } };
  s . sell ( l14 );
  assert ( l14 . size () == 1 );
  assert ( ( l14 == list<pair<string,int> > { { "beer", 5 } } ) );

  s . store ( "ccccb", CDate ( 2019, 3, 11 ), 100 )
    . store ( "ccccd", CDate ( 2019, 6, 9 ), 100 )
    . store ( "dcccc", CDate ( 2019, 2, 14 ), 100 );

  list<pair<string,int> > l15 { { "ccccc", 10 } };
  s . sell ( l15 );
  assert ( l15 . size () == 1 );
  assert ( ( l15 == list<pair<string,int> > { { "ccccc", 10 } } ) );

  return EXIT_SUCCESS;
}