## Process this file with automake to produce Makefile.in

AM_CPPFLAGS= -I@top_srcdir@

include $(top_srcdir)/common.mk

TESTS = check_unoslocos check_cacacomun
check_PROGRAMS = check_unoslocos check_cacacomun
check_unoslocos_SOURCES = check_unoslocos.c $(top_builddir)/src/unoslocos.h
check_unoslocos_CFLAGS = @CHECK_CFLAGS@ -g -I$(ZLOG_DIR)/include  -Wall
check_unoslocos_LDADD =  $(top_builddir)/src/libunoslocos.la $(top_builddir)/src/libcacacomun.la  @CHECK_LIBS@  -L$(ZLOG_DIR)/lib -lzlog -lpthread
#check_unoslocos_LDADD = $(top_builddir)/src/libcacacomun.la $(top_builddir)/src/libunoslocos.la @CHECK_LIBS@ 
#check_unoslocos_LDFLAGS = -static

check_cacacomun_SOURCES = check_cacacomun.c $(top_builddir)/src/cacacomun.h
# XXX: http://stackoverflow.com/questions/20230827/how-to-set-include-paths-with-autotools
check_cacacomun_CFLAGS = @CHECK_CFLAGS@ -g3 -O0 -I$(ZLOG_DIR)/include  -Wall  -ansi -fno-asm -lm -static -std=c99
# XXX: http://osiris.laya.com/coding/dylib_linking.html
# XXX: http://stackoverflow.com/questions/17703510/dyld-library-not-loaded-reason-image-not-loaded
check_cacacomun_LDADD = $(top_builddir)/src/libcacacomun.la @CHECK_LIBS@ -L$(ZLOG_DIR)/lib -lzlog -lpthread
#check_cacacomun_LDFLAGS= -Wl,-stack_size -Wl,4000000
