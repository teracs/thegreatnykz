# Generated automatically from Makefile.in by configure.
#  Makefile for local utility tool kit
# 
#  $Id: Makefile,v 1.2 2004/06/29 14:59:23 rwzmm Exp $
#

CC       = gcc
LIBS     = -ltermcap -ldl -lrpcsvc -lbsd -lnsl -lcrypt
CFLAGS   = -O2 -Wunused -I../include -I../../include 
BBSHOME	 = /home/bbs/bbshome

UTILITY  = mergeboard sortdir showbm

.c.o:   ;   $(CC) $(CFLAGS) -c $*.c

all: $(UTILITY)

install: $(UTILITY)
	cp $(UTILITY) $(BBSHOME)/bin

clean:
	rm -fr *.o *.BAK *.log $(UTILITY) *core* DEAD* *~
