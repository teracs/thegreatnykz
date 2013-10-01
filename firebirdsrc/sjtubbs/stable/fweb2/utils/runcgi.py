#!/usr/bin/env python

import sys

if sys.version_info[0:3] < (2,4,0):
	sys.stderr.write('required python with version >=2.4\n')
	sys.exit(4)

import urllib
import random
import optparse
import os
from os import path
import tempfile
import subprocess
import cPickle

__VERSION__ = '1.0'

general_env = {

	'SERVER_SOFTWARE':'server',
	'SERVER_NAME':'localhost',
	'SERVER_ADDR':'127.0.0.1',
	'SERVER_PORT':'80',
	'GATEWAY_INTERFACE':"CGI/1.1",
	'HTTP_ACCEPT':"*/*",
	'SERVER_PROTOCOL':"HTTP/1.1",
	'REMOTE_PORT':"58711",
	'REMOTE_ADDR':'127.0.0.1',
	'HTTP_USER_AGENT':'client',
	'DOCUMENT_ROOT':'/tmp',
}

class SessionException(Exception):
	pass

class Session(object):

	def __init__(self):
		self.params = {}
		self.files = {}
		self.cookies = {}
		self.method = 'GET'
		self.env = dict(general_env)
		self.env['REQUEST_METHOD'] = 'GET'
		self.content_type = 'application/x-www-form-urlencoded'
		self.boundary = ''
		self.workingdir = './'
		self.args = []
	
	def set_url(self,url):
		if(url[0]!='/'):
			raise SessionException, 'invalid url %s' % (url,)
		self.url = url
		self.env['REQUEST_URI']=url
		self.env['SCRIPT_NAME']=url

	
	def set_cgi(self,cgi):
		if not path.isfile(cgi):
			raise SessionException, 'invalid cgi program %s' % (cgi,)
		
		if not os.access(cgi,os.X_OK):
			raise SessionException, "can't execute cgi program %s, make sure you have execute privilege" %(cgi,)

		self.cgi = path.realpath(cgi)
		self.env['SCRIPT_FILENAME'] = self.cgi
	
	def set_workingdir(self,workingdir):
		if not path.isdir(workingdir):
			raise SessionException, 'invalid workingdir %s' % (workingdir,)
		if not os.access(workingdir,os.X_OK|os.R_OK):
			raise SessionException, "can't access workingdir %s" % (workingdir,)

		self.workingdir = path.realpath(workingdir)
	
	def set_param(self,name,value):
		self.params[name] = value
	
	def del_param(self,name):
		if not self.params.has_key(name):
			raise SessionException, "no such parameters %s to del" %(name,)

		del self.params[name]

	
	def set_file(self,name,fpath):
		import mimetypes
		if not path.isfile(fpath):
			raise SessionException, "no such file %s" %(fpath,)
		if not os.access(fpath,os.R_OK):
			raise SessionException, "can't access file %s" %(fpath,)
		abspath = path.realpath(fpath)
		mime,_ = mimetypes.guess_type(abspath)
		filename = path.basename(abspath)
		if mime is None: mime='application/octet-stream'
		self.files[name] = (filename,abspath,mime)
	
	def __getrstr(self,len):
		sa = []
		for i in range(len):
			sa.append(random.choice('abcdefghijklmn1234567890'))
		return ''.join(sa)

	def __validate_method(self):
		if len(self.files)>0 :
			self.method = 'POST'
			self.env['REQUEST_METHOD'] = 'POST'
			if self.boundary == '':
				self.boundary = self.__getrstr(12)
				self.content_type = 'multipart/form-data; boundary='+self.boundary
	
	def validate(self):
		self.__validate_method()
		if not hasattr(self,'cgi'):
			raise SessionException, 'please specify the cgi program'
		if not hasattr(self,'url'):
			raise SessionException, 'please specify the url'

	def del_file(self,name):
		if not self.files.has_key(name):
			raise SessionException, "no such file %s to del" %(name,)

		del self.files[name]

	def set_useragent(self,agent):
		self.env['HTTP_USER_AGENT'] = agent

	def set_referer(self,referer):
		self.env['HTTP_REFERER'] = referer
	
	def set_remoteaddr(self,remoteaddr):
		self.env['REMOTE_ADDR'] = remoteaddr

	
	def set_method(self,method):
		if method not in ('GET','POST'):
			raise SessionException, 'only GET or POST supported'
		self.method = method
		self.env['REQUEST_METHOD'] = method
	
	def set_cookie(self,name,value):
		self.cookies[name] = value
	
	def del_cookie(self,name):
		if not self.cookies.has_key(name):
			raise SessionException, "no such cookie %s to del" %(name,)

		del self.cookies[name]
	
	def set_args(self,args):
		self.args = args
	
	def show(self,out):
		self.validate()

		out.write('ENV:\n')
		for k in self.env:
			out.write('\t%s=%s\n'%(k,self.env[k]))

		out.write('FORM INPUT:\n')
		for k in self.params:
			out.write('\t%s=%s\n'%(k,self.params[k]))

		out.write('UPLOADING FILE:\n')
		for k in self.files:
			v = self.files[k]
			out.write('\t%s=%s(path: %s mimetype: %s)\n'%(k,v[0],v[1],v[2]))

		out.write('COOKIE:\n')
		for k in self.cookies:
			v = self.cookies[k]
			out.write('\t%s=%s\n'%(k,v))
		return 0;
	
	def __urlencode(self):

		def param(k,v):
			import urllib
			return urllib.quote_plus(k)+'='+urllib.quote_plus(v)

		return '&'.join([param(k,self.params[k]) for k in self.params])
	
	def __multipart(self):
		content = []
		
		for k in self.params:
			v = self.params[k]
			content.append('--%s\r\n'% (self.boundary,))
			content.append('Content-Disposition: form-data; name="%s"\r\n\r\n'% (k,))
			content.append(v)
			content.append('\r\n')

		for k in self.files:
			filename,abspath,mime = self.files[k]
			content.append('--%s\r\n'% (self.boundary,))
			content.append('Content-Disposition: form-data; name="%s"; filename="%s"\r\n' % (k,filename))
			content.append('Content-Type: %s\r\n\r\n' % (mime,))
			f = open(abspath,'r')
			content.append(f.read())
			content.append('\r\n')
		
		content.append('--%s--\r\n'%(self.boundary,))

		return ''.join(content)

			
	
	def __construct_get(self):

		fd,fpath = tempfile.mkstemp('cgiin')
		os.close(fd)

		self.env['QUERY_STRING'] = self.__urlencode()
		self.cgiinfile = fpath
	
	def __construct_post(self):
		fd,fpath = tempfile.mkstemp('cgiin')

		if self.content_type == 'application/x-www-form-urlencoded':
			query = self.__urlencode()
		else:
			query = self.__multipart()

		os.write(fd,query)
		os.close(fd)
		self.env['CONTENT_LENGTH'] = str(len(query))
		self.env['CONTENT_TYPE'] = self.content_type
		self.cgiinfile = fpath
	
	def __construct_cookie(self):
		l = []

		for k in self.cookies:
			v = self.cookies[k]
			l.append('%s=%s'%(k,v))
		if len(l) > 0:
			self.env['HTTP_COOKIE'] = '; '.join(l)
			
			
	def __construct(self):
		self.validate()
		if self.method == 'GET':
			self.__construct_get()
		else:
			self.__construct_post()
		
		self.__construct_cookie()
	
	def __gdb(self):
		fd,filename = tempfile.mkstemp('cgigdb')
		if(len(self.args)>0):
			os.write(fd,"b main\nr '%s' <%s\n" % ("' '".join(self.args),self.cgiinfile ) )
		else:
			os.write(fd,"b main\nr <%s\n" % (self.cgiinfile, ) )
		os.close(fd)
		self.gdbfile = filename
	
	def __degdb(self):
		os.unlink(self.gdbfile)
		delattr(self,'gdbfile')

	
	def __deconstruct(self):
		os.unlink(self.cgiinfile)
		delattr(self,'cgiinfile')


	def run(self):
		self.__construct()
		env = dict(os.environ)
		env.update(self.env)
		stdin = open(self.cgiinfile,'r')
		ret = subprocess.call( [self.cgi]+self.args,
			stdin=stdin,
			env=env,
			cwd=self.workingdir,
			close_fds=True)
		self.__deconstruct()
		return ret
	
	def debug(self):
		self.__construct()
		self.__gdb()
		env = dict(os.environ)
		env.update(self.env)
		ret = subprocess.call( ['gdb','-x',self.gdbfile,self.cgi],
			env=env,
			cwd=self.workingdir,
			close_fds=True)
		self.__degdb()
		self.__deconstruct()
		return ret
		
		

