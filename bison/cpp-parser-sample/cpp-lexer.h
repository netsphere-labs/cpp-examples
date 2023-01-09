// -*- mode:c++ -*-

#ifndef __FLEX_LEXER_H
  // lex.ll の %option prefix と合わせること.
  #define yyFlexLexer CppBaseFlexLexer
  #include <FlexLexer.h>
  #undef yyFlexLexer
#endif

#include "cpp-parse.tab.hh"
#include "location.hh"

/**
yyFlexLexer クラスは, テンプレートではなく, int yylex() 決め打ちになっている。
オーバロードするため, 自分の lexer クラスが必要。ヒドい
 */
class MyCppLexer: public CppBaseFlexLexer
{
  typedef CppBaseFlexLexer super;

  // lexer 側で持つ.
  yy::location loc;
  
public:
  MyCppLexer(std::istream* arg_yyin):
    super(arg_yyin, NULL),
    loc(yy::position(NULL, 1, 0), yy::position(NULL, 1, 0)) { }
  
  // ここで宣言するが, 定義は lex.CppBase.cc の生成されたものを使う.
  yy::parser::symbol_type lex(); // yy::parser::value_type* yylval, yy::location* yyloc );
};

#undef YY_DECL
#define YY_DECL  yy::parser::symbol_type MyCppLexer::lex()
    
