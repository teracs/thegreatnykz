import socket
from twisted.internet import udp 
from twisted.internet import protocol
from twisted.internet import reactor
from ipv6 import *



class Port(udp.Port):
	addressFamily = socket.AF_INET6


	def getHost(self):
		return IPv6Address('UDP', *self.socket.getsockname())
	def connect(self, host, port):
		if self._connectedAddr:
			raise RuntimeError, "already connected, reconnecting is not currently supported (talk to itamar if you want this)"
		if not isIPv6Address(host):
			raise ValueError, "please pass only IP addresses, not domain names"
		self._connectedAddr = (host, port)
		self.socket.connect((host, port))

	def write(self, datagram, addr=None):
		"""write a datagram.
		@param addr: should be a tuple (ip, port), can be None in connected mode.
		"""
		if self._connectedAddr:
			assert addr in (None, self._connectedAddr)
			try:
				return self.socket.send(datagram)
			except socket.error, se:
				no = se.args[0]
				if no == EINTR:
					return self.write(datagram)
				elif no == EMSGSIZE:
					raise error.MessageLengthError, "message too long"
				elif no == ECONNREFUSED:
					self.protocol.connectionRefused()
				else:
					raise
		else:
			assert addr != None
			if not isIPv6Address(addr[0]):
				warnings.warn("Please only pass IPs to write(), not hostnames", DeprecationWarning, stacklevel=2)
			try:
				return self.socket.sendto(datagram, addr)
			except socket.error, se:
				no = se.args[0]
				if no == EINTR:
					return self.write(datagram, addr)
				elif no == EMSGSIZE:
					raise error.MessageLengthError, "message too long"
				elif no == ECONNREFUSED:
					# in non-connected UDP ECONNREFUSED is platform dependent, I think
					# and the info is not necessarily useful. Nevertheless maybe we
					# should call connectionRefused? XXX
					return
				else:
					raise


class ConnectedPort(udp.ConnectedPort):
	addressFamily = socket.AF_INET6

	def getHost(self):
		return IPv6Address('UDP', *self.socket.getsockname())
	
	def getPeer(self):
		return IPv6Address('UDP', *self.socket.getpeername())
	
	def connect(self):
		self.startListening()
	
	def startListening(self):
		self._bindSocket()
		if isIPv6Address(self.remotehost):
			self.setRealAddress(self.remotehost)
		else:
			self.realAddress = None
			d = self.reactor.resolve(self.remotehost)
			d.addCallback(self.setRealAddress).addErrback(self.connectionFailed)


def connectUDP6(remotehost, remoteport, proto, localport=0, interface='',maxPacketSize=8192, reactor=None):
	if reactor is None:
		from twisted.internet import reactor
	#return reactor.connectWith(
	#	ConnectedPort, (remotehost,remoteport), localport, proto, interface, maxPacketSize) 
	p = ConnectedPort( (remotehost,remoteport), localport, proto, interface, maxPacketSize,reactor)
	p.startListening()
	return p


def listenUDP6(port, proto, interface='::', maxPacketSize=8192, reactor=None):
	if reactor is None:
		from twisted.internet import reactor
	return reactor.listenWith(Port, port, proto, interface, maxPacketSize)

def main():
	from twisted.internet import reactor
	class TrivialProtocol(protocol.DatagramProtocol):
		def datagramReceived(self, data,addr):
			print 'from '+ str(addr)
			print 'Received: ' + repr(data)
			self.transport.write(data,addr)
	
	p = listenUDP6(6666, TrivialProtocol())

	class TrivialProtocol2(protocol.ConnectedDatagramProtocol):
		def startProtocol(self):
			self.transport.write('Hello, world!\n')
		def datagramReceived(self,data):
			print 'from ' + str(self.transport.getPeer())
			print 'Received: ' + repr(data)
		def connectionFailed(self,failure):
			print str(failure)

	
	c = connectUDP6('::1', 6666, TrivialProtocol2())
	reactor.run()

if __name__ == '__main__':
	main()
