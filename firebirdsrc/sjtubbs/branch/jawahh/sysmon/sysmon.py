#!/usr/bin/env python
from ConfigParser import SafeConfigParser
from os import path
import os
import sys
import monitor

if sys.version_info[0:3] < (2,3,0):
	sys.stderr.write('required python with version >=2.3\n')
	sys.exit(4)

CONF_FILENAME = 'pysysmon.ini'

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


import time
def str2time(str):
	return time.mktime(time.strptime(str,"%Y-%m-%d/%H:%M:%S"))

def time2str(t=None):
	if t is None:
		t = time.time()
	return time.strftime("%Y-%m-%d/%H:%M:%S",time.localtime(t))


OP_HOST_MON_LOG_DIR='host_monitor_log_dir'
OP_REMOTE_MON_LOG_DIR='remote_monitor_log_dir'
OP_REMOTE_WORKING_DIR='remote_working_dir'
OP_HTML_DIR = 'html_dir'
OP_MONITORS='monitors'
OP_LOGFILE='log_file'
OP_LOGLVL='log_level'


MONLOG_PREFIX=".monitor.log."
def get_monlog_name(hostname,logtime):
	ts = time.localtime(logtime)
	return  hostname+MONLOG_PREFIX+time.strftime("%Y-%m-%d",ts)

monlog = None
def init_monlog(hostname):
	global monlog
	dc = cget()
	#remove monitor log file which is used seven days ago
	now = time.time()
	old_monlog_path = get_monlog_name(hostname,now-3600*24*7)
	if path.isfile(old_monlog_path):
		os.unlink(old_monlog_path)
	#open monitor log used by now
	monlog_path = get_monlog_name(hostname,now)
	monlog = file(monlog_path,"a+")

def final_monlog():
	global monlog
	monlog.close()
	monlog= None

def write_monlog(monlist):
	strlist = [ str(i) for i in monlist]
	strlist.insert(0,time2str())
	monlog.write( "#\t#".join(strlist) )
	monlog.write("\n")
	monlog.flush()


db = None
DB_NAME = 'monlog.db'
	
def init():
	c  = cget()
	os.chdir(c[OP_REMOTE_WORKING_DIR])
	os.umask(0077)
	init_log()
	import dbm
	global db
	db = dbm.open(DB_NAME,'c')

def final():
	logging.shutdown()
	if db:
		db.close()

HTML_HEAD = """<html>
<head>
</head>
<body>
<br/><br/><br/>
"""
HTML_IMG = """<img src="%s"></img><br/><br/><br/>"""
HTML_END = """</body>
</html>
"""
def generate_html(hostname,imgs):
	host_conf = cget(hostname)
	htmlpath = path.join(host_conf[OP_REMOTE_MON_LOG_DIR],"%s.html" % hostname)
	logging.info("generate %s" % htmlpath)
	html = open(htmlpath,"w+")
	html.write(HTML_HEAD)
	for img in imgs:
		html.write( HTML_IMG % img )
	html.write(HTML_END)
	html.close()

def parse_log(hostname):
	host_conf = cget(hostname)
	if not path.isdir(host_conf[OP_REMOTE_MON_LOG_DIR]):
		raise Exception("can't access '%s'" % host_conf[OP_REMOTE_MON_LOG_DIR])
	monitors = {}

	def parse_log_line(line):
		pieces = line.split('#\t#')
		time = str2time(pieces[0])
		platform = pieces[1]
		type = pieces[2]
		args = pieces[3:]
		if len(monitors)==0:
			for t in [ i.strip() for i in host_conf[OP_MONITORS].split(';')]:
				monitors[t] = \
				monitor.get_monitor_by_platform_and_type(platform,t,hostname,host_conf)
		if type in monitors:
			mon = monitors[type]
			mon.update_args(time,args)
	
	now = time.time()
	logfiles = [  get_monlog_name(hostname,now-3600*24*i) for i in range(8)]

	if  db.has_key(logfiles[-1]):
		del db[logfiles[-1]]

	del logfiles[-1]

	for logfile in logfiles:
		fpath = path.join( host_conf[OP_REMOTE_MON_LOG_DIR],logfile )
		if path.isfile(fpath):
			logging.debug("parse logfile %s" % fpath)
			lines = open(fpath).readlines()

			if db.has_key(logfile):
				linenum = int(db[logfile])
			else:
				linenum = 0
			
			if len(lines) > linenum:
				for line in lines[linenum:]:
					if line[-1] == '\n':
						if line[-2] == '\r':
							line = line[:-2]
						else:
							line = line[:-1]
					parse_log_line(line)
					linenum += 1
					db[logfile] = str(linenum)
	
	imgs = []
	for k in monitors:
		imgs.extend(monitors[k].update_and_get_graphs())
		monitors[k].close()

	if len(imgs) >0 :	
		generate_html(hostname,imgs)


def run():
	for hostname in  cget_sections():
		try:
			parse_log(hostname)
		except Exception,e:
			logging.exception(e)
		
	

def host_init(hostname):
	c = cget(hostname)
	os.chdir(c[OP_HOST_MON_LOG_DIR])
	init_log()
	init_monlog(hostname)
	os.umask(0077)
	monitor.check_myplatform()

def host_final():
	final_monlog()
	logging.shutdown()

def host_run(hostname):
	c = cget(hostname)
	for type in [ i.strip() for i in c[OP_MONITORS].split(';') ] :
		try:
			platform,mon = monitor.get_platform_and_monitor_by_type(type,hostname,c)
			for rs in mon.monitor():
				l = [platform,type]
				l.extend(rs) 
				write_monlog( l )
			mon.close()
		except monitor.MonitorException, e:
			logging.exception(e)

def main():
	if len(sys.argv)==2 :
		hostmode = False
		conf = sys.argv[1]
	elif len(sys.argv)==4 and sys.argv[1]=='-H':
		hostmode = True
		hostname = sys.argv[2]
		conf = sys.argv[3]
	else:
		sys.stderr.write("usage: %s [-H hostname] config_file\n"%(sys.argv[0],))
		return 1
	try:
		_readconf(conf)
	except:
		sys.stderr.write("can't read config file at %s\n"%conf)
		return 2
	
	if hostmode:
		try:
			host_init(hostname)
			host_run(hostname)
		except Exception,e:
			logging.exception(e)
			host_final()
			return 3
		host_final()
		return 0
	else:
		try:
			init()
			run()
		except Exception,e:
			logging.exception(e)
			final()
			return 3
		final()



if __name__ == '__main__':
	sys.exit(main())
