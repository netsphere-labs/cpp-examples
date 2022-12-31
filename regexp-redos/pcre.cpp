
#define PCRE2_CODE_UNIT_WIDTH 8  // 定義が必須
#include <pcre2.h>
#include <string.h>
#include <string>
using namespace std;

#define USE_DFA 1

void test(pcre2_code* re, const char* str)
{
  pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
#ifndef USE_DFA
  int rc = pcre2_match(re, (unsigned char*) str, strlen(str),
                   0,
                   0,
                   match_data,
                   NULL);
#else
  // これより小さいと Matching error -43 workspace size exceeded in DFA matching
  constexpr int DFA_WORKSPACE_COUNT = 100000;
  int dfa_workspace[DFA_WORKSPACE_COUNT];
  int rc = pcre2_dfa_match(re, (unsigned char*) str, strlen(str),
                           0,
                           0,
                           match_data,
                           NULL,
                           dfa_workspace,
                           DFA_WORKSPACE_COUNT);
#endif
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

  printf("Matched: %s\n", str);
  pcre2_match_data_free(match_data);
}

int main()
{
  int err_num;
  PCRE2_SIZE err_offset;
  pcre2_code* re = pcre2_compile((unsigned char*) "^(a+)+$", // pattern
                                 PCRE2_ZERO_TERMINATED,      // length
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

  test(re, string(1000, 'a').c_str() );
  test(re, (string(10, 'a') + "x").c_str() );    //=> No match.
  // 限界が低すぎる
  test(re, (string(100, 'a') + "x").c_str() );   //=> Matching error -47
  test(re, (string(1000, 'a') + "x").c_str() );  //       match limit exceeded
  test(re, (string(10000, 'a') + "x").c_str() ); // DFA でも反応が帰ってこない.
  
  pcre2_code_free(re);
  return 0;
}

  
