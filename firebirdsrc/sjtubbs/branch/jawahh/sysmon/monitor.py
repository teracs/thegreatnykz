import os
import popen2 
import select
import fcntl
from os import path
import time
import logging

# try to import rrdtool,  if failed, then try rrdtoolmodule
# if failed again, stop trying, but you can only use Monitor->monitor method
# it was designed for requiring less module for monitor host
try:
	import rrdtool
except ImportError,e:
	try:
		import rrdtoolmodule as rrdtool
	except ImportError,e:
		pass


def run_command(args):

	def _set_nonblocking(fd):
		"""Make a fd non-blocking."""
		flags = fcntl.fcntl(fd, fcntl.F_GETFL)
		flags = flags | os.O_NONBLOCK
		fcntl.fcntl(fd, fcntl.F_SETFL, flags)

	def _split_line(lines):
		if len(lines)>0 and lines[-1]=='\n' :
			lines = lines[:-1]
		def nocr(line):
			if len(line)>0 and line[-1] == '\r':
				return line[:-1]
			else:
				return line
		return [ nocr(l) for l in lines.split('\n') ]

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
	
	code = os.WEXITSTATUS(p.wait())
	rc = iwtd2[p.fromchild]
	ec = iwtd2[p.childerr]

	p.fromchild.close()
	p.childerr.close()
	p.tochild.close()

	return code,_split_line(rc),ec



OP_MOUNT_POINTS = "mount_points"
OP_INTERFACES = "interfaces"
OP_RRD_DIR = 'rrd_dir'
OP_HTML_DIR = 'html_dir'

class MonitorException(Exception):
	pass


class Monitor(object):

	args_num = 0
	heartbeat = "600"
	step = "300"
	rows1 = "105408"
	rows2 = "87840"
	imgtype = 'PNG'
	period = 3600*24
	width= "500"
	height= "120"

	def __init__(self,type,hostname,host_conf):
		self.hostname = hostname
		self.type = type
		self.host_conf = host_conf
		self.imgs = []
	
	def update_args(self,time,args):
		self.args = args
		self.time = time

		
		self.rrd_path = path.join(self.host_conf[OP_RRD_DIR],self.get_unique_rrdname())
		self._rrd_create()
		self._rrd_update()
	
	def update_and_get_graphs(self):
		rrd_path = path.join(self.host_conf[OP_RRD_DIR],self.get_unique_rrdname())
		img_path = path.join(self.host_conf[OP_HTML_DIR],self.get_unique_imgname())
		if self._rrd_graph(img_path,rrd_path):
			return [self.get_unique_imgname()]
		else:
			return []

	def _rrd_graph(self,img_path,rrd_path):
		if not path.isfile(rrd_path):
			return False
		now = int(time.time())
		commands = [ 
			img_path,
			"--imgformat", self.imgtype,
			"--start",	str(now-self.period),
			"--end", str(now),
			"--width", self.width,
			"--height", self.height,
			"--slope-mode",
			"--font", "TITLE:10:",
			"--font", "AXIS:8:",
			"--font", "LEGEND:8:",
			"--font", "UNIT:8:",
			"COMMENT:From %s to %s\\c" % (
					time.strftime("%Y-%m-%d/%H\\:%M\\:%S",time.localtime(now-self.period)),
					time.strftime("%Y-%m-%d/%H\\:%M\\:%S",time.localtime(now)),
			),
			"COMMENT: \\n",
		]
		commands.extend(self.graph(rrd_path))
		logging.debug( str( commands) )
		rrdtool.graph(*commands)
		return True
	
	def _rrd_create(self):
		if not path.isfile(self.rrd_path):
			commands = [
				self.rrd_path,
				"--start","20000101",
				"--step",self.step
				]
			commands.extend(self.create())
			logging.debug( str( commands) )
			rrdtool.create(*commands)

	def _rrd_update(self):
		if rrdtool.last(self.rrd_path) >= self.time :
			return
		commands = [self.rrd_path]
		commands.extend(self.update())
		logging.debug( str( commands) )
		rrdtool.update(*commands)


	def _get_unique_prefix(self):
		return "%s_%s" % (self.hostname,self.type)
	
	def get_unique_rrdname(self):
		return "%s.rrd" % self._get_unique_prefix()
	
	def get_unique_imgname(self):
		return "%s.%s" % (self._get_unique_prefix(),self.imgtype)
	
	def create(self):
		raise NotImplementedError("Monitor->monitor")

	def monitor(self):
		raise NotImplementedError("Monitor->monitor")

	def update(self):
		raise NotImplementedError("Monitor->update")
	
	def graph(self):
		raise NotImplementedError("Monitor->graph")
	
	def close(self):
		pass

