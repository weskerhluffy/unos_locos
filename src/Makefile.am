## Process this file with automake to produce Makefile.in

# XXX: http://stackoverflow.com/questions/323790/autotools-how-to-set-global-compilation-flag
include $(top_srcdir)/common.mk

AM_CFLAGS = -Wall -g3 -O0 -I$(ZLOG_DIR)/include  -std=c99  -ansi -fno-asm -lm -static 
lib_LTLIBRARIES = libunoslocos.la libcacacomun.la
libunoslocos_la_SOURCES = unoslocos.c unoslocos.h
libcacacomun_la_SOURCES = cacacomun.c cacacomun.h

bin_PROGRAMS = main
main_SOURCES = main.c
main_LDADD = libunoslocos.la libcacacomun.la -L$(ZLOG_DIR)/lib -lzlog -lpthread