#############################################################
####   Setup the following site-specific information     ####
#############################################################

# The home directory for user 'bbs'
BBSHOME=$(BBS_HOME)

# The uid/gid info for user 'bbs' and 'bbsadm'
BBSUID=$(BBS_UID)
BBSGRP=$(BBSGID)

OS_DEF   = $(OS_DEF)
CC       = $(CC)
CFLAGS   = $(CFLAGS)
LIBS     = $(LIBS)

INSTALL  = $(INSTALL)

CSIE_DEF = $(CSIE_DEF)

#############################################################
###        You needn't modify the following stuff      ######
#############################################################
PROGNAME = bbsd
CFILE	 = admintool.c announce.c bbs.c bbsd.c bbsgopher.c bcache.c bm.c \
	   boards.c chat.c comm_lists.c delete.c edit.c fileshm.c goodbye.c \
	   help.c io.c list.c mail.c main.c maintain.c modetype.c more.c \
	   namecomplete.c pass.c postheader.c read.c record.c \
	   register.c screen.c sendmsg.c stuff.c talk.c term.c \
	   userinfo.c vote.c xyz.c readabc.c friendtest.c userstat.c \
	   juhuireg.c

COBJS	 = admintool.o announce.o bbs.o bbsd.o bbsgopher.o bcache.o bm.o \
	   boards.o chat.o comm_lists.o delete.o edit.o fileshm.o goodbye.o \
	   help.o io.o list.o mail.o main.o maintain.o modetype.o more.o \
	   namecomplete.o pass.o postheader.o read.o record.o \
	   register.o screen.o sendmsg.o stuff.o talk.o term.o \
	   userinfo.o vote.o xyz.o readabc.o friendtest.o userstat.o \
	   juhuireg.o

GARBAGE  = a.out core installchatd chatd bbsrf thread *~ *.BAK

# The -DINVISIBLE makes bbs users invisible to 'finger' and 'who'.
# Simply remove it if you don't want this feature.

DEFINES	=  $(CSIE_DEF) $(OS_DEF)
EXTRADEF = -DBACKUP_RANGE_DELETE -DINCLUDE_FBFUNC_H

.SUFFIXES: .o .c

.c.o:	;$(CC)  $(CFLAGS) $(DEFINES) $(EXTRADEF) -c $*.c
#.o.so:	;ld -s -G $*.o -o $*.so -L../lib -lBBS

#--------------------- Dependency starts here -------------------
all:  $(PROGNAME) chatd bbsnet expire thread sortdir idadmin CleanID

$(PROGNAME): $(COBJS)
	$(CC) -o $(PROGNAME) $(CFLAGS) $(CSIE_DEF) $(COBJS) $(LIBS) $(EXTRADEF) -L../lib -lBBS
	@echo "Program size: `cat $(PROGNAME)|wc -c` bytes"

chatd: station.c
	$(CC) $(CFLAGS) -o chatd $(OS_DEF) station.c $(LIBS) -L../lib -lBBS

thread: thread.c
	$(CC) $(CFLAGS) -o thread $(OS_DEF) thread.c $(LIBS)

sortdir: sortdir.c
	$(CC) $(CFLAGS) -o sortdir $(OS_DEF) sortdir.c $(LIBS)

CleanID: CleanID.c
	$(CC) $(CFLAGS) -o CleanID $(OS_DEF) CleanID.c $(LIBS) -L../lib -lBBS
idadmin: idadmin.c
	$(CC) $(CFLAGS) -o idadmin idadmin.c 

install:
	sh Install.sh

installbbs: bbsd
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) bbsd $(BBSHOME)/bin/bbsd.new
#	-cp bbsd $(BBSHOME)/bin/bbsd.new
	-rm -f $(BBSHOME)/bin/bbsd.old
	-mv $(BBSHOME)/bin/bbsd $(BBSHOME)/bin/bbsd.old
	mv $(BBSHOME)/bin/bbsd.new $(BBSHOME)/bin/bbsd
installbbsnet: bbsnet
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) bbsnet $(BBSHOME)/bin/bbsnet.new
	-rm -f $(BBSHOME)/bin/bbsnet.old
	-mv $(BBSHOME)/bin/bbsnet $(BBSHOME)/bin/bbsnet.old
	mv $(BBSHOME)/bin/bbsnet.new $(BBSHOME)/bin/bbsnet

installsortdir: sortdir
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) sortdir $(BBSHOME)/bin/sortdir.new
	-rm -f $(BBSHOME)/bin/sortdir.old
	-mv $(BBSHOME)/bin/sortdir $(BBSHOME)/bin/sortdir.old
	mv $(BBSHOME)/bin/sortdir.new $(BBSHOME)/bin/sortdir

installthread: thread
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) thread $(BBSHOME)/bin/thread.new
	-rm -f $(BBSHOME)/bin/thread.old
	-mv $(BBSHOME)/bin/thread $(BBSHOME)/bin/thread.old
	mv $(BBSHOME)/bin/thread.new $(BBSHOME)/bin/thread

installCleanID: CleanID
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) CleanID $(BBSHOME)/bin/CleanID.new
	-rm -f $(BBSHOME)/bin/CleanID.old
	-mv $(BBSHOME)/bin/CleanID $(BBSHOME)/bin/CleanID.old
	mv $(BBSHOME)/bin/CleanID.new $(BBSHOME)/bin/CleanID

installchatd: chatd
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) chatd $(BBSHOME)/bin/chatd.new
	-rm -f $(BBSHOME)/bin/chatd.old
	-mv $(BBSHOME)/bin/chatd $(BBSHOME)/bin/chatd.old
	mv $(BBSHOME)/bin/chatd.new $(BBSHOME)/bin/chatd
installidadmin: idadmin
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) idadmin $(BBSHOME)/bin/idadmin.new
	-rm -f $(BBSHOME)/bin/idadmin.old
	-mv $(BBSHOME)/bin/idadmin $(BBSHOME)/bin/idadmin.old
	-mv $(BBSHOME)/bin/idadmin.new $(BBSHOME)/bin/idadmin

clean: /tmp
	-rm -fr $(GARBAGE) $(SO) $(COBJS) $(PROGNAME) $(LNFILES) expire
	-rm -fr bbsnet sortdir

cleanall: clean
	-rm -f Install.sh
	-rm -f ../include/config.h
	-rm -f ../include/chat.h
	-rm -f bbs bbsrf chatd thread

tags: /tmp
	ctags $(CFILE)

update: installbbs installchatd installthread installexpire installbbsnet installsortdir installidadmin
#installCleanID

expire: expire.c record.c
	$(CC) $(CFLAGS) $(OS_DEF) -o expire expire.c

bbsnet: bbsnet.c
	$(CC) ${CFLAGS} ${LIBS} bbsnet.c -o bbsnet

modestat:
	$(CC) $(CFLAGS) -o modestat modestat.c

installexpire: expire
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) expire $(BBSHOME)/bin/expire.new
	-rm -f $(BBSHOME)/bin/expire.old
	-mv $(BBSHOME)/bin/expire $(BBSHOME)/bin/expire.old
	mv $(BBSHOME)/bin/expire.new $(BBSHOME)/bin/expire

# DO NOT DELETE
