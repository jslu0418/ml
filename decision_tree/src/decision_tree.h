#ifndef DECISION_TREE_H
# define DECISION_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/queue.h>
#include "libarray.h"

#define STRING_LENGTH 1024
#define BUFFER_STRING_SIZE 1024000
#define ARRAY_SIZE_INC 100
#define NUM_FEATURES 20


/* struct storing features */
typedef struct feature{
  char **features;
} *ptr_feature;

/* struct storing training example */
typedef struct array_element{
  int a[NUM_FEATURES];
  int c;
} *ptr_array_element;

/* struct of decision tree node */
typedef struct node{
  int feature; /* node's feature's index, let -1 be '0' and -2 be '1' */
  int nt0; /* number of examples if this feature is 0 */
  int nt1; /* number of examples if this feature is 1 */
  int dep;
  ptr_array_element *t0;
  ptr_array_element *t1;
  struct node *n0;
  struct node *n1;
} *ptr_node;

/* struct of return value of load_data */
typedef struct ret_load_data{
  int a;
  ptr_array_element *b;
  ptr_feature c;
} *ptr_ret_load_data;

/* struct of queue */
CIRCLEQ_HEAD(circleq, entry) head;
struct circleq *headp;
struct entry {
  CIRCLEQ_ENTRY(entry) entries;
  ptr_node n;
};

/* pre-define functions */
char **string_split (char *str1, char *delim1); /* string split function */
double cal_entropy (int i, int j); /* calculate entropy */
ptr_ret_load_data load_data (char *filename1); /* load data from specified file */
ptr_node growtree (ptr_node n, int option); /* growtree function from pseudo code in slide */
int choose_attribute (ptr_node n, int option); /* choose attribute with two heuristics */
int print_tree (ptr_node n, ptr_feature p_feature); /* print tree function according to requirement */
double valid_line_by_tree (ptr_array_element e, ptr_node n); /* valid one example by given tree */
double valid_array_by_tree (ptr_array_element *array1, int size, ptr_node n); /* valid a data set by given tree */
ptr_node copy_tree (ptr_node n); /* copy a tree */
ptr_node pruning_tree_with_dataset (ptr_node D, int L, int K, ptr_array_element *array1, int size, ptr_feature p_feature, int to_print); /* pruning tree */

#endif /* !DECISION_TREE_H */
