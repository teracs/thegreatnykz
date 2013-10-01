#!/usr/bin/env python2.5
import socket
import sys
import telnet
import codecs

s = socket.fromfd(sys.stdin.fileno(),socket.AF_INET,socket.SOCK_STREAM)
f = telnet.TelnetStream(s)
dec = codecs.getincrementaldecoder('utf8')("xmlcharrefreplace")
enc = codecs.getincrementalencoder('utf8')("xmlcharrefreplace")
n = 0
while True:
    n += 1
    #s.sendall('%d:%s'%(n,s.recv(4096)))
    st = f.read(2000)
    st = dec.decode(st)
    f.write('%d:%s'%(n,enc.encode(st)))
    f.flush()

# vim: ts=4 sw=4 sts=4 et:
