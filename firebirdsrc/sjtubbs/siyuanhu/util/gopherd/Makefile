#$Id: Makefile,v 1.2 2004/06/29 14:59:22 rwzmm Exp $

CC        = gcc
BBSSRC    = ../../src
INCLUDE   = -I../../include

mCFLAGS   = $(CFLAGS) ${INCLUDE}

all: gopherd

install: gopherd
	cp gopherd $(BBSHOME)/bin
	chown root:bbs $(BBSHOME)/bin/gopherd
	chmod u+s $(BBSHOME)/bin/gopherd

clean:
	rm -fr *.o *.BAK *.log *~ gopherd

gopherd: gopherd.c
	$(CC) $(mCFLAGS) -o gopherd gopherd.c
