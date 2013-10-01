#!/usr/bin/env python
# -*- coding: gb2312  -*-
import struct
import time
import sys
import getopt
import os
import os.path

BBSNAME = '饮水思源'
#BBSHOME = '/home/bbs/bbshome'
BBSHOME = '/tmp/'
BOARD_DIR = BBSHOME+'/boards'
DEFAULT_ID = 'SJTUBBS'
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
FHLEN = struct.calcsize(FHSTRUCT)


USAGE = """usage: %s [-h] [-m] [-g] [-n] [-o id] board title filename
        -h : print this help
        -m : mark the article
        -g : digest the article
        -n : no reply
        -o id : the owner id, default is %s
        board : the board name
        title : the title of the article
        filename : the file name where the content read from. use - if read from stdin
""" % (os.path.basename(sys.argv[0]),DEFAULT_ID)

owner = DEFAULT_ID
marked = False
digest = False
use_stdin = False
noreply  = False
board = ''
title = ''
filename = ''

perror = sys.stderr.write

def parse_opt():
        global owner,digest,marked,board,title,filename,use_stdin,noreply
        try:
                optlist,args = getopt.getopt(sys.argv[1:],'ghmno:')
        except getopt.GetoptError:
                perror(USAGE)
                sys.exit(1)
        for opt in optlist:
                if opt[0] == '-h':
                        perror(USAGE)
                        sys.exit(0)
                elif opt[0] =='-o':
                        owner = opt[1]
                elif opt[0] =='-g':
                        digest = True
                elif opt[0] =='-m':
                        marked = True
                elif opt[0] =='-n':
                        noreply = True
        if 3 != len(args):
                perror('not enough or too many args\n')
                perror(USAGE)
                sys.exit(1)
        board = args[0]
        title = args[1]
        filename = args[2]
        if filename == '-':
                use_stdin = True

def check_opt():
        if len(owner) > 13 or not owner.isalnum() :
                perror('invalid or too long id: %s\n' % (owner,))
                perror(USAGE)
                sys.exit(2)
        if not ( os.path.isdir('%s/%s'%(BOARD_DIR,board))\
                and os.access('%s/%s'%(BOARD_DIR,board),os.W_OK)):
                perror('invalid board: %s\n' % (board,))
                perror(USAGE)
                sys.exit(2)
        if not ( use_stdin or os.path.isfile(filename) ):
                perror('invalid filename: %s\n' %(filename,))
                perror(USAGE)
                sys.exit(2)
def con_fh(fh):
        fhl = []
        fhl.append(fh['filename'])
        fhl.append(fh['owner'])
        fhl.append(fh['pnum'])
        fhl.append(fh['title'])
        fhl.append(fh['level'])
        fhl.append(fh['accessed'])
        fhl.append(fh['id'])
        fhl.append(fh['reid'])
        str = struct.pack(FHSTRUCT,*fhl)
        return str
def append_fh(path,fhstr):
        fd = os.open(path,\
                os.O_WRONLY|os.O_APPEND|os.O_CREAT,0644)
        os.write(fd,fhstr)
        os.close(fd)
	
	
def open_article():
        now = int(time.time())
        while True:
                try:
                        fd = os.open('%s/%s/%d/M.%d.A'%(BOARD_DIR,board,now%500,now),\
                        os.O_CREAT|os.O_WRONLY|os.O_EXCL,0644)
                except OSError:
                        now += 10
                else:
                        break
        return now,os.fdopen(fd,'w')

def datestring():
        weeknum = ( "一", "二", "三", "四", "五", "六","天" )
        tm = time.localtime()
        return "%4d年%02d月%02d日%02d:%02d:%02d 星期%2s" \
                %( tm[0],tm[1],tm[2],tm[3],tm[4],tm[5],weeknum[tm[6]] )

def write_article(article):
        article.write("\033[1;41;33m发信人: %s (自动发信系统), 信区: %s%s\033[m\n"\
                % (owner,board,' '*(48-len(owner)-len(board))))
        if title > 71:
                article.write("标  题: %s\n" % title[:71])
        else:
                article.write("标  题: %s\n" % title)
        article.write("发信站: %s\033[33m自动发信系统\033[m(%s)\n\n" \
                % (BBSNAME,datestring()))
        if use_stdin:
                rfile = sys.stdin
        else:
                rfile = open(filename)
        for line in rfile:
                article.write(line)
        if not use_stdin:
                rfile.close()


def markit(fh):
        fh['accessed'] = chr(ord(fh['accessed'][0])|FILE_MARKED)+'\x00'*3

def noreplyit(fh):
        fh['accessed'] = chr(ord(fh['accessed'][0])|FILE_NOREPLY)+'\x00'*3

def digestit(fh):
        tmp_accesed = chr(ord(fh['accessed'][0])|FILE_DIGEST)+'\x00'*3
        tmp_filename = fh['filename']
        fh['accessed'] = '\x00'*4
        fh['filename'] = 'G.%d.A'%fh['id']
        str = con_fh(fh)
        append_fh('%s/%s/%s'%(BOARD_DIR,board,DIGEST_DIR),str)
        os.link('%s/%s/%s'%(BOARD_DIR,board,tmp_filename),\
                '%s/%s/%s'%(BOARD_DIR,board,fh['filename']))
        fh['accessed'] = tmp_accesed
        fh['filename'] = tmp_filename


def post():
        fh = {}
        fh['owner']= owner
        fh['filename'] = ''
        fh['pnum'] = 0
        if len(title) > 46:
                fh['title'] = title[:46]
        else:
                fh['title'] = title
        fh['level'] = 0
        fh['accessed'] = '\x00'*4
        fh['id'] = 0
        fh['reid'] = 0

        id,article = open_article()
        fh['id'] = id
        fh['reid'] = id
        fh['filename'] = 'M.%d.A'%id

        write_article(article)
        article.close()

        if marked: markit(fh)
        if noreply: noreplyit(fh)
        if digest: digestit(fh)

        str = con_fh(fh)
        append_fh('%s/%s/%s'%(BOARD_DIR,board,DOT_DIR),str)

if __name__ == '__main__':
        parse_opt()
        check_opt()
        post()
