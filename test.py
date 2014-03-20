import word2vec_ext as w2v

w2v.run('-train tiny_corpus.txt -output tiny_corpus.bin -binary 1 -size 10'.split())
data = w2v.load("tiny_corpus.bin");

def test(x, y):
    if not x == y:
        print 'actual:', x
        print 'expected:', y

    assert x == y

test(
    w2v.search(data, "1")[:5],
    ['A', 'a', 'g', '7', 'G'])

test(w2v.get_vector(data, "2"),
     [-0.35658714175224304, 0.12126342952251434, 0.22197042405605316,
       0.4952980577945709, -0.0008666635840199888, -0.5801302790641785,
       0.22293275594711304, 0.38296881318092346, -0.04806125909090042,
       0.1682969182729721])

test(w2v.find_sub(data, ['A', 'b'], ['a'])[:5],
     ['2', 'B', 'h', '8', '7'])  # 'A' + 'b' - 'a'
