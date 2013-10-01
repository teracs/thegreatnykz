import socket
from twisted.internet.interfaces import	IAddress
from zope.interface import implements

class IPv6Address(object):
	implements(IAddress)
	def __init__(self, type, host, port, flowInfo, scope):
		self.type = type
		self.host = host
		self.port = port
		self.flowInfo = flowInfo
		self.scope = scope

	def __getitem__(self,index):
		return (self.type,self.host,self.port).__getitem__(index)
	
	def __getslice__(self, start, stop):
		return (self.type,self.host,self.port)[start:stop]

	def __eq__(self, other):
		if isinstance(other, IPv6Address):
			a = (self.type, self.host, self.port, self.flowInfo, self.scope)
			b = (other.type, other.host, other.port, other.flowInfo, other.scope)
			return a == b
		return False
	
	def __str__(self):
		return 'IPv6Address(%s, %r, %d, %d, %d)' % (
			self.type, self.host, self.port, self.flowInfo, self.scope)

def isIPv6Address(ip):
	try:
		socket.inet_pton(socket.AF_INET6, ip)
	except:
		return 0
	return 1
