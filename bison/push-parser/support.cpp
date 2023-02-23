
#include <stdio.h>
#include "push-parse.tab.hh"
#include "cpp-lexer.h"
#include <iostream>
#include <stdarg.h>
#include "ptr_map.h"
#include "runtime.h"
using namespace std;


// <FlexLexer.h> class yyFlexLexer で yylex() が宣言されており, 使わなくても
// 定義が必要.
int CppBaseFlexLexer::yylex()
{
    abort(); // 呼び出されてはいけない.
}


int main( int argc, char* argv[] )
{
  MyCppLexer scanner(&cin);
  int status;
  YYLTYPE location { .first_line = 1, .first_column = 0, .last_line = 1, .last_column = 0 };
  
  yypstate *ps = yypstate_new();
  do {
    YYSTYPE value;
    yytokentype t = scanner.lex(&value, &location);
    status = yypush_parse(ps,         // yypstate *ps,
                          t,          // int pushed_char
                          &value,     // YYSTYPE const *pushed_val
                          &location,  // YYLTYPE *pushed_loc
                          &scanner);  // class MyCppLexer* scanner
  } while (status == YYPUSH_MORE);
  yypstate_delete(ps);  

  printf("yypush_parse() finish status = %d\n", status);
  return status;
}


////////////////////////////////////////////////////////////////////

ptr_map<string, Ident*> ident_map;

// @return 見つからなかった場合, NULL.
const Ident* find_ident(const string& name, const YYLTYPE& loc) {
  auto i = ident_map.find(name);
  if ( i == ident_map.end() )
    return nullptr; 
  return i->second;
}


// @return 新たに挿入したとき true. これが false の場合, 呼出側で name を解放
//         すること。
bool Ident::var_update(char* name, int val, const YYLTYPE& loc, int* error )
{
  auto i = ident_map.find(name);
  Ident* ident;
  if ( i != ident_map.end() ) {
    ident = i->second;
    if ( ident->type != 0) {
      *error = 1; return false; 
    }
    ident->u.value = val;
    return false;  // 呼出側で name を解放すること。
  }
  else {
    ident = new Ident(name, loc);
    ident->type = 0;
    ident_map.insert(make_pair(string(name), ident));
  }
  ident->u.value = val;

  assert( ident_map.at(name) );
  return true;
}

// @param name  成功した場合のみ, moveする
void Ident::cond_define(char* name, Checker* check, const YYLTYPE& loc,
                        int* error)
{
  auto i = ident_map.find(name);
  if (i != ident_map.end() ) {
    *error = 1; return;
    //throw yy::parser::syntax_error(loc, string("Redefine `") + name + "`");
  }

  Ident* ident = new Ident(name, loc); // move する
  ident_map.insert(make_pair(string(name), ident));
  ident->type = 1;
  ident->u.checker = check;

  assert( ident_map.at(name) );
}


// syntax_error 例外ハンドラのなかで呼び出される。ここから例外を再送出せず、
// 表示させたりログを記録する.
void yyerror(const YYLTYPE& locp, const char* format, ...)
{
  assert(format);
  
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%d:%d: ", locp.first_line, locp.first_column);
  vfprintf(stderr, format, ap);
  va_end(ap);
}

// %parse-param で指定した追加引数が全部来るバージョンが必要。
// YYBACKUP() 内から呼び出される.
void yyerror( YYLTYPE* locp, MyCppLexer* scanner, const char* str )
{
    assert(locp);
    assert(str);
    yyerror(*locp, str);
}

