#!/usr/bin/env python2.5
# -*- coding: utf8 -*-

import socket
import sys

class UnsupportedOperation(Exception):
    pass


# telnet protocol, see http://www.pcmicro.com/netfoss/telnet.html for details
# we only try to support IAC,DO,DONT,WILL,WONT,SB,SE, ignore others

SE = chr(240) # End of subnegotiation parameters. 
NOP = chr(241) # No operation 
DM = chr(242) #  Data mark
BRK = chr(243) # Break, Indicates that the "break" or "attention" key was hit. 
IP = chr(244) # Suspend, interrupt or abort the process to which the NVT is connected. 
AO = chr(245) # Abort output. Allows the current process to run to completion but do not send its output to the user
AYT = chr(246) # Are you there. Send back to the NVT some visible evidence that the AYT was received. 
EC = chr(247) # Erase character.
EL = chr(248) # Erase line.
GA = chr(249) # Go ahead
SB = chr(250) # subnegotiation of the indicated option follows
WILL = chr(251) # Will
WONT = chr(252) # Will not
DO = chr(253) # Do
DONT = chr(254) # Do not 
IAC = chr(255) # Interpret as command 

# sub-option qulifiers
SO_IS = chr(0)   # option is... 
SO_SEND = chr(1)   # send option 
SO_INFO =  chr(2)   # ENVIRON: informational version of IS 
SO_REPLY = chr(2)   # AUTHENTICATION: client version of IS 
SO_NAME =  chr(3)   # AUTHENTICATION: client version of IS 

# telnet options
OP_TB = chr(0) # Transmit Binary
OP_ECHO = chr(1) # Echo
OP_SGA = chr(3) # Suppress Go Ahead
OP_TT = chr(24) # Terminal Type
OP_WS = chr(31) # Windows Size
OP_TS = chr(32) # Terminal Speed
OP_RFC = chr(33) # Remote Flow Control

