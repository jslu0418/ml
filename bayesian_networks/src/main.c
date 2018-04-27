#include "common.h"

int
main (int argc, char **argv)
{
  char *dirname = argv[1];
  char **filenames = read_dir(dirname);
  char str_buf[512];
  char *train_fn;
  char *test_fn;
  char *valid_fn;
  char train_suffix[] = ".ts.data";
  char test_suffix[] = ".test.data";
  char valid_suffix[] = ".valid.data";
  int i=0;
  int j=0;
  int k;
  int ik;
  int flag[4];
  memset((void *)flag, 0, 4*sizeof(int));
  int opt;
  while ((opt = getopt(argc, argv, "abcd")) != -1)
    {
      switch(opt)
        {
        case 'a':
          flag[0] = 1;
          break;
        case 'b':
          flag[1] = 1;
          break;
        case 'c':
          flag[2] = 1;
          break;
        case 'd':
          flag[3] = 1;
          break;
        default:
          break;
        }
    }
  ptr_doc ts_doc, test_doc, valid_doc;
  p_bn bn1, bn2, bn3;
  struct bn **bns;
  int ret=-1;
  double ll;
  double min = 0.0;
  double *lls = calloc(100, sizeof(double));
  double maxll;
  int maxk;
  double weights[100];
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
          if (flag[0] == 1 || flag[1] == 1 || flag[2] == 1)
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
          free_bn_node(bn2);
          free_bn_edge(bn2);
          /* cal log likelihood */
          printf("# %s #\n", test_doc->filename);
          free_doc(test_doc);
          if (flag[0] == 1)
            {
              ll = cal_log_likelihood(bn1, bn2);
              printf("Log likelihood: %f\n", ll);
            }
          if (flag[1] == 1)
            {
              ret = structure_learning(bn1, 0);
              ll = cal_tree_log_likelihood(bn1, bn2);
              printf("Chow-Liu Tree Log likelihood: %f\n", ll);
              /* print_tree(bn1); */
            }
          /* read valid data */
          if (flag[2] == 1 || flag[3] == 1)
            {
              memset((void *)str_buf, 0, 512);
              strcpy(str_buf, dirname);
              if(str_buf[strlen(dirname)-1]!='/')
                {
                  str_buf[strlen(dirname)] = '/';
                  str_buf[strlen(dirname)+1] = 0;
                }
              valid_fn = strncat(str_buf, filenames[i], strlen(filenames[i])-strlen(train_suffix));
              valid_fn = strcat(str_buf, valid_suffix);
              valid_doc = read_file2(valid_fn);
              /* preprocess valid data */
              bn3 = preprocess_data(valid_doc);
              free_bn_node(bn3);
              free_bn_edge(bn3);
              free_doc(valid_doc);
              k = 5;
              if (flag[2] == 1)
                {
                  for(ik=1; ik<41; ik++)
                    {
                      k = ik;
                      for (j=0; j<k; j++)
                        {
                          lls[j] = 1/k;
                        }
                      bns = mixture_em(bn1, k);
                      ll = cal_mixture_em_log_likelihood(bns, k, bn3);
                      printf("Mixture Chow-Liu Tree using EM Log likelihood on Valid Data: %f K: %d\n", ll, k);
                      fflush(stdout);
                      free_bns(bns, k);
                      if(ik == 1)
                        {
                          maxk = k;
                          maxll = ll;
                        }
                      else
                        {
                          if (maxll < ll)
                            {
                              maxk = k;
                              maxll = ll;
                            }
                        }
                    }
                  free_bn(bn3);
                  bns = mixture_em(bn1, maxk);
                  printf("Mixture Chow-Liu Tree using EM Log likelihood with Best K (%d) : %f\n", maxk, cal_mixture_em_log_likelihood(bns, maxk, bn2));
                  fflush(stdout);
                  free_bns(bns, maxk);
                }
              if (flag[3] == 1)
                {
                  for(ik=1; ik<41; ik++)
                    {
                      k = ik;
                      for (j=0; j<k; j++)
                        {
                          lls[j] = 1/k;
                        }
                      ll = cal_mixture_bagging_log_likelihood(bn1, k, bn3, lls);
                      printf("Mixture Chow-Liu Tree using Bagging Log likelihood on Valid Data: %f\n", ll);
                      fflush(stdout);
                      if(ik == 1)
                        {
                          maxk = k;
                          maxll = ll;
                        }
                      else
                        {
                          if (maxll < ll)
                            {
                              maxk = k;
                              maxll = ll;
                            }
                        }
                    }
                  free_bn(bn3);
                  ll = cal_mixture_bagging_log_likelihood(bn1, maxk, bn2, lls);
                  printf("Mixture Chow-Liu Tree using Bagging Log likelihood: %f\n", ll);
                  fflush(stdout);
                }
            }
          free_bn_data(bn1);
          free_bn_node(bn1);
          free_bn_edge(bn1);
          free_bn(bn1);
          free_bn(bn2);
        }
      i++;
    }
  free(lls);
  return 0;
}
