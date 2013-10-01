# -*- coding: utf8 -*-

cdef extern from "string.h":
    void* memcpy( void *dest, void *src, int n)

cdef extern from "bbsshm.h":
    char* c_bbsshmat(int key, int size, int to_create, int use_hugetlb)
    void c_bbsshmdt(char *shmptr)

cdef extern from "Python.h":
    int PyString_AsStringAndSize(object obj, char **buffer, Py_ssize_t* length) except -1
    object PyString_FromStringAndSize(char *v, Py_ssize_t size)


class ShmException(Exception):
    pass

cdef class Shm:
    cdef char *ptr
    cdef int key
    cdef int size
    cdef int attached
    def __init__(self, int key, int size ):
        self.attached = 0
        self.key = key
        self.ptr = NULL
        if size < 1:
            raise ShmException( "wrong shm size %d" % size)
        self.size = size


    def attach(self, to_create = False, use_hugetlb = False):
        cdef int flag_to_create = 0
        cdef int flag_use_hugetlb = 0

        if self.attached:
            raise ShmException("already attached")

        if to_create:
            flag_to_create = 1

        if use_hugetlb:
            flag_use_hugetlb = 1

        self.ptr = c_bbsshmat(self.key, self.size, flag_to_create, flag_use_hugetlb)
        if self.ptr == NULL:
            raise ShmException(
                "can't attach shm key %d, size %d, to_create %d, use_hugetlb %d" % 
                (self.key, self.size, flag_to_create, flag_use_hugetlb) )
        self.attached = 1

    def detach(self):
        if self.attached != 1:
            raise ShmException("not attached")
        c_bbsshmdt(self.ptr)
        self.attached = 0

    def __dealloc__(self):
        if self.attached == 1:
            c_bbsshmdt(self.ptr)
            self.attached = 0

    def put(self, int offset, object string):
        cdef char *buffer
        cdef Py_ssize_t pylen
        cdef int len

        if self.attached == 0:
            raise ShmException("access unattached shm")

        PyString_AsStringAndSize(string, &buffer,&pylen)
        len = <int>pylen
        if offset >= 0  \
            and len >= 0 \
            and offset + len <= self.size:
            if len > 0:
                memcpy(&self.ptr[offset], buffer, len)
        else:
            raise ShmException(
                "put outside shared memory, shm size %d, offset %d, length %d" %
                (self.size, offset, len) )

    def get(self, int offset, int len):
        if self.attached == 0:
            raise ShmException("access unattached shm")
        if offset >= 0 \
            and len >= 0 \
            and offset + len <= self.size:
            return PyString_FromStringAndSize(&self.ptr[offset], <Py_ssize_t> len)
        else:
            raise ShmException(
                "get outside shared memory, shm size %d, offset %d, length %d" %
                (self.size, offset, len) )

    def __getreadbuffer__(self, int i, void **p):
        if self.attached == 0:
            raise ShmException("access unattached shm")
        if i!=0:
            raise SystemError("Accessing non-existent shm segment")

        p[0] = <void *>self.ptr;
        return self.size

    def __getwritebuffer__(self, int i, void **p):
        if self.attached == 0:
            raise ShmException("access unattached shm")
        if i!=0:
            raise SystemError("Accessing non-existent shm segment")

        p[0] = <void *>self.ptr;
        return self.size

    def __getcharbuffer__(self, int i, char **p):
        if self.attached == 0:
            raise ShmException("access unattached shm")
        if i!=0:
            raise SystemError("Accessing non-existent shm segment")

        p[0] = self.ptr;
        return self.size

    
    def __getsegcount__(self, int *p):
        if p:
            p[0] = self.size
        return 1



# vim: ts=4 sw=4 sts=4 et:
