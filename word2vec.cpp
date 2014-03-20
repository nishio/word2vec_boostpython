#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/str.hpp>
#include <boost/python/class.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/manage_new_object.hpp>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

char *bestw[N];

class Data {
public:
  long long words;
  long long size;
  float *matrix;
  char *vocab;
};

boost::python::list get_nearest(Data* data, float* vec, long long* ignore, int num_ignore);

struct my_exception : std::exception
{
  my_exception(const char* message):_message(message){}
  const char* what() const throw() { return _message; }
  const char* _message;
};

void translate(my_exception const& e)
{
  // Use the Python 'C' API to set up an exception object
  PyErr_SetString(PyExc_RuntimeError, e.what());
}

Data* _load(std::string file_name, bool normalize);
Data* load(std::string file_name){
  return _load(file_name, true);
}
Data* load_without_normalize(std::string file_name){
  return _load(file_name, false);
}

Data* _load(std::string file_name, bool normalize){
  FILE *f;
  long long a, b;
  char ch;
  Data* result = new Data();
  long long words;
  long long size;

  f = fopen(file_name.c_str(), "rb");
  if (f == NULL) {
    throw my_exception("Input file not found");
  }
  fscanf(f, "%lld", &words);
  result->words = words;
  fscanf(f, "%lld", &size);
  result->size = size;

  result->vocab = (char *)malloc((long long) words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  result->matrix = (float *)malloc((long long) words * (long long) size * sizeof(float));
  if (result->matrix == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n",
           (long long)words * size * sizeof(float) / 1048576, words, size);
    throw my_exception("Cannot allocate memory");
  }
  for (b = 0; b < words; b++) {
    fscanf(f, "%s%c", &result->vocab[b * max_w], &ch);
    for (a = 0; a < size; a++) fread(&result->matrix[a + b * size], sizeof(float), 1, f);

    if(normalize){
      // normalize vector
      float len = 0.0;
      for (a = 0; a < size; a++) len += result->matrix[a + b * size] * result->matrix[a + b * size];
      len = sqrt(len);
      for (a = 0; a < size; a++) result->matrix[a + b * size] /= len;
    }
  }
  fclose(f);
  return result;
}

boost::python::list search(Data* data, std::string st1){
  boost::python::list result;
  char st[100][max_size];
  float len, vec[max_size];
  long long a, b, c, cn, bi[100];
  long long size = data->size;
  long long words = data->words;
  
  //for (a = 0; a < N; a++) bestd[a] = 0;
  //for (a = 0; a < N; a++) bestw[a][0] = 0;

  // split words
  cn = 0;
  b = 0;
  c = 0;
  while (1) {
    st[cn][b] = st1[c];
    b++;
    c++;
    st[cn][b] = 0;
    if (st1[c] == 0) break;
    if (st1[c] == ' ') {
      cn++;
      b = 0;
      c++;
    }
  }
  cn++;

  // convert word -> index
  for (a = 0; a < cn; a++) {
    for (b = 0; b < words; b++) {
      if (!strcmp(&data->vocab[b * max_w], st[a])) break;
    }
    if (b == words) b = -1;
    bi[a] = b;
    if (b == -1) {
      return result; // Out of dictionary word
    }
  }

  // sum up all vectors
  for (a = 0; a < size; a++) vec[a] = 0;
  for (b = 0; b < cn; b++) {
    if (bi[b] == -1) continue;
    for (a = 0; a < size; a++) vec[a] += data->matrix[a + bi[b] * size];
  }

  // normalize
  len = 0;
  for (a = 0; a < size; a++) len += vec[a] * vec[a];
  len = sqrt(len);
  for (a = 0; a < size; a++) vec[a] /= len;

  return get_nearest(data, vec, bi, cn);
}

long long get_num_words(Data* data){
  return data->words;
}

boost::python::str get_word(Data* data, long long i){
  return boost::python::str(
    const_cast<const char*>(&data->vocab[i * max_w]));
}

long long get_word_index(Data* data, std::string s){
  const char* cstr = s.c_str();
  long long i;
  long long words = data->words;
  for(i = 0; i < words; i++){
    if(strcmp(&data->vocab[i * max_w], cstr) == 0) break;
  }
  if(i == words) i = -1;
  return i;
}

boost::python::list get_vector(Data* data, std::string word){
  boost::python::list ret;
  long long word_index = get_word_index(data, word);
  if(word_index == -1) return ret;
  for(size_t i = 0; i < data->size; i++){
    ret.append(data->matrix[i + word_index * data->size]);
  }
  return ret;
}

boost::python::list get_nearest(Data* data, float* vec, long long* ignore, int num_ignore){
  boost::python::list result;
  float dist, bestd[N];
  long long a, b, c, d;
  long long words = data->words;
  long long size = data->size;

  for (a = 0; a < N; a++) bestd[a] = -1;
  for (a = 0; a < N; a++) bestw[a][0] = 0;

  for (c = 0; c < words; c++) {
    a = 0;
    for (b = 0; b < num_ignore; b++) if (ignore[b] == c) a = 1;
    if(a == 1) continue;
    dist = 0;
    for (a = 0; a < size; a++) {
      dist += vec[a] * data->matrix[a + c * size];
    }
    for (a = 0; a < N; a++) {
      if (dist > bestd[a]) {
	for (d = N - 1; d > a; d--) {
	  bestd[d] = bestd[d - 1];
	  strcpy(bestw[d], bestw[d - 1]);
	}
	bestd[a] = dist;
	strcpy(bestw[a], &data->vocab[c * max_w]);
	break;
      }
    }
  }
  for (a = 0; a < N; a++){
    //result.append(boost::python::make_tuple(boost::python::str(bestw[a]), bestd[a]));
    result.append(boost::python::str(const_cast<const char*>(bestw[a])));
    //result.append(bestd[a]);
  }
  return result;
}


