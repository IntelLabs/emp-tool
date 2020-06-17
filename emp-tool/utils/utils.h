#ifndef UTILS_H__
#define UTILS_H__
#include <string>
#include "emp-tool/utils/block.h"
#include <sstream>
#include <cstddef>//https://gcc.gnu.org/gcc-4.9/porting_to.html
#include <gmp.h>
#include "emp-tool/utils/prg.h"
#include <chrono>
#include <boost/format.hpp>
#include <iostream>
#include <bitset>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <ios>
#include <fstream>
#include <iomanip>

#define macro_xstr(a) macro_str(a)
#define macro_str(a) #a

#define RDTSC ({unsigned long long res; \
  unsigned hi, lo;   \
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi)); \
  res =  ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 ); \
  res;})
  
#define CHECK_EXPR(X) if((X)==false) { std::cout << (#X) << " failed" \
  << std::endl; exit(1); }
#define CHECK_EXPR_MSG(X, Y) if((X)==false) { std::cout << (#X) << " failed: \"" \
  << (Y) << "\"" << std::endl; exit(1); }

using std::string;
using std::chrono::time_point;
using std::chrono::high_resolution_clock;

namespace emp {
template<typename T>
void inline delete_array_null(T * ptr);

inline void error(const char * s, int line = 0, const char * file = nullptr);

template<class... Ts>
void run_function(void *function, const Ts&... args);

inline void parse_party_and_port(char ** arg, int argc, int * party, int * port);

std::string Party(int p);

// Timing related
inline time_point<high_resolution_clock> clock_start();
inline double time_from(const time_point<high_resolution_clock>& s);

//block conversions
template <typename T = uint64_t>
std::string m128i_to_string(const __m128i var);
block bool_to128(const bool * data);
void int64_to_bool(bool * data, uint64_t input, int length);

//Other conversions
template<typename T>
T bool_to_int(const bool * data, size_t len = 0);
std::string hex_to_binary(std::string hex_);
std::string binary_to_hex(std::string bin);
inline string change_base(string str, int old_base, int new_base);
inline string towsComplement(string num);
inline string dec_to_bin(const string& dec);
inline string dec_to_bin(int64_t dec, uint64_t bit_len);
inline string bin_to_dec(const string& bin2);
inline int64_t bin_to_dec(string bin, bool is_signed);
inline const char* hex_char_to_bin(char c);

void printBlock(block var);

//deprecate soon
void inline parse_party_and_port(char ** arg, int * party, int * port) {
	parse_party_and_port(arg, 2, party, port);
}

#include "emp-tool/utils/utils.hpp"
}
#endif// UTILS_H__