class NetworkMonitor(Monitor):
	# args: interface_name in(bytes) out(bytes)
	args_num = 3

	def _interfaces(self):
		if OP_INTERFACES in self.host_conf:
			return [ s.strip() for s in self.host_conf[OP_INTERFACES].split(';')]
		else:
			raise MonitorException("options '%s' is not defined on host '%s'" % (OP_INTERFACES,self.hostname))

	def update_and_get_graphs(self):
		imgs = ["%s_%s.%s" % (self._get_unique_prefix(),iface,self.imgtype) for iface in self._interfaces()]
		rrds = ["%s_%s.rrd" % (self._get_unique_prefix(),iface) for iface in self._interfaces()]
		rimgs = []
		for ifacename, rrd, img in zip(self._interfaces(),rrds,imgs):
			self._ifacename = ifacename
			rrd_path = path.join(self.host_conf[OP_RRD_DIR],rrd)
			img_path = path.join(self.host_conf[OP_HTML_DIR],img)
			if self._rrd_graph(img_path,rrd_path):
				rimgs.append(img)
		return rimgs
	
	def get_unique_rrdname(self):
		return "%s_%s.rrd" % (self._get_unique_prefix(),\
			self.args[0])
	
	def get_unique_imgname(self):
		return "%s_%s.%s" % (self._get_unique_prefix(),\
			self.args[0],self.imgtype)
	def create(self):
		return \
			(
			"DS:inbytes:COUNTER:"+self.heartbeat+":0:134217728", # max 2G/8 Byte
			"DS:outbytes:COUNTER:"+self.heartbeat+":0:134217728",
			"RRA:AVERAGE:0.5:1:"+self.rows1,
			"RRA:MAX:0.5:1:"+self.rows1,
			"RRA:AVERAGE:0.5:6:"+self.rows2,
			"RRA:MAX:0.5:6:"+self.rows2,
			)

	def update(self):
		return \
			(
			"--template","inbytes:outbytes",
			"%d:%s:%s" % (self.time,self.args[1],self.args[2])
			)
	
	def graph(self,rrd_path):
		return \
			(
			"--title", "%s - Traffic - %s" % (self.hostname,self._ifacename),
			"--vertical-label", "bits per second",
			"--base", "1000",
			"--rigid",
			"--alt-autoscale",
			"DEF:aa="+rrd_path+":inbytes:AVERAGE",
			"DEF:bb="+rrd_path+":inbytes:MAX",
			"DEF:cc="+rrd_path+":outbytes:AVERAGE",
			"DEF:dd="+rrd_path+":outbytes:MAX",
			"CDEF:a=aa,8,*", # transform bytes to bits
			"CDEF:b=bb,8,*",
			"CDEF:c=cc,8,*",
			"CDEF:d=dd,8,*",
			"LINE1:a#00CF00FF:Inbound",
			"GPRINT:a:LAST: Current\:%8.2lf %s",
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:b:MAX:Maximum\:%8.2lf %s\\n",
			"LINE1:c#002A97FF:Outbound",
			"GPRINT:c:LAST:Current\:%8.2lf %s",
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:d:MAX:Maximum\:%8.2lf %s",
			)

