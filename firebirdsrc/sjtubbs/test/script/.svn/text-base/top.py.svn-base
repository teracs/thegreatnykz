#!/usr/bin/python
# -*- coding: gb2312  -*-
# this script calculate top 100 users

import struct
import sys
import heapq



if len(sys.argv) != 2 :
    print "Usage: ",sys.argv[0]," passwdfile"
    sys.exit(1)
USEREC="14si60s5Ii2s14s40s40s16s64sI3i40s80s68sIic3BiIii"

# struct userec
# {                 /* Structure used to hold information in */
#     char userid[IDLEN + 2];     /* PASSFILE */
#     time_t firstlogin;
#     char lasthost[60];
#     unsigned int numlogins;
#     unsigned int numposts;
#     unsigned int medals;        /* 奖章数 */
#     unsigned int money;         /* 金钱 */
#     unsigned int inbank;        /* 存款 */
#     time_t banktime;            /* 存入时间 */
#     char flags[2];
#     char passwd[PASSLEN];
#     char username[NAMELEN];
#     char ident[NAMELEN];
#     char termtype[16];
#     char reginfo[STRLEN - 16];
#     unsigned int userlevel;
#     time_t lastlogin;
#     time_t lastlogout;          /* 最近离线时间 */
#     time_t stay;
#     char realname[NAMELEN];
#     char address[STRLEN];
#     char email[STRLEN - 12];
#     unsigned int nummails;
#     time_t lastjustify;
#     char gender;
#     unsigned char birthyear;
#     unsigned char birthmonth;
#     unsigned char birthday;
#     int signature;
#     unsigned int userdefine;
#     time_t notedate;
#     int noteline;
# };

USEREC_len=struct.calcsize(USEREC)

userid=0
firstlogin=1
lasthost=2
numlogins=3
numposts=4
medals=5
money=6
inbank=7
banktime=8
flags=9
passwd=10
username=11
ident=12
termtype=13
reginfo=14
userlevel=15
lastlogin=16
lastlogout=17
stay=18
realname=19
address=20
email=21
nummails=22
lastjustify=23
gender=24
birthyear=25
birthmonth=26
birthday=27
signature=28
userdefine=29
notedate=30
noteline=31

def iter(func):
	old=open(sys.argv[1],"r")
	sts=old.read(USEREC_len)
	while len(sts) == USEREC_len :
		st =  struct.unpack(USEREC, sts)
		tu = func(st)
		if(tu):
			yield (tu,st)
		sts=old.read(USEREC_len)
	old.close()


import time
now = int(time.time())

def print_top(topname,name,func):
	print topname
	print ""
	print "%-14s%-20s%s"%("Sequence","Userid",name)
	print ""
	top_l = heapq.nlargest(100,iter(func))

	count=1
	for val,st in top_l:
		print "%-14d%-20s%.3f"% (count,st[userid][:st[userid].find('\0')],float(val))
		count+=1
	print ""
	print ""
if __name__ == "__main__":
	print_top("Post Top 100","Posts",lambda st: st[numposts])
	print_top("Login Top 100","Logins",lambda st: st[numlogins])
	print_top("BBS Age Top 100","Days",lambda st: st[firstlogin]>0 and (now-st[firstlogin])/3600.0/24 or False)
	print_top("Online Time Top 100","Days",lambda st: st[stay]/3600.0/24)
	print_top("Posts per Login Top 100 (Logins>=100)","Posts/Login",lambda st: st[numlogins]>99 and st[numposts]*1.0/st[numlogins] or False)
	print_top("Posts per Login Top 100 (Logins>=30)","Posts/Login",lambda st: st[numlogins]>29 and st[numposts]*1.0/st[numlogins] or False)
	print_top("Logins per Day Top 100 (Days>=30) ", "Logins/Day",lambda st: (now-st[firstlogin])>(3600*24*30-1) and st[numlogins]*3600.0*24/(now-st[firstlogin]) or False)
	print_top("Posts per Day Top 100 (Days>=30) ", "Posts/Day",lambda st: (now-st[firstlogin])>(3600*24*30-1) and st[numposts]*3600.0*24/(now-st[firstlogin]) or False)
	print_top("Posts per Online Day Top 100 (Logins>=100) ", "Posts/Day",lambda st: (st[stay]>0 and st[numlogins]>=100) and st[numposts]*3600.0*24/st[stay] or False )
	print_top("Online Time per Day Top 100 (Days>=30)","Hours/Day",lambda st: (now-st[firstlogin])>(3600*24*30-1) and st[stay]*24.0/(now-st[firstlogin]) or False )


