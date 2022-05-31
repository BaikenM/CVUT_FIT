#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
using namespace std;
#endif /* __PROGTEST__ */

const int    INIT_FILE_SIZE = 10;
const double FILE_INCREASE  = 1.5;

class CFile;

struct Data_storage
{
  int            ref_count;  // CFile instances referencing this storage
  uint32_t       file_size;  // allocated storage size
  uint32_t       byte_count; // number of bytes written to storage
  uint32_t       file_pos;   // last saved file position      
  uint8_t      * data_bytes; // array of bytes
  Data_storage * prev;       // previous versions, linked list

  Data_storage()
  : ref_count(1),
    file_size(INIT_FILE_SIZE),
    byte_count(0),
    file_pos(0),
    prev(nullptr)
  {
    data_bytes = new uint8_t[file_size];
  }
  
  Data_storage(Data_storage& rhs)
  : ref_count(1),
    file_size(rhs.file_size),
    byte_count(rhs.byte_count),
    file_pos(rhs.file_pos),
    prev(nullptr)
  {
    // copy storage contents
    data_bytes = new uint8_t[file_size];
    for(uint32_t i = 0; i < byte_count; i++) 
      data_bytes[i] = rhs.data_bytes[i];
    
    // if(prev) prev->ref_count++;
    rhs.ref_count--;
  }

  ~Data_storage()
  {
    delete[] data_bytes;
    prev = nullptr;
  }
};

class CFile
{
  public:
                             CFile                         ( void );                     
                             CFile                         ( CFile& rhs );
                             CFile&               operator=( CFile& rhs );
                            ~CFile();
    bool                     seek                          ( uint32_t          offset );
    uint32_t                 read                          ( uint8_t         * dst,
                                                             uint32_t          bytes );
    uint32_t                 write                         ( const uint8_t   * src,
                                                             uint32_t          bytes );
    void                     truncate                      ( void );
    uint32_t                 fileSize                      ( void ) const;
    void                     addVersion                    ( void );
    bool                     undoVersion                   ( void );
  
    // ADDITIONAL METHODS

    uint32_t                 overwrite_file(const uint8_t * src,
                                            uint32_t        bytes);
    uint32_t                 update_storage(const uint8_t * src,
                                            uint32_t        bytes);
    void                     resize_data_storage();
    void                     detach_storage();
    // TODO: REMOVE (DEBUG)
    void print_storage() const;
    void print_vers_hstr(Data_storage * curr) const;
    void print_info() const;
    Data_storage * get_st() const { return prev_vers; }
  private:
    Data_storage * stable_vers; // byte storage info
    Data_storage * prev_vers;   // version history represented by a linked list
};

// Default ctor
CFile::CFile()
: prev_vers(nullptr)
{
  // allocation should take place in order to init vars inside
  // data_st to avoid errors upon calling CFile methods
  stable_vers   = new Data_storage;     
}

// add references to shared data
CFile::CFile(CFile& rhs)
{
  stable_vers = rhs.stable_vers;
  prev_vers   = rhs.prev_vers;  

  stable_vers->ref_count++;
  prev_vers->ref_count++; 
}

CFile::~CFile()
{
  // if storage was allocated before and no more references to storage left -> delete storage
  if(--stable_vers->ref_count == 0
  || (stable_vers == prev_vers && stable_vers->ref_count == 1))
  {
    // cout << "deleting storage" << endl;

    delete stable_vers;
  }

  // if previous version was allocated before and no more references to previous version left -> delete previous version
  Data_storage * tmp;

  // cout << "deleting prev 1" << endl;

  while(prev_vers)
  {
    // cout << "deleting prev 2" << endl;

    tmp = prev_vers->prev;
    
    if(--prev_vers->ref_count == 0
    || (prev_vers->ref_count == 1 && prev_vers == stable_vers))
    {
      delete prev_vers;
    }

    prev_vers = tmp;
    // cout << "deleted prev 2" << endl;
  }
}

