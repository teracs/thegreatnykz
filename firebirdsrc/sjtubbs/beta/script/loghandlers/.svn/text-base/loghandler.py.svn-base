#!/usr/bin/env python

# pylint: disable-msg=W0312, C0324, C0111

import sys
import os
import time
import signal
import imp
import logging
from glob import glob


OP_WORK_DIR= '/home/bbs/bbshome/reclog'
OP_ROTATE_TIME = 1800
# set to true to turn on debug 
OP_DEBUG = False

# application own log file name 
OP_LOGFILE = './loghandler.log'

# how many log files to monitor
OP_LOGNUM = 2

# paths to load handler
OP_HANDLER_DIR = ['/home/bbs/bbshome/bin/handlers']

# monitored log file format
OP_LOGNAME_FORMAT = 'log%d/%d'

# running flag, set to false if recieve signal term
F_RUNGING = True

# check python version >= 2.4
if sys.version_info < (2,4,0,'',0):
	sys.stderr.write('unsupported python version. plz use python2.4 and above\n') 
	sys.exit(1)

class LogBaseError(Exception):
	pass

class EmptyLineError(LogBaseError):
	pass

class LogMonitor(object):
	def _open_log(self,name):
		fd = os.open(name,os.O_RDWR|os.O_CREAT,0644)
		return os.fdopen(fd,'r')
	def _open_size(self,name):
		return file(name,'w')
	def __init__(self,path,sizepath):
		self._log = self._open_log(path)
		self._sizefile = self._open_size(sizepath)
		self._size = os.fstat(self._sizefile.fileno()).st_size
		self._line = ''
		# yes, I use the sizefile's size to record the log size that have been read
		tmpsize = os.fstat(self._log.fileno()).st_size
		if self._size > tmpsize:
			logging.warn\
			('detect some sync error at %s: recorded size > log file size',path)
			self._size = 0
			self._sizefile.truncate(self._size)
		else:
			self._log.seek(self._size)
		
	def check(self):
		tmpsize = os.fstat(self._log.fileno()).st_size
		if tmpsize > self._size:
			self._line += self._log.readline()
			self._size = self._log.tell()
			if len(self._line)>0 and self._line[-1] == '\n':
				return True
		return False
	
	def getline(self):
		if len(self._line) > 0:
			line = self._line
			self._line = ''
			return line
		else:
			raise EmptyLineError('got an empty line')

	def syncsize(self):
		self._sizefile.truncate(self._size)
	
	def truncate(self):
		if self._size > 0:
			self._log.seek(0)
			self._log.truncate(0)
			self._sizefile.truncate(0)
			self._size = 0
	
	def checkalive(self):
		return os.fstat(self._log.fileno()).st_nlink
	
	def finalize(self):
		self._sizefile.close()
		self._log.close()

class SameCommandError(LogBaseError):
	pass
class HandlerIntializeError(LogBaseError):
	pass

class Sleeper(object):
	def __init__(self,start,end,base):
		self._base = base
		self._sleep_start = int(end + base/20) % base
		self._sleep_end = int(start - base/20) % base
	def sleep(self):
		now = int(time.time())%self._base
		if now > self._sleep_start and now < self._sleep_end:
			time.sleep(int(self._base/40))
			return True
		else:
			time.sleep(1)
			return False
		
class LogDispatcher(object):
	def __init__(self,monitor,sleeper,handler_dirs):
		self._monitor = monitor
		self._sleeper = sleeper
		try:
			self._prepare_handlers(handler_dirs)
		except:
			self.finalize()
			raise

	def _load_handler(self,paths,name):
		try:
			minfo = imp.find_module(name,paths)
			h = imp.load_module(name,*minfo)
			return {'init': getattr(h,'initialize'),\
				'handle': getattr(h,'handle'),\
				'final': getattr(h,'finalize'),\
				'initialized':False}
		except ImportError,e:
			logging.error('can not load handler %s',name)
			raise LogBaseError(e)
		except AttributeError,e:
			logging.error\
			('can find enough functions in handler %s',name)
			raise LogBaseError(e)
	def _prepare_handlers(self,dirs):
		self._handlers = {}
		self._commands = {}
		for dir in dirs:
			if not os.path.isdir(dir): 
				continue
			for path in glob(os.path.join(dir,'handler_*.py')):
				name = os.path.basename(path)
				self._handlers[path] = \
				self._load_handler([dir],name[:-3])
		for name,handler in self._handlers.items():
			try:
				commands_list = handler['init']()
				handler['initialized'] = True
			except Exception,e:
				logging.error\
				('call init function failed at %s'%name)
				raise HandlerIntializeError(e)
			for cmd in commands_list:
				if cmd in self._commands:
					raise SameCommandError(\
					'command %s is registered twice time')
				self._commands[cmd] = handler
			logging.debug('load %s, commands: %s',name,str(commands_list))
	def finalize(self):
		if hasattr(self,'_handlers'):
			for name,handler in self._handlers.items():
				if not handler['initialized']: continue
				try:
					handler['final']()
				except:
					logging.exception\
					('call final function failed at %s'%name)
			del self._handlers
		if hasattr(self,'_commands'):
			del self._commands
		if hasattr(self,'_monitor'): 
			del self._monitor
		
	
	def start(self):
		while F_RUNGING:
			f_no_line = True
			while F_RUNGING and self._monitor.check():
				self._process_line(self._monitor.getline())
				f_no_line = False
				self._monitor.syncsize()
			if F_RUNGING and f_no_line:
				if self._sleeper.sleep():
					self._monitor.truncate()
		logging.info('recieve SIGTERM, exit')
	
	def _process_line(self,line):
		pieces = line.split(None,1)
		if len(pieces) != 2:
			logging.warn('invalid syntax: %s',line[:-1])
			return
		if pieces[0] not in self._commands:
			logging.warn('unknown command: %s',pieces[0])
			return
		try:
			self._commands[pieces[0]]['handle'](pieces[0],pieces[1][:-1])
		except:
			logging.error('unexcepted error while processing line')
			raise

class Child(object):
	def __init__(self,timeindex,logpath):
		assert timeindex == 0 or timeindex == 1
		self.logpath = logpath
		base = OP_ROTATE_TIME * 2
		if timeindex == 0:
			start = 0
			end = OP_ROTATE_TIME - 1
		elif timeindex == 1:
			start = OP_ROTATE_TIME
			end = OP_ROTATE_TIME * 2 -1
		self._sleeper = Sleeper(start,end,base)
		self._punitive_time = 0
		self.max = 64
		self.factor = 2
		self.duration = 300
	
	def fork(self):
		self._start_time = time.time()
		ret = os.fork()
		if ret > 0:
			self.pid = ret
			return
		try:
			self._sleep()
			self._child_run()
		except:
			logging.exception('child %d stopped unexceptedly:\n',os.getpid())
			if hasattr(self,'_dispatcher'):
				self._dispatcher.finalize()
			if hasattr(self,'_monitor'):
				self._monitor.finalize()
			os._exit(1)
		else:
			logging.info('child %d stopped',os.getpid())
			self._dispatcher.finalize()
			os._exit(0)

	def _child_run(self):
		self._monitor = LogMonitor(self.logpath,self.logpath+'size')
		self._dispatcher = LogDispatcher(self._monitor,self._sleeper,OP_HANDLER_DIR)
		self._dispatcher.start()

	def _sleep(self):
		if self._punitive_time > 0:
			time.sleep(self._punitive_time)
	
	def died(self):
		now = time.time()
		if self._start_time < now - self.duration:
			self._punitive_time = 0
		elif self._punitive_time == 0:
			self._punitive_time = 1
		else:
			self._punitive_time *= self.factor
			if self._punitive_time > self.max:
				self._punitive_time = self.max

def set_log(level):
	logging.basicConfig(level=level,
			format='%(asctime)s %(levelname)s %(message)s',
			filename=OP_LOGFILE,
			filemode='a')
def daemonize():
	if os.fork() > 0:
		sys.exit()
	os.setsid()
	signal.signal(signal.SIGTSTP,signal.SIG_IGN)
	signal.signal(signal.SIGTTOU,signal.SIG_IGN)
	signal.signal(signal.SIGTTIN,signal.SIG_IGN)
	signal.signal(signal.SIGHUP,signal.SIG_IGN)
	sys.stdin.close()
	sys.stdout.close()
	sys.stderr.close()

def sigterm_handler(signum,frame):
	global F_RUNGING
	F_RUNGING = False

def run():
	children = {}
	for i in range(OP_LOGNUM):
		for j in range(2):
			path = OP_LOGNAME_FORMAT % (j,i)
			child = Child(j,path)
			child.fork()
			children[child.pid] = child
			logging.info("start child %d for log %s",child.pid,path)
	try:
		while F_RUNGING:
			if len(children) == 0:
				logging.warn('all children exited')
				break
			try:
				pid,status = os.wait()
			except:
				pass
			if not F_RUNGING:
				break

			if os.WIFSIGNALED(status):
				logging.info\
				("child %d exited caused by signal %d",pid,os.WTERMSIG(status))
			else:
				logging.info\
				("child %d exited with status %d",pid,os.WEXITSTATUS(status))
			
			if os.WIFEXITED(status) and 0 == os.WEXITSTATUS(status):
			# child exited normally
				del children[pid]
			else:
				child = children[pid]
				del children[pid]
				child.died()
				child.fork()
				logging.info\
				("restart child %d for log %s",child.pid,child.logpath)
				children[child.pid] = child
	finally:
		try:
			for pid in children.keys():
				os.kill(pid,signal.SIGTERM)
		except:
			pass
			


def main():
	os.chdir(OP_WORK_DIR)
	if OP_DEBUG:
		set_log(logging.DEBUG)
	else:
		set_log(logging.INFO)
		daemonize()
	signal.signal(signal.SIGTERM,sigterm_handler)
	logging.info("main process started, pid: %d",os.getpid())
	try:
		run()
	except:
		logging.exception("process %d:\n",os.getpid())
		logging.error("main process %d unexpectedly",os.getpid())
	else:
		logging.info("main process stopped")

if __name__ == '__main__':
	main()
