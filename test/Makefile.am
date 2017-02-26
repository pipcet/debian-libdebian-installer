TESTS = test

check_PROGRAMS = test

EXTRA_DIST = Packages

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh
TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh

# Force include/debian-installer not to be in the default search path.
# (It provides string.h and trumps the system copy.)
DEFAULT_INCLUDES = -I.@am__isrc@

AM_CFLAGS = -I$(top_srcdir)/include @CHECK_CFLAGS@ -DDATADIR=\"$(srcdir)\"

test_LDFLAGS = \
	-static

test_SOURCES = main.c \
	test_exec.c \
	test_file_rfc822.c \
	test_hash.c \
	test_package.c \
	test_packages.c \
	test_release.c \
	test_version.c

test_LDADD = \
	$(top_builddir)/src/.libs/libdebian-installer.la \
	$(top_builddir)/src/.libs/libdebian-installer-extra.la \
	@CHECK_LIBS@