class L26NetworkMonitor(NetworkMonitor):
	net_dev = '/proc/net/dev'
	def monitor(self):

		def get_interfaces():
			if path.isfile(self.net_dev):
				lines = [l.rstrip() for l in file(self.net_dev)]
				# lines[0] is: Inter-|   Receive                                                |  Transmit
				# lines[1] is:  face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
				# lines[2:] is:   eth0:365084457 4702537    0    0    0     0          0         0 2122580331 7779954    1    0    0     0       1          0
				iotype = [ i.strip() for i in lines[0].split('|')]
				del iotype[0]
				statnames =  [ i.split() for i in lines[1].split('|')] 
				del statnames[0]
				ifaces = {}
				for l in lines[2:]:
					ifacename, static =  l.split(':',2)
					ifacename = ifacename.strip()
					stats = [ int(i) for i in static.split()]
					iface = {}
					for i in range(len(iotype)):
						iface[iotype[i]] = dict(zip( statnames[i], stats[0:len(statnames[i])] ))
						del stats[0:len(statnames[i])]
					ifaces[ifacename] = iface
				# ifaces = { 'eth0':{'Recieve':{'bytes':365084457, 'packets':4702537 ...}, 'Transmit':{'bytes':2122580331,....}}}
				return ifaces

			else:
				raise MonitorException("can't open /proc filesystem")
	
		rs = []
		ifaces = get_interfaces()
		for iface in self._interfaces():
			if iface in ifaces:
				rs.append( (iface,ifaces[iface]['Receive']['bytes'],ifaces[iface]['Transmit']['bytes']) )
			else:
				raise MonitorException("interface '%s' is not exist" % iface)

		return rs

def _get_kstat( args ):
	cmd = ['/usr/bin/kstat']
	cmd.extend(args)
	code,lines,error = run_command(cmd)
	if code == 0 and len(lines)>2:
	#lines is something like:
	#module: e1000g                          instance: 0
	#name:   e1000g0                         class:    net
	#        obytes64                        715080
	#        rbytes64                        643858
		if lines[-1] == "":
			del lines[-1]
		return dict( [ l.split() for l in lines[2:]] )
	else:
		raise MonitorException("can't run command '%s', return code[%d], error:\n%s" % (self.kstatbin,code,error))

class S10NetworkMonitor(NetworkMonitor):
	
	kstatbin = "/usr/bin/kstat"
	def monitor(self):

		def get_module_instance(ifacename):
			for i in range(1,len(ifacename)+1):
				if not ifacename[-i].isdigit():
					break
			i -= 1
			return ifacename[:-i],str(int(ifacename[-i:]))

		def get_statics(module,instance,iface):
			d  = _get_kstat( [ 
				"-m", module, 
				"-c", "net",
				"-i", instance,
				"-n", iface,
				"-s", '[or]bytes64'
				])
			return iface, int(d['rbytes64']), int(d['obytes64'])

		
		rs = []
		for iface in self._interfaces():
			if len(iface)>2 and iface[-1].isdigit():
				module,instance = get_module_instance(iface)
				rs.append( get_statics(module,instance,iface) )
				return rs
			else:
				raise MonitorException("may be invalid interface name '%s'" % iface)

	
