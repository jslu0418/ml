#ifndef PERCEPTRON_H
#define PERCEPTRON_H
#include "common.h"
#include "logistic_regression.h"

int perceptron_update_weights(double *weights, int **indices, int size, double eta, int feature_size);
ptr_ret_train_lr train_perceptrons(int **indices, int size, int feature_size, double eta, int loop, double *weights);
double perceptron_valid_auc(double *weights, int **indices, int size, int feature_size);
ptr_ret_train_lr apt_train_perceptrons(int **indices, int size, int feature_size, double eta, int loop, double *weights, double lambda, int K, int T, int *a, double *u);
#endif /* PERCEPTRON_H */
