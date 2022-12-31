
// Boost.Regex

#include <stdio.h>
#include <string>
#include <boost/regex/icu.hpp>
using namespace std;
//using namespace boost;

void test_regex(boost::u32regex* re, const char* str)
{
  boost::smatch m;
  if ( boost::u32regex_search(str, m, *re) ) 
    printf("Matched: %s\n", str );
  else
    printf("Not match.\n");
}

int main()
{
  boost::u32regex re = boost::make_u32regex("^(a+)+$");
  
  test_regex(&re, string(1000, 'a').c_str());
/*
terminate called after throwing an instance of 'boost::wrapexcept<boost::regex_error>'
  what():  The complexity of matching the regular expression exceeded predefined bounds.  Try refactoring the regular expression to make each choice made by the state machine unambiguous.  This exception is thrown to prevent "eternal" matches that take an indefinite period time to locate.
中止 (コアダンプ)
*/    
  test_regex(&re, (string(100, 'a') + "x").c_str()); // 例外発生!
  test_regex(&re, (string(1000, 'a') + "x").c_str());
  return 0;
}

