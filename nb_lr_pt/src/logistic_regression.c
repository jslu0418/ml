#include "logistic_regression.h"
#include "load_file.h"

#define LRNUMLOOP 9

ptr_ret_train_lr
train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda)
{
  int i,j,k,l,m,n;

  ptr_ret_train_lr p_ret = calloc(1, sizeof(struct ret_train_lr));

  /* initialize weights */
  double *weights = calloc(feature_size, sizeof(double));
  for(i=0; i<feature_size; i++)
    {
      weights[i] = 0.0;
    }

  gradient_descent(weights, train_indices, size, eta, lambda, feature_size);
  p_ret->weights = weights;
  return p_ret;
}

double
wtimesx(double *weights, int *indices, int feature_size)
{
  int i;
  double product = 0.0;
  for(i=0; i<feature_size+1;i++)
    {
      product += weights[i] * (double) indices[i];
    }
  return product;
}

double
sigmoid(double x)
{
  return (double) 1.0 - 1.0/(1.0+exp(x));
}

int
update_weights (double *weights, int **all_indices, int size, double eta, double lambda, int feature_size)
{
  int i,j;
  double sum[size]; /* Y-P(Y=1|X,W) */
  double delta;
  int cont = 0;
  for(i=0; i<size; i++)
    {
      sum[i] = all_indices[i][feature_size] - sigmoid(wtimesx(weights, all_indices[i], feature_size));
    }

  /* sum SUM(X_i * (Y-P(Y=1|X,W))) - lambda * W_i */
  for(j=0;j<feature_size;j++)
    {
      /* update every weights */
      delta = 0.0;
      for(i=0; i<size; i++)
        {
          delta += all_indices[i][j] * sum[i];
        }
      delta -= lambda * weights[j];
      cont = delta==0.0?cont:1; /* if a weights delta is not 0.0, cont */
      weights[j] += delta * eta;
      //  printf("weights[%d].delta: %lf | weights %lf \n", j, delta[j], weights[j]);
    }
  return cont;
}

double
valid_auc(double *weights, int **valid_indices, int valid_size, int feature_size)
{
  double l;
  int j;
  l = 0.0;
  for(j=0; j<valid_size; j++)
    {
      if(wtimesx(weights, valid_indices[j], feature_size) > 0.0)
        {
          if(valid_indices[j][feature_size] == 1)
            l+=1.0;
        }
      else
        {
          if(valid_indices[j][feature_size] == 0)
            l+=1.0;
        }
    }
  // printf("correct case, %lf\n", l);
  return l/valid_size;
}

int
gradient_descent(double *weights, int **all_indices, int size, double eta, double lambda, int feature_size)
{
  int k=0;
  while(k<=LRNUMLOOP)
    {
      update_weights(weights, all_indices, size, eta, lambda, feature_size);
      /* debug */
      /* if(k%500 == 0) */
      /*   { */
      /*     ln_prob = 0.0; */
      /*     for(i=0; i<size; i++) */
      /*       { */
      /*         product = wtimesx(weights, all_indices[i], feature_size); */
      /*         ln_prob += all_indices[i][feature_size] * product; */
      /*         ln_prob -= log(1+exp(product)); */
      /*       } */
      /*     fprintf(stdout, "Step %d: %lf,", k, ln_prob); */
      /*     fflush(stdout); */
      /*   } */
      k++;
    }
  return 0;
}
