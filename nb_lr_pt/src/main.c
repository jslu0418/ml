#include "common.h"
#include "utils.h"
#include "load_file.h"
#include "naive_bayes.h"
#include "logistic_regression.h"
#include "perceptrons.h"

struct sl_docs_entry{
  SLIST_ENTRY(sl_docs_entry) ent;
  ptr_doc p_doc;
};

#define NB /* enable NB */
#define APT
#define LR /* enable LR */
#define PT /* enable PT */

#define ETA 0.1
#define NUMLOOP 1
#define LAMBDA 0.00001
#define BESTLOOP 200

int
main (int argc, char **argv)
{
  int i;
  int j;
  char dirname[1000];
  char *argv1 = calloc(strlen(argv[1])+10, sizeof(char));
  int input_k = 5;
  if(argc>2)
    input_k = atoi(argv[2]);
  if(input_k<1||input_k>17)
    input_k = 5;
  printf("***Note: you could run this with additional parameter K.\n");
  printf("***E.g. src/main data_sets/hw2 [K]\n");
  printf("***K from 1 to 17 means how many K values will be used in LR_APT.\n");
  printf("***Because LR with APT training is slow, will only try 5 different values by default.\n");
  char *sets[] = {"/train/", "/test/"};
  char *classes[] = {"spam", "ham", ""}; /* "" for end test */
  char **filenames;
  SLIST_HEAD(sl_docs, sl_docs_entry) sl_head;
  /* array for store classes docs */
  ptr_class_docs p_class_docs[2];
  ptr_class_docs p_test_class_docs[2];
  /* save original dir value */
  for (j=0; j<2; j++)
    {
      p_class_docs[j] = calloc(1, sizeof(struct class_docs));
      strcpy1(p_class_docs[j]->class, classes[j]);
      strcpy(argv1, argv[1]);
      strcat(argv1, sets[0]);
      strcat(argv1, classes[j]);
      strcat(argv1, "/");
      filenames = readdir1(argv1);
      /* load files to list sl_head */
      struct sl_docs_entry *e;
      SLIST_INIT(&sl_head);
      sl_head.slh_first = NULL;
      i = 0;
      while(strcmp(filenames[i], "") != 0)
        {
          strcpy(dirname, argv1);
          e = calloc(1, sizeof(struct sl_docs_entry));
          e->p_doc = load_file(strcat(dirname, filenames[i++]));
          SLIST_INSERT_HEAD(&sl_head, e, ent);
          e = NULL;
        }
      p_class_docs[j]->docs = calloc(i, sizeof(ptr_doc));
      /* save all p_docs to p_class_docs for passing to train_multinomial_nb */
      i = 0;
      while(sl_head.slh_first != NULL)
        {
          p_class_docs[j]->docs[i++] = sl_head.slh_first->p_doc;
          SLIST_REMOVE_HEAD(&sl_head, ent);
        }
      p_class_docs[j]->len = i;

      /* load test files */
      p_test_class_docs[j] = calloc(1, sizeof(struct class_docs));
      strcpy1(p_test_class_docs[j]->class, classes[j]);
      strcpy(argv1, argv[1]);
      strcat(argv1, sets[1]);
      strcat(argv1, classes[j]);
      strcat(argv1, "/");
      filenames = readdir1(argv1);
      SLIST_INIT(&sl_head);
      sl_head.slh_first = NULL;
      i = 0;
      while(strcmp(filenames[i], "") != 0)
        {
          strcpy(dirname, argv1);
          e = calloc(1, sizeof(struct sl_docs_entry));
          e->p_doc = load_file(strcat(dirname, filenames[i++]));
          SLIST_INSERT_HEAD(&sl_head, e, ent);
          e = NULL;
        }
      p_test_class_docs[j]->docs = calloc(i, sizeof(ptr_doc));
      /* save all p_docs to p_class_docs for passing to train_multinomial_nb */
      i = 0;
      while(sl_head.slh_first != NULL)
        {
          p_test_class_docs[j]->docs[i++] = sl_head.slh_first->p_doc;
          SLIST_REMOVE_HEAD(&sl_head, ent);
        }
      p_test_class_docs[j]->len = i;
    }

#ifdef APT
  int Ka[]={1,2,5,10,20,50,100,200,500,1000,2000,5000,10000,15000,20000,25000,30000};
  int bestKa = 1;
  int K,T;
#endif /* APT */

  /* train nb */
  ptr_ret_train_nb ret =  train_multinomial_nb(classes, p_class_docs);
#ifdef NB
  printf("#######Naïve Bayes Start#######\n");
  int max_class;
  int correct_case_num = 0;
  int total_case_num = 0;
  for(j=0; j<2; j++)
    {
      total_case_num += p_test_class_docs[j]->len;
      for(i=0; i<p_test_class_docs[j]->len; i++)
        {
          max_class = apply_multinomial_nb(classes, ret->priors, ret->condprobs, p_test_class_docs[j]->docs[i], ret->htab_vocab);
          if (max_class == j)
            correct_case_num++;
        }
    }
  printf("Accuracy of NaïveBayes with Laplace smoothing on test set: %lf\n", (double) correct_case_num/ (double) total_case_num);
  free(ret->condprobs);
#ifdef APT
  printf("####With Automatic Parameters Tying####\n");
  ptr_ret_train_nb ret_apt;
  bestKa = 1;
  int max_correct_case_num = -1;
  int c;
  for(c=0;c<17;c++)
    {
      ret_apt = apt_train_multinomial_nb(classes, p_class_docs, Ka[c], 0.7);
      correct_case_num = 0;
      total_case_num = 0;
      for(j=0; j<2; j++)
        {
          total_case_num += p_class_docs[j]->len - p_class_docs[j]->len * 0.7;
          for(i=p_class_docs[j]->len*0.7; i<p_class_docs[j]->len; i++)
            {
              max_class = apply_multinomial_nb(classes, ret_apt->priors, ret_apt->aptcondprobs, p_class_docs[j]->docs[i], ret_apt->htab_vocab);
              if (max_class == j)
                correct_case_num++;
            }

        }
      printf("## Auc of NaïveBayes with APT K:%d on valid set: %lf\n", Ka[c], (double) correct_case_num/ (double) total_case_num);
      if(correct_case_num>max_correct_case_num)
        {
          bestKa = Ka[c];
          max_correct_case_num = correct_case_num;
        }
    }
  printf("### Best Accuracy of NaïveBayes with APT K:%d on valid set: %lf\n", bestKa, (double) max_correct_case_num/ (double) total_case_num);

  ret_apt = apt_train_multinomial_nb(classes, p_class_docs, bestKa, 1.0);
  correct_case_num = 0;
  total_case_num = 0;
  for(j=0; j<2; j++)
    {
      total_case_num += p_test_class_docs[j]->len;
      for(i=0; i<p_test_class_docs[j]->len; i++)
        {
          max_class = apply_multinomial_nb(classes, ret_apt->priors, ret_apt->aptcondprobs, p_test_class_docs[j]->docs[i], ret_apt->htab_vocab);
          if (max_class == j)
            correct_case_num++;
        }
    }
  printf("Accuracy of NaïveBayes with Laplace smoothing and APT K:%d on test set: %lf\n", bestKa, (double) correct_case_num/ (double) total_case_num);
  free(ret->aptcondprobs);
#endif /* APT */
  free(ret->priors);
  printf("#######Naïve Bayes End#######\n");
#endif /* NB */

  /* prepare example vectors for LR and perceptron */

  /* split files to 70% train and 30% validation */
  int train_len[2];
  int valid_len[2];
  int **indices = calloc(1, sizeof(int*));
  int vocab_size = ret->size_vocab;
  int feature_size = vocab_size + 1;

  for(i=0; i<2; i++)
    {
      train_len[i] = p_class_docs[i]->len * 0.7;
      valid_len[i] = p_class_docs[i]->len - train_len[i];
    }
  ENTRY e, *ep;
  int **train_indices = calloc(p_class_docs[0]->len+p_class_docs[1]->len, sizeof(int *));
  int **valid_indices = calloc(valid_len[0]+valid_len[1], sizeof(int *));
  int **test_indices = calloc(p_test_class_docs[0]->len+p_test_class_docs[1]->len, sizeof(int *));
  for(i=0; i< train_len[0]+train_len[1]; i++)
    {
      train_indices[i] = calloc(vocab_size+2, sizeof(int)); /* plus 1 for x_0, plus 1 for y */
    }
  for(i=0; i< valid_len[0]+valid_len[1]; i++)
    {
      valid_indices[i] = calloc(vocab_size+2, sizeof(int)); /* plus 1 for x_0, plus 1 for y */
      train_indices[train_len[0]+train_len[1]+i] = valid_indices[i]; /* let train_indices in fact has all cases */
    }
  for(i=0; i< p_test_class_docs[0]->len+p_test_class_docs[1]->len; i++)
    {
      test_indices[i] = calloc(vocab_size+2, sizeof(int)); /* plus 1 for x_0, plus 1 for y */
    }
  /* tracking index of training and validation data */
  int k,l,m,n,o;
  m = 0;
  n = 0;
  o = 0;
  /* examples index in all examples */
  for(i=0; i<2; i++)
    {
      /* train and valid case indices */
      for(j=0; j<p_class_docs[i]->len; j++)
        {
          /* set every docs token indices */
          if(j<train_len[i])
            {
              *indices = train_indices[m];
              m++;
            }
          else
            {
              *indices = valid_indices[n];
              n++;
            }
          (*indices)[0] = 1;
          (*indices)[vocab_size+1] = i; /* class Y = 0|1 */
          k=0;
          while(strcmp(p_class_docs[i]->docs[j]->tokens[k], "") != 0)
            {
              e.key = p_class_docs[i]->docs[j]->tokens[k];
              if(hsearch_r(e, FIND, &ep, &(ret->htab_vocab)) != 0)
                {
                  l = (*(int*)ep->data);
                  (*indices)[l+1] = 1;
                }
              k++;
            }
        }

      /* test cases indices */
      for(j=0;j<p_test_class_docs[i]->len; j++)
        {
          *indices = test_indices[o];
          o++;
          (*indices)[0] = 1;
          (*indices)[vocab_size+1] = i; /* class Y = 0|1 */
          k=0;
          while(strcmp(p_test_class_docs[i]->docs[j]->tokens[k], "") != 0)
            {
              e.key = p_test_class_docs[i]->docs[j]->tokens[k];
              if(hsearch_r(e, FIND, &ep, &(ret->htab_vocab)) != 0)
                {
                  l = (*(int*)ep->data);
                  (*indices)[l+1] = 1;
                }
              k++;
            }
        }
    }

  double best_auc = 0.0;
  double cur_auc = 0.0;
  double best_lambda = LAMBDA;
  double lambda;
  double eta = ETA;
  ptr_ret_train_lr p_ret_lr;
#ifdef LR
  printf("#######Logistic Regression Start#######\n");
  /* for select lambda */
  for(i=-5;i<1;i++)
    {
      lambda = pow(10.0, i);
      p_ret_lr = train_mcap_logistic_regression(train_indices, train_len[0] + train_len[1], feature_size, eta, lambda, valid_indices, valid_len[0]+valid_len[1]);
      cur_auc = valid_auc(p_ret_lr->weights, valid_indices, valid_len[0]+valid_len[1], feature_size);
      printf("## Auc of lambda:%lf on valid set: %lf\n", lambda, cur_auc);
      if(best_auc < cur_auc)
        {
          best_auc = cur_auc;
          best_lambda = lambda;
        }
    }
  printf("### Best Accuracy best lambda:%lf on valid set: %lf\n", best_lambda, best_auc);

  /* use best lambda to train whole training set */

  p_ret_lr = train_mcap_logistic_regression (train_indices, p_class_docs[0]->len+p_class_docs[1]->len, feature_size, eta, best_lambda, valid_indices, valid_len[0]+valid_len[1]);
  cur_auc = valid_auc(p_ret_lr->weights, test_indices, p_test_class_docs[0]->len+p_test_class_docs[1]->len, feature_size);
  printf("Accuracy of LogisticRegression with lambda %lf and eta %lf on test set: %lf\n", best_lambda, eta, cur_auc);

#ifdef APT
  printf("####With Automatic Parameters Tying####\n");
  /* for select lambda and K */
  T=40;
  eta = 0.1;
  best_auc = 0.0;
  i=-5;
  for(j=0;j<input_k;j++)
    for(i=-5;i<1;i++)
       {
        lambda = pow(10.0, i);
        p_ret_lr = apt_train_mcap_logistic_regression (train_indices, train_len[0] + train_len[1], feature_size, eta, lambda, Ka[j], T);
        cur_auc = valid_auc(p_ret_lr->weights, valid_indices, valid_len[0]+valid_len[1], feature_size);
        printf("## Auc of lambda:%lf and K:%d LR_APT on valid set: %lf\n", lambda, Ka[j], cur_auc);
        if(best_auc < cur_auc)
          {
            best_auc = cur_auc;
            best_lambda = lambda;
            K = Ka[j];
          }
       }
  printf("### Best Accuracy of best lambda:%lf and best K:%d on valid set: %lf\n", best_lambda, K, best_auc);
  p_ret_lr = apt_train_mcap_logistic_regression (train_indices, p_class_docs[0]->len+p_class_docs[1]->len, feature_size, eta, best_lambda, K, T);
  cur_auc = valid_auc(p_ret_lr->weights, test_indices, p_test_class_docs[0]->len+p_test_class_docs[1]->len, feature_size);
  printf("Accuracy of LogisticRegression with lambda %lf, K %d LR_APT and eta %lf on test set: %lf\n", best_lambda, K, eta, cur_auc);
#endif /* APT */
  printf("#######Logistic Regression End#######\n");
#endif /* LR */

  int best_loop;
  best_auc = 0.0;
  /* train perceptron */

  /* initialize weights */

#ifdef PT
  printf("#######Perceptron Start#######\n");
  double *weights = calloc(feature_size, sizeof(double));
  for(i=0; i<feature_size; i++)
    {
      weights[i] = 0.0;
    }

  eta = eta * 0.01; /* 0.001 */

  /* choose loop time */
  for(i=1; i<=30; i++)
    {
      train_perceptrons(train_indices, train_len[0] + train_len[1], feature_size, eta, NUMLOOP, weights);
      cur_auc = perceptron_valid_auc(weights, valid_indices, valid_len[0] + valid_len[1], feature_size);
      printf("## Auc of best iterations %d on valid set: %lf\n", i, cur_auc);
      if(best_auc < cur_auc)
        {
          best_auc = cur_auc;
          best_loop = NUMLOOP * i;
        }
    }
  printf("### Best Accuracy of best iterations %d on valid set: %lf\n", best_loop, best_auc);
  train_perceptrons(train_indices, p_class_docs[0]->len+p_class_docs[1]->len, feature_size, eta, best_loop, weights);
  cur_auc = perceptron_valid_auc(weights, test_indices, p_test_class_docs[0]->len+p_test_class_docs[1]->len, feature_size);
  printf("Accuracy of Perceptron with iterations %d and eta %lf on test set: %lf\n", best_loop, eta, cur_auc);

#ifdef APT
  printf("####With Automatic Parameters Tying####\n");
  T = train_len[0]+train_len[1]; /* one iteration */
  eta = 0.1;
  best_auc = 0.0;
  j=1;
  i=-5;
  int ii;
  //best_loop = 1;
  int *a = calloc(feature_size, sizeof(int));
  double *u = calloc(Ka[16], sizeof(double));
  for(j=0;j<input_k;j++)
    for(i=-5;i<-2;i++)
      {
        lambda = pow(10.0, i);
        for(l=0; l<feature_size; l++)
          {
            weights[l] = 0.0;
          }
        u = calloc(Ka[j], sizeof(double));
        srand(time(NULL));
        for(ii=0; ii<feature_size; ii++)
          {
            a[ii] = rand()%Ka[j];
          }
        for(ii=0; ii<Ka[j]; ii++)
          {
            u[ii] = 0.0;
          }
        for(k=1;k<=20;k++)
          {
            apt_train_perceptrons(train_indices, train_len[0]+train_len[1], feature_size, eta, best_loop, weights, lambda, Ka[j], T, a, u);
            cur_auc = perceptron_valid_auc(weights, valid_indices, valid_len[0] + valid_len[1], feature_size);
            printf("## Auc of lambda:%lf, interation: %d, and K:%d PT_APT on valid set: %lf\n", lambda, k, Ka[j], cur_auc);
            if(best_auc <= cur_auc)
              {
                best_auc = cur_auc;
                best_lambda = lambda;
                K = Ka[j];
                best_loop = k;
              }
          }
      }
  printf("### Best Accuracy of best lambda:%lf, iteration: %d, and best K:%d on valid set: %lf\n", best_lambda, best_loop, K, best_auc);
  srand(time(NULL));
  for(ii=0; ii<feature_size; ii++)
    {
      weights[ii] = 0.0;
      a[ii] = rand()%K;
    }
  for(ii=0; ii<K; ii++)
    {
      u[ii] = 0.0;
    }
  apt_train_perceptrons(train_indices, p_class_docs[0]->len+p_class_docs[1]->len, feature_size, eta, best_loop, weights, best_lambda, K, T * best_loop, a, u);
  cur_auc = perceptron_valid_auc(weights, test_indices, p_test_class_docs[0]->len + p_test_class_docs[1]->len, feature_size);
  printf("Accuracy of Perceptron with iterations %d, lambda:%lf, APT K:%d, and eta %lf on test set: %lf\n", best_loop, best_lambda, K, eta, cur_auc);
#endif /* APT */
  printf("#######Perceptron End#######\n");
#endif /* PT */
  return 0;
}