class LoadavgMonitor(Monitor):
	#args: loadavg-1min loadavg-5min loadavg-15min
	args_num = 3
	def create(self):
		return \
			( 
			"DS:loadavg1:GAUGE:"+self.heartbeat+":0:100", 
			"DS:loadavg5:GAUGE:"+self.heartbeat+":0:100", 
			"DS:loadavg15:GAUGE:"+self.heartbeat+":0:100",
			"RRA:AVERAGE:0.5:1:"+self.rows1,
			"RRA:MAX:0.5:1:"+self.rows1,
			"RRA:AVERAGE:0.5:6:"+self.rows2,
			"RRA:MAX:0.5:6:"+self.rows2,
			)
	def monitor(self):
		return ([ "%.3f" % i for i in os.getloadavg()],)
	
	def update(self):
		return \
			(
			"--template", "loadavg1:loadavg5:loadavg15",
			"%d:%s:%s:%s" % (self.time,self.args[0],self.args[1],self.args[2]),
			)
	def graph(self,rrd_path):
		return \
			(
			"--title", "%s - Load Average" % self.hostname,
			"--vertical-label", "processes in the run queue",
			"--base", "1000",
			"--rigid",
			"--lower-limit", "0",
			"--alt-autoscale-max",
			"DEF:a="+rrd_path+":loadavg1:AVERAGE",
			"DEF:b="+rrd_path+":loadavg1:MAX",
			"DEF:c="+rrd_path+":loadavg5:AVERAGE",
			"DEF:d="+rrd_path+":loadavg5:MAX",
			"DEF:e="+rrd_path+":loadavg15:AVERAGE",
			"DEF:f="+rrd_path+":loadavg15:MAX",
			"AREA:a#EACC00FF:1 Minute Average",  
			"GPRINT:a:LAST: Current\:%8.2lf %s",
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:b:MAX:Maximum\:%8.2lf %s\\n"  ,
			"LINE1:c#0000FFFF:5 Minute Average"  ,
			"GPRINT:c:LAST: Current\:%8.2lf %s"  ,
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s"  ,
			"GPRINT:d:MAX:Maximum\:%8.2lf %s\\n"  ,
			"LINE1:e#FF0000FF:15 Minute Average"  ,
			"GPRINT:e:LAST: Current\:%8.2lf %s"  ,
			"GPRINT:e:AVERAGE:Average\:%8.2lf %s"  ,
			"GPRINT:f:MAX:Maximum\:%8.2lf %s\\n" ,
			)
			
class DiskMonitor(Monitor):
	# args: mountpoint used(bytes) free(bytes)
	args_num = 3

	def _mountpoints(self):
		if OP_MOUNT_POINTS in self.host_conf:
			return [ s.strip() for s in self.host_conf[OP_MOUNT_POINTS].split(';')]
		else:
			raise MonitorException("options '%s' is not defined on host '%s'" % (OP_MOUNT_POINTS,self.hostname))

	def get_unique_rrdname(self):
		return "%s_%s.rrd" % (self._get_unique_prefix(),\
			self.args[0].replace('/',','))
	
	def get_unique_imgname(self):
		return "%s_%s.%s" % (self._get_unique_prefix(),\
			self.args[0].replace("/",","),self.imgtype)
	
	def update_and_get_graphs(self):
		imgs = ["%s_%s.%s" % (self._get_unique_prefix(),mp.replace("/",","),self.imgtype) for mp in self._mountpoints()]
		rrds = ["%s_%s.rrd" % (self._get_unique_prefix(),mp.replace('/',',')) for mp in self._mountpoints()]
		rimgs = []
		for mp, rrd, img in zip(self._mountpoints(),rrds,imgs):
			self._mount_point = mp
			rrd_path = path.join(self.host_conf[OP_RRD_DIR],rrd)
			img_path = path.join(self.host_conf[OP_HTML_DIR],img)
			if self._rrd_graph(img_path,rrd_path):
				rimgs.append(img)
		return rimgs
			

	def create(self):
		return \
			(
			"DS:hdd_used:GAUGE:"+self.heartbeat+":0:1125899906842624", #max 1PiB
			"DS:hdd_free:GAUGE:"+self.heartbeat+":0:1125899906842624", 
			"RRA:AVERAGE:0.5:1:"+self.rows1,
			"RRA:MAX:0.5:1:"+self.rows1,
			"RRA:AVERAGE:0.5:6:"+self.rows2,
			"RRA:MAX:0.5:6:"+self.rows2,
			)
	
	def update(self):
		return \
			(
			"--template", "hdd_used:hdd_free",
			"%d:%s:%s" % (self.time,self.args[1],self.args[2]),
			)
	
	def graph(self,rrd_path):
		return \
			(
			"--title", "%s - Disk Space - %s" % (self.hostname,self._mount_point),
			"--vertical-label", "bytes",
			"--base", "1024",
			"--rigid",
			"--lower-limit", "0",
			"--alt-autoscale-max",
			"DEF:a="+rrd_path+":hdd_used:AVERAGE",
			"DEF:b="+rrd_path+":hdd_used:MAX",
			"DEF:c="+rrd_path+":hdd_free:AVERAGE",
			"DEF:d="+rrd_path+":hdd_free:MAX",
			"CDEF:cdefi=a,c,+",
			"AREA:a#F51D30FF:Used" ,
			"GPRINT:a:LAST:     Current\:%8.2lf %s" ,
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:b:MAX:Maximum\:%8.2lf %s\\n" ,
			"AREA:c#002A97FF:Available:STACK",
			"GPRINT:c:LAST:Current\:%8.2lf %s" ,
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:d:MAX:Maximum\:%8.2lf %s\\n" ,
			"LINE1:cdefi#000000FF:Total" ,
			"GPRINT:cdefi:LAST:    Current\:%8.2lf %s" ,
			"GPRINT:cdefi:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:cdefi:MAX:Maximum\:%8.2lf %s",
			)

