# Generated automatically from Makefile.in by configure.
CXXFLAGS=-g -O2 -Wall


#comment this if you are using readline >= 4.0
#I can't come up with an easy configure.in test for this.. blah.  oh well, it
#doesn't seem to hurt anything if its defined even with the new readline
CXXFLAGS+=-DOLD_READLINE

CXX=c++
LIBS=-lhistory -lreadline -lcurses 

prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
mandir=${prefix}/man
install=/usr/bin/install -c

DESTDIR=

all: id3ed

OBJS=id3ed.o misc.o

id3ed: $(OBJS) .depend
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

dep depend .depend:
	$(CXX) -MM *.cc > .depend

install: all
	$(install) -s -o root -g bin -m 0755 id3ed $(DESTDIR)$(bindir)
	$(install) -o root -g bin -m 0644 id3ed.1 $(DESTDIR)$(mandir)/man1

uninstall:
	-rm $(DESTDIR)$(bindir)/id3ed
	-rm $(DESTDIR)$(mandir)/man1/id3ed.1

configure: configure.in acconfig.h
	autoheader
	autoconf

Makefile: Makefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

				

#VER=`pwd | sed "s/-\([0-9.]\+\)$/\1/"`
#SDIR=id3ed-$(VER)/
#dist:
#	cd ..;tar -czhf distro/id3ed-$(VER).tar.gz $(SDIR)README $(SDIR)Changelog $(SDIR)COPYING $(SDIR)*.cc $(SDIR)*.h $(SDIR)Makefile $(SDIR)id3ed.1


clean:
	-rm id3ed *.o
		 
distclean: clean
	-rm *~ config.h config.cache config.log Makefile .depend
	-touch .depend
		 

ifeq (.depend,$(wildcard .depend))
include .depend
endif

.PHONY: clean distclean dep depend all install
