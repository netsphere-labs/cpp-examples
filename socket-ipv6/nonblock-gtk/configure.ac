dnl Process this file with autoconf to produce a configure script.

# mingw: autoconf 2.69
AC_PREREQ([2.69])

AC_INIT([nonblock-gtk],[0.1])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE

#AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_CC
AC_PROG_CXX

# OpenGL 関係の導入 v3.16
#pkg_modules="gtk+-3.0 >= 3.16.0"
pkg_modules="gtk4 >= 4.0.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
PACKAGE_CFLAGS="$PACKAGE_CFLAGS -Wextra -Wall -Wno-unused-parameter"
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# AC_ARG_ENABLE(feature, help-string, [action-if-given], [action-if-not-given])
AC_ARG_ENABLE([thread],
  AS_HELP_STRING([--enable-thread],
            [マルチスレッド版. disable の場合は non-blocking ソケット (default is YES)]),
  [], [enable_thread=yes])
if test x"${enable_thread}" != x"no"; then
  AC_DEFINE([USE_THREAD], 1, [Define if you want to use the multi-thread version.])
fi

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
