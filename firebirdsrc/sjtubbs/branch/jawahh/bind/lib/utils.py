import time
from bsddb import db as bdb
from bsddb import dbshelve
import os
from os import path
import conf
import db
import logging
import sys

def inttime():
	return int(time.time())

def ctime(t=None):
	if t == None:
		t = inttime()
	t = time.localtime(t)
	return time.strftime("%Y/%m/%d-%H:%M:%S",t)

def check_domain(name):
	"""The rules are 67 characters including the extension 
(e.g. .com, .net, .org, etc.). 
The domain name without the extension can be up to 63 characters.
Only 0-9, a-z and the dash "-" are valid characters."""
	if len(name) > 68 or name[-1] != '.':
		return False
	import string
	for c in name:
		if c not in string.ascii_letters + string.digits + '-.':
			return False
	return True


def getage(start_time):
	age = inttime() - start_time
	prefix = ''
	if age < 0:
		age = -age
		prefix = '-'
	sec = age % 60
	age = (age - sec) / 60
	min = age % 60
	age = (age - min) / 60
	hour = age % 24
	day = (age - hour) / 24
	return '%s%dd%02dh%02dm%02ds' % (prefix,day,hour,min,sec)


import subprocess
import select
import fcntl
def _set_nonblocking(fd):
	"""Make a fd non-blocking."""
	flags = fcntl.fcntl(fd, fcntl.F_GETFL)
	flags = flags | os.O_NONBLOCK
	fcntl.fcntl(fd, fcntl.F_SETFL, flags)

def popen(args):
	p = subprocess.Popen(\
		args,stdout=subprocess.PIPE,stderr=subprocess.PIPE,bufsize=8012)
	iwtd = {p.stdout:[],p.stderr:[]}
	iwtd2 = {p.stdout:"",p.stderr:""}
	for f in iwtd.keys():
		_set_nonblocking(f.fileno())
	r,w,e = select.select(iwtd.keys(),[],[])
	while True:
		for f in r:
			content = f.read()
			if content == "":
				iwtd2[f] = "".join(iwtd[f])
				del iwtd[f]
			else:
				iwtd[f].append(content)
		if len(iwtd) == 0:
			break
		r,w,e = select.select(iwtd.keys(),[],[])
	
	rc = iwtd2[p.stdout]
	ec = iwtd2[p.stderr]
	code = p.wait()
	p.stdout.close()
	p.stderr.close()
	return code,rc,ec
	


def opendb(filename,to_create=False):
	flag = bdb.DB_THREAD
	if to_create: 
		flag = flag | bdb.DB_CREATE
	return dbshelve.open(filename,flag)

def init_log():
	dc = conf.get()
	filename = dc['log_file']
	level = dc['log_level'].upper()
	if(hasattr(logging,level)):
		logging.basicConfig(level=getattr(logging,level),\
			format = path.basename(sys.argv[0]) + \
			'(%(process)d) %(asctime)s %(name)s %(levelname)s %(message)s',\
			filename=filename,\
			filemode='a')
	else:
		raise conf.ConfException('no such log_level '+level)

def init():
	c  = conf.get()
	os.chdir(c['working_dir'])
	os.umask(0077)
	init_log()

def final():
	logging.shutdown()
	db.close_alldb()


