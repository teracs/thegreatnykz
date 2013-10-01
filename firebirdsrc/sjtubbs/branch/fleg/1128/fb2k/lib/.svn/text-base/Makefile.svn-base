#
#   lib/Makefile
#
#	$Id: Makefile,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
#
#############################################################
####   Setup the following site-specific information     ####
#############################################################

DIRS += libBBS

#############################################################
###        You needn't modify the following stuff      ######
#############################################################

all: rmlib
	@for x in $(DIRS); \
	do \
		echo "  --> lib/$$x"; \
		(cd $$x; make all CC=$(CC); cd .. ); \
		echo "  <-- lib/$$x"; \
		echo " "; \
	done

rmlib:
	-rm -rf *.a

clean: /tmp
	@echo "== cleanning junks, object files, and binaries .. =="
	-rm -fr *~ *.o *.BAK
	@for x in $(DIRS); \
	do \
		echo "  --> lib/$$x"; \
		(cd $$x; make clean ; cd .. ); \
		echo "  <-- lib/$$x"; \
		echo " "; \
	done

cleanall: /tmp
	@echo "== cleanning junks, object files, and binaries .. =="
	-rm -fr *~ *.o *.BAK
	@for x in $(DIRS); \
	do \
		echo "  --> lib/$$x"; \
		(cd $$x; make cleanall ; cd .. ); \
		echo "  <-- lib/$$x"; \
		echo " "; \
	done

install:
	@echo --- lib: no install skip ---
