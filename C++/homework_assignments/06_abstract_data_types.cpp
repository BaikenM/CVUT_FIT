#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

/* An implementation of an abstract class and its derived classes. 
 * An attempt to build an interface of an abstract class. */

/* abstract class, parent for 
 * CDataTypeInt, 
 * CDataTypeDouble, 
 * CDataTypeEnum, 
 * CDataTypeStruct
 * */
class CBaseType
{
	public:
		string fieldName;

		                              CBaseType() = default;
		                              CBaseType(string_view fieldName);
    virtual                       ~CBaseType() = default;
		
    virtual bool                  operator==(const CBaseType& rhs) const = 0;
    virtual bool                  operator!=(const CBaseType& rhs) const = 0;
    friend ostream&               operator<<(ostream& os, const CBaseType& rhs);

    virtual size_t                getSize() const = 0;
    virtual ostream&              printInfo(ostream& os, int offset = 0) const = 0;
  protected:
};

CBaseType::CBaseType(string_view fieldName)
: fieldName(fieldName) {}
ostream& operator<<(ostream& os, const CBaseType& rhs)
{
  return rhs.printInfo(os);
}

//=================================================================================
class CDataTypeInt : public CBaseType
{
	public:
		                      CDataTypeInt() = default;
		                      CDataTypeInt(const CDataTypeInt& rhs) = default;
		                      CDataTypeInt(const CDataTypeInt& rhs, string_view newName);
		                      CDataTypeInt(CDataTypeInt&& rhs);
		
    bool                  operator==(const CBaseType& rhs) const override;
    bool                  operator!=(const CBaseType& rhs) const override;
    friend ostream&       operator<<(ostream& os, const CBaseType& rhs);

    size_t                getSize() const override;
    ostream&              printInfo(ostream& os, int offset = 0) const override;
	private:
};

CDataTypeInt::       CDataTypeInt(const CDataTypeInt& rhs, string_view newName)
: CBaseType(newName){}
CDataTypeInt::       CDataTypeInt(CDataTypeInt&& rhs)
{
	fieldName = rhs.fieldName;
}
bool CDataTypeInt::  operator==(const CBaseType& rhs) const
{
  return typeid(rhs) == typeid(CDataTypeInt);
}
bool CDataTypeInt::  operator!=(const CBaseType& rhs) const
{
  return !(*this == rhs);
}
size_t CDataTypeInt::getSize() const
{
  return 4;
}
ostream& CDataTypeInt:: printInfo(ostream& os, int offset) const 
{
  return os << "int";
}

//=================================================================================
class CDataTypeDouble : public CBaseType
{
	public:
		                      CDataTypeDouble() = default;
		                      CDataTypeDouble(const CDataTypeDouble& rhs) = default;
		                      CDataTypeDouble(const CDataTypeDouble& rhs, string_view newName);
		                      CDataTypeDouble(CDataTypeDouble&& rhs);
		
    bool                  operator==(const CBaseType& rhs) const override;
    bool                  operator!=(const CBaseType& rhs) const override;
    friend ostream&       operator<<(ostream& os, const CBaseType& rhs);
    
    size_t                getSize() const override;
    ostream&              printInfo(ostream& os, int offset = 0) const  override;
	private:
};

CDataTypeDouble::         CDataTypeDouble(const CDataTypeDouble& rhs, string_view newName)
: CBaseType(newName) {}
CDataTypeDouble::         CDataTypeDouble(CDataTypeDouble&& rhs)
{
	fieldName = rhs.fieldName;
}
bool CDataTypeDouble::    operator==(const CBaseType& rhs) const
{
    return typeid(rhs) == typeid(CDataTypeDouble);
}
bool CDataTypeDouble::    operator!=(const CBaseType& rhs) const
{
  return !(*this == rhs);
}
size_t CDataTypeDouble::  getSize() const
{
  return 8;
}
ostream& CDataTypeDouble::printInfo(ostream& os, int offset) const  
{
  return os << "double";
}

//=================================================================================
class CDataTypeEnum : public CBaseType
{
	public:
		                      CDataTypeEnum() = default;
		                      CDataTypeEnum(const CDataTypeEnum& rhs);
		                      CDataTypeEnum(const CDataTypeEnum& rhs, string_view newName);
		                      CDataTypeEnum(CDataTypeEnum&& rhs);

    bool                  operator==(const CBaseType& rhs) const override;
    bool                  operator!=(const CBaseType& rhs) const override;
    friend ostream&       operator<<(ostream& os, const CBaseType& rhs);
    CDataTypeEnum&        operator=(const CDataTypeEnum& rhs);
    
