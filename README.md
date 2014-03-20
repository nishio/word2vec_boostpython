# word2vec_boostpython

word2vec binding for Python using Boost.Python

It's pre-alpha status, interfaces may change.

Original C implementation: https://code.google.com/p/word2vec/

Code License: Apache License 2.0 (same to original project)


## How to use

You need to fix some hard-coded config.

path to boost in Jamroot:

    use-project boost
      : /home/nishio/boost_1_55_0 ;

path to boost tools in boost-build:

    boost-build /home/nishio/boost_1_55_0/tools/build/v2 ;

path to word2vec output for test in hello.py:

    data = hello_ext.load("../word2vec/vectors.bin");

Then run $ <bjam_path>/bjam

