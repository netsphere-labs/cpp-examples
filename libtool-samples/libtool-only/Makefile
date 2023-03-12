# -*- coding:utf-8 -*-

EXEEXT = .exe

# mingw
LIBTOOL = ./libtool


# 共用ライブラリは拡張子 .la; 'lib' で始まること.
TARGETS = libmydll.la mydll_test$(EXEEXT)

CXXFLAGS = -Wall -Wextra -g
CXXLD = g++

%.lo: %.cpp
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=compile $(CXX) -c $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



all: $(TARGETS)

mydll.lo: mydll.cpp mydll.h

# -version-info, -no-undefined および -rpath は libtool のオプション.
libmydll.la: mydll.lo
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -version-info 3:1:2 -no-undefined -rpath /tmp/hoge/lib   -o $@  $^

mydll_test$(EXEEXT): mydll_test.o libmydll.la
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -o $@ $^

mydll_test.o: mydll_test.cpp mydll.h


clean:
	$(LIBTOOL) --mode=clean rm -f $(TARGETS) *.o *.lo
