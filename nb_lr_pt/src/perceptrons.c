#include "perceptrons.h"
#include "logistic_regression.h"

ptr_ret_train_lr
train_perceptrons(int **indices, int size, int feature_size, double eta, int loop, double *weights)
{
  ptr_ret_train_lr p_ret = calloc(1, sizeof(struct ret_train_lr));
  int i;
  /* /\* initialize weights *\/ */
  /* double *weights = calloc(feature_size, sizeof(double)); */
  /* for(i=0; i<feature_size; i++) */
  /*   { */
  /*     weights[i] = 0.0; */
  /*   } */
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
