#!/usr/bin/env python
from ConfigParser import SafeConfigParser
from os import path
import os
import sys

if sys.version_info[0:3] < (2,3,0):
	sys.stderr.write('required python with version >2.4\n')
	sys.exit(4)

CONF_FILENAME = 'pysync.ini'

class ConfException(Exception):
	pass
import UserDict
class ConfDict(UserDict.IterableUserDict):
	def __getitem__(self,key):
		try:
			val = self.data[key].strip()
			if val[0]=="'" and val[-1]=="'":
				return val[1:-1]
			elif val[0]=='"' and val[-1]=='"':
				return val[1:-1]
			else:
				return val
		except:
			raise ConfException('required config "%s", but not found'%(key,))

_conf = None

def _readconf(fname=None):
	global _conf
	if not fname:
		fname = path.join(path.dirname(path.abspath(sys.argv[0])),CONF_FILENAME)
	_conf = SafeConfigParser()
	_conf.read([fname])

def cget(domain='DEFAULT'):
	if not _conf:
		_readconf()
	d = ConfDict(dict(_conf.items(domain)))
	if(domain == 'DEFAULT'): return d
	for k,v in _conf.items('DEFAULT'):
		if not d.has_key(k):
			d[k]=v
	return d

def cget_sections():
	if not _conf:
		_readconf()
	return _conf.sections()

import popen2 
import select
import fcntl
def _set_nonblocking(fd):
	"""Make a fd non-blocking."""
	flags = fcntl.fcntl(fd, fcntl.F_GETFL)
	flags = flags | os.O_NONBLOCK
	fcntl.fcntl(fd, fcntl.F_SETFL, flags)


def popen(args):
	p = popen2.Popen3( args,True,8012)
	iwtd = {p.fromchild:[],p.childerr:[]}
	iwtd2 = {p.fromchild:"",p.childerr:""}
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
	
	rc = iwtd2[p.fromchild]
	ec = iwtd2[p.childerr]
	code = os.WEXITSTATUS(p.wait())
	p.fromchild.close()
	p.childerr.close()
	p.tochild.close()
	return code,rc,ec
	

import logging
def init_log():
	dc = cget()
	filename = dc['log_file']
	level = dc['log_level'].upper()
	if(hasattr(logging,level)):
		rootlog = logging.getLogger('')
		format = logging.Formatter(path.basename(sys.argv[0])+'(%(process)d) %(asctime)s %(name)s %(levelname)s %(message)s')
		logfile = logging.FileHandler(filename,'a')
		logfile.setFormatter(format)
		rootlog.setLevel(getattr(logging,level))
		rootlog.addHandler(logfile)
	else:
		raise ConfException('no such log_level '+level)

OP_LOCAL_RSYNC = 'local_rsync'
OP_REMOTE_RSYNC = 'remote_rsync'
OP_REMOTE_USER = 'remote_user'
OP_IDENTIFY_FILE = 'identify_file'
OP_SYNC_SRC = 'sync_src'
OP_SYNC_DST = 'sync_dst'
OP_VERBOSE_SYNC = 'verbose_sync'
OP_LOG_LEVEL = 'log_level'
OP_LOG_FILE = 'log_file'
OP_USE_CHECKSUM = 'use_checksum'
OP_WORKING_DIR = 'working_dir'
OP_TIMEOUT = 'timeout'

DEFAULT_CMD = [ '-rlt', '-z' ]

def build_command(job):
	c = cget(job)
	com = []
	if c.has_key(OP_LOCAL_RSYNC) :
		com.append(c[OP_LOCAL_RSYNC])
	else:
		com.append('rsync')
	if c.has_key(OP_REMOTE_RSYNC):
		com.append('--rsync-path='+c[OP_REMOTE_RSYNC])
	if c.has_key(OP_TIMEOUT):
		com.append('--timeout='+c[OP_TIMEOUT])
	com.append('-e')
	com.append('ssh -i '+c[OP_IDENTIFY_FILE])
	if c[OP_USE_CHECKSUM]=='yes':
		com.append('-c')
	if c[OP_VERBOSE_SYNC]=='yes':
		com.append('-v')
	com = com + DEFAULT_CMD
	src = c[OP_SYNC_SRC]
	dst = c[OP_SYNC_DST]
	if src.find(':')>-1 :
		src = c[OP_REMOTE_USER] + '@' + src
	elif dst.find(':')>-1 :
		dst = c[OP_REMOTE_USER] + '@' + dst
	else:
		raise ConfException('none of sync_src or sync_dst contains remote address')
	com.append(src)
	com.append(dst)
	return com

def run_jobs():
	for job in cget_sections():
		try:
			logging.info("start job %s",job)
			com = build_command(job)
			logging.debug("run command: %s",str(com))
			rcode, out_str, err_str = popen(com)
			if rcode != 0:
				logging.error("rsync return code: %d",rcode)
			if len(out_str) >0 :
				logging.info('rsync stdout:\n %s',out_str)
			if len(err_str) >0 :
				logging.error('rsync stderr:\n %s',err_str)
			logging.info("end job %s",job)
		except Exception,e:
			logging.exception(e)


def init():
	c  = cget()
	os.chdir(c[OP_WORKING_DIR])
	os.umask(0077)
	init_log()

def final():
	logging.shutdown()

def main():
	if len(sys.argv)!=2 :
		sys.stderr.write("usage: %s config_file\n"%(sys.argv[0],))
		return 1
	try:
		_readconf(sys.argv[1])
	except:
		sys.stderr.write("can't read config file at %s\n"%(sys.argv[1],))
		return 2
	try:
		init()
		run_jobs()
	except Exception,e:
		logging.exception(e)
		final()
		return 3

	final()
	return 0

if __name__ == '__main__':
	sys.exit(main())
