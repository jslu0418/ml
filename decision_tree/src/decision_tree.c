/* implementation of decision tree functions */

#include "decision_tree.h"


char **
string_split (char *str1, char *delim1)
{
  char **ptr_strs;
  ptr_strs = calloc(NUM_FEATURES+1, sizeof(char *));
  char *str_buf;
  int i = 0;
  str_buf = strtok(str1, delim1);
  while (str_buf != NULL)
    {
      ptr_strs[i] = calloc(STRING_LENGTH, sizeof(char));
      strcpy(ptr_strs[i++], str_buf);
      str_buf = strtok(NULL, delim1);
    }
  return ptr_strs;
}

double
cal_entropy (int i, int j)
{
  if (i==0 || j==0)
    return (double) 0.0;
  double k = (double) i + (double) j;
  return -(((double) i)/k * log2(((double) i)/k) + ((double) j)/k * log2(((double) j)/k));
}

/* load data set */
ptr_ret_load_data
load_data (char *filename1)
{
  int array_size;
  int i;
  int j;
  array_size = 10;

  ptr_array_element *array1;
  ptr_feature feature1;
  array1 = array_init(array_size, sizeof(ptr_array_element));
  feature1 = calloc(1, sizeof(struct feature));

  char *str_buf;
  char **tmp_str;
  str_buf = calloc(BUFFER_STRING_SIZE, sizeof(char));

  FILE *data_txt;
  data_txt = fopen (filename1, "r");
  if (data_txt == NULL)
    {
      printf ("File could not be opened.\n");
      return NULL;
    }
  else
    {
      i = 0;
      while (fgets(str_buf, STRING_LENGTH, data_txt) != NULL)
        {
          if (i >= array_size)
            {
              array1 = (ptr_array_element*)(array_extend(array1, array_size, ARRAY_SIZE_INC, sizeof(ptr_array_element)));
              array_size = array_size + ARRAY_SIZE_INC;
            }
          array1[i] = calloc(1, sizeof(struct array_element));
          if (i == 0){
            feature1[0].features = string_split(str_buf, ",");
          }
          else {
            tmp_str = string_split(str_buf, ",");
            for (j=0; j<NUM_FEATURES; j++)
              {
                array1[i]->a[j] = atoi(tmp_str[j]); /* store feature value in array element */
              }
            array1[i]->c = atoi(tmp_str[NUM_FEATURES]); /* store current example's class */
          }
          i++;
        }
    }
  struct ret_load_data *ret = calloc(1, sizeof(struct ret_load_data));
  ret->a = i-1;
  ret->b = array1;
  ret->c = feature1;
  return ret;
}

/* growtree function */

ptr_node
growtree (ptr_node n, int option)
{
  if (n->nt0 == 0)
    {
      n->feature = -2; /* this feature is 1 */
      return n;
    }
  else if (n->nt1 == 0)
    {
      n->feature = -1; /* this feature is 0 */
      return n;
    }
  if (choose_attribute(n, option) < 0) /* if choose attribute exit with value -1 which means encounter contradictory examples */
    return n;
  n->n0 = growtree(n->n0, option); /* recursively grow a child tree */
  n->n1 = growtree(n->n1, option); /* similar */
  return n;
}

