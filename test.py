import word2vec_ext as w2v

w2v.run('-train tiny_corpus.txt -output tiny_corpus.bin -binary 1'.split())

data = w2v.load("../word2vec/vectors.bin");
assert "felis" in w2v.search(data, "cat");

