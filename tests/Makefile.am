## Process this file with automake to produce Makefile.in

include $(top_srcdir)/common.mk

TESTS = check_unoslocos check_cacacomun
check_PROGRAMS = check_unoslocos check_cacacomun
check_unoslocos_SOURCES = check_unoslocos.c $(top_builddir)/src/unoslocos.h
check_unoslocos_CFLAGS = @CHECK_CFLAGS@
check_unoslocos_LDADD = $(top_builddir)/src/libcacacomun.la $(top_builddir)/src/libunoslocos.la @CHECK_LIBS@

check_cacacomun_SOURCES = check_cacacomun.c $(top_builddir)/src/cacacomun.h
# XXX: http://stackoverflow.com/questions/20230827/how-to-set-include-paths-with-autotools
check_cacacomun_CFLAGS = @CHECK_CFLAGS@ -g -I$(ZLOG_DIR)/include 
# XXX: http://osiris.laya.com/coding/dylib_linking.html
# XXX: http://stackoverflow.com/questions/17703510/dyld-library-not-loaded-reason-image-not-loaded
check_cacacomun_LDADD = $(top_builddir)/src/libcacacomun.la @CHECK_LIBS@ -L$(ZLOG_DIR)/lib -lzlog -lpthread
