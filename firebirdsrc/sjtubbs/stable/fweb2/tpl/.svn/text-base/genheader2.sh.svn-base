#!/bin/sh

FILENAME=$1

BASENAME=`basename $FILENAME`
PREFILENAME=`echo $BASENAME|sed 's/\.tpl//'`
TMPFILE=${BASENAME}.tmp
OUTFILE=$2

#echo  checking $BASENAME 
if make_tpl_varnames_h -q -s ".tmp" $FILENAME
then
    if grep 'google/template_string.h' ${TMPFILE} >/dev/null 2>&1 # check if new header file format
    then
        OP1='s/^static.\+"\([[:graph:]]\+\)".\+$/static const char * const k_'
        OP2='_\1 = "\1";/'
        sed -e '/google\/template_string.h/d' \
            -e "${OP1}${PREFILENAME}${OP2}" \
            ${TMPFILE} >${OUTFILE}
    else
        sed -e 's/^const/static const/' \
            -e "s/const\sk[0-9a-zA-Z]_/const k_${PREFILENAME}_/" \
            ${TMPFILE} >${OUTFILE}
    fi
	rm ${TMPFILE}
	#echo "OK, generated ${BASENAME}.h"
else
	#echo "FAILED"
	exit 1
fi

