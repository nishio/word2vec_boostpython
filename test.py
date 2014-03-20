#  Copyright Joel de Guzman 2002-2007. Distributed under the Boost
#  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
#  or copy at http://www.boost.org/LICENSE_1_0.txt)
#  Hello World Example from the tutorial

import word2vec_ext as w2v

data = w2v.load("../word2vec/vectors.bin");
assert "felis" in w2v.search(data, "cat");

