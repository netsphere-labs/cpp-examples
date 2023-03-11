# -*- coding:utf-8 -*-

# 改行は LF のみににしなければならない。DOSだと次のような変なエラーがでる:
#   configure: creating ./config.status
#   .in'ig.status: error: cannot find input file: `

AC_PREREQ([2.71])

# [FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS]
AC_INIT([mydll],[0.1],[hisashi dot horikawa at gmail dot com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([README.md])

AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CXX([clang++ g++ c++])

# libtool init
LT_INIT

CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wshadow -Wformat=2 -Wformat-truncation -Wundef -fno-common -Wcast-align -Wwrite-strings -Wno-unused-parameter -fstack-protector-strong "  

# for test. AM_ENABLE_STATIC is deprecated.
AC_ENABLE_STATIC(no)

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  test/Makefile
])
AC_OUTPUT
