#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H
#include "common.h"

typedef struct ret_train_lr{
  double *weights;
} *ptr_ret_train_lr;

double wtimesx(double *weights, int *indices, int feature_size);
int gradient_descent(double *weights, int **all_indices, int size, double eta, double lambda, int vocab_size);
double valid_auc(double *weights, int **valid_indices, int valid_size, int feature_size);
ptr_ret_train_lr train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda);

#endif
