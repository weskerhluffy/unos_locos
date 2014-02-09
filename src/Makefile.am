## Process this file with automake to produce Makefile.in

AM_CFLAGS = -g
lib_LTLIBRARIES = libunoslocos.la libcacacomun.la
libunoslocos_la_SOURCES = unoslocos.c unoslocos.h
libcacacomun_la_SOURCES = cacacomun.c cacacomun.h

bin_PROGRAMS = main
main_SOURCES = main.c
main_LDADD = libunoslocos.la libcacacomun.la