    size_t                getSize() const override;
		CDataTypeEnum&&       add(string_view valName);
    int                   fieldExists(string_view fName) const;
    ostream&              printInfo(ostream& os, int offset = 0) const override;
		
		vector<string> m_Values;
	private:
};

CDataTypeEnum::                      CDataTypeEnum(const CDataTypeEnum& rhs)
: CBaseType(rhs.fieldName)
{
  m_Values = rhs.m_Values;
}
CDataTypeEnum::                      CDataTypeEnum(const CDataTypeEnum& rhs, string_view newName)
: CBaseType(newName),
  m_Values(rhs.m_Values) {}
CDataTypeEnum::                      CDataTypeEnum(CDataTypeEnum&& rhs)
: m_Values(move(rhs.m_Values))
{
  fieldName = rhs.fieldName;
}
bool CDataTypeEnum::                 operator==(const CBaseType& rhs) const
{
  // check for data types mismatch
  if(typeid(rhs) != typeid(CDataTypeEnum))
   return false;

  // check for enum storage size mismatch
  const CDataTypeEnum& local_ref = dynamic_cast<const CDataTypeEnum&>(rhs);
  if(local_ref.m_Values.size() != m_Values.size())
    return false;  
  
  // check individual fields
  for(size_t i = 0; i < m_Values.size(); ++i)
    if(m_Values[i] != local_ref.m_Values[i]) 
      return false;
  return true;
}
bool CDataTypeEnum::                 operator!=(const CBaseType& rhs) const
{
  return !(*this == rhs);
}
CDataTypeEnum& CDataTypeEnum::       operator=(const CDataTypeEnum& rhs)
{
  fieldName = rhs.fieldName;
  for(auto& it : rhs.m_Values)
    m_Values.push_back(it);
  return *this;
}
CDataTypeEnum&& CDataTypeEnum::      add(string_view valName)
{
  if(fieldExists(valName) != -1) throw invalid_argument("Duplicate enum value: " + string(valName));
	m_Values.push_back(string(valName));
	return move(*this);
}
size_t CDataTypeEnum::               getSize() const
{
  return 4;
}
int CDataTypeEnum::                  fieldExists(string_view fName) const
{
  size_t i;
  for(i = 0; i < m_Values.size(); ++i)
    if(m_Values[i] == fName) return i;
  return -1;
}
ostream& CDataTypeEnum::              printInfo(ostream& os, int offset) const 
{
  size_t vec_s = m_Values.size();

  os << string(offset, ' ') << "enum\n";
  os << string(offset, ' ') << "{\n";
  for(size_t i = 0; i < vec_s; ++i)
    if(i + 1 != vec_s) os << string(offset + 2, ' ') << m_Values[i] << ",\n";
    else               os << string(offset + 2, ' ') << m_Values[i] << "\n";
  os << string(offset, ' ') << "}";
  
  return os;
}

//=================================================================================
class CDataTypeStruct : public CBaseType
{
	public:
                          CDataTypeStruct() = default;
		                      CDataTypeStruct(const CDataTypeStruct& rhs);
                          CDataTypeStruct(CDataTypeStruct&& rhs) noexcept;

    bool                  operator==(const CBaseType& rhs) const override;
    bool                  operator!=(const CBaseType& rhs) const override;
    friend ostream&       operator<<(ostream& os, const CBaseType& rhs);
    
    size_t                getSize() const override;
		CDataTypeStruct&&     addField(string_view fieldName, CBaseType&& rhs); // dumb or very dumb?
		CDataTypeStruct&&     addField(string_view fieldName, CBaseType& rhs);  // dumb or very dumb?
    int                   fieldExists(string_view fName) const;
    CBaseType&            field(string_view name) const;
    ostream&              printInfo(ostream& os, int offset = 0) const override;
	private:
		vector<unique_ptr<CBaseType>> m_Fields;
};

