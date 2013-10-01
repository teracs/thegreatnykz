CC=gcc-3.3
OBJS=jbbs_wrap.o shm.o jbbs.o log.o
PROG=php_jbbs.so
CFLAGS=-fpic  -Wall -O2 -g
LDFLAGS=-shared
PHP_INC=`php-config --includes`
EXTRA_INC=-I../fb2k/include `pkg-config --cflags glib-2.0`
EXTRA_LIB=`pkg-config --libs glib-2.0`

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROG) $(EXTRA_LIB)

%.o: %.c
	$(CC) $(EXTRA_INC) $(PHP_INC) $(CFLAGS) -c $<
clean:
	-rm *.o $(PROG)
