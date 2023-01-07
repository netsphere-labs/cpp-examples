
// Bison 3.2: C++ position.hh, stack.hh が廃止.
%require "3.7"

// Bison 3.8: "glr2.cc" は、直接 %skeleton で指定するしかない。
%skeleton "glr2.cc"

/* GLR parser */
// %glr-parser

//%language "c++"

/* glr.tab.h を生成 */
%defines

// %error-verbose は非推奨
%define parse.error verbose

// location.hh を生成.
%locations
%define api.location.file "glr2-loc.hh"

// 'api.pure' is not supported in GLR C++ parser. Always pure parser.
// In "glr.c", only true / false
// 'lalr1.cc' スケルトンでは, 常に pure なので、オプションはない。
// %define api.pure true


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