class L26DiskMonitor(DiskMonitor):
	def monitor(self):
		rs = []
		for mp in self._mountpoints():
			if path.isdir(mp):
				vfs = os.statvfs(mp)
				# on linux26, fs size is f_bsize*f_blocks
				rs.append( (mp,vfs.f_bsize*(vfs.f_blocks-vfs.f_bfree), vfs.f_bsize*vfs.f_bfree) )
			else:
				raise MonitorException("mountpoint '%s' is not a directory" % mp)
		
		return rs

class S10DiskMonitor(DiskMonitor):
	def monitor(self):
		rs = []
		for mp in self._mountpoints():
			if path.isdir(mp):
				vfs = os.statvfs(mp)
				# on sun10, fs size is f_frsize*f_blocks
				rs.append( (mp,vfs.f_frsize*(vfs.f_blocks-vfs.f_bfree), vfs.f_frsize*vfs.f_bfree) )
			else:
				raise MonitorException("mountpoint '%s' is not a directory" % mp)
		return rs


class MemoryMonitor(Monitor):
	# args: kernel(bytes) cache(bytes) free(bytes)
	args_num = 3
	
	def create(self):
		return \
			(
			# for linux26, kernel is kernel+user used memory, cache is cache memory
			# for sun10, kernel is kernel memory, cache is cache+user used memory
			"DS:kernel:GAUGE:"+self.heartbeat+":0:1099511627776", #max 1TiB
			"DS:cache:GAUGE:"+self.heartbeat+":0:1099511627776", 
			"DS:free:GAUGE:"+self.heartbeat+":0:1099511627776", 
			"RRA:AVERAGE:0.5:1:"+self.rows1,
			"RRA:MAX:0.5:1:"+self.rows1,
			"RRA:AVERAGE:0.5:6:"+self.rows2,
			"RRA:MAX:0.5:6:"+self.rows2,
			)

	def update(self):
		return \
			(
			"--template", "kernel:cache:free",
			"%d:%s:%s:%s" % (self.time,self.args[0],self.args[1],self.args[2]),
			)
	

