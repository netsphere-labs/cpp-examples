
#include <stdio.h>
#include "cpp-parse.tab.hh"
#include "cpp-lexer.h"
#include <iostream>
#include "../ptr_map.h"
#include "runtime.h"
using namespace std;

namespace yy {
/**
cpp-parse.tab.cc 内で 1引数の error(const syntax_error& ) は定義される.
2 引数版は、開発者が定義してやる必要がある。
```
    void parser::error(const syntax_error& yyexc) {
      error (yyexc.location, yyexc.what ());
    }
```

次のように使われる。ログ出力ぐらいしかできることはない。うーむ・・・
```
    catch (const syntax_error& yyexc) {
      YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
      error (yyexc);
      goto yyerrlab1;  あるいは YYERROR;
    }
```
 */
void parser::error(const location_type& loc, const std::string& msg)
{
  printf("%d:%d %s\n", loc.begin.line, loc.begin.column, msg.c_str());
}

} // namespace yy


// <FlexLexer.h> class yyFlexLexer で yylex() が宣言されており, 使わなくても
// 定義が必要.
int CppBaseFlexLexer::yylex()
{
    abort(); // 呼び出されてはいけない.
}


int main( int argc, char* argv[] )
{
    //const char* text = "type t = (a) .. b;";
  MyCppLexer scanner(&cin);
  yy::parser my_parser(&scanner);

  // 成功時 0
  int r = my_parser.parse();

  return r;
}


////////////////////////////////////////////////////////////////////

ptr_map<string, Ident*> ident_map;

const Ident* find_ident(const string& name, const yy::parser::location_type& loc) {
  auto i = ident_map.find(name);
  if ( i == ident_map.end() )
    throw yy::parser::syntax_error(loc, string("`") + name + "` not found");
  return i->second;
}

void Ident::var_update(const string& name, int val,
                       const yy::parser::location_type& loc)
{
  auto i = ident_map.find(name);
  Ident* ident;
  if ( i != ident_map.end() ) {
    ident = i->second;
    if ( ident->type != 0)
      throw yy::parser::syntax_error(loc, string("`") + name + "` is not a variable");
  }
  else {
    ident = new Ident(name, loc);
    ident_map.insert(make_pair(name, ident));
  }
  ident->type = 0;
  ident->u.value = val;

  assert( ident_map.at(name) );
}

void Ident::cond_define(const string& name, Checker* check,
                        const yy::parser::location_type& loc) {
  auto i = ident_map.find(name);
  if (i != ident_map.end() )
    throw yy::parser::syntax_error(loc, string("Redefine `") + name + "`");

  Ident* ident = new Ident(name, loc);
  ident_map.insert(make_pair(name, ident));
  ident->type = 1;
  ident->u.checker = check;

  assert( ident_map.at(name) );
}
