BBSHOME = /home/bbs/bbshome
CGIPATH = $(BBSHOME)/httpd
HTMPATH = $(BBSHOME)/httpd/html
CC	= gcc -O2 -I/home/bbs/bbssrc/fb2k/include/ 
LIB	= -lgdbm
LIBS	= kernel.a
K_OBJS	= kernel.o crypt.o boardrc.o
OBJS	= httpd.o bbsusr.o bbstop10.o bbssec.o bbsdoc.o \
	  bbscon.o bbstopcon.o bbsbrdadd.o bbsboa.o bbsall.o bbsanc.o bbs0an.o \
          bbslogout.o bbsleft.o bbslogin.o bbsqry.o bbsnot.o bbsfind.o \
          bbsfadd.o bbsfdel.o bbsfall.o bbsfriend.o bbsfoot.o bbsform.o bbspwd.o bbsplan.o bbsinfo.o \
          bbsmybrd.o bbssig.o bbspst.o bbsgcon.o bbsgdoc.o bbsdel.o bbsdelmail.o bbsmailcon.o bbsmail.o \
          bbsdelmsg.o bbssnd.o bbsmovie.o bbsalluser.o bbsnotepad.o bbsmsg.o bbssendmsg.o bbsreg.o \
          bbsmailmsg.o bbssndmail.o bbsnewmail.o bbspstmail.o bbsgetmsg.o bbssel.o bbscloak.o main.o bbsmain.o\
          bbsbrddel.o bbsmdoc.o bbsbfind.o bbsnick.o bbstfind.o bbstcon.o bbstdoc.o \
          bbsdoreg.o bbsmywww.o bbsccc.o bbsufind.o bbsclear.o bbsstat.o bbsedit.o bbsman.o bbsman2.o bbsparm.o \
          bbsupload2.o bbsupload.o bbsfwd.o bbsmnote.o bbsmnote3.o bbsdenyall.o bbsdenydel.o bbsdenyadd.o \
          bbstopb10.o bbsfexp.o bbsblockmail.o bbsallf.o bbsfdoc.o showfile.o bbsudel.o \
          showinfo.o top10.o bbstfind0.o bbsdoupload.o bbsfind2.o bbsafind.o \
	  bbsfexp2.o bbsfdoc2.o bbsudel2.o bbsfddoc.o bbsurdel.o bbsrss.o bbsvcode.o
ALL	= sjtuwebbsd

all: $(ALL)

kernel.a: $(K_OBJS)
	ar -rcs kernel.a $(K_OBJS)
	chmod +x kernel.a

sjtuwebbsd: $(LIBS) $(OBJS)
	$(CC) -o sjtuwebbsd $(OBJS) $(LIBS) $(LIB)
	strip sjtuwebbsd
	@echo "File size:`cat sjtuwebbsd | wc -c` bytes"

update: all
	rm $(CGIPATH)/$(ALL)
	cp $(ALL) $(CGIPATH)
	#chown bbs.bbs $(CGIPATH) -R

install: all
	rm $(CGIPATH)/$(ALL)
	cp $(ALL) $(CGIPATH)
	mkdir $(HTMPATH)
	cp html/* $(HTMPATH)
	chmod u+w,a+r $(HTMPATH)/*
	mkdir $(CGIPATH)/upload
	touch $(CGIPATH)/upload/.DIR
	#chown bbs.bbs $(CGIPATH) -R

clean:
	rm -rf *.a *.o a.out core $(OBJS) $(K_OBJS) $(LIBS) $(ALL)
