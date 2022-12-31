
// RE2 - $1, $2 ... に相当するもの submatch Extraction
  
#include <iostream>
#include <re2/re2.h>
#include <string>
#include <assert.h>
using namespace std;

int main()
{
  re2::RE2 re("(\\w+):(\\d+)"); // 括弧で区切る
  assert(re.ok() );

  re2::StringPiece input("ruby:1234");
  // それぞれの括弧の型に合わせて変数定義
  string w;
  int d;
  if (re2::RE2::PartialMatch(input, re, &w, &d) ) {
    cout << "w+ = " << w << "\n";
    cout << "d+ = " << d << "\n";
  }
  
  return 0;
}

