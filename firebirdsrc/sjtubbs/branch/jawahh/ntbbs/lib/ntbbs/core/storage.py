# -*- coding: utf8 -*-

from __future__ import with_statement
from struct import Struct
class StructException(Exception):
    pass

class BaseStructMeta(type):
    def __init__(self, name, bases, map):
        super(BaseStructMeta, self).__init__(self, name, bases, map)

        fmtstr = ''.join( x[1]*x[3] if len(x)>3 else x[1] for x in self.__format__)
        self._st = Struct(fmtstr)
        if self.size != self._st.size:
            raise StructException('class %s size is %d, but expected size is %d' % (self.__name__, self._st.size, self.size) )

        format = []
        for x in self.__format__:
            if x[1][-1] in 'x': #ignore pad byte
                continue
            lenx = len(x)
            if lenx == 2:
                format.append( (x[0], x[1], None, 1) )
            elif lenx == 3:
                format.append( (x[0], x[1], x[2], 1) )
            elif lenx == 4:
                format.append( x )
 
        _map = {}
        _indexs = []
        _need_flat = False
        start = 0
        for i in xrange(len(format)):
            length = format[i][3] 
            _map[format[i][0]] = i
            _indexs.append( (start, start+length) )
            start += length
            if length > 1:
                _need_flat = True
 
        self._need_flat = _need_flat
        self._map = _map
        self.format = format
        if _need_flat:
            self._indexs = _indexs
 
 
class BaseStruct(object):
    __metaclass__ = BaseStructMeta
    __format__ = [
        ('name','I', 1),
        ('name2','4s', ['1']*4, 4),
    ]
    size = 20
    def __init__(self, binstr = None, offset = 0, copy_from = None ):
        if copy_from != None:
            self._elements = [ list(x) if type(x) is list else x for x in copy_from._elements ]
        elif binstr is None:
            self._elements = [ list(x[2]) if x[3]>1 else x[2] for x in self.format ]
        else:
            self._elements = [None]*len(self.format)
            self.unpack_from(binstr, offset)


    def __to_flat_list(self):
        if self._need_flat:
            flat = []
            for i in xrange(len(self.format)):
                if self.format[i][3] > 1:
                    flat.extend(self._elements[i])
                else:
                    flat.append(self._elements[i])
            return flat
        else:
            return self._elements

    def __from_flat_list(self, flat):
        flat = [ x.rstrip('\x00') if type(x) is str else x for x in flat]
        if self._need_flat:
            for i in xrange(len(self.format)):
                start, stop  = self._indexs[i]
                if self.format[i][3] > 1:
                    self._elements[i] = flat[start:stop]
                else:
                    self._elements[i] = flat[start]
        else:
            self._elements = flat

        self.__notify()

    def notifyme(self, callback):
        self.callback = callback

    def reset_notify(self):
        if hasattr(self, "callback") :
            del self.callback  

    def __notify(self):
        if hasattr(self, "callback") :
            self.callback(self)
            # one shot notify, then remove the reference
            # to deal with reference cycling
            del self.callback  

    def copy(self):
        return self.__class__( copy_from = self )

    def pack(self):
        return self._st.pack(*self.__to_flat_list() )

    def pack_into(self, buf, offset=0):
        self._st.pack_into(buf, offset, *self.__to_flat_list() )

    def unpack(self, binstr):
        self.__from_flat_list( self._st.unpack(binstr) )

    def unpack_from(self, buf, offset=0):
        self.__from_flat_list(  self._st.unpack_from(buf, offset) )

    __str__ = pack

    def __len__(self):
        return len(self._elements)

    def __getitem__(self, key):
        return self._elements[self._map[key]]

    def __setitem__(self, key, value):
        if key in self._map:
            self._elements[ self._map[key] ] = value
            self.__notify()
        else:
            raise KeyError(key)

    def __iter__(self):
        elements = self._elements
        format = self.format
        return ( (format[i][0], elements[i]) for i in xrange(len(elements)) )

    def __eq__(self, obj):
        if type(obj) is not self.__class__:
            return False
        else:
            return self._elements == obj._elements


from ntbbs.utils import Shm


class BinaryStorage(object):
    def is_resizeable(self):
        raise NotImplementedError

    def is_writeable(self):
        raise NotImplementedError

    def open(self):
        raise NotImplementedError
    
    def close(self):
        raise NotImplementedError

    def get_size(self):
        raise NotImplementedError

    def get_starting_offset(self):
        raise NotImplementedError
    
    def get_inteface(self):
        raise NotImplementedError

    def lock_read(self):
        raise NotImplementedError

    def lock_write(self):
        raise NotImplementedError

    def unlock(self):
        raise NotImplementedError

    def get_lock_state(self):
        raise NotImplementedError

import fcntl
import os
import stat

class FileStorage(BinaryStorage):
    def __init__(self, path, writeable=True, create=True):
        self.path = path
        self.writeable = writeable 
        self.create = create
        self.lock_state = ''

    def is_resizeable(self):
        return True

    def is_writeable(self):
        return self.writeable

    def open(self):
        fdflags = os.O_RDONLY
        flags = 'rb'
        if self.writeable:
            fdflags = os.O_RDWR
            flags = 'rb+'
        if self.create:
            fdflags |= os.O_CREAT
        
        fd = os.open(self.path, fdflags)
        self.fileobj = os.fdopen(fd, flags)
    
    def close(self):
        self.fileobj.close()
        del self.fileobj

    def get_size(self):
        return os.fstat(self.fileobj.fileno())[stat.ST_SIZE]

    def get_starting_offset(self):
        return 0
    
    def get_inteface(self):
        return self.fileobj

    def lock_read(self):
        fcntl.flock(self.fileobj.fileno(), fcntl.LOCK_SH)
        self.lock_state = 'r'

    def lock_write(self):
        fcntl.flock(self.fileobj.fileno(), fcntl.LOCK_EX)
        self.lock_state = 'w'

    def unlock(self):
        fcntl.flock(self.fileobj.fileno(), fcntl.LOCK_UN)
        self.lock_state = ''

    def get_lock_state(self):
        return self.lock_state

class ConnectionException(Exception):
    pass

from contextlib import contextmanager
from functools import partial


