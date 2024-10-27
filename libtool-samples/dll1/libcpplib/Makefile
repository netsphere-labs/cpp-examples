# -*- coding:utf-8 -*-

EXEEXT = 

# mingw: libtool の再インストール
#LIBTOOL = /usr/bin/libtool
LIBTOOL = libtool

# 共用ライブラリは拡張子 .la; 'lib' で始まること.
TARGETS = libmydll.la 

# See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
#     Compiler Options Hardening Guide for C and C++

CXXFLAGS = -std=c++20 -fconstexpr-ops-limit=2147483647 -DLIBCPPLIB_EXPORTS -Wall -Wextra -Wno-unused-parameter -g3    \
    -O2 -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
    -Werror=format-security \
    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
    -D_GLIBCXX_ASSERTIONS \
    -fstrict-flex-arrays=3 \
    -fstack-clash-protection -fstack-protector-strong \
    -Wl,-z,nodlopen -Wl,-z,noexecstack \
    -Wl,-z,relro -Wl,-z,now \
    -Wl,--as-needed -Wl,--no-copy-dt-needed-entries

CXXLD = g++

%.lo: %.cpp
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=compile $(CXX) -c $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



all: $(TARGETS)

fuga.lo: fuga.cpp fuga.h pch.h framework.h

# -version-info, -no-undefined および -rpath は libtool のオプション.
libmydll.la: fuga.lo
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -version-info 3:1:2 -no-undefined -rpath /tmp/hoge/lib   -o $@  $^

mydll_test$(EXEEXT): mydll_test.o libmydll.la
	$(LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link $(CXXLD) $(LDFLAGS) $(LDLIBS) -o $@ $^

mydll_test.o: mydll_test.cpp mydll.h


clean:
	$(LIBTOOL) --mode=clean rm -f $(TARGETS) *.o *.lo
