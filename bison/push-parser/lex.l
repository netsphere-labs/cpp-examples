/* -*- mode:c++ -*- */

%option c++

/* C++では必須. yyFlexLexer が $(prefix)FlexLexer になる。
   出力ファイルが lex.$(prefix).cc になる。
   この $(prefix)FlexLexer を派生させて自分の lexer クラスを作る. */
%option prefix="CppBase"

/* interactive 不要. コメントアウトすると, YY_INTERACTIVE が定義され, get() で
  読み込む. */
%option batch

%option 8bit

/* マッチしなかったときのデフォルト動作 (ECHO: 標準出力への出力) を抑制. */
%option nodefault

/* YY_NO_UNPUT が定義され, yyunput() がコメントアウトされる. */   
%option nounput

/* ファイル一つで終了 */
%option noyywrap

/* <unistd.h> は不要. YY_NO_UNISTD_H が定義される. */
%option nounistd

/* To use start condition stacks.
   <state> の切替えに, BEGIN に代えて yy_push_state() / yy_pop_state() を使う */
%option stack

%option warn

/* 入力の行数を大域変数 yylineno に格納。型をパーサ側に合わせるため使わない */
/* %option yylineno */

%{
// lex.CppBase.cc に展開される
#include "push-parse.tab.hh"
#include "cpp-lexer.h"
%}

%{
  // Bison も Flex も, 自動で position を更新しない.
  // flex 側で, 位置情報を適切に更新する必要がある.
  // 行番号だけで十分な場合は, %option yylineno を指定し, yylineno が使える.
  // loc.end.column はトークンの最後の文字 "の次" を指す.
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column + 1; \
    for (const char* pp = yytext; *pp; pp++) { \
      if (*pp == '\n') {                        \
        yylloc->last_line++; yylloc->last_column = 0; \
      } \
      else                     \
        yylloc->last_column++;                 \
    }
  
%}


/***********************************************************************
 * Regular Expressions Part
 */
%%

#.*    { // Comment. Skip
    }

[a-zA-Z][a-zA-Z0-9]*  {
        if ( !strcmp(yytext, "let"))
          return yytokentype::TK_LET;
        else if ( !strcmp(yytext, "condition"))
          return yytokentype::TK_CONDITION;
        else {
          yylval->sval = strdup(yytext);
          return yytokentype::TK_IDENT;
        }
    }

".."    return yytokentype::TK_DOTDOT;

[0-9]+  {
        yylval->intval = atoi(yytext);
        return yytokentype::TK_INTEGER;
    }

[\r\n]([ \t]*[\r\n])* { return yytokentype::TK_SEP; }

[ \t]+   { }

"!"   {   // 字句解析でのエラー
        return yytokentype::YYUNDEF; }

.       return (yytokentype) yytext[0];

<<EOF>>    return yytokentype::YYEOF;

%%