class BinaryConnection(object):
    class ShadowAction(object):

        def make_it_real(self, conn):
            raise NotImplementedError

        def affect_index(self, old_index):
            # return new index of old_index, if make_it_real is called
            # return -1 if nonexist anymore
            raise NotImplementedError

        def unaffect_index(self, new_index):
            # return original index of new_index
            # retrun -1 if nonexist
            raise NotImplementedError
    
    class Resizing(ShadowAction):
        def __init__(self, old_size, to_size):
            self.to_size = to_size
            self.old_size = old_size 

        def make_it_real(self, conn):
            conn._resize(self.to_size)

    class Shrinking(Resizing):
        def affect_index(self, old_index):
            if old_index < self.to_size:
                return old_index
            else:
                return -1
        
        def unaffect_index(self, new_index):
            return new_index 

    class Extending(Resizing):
        def affect_index(self, old_index):
            return old_index

        def unaffect_index(self, new_index):
            if self.old_size <= new_index:
                return -1
            else:
                return new_index

    class Bzeroing(ShadowAction):
        def __init__(self, start, stop):
            self.start = start
            self.stop = stop

        def make_it_real(self, conn):
            conn._pwrite( self.start, '\x00'*conn.item_size*(self.stop-self.start) )

        def affect_index(self, old_index):
            if self.start <= old_index <self.stop:
                return -1
            else:
                return old_index

        def unaffect_index(self, new_index):
            if self.start <= new_index <self.stop:
                return -1
            else:
                return new_index

    class Moving(ShadowAction):
        def __init__(self, start, stop, offset):
            self.start = start
            self.stop = stop
            self.offset = offset

        def make_it_real(self, conn):
            binstr = conn._pread( self.start, self.stop-self.start) 
            conn._pwrite( self.start, '\x00'*(self.stop - self.start)*conn.item_size )
            conn._pwrite( self.offset, binstr)

        def affect_index(self, old_index):
            if self.start <= old_index < self.stop:
                return old_index - self.start + self.offset
            elif 0 <= old_index - self.offset < self.stop - self.start:
                return -1
            else:
                return old_index

        def unaffect_index(self, new_index):
            if 0 <= new_index - self.offset < self.stop - self.start:
                return new_index - self.offset + self.start 
            elif self.start <= new_index < self.stop:
                return -1
            else:
                return new_index

    
    def __init__(self, storage, struct_cls):
        self.struct_cls = struct_cls
        self.item_size = struct_cls.size

        self.storage= storage 
        
        self.opened = False

    def _write_struct(self, index, item):
        raise NotImplementedError

    def _read_struct(self, index):
        raise NotImplementedError

    def _pread(self, offset, num ):
        raise NotImplementedError

    def _pwrite(self, offset, binstr):
        raise NotImplementedError

    def _resize(self, num):
        raise NotImplementedError

    def _sync(self):
        raise NotImplementedError

    def open(self):
        if self.opened:
            raise ConnectionException("try to reopen connection that was opened")
        self.storage.open()
        self.writeable = self.storage.is_writeable()
        self.resizeable = self.storage.is_resizeable()
        self.num_bytes = self.storage.get_size()
        self.starting_offset = self.storage.get_starting_offset()
        self.interface = self.storage.get_inteface()
        self.num_items = int( self.storage.get_size()/self.item_size )
        self.unsaved = {}
        self.pending_action = []
        self.opened = True

    def close(self):
        if not self.opened:
            raise ConnectionException("try to close connection that is not opened")
        if len(self.unsaved)>0 and len(self.pending_action)>0:
            raise ConnectionException("there is uncommited data, but required to close")
        if self.storage.get_lock_state() != '':
            raise ConnectionException("there is hold lock, but required to close")
        del self.interface
        self.storage.close()
        self.opened = False

    def __struct_changed(self, index, rev, st):
        assert rev <= len(self.pending_action)
        if not self.writeable:
            raise ConnectionException("try to write readonly storage")
        if rev < len(self.pending_action):
            for action in self.pending_action[rev:]:
                index = action.affect_index(index)
                if index == -1:
                    return
        if index not in self.unsaved:
            self.unsaved[index] = st


    def __check_index(self, index):
        if index < 0:
            i = self.num_items + index
        else:
            i = index

        if i < 0 or i >= self.num_items:
            raise IndexError("index %d out of range" % (index))
        
        return i

    def __getitem__(self, index):
        i = self.__check_index(index)

        if i in self.unsaved:
            return self.unsaved[i]
        else:
            for action in reversed(self.pending_action):
                i = action.unaffect_index(i)
                if index < 0:
                    st = self.struct_cls()
                    break
            else:
                st = self._read_struct(i)
            st.notifyme(partial(self.__struct_changed, i, len(self.pending_action)))
            return st

    def __try_to_write(self):
        if not self.writeable:
            raise ConnectionException("write to non writeable storage")

    def __try_to_resize(self):
        if not (self.resizeable and self.writeable):
            raise ConnectionException("resize to non resizeable storage")

    def __setitem__(self, index, value):
        self.__try_to_write()
        i = self.__check_index(index)
        value.reset_notify()
        self.unsaved[i] = value

    def append(self, value):
        self.__try_to_resize()
        self.__real_append(value)

    def __real_append(self, value):
        value.reset_notify()
        self.unsaved[self.num_items] = value
        self.num_items += 1
    
    def extend(self, values):
        self.__try_to_resize()
        for v in values:
            self.__real_append(v)

    def abort(self):
        self.unsaved = {}
        self.pending_action = []

    def commit(self):
        for action in self.pending_action:
            action.make_it_real(self)

        for index,st in  sorted( self.unsaved.iteritems(), 
                            cmp = lambda x,y: cmp(x[0],y[0])):
            self._write_struct(index, st)

        self.pending_action = []
        self.unsaved = {}
        self._sync()

    def __apply_action(self, action):
        unsaved = {}
        for index, item in self.unsaved.iteritems():
            nindex = action.affect_index(index)
            if nindex > -1:
                unsaved[nindex] = item
        self.unsaved = unsaved

        self.pending_action.append(action)

    def resize(self, size):
        if size == self.num_items:
            return
        elif size > self.num_items:
            action = Extending(size)
        else:
            action = Shrinking(size)
        self.__apply_action(action)
        self.num_items = size

    def copyto(self, start, stop, offset):
        assert stop > -1
        assert start > -1
        assert offset > -1
        assert start < stop
        if stop > self.num_items \
            or offset + stop - start > self.num_items:
            raise ConnectionException("illegal copy, start %d, stop %d, offset %d, num items %d" % \
                (start, stop, offset, self.num_items))
        self.__apply_action( Copying(start, stop, offset) )

    def bzero(self, start=0, stop=-1):
        assert start > -1
        assert stop > -2
        if stop == -1:
            stop = self.num_items
        if start > stop or stop > self.num_items:
            raise ConnectionException("illegal bzero, start %d, stop %d, num items %d" % \
                (start, stop, self.num_items))
        if start == stop:
            return
        self.__apply_action( Bzeroing(start, stop) )



    @contextmanager
    def lock_read(self):
        if self.storage.get_lock_state() != '':
            raise ConnectionException("storage have been locked: %s" %(self.storage.get_lock_state()))
        self.storage.lock_read()
        try:
            yield self 
        except:
            self.storage.unlock()
            raise 
        else:
            self.storage.unlock()

    @contextmanager
    def lock_write(self):
        if not self.writeable:
            raise ConnectionException("lock write non wirteable storage")

        if self.storage.get_lock_state() != '':
            raise ConnectionException("storage have been locked: %s" %(self.storage.get_lock_state()))

        self.storage.lock_write()
        try:
            yield self 
        except:
            self.abort()  
            self.storage.unlock()
            raise 
        else:
            self.commit()
            self.storage.unlock()

    def __len__(self):
        return self.num_items

    def __check_range(self, start, stop):
        if start > self.num_items or stop > self.num_items:
            raise IndexError("out of range: start %d stop %d number of items %d"%
                (start, stop, self.num_items) )

    def iterate(self, start, stop):
        self.__check_range(self, start, stop)
        if stop == -1:
            stop = self.num_items
        for i in xrange(start, stop):
            yield self.__getitem__(i)

    def find(self, selector, start=0, stop=-1):
        self.__check_range(self, start, stop)
        if stop == -1:
            stop = self.num_items
        for i in xrange(start, stop):
            st = self.__getitem__(i)
            if selector(st):
                return st
        return None 

    