def getop():
	op = optparse.OptionParser( usage='%prog [options] [args]', version =  __VERSION__,
		description="""build a shell script which simulates cgi interface as web server to invoke cgi script. args will be pass to cgi script, used -- to escape if args contains options""")

	op.add_option('-a','--action',action='store',help='the action to take, it should be one of [show|run|debug]. show: show the session info; run: run the cgi program; debug: call gdb to run cgi program, break in main function. if this option is not set, program will process session info: read or write session info, or do nothing, depends on your other options',choices=['noaction','show','run','debug'])
	op.add_option('-u','--url',action='store',help='set the url, for example: /cgi-bin/printenv')
	op.add_option('-c','--cgi',action='store',help='set the cgi to invoke')
	op.add_option('-i','--infile',action='store',help='read session info from INPATH', metavar='INPATH')
	op.add_option('-o','--outfile',action='store',help='write session info to OUTPATH', metavar='OUTPATH')
	op.add_option('-w','--workingdir',action='store',help='set the cgi working directory')
	op.add_option('-m','--method',action="store",choices=['get','post'],help="which method to used, get or post, default is get");
	op.add_option('-p','--param',action="append",help="add or update a form input, followed by name and value",nargs=2,metavar='NAME VALUE')
	op.add_option('-P','--del-param',action="append",help="del a form input",metavar="NAME")
	op.add_option('-f','--file',action='append',help='add or update a uploading file, folowed by name and file path, set this option will cause --method post',nargs=2,metavar='NAME FILE')
	op.add_option('-F','--del-file',action='append',help='del a uploading file',metavar='NAME')
	op.add_option('-k','--cookie',action='append',help='add a cookie, followed by name and value',nargs=2,metavar='NAME VLALUE')
	op.add_option('-K','--del-cookie',action='append',help='del a cookie', metavar='NAME')
	op.add_option('-r','--referer',action='store',help='set HTTP_REFERER')
	op.add_option('-R','--remoteaddr',action='store',help='set REMOTE_ADDR, default is 127.0.0.1')
	op.add_option('-U','--useragent',action='store',help='set HTTP_USER_AGENT, default is client')

	op.set_defaults(action='noaction')
	op.disable_interspersed_args()
	return op