uint32_t CFile::overwrite_file(const uint8_t * src,
                               uint32_t        bytes)
{
  uint32_t src_pos = 0;

  // overwrite bytes one by one until end of file is reached
  // or byte count to overwrite is reached
  while(stable_vers->file_pos < stable_vers->byte_count && bytes-- > 0)
    stable_vers->data_bytes[stable_vers->file_pos++] = src[src_pos++]; 

  return src_pos;
}
// increase storage by 1.5
void CFile::resize_data_storage()
{
  stable_vers->file_size *= FILE_INCREASE;
  uint8_t * enlarged_storage = new uint8_t[stable_vers->file_size];

  for(uint32_t i = 0; i < stable_vers->byte_count; i++)
    enlarged_storage[i] = stable_vers->data_bytes[i];

  delete[] stable_vers->data_bytes;
  stable_vers->data_bytes = enlarged_storage;
}

CFile& CFile::operator=(CFile& rhs)
{
  if(this != &rhs) // exclude self assignment
  {
    // set pointers to shared data
    stable_vers = rhs.stable_vers;   
    prev_vers   = rhs.prev_vers;
    
    // increment reference count
    stable_vers->ref_count++;
    if(prev_vers) prev_vers->ref_count++;
  }
  return *this;  
}

bool CFile::seek(uint32_t offset)
{
  if(offset < 0 || offset > stable_vers->byte_count)
    return false;

  // create storage copy before modifying it
  if(stable_vers->ref_count != 1) 
  // || (stable_vers->ref_count == 2 && stable_vers != prev_vers))
    detach_storage();

  stable_vers->file_pos = offset;
  return true;
}

uint32_t CFile::read(uint8_t * dst,
                     uint32_t bytes)
{
  uint32_t bytes_read = 0;

  while(bytes-- > 0 && stable_vers->file_pos < stable_vers->byte_count)
    dst[bytes_read++] = stable_vers->data_bytes[stable_vers->file_pos++];
  return bytes_read;
}

uint32_t CFile::update_storage(const uint8_t * src,
                               uint32_t        bytes)
{
  int      src_pos = 0;
  
  // ALLOCATE more memory if needed
  if(bytes + stable_vers->byte_count > stable_vers->file_size)
  {
    resize_data_storage();  
  }
  // OVERWRITE bytes if pos is not at file end
  if(stable_vers->file_pos != stable_vers->byte_count)
  {
    return overwrite_file(src, bytes);
  }

  // APPEND bytes
  uint32_t bytes_init = stable_vers->byte_count; // save byte count for later
  while(bytes-- > 0)
  {
    stable_vers->data_bytes[stable_vers->file_pos++] = src[src_pos++];
    stable_vers->byte_count++;
  }
  return stable_vers->byte_count - bytes_init;
}

uint32_t CFile::write(const uint8_t * src,
                      uint32_t        bytes)
{
  if(stable_vers->ref_count != 1) // create storage copy if it has more than one reference to it 
    detach_storage();

  return update_storage(src, bytes);    
}

void CFile::truncate()
{
  if(stable_vers->ref_count != 1)
    detach_storage();
  
  uint8_t * reduced_storage = new uint8_t[stable_vers->file_pos];
  
  for(uint32_t i = 0; i < stable_vers->file_pos; i++)
    reduced_storage[i] = stable_vers->data_bytes[i];
  
  delete[] stable_vers->data_bytes;
  stable_vers->ref_count = 1;
  stable_vers->data_bytes = reduced_storage;
  stable_vers->byte_count = stable_vers->file_pos;
}

uint32_t CFile::fileSize(void) const { return stable_vers->byte_count; }

void CFile::addVersion(void)
{
  Data_storage * tmp = prev_vers;
  
  prev_vers = stable_vers;
  prev_vers->ref_count++;
  prev_vers->prev = tmp;
}

bool CFile::undoVersion()
{
  if(!prev_vers) return false;

  if(--stable_vers->ref_count == 0)
  {
    // cout << "deleting: undo" << endl;
    stable_vers->prev = nullptr;
    delete stable_vers;
  }
  stable_vers = prev_vers;
  prev_vers = stable_vers->prev;

  // cout << "hello there" << endl;

  if(prev_vers)
  {
    prev_vers->ref_count++;
  }
  return true;
}

void CFile::detach_storage()
{
  stable_vers = new Data_storage(*stable_vers);
}

void CFile::print_storage() const
{
  cout << "printing storage..." << endl;
  cout << "pos: " << stable_vers->file_pos << endl;
  cout << "ref: " << stable_vers->ref_count << endl;
  cout << "bytes: " << stable_vers->byte_count << endl;
  for(uint32_t i = 0; i < stable_vers->byte_count; i++)
    cout << i + 1 << ". " << (int)stable_vers->data_bytes[i] << " ";
}
void CFile::print_vers_hstr(Data_storage * curr) const
{
  if(!curr) return;
  cout << "printing next version..." << endl;
  cout << "pos: " << curr->file_pos << endl;
  cout << "ref: " << curr->ref_count << endl;
  cout << "bytes: " << curr->byte_count << endl;
  for(uint32_t i = 0; i < curr->byte_count; i++)
    cout << i + 1 << ". " << (int)curr->data_bytes[i] << " ";
  cout << "version printed!" << endl;
  print_vers_hstr(curr->prev);
}

void CFile::print_info() const
{
  print_storage();
  cout << endl;
  print_vers_hstr(prev_vers);
  cout << endl << endl;
}

#ifndef __PROGTEST__
bool               writeTest                               ( CFile           & x,
                                                             const initializer_list<uint8_t> & data,
                                                             uint32_t          wrLen )
{
  return x . write ( data . begin (), data . size () ) == wrLen;
}

bool               readTest                                ( CFile           & x,
                                                             const initializer_list<uint8_t> & data,
                                                             uint32_t          rdLen )
{
  uint8_t  tmp[100];
  uint32_t idx = 0;

  if ( x . read ( tmp, rdLen ) != data . size ())
    return false;
  for ( auto v : data )
    if ( tmp[idx++] != v )
      return false;
  return true;
}

int main ( void )
{
  CFile f0;

  assert ( writeTest ( f0, { 10, 20, 30 }, 3 ) );
  assert ( f0 . fileSize () == 3 );
  assert ( writeTest ( f0, { 60, 70, 80 }, 3 ) );
  assert ( f0 . fileSize () == 6 );
  assert ( f0 . seek ( 2 ));
  assert ( writeTest ( f0, { 5, 4 }, 2 ) );
  assert ( f0 . fileSize () == 6 );
  assert ( f0 . seek ( 1 ));
  assert ( readTest ( f0, { 20, 5, 4, 70, 80 }, 7 ));
  assert ( f0 . seek ( 3 ));
  f0 . addVersion();
  assert ( f0 . seek ( 6 ));
  assert ( writeTest ( f0, { 100, 101, 102, 103 }, 4 ) );
  f0 . addVersion();
  assert ( f0 . seek ( 5 ));
  CFile f1 ( f0 );
  f0 . truncate ();
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70 }, 20 ));
  assert ( f0 . undoVersion () );
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80, 100, 101, 102, 103 }, 20 ));
  assert ( f0 . undoVersion () );
  assert ( f0 . seek ( 0 ));
  assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80 }, 20 ));
  assert ( !f0 . seek ( 100 ));
  assert ( writeTest ( f1, { 200, 210, 220 }, 3 ) );
  assert ( f1 . seek ( 0 ));
  assert ( readTest ( f1, { 10, 20, 5, 4, 70, 200, 210, 220, 102, 103 }, 20 ));
  
  // cout <<"==============" << endl;
  assert ( f1 . undoVersion () );
  // f0.print_info();
  // f1.print_info();
  // assert ( f1 . undoVersion () );
  
  
  // assert ( readTest ( f1, { 4, 70, 80 }, 20 ));
  // assert ( !f1 . undoVersion () );

  // f0.print_storage();
  // f0.print_vers_hstr(f0.get_st());
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
