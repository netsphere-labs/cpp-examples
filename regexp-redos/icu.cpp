
// ICU
//   パフォーマンスもダメ, そもそも簡単な正規表現でもバグってる
// $ gcc -Wall -Wextra -lstdc++ -licui18n -licuuc icu.cpp

#include <string>
#include <assert.h>
#include <unicode/regex.h>
using namespace std;
using namespace icu;

void run_regex(RegexMatcher* re, const string& str)
{
  re->reset(str.c_str() );
  UErrorCode status = U_ZERO_ERROR;
  bool r = re->find(status);
  assert(!status);
  if (r)
    printf("matched: %s\n", str.c_str());
  else
    printf("not matched.\n");
}

int main()
{
  UErrorCode status = U_ZERO_ERROR;
  RegexMatcher re("(a+)+$",  // ^ は改行の直後にもマッチしない! 文字列の最初にもマッチしない! Bug
                  UREGEX_MULTILINE, // flags
                  status);
  assert(!status);
  
  run_regex(&re, string(1000, 'a') );
  run_regex(&re, string(10000, 'a') ); // 長くするとマッチも帰ってこなくなる。
  run_regex(&re, string(1000, 'a') + "x" );  // すぐ not matched. OK
  //run_regex(&re, string(10000, 'a') + "x" ); // 帰ってこなくなる

  return 0;
}