CDataTypeStruct::                      CDataTypeStruct(const CDataTypeStruct& rhs)
{
  for(auto& it : rhs.m_Fields)
  {
    unique_ptr<CBaseType> nextField;
    if(typeid(*it) == typeid(CDataTypeInt))
    {
      CDataTypeInt& downcast_rhs = dynamic_cast<CDataTypeInt&>(*it);
      m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeInt(downcast_rhs))));
    }
    else if(typeid(*it) == typeid(CDataTypeDouble))
    {
      CDataTypeDouble& downcast_rhs = dynamic_cast<CDataTypeDouble&>(*it);
      m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeDouble(downcast_rhs))));
    }
    else if(typeid(*it) == typeid(CDataTypeEnum))
    {
      CDataTypeEnum& downcast_rhs = dynamic_cast<CDataTypeEnum&>(*it);
      m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeEnum(downcast_rhs))));
    }
  }
}
CDataTypeStruct::                      CDataTypeStruct(CDataTypeStruct&& rhs) noexcept
{
	// m_Fields.resize(rhs.m_Fields);
	move(rhs.m_Fields.begin(), rhs.m_Fields.end(), back_inserter(m_Fields));
}
bool CDataTypeStruct::                 operator==(const CBaseType& rhs) const
{
  // check for data types mismatch
  if(typeid(rhs) != typeid(CDataTypeStruct))
   return false;

  // check for enum storage size mismatch
  const CDataTypeStruct& local_ref = dynamic_cast<const CDataTypeStruct&>(rhs);
  if(local_ref.m_Fields.size() != m_Fields.size())
    return false;  
  
  // check individual fields
  for(size_t i = 0; i < m_Fields.size(); ++i)
    if(typeid(*m_Fields[i]) != typeid(*local_ref.m_Fields[i]) // field types are same
    || ((typeid(*m_Fields[i]) == typeid(CDataTypeEnum))       // both fields are of type enum  
       && (*local_ref.m_Fields[i] != *m_Fields[i])))          // those enums must have equal enum values
      return false;
  return true;
}
bool CDataTypeStruct::                 operator!=(const CBaseType& rhs) const
{
  return !(*this == rhs);
}
size_t CDataTypeStruct::               getSize() const
{
  size_t total = 0;

  for(auto& it : m_Fields)
    total += it->getSize(); // TODO: redo for 6.2? (nested struct)
  return total;
}
CDataTypeStruct&& CDataTypeStruct::    addField(string_view fieldName, CBaseType&& rhs)
{
  if(fieldExists(fieldName) != -1) throw invalid_argument("Duplicate field: " + string(fieldName));

	rhs.fieldName = fieldName;
	if(typeid(rhs) == typeid(CDataTypeInt))
  {
    CDataTypeInt& downcast_rhs = dynamic_cast<CDataTypeInt&>(rhs);
    m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeInt(move(downcast_rhs)))));
  }
	else if(typeid(rhs) == typeid(CDataTypeDouble))
  {
    CDataTypeDouble& downcast_rhs = dynamic_cast<CDataTypeDouble&>(rhs);
    m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeDouble(move(downcast_rhs)))));
  }
	else if(typeid(rhs) == typeid(CDataTypeEnum))
  {
    CDataTypeEnum& downcast_rhs = dynamic_cast<CDataTypeEnum&>(rhs);
		m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeEnum(move(downcast_rhs)))));
  }
	return move(*this);
}
CDataTypeStruct&&     CDataTypeStruct::addField(string_view fieldName, CBaseType& rhs)
{
  if(fieldExists(fieldName) != -1) throw invalid_argument("Duplicate field: " + string(fieldName));

  if(typeid(rhs) == typeid(CDataTypeInt))
  {
    CDataTypeInt& downcast_rhs = dynamic_cast<CDataTypeInt&>(rhs);
    m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeInt(downcast_rhs, fieldName))));
  }
	else if(typeid(rhs) == typeid(CDataTypeDouble))
  {
    CDataTypeDouble& downcast_rhs = dynamic_cast<CDataTypeDouble&>(rhs);
    m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeDouble(downcast_rhs, fieldName))));
  }
	else if(typeid(rhs) == typeid(CDataTypeEnum))
  {
    CDataTypeEnum& downcast_rhs = dynamic_cast<CDataTypeEnum&>(rhs);
		m_Fields.push_back(move(unique_ptr<CBaseType>(new CDataTypeEnum(downcast_rhs, fieldName))));
  }
  return move(*this);
}
CBaseType& CDataTypeStruct::           field(string_view name) const
{
  int pos = fieldExists(name);

  if(pos == -1) throw invalid_argument("Unknown field: " + string(name));
  
  if(typeid(*m_Fields[pos]) == typeid(CDataTypeInt))
  {
    CDataTypeInt& downcast_rhs = dynamic_cast<CDataTypeInt&>(*m_Fields[pos]);
    return downcast_rhs;
  }
	else if(typeid(*m_Fields[pos]) == typeid(CDataTypeDouble))
  {
    CDataTypeDouble& downcast_rhs = dynamic_cast<CDataTypeDouble&>(*m_Fields[pos]);
    return downcast_rhs;
  }
	// else if(typeid(*m_Fields[pos]) == typeid(CDataTypeEnum))
  CDataTypeEnum& downcast_rhs = dynamic_cast<CDataTypeEnum&>(*m_Fields[pos]);
  return downcast_rhs;
}
int CDataTypeStruct::                  fieldExists(string_view fName) const
{
  for(size_t i = 0; i < m_Fields.size(); ++i)
  {
    if(m_Fields[i]->fieldName == fName) return i;
  }
  return -1;
}
ostream& CDataTypeStruct::             printInfo(ostream& os, int offset) const 
{
  os << string(offset, ' ') << "struct\n";
  os << string(offset, ' ') << "{\n";
  for(auto& it : m_Fields)
  {
    if(typeid(*it) == typeid(CDataTypeInt) || typeid(*it) == typeid(CDataTypeDouble))
     os << string(offset + 2, ' ');
    it->printInfo(os, offset + 2); // send tab width
    os << " " << it->fieldName << ";\n";
  } 
  os << string(offset, ' ') << "}";
  if(offset) os << '\n';
  return os;
}

