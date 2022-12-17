## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = nonblock_client

nonblock_client_SOURCES = \
	main.cpp \
	callbacks.cpp callbacks.h \
	client.cpp client.h \
	../../ipv6_sock/connect_to_server.cpp \
	../../ipv6_sock/connect_to_server.h \
	../../ipv6_sock/debug_print.cpp \
	../../ipv6_sock/debug_print.h

nonblock_client_LDADD = @PACKAGE_LIBS@

