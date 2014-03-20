build:
	env LD_RUN_PATH=/home/nishio/localchat/word2vec_boostpython ~/boost_1_55_0/bjam

test:
	python test.py

debug:
	gdb python -x debug.gdb
