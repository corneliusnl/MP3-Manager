AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = id3tool

id3tool_SOURCES = \
	id3tool.c \
	id3.c \
	id3.h \
	getopt.c \
	getopt.h

EXTRA_DIST = \
	README \
	INSTALL \
	CHANGELOG \
	COPYING \
	id3tool.1 \
	README.Win32 \
	update-conf \
	config.h.win32 \
	id3tool.mak

man_MANS = id3tool.1

CLEANFILES = *~

dist-hook:	
	sudo debian/rules clean
	cp -pr debian $(distdir)
