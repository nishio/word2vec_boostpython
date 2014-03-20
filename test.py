import word2vec_ext as w2v

def test(x, y=None):
    if not x == y:
        print 'actual:', x
        print 'expected:', y

    assert x == y


w2v.run('-train tiny_corpus.txt -output tiny_corpus.bin -binary 1 -size 10'.split())

test(w2v.get_outvector(1),
     [0.058247972279787064, 0.14208656549453735, 0.2910027503967285,
      -1.287571668624878, 0.98544842004776, 0.7291287183761597,
      -0.021420147269964218, -1.3358420133590698, -0.18779148161411285,
      0.16565284132957458])

invec = w2v.get_invector(1)
test(invec,
     [-0.12175916135311127, 0.9163146615028381, 0.5882982611656189,
       0.45825865864753723, -0.6081610918045044, 0.4681828022003174,
       0.56097811460495, -0.3129468262195587, -0.019592970609664917,
       -0.6972821950912476])


data = w2v.load("tiny_corpus.bin");

test(
    w2v.search(data, "1")[:5],
    ['A', 'a', 'g', '7', 'G'])

data2 = w2v.load_without_normalize("tiny_corpus.bin");

words = [w2v.get_word(data2, i) for i in range(w2v.get_num_words(data2))]
test(words[1], 'd')
test(w2v.get_vector(data2, 'd'), invec)

test(w2v.find_sub(data, ['A', 'b'], ['a'])[:5],
     ['2', 'B', 'h', '8', '7'])  # 'A' + 'b' - 'a'

