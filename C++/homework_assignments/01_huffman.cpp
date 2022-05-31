#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

/* INFO: decompression is incomplete (bonus task), class definitions are somewhat 
 * sloppy (first usage of classes in a program).
 *
 * An implementation of Huffman's compression algorithm. */

const int LAST_CHARS_COUNT  = 12;
const int ENCODED_CHARS_MAX = 4096;

// need to restruct byte_arr, too many conversions when accessing individual bytes
struct Huffman_node
{
    int 					 byte_count;
    char *  			 byte_arr[4]{}; // TODO: consider switching to char8_t
    Huffman_node * left;
    Huffman_node * right;

    Huffman_node(){
      byte_count = 0;
      left       = nullptr;
      right      = nullptr;
      for(auto & i : byte_arr)
        i = nullptr;
    }

    ~Huffman_node(){
      for(int i = 0; i < byte_count; i++) // release bytes
      {
        delete[] byte_arr[i];
      }
    }

    bool read_char_bits(char *& byte_arr_next, stringstream & bit_stream){
      byte_arr_next = new char[8];
      for(int i = 0; i < CHAR_BIT; i++){
        if(!(bit_stream >> byte_arr_next[i])){ // stream error occured
          byte_count--;
          delete[] byte_arr_next;
          return false;
        }
      }
      return true;
    }

    bool max_uchar(){
      int res;

      if((res = (int)(unsigned char)convert_byte_to_char(0)) < 244) return false;
      else if(res > 244) return true;
      else if(res == 244){
        if((res = (int)(unsigned char)convert_byte_to_char(1)) < 143 ) return false;
        else if(res > 143) return true;
        else if(res == 143){
          if((res = (int)(unsigned char)convert_byte_to_char(2)) < 191) return false;
          else if(res > 191) return true;
          else if(res == 191){
            if((res = (int)(unsigned char)convert_byte_to_char(3)) <= 191) return false;
            else if(res > 191) return true;
         }
        }
      }
      return false;
    }

    // TODO: need to add UTF-8 max value
    bool extract_bytes(stringstream & bit_stream){
      // read byte 1
      if(!read_char_bits(byte_arr[byte_count++], bit_stream)) return false;

      // all good, ASCII character read, quit
      if(byte_arr[0][0] == '0') return true;

      // read UTF-8 characters from here
      // read byte 2
      if(!read_char_bits(byte_arr[byte_count++], bit_stream)) return false;
      // wrong format, byte 1 or 2
      if(byte_arr[0][1] == '0'
         || byte_arr[1][0] == '0'
         || byte_arr[1][1] == '1') return false;
      // all good, 2 bytes read
      if(byte_arr[0][2] == '0') return true;

      // read byte 3
      if(!read_char_bits(byte_arr[byte_count++], bit_stream)) return false;
      // wrong format, byte 3
      if(byte_arr[2][0] == '0'
         || byte_arr[2][1] == '1') return false;
      // all good, 3 bytes read
      if(byte_arr[0][3] == '0') return true;

      // read byte 4
      if(!read_char_bits(byte_arr[byte_count++], bit_stream)) return false;
      // wrong format, byte 1 or 4
      if(byte_arr[0][4] == '1'
         || byte_arr[3][0] == '0'
         || byte_arr[3][1] == '1') return false;
      // all good, 4 bytes read

      if(max_uchar()) return false;
      return true;
    } // this was fun btw

    char convert_byte_to_char(int byte_offset){
      char res = 0;
      int  power_of_two;

      for(int i = 0; i < CHAR_BIT; i++)
      {

        power_of_two = 1;
        if(byte_arr[byte_offset][i] == '0')
          continue;

        for(int j = 0; j < CHAR_BIT - 1 - i; j++)
          power_of_two *= 2;

        res += power_of_two;
      }
      return res;
    }

    void print_node(Huffman_node * node, const char * outname){
      if(node){
        print_node(node->left, outname);
        print_node(node->right, outname);

        for(int i = 0; i < node->byte_count; i++)
        {
          ofstream out_file(outname);
          out_file << node->convert_byte_to_char(i);
        }
      }
    }
};

class Huffman_file;

// questionable class, as all private members are accessed
class Huffman_tree {
public:
    Huffman_tree(){
      root = nullptr;
    }

    ~Huffman_tree(){
      if(root) delete_tree_recursively(root);
    }

