# Generated automatically from Makefile.in by configure.
# Generated automatically from Makefile.in by configure.
BBSHOME=/home/bbs/bbshome

CC	= gcc
CFLAGS	= -O
DEFINES	= -I../../../include/

PIP_C =  pip_file.c      pip_menu.c      pip_practice.c \
pip_basic.c     pip_for_fb.c    pip_other.c     pip_special.c \
pip_ending.c    pip_job.c       pip_pic.c       pip_store.c   \
pip_fight.c     pip_main.c      pip_play.c      pip_system.c

PIP_OBJ = pip_file.o      pip_menu.o      pip_practice.o \
pip_basic.o     pip_for_fb.o    pip_other.o     pip_special.o \
pip_ending.o    pip_job.o       pip_pic.o       pip_store.o   \
pip_fight.o     pip_main.o      pip_play.o      pip_system.o

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -c $*.c

pip.so: $(PIP_OBJ)
	ld -s -L../../../lib -lBBS -G $(PIP_OBJ) -o pip.so

install: pip.so
	cp pip.so $(BBSHOME)/so

clean:
	-rm -f *~ *.o DEADJOE *.so
 
