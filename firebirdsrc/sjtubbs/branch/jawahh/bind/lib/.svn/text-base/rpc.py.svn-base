import httplib
import socket
import xmlrpclib
import conf
import time
import utils
import M2Crypto.X509 as x509
import cert
import base64

class RpcException(Exception):
	pass

class HTTPSConnection(httplib.HTTPSConnection):
	default_port = httplib.HTTPS_PORT
	def connect(self):
		"Connect to a host on a given (SSL) port."

		httplib.HTTPConnection.connect(self)
		sock = self.sock
		ssl = socket.ssl(sock, self.key_file, self.cert_file)
		self.sock = httplib.FakeSocket(sock, ssl)
class HTTPS(httplib.HTTP):

	_connection_class = HTTPSConnection

	def __init__(self, host='', port=None, key_file=None, cert_file=None,
		     strict=None):
	    # provide a default host, pass the X509 cert info

	    # urf. compensate for bad input.
	    if port == 0:
		port = None
	    self._setup(self._connection_class(host, port, key_file,
					       cert_file, strict))

	    # we never actually use these for anything, but we keep them
	    # here for compatibility with post-1.5.2 CVS.
	    self.key_file = key_file
	    self.cert_file = cert_file

class SafeTransport(xmlrpclib.Transport):
    """Handles an HTTPS transaction to an XML-RPC server."""


    def make_connection(self, host):
        # create a HTTPS connection object from a host descriptor
        # host may be a string, or a (host, x509-dict) tuple
        host, extra_headers, x509 = self.get_host_info(host)
	return HTTPS(host, None, **(x509 or {}))


class DNSRPC(object):
	support_version = [1]
	def __init__(self,url):
		from urlparse import urlparse
		scheme = urlparse(url)[0]
		if scheme == 'https':
			self.server = xmlrpclib.Server(url,SafeTransport())
		elif scheme == 'http':
			self.server = xmlrpclib.Server(url)
		else:
			raise RpcException(\
				'only http and https scheme support, url: ' + url)
		now = utils.inttime()
		version = self._rpccall('getver',['version'],str(now))['version']
		try:
			self.version = int(version)
		except ValueError, e:
			raise RpcException('rpc version error')
		if self.version not in self.support_version:
			raise RpcException('unsupported version '+str(self.version))
	
	def _rpccall(self,name,rsnames,*args):
		try:
			returns = getattr(self.server,name)(*args)
		except Exception, e:
			raise RpcException(str(e))
		if not (isinstance(returns,dict) and returns.has_key('code')\
			and returns.has_key('message')):
			raise RpcException('rpc call return unknown format: '+str(returns))
		self._check_code(returns['code'],returns['message'])
		for name in rsnames:
			if not returns.has_key(name):
				raise RpcException(\
				'rpc call return incomplete results, expect name %s: %s:'\
				%(name,str(returns)))
		return returns

	def _check_code(self,code,msg):
		try:
			if int(code) != 0:
				raise RpcException(msg)
		except ValueError, e:
			raise RpcException('rpc call return unknown code '+str(code))

	
	def get_x509_crt(self,domain):
		now = utils.inttime()
		ts = str(now)
		crtstring = self._rpccall('getcrt',['crt'],ts,domain)['crt']
		try:
			crt = x509.load_cert_string(crtstring)
			crt.verify()
		except:
			RpcException('recieve corrupt data format for x509 cert')
		if crt.get_subject().CN.lower() != domain.lower():
			raise RpcException('wrong domain name in x509 cert %s!=%s'\
				% (domain,crt.get_subject().CN))
		notb = cert.asn12time(crt.get_not_before())
		nota = cert.asn12time(crt.get_not_after())
		if now > nota or now < notb:
			raise RpcException('get expired x509 cert for domain %s'+domain)
		return crt
	
	def set_x509_crt(self,crt):
		ts = str(utils.inttime())
		crtstring = crt.as_pem()
		self._rpccall('setcrt',[],ts,crtstring)
	
	def update_ksk(self,ksk,pkey):
		ts = str(utils.inttime())
		keycontent = ksk.get_public_key()
		keyname = ksk.keyname
		pkey.sign_init()
		pkey.sign_update(ts)
		pkey.sign_update(keyname)
		pkey.sign_update(keycontent)
		signature = base64.standard_b64encode(pkey.sign_final())
		rtstring = self._rpccall\
			('updateksk',['roll_time'],ts,keyname,keycontent,signature)\
			['roll_time']
		try:
			roll_time = int(rtstring)
		except ValueError,e:
			RpcException(\
				'recieve corrupt data format, excpet an int: '+rtstring)
		return roll_time
	
	def update_ip(self,domain,pkey):
		ts = str(utils.inttime())
		pkey.sign_init()
		pkey.sign_update(ts)
		pkey.sign_update(domain)
		signature = base64.standard_b64encode(pkey.sign_final())
		dustring = self._rpccall\
			('updateip',['duration'],ts,domain,signature)['duration']
		try:
			duration = int(dustring)
		except ValueError,e:
			RpcException(\
				'recieve corrupt data format, excpet an int: '+dustring)
		return duration 

