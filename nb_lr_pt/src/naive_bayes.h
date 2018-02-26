#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H
#include "load_file.h"
#include "utils.h"
#include "common.h"

typedef struct nb_token{
  char *token;
  double *condprob;
} *ptr_token;

typedef struct ret_train_nb{
  struct hsearch_data htab_vocab;
  double *priors;
  double **condprobs;
  int size_vocab;
  int size_prior;

} *ptr_ret_train_nb;

ptr_ret_train_nb train_multinomial_nb (char** classes, ptr_class_docs *docs);
int apply_multinomial_nb(char **classes, double *priors, double **condprob, ptr_doc p_doc, struct hsearch_data htab_vocab);


#endif /* NAIVE_BAYES_H */
