#!/bin/sh

BBSHOME=/home/bbs/bbshome
DATESTRING=`date +%YÄê%mÔÂ%dÈÕ`
DATAFILE=$BBSHOME/usies
OBJFILE=$BBSHOME/tmp/bbs.listkill

echo "[1;33m±¾ÈÕËæ·çÆ®ÊÅµÄID:[m "  > $OBJFILE
grep KILL $DATAFILE | grep $DATESTRING | cut -f 6 -d "x" >> $OBJFILE
