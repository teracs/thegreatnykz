# Generated automatically from Makefile.in by configure.
BBSHOME=/home/bbs/bbshome

CC      = gcc
CFLAGS  = -O
DEFINES = -I../../include/

CGAMES 	 = chicken.c game.c money.c others.c
OBJGAMES = chicken.o game.o money.o others.o

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -c $*.c

all: winmine game.so winmine2 tetris

winmine: winmine.c
	$(CC) -o winmine winmine.c
winmine2: winmine2.c
	$(CC) -o winmine2 winmine2.c
tetris: tetris.c
	$(CC) $(DEFINES) -o tetris tetris.c

game.so: $(OBJGAMES)
	ld -s -L../../lib -lBBS -G $(OBJGAMES) -o game.so

install: game.so winmine winmine2 tetris
	cp game.so $(BBSHOME)/so
	cp winmine $(BBSHOME)/so/winmine.new
	-mv $(BBSHOME)/so/winmine $(BBSHOME)/so/winmine.old
	mv $(BBSHOME)/so/winmine.new $(BBSHOME)/so/winmine
	cp winmine2 $(BBSHOME)/so/winmine2.new
	-mv $(BBSHOME)/so/winmine2 $(BBSHOME)/so/winmine2.old
	mv $(BBSHOME)/so/winmine2.new $(BBSHOME)/so/winmine2
	cp tetris $(BBSHOME)/so/tetris.new
	-mv $(BBSHOME)/so/tetris $(BBSHOME)/so/tetris.old
	mv $(BBSHOME)/so/tetris.new $(BBSHOME)/so/tetris



clean:
	-rm -f *~ *.o DEADJOE *.so winmine