/* choose_attribute */
int
choose_attribute (ptr_node n, int option)
{
  int i, j, cur_feature;
  cur_feature = -3;
  int n00, n01, n10, n11, nt0, nt1;
  nt0 = nt1 = 0;
  int sum = n->nt0+n->nt1;
  /* four arrays for storing classified elements */
  ptr_array_element *t00 = calloc(n->nt0, sizeof(ptr_array_element));
  ptr_array_element *t01 = calloc(n->nt1, sizeof(ptr_array_element));
  ptr_array_element *t10 = calloc(n->nt0, sizeof(ptr_array_element));
  ptr_array_element *t11 = calloc(n->nt1, sizeof(ptr_array_element));
  double cur_entropy;
  /* calculate initial entropy and gain */
  if (option == 0)
    {
      cur_entropy = cal_entropy(n->nt0, n->nt1); /* algorithm 1 */
    }
  else
    {
      cur_entropy = (double) n->nt0 * n->nt1 / sum / sum; /* algorithm 2 */
    }
  double new_entropy = cur_entropy;
  double gain = cur_entropy - new_entropy;
  for (i=0; i<NUM_FEATURES; i++) /* loop all features */
    {
      n00 = n01 = n10 = n11 = 0;
      for (j=0; j<n->nt0; j++)
        {
          if (n->t0[j]->a[i] == 0)
            t00[n00++] = n->t0[j];
          else
            t10[n10++] = n->t0[j];
        }
      for (j=0; j<n->nt1; j++)
        {
          if (n->t1[j]->a[i] == 0)
            t01[n01++] = n->t1[j];
          else
            t11[n11++] = n->t1[j];
        }
      if (option == 0)
        {
          new_entropy = cal_entropy(n00, n01) * ((double) (n00 + n01))/((double) sum) + cal_entropy(n10, n11) * ((double) (n10 + n11))/((double) sum);
        }
      else
        {
          new_entropy = ((double) n00 * n01 / (n00+n01) / (n00+n01)) * (n00+n01)/sum + ((double) n10 * n11 / (n10+n11) / (n10+n11)) * (n10+n11)/sum;
        }

      if (cur_entropy - new_entropy >= gain && cur_entropy - new_entropy != 0)
        {
          gain = cur_entropy - new_entropy;
          cur_feature = i;
          nt0 = n00 + n01;
          nt1 = n10 + n11;
        }
    }
  ptr_array_element *t0 = calloc(nt0, sizeof(ptr_array_element));
  ptr_array_element *t1 = calloc(nt1, sizeof(ptr_array_element));
  if (cur_feature == -3) /* cannot classify more, use majority class */
    {
      if (n->nt0 >= n->nt1)
        n->feature = -1;
      else
        n->feature = -2;
      return -1;
    }
  n->feature = cur_feature; /* else, use the feature which give the most gain */
  nt0=nt1=0;
  /* construct the elements for two next level nodes */
  for (j=0; j<n->nt0; j++)
    {
      if (n->t0[j]->a[cur_feature] == 0)
        {
          t0[nt0++] = n->t0[j];
          n00++;
        }
      else
        {
          t1[nt1++] = n->t0[j];
          n10++;
        }
    }
  for (j=0; j<n->nt1; j++)
    {
      if (n->t1[j]->a[cur_feature] == 0)
        {
          t0[nt0++] = n->t1[j];
          n01++;
        }
      else
        {
          t1[nt1++] = n->t1[j];
          n11++;
        }
    }
  ptr_node n0 = calloc(1, sizeof(struct node));
  ptr_node n1 = calloc(1, sizeof(struct node));
  n0->t0 = calloc(n00, sizeof(ptr_array_element));
  n0->t1 = calloc(n01, sizeof(ptr_array_element));
  n1->t0 = calloc(n10, sizeof(ptr_array_element));
  n1->t1 = calloc(n11, sizeof(ptr_array_element));
  n00=n01=n10=n11=0;
  for (j=0; j<n->nt0; j++)
    {
      if (n->t0[j]->a[cur_feature] == 0)
        n0->t0[n00++] = n->t0[j];
      else
        n1->t0[n10++] = n->t0[j];
    }
  for (j=0; j<n->nt1; j++)
    {
      if (n->t1[j]->a[cur_feature] == 0)
        n0->t1[n01++] = n->t1[j];
      else
        n1->t1[n11++] = n->t1[j];
    }
  n0->nt0 = n00;
  n0->nt1 = n01;
  n1->nt0 = n10;
  n1->nt1 = n11;
  n0->dep = n->dep + 1;
  n1->dep = n->dep + 1;
  n->n0 = n0;
  n->n1 = n1;
  n->t0 = t0;
  n->t1 = t1;
  n->nt0 = n00+n01;
  n->nt1 = n10+n11;
  /* printf("dep:%d, %d, %d, %d\n", n->dep, n->nt0, n->nt1, n->feature); */
  /* printf("child_dep:%d, %d, %d\n", n->n0->dep, n->n0->nt0, n->n0->nt1); */
  /* printf("child_dep:%d, %d, %d\n", n->n1->dep, n->n1->nt0, n->n1->nt1); */
  return 0;
}

int
print_tree (ptr_node n, ptr_feature p_feature)
{
  if (n->feature == -1)
    {
      printf(" %d", 0);
      return 0;
    }
  else if (n->feature == -2)
    {
      printf(" %d", 1);
      return 0;
    }
  printf("\n");
  int i = 0;
  while (i++ < n->dep)
    printf("| ");
  printf("%s = 0 :", p_feature->features[n->feature]);
  print_tree(n->n0, p_feature);
  printf("\n");
  i = 0;
  while (i++ < n->dep)
    printf("| ");
  printf("%s = 1 :", p_feature->features[n->feature]);
  print_tree(n->n1, p_feature);
  return 0;
}

