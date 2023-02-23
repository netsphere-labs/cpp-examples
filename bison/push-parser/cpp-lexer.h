// -*- mode:c++ -*-

#ifndef __FLEX_LEXER_H
  // lex.ll の %option prefix と合わせること.
  #define yyFlexLexer CppBaseFlexLexer
  #include <FlexLexer.h>
  #undef yyFlexLexer
#endif

#include "push-parse.tab.hh"
//#include "location.hh"

/**
yyFlexLexer クラスは, テンプレートではなく, int yylex() 決め打ちになっている。
オーバロードするため, 自分の lexer クラスが必要。ヒドい
 */
class MyCppLexer: public CppBaseFlexLexer
{
  typedef CppBaseFlexLexer super;

  // Bison 側で自動的に宣言される. yylex() の引数として渡してもらう.
  //YYLTYPE loc;
  
public:
  MyCppLexer(std::istream* arg_yyin): super(arg_yyin, NULL)
    { }
  
  // ここで宣言するが, 定義は lex.CppBase.cc の生成されたものを使う.
  yytokentype lex(YYSTYPE* yylval, YYLTYPE* yylloc);
};

#undef YY_DECL
#define YY_DECL  yytokentype MyCppLexer::lex(YYSTYPE* yylval, YYLTYPE* yylloc)
    
