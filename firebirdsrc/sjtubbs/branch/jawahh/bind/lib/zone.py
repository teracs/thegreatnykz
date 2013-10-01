import conf
import utils 
import os
from os import path
import logging

class KeyException(Exception):
	pass


_ALGS = {1:'RSAMD5',3:'DSASHA1',5:'RSASHA1'}

_TYPES = {256:'ZSK',257:'KSK'}

KEY_DEACTIVE = 0
KEY_PUBLISHED  = 1
KEY_ACTIVE = 2
KEY_TODEL = 3

_STATUS =  {
	KEY_DEACTIVE:'Deactive',
	KEY_PUBLISHED:'Published',
	KEY_ACTIVE:'Active',
	KEY_TODEL:"Todel"
}

def gen_keyname(domain,keyid):
	return '%s+005+%s' % (domain,keyid)

class Key(object):
	def __init__(self,belong_domain,keyname,start_time=0,\
		active_time=0,end_time=0,status = KEY_DEACTIVE):
		self.belong_domain = belong_domain
		self.keyname = keyname
		self.start_time = start_time
		self.active_time = active_time
		self.end_time = end_time
		self.status = status

	def get_keyid(self):
		return self.keyname.split('+')[2]
	
	def get_domain(self):
		return self.keyname.split('+')[0]
	
	def get_alg(self):
		return _ALGS[int(self.keyname.split('+')[1])]
	
	def get_public_name(self):
		return 'K%s.key' % (self.keyname,)
	
	def get_public_key(self):
		fpath = \
			path.join(conf.get(self.belong_domain)['dir'],\
				conf.KEY_DIR,self.get_public_name())
		f = file(fpath,'r')
		all = f.read()
		f.close()
		return all
	def save_public_key(self,pubkey):
		fpath = \
			path.join(conf.get(self.belong_domain)['dir'],\
				conf.KEY_DIR,self.get_public_name())
		f = file(fpath,'w')
		f.write(pubkey)
		f.close()

	
	def get_private_name(self):
		return 'K%s.private' % (self.keyname,)
	
	def get_keyset_name(self):
		return 'keyset-%s' % (self.get_domain(),)
	
	def should_be_live(self):
		t = utils.inttime()
		return self.start_time < t and \
			(self.end_time > t or self.end_time == 0)
	
	def should_be_published(self):
		t = utils.inttime()
		return self.start_time < t 
	
	def should_be_active(self):
		if self.active_time == 0:
			return False
		t = utils.inttime()
		if t > self.active_time:
			return True
		else:
			return False
			
	def is_active(self):
		return self.status == KEY_ACTIVE
	
	def is_published(self):
		return self.status == KEY_DEACTIVE or self.status == KEY_PUBLISHED
	
	def set_published(self):
		self.start_time = utils.inttime()
		self.status = KEY_PUBLISHED

	def is_ksk(self):
		fname = path.join(conf.get(self.belong_domain)['dir'], \
				conf.KEY_DIR,self.get_public_name())
		f = file(fname)
		l = f.readline().strip()
		f.close()
		return "KSK" == _TYPES[ int(l.split()[3]) ]
	
	def __repr__(self):
		return self.keyname
	
	def __str__(self):
		type = 'ZSK'
		if self.is_ksk(): type = 'KSK'
		status = _STATUS[self.status]
		age = '-'
		if self.start_time: age = utils.getage(self.start_time)
		st,et,at = '-','-','-'
		if self.start_time: st = utils.ctime(self.start_time)
		if self.end_time: et = utils.ctime(self.end_time)
		if self.active_time: at = utils.ctime(self.active_time)

		return '%s %s %s %s %s %s %s' % \
			(self.keyname,type,status,st,at,et,age)
	
	def gen_keyset_format(self,ttl):
		fname = path.join(conf.get(self.belong_domain)['dir'], \
				conf.KEY_DIR,self.get_public_name())
		f = file(fname)
		line = f.readline()
		f.close()
		kl = line.split()
		kl[1:2] = (ttl,)
		kl.append(self.get_keyid())
		return "%s %d %s %s %s %s (  %s%s%s%s ); keyid = %s " % tuple(kl)
	
	def remove_files(self):
		dc = conf.get(self.belong_domain)
		pripath = path.join(\
			dc['dir'],conf.KEY_DIR,self.get_private_name())
		pubpath = path.join(\
			dc['dir'],conf.KEY_DIR,self.get_public_name())
		if path.isfile(pripath): os.remove(pripath)
		if path.isfile(pubpath): os.remove(pubpath)

def create_key(domain,bits=1024,ksk=False):
	dc = conf.get()
	c = conf.get(domain)
	cwd = os.getcwd()
	os.chdir(path.join(c['dir'], conf.KEY_DIR))
	try:
		args = []
		args.append(dc['keygen_path'])
		args.extend(\
			('-n','ZONE','-a','RSASHA1','-b',str(bits),'-r','/dev/urandom'))
		if(ksk):
			args.extend(('-f','KSK'))
		args.append(domain)
		code,out,err = utils.popen(args)
	finally:
		os.chdir(cwd)
	if code == 0:
		return Key(domain,out.strip()[1:])
	else:
		logging.error("dnssec-keygen error, args %s",str(args))
		logging.error(err)
		raise KeyException("can't create key")
		

class IP(object):
	def __init__(self,domain,ip,expired_time):
		self.domain = domain
		self.ip = ip
		self.expired_time = expired_time 
	def is_ipv6(self):
		return self.ip.find(':') > -1
	def is_expired(self):
		return utils.inttime() > self.expired_time
	def __str__(self):
		if self.is_ipv6():
			return '%s IN AAAA %s' % (self.domain,self.ip)
		else:
			return '%s IN A %s' % (self.domain,self.ip)
	def desc(self):
		if self.is_expired():
			return "%s %s %s expired" % \
			(self.domain,self.ip,utils.ctime(self.expired_time))
		else:
			return "%s %s %s live" % \
			(self.domain,self.ip,utils.ctime(self.expired_time))

if __name__ == '__main__':
	#kl = []
	#kl.append(Key('example.com.+005+37911',1))
	#kl.append(Key('example.com.+005+63002',1000))
	#for k in kl:
		#print k
		#print k.get_keyset_format(3600)
	create_key('example.com.')
	create_key('example.com.',ksk=True)
