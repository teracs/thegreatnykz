#!/usr/bin/env python
# -*- coding: gb2312  -*-
# this script calculate on-board posts top 100 users 
import struct
import time
import sys
import getopt
import os
from os import path
import heapq

BBSHOME = '/home/bbs/bbshome'
BOARD_DIR = BBSHOME+'/boards'
FILE_MARKED = 0x8
FILE_DIGEST = 0x10
FILE_NOREPLY = 0x40
DIGEST_DIR = '.DIGEST'
DOT_DIR = '.DIR'

FHSTRUCT = '80s76sI80sI4sII'
#struct fileheader
#{
#0      char filename[STRLEN];      /* the DIR files */
#1      char owner[STRLEN-4];
#2      unsigned int pnum;//the number of push articles
#3      char title[STRLEN];
#4      unsigned level;
#5      unsigned char accessed[4];
#6      unsigned int id;
#7      unsigned int reid;              //re id. add by rwzmm @ sjtubbs.
#};
filename = 0
owner = 1
pnum = 2
title = 3
level = 4
accessed = 5
id = 6
reid = 7

FHLEN = struct.calcsize(FHSTRUCT)

id2post={}
id2mpost={}
id2gpost={}

def iter_board(filename):
        f = open(filename,"r")

        sts = f.read(FHLEN)
        while(len(sts)==FHLEN):
                st = struct.unpack(FHSTRUCT,sts)
                yield st
                sts = f.read(FHLEN)
        f.close()

def count():
        for board in os.listdir(BOARD_DIR):
                filename = BOARD_DIR+"/"+board+"/"+DOT_DIR
                if path.isfile(filename):
                        for st in iter_board(filename):
                                if id2post.has_key(st[owner]):
                                        id2post[st[owner]] += 1
                                else:
                                        id2post[st[owner]] = 1
				if ord(st[accessed][0]) & FILE_MARKED:
					if id2mpost.has_key(st[owner]):
						id2mpost[st[owner]] += 1
					else:
						id2mpost[st[owner]] = 1
				if ord(st[accessed][0]) & FILE_DIGEST:
					if id2gpost.has_key(st[owner]):
						id2gpost[st[owner]] += 1
					else:
						id2gpost[st[owner]] = 1


def print_top(name,map):
        print name
        print ""
        print "%-14s%-20s%s"%("Sequence","Userid","Posts")
        print ""
        top_l = heapq.nlargest(100,[ (map[k],k) for k in map])
        count = 1
        for val,id in top_l:
                print "%-14d%-20s%d"% (count,id[:id.find('\0')],val)
                count += 1
        print ""
        print ""

if __name__ == "__main__":
        count()
        print_top("Posts in Board Top 100",id2post)
	print_top("Marked Posts in Board Top 100", id2mpost)
	print_top("Digest Posts in Board Top 100", id2gpost)
