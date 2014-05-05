# word2vec_boostpython

word2vec binding for Python using Boost.Python

Its goal: good wrapper to easily handle vectors which were created by word2vec.
For example, `get_vectors` returns Numpy's ndarray.

Original C implementation: https://code.google.com/p/word2vec/

Code License: Apache License 2.0 (same to original project)


## How to use

If you are using Ubuntu,

    $ sudo apt-get install libboost_all_dev

If you use other OS, install boost and fix path in setup.py.

Then run:

    $ sudo python setup.py install

