TESTS = test

check_PROGRAMS = test

EXTRA_DIST = Packages

AM_CFLAGS = -I$(top_srcdir)/include @CHECK_CFLAGS@ -DDATADIR=\"$(srcdir)\"

test_SOURCES = main.c test_hash.c test_system_packages.c
test_LDADD = $(top_builddir)/src/.libs/libdebian-installer.la @CHECK_LIBS@
