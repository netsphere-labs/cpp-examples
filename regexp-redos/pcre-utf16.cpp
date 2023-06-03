
// ICU と組み合わせる方法

#define PCRE2_CODE_UNIT_WIDTH 16  // 定義が必須
#include <unicode/unistr.h>
#include <pcre2.h>
#include <string.h>
#include <string>
using namespace std;
using namespace icu;


void test(pcre2_code* re, const UnicodeString& str)
{
  pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);

  int rc = pcre2_match(re, (PCRE2_SPTR16) str.getBuffer(),
                       str.length(), // length: code units, not chars.
                   0,
                   0,
                   match_data,
                   NULL);
  if (rc < 0) {
    switch (rc) {
    case PCRE2_ERROR_NOMATCH:
      printf("No match.\n"); break;
    default:
      printf("Matching error %d\n", rc); break;
    }
    pcre2_match_data_free(match_data);
    return;
  }

  string t;
  printf("Matched: %s\n", str.toUTF8String(t).c_str() );
  pcre2_match_data_free(match_data);
}


int main()
{
  int err_num;
  PCRE2_SIZE err_offset;
  static const UnicodeString re_str("^(a+)+$");
  pcre2_code* re = pcre2_compile((PCRE2_SPTR16) re_str.getBuffer(), // pattern
                                 re_str.length(),    // length
                                 0,
                                 &err_num,
                                 &err_offset,
                                 NULL);
  if (re == NULL) {
    PCRE2_UCHAR buf[256];
    pcre2_get_error_message(err_num, buf, sizeof(buf));
    printf("PCRE2 compilation failed at offset %ld: %s\n", err_offset, buf);
    return 1;
  }

  test(re, string(3, 'a').c_str() );
  test(re, string(1000, 'a').c_str() );
  test(re, (string(10, 'a') + "x").c_str() );    //=> No match.
  // 限界が低すぎる
  test(re, (string(100, 'a') + "x").c_str() );   //=> Matching error -47
  test(re, (string(1000, 'a') + "x").c_str() );  //       match limit exceeded
  test(re, (string(10000, 'a') + "x").c_str() ); // DFA でも反応が帰ってこない.
  
  pcre2_code_free(re);
  return 0;
}

  
