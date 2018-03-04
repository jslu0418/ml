#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H
#include "common.h"

typedef struct ret_train_lr{
  double *weights;
} *ptr_ret_train_lr;

double wtimesx(double *weights, int *indices, int feature_size);
int gradient_descent(double *weights, int **all_indices, int size, double eta, double lambda, int vocab_size);
double valid_auc(double *weights, int **valid_indices, int valid_size, int feature_size);
//ptr_ret_train_lr train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda);
ptr_ret_train_lr train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda, int ** valid_indices, int vsize);
int automatic_parameter_tying(double *weights, int **all_indices, int size, double eta, double lambda, int feature_size, int K, int T, int *a, double *u);
ptr_ret_train_lr apt_train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda, int K, int T);
int apt_update_weights(double *weights, int **indices, int size, double eta, double lambda, int feature_size, int K, int T, int *a, double *u, int i);
int init_center(double *weights, int feature_size, int K, int *a, double *u);
int reassignment(double *weights, int feature_size, int K, int *a, double *u);
#endif
