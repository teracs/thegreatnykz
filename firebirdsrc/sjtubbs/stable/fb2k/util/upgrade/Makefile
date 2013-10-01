# Generated automatically from Makefile.in by configure.
#  Makefile for local utility tool kit
# 
#  $Id: Makefile.in,v 1.2 2004/06/29 14:59:22 rwzmm Exp $
#

CC       = gcc
CFLAGS   = -O2 -Wunused -I../include -I../../include
BBSHOME	 = /home/bbs/bbshome

UTILITY  = reboard rehome

.c.o:   ;   $(CC) $(CFLAGS) -c $*.c

all: $(UTILITY)

install: $(UTILITY)
	cp $(UTILITY) $(BBSHOME)/bin

clean:
	rm -fr *.o *.BAK *.log $(UTILITY) *core* DEAD* *~

