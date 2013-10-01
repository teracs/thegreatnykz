#!/usr/bin/env python
# -*- coding: utf8 -*-

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

VERSION = 0.1

setup(
    name = "ntbbs",
    version=VERSION,
    author='Jawahh',
    author_email='jawahh@gmail.com',
    license = 'GPL',
    description='new type bbs',
    package_dir = {'':'lib'},
    packages = ['ntbbs','ntbbs.utils','ntbbs.core'] ,
    ext_modules=[
        Extension("ntbbs.utils._shm", ["lib/ntbbs/utils/ntbbs.utils._shm.pyx","lib/ntbbs/utils/bbsshm.c"], include_dirs=['lib/ntbbs/utils'] ),
    ],
    cmdclass = {'build_ext': build_ext},
)

# vim: ts=4 sw=4 sts=4 et:
