#include "perceptrons.h"
#include "logistic_regression.h"

ptr_ret_train_lr
train_perceptrons(int **indices, int size, int feature_size, double eta, int loop, double *weights)
{
  ptr_ret_train_lr p_ret = calloc(1, sizeof(struct ret_train_lr));
  int i;
  for(i=0; i<loop; i++)
    {
      perceptron_update_weights(weights, indices, size, eta, feature_size);
    }
  p_ret->weights = weights;
  return p_ret;
}

double
perceptron_valid_auc(double *weights, int **indices, int size, int feature_size)
{
  double l;
  int j;
  l = 0.0;
  for(j=0; j<size; j++)
    {
      if(wtimesx(weights, indices[j], feature_size) > 0.0)
        {
          if(indices[j][feature_size] == 1)
            l+=1.0;
        }
      else
        {
          if(indices[j][feature_size] == 0)
            l+=1.0;
        }
    }
  //  printf("correct case, %lf\n", l);
  return l/size;
}

int
perceptron_update_weights(double *weights, int **indices, int size, double eta, int feature_size)
{
  int i,j;
  int od;
  int cont=0;
  double delta;
  for(i=0; i<size; i++)
    {
      od=0;
      if((wtimesx(weights, indices[i], feature_size))>0)
        od=1;
      for(j=0;j<feature_size;j++)
        {
          if(indices[i][j]==0 || indices[i][feature_size] == od)
            continue;
          delta = eta * indices[i][j] * (indices[i][feature_size] - od);
          //printf("%lf, %lf\n", delta, weights[j]);
          cont = delta==0.0?cont:1;
          weights[j] += delta;
        }
    }
  return cont;
}

int
apt_perceptron_update_weights(double *weights, int *indices, double eta, double lambda, int feature_size, int K, int T, int *a, double *u)
{
  int i,j;
  //int td = indices[feature_size]==0?-1:1;
  int cont=0;
  double delta;
  int od = wtimesx(weights, indices, feature_size)<0?0:1;
  for(j=0;j<feature_size;j++)
    {
      delta =  (indices[feature_size] - od) * indices[j] + lambda *weights[j] - lambda*u[a[j]];
      //  printf("%lf, %lf\n", delta, weights[j]);
      cont = delta==0.0?cont:1;
      weights[j] += delta * eta;
    }
  return cont;
}

int
pct_automatic_parameter_tying(double *weights, int **all_indices, int size, double eta, double lambda, int feature_size, int K, int T, int *a, double *u)
{
  int k=0,l=0,m;
  int i=0,j;
  double tu[K]; /* for tmp record */
  int tmp = 0;
  while(k<=T)
    {
      l = 0;
      if(apt_perceptron_update_weights(weights, all_indices[i], eta, lambda, feature_size, K, T, a, u)!=0)
        tmp = 1;
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
      //printf("K-means times: %d\n", l);
      k++;
      i++;
      if(i>=size)
        {
          i=0;
          if(tmp==0)
            break;
          tmp=0;
        }
    }
  printf("Iteration times: %d\n", k);
  return 0;
}

ptr_ret_train_lr
apt_train_perceptrons(int **indices, int size, int feature_size, double eta, int loop, double *weights, double lambda, int K, int T)
{
  int *a = calloc(feature_size, sizeof(int));
  double *u = calloc(K, sizeof(double));
  srand(time(NULL));
  int i;
  for(i=0; i<feature_size; i++)
    {
      weights[i] = 0.0;
      a[i] = rand()%K;
    }
  for(i=0; i<K; i++)
    {
      u[i] = 0.0;
    }
  ptr_ret_train_lr p_ret = calloc(1, sizeof(struct ret_train_lr));
  pct_automatic_parameter_tying(weights, indices, size, eta, lambda, feature_size, K, T, a, u);
  free(a);
  free(u);
  p_ret->weights = weights;
  return p_ret;
}
