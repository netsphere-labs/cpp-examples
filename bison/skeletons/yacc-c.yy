
// Bison 3.2: C++ position.hh, stack.hh が廃止.
%require "3.7"

/* GLR parser */
// %glr-parser

//%language "c++"

// 何も指定しなければ yacc.c スケルトン。%skeleton を省略してよい。
// %skeleton "glr.c"

/* glr.tab.h を生成 */
%defines

// %error-verbose は非推奨
%define parse.error verbose

// location.hh を生成.
%locations

// GLR C++ parser: 'api.pure' is not supported. Always pure parser.
// `glr.c`:        Only true / false ("full" はエラー)
// `lalr1.cc`:     常に pure なので、オプションはない。
// `yacc.c`:       再入可能にするには, "full" にする。(true ではない.)
%define api.pure full


///////////////////////////
// ここから定義

%token TK_TYPE DOTDOT
%token ID
%left '+' '-'

%{
#include "glr.tab.hh" // YYSTYPE が定義される
extern int yylex( YYSTYPE* yylval_param );
%}

%%

/*
  type subrange = lo .. hi;
  type enum = (a, b, c);
 */
type_decl : TK_TYPE ID '=' type ';'
  ;

type : '(' id_list ')' { printf("id_list\n"); }
  | expr DOTDOT expr { printf("dotdot\n"); }
  ;

id_list :
    ID
  | id_list ',' ID
  ;

expr:
    '(' expr ')'
  | expr '+' expr
  | expr '-' expr
  | ID
  ;
