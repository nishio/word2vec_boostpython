import word2vec_boostpython as w2v

def test(x, y=None):
    if not x == y:
        print 'actual:', x
        print 'expected:', y

    assert x == y

if 0:  # because of clean-up was not done properly, these tests broke next tests
    w2v.run('-train tiny_corpus.txt -output tiny_corpus.bin -hs 1 -binary 1 -size 2'.split())

    test(w2v.get_outvector_hs(1),
         [0.7281824350357056, 0.27238667011260986])

    invec = w2v.get_invector(1)
    test(invec,
         [-0.45740053057670593, 1.1142560243606567])


w2v.run('-train tiny_corpus.txt -output tiny_corpus.bin -hs 0 -negative 5 -binary 1 -size 10'.split())

test(w2v.get_outvector_ns(1),
     [0.1442144811153412, -0.274962455034256, -1.260462999343872,
      -0.42235079407691956, 0.15925364196300507, -0.6748839616775513,
      -1.4316737651824951, 0.473715215921402, 0.5771697759628296,
      0.05916936323046684])

invec = w2v.get_invector(1)
test(invec,
     [-0.21694795787334442, 0.38501447439193726, 1.080917239189148,
       0.25622865557670593, -0.22400374710559845, 0.7944273948669434,
       0.8532216548919678, -0.3066456913948059, -0.6160392761230469,
       -0.19778962433338165])

num_words = w2v.get_num_words_from_model()


data = w2v.load("tiny_corpus.bin");
test(
    w2v.get_num_words(data),
    num_words)

test(
    w2v.search(data, "1")[:5],
    ['a', 'A', 'g', 'G', '6'])

data2 = w2v.load_without_normalize("tiny_corpus.bin");

words = [w2v.get_word(data2, i) for i in range(w2v.get_num_words(data2))]

test(words[1], 'd')
test(w2v.get_vector(data2, 'd'), invec)
test(list(w2v.get_vectors(data2)[1]), invec)


test(w2v.find_sub(data, ['A', 'b'], ['a'])[:5],
     ['B', '2', 'H', 'h', '8'])  # 'A' + 'b' - 'a'




# test in C++, for easy develop
w2v.test()