def main():
	op = getop()
	opts,args = op.parse_args()
	
	try:
		if not opts.infile is None:
			if not (path.isfile(opts.infile) and os.access(opts.infile,os.R_OK)):
				op.error("can't access infile at %s"%(opts.infile,))
			f = open(opts.infile)
			sess = cPickle.load(f)
			f.close()
			if not isinstance(sess,Session):
				op.error("can't load session info from infile")
		else:
			sess = Session()
		
		if not opts.url is None:
			sess.set_url(opts.url)
		
		if not opts.cgi is None:
			sess.set_cgi(opts.cgi)
		
		if not opts.workingdir is None:
			sess.set_workingdir(workingdir)
		
		if not opts.referer is None:
			sess.set_referer(opts.referer)
		
		if not opts.remoteaddr is None:
			sess.set_remoteaddr(opts.remoteaddr)
		
		if not opts.useragent is None:
			sess.set_useragent(opts.useragent)
		
		if not opts.method is None:
			sess.set_method(opts.method.upper())
		
		if not opts.param is None:
			for name,value in opts.param:
				sess.set_param(name,value)
		
		if not opts.file is None:
			for name,value in opts.file:
				sess.set_file(name,value)

		if not opts.cookie is None:
			for name,value in opts.cookie:
				sess.set_cookie(name,value)
		
		if not opts.del_param is None:
			for name in opts.del_param:
				sess.del_param(name)
		
		if not opts.del_file is None:
			for name in opts.del_file:
				sess.del_file(name)

		if not opts.del_cookie is None:
			for name in opts.del_cookie:
				sess.del_cookie(name)

		if len(args)>0:
			sess.set_args(args)

		sess.validate()
		if not opts.outfile is None:
			f = open(opts.outfile,'w')
			cPickle.dump(sess,f)
			f.close()
		
		if opts.action=='show':
			return sess.show(sys.stdout)
		elif opts.action=='run':
			return sess.run()
		elif opts.action=='debug':
			return sess.debug()
	
	except Exception,e:
		op.error(str(e))
		return 1
	return 0
	
if __name__ == '__main__':
	sys.exit(main())
