#include "naive_bayes.h"

#define VOCAB_SIZE 21000 /* size and increment */
#define APT

int
cmpstr(void *a, void *b)
{
  return strcmp((char *) a, (char *) b);
}

struct hsearch_data *
create_vocab_hash_by_tokens(char **tokens, char ***p_vocab)
{
  struct hsearch_data htab, *p_htab;
  p_htab = &htab;
  memset((void *)&htab, 0, sizeof(htab));
  (*p_vocab) = calloc(VOCAB_SIZE, sizeof(char*));
  ENTRY e;
  ENTRY *ep;
  int i = 0; /* track item in tokens */
  int j = 0; /* track item in vocabulary */
  hcreate_r(VOCAB_SIZE*sizeof(ENTRY), &htab);
  while (strcmp(tokens[i], "") != 0)
    {
      e.key = tokens[i];
      e.data = (void *) NULL;
      if((hsearch_r(e, FIND, &ep, &htab)) == 0)
        {
          if(hsearch_r(e, ENTER, &ep, &htab) == 0)
            {
              printf("enter failed\n");
            }
          else
            {
              ep->data = (int *)calloc(1, sizeof(int));
              (*(int*)ep->data) = j;
              strcpy1((*p_vocab)[j], e.key);
              j++;
            }
        }
      i++;
    }
  strcpy1((*p_vocab)[j], "");
  return p_htab;
}

struct hsearch_data *
create_hash_by_tokens(char **tokens)
{
  struct hsearch_data htab, *p_htab;
  p_htab = &htab;
  memset((void *)&htab, 0, sizeof(htab));  ENTRY e;
  ENTRY *ep;
  int i = 0; /* track item in tokens */
  hcreate_r(VOCAB_SIZE*sizeof(ENTRY), &htab);
  while (strcmp(tokens[i], "") != 0)
    {
      //  printf("%d, %s", i, tokens[i]);
      e.key = tokens[i];
      e.data = (void *) NULL;
      if((hsearch_r(e, FIND, &ep, &htab)) != 0)
        {
          (* (int*) ep->data)++;
        }
      else
        {
          e.data = (int *)calloc(1, sizeof(int));
          (*(int*) e.data) = 1;
          if(hsearch_r(e, ENTER, &ep, &htab) == 0)
            {
              printf("enter failed\n");
            }
        }
      i++;
    }
  return p_htab;
}


ptr_ret_train_nb
train_multinomial_nb (char** classes, ptr_class_docs *docs)
{
  ptr_ret_train_nb ret = calloc(1, sizeof(struct ret_train_nb));
  int i = 0;
  int j;
  int N = 0;
  int num_classes;
  int vocab_size;
  while (strcmp(classes[i],"") != 0)
    {
      N += docs[i++]->len;
    }
  num_classes = i;
  double *priors = calloc(num_classes, sizeof(double));
  int N_c[num_classes]; /* save different class N_c */
  /* struct for save concatenation documents */
  struct doc *doc1 = calloc(1, sizeof(struct doc));
  doc1->text = calloc(TEXT_LENGTH, sizeof(char));
  doc1->len = TEXT_LENGTH;
  strcpy(doc1->text, "");
  i = 0;
  while (i < num_classes)
    {
      N_c[i] = docs[i]->len;
      priors[i] = (double) N_c[i] / (double) N;
      /* concatenate all docs */
      for(j=0; j<N_c[i]; j++)
        {
          strcat1(doc1, " ");
          strcat1(doc1, docs[i]->docs[j]->text);
        }
      i++;
    }
  char **tokens = string_split(doc1->text, DELIM);
  /* extract vocab and save a hash table with every token's index in vocab[vocab_size]*/
  char **vocab;
  char ***p_vocab = &vocab;
  struct hsearch_data htab_vocab = *(create_vocab_hash_by_tokens(tokens, p_vocab));
  i = 0;
  vocab = *p_vocab;
  while(strcmp(vocab[i], "") != 0){
    i++;
  }
  vocab_size = i;
  printf("vocab_size:%d\n", vocab_size);

  /* calculate */
  int T_ct[vocab_size];
  int T;
  double **condprob = calloc(vocab_size, sizeof(double *));
  for (i=0; i<vocab_size; i++){
    condprob[i] = calloc(num_classes, sizeof(double));
  }
  double **aptcondprob = calloc(vocab_size, sizeof(double *));
  for (i=0; i<vocab_size; i++){
    aptcondprob[i] = calloc(num_classes, sizeof(double));
  }
  struct hsearch_data token_htab;
  ENTRY e, *ep;
  for (i=0; i<num_classes;i++)
    {
      T = 0;
      /* concatenate all docs in a class */
      strcpy(doc1->text, "");
      for(j=0; j<N_c[i]; j++)
        {
          strcat1(doc1, " ");
          strcat1(doc1, docs[i]->docs[j]->text);
        }
      tokens = string_split(doc1->text, DELIM);
      token_htab = *(create_hash_by_tokens(tokens));
      for(j=0; j<vocab_size; j++)
        {
          e.key = vocab[j];
          T_ct[j] = 0;
          if(hsearch_r(e, FIND, &ep, &token_htab) != 0)
            {
              T_ct[j] = * (int *) ep->data;
              T += T_ct[j];
            }
        }
      for(j=0; j<vocab_size; j++)
        {
          /* laplace smoothing */
          condprob[j][i] = (double) (T_ct[j] + 1) / (T + vocab_size);
        }
      hdestroy_r(&token_htab);

    }
  /* use reentrant hash table */
  ret->htab_vocab = htab_vocab;
  ret->priors = priors;
  ret->condprobs = condprob;
  ret->size_vocab = vocab_size;
  ret->size_prior = num_classes;
  return ret;
}

