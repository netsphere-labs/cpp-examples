
# <prefix>/include/mydll/mydll.h
pkginclude_HEADERS = mydll.h mydll_c.h

# <prefix>/lib/libmydll.a 
lib_LTLIBRARIES = libmydll.la

# current:revision:age
#   => *.so.(current-age).age.revision
# libtool:   error: AGE '4' is greater than the current interface number '2'
# libtool:   error: '2:1:4' is not valid version information

libmydll_la_LDFLAGS = -version-info 3:1:2 -no-undefined
libmydll_la_SOURCES = mydll.cpp
