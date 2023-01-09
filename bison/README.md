
# GNU Bison サンプル

GNU Bison は構文解析器を生成する, パーサジェネレータ。LALR(1) 法 か GLR 法による。手書きで速く正確なパーサを作るのはそれなりに大変で、しかも LL(1) にしかならなかったりする。無理せず Bison のようなパーサジェネレータを使ったほうがいい。

Bison は字句解析器が分かれている (別に用意する) のがよい。字句解析が複雑になると手書きしたいことが多く、一体型のは逆に困る。
簡単な場合は <i>flex</i> と組み合わせるのが一般的。

Bison などのパーサジェネレータが生成するパーサは, 文脈自由文法 (context-free grammar; CFG) のサブセットの文法を解釈する。




## `skeletons/`

Bison は, m4 で作られたスケルトンをベースに、構文解析器を生成する。元となるスケルトンごとに、プログラミング言語、機能性が決まる。

Bison 3.8 では 7ヶ. `glr2.cc` が加わった。


## `cpp-parser-sample/`

C++ スケルトン `lalr1.cc` の, 一式動くサンプル。過去のしがらみとかいろいろあって、動かすまでが大変。
Flex と組み合わせた。これを出発点にできる。

再入可能にできる。
```c++
int main( int argc, char* argv[] ) 
{
  MyCppLexer scanner(&cin);
  yy::parser my_parser(&scanner);

  // 成功時 0
  int r = my_parser.parse();

  return r;
}
```

Bison 側:
 - `%language "c++"`
 - `%define api.value.type variant` -- 各トークンの型を <code>%union</code> ではなく、C++ っぽく構築する。`string` をポインタにしなくてよい。
   これを指定するときは必ず `%define api.token.constructor` も指定すること。でないとビルドできない。
 - 構文エラーの場合は, 例外 `yy::parser::syntax_error` を投げればよい。
 
Flex側:
 - `%option c++`  -- `yyFlexLexer` クラスを派生して、自分の lexer クラスを作る.
 - 行番号の更新. Bison も Flex も自動で行番号を更新しない。Flex 側で位置情報を更新する。
```c++
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
```
 - 各パターンにマッチしたときの戻り値を `yy::parser::make_TK_IDENT(yytext, loc)` のようにして, Bison 側にトークンを渡す。
 - したがい, <code>lex()</code> メソッドの戻り値の型は `yy::parser::symbol_type`. <code>YY_DECL</code> の定義も必要。




## 類似のソフトウェア

 - Bisonc++ &amp; Flexc++ https://gitlab.com/fbb-git/  試していない. Debian にはパッケージがある。
 - https://www.antlr.org/ Java, C#, Python, JavaScript など多くのターゲットをサポート。LL(*) 法 -- 任意個を先読みして DFA に入力して適用する規則を決定。多くの IDE のプラグインで、グラフィカルに表示。
 
字句解析:
 - https://github.com/Genivia/RE-flex/  RE/flex - 高速. Unicodeサポートを謳うが、実際に識別子などに Unicode を使おうとすると, 表が爆発する。
 - https://quex.sourceforge.net/  昔、使ってみたことがあるが, 結局 flex に戻った。何でだったかな?






## PEG パーサジェネレータ

CFG と別に, 解析表現文法 (parsing expression grammar; PEG) という文法の書き方もある。CFG に似ているが, 曖昧性がない。
LL(1), LR(1), LALR(1), etc... にある shift/reduce 競合が存在しない

素朴に実装するとバックトラックあり再帰下降構文解析で、ケースによっては指数関数時間で爆発する。Packrat parsing という手法で、メモ化を組み合わせることで、メモリは要するが線形時間で解析できるようになった。
正規表現の実装の分類と似た話だなぁ。正規言語における DFA の活用と相似。

実装: それなりに数がある。
 - https://github.com/arithy/packcc/
 - https://tanakh.github.io/Peggy/ja/   Haskell 向け
 - Boost Spirit V2 <code>boost::spirit::qi</code> は C++03 用. C++14 用は Boost Spirit X3.
 - https://github.com/taocpp/PEGTL/
 - https://github.com/yhirose/cpp-peglib/


