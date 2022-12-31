
// RE2  -> OK!

#include <stdio.h>
#include <re2/re2.h>
#include <string>
#include <assert.h>
using namespace std;

void test_regex(re2::RE2* re, const char* str)
{
  re2::StringPiece input(str);
  if (re2::RE2::PartialMatch(input, *re) )
    printf("Matched: %s\n", str);
  else
    printf("Not match.\n");
}

int main()
{
  re2::RE2 re("^(a+)+$");
  assert(re.ok());
  
  test_regex(&re, string(10, 'a').c_str());
  test_regex(&re, string(1000, 'a').c_str());
  test_regex(&re, (string(10000, 'a') + "x").c_str());
  test_regex(&re, (string(100000, 'a') + "x").c_str()); // 一瞬で完了.
  return 0;
}
