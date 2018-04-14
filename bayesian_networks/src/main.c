#include "common.h"

int
main (int argc, char **argv)
{
  char *dirname = argv[1];
  char **filenames = read_dir(dirname);
  char str_buf[512];
  char *train_fn;
  char *test_fn;
  char train_suffix[] = ".ts.data";
  char test_suffix[] = ".test.data";
  int i=0;
  int j=0;
  ptr_doc ts_doc, test_doc;
  p_bn bn1, bn2;
  struct bn **bns;
  int ret=-1;
  double ll;
  while (strcmp(filenames[i], "") != 0)
    {
      if(strcmp(&filenames[i][strlen(filenames[i])-strlen(train_suffix)], train_suffix) == 0)
        {
          /* read train data */
          memset((void *)str_buf, 0, 512);
          strcpy(str_buf, dirname);
          if(str_buf[strlen(dirname)-1]!='/')
            {
              str_buf[strlen(dirname)] = '/';
              str_buf[strlen(dirname)+1] = 0;
            }
          train_fn = strcat(str_buf, filenames[i]);
          ts_doc = read_file2(train_fn);

          /* preprocess */
          bn1 = preprocess_data(ts_doc);
          free_doc(ts_doc);
          /* cal parameters */
          ret = cal_probs(bn1);
          /* free data */
          /* read test data */
          memset((void *)str_buf, 0, 512);
          strcpy(str_buf, dirname);
          if(str_buf[strlen(dirname)-1]!='/')
            {
              str_buf[strlen(dirname)] = '/';
              str_buf[strlen(dirname)+1] = 0;
            }
          test_fn = strncat(str_buf, filenames[i], strlen(filenames[i])-strlen(train_suffix));
          test_fn = strcat(str_buf, test_suffix);
          test_doc = read_file2(test_fn);
          /* preprocess test data */
          bn2 = preprocess_data(test_doc);
          /* cal log likelihood */
          ll = cal_log_likelihood(bn1, bn2);
          printf("# %s #\n", test_doc->filename);
          printf("Log likelihood: %f\n", ll);
          ret = structure_learning(bn1);
          ll = cal_tree_log_likelihood(bn1, bn2);
          printf("Chow-Liu Tree Log likelihood: %f\n", ll);
          print_tree(bn1);
          bns = mixture_em(bn1, 5);
          printf("Mixture Chow-Liu Tree using EM Log likelihood: %f\n", cal_mixture_em_log_likelihood(bns, 5, bn2));
          free_bns(bns, 5);
          free_bn_data(bn1);
          free_bn_node(bn1);
          free_bn_edge(bn1);
          free(bn1);
          free_bn(bn2);
          free_doc(test_doc);
        }
      i++;
    }
  return 0;
}
