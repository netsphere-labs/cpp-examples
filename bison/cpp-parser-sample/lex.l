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
#include "cpp-parse.tab.hh"
#include "cpp-lexer.h"
%}

%{
  // Bison も Flex も, 自動で position を更新しない.
  // flex 側で, 位置情報を適切に更新する必要がある.
  // 行番号だけで十分な場合は, %option yylineno を指定し, yylineno が使える.
#define YY_USER_ACTION \
    loc.begin.line = loc.end.line; \
    loc.begin.column = loc.end.column ; \
    for (const char* pp = yytext; *pp; pp++) { \
      if (*pp == '\n') {                        \
        loc.end.line++; loc.end.column = 0; \
      } \
      else                     \
        loc.end.column++; \
    }
  
%}


/***********************************************************************
 * Regular Expressions Part
 */
%%

#.*    { }

[a-zA-Z][a-zA-Z0-9]*  {
        if ( !strcmp(yytext, "let"))
          return yy::parser::make_TK_LET(loc);
        else if ( !strcmp(yytext, "condition"))
          return yy::parser::make_TK_CONDITION(loc);
        else {
          return yy::parser::make_TK_IDENT(yytext, loc);
    }
  }

".."    return yy::parser::make_TK_DOTDOT(loc);

[0-9]+  {
        return yy::parser::make_TK_INTEGER(atoi(yytext), loc);
  }

[\r\n]([ \t]*[\r\n])* { return yy::parser::make_TK_SEP(loc); }

[ \t]+   { }

.       return yy::parser::symbol_type(yytext[0], loc);

<<EOF>>    return yy::parser::make_YYEOF(loc);

%%
