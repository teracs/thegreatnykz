# Generated automatically from Makefile.in by configure.
#  Makefile for local utility tool kit
# 
#  $Id: Makefile.in,v 1.2 2004/06/29 14:59:22 rwzmm Exp $
#

CC       = gcc
CFLAGS   = -O2 -Wunused -I../include -I../../include
BBSHOME	 = /home/bbs/bbshome

UTILITY  = account averun bbstop bbspop3d birthday horoscope \
	   newacct poststat usage showuser bbsuptime AnnIndex \
	   badbms

.c.o:   ;   $(CC) $(CFLAGS) -c $*.c

all: $(UTILITY)

install: $(UTILITY)
	cp $(UTILITY) $(BBSHOME)/bin

clean:
	rm -fr *.o *.BAK *.log $(UTILITY) *core* DEAD* *~

bbspop3d: bbspop3d.c
	$(CC) $(CFLAGS) -o bbspop3d bbspop3d.c -lcrypt

usage: usage.c ../../src/record.c
	$(CC) $(CFLAGS) -o usage usage.c ../../src/record.c
