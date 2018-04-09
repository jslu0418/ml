#include "common.h"

int
main (int argc, char **argv)
{
  int i,j,ret;
  double t_start, t_end;
  char *train_file = argv[1];
  char *test_file = argv[2];
  t_start = clock();
  ptr_doc train_doc = read_file2(train_file);
  t_end = clock();
  printf("# Load training data in %f seconds\n", (t_end-t_start)/CLOCKS_PER_SEC);
  int num_train_concepts = 0;
  t_start = clock();
  char **train_concepts = string_split(train_doc->text, "\n", &num_train_concepts);
  ptr_vecs p_vecs = prepare_data(train_concepts, num_train_concepts);
  t_end = clock();
  printf("# Preprocess training data in %f seconds\n", (t_end-t_start)/CLOCKS_PER_SEC);
  printf ("## lines: %d\n", num_train_concepts);
  printf("## users: %d\n", p_vecs->user_len);
  printf("## movies: %d\n", p_vecs->movie_len);
  float **cors = calloc(p_vecs->user_len, sizeof(float *));
  for (i=0; i<p_vecs->user_len; i++)
    {
      cors[i] = calloc(p_vecs->user_len, sizeof(float));
    }
  t_start = clock();
  ret = cal_correlation(p_vecs, cors);
  t_end = clock();
  printf("# Calculate correlation of training data in %f seconds\n", (t_end-t_start)/CLOCKS_PER_SEC);
  t_start = clock();
  ptr_doc test_doc = read_file2(test_file);
  t_end = clock();
  printf("# Load testing data in %f seconds\n", (t_end-t_start)/CLOCKS_PER_SEC);
  int num_test_concepts = 0;
  char **test_concepts = string_split(test_doc->text, "\n", &num_test_concepts);
  printf("## lines: %d\n", num_test_concepts);
  t_start = clock();
  measure(p_vecs, cors, test_concepts, num_test_concepts);
  t_end = clock();
  printf("# Measuring testing data in %f seconds\n", (t_end-t_start)/CLOCKS_PER_SEC);
  return 0;
}
