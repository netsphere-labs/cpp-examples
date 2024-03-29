
/*
ICU と組み合わせる. 
  - PCRE2_CODE_UNIT_WIDTH を 16 に定義する。UTF-16

毎回 pcre2_match_data_free() しなくてもよい.
  - pcre2_match_data_create(ovecsize, gcontext) で pcre2_match_data を生成
  - 成功した場合と、最後だけ解放すればよい。

C++ wrapper
  - (ヘッダのみ) https://github.com/jpcre2/jpcre2/
    -> 使い方 https://stackoverflow.com/questions/32580066/using-pcre2-in-a-c-project
  - POCO も PCRE2 ラッパ。
  - Qt6 QRegularExpression もラッパ。
*/

#define PCRE2_CODE_UNIT_WIDTH 16  // 定義が必須
#include <unicode/unistr.h>
#include <pcre2.h>
#include <string.h>
#include <string>
#include <assert.h>
using namespace std;
using namespace icu;


int test( const pcre2_code* re, const UnicodeString& str,
          pcre2_match_data* matched )
{
  assert(re);
  assert( matched );
  
  int rc = pcre2_match(re,  // code
                       (PCRE2_SPTR16) str.getBuffer(),  // subject
                       str.length(), // length: code units, not chars.
                       0,            // startoffset
                       0,            // options
                       matched,      // match_data  NULL 不可
                       NULL);        // mcontext
  if (rc < 0) {
    switch (rc) {
    case PCRE2_ERROR_NOMATCH:
      printf("No match.\n"); break;
    default:
      printf("Matching error %d\n", rc); break;
    }
    //pcre2_match_data_free( matched ); matched = NULL;
    return rc;
  }
  // rc = 0: 成功したが, the vector of offsets が小さすぎる
  
  // Match succeeded. Get a pointer to the output vector.
  // pcre2_match_data 構造体は opaque. 直接は使えない
  PCRE2_SIZE* ovec = pcre2_get_ovector_pointer(matched);
  printf("Match succeeded at offset %ld\n", ovec[0]);

  // Show substrings stored in the output vector.
  // 0: がマッチした全体
  for (int i = 0; i < rc; i++) {
    // ペアが配列の交互になっいる。
    PCRE2_SIZE substring_length = ovec[2 * i + 1] - ovec[2 * i];
    UnicodeString substring(str.getBuffer() + ovec[2 * i], substring_length);
    string t;
    printf("%2d: %s\n", i, substring.toUTF8String(t).c_str() );
  }

  string t;
  printf("All text = %s\n", str.toUTF8String(t).c_str() );
  return rc;
}


const UnicodeString re_str("^(a+)+(b+)$"); // Not safe pattern

int main()
{
  int err_num;
  PCRE2_SIZE err_offset;
  pcre2_code* re = pcre2_compile((PCRE2_SPTR16) re_str.getBuffer(), // pattern
                                 re_str.length(),    // length
                                 0,                  // options
                                 &err_num,           // errorcode
                                 &err_offset,        // erroroffset
                                 NULL);              // ccontext
  if (re == NULL) {
    PCRE2_UCHAR buf[256];
    pcre2_get_error_message(err_num, buf, sizeof(buf));
    string t;
    printf("PCRE2 compilation failed at offset %ld: %s\n", err_offset,
           UnicodeString(buf).toUTF8String(t).c_str() );
    return 1;
  }

  //pcre2_match_data* matched = pcre2_match_data_create_from_pattern(re, NULL);
  pcre2_match_data* matched = pcre2_match_data_create(128, NULL);
  int rc = test(re, (string(3, 'a') + "bbb").c_str(), matched );
  assert(rc >= 0);
  pcre2_match_data_free(matched);
  
  matched = pcre2_match_data_create(128, NULL);
  rc = test(re, (string(1000, 'a') + string(1000, 'b')).c_str(), matched );
  assert(rc >= 0);
  pcre2_match_data_free(matched);
  
  matched = pcre2_match_data_create(128, NULL);
  rc = test(re, (string(10, 'a') + "x").c_str(), matched );    //=> No match.
  assert(rc < 0);
  // 解放不要
  
  // 限界が低い. OK
  // Matching error -47 (match limit exceeded)
  rc = test(re, (string(100, 'a') + string(100, 'b') + "x").c_str(), matched );
  assert( rc < 0 );
  
  rc = test(re, (string(1000, 'a') + string(1000, 'b') + "x").c_str(), matched );
  assert( rc < 0 );
  
  rc = test(re, (string(10000, 'a') + "x").c_str(), matched ); 
  assert( rc < 0 );
  
  pcre2_code_free(re);
  pcre2_match_data_free(matched);
  return 0;
}

  