class L26MemoryMonitor(MemoryMonitor):
	meminfo = '/proc/meminfo'
	def monitor(self):
		
		def get_meminfo():
			if path.isfile(self.meminfo):
				d = {}
				for line in [l.rstrip() for l in file(self.meminfo)]:
					# format is :
					#MemTotal:       507400 kB
					#MemFree:          7236 kB
					#Buffers:         33372 kB
					#Cached:         430544 kB
					#SwapCached:          0 kB
					name,num,unit = line.split()
					if unit == 'kB':
						if name[-1] == ":":
							name = name[:-1]
						num = int(num)
						d[name] = num*1024
					else:
						raise MonitorException("unexpected memory unit '%s'" % unit)
				return d
			else:
				raise MonitorException("can't open /proc filesystem")

		meminfo = get_meminfo()
		return [( 
			meminfo['MemTotal']-meminfo['Cached']-meminfo['Buffers']-meminfo['MemFree'],
			meminfo['Cached']+meminfo['Buffers'],
			meminfo['MemFree'],
			)]
	
			
	def graph(self,rrd_path):
		return \
			(
			"--title","%s - Memory Usage" %self.hostname,
			"--vertical-label", "bytes",
			"--base", "1024",
			"--rigid",
			"--lower-limit","0",
			"--alt-autoscale-max",
			"DEF:a="+rrd_path+":free:AVERAGE",
			"DEF:b="+rrd_path+":free:MAX",
			"DEF:c="+rrd_path+":cache:AVERAGE",
			"DEF:d="+rrd_path+":cache:MAX",
			"DEF:e="+rrd_path+":kernel:AVERAGE",
			"DEF:f="+rrd_path+":kernel:MAX",
			"AREA:e#FF5700FF:Kernel+User",
			"GPRINT:e:LAST:Current\:%8.2lf %s",
			"GPRINT:e:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:f:MAX:Maximum\:%8.2lf %s\\n",
			"AREA:c#FFC73BFF:Cache:STACK",
			"GPRINT:c:LAST:      Current\:%8.2lf %s",
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:d:MAX:Maximum\:%8.2lf %s\\n",
			"AREA:a#8F005CFF:Free:STACK",
			"GPRINT:a:LAST:       Current\:%8.2lf %s",
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:b:MAX:Maximum\:%8.2lf %s",
			)


class S10MemoryMonitor(MemoryMonitor):

	def monitor(self):
		pagesize = os.sysconf( os.sysconf_names['SC_PAGESIZE'] )
		pageinfo = _get_kstat( ["-m",'unix','-n','system_pages', '-c', 'pages'])
		free = int(pageinfo['pagesfree'])*pagesize
		total = int(pageinfo['pagestotal'])*pagesize
		kernel = int(pageinfo['pp_kernel'])*pagesize
		return [(kernel,total-free-kernel,free)]

	def graph(self,rrd_path):
		return \
			(
			"--title","%s - Memory Usage" %self.hostname,
			"--vertical-label", "bytes",
			"--base", "1024",
			"--rigid",
			"--lower-limit","0",
			"--alt-autoscale-max",
			"DEF:a="+rrd_path+":free:AVERAGE",
			"DEF:b="+rrd_path+":free:MAX",
			"DEF:c="+rrd_path+":cache:AVERAGE",
			"DEF:d="+rrd_path+":cache:MAX",
			"DEF:e="+rrd_path+":kernel:AVERAGE",
			"DEF:f="+rrd_path+":kernel:MAX",
			"AREA:e#FF5700FF:Kernel",
			"GPRINT:e:LAST:    Current\:%8.2lf %s",
			"GPRINT:e:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:f:MAX:Maximum\:%8.2lf %s\\n",
			"AREA:c#FFC73BFF:User+Cache:STACK",
			"GPRINT:c:LAST:Current\:%8.2lf %s",
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:d:MAX:Maximum\:%8.2lf %s\\n",
			"AREA:a#8F005CFF:Free:STACK",
			"GPRINT:a:LAST:      Current\:%8.2lf %s",
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s",
			"GPRINT:b:MAX:Maximum\:%8.2lf %s",
			)