double
valid_line_by_tree (ptr_array_element e, ptr_node n)
{
  int i;
  ptr_node cur_node;
  cur_node = n;
  while (cur_node->feature != -1 && cur_node->feature != -2)
    {
      if (e->a[cur_node->feature] == 0)
        cur_node = cur_node->n0;
      else
        cur_node = cur_node->n1;
    }
  if (cur_node->feature + e->c == -1)
    return 1.0;
  else
    {
      return 0.0;
    }
}

double
valid_array_by_tree (ptr_array_element *array1, int size, ptr_node n)
{
  int i;
  double result = 0.0;
  for (i=0; i<size; i++)
    {
      result += valid_line_by_tree(array1[i], n);
    }
  return result/(double) size;
}

ptr_node
copy_tree(ptr_node n)
{
  ptr_node nn = calloc(1, sizeof(struct node));
  nn->feature = n->feature;
  nn->nt0 = n->nt0;
  nn->nt1 = n->nt1;
  nn->dep = n->dep;
  nn->t0 = n->t0;
  nn->t1 = n->t1;
  if (nn->feature != -1 && nn->feature != -2)
    {
      /* not leaf node, recursively copy */
      nn->n0 = copy_tree(n->n0);
      nn->n1 = copy_tree(n->n1);
    }
  return nn;
}

ptr_node
pruning_tree_with_dataset (ptr_node D, int L, int K, ptr_array_element *array1, int size, ptr_feature p_feature, int to_print)
{
  ptr_node D_new, D_best;
  int i, j, M, k, l, array_size;
  double acc, new_acc, pristine_acc;
  srand(time(NULL));
  acc = pristine_acc = valid_array_by_tree(array1, size, D);
  CIRCLEQ_INIT(&head);
  for (i=0; i<L; i++)
    {
      D_new = copy_tree(D);
      if (K <= 1) /* boundary */
        M = K;
      else
        M = rand()%(K-1);
      for (j=0; j<M; j++)
        {
          /* order all the nodes in current D_new */
          array_size = 100;
          ptr_node *nodes = (ptr_node*) array_init(array_size, sizeof(ptr_node));
          struct entry *e = calloc(1, sizeof(struct entry));
          e->n = D_new;
          CIRCLEQ_INSERT_TAIL(&head, e, entries);
          k=0;
          while (head.cqh_first != (void *) &head)
            {
              if (k >= array_size)
                {
                  nodes = (ptr_node*) array_extend(nodes, array_size, ARRAY_SIZE_INC, sizeof(ptr_node));
                  array_size = array_size + ARRAY_SIZE_INC;
                }
              if (head.cqh_first->n->feature >= 0)
                {
                  nodes[k++] = head.cqh_first->n;
                  struct entry *e0 = calloc(1, sizeof(struct entry));
                  struct entry *e1 = calloc(1, sizeof(struct entry));
                  e0->n = head.cqh_first->n->n0;
                  e1->n = head.cqh_first->n->n1;
                  CIRCLEQ_INSERT_TAIL(&head, e0, entries);
                  CIRCLEQ_INSERT_TAIL(&head, e1, entries);
                }
              CIRCLEQ_REMOVE(&head, head.cqh_first, entries);
            }
          if (j==0){
            /* printf("total nodes: %d\n", k); */
            if (k < M)
              {
                printf("K is too larger than total non-leaf node number of the tree, will adjust M to be a random integer between 1 to #non-leaf nodes.\n");
                if (k == 1) /* boundary */
                  M == 1;
                else
                  M = rand()%(k-1);
              }
          }
          if (k == 0)
            continue;
          if (k == 1) /* boundary */
            l = 0;
          else
            l = rand()%(k-1);
          ptr_node pruning_node = nodes[l];
          if (pruning_node->nt0 >= pruning_node->nt1)
            pruning_node->feature = -1;
          else
            pruning_node->feature = -2;
          free(nodes);
        }
      new_acc = valid_array_by_tree(array1, size, D_new);
      /* printf("new_acc: %lf\n", new_acc); */
      if (new_acc > acc) {
        D_best = D_new;
        acc = new_acc;
      }
    }
  if (to_print == 1)
    {
      printf("Decision tree after pruning:");
      print_tree(D_best, p_feature);
    }
  printf("\nAccuracy of original tree on validation set: %lf", pristine_acc);
  printf("\nAfter pruning, accuracy on validation set is %lf", acc);
  return D_best;
}
