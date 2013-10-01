#!/bin/sh

FILENAME=$1

BASENAME=`basename $FILENAME`
PREFILENAME=`echo $BASENAME|sed 's/\.tpl//'`

#echo  checking $BASENAME 
if make_tpl_varnames_h -q -s ".tmp" $FILENAME
then
	sed -e "/const\skf_header\s/d" -e "s/const\skf_/const kf_${PREFILENAME}_/" ${BASENAME}.tmp >${BASENAME}.h
	rm ${BASENAME}.tmp
	#echo "OK, generated ${BASENAME}.h"
else
	#echo "FAILED"
	exit 1
fi