class CpuMonitor(Monitor):
	#args: system(percentage) user(percentage)
	args_num = 2
	

	def create(self):
		return \
			(
			"DS:system:GAUGE:"+self.heartbeat+":0:101",
			"DS:user:GAUGE:"+self.heartbeat+":0:101", 
			"RRA:AVERAGE:0.5:1:"+self.rows1,
			"RRA:MAX:0.5:1:"+self.rows1,
			"RRA:AVERAGE:0.5:6:"+self.rows2,
			"RRA:MAX:0.5:6:"+self.rows2,
			)

	def update(self):
		return \
			(
			"--template", "system:user",
			"%d:%s:%s" % (self.time,self.args[0],self.args[1]),
			)
	
	def graph(self,rrd_path):
		return \
			(
			"--title", "%s - CPU Usage" % self.hostname,
			"--vertical-label", "percent",
			"--base", "1000",
			"--rigid",
			"--lower-limit", "0",
			"--alt-autoscale-max",
			"DEF:a="+rrd_path+":system:AVERAGE",
			"DEF:b="+rrd_path+":system:MAX",
			"DEF:c="+rrd_path+":user:AVERAGE",
			"DEF:d="+rrd_path+":user:MAX",
			"CDEF:cdefi=a,c,+",
			"AREA:a#FF0000FF:System" ,
			"GPRINT:a:LAST:Current\:%8.2lf %s" ,
			"GPRINT:a:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:b:MAX:Maximum\:%8.2lf %s\\n" ,
			"AREA:c#0000FFFF:User  :STACK",
			"GPRINT:c:LAST:Current\:%8.2lf %s" ,
			"GPRINT:c:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:d:MAX:Maximum\:%8.2lf %s\\n" ,
			"LINE1:cdefi#000000FF:Total " ,
			"GPRINT:cdefi:LAST:Current\:%8.2lf %s" ,
			"GPRINT:cdefi:AVERAGE:Average\:%8.2lf %s" ,
			"GPRINT:cdefi:MAX:Maximum\:%8.2lf %s",
			)

	vmstatbin = "/usr/bin/vmstat"
	def monitor(self):
		# run 'vmstat 2 2', get cpu usage in 2 seconds
		code,lines,error = run_command([self.vmstatbin,"2","2"])
		if code==0 and len(lines)==4:
			# lines[1] is: r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa
			# lines[3] is: 0  0      0   5876  33320 431684    0    0     0     0  110   79  0  1 99  0
			dic = dict( zip( lines[1].split(), [int(i) for i in lines[3].split()] )) 
			if 'us' in dic and 'sy' in dic:
				us = dic['us']
				sy = dic['sy']
			else:
				raise MonitorException("unexpected vmstat format:\n%s" % '\n'.join(lines))
			# on linux26, there is a wa(iowait) cpu usage, it should be belong to system cpu
			if 'wa' in dic:
				sy += dic['wa']
			return [(sy,us)]
		else:
			raise MonitorException("can't run command '%s', return code[%d], error:\n%s" % (self.vmstatbin,code,error))
			
			



def is_linux26():
	uname = os.uname()
	return uname[0]=="Linux" and uname[2][0:3]=="2.6"

_linux26_key2class = {
	"network":L26NetworkMonitor,
	"loadavg":LoadavgMonitor,
	"disk":L26DiskMonitor,
	"cpu":CpuMonitor,
	"memory":L26MemoryMonitor
}


def is_sun10():
	uname = os.uname()
	return uname[0]=="SunOS" and uname[2]=="5.10"

_sun10_key2class = {
	"network":S10NetworkMonitor,
	"loadavg":LoadavgMonitor,
	"disk":S10DiskMonitor,
	"cpu":CpuMonitor,
	"memory":S10MemoryMonitor
}

_platform_key2class = (
	(is_linux26,"linux26",_linux26_key2class),
	(is_sun10,"sun10",_sun10_key2class)
)

_key2class = None
_platform = None

class NotSupportedExecption(Exception):
	pass

def check_myplatform():
	global _key2class, _platform
	if _key2class is None:
		for func,platform,key2class in _platform_key2class:
			if func():
				_key2class = key2class
				_platform = platform
				return
		raise NotSupportedExecption("Not supported platform '%s'" % os.uname()[0])

def get_platform_and_monitor_by_type(type,hostname,host_conf):
	check_myplatform()
	if type in _key2class:
		return (_platform,_key2class[type](type,hostname,host_conf))
	else:
		raise NotSupportedExecption("Not supported monitor type '%s'"%type)

def get_monitor_by_platform_and_type(platform,type,hostname,host_conf):
	for _, myplatform, key2class in _platform_key2class:
		if myplatform == platform :
			if type in key2class:
				return key2class[type](type,hostname,host_conf)
			else:
				raise NotSupportedExecption("Not supported monitor type '%s' on platform '%s'" % (type,platform))

	raise NotSupportedExecption("Not supported platform '%s'" % platform)
	
