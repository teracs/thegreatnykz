import socket
from twisted.internet import tcp
from twisted.internet import protocol
from twisted.internet import reactor
from ipv6 import *


class Client(tcp.Client):
    addressFamily = socket.AF_INET6

    def resolveAddress(self):
        if isIPv6Address(self.addr[0]):
            self._setRealAddress(self.addr[0])
        else:
            reactor.resolve(self.addr[0]).addCallbacks(
                self._setRealAddress, self.failIfNotConnected
            )

    def getHost(self):
        return IPv6Address('TCP', *self.socket.getsockname())

    def getPeer(self):
        return IPv6Address('TCP', *self.socket.getpeername())


class Connector(tcp.Connector):
    def _makeTransport(self):
        return Client(self.host, self.port, self.bindAddress, self, self.reactor)
    
    def getDestination(self):
        return IPv6Address('TCP', self.host, self.port)

class Server(tcp.Server):
    def getHost(self):
        return IPv6Address('TCP', *self.socket.getsockname())

    def getPeer(self):
        return IPv6Address('TCP', *self.client)

class Port(tcp.Port):
    addressFamily = socket.AF_INET6

    transport = Server

    def _buildAddr(self, address):
        return IPv6Address('TCP', *address)

    def getHost(self):
        return IPv6Address('TCP', *self.socket.getsockname())

    def getPeer(self):
        return IPv6Address('TCP', *self.socket.getpeername())

def connectTCP6(host, port, factory, timeout=30, bindAddress=None, reactor=None):
    if reactor is None:
        from twisted.internet import reactor
    return reactor.connectWith(
        Connector, host, port, factory, timeout, bindAddress
    )


def listenTCP6(port, factory, backlog=5, interface='::', reactor=None):
    if reactor is None:
        from twisted.internet import reactor
    return reactor.listenWith(Port, port, factory, backlog, interface)

def main():
    from twisted.internet import reactor

    class TrivialProtocol(protocol.Protocol):
        def connectionMade(self):
            print 'I (', self.transport.getHost(), ') am connected! (to ', self.transport.getPeer(), ')'
            self.transport.write('Hello, world!\n')
        
        def dataReceived(self, data):
            print 'Received: ' + repr(data)

    class TrivialServerFactory(protocol.ServerFactory):
        protocol = TrivialProtocol
    class TrivialClientFactory(protocol.ClientFactory):
        protocol = TrivialProtocol
    
    p = listenTCP6(6666, TrivialServerFactory())
    c = connectTCP6('::1', 6666, TrivialClientFactory())
    
    reactor.run()

if __name__ == '__main__':
    main()
