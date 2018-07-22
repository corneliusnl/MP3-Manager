#
# You shouldn't need to modify this file.
#

VERSION = 0.3.2
SOURCEDIR = ./src/
EXTRALIBS = -lpq -lcrypt
CPP	 = g++
CFLAGS	 = -O6 -Wall -Wstrict-prototypes -fomit-frame-pointer

PLUGINS  = $(SOURCEDIR)db_pgsql.o $(SOURCEDIR)db_sql.o $(SOURCEDIR)db_raw.o
OBJFILES = $(SOURCEDIR)main.o $(SOURCEDIR)mp3scan.o $(SOURCEDIR)stacker.o \
	   $(SOURCEDIR)cmdline.o ./argtable/argtable.o $(PLUGINS)


all	: $(OBJFILES)
	  $(CPP) $(CFLAGS) $(OBJFILES) $(EXTRALIBS) -o mp3db
	  strip ./mp3db
clean	:
	  rm -f $(OBJFILES) ./mp3db