boost::python::list get_sum_vector(Data* data, boost::python::list xs){
  float vec[max_size];
  long long a;
  long long bi[100];
  long long size = data->size;
  boost::python::list ret;
  int num_used_words = 0;

  for (a = 0; a < size; a++) vec[a] = 0.0;
  size_t N = len(xs);
  if(N > 99) N = 99;
  for(size_t i = 0; i < N; i++){
    std::string s = boost::python::extract<std::string>(xs[i]);
    bi[i] = get_word_index(data, s);
    if(bi[i] == -1) continue;
    num_used_words++;
    for (a = 0; a < size; a++) {
      vec[a] += data->matrix[a + bi[i] * size];
    }
  }
  if(num_used_words == 0) return ret;
  for (a = 0; a < size; a++) {
    ret.append(vec[a]);
  }
  return ret;
}

boost::python::list find_with_vector(Data* data, boost::python::list xs, boost::python::list ignore_words){
  float vec[max_size];
  long long a;
  long long ignore_words_index[100];
  long long size = data->size;
  size_t num_ignore_words = len(ignore_words);

  for (a = 0; a < size; a++) {
    vec[a] = boost::python::extract<float>(xs[a]);
  }
  for(size_t i = 0; i < num_ignore_words; i++){
    std::string s = boost::python::extract<std::string>(ignore_words[i]);
    ignore_words_index[i] = get_word_index(data, s);
    if(ignore_words_index[i] == -1) continue;
  }
  return get_nearest(data, vec, ignore_words_index, num_ignore_words);
}

boost::python::list find_add(Data* data, boost::python::list xs){
  float vec[max_size];
  long long a;
  long long bi[100];
  long long size = data->size;

  for (a = 0; a < size; a++) vec[a] = 0.0;
  size_t N = len(xs);
  if(N > 99) N = 99;
  for(size_t i = 0; i < N; i++){
    std::string s = boost::python::extract<std::string>(xs[i]);
    bi[i] = get_word_index(data, s);
    if(bi[i] == -1) continue;
    for (a = 0; a < size; a++) {
      vec[a] += data->matrix[a + bi[i] * size];
    }
  }
  return get_nearest(data, vec, bi, N);
}

boost::python::list find_sub(Data* data, boost::python::list xs, boost::python::list ys){
  float vec[max_size];
  long long a;
  long long bi[100], cn;
  long long size = data->size;

  for (a = 0; a < size; a++) vec[a] = 0.0;
  size_t N = len(xs);
  if(N > 99) N = 99;
  for(size_t i = 0; i < N; i++){
    std::string s = boost::python::extract<std::string>(xs[i]);
    bi[i] = get_word_index(data, s);
    if(bi[i] == -1) continue;
    for (a = 0; a < size; a++) {
      vec[a] += data->matrix[a + bi[i] * size];
    }
  }
  // subtract 2nd
  cn = N;
  N = len(ys);
  if(cn + N > 99) N = 99 - cn;
  for(size_t i = 0; i < N; i++){
    std::string s = boost::python::extract<std::string>(ys[i]);
    long long word_index = get_word_index(data, s);
    if(word_index == -1) continue;
    for (a = 0; a < size; a++) {
      vec[a] -= data->matrix[a + word_index * size];
    }
    bi[cn + i] = word_index;
  }
  cn += N;

  return get_nearest(data, vec, bi, cn);
}


boost::python::list find_analogy(Data* data, std::string s1, std::string s2, std::string s3){
  float vec[max_size];
  long long a;
  long long bi[100], cn = 3;
  long long size = data->size;

  bi[0] = get_word_index(data, s1);
  if(bi[0] == -1) return boost::python::list();
  bi[1] = get_word_index(data, s2);
  if(bi[1] == -1) return boost::python::list();
  bi[2] = get_word_index(data, s3);
  if(bi[2] == -1) return boost::python::list();
  for (a = 0; a < size; a++) vec[a] = -data->matrix[a + bi[0] * size];
  for (a = 0; a < size; a++) vec[a] += data->matrix[a + bi[1] * size];
  for (a = 0; a < size; a++) vec[a] += data->matrix[a + bi[2] * size];
  return get_nearest(data, vec, bi, cn);
}

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    def("load", load, return_value_policy<manage_new_object>());
    def("load_without_normalize", load_without_normalize, return_value_policy<manage_new_object>());
    def("search", search);
    def("get_num_words", get_num_words);
    def("get_word", get_word);
    def("get_word_index", get_word_index);
    def("get_vector", get_vector);
    def("find_add", find_add);
    def("find_sub", find_sub);
    def("find_analogy", find_analogy);
    def("get_sum_vector", get_sum_vector);
    def("find_with_vector", find_with_vector);

    class_<Data>("Data")
      //      .def("__call__",
      //     &accumulator::operator())
      //.add_property("value", &accumulator::value)
      ;

    register_exception_translator<my_exception>(&translate);
}

