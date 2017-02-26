SUBDIRS = system

# Force include/debian-installer not to be in the default search path.
# (It provides string.h and trumps the system copy.)
DEFAULT_INCLUDES = -I.@am__isrc@

AM_CPPFLAGS = \
	-I$(top_srcdir)/include -I$(top_builddir)/

lib_LTLIBRARIES = libdebian-installer.la libdebian-installer-extra.la

libdebian_installer_la_SOURCES = \
	exec.c \
	file.c \
	file_rfc822_read.c \
	hash.c \
	log.c \
	mem.c \
	mem_chunk.c \
	package.c \
	package_file.c \
	packages.c \
	slist.c \
	string.c \
	tree.c \
	utils.c \
	version.c

libdebian_installer_extra_la_SOURCES = \
	list.c \
	release.c \
	release_file.c

libdebian_installer_la_LIBADD = \
	system/libsystem.la

libdebian_installer_extra_la_LIBADD = \
	libdebian-installer.la

libdebian_installer_la_LDFLAGS = \
	-version-number $(LIBRARY_VERSION_LIBTOOL) \
	-Wl,--version-script=$(srcdir)/libdebian-installer.ver \
	-Wl,--no-undefined-version

libdebian_installer_extra_la_LDFLAGS = \
	-version-number $(LIBRARY_VERSION_LIBTOOL) \
	-Wl,--version-script=$(srcdir)/libdebian-installer-extra.ver \
	-Wl,--no-undefined-version

libdebian_installer_la_DEPENDENCIES = \
	$(srcdir)/libdebian-installer.ver \
	system/libsystem.la

libdebian_installer_extra_la_DEPENDENCIES = \
	$(srcdir)/libdebian-installer-extra.ver \
	libdebian-installer.la