class FileConnection(BinaryConnection):
    def open(self):
        super(FileConnection, self).open()
        self.file_pos = 0

    def close(self):
        super(FileConnection, self).close()
        del self.file_pos
    
    def __check_pos(self, pos):
        if pos != self.file_pos:
            self.interface.seek(pos)
            self.file_pos = pos

    def _write_struct(self, index, st):
        self.__check_pos(index*self.item_size + self.starting_offset)
        self.interface.write(st.pack())
        self.file_pos += self.item_size

    def _read_struct(self, index):
        self.__check_pos(index*self.item_size + self.starting_offset)
        binstr = self.interface.read(self.item_size)
        if len(binstr) != self.item_size :
            raise ConnectionException("can't get enough data from file")
        self.file_pos += self.item_size
        return self.struct_cls(binstr)

    def _pwrite(self, offset, binstr):
        self.__check_pos(offset+self.starting_offset)
        self.interface.write(binstr)
        self.file_pos += len(binstr)

    def _pread(self, offset, num):
        self.__check_pos(offset+self.starting_offset)
        binstr = self.interface.read(num)
        if len(binstr)!=num:
            raise ConnectionException("can't get enough data from file")
        self.file_pos += num
        return binstr

    def _resize(self, num):
        self.interface.truncate(num)

    def _sync(self):
        self.interface.flush()
        self.file_pos = self.interface.tell()



def connect(storage, struct_cls):
    if type(storage) is FileStorage:
        return FileConnection(storage, struct_cls)
    else:
        raise ConnectionException("no appropriate connection")

import unittest
class TestStruct(BaseStruct):
    __format__ = [
        ('e0', 'I', 0),
        ('e1', 'I', [1,2], 2),
    ]
    size = 12
class TestFileConnection(unittest.TestCase):

    def set(self):
        self.path = os.tmpnam()
        self.storage = FileStorage( self.path )
        self.conn = connect(self.storage, TestStruct)
        self.conn.open()

    def unset(self):
        self.conn.close()
        os.unlink(self.path)
        del self.path
        del self.storage
        del self.conn

    def atestAppend(self):
        self.set()
        conn = self.conn
        self.assertEqual(len(conn), 0)
        with conn.lock_write():
            conn.append( TestStruct() )
            conn.append( TestStruct() )
            self.assertEqual(conn[0], TestStruct())
            self.assertEqual(conn[1], TestStruct())
        self.assertEqual( len(conn), 2)
        with conn.lock_read():
            self.assertEqual(conn[0], TestStruct())
            self.assertEqual(conn[1], TestStruct())
        self.unset()

    def atestExtend(self):
        self.set()
        conn = self.conn
        
        self.assertEqual(len(conn), 0)
        with conn.lock_write():
            conn.extend( [ TestStruct(), TestStruct() ])
        self.assertEqual( len(conn), 2)
        with conn.lock_read():
            self.assertEqual(conn[0], TestStruct())
            self.assertEqual(conn[1], TestStruct())
        self.unset()

    def set2(self):
        self.set()
        conn = self.conn
        with conn.lock_write():
            conn.extend( [ TestStruct(), TestStruct() ])

    def testWrite(self):
        self.set2()
        conn = self.conn
        
        with conn.lock_write():
            conn[0]['e0'] = 1
            self.assertEqual(conn[0]['e0'],1) 

        with conn.lock_read():
            self.assertEqual(conn[1]['e1'][1],2)
            self.assertEqual(conn[0]['e0'],1) 

        self.unset()


if __name__ == '__main__':
    unittest.main()


# vim: ts=4 sw=4 sts=4 et:
