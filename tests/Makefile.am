## Process this file with automake to produce Makefile.in

TESTS = check_unoslocos
check_PROGRAMS = check_unoslocos
check_unoslocos_SOURCES = check_unoslocos.c $(top_builddir)/src/unoslocos.h
check_unoslocos_CFLAGS = @CHECK_CFLAGS@
check_unoslocos_LDADD = $(top_builddir)/src/libunoslocos.la @CHECK_LIBS@