# if encounter DO/DONT command and there is some one listen on this option, call func(DO/DNOT, OPTION), this 
# function should return WILL or WONT or nothing
# if encounter DO/DONT command and no one listen on it, return WONT 
# if encounter WILL/WONT command and there is some on listen on it, call func(WILL/WONT, OPTION), this function
# should return DO or DONT or nothing
# if encounter WILL/WONT command and there is no one listen on it, return DONT
# if encounter suboption command and there is some on listen on it, call func(SUBOPTION, VALUE), this function 
# should return nothing
# if encounter suboption command and no one listen on it, ignore it
class TelnetStream(object):
    _BUFSIZE = 4 * 1024
    def __init__(self, sock):
        self._sock = sock
        self.closed = False
        self._rbuf = ''
        self._wbuf = []
        self._wbuf_size = 0

        def telnet_tb(request, option):
            # transmit binary by default
            if request == DO:
                return WILL
            elif request == DONT:
                return WONT # we will transmit binary regardless, let the client handle it

        def telnet_sga(request, option):
            # sga by default
            if request == DO:
                return WILL
            elif request == DONT:
                return WONT # we will sga regardless, let the client handle it
            elif request == WILL:
                return DO
            elif request == WONT:
                return DONT # as above, we will sga

        def telnet_echo(request, option):
            if request == DO:
                return WILL
            elif request == DONT:
                return WONT 

        self._option_cbs= {
            OP_TB: telnet_tb,
            OP_ECHO: telnet_echo,
            OP_SGA: telnet_sga,
            OP_TT: None,
            OP_WS: None,
            OP_TS: None,
            OP_RFC: None,
        }
        self._suboption_cbs = {
            OP_TT: None,
            OP_WS: None,
            OP_TS: None,
        }
        self._pending_read_telnet = ''

        self.write_option(DO, OP_TB)

    def close(self):
        """flush and close this stream"""
        if not self.closed:
            self.flush()
            self._sock.close()
            self.closed = True

    def fileno(self):
        """get the file descripter"""
        return self._sock.fileno()


    def isatty(self):
        return False

    def readable(self):
        return True

    def writeable(self):
        return True

    def seekable(self):
        return False

    def readline(self,limit=None):
        raise UnsupportedOperation("method readline is not supported")

    def readlines(self,hint=None):
        raise UnsupportedOperation("method readlines is not supported")

    def seek(self,offset,whence=None):
        raise UnsupportedOperation("method seek is not supported")

    def tell(self):
        raise UnsupportedOperation("method tell is not supported")
        
    def truncate(self, size=None):
        raise UnsupportedOperation("method truncate is not supported")

    def readall(self):
        raise UnsupportedOperation("method readall is not supported")

    def readinto(self, bytes):
        raise UnsupportedOperation("method readinto is not supported")

    def _addto_wbuf_tb(self, str):
        # translate char 0xff to IAC 0xff, comply with binary transmission of telnet protocol
        self._wbuf_size += len(str)
        while True:
            iac_pos = str.find(IAC)
            if iac_pos == -1:
                break
            else:
                self._wbuf.append(str[0:iac_pos])
                self._wbuf.append(IAC+IAC)
                str = str[iac_pos+1:]
                self._wbuf_size += 1
        self._wbuf.append(str)

    def write(self, bstr, trans=True):
        if len(bstr) == 0:
            return

        if trans:
            self._addto_wbuf_tb(bstr)
        else:
            self._wbuf.append(bstr)
            self._wbuf_size += len(bstr)

        if self._wbuf_size >= self._BUFSIZE:
            # try to write some data
            str = ''.join(self._wbuf)
            nwrite = self._sock.send(str)
            if nwrite != self._wbuf_size:
                self._wbuf = [str[nwrite:]]
                self._wbuf_size -= nwrite
            else:
                self._wbuf = []
                self._wbuf_size = 0

    def flush(self):
        if self._wbuf_size > 0:
            str = ''.join(self._wbuf)
            self._wbuf = []
            self._wbuf_size = 0
            self._sock.sendall(str)

    def read(self, size=-1):
        if size == 0:
            return ''
        if len(self._rbuf) == 0:
            toread = max(self._BUFSIZE, size)
            data = self._telnet_read(toread)
            self._rbuf += data
            del data

        if size < 0 or size > len(self._rbuf):
            size = len(self._rbuf)
        retstr = self._rbuf[0:size]
        self._rbuf = self._rbuf[size:]
        return retstr

    def _telnet_read(self, size):
        data = self._sock.recv(size)
        if len(data) == 0:
            # EOF, no more data 
            return ''
        
        if len(self._pending_read_telnet) != 0:
            if len(self._pending_read_telnet) > self._BUFSIZE:
                # too much data, it seems a error in parsing protocol
                raise IOError("telnet protocol parsing error, too much data")
            # there are some unparsed data left
            data = self._pending_read_telnet + data
            self._pending_read_telnet = ''
        
        # try to find char IAC. if found, parse it
        # the algorithm to parse telnet protocol is not optimized.
        # since it's not used frequently, it should be ok
        iac_pos = 0
        iac_pos = data.find(IAC, iac_pos)

        if iac_pos == -1:
            # shortcut if no IAC found at all
            return data

        while iac_pos != -1:
            data = data[:iac_pos] + self._telnet_parse(data[iac_pos:])        
            iac_pos = data.find(IAC, iac_pos)

        if len(data) == 0:
            # no data to return, we try to get more
            self.flush() # may be client is waiting for some data in _wbuf
            return self._telnet_read(size)
        else:
            return data

    def _telnet_parse(self, data):
        try:
            # here we suppose we got enough data to parse. it will simplify logic
            # if data is not enough actually, an IndexError will be raised
            if data[1] == IAC:
                # char 0xff escaped by IAC
                return data[1:]
            elif '\xfe\xfd\xfc\xfb'.find(data[1]) > -1:
                # IAC WILL/WONT/DO/DONT OPTION
                self._telnet_parse_option(data[:3])
                return data[3:]
            elif data[1] == SB:
                # IAC SB suboption
                iac_se_pos = data.find('\xff\xf0',3)  # try to find IAC SE
                if iac_se_pos > -1:
                    self._telnet_parse_suboption(data[:iac_se_pos + 2])
                    return data[iac_se_pos+2:]
                else:
                    # need more data
                    self._pending_read_telnet = data
                    return ''
            else:
                # unsupported option, ignore it
                return data[2:]
        except IndexError, e:
            # this error means we need more data to parse the telnet command
            self._pending_read_telnet = data
            return ''

    def _telnet_parse_option(self, data):
        if data[2] in self._option_cbs \
        and self._option_cbs[data[2]] is not None:
            ret = self._option_cbs[data[2]](data[1], data[2])
            if ret is None:
                return
        else:
            if data[1] == DO or data[1] == DONT:
                ret = WONT
            else:
                ret = DONT
        self.write_option(ret, data[2])


    def _telnet_parse_suboption(self, data):
        if data[2] in self._suboption_cbs \
        and self._option_cbs[data[2]] is not None:
            self._option_cbs[data[2]](data[2], data[3,-2])


    def listen_option(self, option, func):
        """Listen on 'IAC DO/DONT/WILL/WONT option' command, return old func.
        If encounter DO/DONT command and there is some one listen on this option, call func(DO/DNOT, OPTION). This 
        function should return WILL or WONT or nothing. If encounter DO/DONT command and no one listen on it, return 
        WONT. If encounter WILL/WONT command and there is some on listen on it, call func(WILL/WONT, OPTION). This function
        should return DO or DONT or nothing. If encounter WILL/WONT command and there is no one listen on it, return DONT.
        be careful of request/response circle"""
        old = self._option_cbs[option]
        self._option_cbs[option] = func
        return old

    def listen_suboption(self, suboption, func):
        """Listen on 'IAC SB suboption .... IAC SE' command, return old func.
        If encounter suboption command and there is some on listen on it, call func(SUBOPTION, VALUE). This function 
        should return nothing. If encounter suboption command and no one listen on it, ignore it"""
        old = self._suboption_cbs[suboption]
        self._suboption_cbs[suboption] = func
        return old

    def write_option(self, request, option):
        """write 'IAC request option' to the client"""
        self.write(IAC + request + option, False)
        
    def send_suboption(self, suboption, data=''):
        """write 'IAC SB suboption SO_SEND data IAC SE' to the client"""
        self.write(IAC + SB + suboption + SO_SEND + data + IAC + SE, False)

    def is_suboption(self, suboption, data=''):
        """write 'IAC SB suboption SO_IS data IAC SE' to the client"""
        self.write(IAC + SB + suboption + SO_IS + data + IAC +SE, False)





# vim: ts=4 sw=4 sts=4 et:
