# -*- coding:utf-8 -*-

EXEEXT = .exe

# 共用ライブラリは拡張子 .la; 'lib' で始まること.
TARGETS = libmydll.la mydll_test$(EXEEXT)

LIBTOOL = ./libtool
CXXFLAGS = -Wall -Wextra -g
CXXLD = g++

%.lo: %.cpp
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=compile $(CXX) -c $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



all: $(TARGETS)

mydll.lo: mydll.cpp mydll.h

libmydll.la: mydll.lo
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -version-info 3:1:2 -no-undefined -rpath /tmp/hoge/lib   -o $@  $^

mydll_test$(EXEEXT): mydll_test.o libmydll.la
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	$(LIBTOOL) --mode=clean rm -f $(TARGETS) *.o *.lo
