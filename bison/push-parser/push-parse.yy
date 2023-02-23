// -*- mode:c++ -*-

// Bison 3.2: C++ position.hh, stack.hh が廃止.
%require "3.7"

/* GLR parser */
// %glr-parser

// `lalr1.cc` スケルトンを選択
 //%language "c++"

// 上記で決まるので, %skeleton を省略してよい.
// %skeleton "glr.c"

/* glr.tab.h を生成 */
%defines

// %error-verbose は非推奨
%define parse.error verbose

// C++: location.hh を生成.
%locations

// C++: %union の代わりに, variant を使う. 次のようなコードが生成される.
// ```
//     yylhs.value.emplace< string > ();
// ```
 //%define api.value.type variant

// C++: variant を指定するとき, 必ず次も定義すること。定義しないと, yylex() の戻り値
// の型として int を期待する場所と yy::parser::symbol_type を期待する場所の両方
// があり、ビルドできない。
 //%define api.token.constructor

// 'api.pure' is not supported in GLR C++ parser. Always pure parser.
// In "glr.c", only true / false
// 'lalr1.cc' スケルトンでは, 常に pure なので、オプションはない。
%define api.pure full

// Plain C, D Java only. %.tab.hh で yypstate_new(), yypush_parse(), yypstate_delete()
//     値: pull, push, both
%define api.push-pull push

//%define api.token.prefix {TK_}

// グローバルな yylex() ではなく, lexer インスタンスを渡すようにする.
%parse-param { class MyCppLexer* scanner }

%code requires {
// cpp-parse.tab.hh の冒頭に出力されるセクション.
#include <vector>
  
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

// %parse-param で指定した追加引数が全部来るバージョンが必要。
// YY_("memory exhausted") などは, こちらが呼び出される.
void yyerror(YYLTYPE* locp, MyCppLexer* scanner, const char* str);
 
// 自分で呼び出すほうの宣言.
void yyerror(const YYLTYPE& locp, const char* format, ...);

// グローバルな yylex() ではなく, lexer の yylex() メソッド.
#undef yylex
#define yylex scanner->lex
%}


///////////////////////////////////////////////////////
// ここから構文定義

// ここで定義するトークンとは別に, `token_kind_type` enum に次が定義される:
//     YYEOF   = 0 "end of file"
//     YYerror  error                ..これは使わない!
//     YYUNDEF  "invalid token"      ..字句解析エラーのときはこちらを返す

// union YYSTYPE 型
%union {
  int  intval;

  char* sval;

  std::vector<int>* intvec;

  const Ident* ident;
  Checker* checker;
}

%token TK_LET TK_CONDITION TK_DOTDOT TK_SEP
// variant の場合, 型名を書く. %union と異なり, ポインタではない。
%token<intval> TK_INTEGER
%token<sval> TK_IDENT
%left '+' '-'

%type<intvec> val_list
%type<ident> type_use0_1
%type<checker> type
%type<intval> expr primary


 // 以下が必須。これがないとメモリリークする。
 // エラーリカバリのとき、先読みしていたトークンについて呼び出される.
%destructor { free($$); } <sval>
%destructor { delete $$; } <intvec>
%destructor { delete $$; } <ident>
%destructor { delete $$; } <checker>

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

stmt :  cond_decl { printf("%d:%d cond_decl\n", @1.first_line, @1.first_column); }
      | let_stmt  { printf("let_stmt\n"); }
      ;

/*
  condition subrange = lo .. hi;
  condition enum = (x1, x2, x3);
 */
cond_decl : TK_CONDITION TK_IDENT '=' type {
          int error = 0;
          Ident::cond_define($2, $4, @2, &error);
          if (error) {
            yyerror(@1, "Syntax error: redefine: %s", $2);
            free($2); delete $4;
            YYERROR; // YYERROR はマクロで, goto yyerrorlab
          }
          // free $2 は不要
        }
  ;

type :  '(' val_list ')' {
          printf("val_list\n");
          $$ = new EnumChecker($2);  // move する
        }
      | expr TK_DOTDOT expr {
          printf("dotdot: %d..%d\n", $1, $3);
          $$ = new RangeChecker($1, $3);
        }
  ;

let_stmt: TK_LET TK_IDENT type_use0_1 '=' expr {
          bool r;
          int error;
          if ($3) {
            error = 0;
            r = $3->check_val($5, &error);
            if (error) {
              yyerror(@1, "Type error: `%s' is not a condition.", $2);
              free($2);
              YYERROR;
            }
            else if (!r) {
              free($2);
              yyerror(@2, "Domain error: check failed"); YYERROR;
            }
          }
          error = 0;
          r = Ident::var_update($2, $5, @2, &error);
          if (error) {
            yyerror(@2, "Type error: `%s' is not a variable", $2);
            free($2);
            YYERROR;
          }
          printf("Var `%s' = %d\n", $2, $5); // DEBUG
          if (!r)  // すでにあって挿入されなかった
            free($2);
        }
  ;
  
type_use0_1: ':' TK_IDENT {
          $$ = find_ident($2, @2);
          if (!$$) {
            yyerror(@2, "Syntax error: `%s' not found", $2);
            free($2);
            YYERROR;
          }
          free($2);
        }
      | %empty  { $$ = NULL; }
  ;

val_list : expr     { $$ = new vector<int>{$1}; }
      | val_list ',' expr  { $1->push_back($3); $$ = $1; }
      ;

expr :  expr '+' primary { $$ = $1 + $3; }
      | expr '-' primary { $$ = $1 - $3; }
      | primary    { $$ = $1; }
      ;

primary : TK_INTEGER  { $$ = $1; }
      | TK_IDENT    {
          const Ident* ident = find_ident($1, @1);
          if (!ident) {
            yyerror(@1, "Syntax error: `%s' not found", $1);
            free($1);
            YYERROR;
          }
          free($1); // もういらん
          int error = 0;
          $$ = ident->get_val(&error);
          if (error) {
            yyerror(@1, "Type error: `%s' is not a variable", ident->name() );
            YYERROR;
          }
        } 
      | '(' expr ')' { $$ = $2; }
      ;

