
# GNU Bison / Flex テンプレート


# Flex は昔は http://www.gnu.org/software/flex/ だったが, 離脱して https://github.com/westes/flex/
#                 Bison   Flex
# Fedora 37      v3.8.2  v2.6.4
# Debian 11      v3.7.5  v2.6.4
# openSUSE 15.4  v3.0.4  v2.6.4  -- ちょっと古すぎる。せめて v3.2 以降でないと.

TARGETS = push-parser-sample
  #push-parser-sample

CC = gcc
CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -g
LDLIBS = -lstdc++

all: $(TARGETS)

#
## Plain C push parser (push parser は plain C のみ.)

YACC_OUTPUT = push-parse.gv push-parse.output push-parse.tab.cc \
              push-parse.tab.hh location.hh

push-parser-sample: push-parse.tab.o lex.CppBase.o support.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

push-parse.tab.o: push-parse.tab.cc push-parse.tab.hh

# %glr-parser を指定したとき, 拡張子を .yy にするだけでは, glr.tab.{cc,hh} は生
# 成されるものの, スケルトンは glr.c のまま. YYPURE 0
# コマンドラインオプションで -L c++ を与えると, glr.cc になって, YYPURE 1 にな
# る.
$(YACC_OUTPUT): push-parse.yy
	bison --warnings --verbose --graph $<


#
## lex, support

LEX_OUTPUT = lex.CppBase.cc lex.backup

support.o: support.cpp push-parse.tab.hh
#support-lr1.o: support-lr1.cc lr1.tab.hh

lex.CppBase.o: lex.CppBase.cc push-parse.tab.hh

# lex のほうは, .ll にしなくてもよい。.cc が生成される。
# --bison-bridge not supported for the C++ scanner.
# --bison-locations too.
$(LEX_OUTPUT): lex.l push-parse.tab.hh
	flex --perf-report --verbose $<

clean:
	rm -f $(TARGETS) $(YACC_OUTPUT) $(LR1_OUTPUT) $(LEX_OUTPUT) *.o *~
