// -*- mode:c++ -*-

// Bison 3.2: C++ position.hh, stack.hh が廃止.
%require "3.7"

/* GLR parser */
// %glr-parser

// `lalr1.cc` スケルトンを選択
%language "c++"

// 上記で決まるので, %skeleton を省略してよい.
// %skeleton "glr.c"

/* glr.tab.h を生成 */
%defines

// %error-verbose は非推奨
%define parse.error verbose

// location.hh を生成.
%locations

// C++: %union の代わりに, variant を使う. 次のようなコードが生成される.
// ```
//     yylhs.value.emplace< string > ();
// ```
%define api.value.type variant
// variant を指定するとき, 必ず次も定義すること。定義しないと, yylex() の戻り値
// の型として int を期待する場所と yy::parser::symbol_type を期待する場所の両方
// があり、ビルドできない。
%define api.token.constructor

// 'api.pure' is not supported in GLR C++ parser. Always pure parser.
// In "glr.c", only true / false
// 'lalr1.cc' スケルトンでは, 常に pure なので、オプションはない。
// %define api.pure true

//%define api.token.prefix {TK_}

// グローバルな yylex() ではなく, lexer インスタンスを渡すようにする.
%parse-param { class MyCppLexer* scanner }

%code requires {
// cpp-parse.tab.hh の冒頭に出力されるセクション.
// namespace yy の外側で宣言.
class MyCppLexer;
class Checker;
class Ident;
}

%{
// cpp-parse.tab.cc の冒頭に出力されるセクション.
//#include "cpp-parse.tab.hh"  ヘッダは生成ソースで include されるので明記不要.
/*
cpp-parse.tab.hh:
    struct symbol_type : basic_symbol<by_kind> {};
        /// The semantic value.
        value_type value;
        /// The location.
        location_type location;
      
cpp-parse.tab.cc:
    symbol_type yyla;
    yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
*/

#include "cpp-lexer.h"
#include "runtime.h"
using namespace std;

// グローバルな yylex() ではなく, lexer の yylex() メソッド.
#undef yylex
#define yylex scanner->lex
%}


///////////////////////////////////////////////////////
// ここから構文定義

%token TK_LET TK_CONDITION TK_DOTDOT TK_SEP
// variant の場合, 型名を書く. %union と異なり, ポインタではない。
%token<int> TK_INTEGER
%token<std::string> TK_IDENT
%left '+' '-'

%type<std::vector<int> > val_list
%type<const Ident*> type_use0_1
%type<Checker*> type
%type<int> expr primary

%%

program : sep0_n stmts sep0_n YYEOF

stmts : stmts sep1_n stmt 
      | stmt
  ;

sep0_n : sep0_n TK_SEP
      | %empty
      ;

sep1_n : sep1_n TK_SEP
      | TK_SEP
      ;

stmt :  cond_decl { printf("%d:%d cond_decl\n", @1.begin.line, @1.begin.column); }
      | let_stmt  { printf("let_stmt\n"); }
      ;

/*
  condition subrange = lo .. hi;
  condition enum = (x1, x2, x3);
 */
cond_decl : TK_CONDITION TK_IDENT '=' type {
          Ident::cond_define($2, $4, @2);
        }
  ;

type :  '(' val_list ')' {
          printf("val_list\n");
          $$ = new EnumChecker($2);
        }
      | expr TK_DOTDOT expr {
          printf("dotdot: %d..%d\n", $1, $3);
          $$ = new RangeChecker($1, $3);
        }
  ;

let_stmt: TK_LET TK_IDENT type_use0_1 '=' expr {
          if ($3) {
            if ( !$3->check_val($5) )
              throw syntax_error(@2, "Check failed");
          }
          Ident::var_update($2, $5, @2);
          printf("`%s` = %d\n", $2.c_str(), $5);
        }
  ;
  
type_use0_1: ':' TK_IDENT { $$ = find_ident($2, @2); }
      | %empty  { $$ = NULL; }
;

val_list : expr     { $$ = vector<int>{$1}; }
      | val_list ',' expr  { $1.push_back($3); $$ = $1; }
      ;

expr :  expr '+' primary { $$ = $1 + $3; }
      | expr '-' primary { $$ = $1 - $3; }
      | primary    { $$ = $1; }
      ;

primary : TK_INTEGER  { $$ = $1; }
      | TK_IDENT    {
          const Ident* ident = find_ident($1, @1);
          $$ = ident->get_val();
        } 
      | '(' expr ')' { $$ = $2; }
      ;

