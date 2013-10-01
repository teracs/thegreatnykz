# Makefile for FB2000 series
# $Id: Makefile,v 1.2 2004/06/29 14:59:23 rwzmm Exp $

all:
	@echo " "
	@echo "please read 00README first before upgrade!"
	@echo "[1;31mÇëÏÈ×ÐÏ¸ÔÄ¶Á 00README ÎÄ¼þ£¬ÒÔÈ·±£Éý¼¶ÎÞÎó£¡[m"
	@echo "check out the 00README if your version is out of list"
	@echo " "
	@echo "please type 'make versionid', "
	@echo " "
	@echo "make ph4     : upgrade from phoenix 4.0 to FB2000"
	@echo "make 2.0     : upgrade from 2.0 (CCU) to FB2000"
	@echo "make 2.66m   : upgrade from 2.66M to to FB2000"
	@echo "make 2.62m   : upgrade from 2.62M to to FB2000"
	@echo "make 2.5     : upgrade from 2.5 (CCU) to FB2000"
	@echo "make 3.0     : upgrade from 3.0 SNAP to FB2000"
	@echo "make 3.0K    : upgrade from 3.0 KCN to FB2000"
	@echo "make Maple2  : upgrade from Maple to FB2000"
	@echo "make unknown : upgrade from unknown version (read 00README first.)"
	@echo " "
	@echo " "
	
clean:
	rm -fr *.o *~ DEAD* repass chboard rehome refriend *core*

2.0:
	@echo "You may put your PH4 struct.h into include/struct.PH4.h, then"
	@echo "you may enter this translation procedure."
	@echo -n "press any key to continue."
	@read xx
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'PH4'\" -o repass repass.c -lBBS
	$(CC) -g $(CFLAGS) -I./include -I../../include -o rehome rehome.c
	$(CC) -g $(CFLAGS) -I../../include -o chboard chboard.c ../../src/record.c

3.0:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'3.0SNAP'\" -o repass repass.c -lBBS
	./repass 3.0SNAP

3.0K:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'3.0KCN'\" -o repass repass.c -lBBS
	./repass 3.0KCN

2.66m:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'2.66M'\" -o repass repass.c -lBBS
	./repass 2.66M

2.62m:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'2.62M'\" -o repass repass.c -lBBS
	./repass 2.62M
	$(CC) -g $(CFLAGS) -I./include -I../../include \
	-o refriend refriend.c ../../src/record.c
	./refriend

Maple2:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-D_MAPLE_2 -DSTRUCT_VERSION=\"'M2'\" -o repass repass.c -lBBS
	./repass Maple2

2.5:
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'2.5'\" -o repass repass.c -lBBS
	./repass 2.5
	@echo -n "now will translate boards flag .."
	@read xx
	$(CC) -g $(CFLAGS) -I../../include -o chboard chboard.c ../../src/record.c
	./chboard
	$(CC) -g $(CFLAGS) -DPH4_CCU -I./include -I../../include \
	-o refriend refriend.c ../../src/record.c
	./refriend
	@echo "done. .PASSWDS.new is transalted password file."

unknown:
	@echo "**warning** this section will tranaslate to 3.0 from unknown version"
	@echo "you must read 00README before."
	@read xx
	$(CC) -g $(CFLAGS) -I./include -I../../include -L../../lib \
	-DSTRUCT_VERSION=\"'unknown'\" -o repass repass.c -lBBS
	./repass unknown
