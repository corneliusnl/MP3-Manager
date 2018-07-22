##########################################################################
#
# I admit that this is a perfectly horrible Makefile, but I really don't
# know how to make a good one.  If someone would be so kind as to do so,
# I'll be very grateful.  Find my email address in the README and contact
# me if you're interested.
#
# Current targets: all mp3html dos install uninstall clean
#
##########################################################################



#
# Makefile variables
#

CC = gcc
CFLAGS = -s -O2 -Wall -ansi -pedantic
INSTALL = `which install`
DEST = /usr/local/bin
MANDEST = /usr/local/man/man1
BINARY = mp3html
SRC_FILES = header.c llist.c misc.c mp3html.c mp3html.h subst.c wrappers.c libid3/libid3.c libid3/libid3.h macro.c




#
# Targets
#

all:
	$(MAKE) mp3html

mp3html: $(SRC_FILES)
	$(CC) $(CFLAGS) \
	  mp3html.c  -o mp3html

dos: $(SRC_FILES)
	$(CC) $(CFLAGS) -DMSDOS \
	    mp3html.c -o mp3html.exe

install: mp3html mp3html.1
	$(INSTALL) $(BINARY) $(DEST)/$(BINARY)
	$(INSTALL) mp3html.1 $(MANDEST)/mp3html.1

uninstall: $(DEST)/$(BINARY)
	rm -f $(DEST)/$(BINARY)

clean:
	rm -f *.o core $(BINARY)
