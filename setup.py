#!/usr/bin/env python
# setup.py
from distutils.core import setup, Extension

module = Extension(
    'word2vec_boostpython',
    sources = ['word2vec.cpp'],
    include_dirs = ['/usr/include/boost'],
    library_dirs = ['/usr/lib'],
    libraries = ['boost_python'],
)

setup(
    name = 'word2vec_boostpython',
    version = '0.01',
    ext_modules = [module],
)