    Huffman_node * add_nodes(stringstream & bit_stream){
      Huffman_node * new_node;
      char           next_bit;

      if(!bit_stream)
        return nullptr; // no more bits or some other error

      bit_stream.get(next_bit);

      if(!bit_stream.good()) return nullptr;

      new_node = new Huffman_node;
      if(next_bit == '1') // bit == 1
      {
        if(!new_node->extract_bytes(bit_stream)) {
          delete new_node;
          return nullptr; // try to get encoded bytes
        }
        return new_node;
      }
      // bit == 0

      new_node->left  = add_nodes(bit_stream);
      if(new_node->left == nullptr)
      {
        if(new_node->right) delete_tree_recursively(new_node->right);
        delete new_node;
        return nullptr;
      } 
      
      new_node->right = add_nodes(bit_stream);
      if(new_node->right == nullptr)
      {
        if(new_node->left) delete_tree_recursively(new_node->left);
        delete new_node;
        return nullptr;
      } 
      return new_node;
    }

    // called from destructor
    void delete_tree_recursively(Huffman_node * node){
      if(node){
        delete_tree_recursively(node->left);
        delete_tree_recursively(node->right);

        delete node;
        node = nullptr; // may be a safe practice, idk
      }
    }

    // TODO: remove (debug)
    void print_tree(Huffman_node * node){
      if(node){
        print_tree(node->left);
        print_tree(node->right);
        if(node->byte_count)
          cout << "next node: " << node->convert_byte_to_char(0) << endl;
      }
    }

    Huffman_node * get_tree_root(){ return root; }
    bool check_minimum_nodes(){
      return ((root->left != nullptr) && (root->right != nullptr));
    }
private:
    friend         Huffman_file;
    Huffman_node * root;
};

class Huffman_file {
public:
    Huffman_file(){}
    ~Huffman_file(){}

    bool parse_tree(stringstream & bit_stream){
      h_tree.root = h_tree.add_nodes(bit_stream);
      if(h_tree.root == nullptr) return false;
      if(!h_tree.check_minimum_nodes()) return false;
      return true;
    }

    // TODO: remove (debug)
    void print_bit_stream(fstream & bit_stream){ 
      string bit_stream_str;
      bit_stream >> bit_stream_str;
      cout << bit_stream_str;
    }

    Huffman_node * get_encoded_char(Huffman_node * node, stringstream & bit_stream){
      // node should not be nullptr, so this condition might be redundant
      if(!node) return nullptr;

      // byte(s) is(are) stored in this node, return this node
      if(node->byte_count > 0)
        return node;

      char next_bit;
      if(!(bit_stream >> next_bit)) return nullptr;

      // else recursively go to an appropriate child node
      return (next_bit == '0') ? get_encoded_char(node->left, bit_stream) : get_encoded_char(node->right, bit_stream);
    }

    int get_encoded_char_count(stringstream & bit_stream){
      int  len = 0;
      int  power_of_two;
      char next_bit;

      for(int i = 0; i < LAST_CHARS_COUNT; i++){
        if(!(bit_stream >> next_bit))
          return false; // no more bits or other stream error

        if(next_bit == '0')
          continue;

        power_of_two = 1;
        for(int j = 0; j < LAST_CHARS_COUNT - 1 - i; j++)
          power_of_two *= 2;

        len += power_of_two;
      }
      return len;
    }

    bool extract_chunk_to_output_file(ofstream& out_file, bool is_last_chunk, stringstream & bit_stream, int & decoded_char_count){
      Huffman_node * encoded_char = nullptr;
      char           decoded_char;
      int            encoded_char_count;

      //=============== NOT A LAST CHUNK =============================
      if(!is_last_chunk){
        decoded_char_count = 0;
        do {
          // recursively find a Huffman-encoded char in a tree
          encoded_char = get_encoded_char(h_tree.get_tree_root(), bit_stream);
          if(!encoded_char) return false;

          // decode up to 4 bytes and put them in output file
          for(int i = 0; i < encoded_char->byte_count; i++){
            decoded_char = encoded_char->convert_byte_to_char(i);
            out_file << decoded_char;
            if(!out_file) return false;
          }
          decoded_char_count++;
        } while(decoded_char_count < ENCODED_CHARS_MAX
                && bit_stream); // TODO: this should be correct
        // chunk was incomplete
        return decoded_char_count == ENCODED_CHARS_MAX;
      }
      //=================== LAST CHUNK ================================
      encoded_char_count = get_encoded_char_count(bit_stream);
      // previous chunk had 4096 encoded chars, this (last) chunk is empty
      if(encoded_char_count == 0 && decoded_char_count == 4096)
        return true;

      // reset counter
      decoded_char_count = 0;
      for(int i = 0; i < encoded_char_count; i++){
        // recursively find a Huffman-encoded char in a tree
        encoded_char = get_encoded_char(h_tree.get_tree_root(), bit_stream);
        if(!encoded_char) return false;

        // decode up to 4 bytes and put them in output file
        for(int j = 0; j < encoded_char->byte_count; j++){
          decoded_char = encoded_char->convert_byte_to_char(j);
          out_file << decoded_char;
            if(!out_file) return false;
        }
        decoded_char_count++;
      }
      // mismatch in expected bytes to decode
      return encoded_char_count == decoded_char_count;
    }

    bool check_bits_padding(stringstream & bit_stream){
      char padding_bit, last_bit;

      bit_stream >> padding_bit;
      if(bit_stream.eof()) return true; // only one bit was left

      while(bit_stream >> last_bit)
        if(last_bit != padding_bit || !bit_stream) return false; // padding bits mismatch
      return true;
    }

    bool huf_to_orig(ofstream& out_file, stringstream & bit_stream){
      char chunk_type;
      int  decoded_char_count = 0;

      while(bit_stream.get(chunk_type)){
        // 1: get and decode next 4096 bits, send them to output file
        if(chunk_type == '1'){
          if(!extract_chunk_to_output_file(out_file, false, bit_stream, decoded_char_count))
            return false;
          continue;
        }


        // 0: process last chunk
        if(!extract_chunk_to_output_file(out_file, true, bit_stream, decoded_char_count))
          return false;
        break;
      }
      if(chunk_type == '1') // last chunk (0) absent
        return false;

      // padding bits should either all be 1s or 0s,
      // check this if there's something left in stream
      if(!bit_stream.eof()
         && !check_bits_padding(bit_stream))
        return false;
      return true;
    }
private:
    Huffman_tree h_tree;
};

// try to convert bytes from a binary file to a stream of bits,
// which will get stored and parsed later
bool convert_file_to_bit_stream(ifstream& in_file, stringstream & bit_stream){
  char byte_c;
  int  byte_i;
  int  bits_reversed[8];

  while(in_file.get(byte_c))
  {
    if(!in_file.good()) return false;

    byte_i = (int)(unsigned char)byte_c;

    // reset all bits to 0
    for(int & i : bits_reversed)
      i = 0;

    // get binary form of byte
    for(int & i : bits_reversed)
    {
      i = byte_i % 2;
      byte_i /= 2;
    }

    // reverse bits array
    int tmp ;
    for(int i = 0; i < CHAR_BIT/2; i++)
    {
      tmp = bits_reversed[i];
      bits_reversed[i] = bits_reversed[CHAR_BIT - 1 - i];
      bits_reversed[CHAR_BIT - 1 - i] = tmp;
    }
    for(int i : bits_reversed)
      bit_stream << i;
  }
  if(!in_file.eof()) return false; // something left in input stream
  return true;
}

// check status of input file stream
bool in_file_valid(ifstream& ifs)
{
  return ((ifs.peek() != ifstream::traits_type::eof()) // not empty 
          && ifs.good()                                // could not open
          && ifs.is_open());                           // did not open
}

// check if file already exists
bool file_exists(const string& file_name) {
    ifstream f(file_name.c_str());
    return f.good();
}

// decompress a compressed file
bool decompressFile ( const char * inFileName, const char * outFileName )
{
  // input file with encoded characters in bytes
  ifstream in_file_stream(inFileName, ios::binary);

  // check if file is valid
  if(!in_file_valid(in_file_stream))
    return false;

  // temp storage for converted aforementioned bytes to bits
  stringstream bit_stream;
  // output file for decoded bits converted to chars
  ofstream out_file_stream;

  // convert bytes from a binary file to a bitstream
  // if unsuccessful, remove temp bit_stream
  if(!convert_file_to_bit_stream(in_file_stream, bit_stream))
    return false;


  // try to get Huffman tree
  Huffman_file h_file;

  if(!h_file.parse_tree(bit_stream))
    return false;

  // decompress file, put chars in output file
  out_file_stream.open(outFileName, ios::out);
  if(!out_file_stream)
    return false;

  // try to decompress huf to orig
  if(!h_file.huf_to_orig(out_file_stream, bit_stream))
    return false;

  return true;
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  // keep this dummy implementation (no bonus) or implement the compression (bonus)
  return false;
}

void close_files(ifstream & file1, ifstream & file2)
{
  file1.close();
  file2.close();
}

bool is_valid_file(ifstream& fs){ return fs.good(); }

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  string   file_line1, file_line2;
  ifstream file_stream1(fileName1), file_stream2(fileName2);

  if(!is_valid_file(file_stream1) || !is_valid_file(file_stream2))
    return false;

  do
  {
    getline(file_stream1, file_line1);
    getline(file_stream2, file_line2);

    // check if lines are identical
    if(file_line1 != file_line2)
      return false;
  } while (!file_stream1.eof() && !file_stream2.eof());

  // check if anything is left in the streams
  if(!file_stream1.eof() || !file_stream2.eof()){
    close_files(file_stream1, file_stream2);
    return false;
  }
  close_files(file_stream1, file_stream2);
  return true;
}

int main ( void )
{

  
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

    assert ( ! decompressFile ( "tests/in_4581074.bin", "tempfile" ) );
  // assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
