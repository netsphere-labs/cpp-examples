
// POSIX (glibc)
// => これは一瞬で完了する。問題ない。

#include <regex.h>
#include <stdio.h>
#include <string>
using namespace std;

void run_regex(regex_t* re, const char* str)
{
  int status = regexec(re, str, 0, NULL,
                       0); // eflags
  if (status == 0) 
    printf("matched: %s\n", str);
  else if (status == REG_NOMATCH)
    printf("not matched\n");
  else {
    fprintf(stderr, "regexec() failed.\n");
    return;
  }
}

int main()
{
  regex_t re;
  if (regcomp(&re, "^(a+)+$",
              REG_EXTENDED) != 0) { // REG_EXTENDED = 現代の. いつでも指定すること.
    fprintf(stderr, "pattern error.\n");
    return 1;
  }

  run_regex(&re, string(1000, 'a').c_str());
  run_regex(&re, (string(10000, 'a') + "x").c_str());
  run_regex(&re, (string(100000, 'a') + "x").c_str());  // すぐ not matched. OK
  
  regfree(&re);
  return 0;
}
