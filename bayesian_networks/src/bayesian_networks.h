#ifndef BAYESIAN_NETWORKS_H
#define BAYESIAN_NETWORKS_H
#include "utils.h"

typedef struct list_edge {
    struct list_edge *next;
    struct edge *cur;
} *p_list_edge;

typedef struct node {
  int pa;
  int node_num;
  double ll_cpt[2];
  double ll_xipa_cpt[4];
  struct list_edge *list_edge;
} *p_cpt;

typedef struct bn {
  double weight; /* delta or p_i */
  double *gamma; /* each line data's weight of Z = i */
  double sum_gamma; /* sum gamma */
  struct node **nodes;
  struct edge **edges;
  int **data;
  int num_node;
  int num_data;
  int num_edge;
  int root;
} *p_bn;

typedef struct edge {
  int from;
  int to;
  double count[4]; /* 00, 01, 10, 11 */
  double sum; /* sum */
  double mi;
} *p_edge;


int cal_probs (struct bn *bn1);
int cal_bagging_probs (struct bn *bn1, int *bagging_data_index, int num_data);
double cal_log_likelihood (struct bn *bn1, struct bn *bn2);
double cal_tree_log_likelihood (struct bn *bn1, struct bn *bn2);
p_bn preprocess_data (ptr_doc doc1);
void free_bn(p_bn bn1);
void free_bn_data(p_bn bn1);
void free_bn_node(p_bn bn1);
void free_bn_edge(p_bn bn1);
void free_bns(p_bn *bns, int k);
int get_maximum_weight_spanning_tree (struct bn *bn1, struct edge **edge_ptrs, int a_root);
int structure_learning (struct bn *bn1, int a_root);
int print_tree(struct bn *bn1);
struct bn **mixture_em(struct bn *bn1, int k);
int cal_mixture_em_probs (struct bn *bn1);
int cal_mixture_em_data_probs (struct bn *bn1);
double cal_mixture_em_log_likelihood (struct bn **bns, int k, struct bn *bn2);
double cal_mixture_bagging_log_likelihood(struct bn *bn1, int k, struct bn *bn2, double *lls);
#endif /* BAYESIAN_NETWORKS_H */
