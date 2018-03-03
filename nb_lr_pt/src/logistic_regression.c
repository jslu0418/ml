#include "logistic_regression.h"
#include "load_file.h"

#define LRNUMLOOP 200
#define APTNUMLOOP 1000

ptr_ret_train_lr
train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda, int ** valid_indices, int vsize)
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
  //printf("Auc: %lf of iteration:%d\n", valid_auc(weights, valid_indices, vsize, feature_size), i * LRNUMLOOP);
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
  while(k<LRNUMLOOP)
    {
      update_weights(weights, all_indices, size, eta, lambda, feature_size);
      k++;
    }
  return 0;
}

ptr_ret_train_lr
apt_train_mcap_logistic_regression(int **train_indices, int size, int feature_size, double eta, double lambda, int K, int T)
{
  int i,j,k,l,m,n;

  ptr_ret_train_lr p_ret = calloc(1, sizeof(struct ret_train_lr));

  /* initialize weights */
  double *weights = calloc(feature_size, sizeof(double));
  int *a = calloc(feature_size, sizeof(int));
  double *u = calloc(K, sizeof(double));
  srand(time(NULL));
  for(i=0; i<feature_size; i++)
    {
      weights[i] = 0.0;
      a[i] = rand()%K;
    }
  for(i=0; i<K; i++)
    {
      u[i] = 0.0;
    }


  automatic_parameter_tying(weights, train_indices, size, eta, lambda, feature_size, K, T, a, u);
  p_ret->weights = weights;
  return p_ret;
}

int
apt_update_weights(double *weights, int *indices, double eta, double lambda, int feature_size, int K, int T, int *a, double *u)
{
  double sum = indices[feature_size] - sigmoid(wtimesx(weights, indices, feature_size));
  int j;
  int i = 1;
  double delta;
  for(j=0;j<feature_size;j++)
    {
      delta = sum * indices[j] - lambda*weights[j] + lambda*u[a[j]];
      i = delta == 0.0?i:0;
      weights[j] += eta * delta;
    }
  return i;
}

int
init_center(double *weights, int feature_size, int K, int *a, double *u)
{
  /* also update center */
  int i,j;
  int *uu = calloc(K, sizeof(int));
  for(i=0;i<K;i++)
    {
      u[i]=0.0;
      uu[i]=0;
    }
  for(j=0;j<feature_size;j++)
    {
      u[a[j]]+=weights[j];
      uu[a[j]]++;
    }
  for(i=0;i<K;i++)
    {
      u[i]=u[i]/uu[i];
    }
  free(uu);
  return 0;
}

int
reassignment(double *weights, int feature_size, int K, int *a, double *u)
{
  int j,i;
  double min = INFINITY;
  double tmp;
  int g;
  for(j=0;j<feature_size;j++)
    {
      g=a[j];
      for(i=0;i<K;i++)
        {
          tmp = weights[j]-u[a[j]];
          tmp = tmp<0?tmp*-1.0:tmp;
          {
            if(min > tmp)
              {
                min = tmp;
                g = i;
              }
          }
        }
      a[j] = g;
    }
  return 0;
}

double
euclidean_distance(double *weights, int feature_size, int K, int *a, double *u)
{
  double D=0.0;
  int j;
  for(j=0;j<feature_size;j++)
    {
      D += pow(weights[j] - u[a[j]], 2);
    }
  return D;
}


int
automatic_parameter_tying(double *weights, int **all_indices, int size, double eta, double lambda, int feature_size, int K, int T, int *a, double *u)
{
  int k=0,l=0,m;
  int i=0,j;
  double tu[K]; /* for tmp record */
  while(k<=T)
    {
      l = 0;
      if(apt_update_weights(weights, all_indices[i], eta, lambda, feature_size, K, T, a, u)!=0)
        break;
      init_center(weights, feature_size, K, a, u);
      while(l<10)
        {
          m = 0;
          reassignment(weights, feature_size, K, a, u);
          for(j=0;j<K;j++)
            tu[j]=u[j];
          init_center(weights, feature_size, K, a, u);
          for(j=0;j<K;j++)
            {
              if(tu[j]!=u[j])
                {
                  m = 1;
                  break;
                }
            }
          if(m==0)
            break;
          l++;
        }
      //  printf("K-means times: %d\n", l);
      for(j=0;j<K;j++)
        u[j]=tu[j];
      k++;
      i++;
      i=i>=size?0:i;
    }
  printf("Iteration times: %d\n", k);
  return 0;
}