int
apply_multinomial_nb(char **classes, double *priors, double **condprob, ptr_doc p_doc, struct hsearch_data htab_vocab)
{
  int i,j,k;
  ENTRY e, *ep;
  j = 0;
  while(strcmp(classes[j], "") != 0)
    {
      j++;
    }
  //char **W = string_split(p_doc->text, " \n\t.,:\"<>`!~@#$%^&*()-_=+{}[];|\\/?");
  char **W = p_doc->tokens;
  int num_classes = j;
  double score[num_classes];
  j = 0;
  while(j < num_classes)
    {
      score[j] = 0.0;
      j++;
    }
  j = 0;
  while(j < num_classes)
    {
      score[j] = log(priors[j]);
      i = 0;
      while(strcmp(W[i], "") != 0)
        {
          e.key = W[i];
          if(hsearch_r(e, FIND, &ep, &htab_vocab) != 0)
            {
              k = (*(int*)ep->data);
              score[j] += log(condprob[k][j]);
            }
          i++;
        }
      j++;
    }
  double max_score = -1;
  int max_class = -1;
  j = 0;
  while(j < num_classes)
    {
      if (max_class == -1)
        {
          max_score = score[j];
          max_class = j;
          j++;
          continue;
        }
      if(max_score < score[j])
        {
          max_score = score[j];
          max_class = j;
        }
      j++;
    }
  return max_class;
}


int
nb_init_center(double **condprobs, int class, int vocab_size, int *a, double *uprobs, int K)
{
  /* also update center */
  int i,j;
  int *uu = calloc(K, sizeof(int));
  for(i=0;i<K;i++)
    {
      uprobs[i]=0.0;
      uu[i]=0;
    }
  for(j=0;j<vocab_size;j++)
    {
      uprobs[a[j]]+=condprobs[j][class];
      uu[a[j]]++;
    }
  for(i=0;i<K;i++)
    {
      uprobs[i]=uprobs[i]/uu[i];
      //  printf("%lf,%d\n", uprobs[i], i);
    }
  free(uu);
  return 0;
}

int
nb_reassignment(double **condprobs, int class, int vocab_size, int *a, double *uprobs, int K)
{
  int j,i;
  double min = INFINITY;
  double tmp;
  int g;
  for(j=0;j<vocab_size;j++)
    {
      g = a[j];
      for(i=0;i<K;i++)
        {
          tmp = condprobs[j][class]-uprobs[i];
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

int
nb_automatic_parameter_tying(double **condprobs, int num_classes, int vocab_size, int K, double **aptcondprobs)
{
  double *uprobs = calloc(K, sizeof(double));
  int *a = calloc(vocab_size, sizeof(int));
  int i,j,k,l,m;
  double tu[K];
  srand(time(NULL));
  for(k=0;k<num_classes;k++)
    {
      for(i=0;i<vocab_size;i++)
        a[i] = rand()%K;
      nb_init_center(condprobs, k, vocab_size, a, uprobs, K);
      l=0;
      while(l<30)
        {
          m = 0;
          nb_reassignment(condprobs, k, vocab_size, a, uprobs, K);
          for(j=0;j<K;j++)
            tu[j]=uprobs[j];
          nb_init_center(condprobs, k, vocab_size, a, uprobs, K);
          for(j=0;j<K;j++)
            {
              if(tu[j]!=uprobs[j])
                {
                  m = 1;
                  break;
                }
            }
          if(m==0)
            break;
          l++;
        }
      for(j=0;j<vocab_size;j++)
        {
          aptcondprobs[j][k] = uprobs[a[j]];
        }
    }
  return 0;
}

ptr_ret_train_nb
apt_train_multinomial_nb (char** classes, ptr_class_docs *docs, int K, double pct)
{
  ptr_ret_train_nb ret = calloc(1, sizeof(struct ret_train_nb));
  int i = 0;
  int j;
  int N = 0;
  int num_classes;
  int vocab_size;
  while (strcmp(classes[i],"") != 0)
    {
      N += docs[i++]->len*pct; /* pct 0.7 or 1.0 */
    }
  num_classes = i;
  double *priors = calloc(num_classes, sizeof(double));
  int N_c[num_classes]; /* save different class N_c */
  /* struct for save concatenation documents */
  struct doc *doc1 = calloc(1, sizeof(struct doc));
  doc1->text = calloc(TEXT_LENGTH, sizeof(char));
  doc1->len = TEXT_LENGTH;
  strcpy(doc1->text, "");
  i = 0;
  while (i < num_classes)
    {
      N_c[i] = docs[i]->len*pct;
      priors[i] = (double) N_c[i] / (double) N;
      /* concatenate all docs */
      for(j=0; j<N_c[i]; j++)
        {
          strcat1(doc1, " ");
          strcat1(doc1, docs[i]->docs[j]->text);
        }
      i++;
    }
  char **tokens = string_split(doc1->text, DELIM);
  /* extract vocab and save a hash table with every token's index in vocab[vocab_size]*/
  char **vocab;
  char ***p_vocab = &vocab;
  struct hsearch_data htab_vocab = *(create_vocab_hash_by_tokens(tokens, p_vocab));
  i = 0;
  vocab = *p_vocab;
  while(strcmp(vocab[i], "") != 0){
    i++;
  }
  vocab_size = i;
  //printf("vocab_size:%d\n", vocab_size);

  /* calculate */
  int T_ct[vocab_size];
  int T;
  double **condprob = calloc(vocab_size, sizeof(double *));
  for (i=0; i<vocab_size; i++){
    condprob[i] = calloc(num_classes, sizeof(double));
  }
  double **aptcondprob = calloc(vocab_size, sizeof(double *));
  for (i=0; i<vocab_size; i++){
    aptcondprob[i] = calloc(num_classes, sizeof(double));
  }
  struct hsearch_data token_htab;
  ENTRY e, *ep;
  for (i=0; i<num_classes;i++)
    {
      T = 0;
      /* concatenate all docs in a class */
      strcpy(doc1->text, "");
      for(j=0; j<N_c[i]; j++)
        {
          strcat1(doc1, " ");
          strcat1(doc1, docs[i]->docs[j]->text);
        }
      tokens = string_split(doc1->text, DELIM);
      token_htab = *(create_hash_by_tokens(tokens));
      for(j=0; j<vocab_size; j++)
        {
          e.key = vocab[j];
          T_ct[j] = 0;
          if(hsearch_r(e, FIND, &ep, &token_htab) != 0)
            {
              T_ct[j] = * (int *) ep->data;
              T += T_ct[j];
            }
        }
      for(j=0; j<vocab_size; j++)
        {
          /* laplace smoothing */
          condprob[j][i] = (double) (T_ct[j] + 1) / (T + vocab_size);
        }
      hdestroy_r(&token_htab);
      nb_automatic_parameter_tying(condprob, num_classes, vocab_size, K, aptcondprob);
    }
  /* use reentrant hash table */
  ret->htab_vocab = htab_vocab;
  ret->priors = priors;
  free(condprob);
  ret->aptcondprobs = aptcondprob;
  ret->size_vocab = vocab_size;
  ret->size_prior = num_classes;
  return ret;
}