#ifndef __PROGTEST__
static bool        whitespaceMatch                         ( const string    & a,
                                                             const string    & b )
{
  cout << "=================== STRING A: ===================" << endl;
  cout << a << endl;
  cout << "=================== STRING B: ===================" << endl;
  cout << b << endl;
  return a == b;
}
template <typename T_>
static bool        whitespaceMatch                         ( const T_        & x,
                                                             const string    & ref )
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch ( oss . str (), ref );
}

int main()
{
  CDataTypeStruct  a = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct b = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "READY" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct c =  CDataTypeStruct () .
                        addField ( "m_First", CDataTypeInt () ) .
                        addField ( "m_Second", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () . 
                          add ( "NEW" ) . 
                          add ( "FIXED" ) . 
                          add ( "BROKEN" ) . 
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeInt () );
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "}") );

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a . field ( "m_Status" ) == CDataTypeEnum () . add ( "NEW" ) . add ( "FIXED" ) . add ( "BROKEN" ) . add ( "DEAD" ) );
  assert ( a . field ( "m_Status" ) != CDataTypeEnum () . add ( "NEW" ) . add ( "BROKEN" ) . add ( "FIXED" ) . add ( "DEAD" ) );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  CDataTypeStruct aOld = a;
  b . addField ( "m_Other", CDataTypeDouble ());

  a . addField ( "m_Sum", CDataTypeInt ());

  assert ( a != aOld );
  assert ( a != c );
  assert ( aOld == c );
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  int m_Sum;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  double m_Other;\n"
    "}") );

  c . addField ( "m_Another", a . field ( "m_Status" ));

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Another;\n"
    "}") );

  d . addField ( "m_Another", a . field ( "m_Ratio" ));

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "  double m_Another;\n"
    "}") );

  assert ( a . getSize () == 20 );
  assert ( b . getSize () == 24 );
  try
  {
    a . addField ( "m_Status", CDataTypeInt () );
    assert ( "addField: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate field: m_Status"sv );
  }

  try
  {
    cout << a . field ( "m_Fail" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Unknown field: m_Fail"sv );
  }

  try
  {
    CDataTypeEnum en;
    en . add ( "FIRST" ) .
         add ( "SECOND" ) .
         add ( "FIRST" );
    assert ( "add: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate enum value: FIRST"sv );
  }
  
  // own tests

  // cout << endl << "additional test..." << endl;

  // cout << CDataTypeEnum().add("m_Field1").add("m_Field2") << endl;
  
  // CDataTypeStruct new_str1 = CDataTypeStruct() .
  //       addField("m_Field1", CDataTypeInt()) .
  //       addField("m_Field2", CDataTypeDouble()) .
  //       addField("m_Field3", CDataTypeEnum().add("m_Enum1").add("m_Enum2") );

  // cout << new_str1 << endl << endl;

  // CDataTypeStruct new_str2 = CDataTypeStruct() .
  //       addField("m_Field1", CDataTypeInt()) .
  //       addField("m_Field2", CDataTypeDouble()) .
  //       addField("m_Field3", CDataTypeEnum().add("m_Enum1").add("m_Enum2") );

  // new_str2.addField("m_New_str1", new_str1);
  // cout << new_str1 << endl << endl;
  // cout << new_str2 << endl << endl;

  // cout << CDataTypeStruct(new_str2) << endl;

  // cout << CDataTypeInt() << endl;
  // cout << CDataTypeDouble() << endl;
  // cout << CDataTypeEnum() << endl;
  // cout << CDataTypeStruct() << endl;

  // CDataTypeInt intA;
  // CDataTypeInt intB(intA, "new_int");
  // cout << intB << endl;
  // CDataTypeInt intB = CDataTypeInt(new_str1.field("m_Field1"));
  // cout << intA.fieldName << endl;
  cout << a . field( "m_Length" );

}
#endif /* __PROGTEST__ */
