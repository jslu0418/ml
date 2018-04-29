#include "common.h"
/* #define PRINTTREE */
#define BAGGING_PORTION (2.0/3.0)

/* struct of queue */
struct ent{
  CIRCLEQ_ENTRY(ent) entries;
  struct node *n;
};

CIRCLEQ_HEAD(circleq, ent) head;

int
cal_probs (struct bn *bn1)
{
  struct node **nodes = bn1->nodes;
  int **data = bn1->data;
  struct edge **edges = bn1->edges;
  int num_node = bn1->num_node;
  int num_data = bn1->num_data;
  int i;
  int j;
  int k;
  double prob[4];
  double counts[num_node * 2];
  for (j=0; j<num_node * 2; j++)
    {
      counts[j] = 1.0; /* add-one laplace smoothing */
    }
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<num_node; j++)
        {
          counts[j*2 + data[i][j]] += 1;
          for (k=j+1; k<num_node; k++)
            {
              edges[j*num_node + k]->count[data[i][j]*2+data[i][k]] += 1;
              edges[j*num_node + k]->sum += 1;
            }
        }
    }
  for (j=0; j<num_node; j++)
    {
      nodes[j]->ll_cpt[1] = log(counts[j*2 + 1]/(counts[j*2] + counts[j*2 + 1]));
      nodes[j]->ll_cpt[0] = log(counts[j*2]/(counts[j*2] + counts[j*2 + 1]));
    }
  for (j=0; j<num_node; j++)
    {
      for (k=j+1; k<num_node; k++)
        {
          prob[0] = edges[j*num_node + k]->count[0]/edges[j*num_node + k]->sum;
          prob[1] = edges[j*num_node + k]->count[1]/edges[j*num_node + k]->sum;
          prob[2] = edges[j*num_node + k]->count[2]/edges[j*num_node + k]->sum;
          prob[3] = edges[j*num_node + k]->count[3]/edges[j*num_node + k]->sum;
          edges[j*num_node + k]->mi = prob[0] * (log(prob[0]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[0]) \
          + prob[1] * (log(prob[1]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[1]) \
          + prob[2] * (log(prob[2]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[0]) \
          + prob[3] * (log(prob[3]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[1]);
          /* printf("%d:%d, %f\n", j, k, edges[j*num_node + k]->mi); */
        }
      /* printf("%f, %f\n", nodes[j]->ll_cpt[1], nodes[j]->ll_cpt[0]); */
    }
  return 0;
}

int
cal_bagging_probs (struct bn *bn1, int *bagging_data_index, int num_data)
{
  struct node **nodes = bn1->nodes;
  int **data = bn1->data;
  struct edge **edges = bn1->edges;
  int num_node = bn1->num_node;
  int i;
  int j;
  int k;
  double prob[4];
  double counts[num_node * 2];
  for (j=0; j<num_node * 2; j++)
    {
      counts[j] = 1.0; /* add-one laplace smoothing */
    }
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<num_node; j++)
        {
          counts[j*2 + data[bagging_data_index[i]][j]] += 1;
          for (k=j+1; k<num_node; k++)
            {
              edges[j*num_node + k]->count[data[bagging_data_index[i]][j]*2+data[bagging_data_index[i]][k]] += 1;
              edges[j*num_node + k]->sum += 1;
            }
        }
    }
  for (j=0; j<num_node; j++)
    {
      nodes[j]->ll_cpt[1] = log(counts[j*2 + 1]/(counts[j*2] + counts[j*2 + 1]));
      nodes[j]->ll_cpt[0] = log(counts[j*2]/(counts[j*2] + counts[j*2 + 1]));
    }
  for (j=0; j<num_node; j++)
    {
      for (k=j+1; k<num_node; k++)
        {
          prob[0] = edges[j*num_node + k]->count[0]/edges[j*num_node + k]->sum;
          prob[1] = edges[j*num_node + k]->count[1]/edges[j*num_node + k]->sum;
          prob[2] = edges[j*num_node + k]->count[2]/edges[j*num_node + k]->sum;
          prob[3] = edges[j*num_node + k]->count[3]/edges[j*num_node + k]->sum;
          edges[j*num_node + k]->mi = prob[0] * (log(prob[0]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[0]) \
          + prob[1] * (log(prob[1]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[1]) \
          + prob[2] * (log(prob[2]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[0]) \
          + prob[3] * (log(prob[3]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[1]);
          /* printf("%d:%d, %f\n", j, k, edges[j*num_node + k]->mi); */
        }
      /* printf("%f, %f\n", nodes[j]->ll_cpt[1], nodes[j]->ll_cpt[0]); */
    }
  return 0;
}

/* only different between this and cal_probs is count is add with P_k(i) */
int
cal_mixture_em_probs (struct bn *bn1)
{
  double *gamma = bn1->gamma;
  double sum_gamma = bn1->sum_gamma;
  int num_node = bn1->num_node;
  int num_data = bn1->num_data;
  int i;
  int j;
  int k;
  double prob[4];
  double counts[num_node * 2];
  int **data = bn1->data;
  struct node **nodes = bn1->nodes;
  struct edge **edges = bn1->edges;
  double cur_gamma;
  for (j=0; j<num_node * 2; j++)
    {
      counts[j] = 1.0; /* add-one laplace smoothing */
    }
  for (i=0; i<num_data; i++)
    {
      cur_gamma = gamma[i]/sum_gamma;
      for (j=0; j<num_node; j++)
        {
          counts[j*2 + data[i][j]] += cur_gamma;
          for (k=j+1; k<num_node; k++)
            {
              edges[j*num_node + k]->count[data[i][j]*2+data[i][k]] += cur_gamma;
              edges[j*num_node + k]->sum += cur_gamma;
            }
        }
    }
  for (j=0; j<num_node; j++)
    {
      nodes[j]->ll_cpt[1] = log(counts[j*2 + 1]/(counts[j*2] + counts[j*2 + 1]));
      nodes[j]->ll_cpt[0] = log(counts[j*2]/(counts[j*2] + counts[j*2 + 1]));
    }
  for (j=0; j<num_node; j++)
    {
      for (k=j+1; k<num_node; k++)
        {
          prob[0] = edges[j*num_node + k]->count[0]/edges[j*num_node + k]->sum;
          prob[1] = edges[j*num_node + k]->count[1]/edges[j*num_node + k]->sum;
          prob[2] = edges[j*num_node + k]->count[2]/edges[j*num_node + k]->sum;
          prob[3] = edges[j*num_node + k]->count[3]/edges[j*num_node + k]->sum;
          edges[j*num_node + k]->mi = prob[0] * (log(prob[0]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[0]);
          edges[j*num_node + k]->mi += prob[1] * (log(prob[1]) - nodes[j]->ll_cpt[0] - nodes[k]->ll_cpt[1]);
          edges[j*num_node + k]->mi += prob[2] * (log(prob[2]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[0]);
          edges[j*num_node + k]->mi += prob[3] * (log(prob[3]) - nodes[j]->ll_cpt[1] - nodes[k]->ll_cpt[1]);
          /* printf("%d:%d, %f\n", j, k, edges[j*num_node + k]->mi); */
        }
      /* printf("%f, %f\n", nodes[j]->ll_cpt[1], nodes[j]->ll_cpt[0]); */
    }
  return 0;
}

int
cal_mixture_em_data_probs(struct bn* bn1)
{
  int i,j;
  int num_node = bn1->num_node;
  int num_data = bn1->num_data;
  int **data = bn1->data;
  struct node **nodes = bn1->nodes;
  double log_weight = log(bn1->weight);
  bn1->sum_gamma = 0.0;
  for (i=0; i<num_data; i++)
    {
      bn1->gamma[i] = 0.0;
    }
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<num_node; j++)
        {
          bn1->gamma[i] += nodes[j]->ll_xipa_cpt[data[i][j]*2+data[i][nodes[j]->pa]];
        }
      bn1->gamma[i] += log_weight;
    }
  return 0;
}

int
edge_sort(const void *e1, const void *e2)
{
  if((*(struct edge **)e1)->mi > (*(struct edge **)e2)->mi)
    return -1;
  else if((*(struct edge **)e1)->mi == (*(struct edge **)e2)->mi)
    return 0;
  return 1;
}

struct bn **
mixture_em(struct bn *bn1, int k)
{
  int num_node = bn1->num_node;
  int num_data = bn1->num_data;
  struct bn **bns = calloc (k, sizeof(struct bn *));
  double weights[k];
  /* for calculating gamma */
  double sum_mixture_data_probs;
  double sum_weights = 0.0;
  double ll = 0.0;
  int i,j,m,n;
  int stop;
  srand(time(NULL));
  for (i=0; i<k; i++)
    {
      weights[i] = rand()%(k*k*k) + 1;
      bns[i] = calloc(1, sizeof(struct bn));
      bns[i]->data = bn1->data;
      bns[i]->num_node = num_node;
      bns[i]->num_data = bn1->num_data;
      bns[i]->gamma = calloc(bn1->num_data, sizeof(double));
      bns[i]->nodes = calloc(num_node, sizeof(struct node*));
      bns[i]->edges = calloc(num_node * num_node, sizeof(struct edge *));
      sum_weights += weights[i];
    }
  for (i=0; i<k; i++)
    {
      weights[i] = weights[i]/sum_weights;
      bns[i]->weight = weights[i];
    }

  for (i=0; i<k; i++)
    {
      for (m=0; m<num_node; m++)
        {
          bns[i]->nodes[m] = calloc(1, sizeof(struct node));
          bns[i]->nodes[m]->node_num = m;
          bns[i]->nodes[m]->ll_cpt[0] = bn1->nodes[m]->ll_cpt[0];
          bns[i]->nodes[m]->ll_cpt[1] = bn1->nodes[m]->ll_cpt[1];
        }
      /* start initialize all node struct */
      for (n=0; n<num_node; n++)
        {
          for (m=n+1; m<num_node; m++)
            {
              bns[i]->edges[n*num_node+m] = calloc(1, sizeof(struct edge));
              bns[i]->edges[n*num_node+m]->count[0]=1;
              bns[i]->edges[n*num_node+m]->count[1]=1;
              bns[i]->edges[n*num_node+m]->count[2]=1;
              bns[i]->edges[n*num_node+m]->count[3]=1;
              bns[i]->edges[n*num_node+m]->sum = 4;
              bns[i]->edges[n*num_node+m]->from = n;
              bns[i]->edges[n*num_node+m]->to = m;
              bns[i]->edges[n*num_node+m]->mi = 0.0;
            }
        }
    }
  int bagging_num_data = num_data * BAGGING_PORTION;
  int *bagging_data_index = calloc (bagging_num_data, sizeof(int));
  for (i=0; i<k; i++)
    {
      for(j=0;j<bagging_num_data;j++)
        {
          bagging_data_index[j] = rand()%num_data;
        }
      cal_bagging_probs(bns[i], bagging_data_index, bagging_num_data);
      structure_learning(bns[i], 0);
      for(n=0;n<num_node;n++)
        for(m=n+1;m<num_node;m++)
          {
            bns[i]->edges[n*num_node+m]->count[0]=1;
            bns[i]->edges[n*num_node+m]->count[1]=1;
            bns[i]->edges[n*num_node+m]->count[2]=1;
            bns[i]->edges[n*num_node+m]->count[3]=1;
            bns[i]->edges[n*num_node+m]->sum = 4;
            bns[i]->edges[n*num_node+m]->from = n;
            bns[i]->edges[n*num_node+m]->to = m;
            bns[i]->edges[n*num_node+m]->mi = 0.0;
          }
    }
  free(bagging_data_index);
  j = 0;
  while (j<100)
    {
      stop = 1; /* stop signal */
      ll = 0.0;
      sum_weights = 0.0;
      /* calculate statistics for learning chow-liu tree */
      for (i=0; i<k; i++)
        {
          cal_mixture_em_data_probs(bns[i]);
        }
      /* calculate gamma for each tree */
      for(m=0; m<num_data; m++)
        {
          sum_mixture_data_probs = 0.0;
          for (i=0; i<k; i++)
            {
              sum_mixture_data_probs += exp(bns[i]->gamma[m]);
            }
          for (i=0; i<k; i++)
            {
              bns[i]->gamma[m] = exp(bns[i]->gamma[m])/sum_mixture_data_probs;
              bns[i]->sum_gamma += bns[i]->gamma[m];
            }
        }
      /* calculate delta (p_i) */
      for (i=0; i<k; i++)
        {
          weights[i] = bns[i]->sum_gamma;
          sum_weights += weights[i];
        }
      for (i=0; i<k; i++)
        {
          weights[i] = weights[i]/sum_weights;
          /* printf("%d: %lf, %lf\n", j, bns[i]->weight, weights[i]); */
          if ((bns[i]->weight - weights[i] > 0 && bns[i]->weight - weights[i] > 0.000001)||(bns[i]->weight - weights[i] < 0 && bns[i]->weight - weights[i] < -0.000001))
            {
              stop = 0;
              bns[i]->weight = weights[i];
            }
        }
      if (stop == 0)
        {
          for (i=0; i<k; i++)
            {
              cal_mixture_em_probs(bns[i]);
              structure_learning(bns[i], 0);
              /* reset count */
              for(n=0;n<num_node;n++)
                for(m=n+1;m<num_node;m++)
                  {
                    bns[i]->edges[n*num_node+m]->count[0]=1;
                    bns[i]->edges[n*num_node+m]->count[1]=1;
                    bns[i]->edges[n*num_node+m]->count[2]=1;
                    bns[i]->edges[n*num_node+m]->count[3]=1;
                    bns[i]->edges[n*num_node+m]->sum = 4;
                    bns[i]->edges[n*num_node+m]->from = n;
                    bns[i]->edges[n*num_node+m]->to = m;
                    bns[i]->edges[n*num_node+m]->mi = 0.0;
                  }
            }
        }
      if (stop == 1)
          break;
      j++;
    }
  return bns;
}

double
cal_mixture_em_log_likelihood (struct bn **bns, int k, struct bn *bn2)
{
  double ll = 0;
  int i,j,m;
  int num_data = bn2->num_data;
  int num_node = bns[0]->num_node;
  int **data = bn2->data;
  double gamma;
  for (j=0; j<k; j++)
    {
      bns[j]->sum_gamma = 0.0;
      free(bns[j]->gamma);
      bns[j]->gamma = calloc(num_data, sizeof(double));
    }
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<k; j++){
        bns[j]->gamma[i] = log(bns[j]->weight);
      }
    }
  for (i=0; i<num_data; i++)
    {
      gamma = 0.0;
      for(m=0; m<k; m++)
        {
          for (j=0; j<num_node; j++)
            {
              bns[m]->gamma[i] += bns[m]->nodes[j]->ll_xipa_cpt[data[i][j]*2+data[i][bns[m]->nodes[j]->pa]];
            }
          gamma += exp(bns[m]->gamma[i]);
        }
      for(m=0; m<k; m++)
        {
          ll += (exp(bns[m]->gamma[i])/gamma) * bns[m]->gamma[i];
        }
    }
  return ll;
}

double
cal_mixture_bagging_log_likelihood(struct bn *bn1, int k, struct bn *bn2, double *lls)
{
  int num_node = bn1->num_node;
  int num_data = bn1->num_data;
  struct node **nodes = bn1->nodes;
  int **data = bn2->data;
  int bagging_num_data = num_data * BAGGING_PORTION;
  int i,j,m;
  int *bagging_data_index = calloc (bagging_num_data, sizeof(int));
  double weights[k];
  for(m=0; m<k; m++)
    {
      weights[m] = log(lls[m]);
    }
  double ll_sum[bn2->num_data];
  double ll_sum_nw[bn2->num_data];
  double ll = 0.0;
  double ll_nw = 0.0;
  double max;
  for (i=0; i<bn2->num_data; i++)
    ll_sum[i] = ll_sum_nw[i] = 0.0;
  srand(time(NULL));
  for(m=0; m<k; m++)
    {
      lls[m] = 0.0;
      for(i=0;i<bagging_num_data;i++)
        {
          bagging_data_index[i] = rand()%num_data;
        }
      for(i=0;i<num_node;i++)
        for(j=i+1;j<num_node;j++)
          {
            bn1->edges[i*num_node+j]->count[0]=1;
            bn1->edges[i*num_node+j]->count[1]=1;
            bn1->edges[i*num_node+j]->count[2]=1;
            bn1->edges[i*num_node+j]->count[3]=1;
            bn1->edges[i*num_node+j]->sum = 4;
            bn1->edges[i*num_node+j]->from = i;
            bn1->edges[i*num_node+j]->to = j;
            bn1->edges[i*num_node+j]->mi = 0;
          }
      cal_bagging_probs(bn1, bagging_data_index, bagging_num_data);
      structure_learning(bn1, 0);
      lls[m] = cal_tree_log_likelihood(bn1, bn2);
      /* # This is for preventing inf # */
      /* for (i=0; i<bn2->num_data; i++) */
      /*   { */
      /*     for (j=0; j<num_node; j++) */
      /*       { */
      /*         lls[m] += nodes[j]->ll_xipa_cpt[data[i][j]*2 + data[i][nodes[j]->pa]]; */
      /*         ll_sum_nw[i] += exp(nodes[j]->ll_xipa_cpt[data[i][j]*2 + data[i][nodes[j]->pa]]); */
      /*         ll_sum[i] += exp(nodes[j]->ll_xipa_cpt[data[i][j]*2 + data[i][nodes[j]->pa]]) * weights[m]; */
      /*       } */
      /*   } */
    }
  /* for(i=0; i<bn2->num_data; i++) */
  /*   { */
  /*     ll += log(ll_sum[i]); */
  /*     ll_nw += log(ll_sum_nw[i]); */
  /*   } */
  for(m=0; m<k; m++)
    {
      if(m==0)
        max = lls[m];
      else
        {
          if (max < lls[m])
            max = lls[m];
        }
    }
  for(m=0; m<k; m++)
    {
      ll += exp(lls[m] - max);
    }
  ll += max;
  for(m=0; m<k; m++)
    {
      lls[m] = exp(lls[m] - ll);
    }
  free(bagging_data_index);
  return ll;
}

int
structure_learning (struct bn *bn1, int a_root)
{
  struct edge **edges = bn1->edges;
  int num_node = bn1->num_node;
  int i,j,k=0;
  struct edge **edge_ptrs = calloc((num_node * (num_node-1)) / 2, sizeof(struct edge*));
  for (i=0; i<num_node; i++)
    for (j=i+1; j<num_node; j++)
      {
        edge_ptrs[k] = edges[i*num_node+j];
        k++;
      }
  qsort((void *)edge_ptrs, (num_node*(num_node-1)) /2, sizeof(struct edge*), &edge_sort);
  /* for (k=0; k<(num_node*(num_node-1)) /2; k++) */
  /*   printf("%d,%d: %f\n", edge_ptrs[k]->from, edge_ptrs[k]->to, edge_ptrs[k]->mi); */
  get_maximum_weight_spanning_tree(bn1, edge_ptrs, a_root);
  free(edge_ptrs);
  return 0;
}

int
get_maximum_weight_spanning_tree (struct bn *bn1, struct edge **edge_ptrs, int a_root)
{
  struct node **nodes = bn1->nodes;
  int num_node = bn1->num_node;
  int node_s_indices[num_node][num_node];
  int node_indices[num_node];
  memset((void *)node_indices, 0, sizeof(int) * num_node);
  memset((void *)node_s_indices, 0, sizeof(int) * num_node * num_node);
  int j,k,l;
  int i=0;
  struct edge **tree_edges = calloc(num_node-1, sizeof(struct edge*));
  for (j=0; j<num_node; j++)
    node_s_indices[j][j] = 1;
  for (k=0; k<(num_node*(num_node-1))/2; k++)
    {
      if(i>=num_node-1)
        break;
      if(node_s_indices[edge_ptrs[k]->from][edge_ptrs[k]->to]==0&&node_s_indices[edge_ptrs[k]->to][edge_ptrs[k]->from]==0)
        {
          tree_edges[i] = edge_ptrs[k];
          node_s_indices[edge_ptrs[k]->from][edge_ptrs[k]->to] = 1;
          node_s_indices[edge_ptrs[k]->to][edge_ptrs[k]->from] = 1;
          for (j=0;j<num_node;j++){
            if (node_s_indices[edge_ptrs[k]->from][j]==1)
              {
                for (l=0; l<num_node;l++)
                  {
                    if(node_s_indices[edge_ptrs[k]->to][l]==1)
                      {
                        node_s_indices[j][l] = 1;
                        node_s_indices[l][j] = 1;
                      }
                  }
              }
          }
          for (j=0;j<num_node;j++){
            if (node_s_indices[edge_ptrs[k]->to][j]==1)
              {
                for (l=0; l<num_node;l++)
                  {
                    if(node_s_indices[edge_ptrs[k]->from][l]==1)
                      {
                        node_s_indices[j][l] = 1;
                        node_s_indices[l][j] = 1;
                      }
                  }
              }
          }
          i++;
        }
    }
  /* for bfs */
  struct list_edge head;
  struct list_edge *cur;
  struct list_edge *next;
  head.cur = NULL;
  head.next = NULL;
  for (k=0; k<num_node-1; k++)
    {
      cur = calloc(1, sizeof(struct list_edge));
      cur->cur = tree_edges[k];
      cur->next = head.next;
      head.next = cur;
    }

  /* set node 0 be the root */
  struct circleq cqhead;
  CIRCLEQ_INIT(&cqhead);
  struct ent *e;
  e = calloc(1, sizeof(struct ent));
  srand(time(NULL));
  bn1->root = a_root;
  e->n = nodes[bn1->root];
  CIRCLEQ_INSERT_TAIL(&cqhead, e, entries);
  struct node *x;
  x = e->n;
  x->pa = 0;
  x->ll_xipa_cpt[0] = x->ll_xipa_cpt[1] = x->ll_cpt[0];
  x->ll_xipa_cpt[2] = x->ll_xipa_cpt[3] = x->ll_cpt[1];
  memset((void *)node_indices, 0, sizeof(int) * num_node);
  struct list_edge *edge;
  while(cqhead.cqh_first != (void *)&cqhead)
    {
      /* if cur node -> list_edge != NULL, continue */
      x = cqhead.cqh_first->n;
      if (x->list_edge == NULL)
        {
          edge = calloc(1, sizeof(struct list_edge));
          edge->next = NULL;
          edge->cur = NULL;
          cur = &head;
          while(cur->next != NULL)
            {
              next = cur->next;
              if(next->cur->from==x->node_num||next->cur->to==x->node_num)
                {
                  if(next->cur->to == x->node_num)
                    {
                      next->cur->to = next->cur->from;
                      next->cur->from = x->node_num;
                    }
                  e = calloc(1,sizeof(struct ent));
                  e->n = nodes[next->cur->to];
                  CIRCLEQ_INSERT_TAIL(&cqhead, e, entries);
                  nodes[next->cur->to]->pa = x->node_num;
                  if(next->cur->from > next->cur->to)
                    {
                      nodes[next->cur->to]->ll_xipa_cpt[0] = log(next->cur->count[0]/(next->cur->count[0]+next->cur->count[1]));
                      nodes[next->cur->to]->ll_xipa_cpt[1] = log(next->cur->count[1]/(next->cur->count[0]+next->cur->count[1]));
                      nodes[next->cur->to]->ll_xipa_cpt[2] = log(next->cur->count[2]/(next->cur->count[2]+next->cur->count[3]));
                      nodes[next->cur->to]->ll_xipa_cpt[3] = log(next->cur->count[3]/(next->cur->count[2]+next->cur->count[3]));
                    }
                  else
                    {
                      nodes[next->cur->to]->ll_xipa_cpt[0] = log(next->cur->count[0]/(next->cur->count[0]+next->cur->count[2]));
                      nodes[next->cur->to]->ll_xipa_cpt[1] = log(next->cur->count[1]/(next->cur->count[1]+next->cur->count[3]));
                      nodes[next->cur->to]->ll_xipa_cpt[2] = log(next->cur->count[2]/(next->cur->count[0]+next->cur->count[2]));
                      nodes[next->cur->to]->ll_xipa_cpt[3] = log(next->cur->count[3]/(next->cur->count[1]+next->cur->count[3]));
                    }
                  cur->next = next->next;
                  next->next = edge->next;
                  edge->next = next;
                }
              else{
                cur = next;
              }
            }
          x->list_edge = edge;
        }
      CIRCLEQ_REMOVE(&cqhead, cqhead.cqh_first, entries);
    }
  free(tree_edges);
  return 0;
}



int
print_tree(struct bn *bn1)
{
  struct node **nodes = bn1->nodes;
  struct list_edge *edge;
  struct circleq head;
  CIRCLEQ_INIT(&head);
  struct ent *e;
  e = calloc(1, sizeof(struct ent));
  e->n = nodes[bn1->root];
  CIRCLEQ_INSERT_TAIL(&head, e, entries);
#ifdef PRINTTREE
  printf("digraph G {\n");
#endif
  /* int printed[bn1->num_node]; */
  /* memset((void *)printed, 0, sizeof(int) * bn1->num_node); */
  while(head.cqh_first != (void *)&head)
    {
      /* if(printed[head.cqh_first->n->node_num]==0){ */
        edge = head.cqh_first->n->list_edge;
        while(edge->next != NULL)
          {
            edge = edge->next;
#ifdef PRINTTREE
            printf("\"%d\" -> \"%d\";\n", edge->cur->from, edge->cur->to);
#endif
            e = calloc(1, sizeof(struct ent));
            e->n = nodes[edge->cur->to];
            CIRCLEQ_INSERT_TAIL(&head, e, entries);
          }
      /*   printed[head.cqh_first->n->node_num]=1; */
      /* } */
      CIRCLEQ_REMOVE(&head, head.cqh_first, entries);
    }
#ifdef PRINTTREE
  printf("}\n");
#endif
  return 0;
}

double
cal_log_likelihood (struct bn *bn1, struct bn *bn2)
{
  struct node **nodes = bn1->nodes;
  int **data = bn2->data;
  int num_node = bn1->num_node;
  int num_data = bn2->num_data;
  double ll = 0.0;
  int i,j;
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<num_node; j++)
        {
          ll += nodes[j]->ll_cpt[data[i][j]];
        }
    }
  return ll;
}

double
cal_tree_log_likelihood (struct bn *bn1, struct bn *bn2)
{
  struct node **nodes = bn1->nodes;
  int **data = bn2->data;
  int num_node = bn1->num_node;
  int num_data = bn2->num_data;
  double ll = 0.0;
  int i,j;
  for (i=0; i<num_data; i++)
    {
      for (j=0; j<num_node; j++)
        {
          /* printf("%d, %d, %d\n", data[i][j], data[i][nodes[j]->pa], nodes[j]); */
          ll += nodes[j]->ll_xipa_cpt[data[i][j]*2+data[i][nodes[j]->pa]];
        }
    }
  return ll;
}

p_bn
preprocess_data (ptr_doc doc1)
{
  struct bn *bn1 = calloc (1, sizeof(struct bn));
  int i,j,tmp,num;
  char **lines = string_split(doc1->text, "\n", &bn1->num_data);
  char **fields;
  if (bn1->num_data > 0)
    {
      fields = string_split(lines[0], ",", &bn1->num_node);
      free(lines[0]);
      bn1->nodes = calloc(bn1->num_node, sizeof(struct node*));
      bn1->data = calloc(bn1->num_data, sizeof(int *));
      bn1->data[0] = calloc(bn1->num_node, sizeof(int));
      for (j=0; j<bn1->num_node; j++)
        {
          /* start initialize all node struct */
          bn1->nodes[j] = calloc(1, sizeof(struct node));
          bn1->nodes[j]->node_num = j;
          /* end */
          tmp = atoi(fields[j]);
          free(fields[j]);
          bn1->data[0][j] = tmp;
        }
      free(fields);
      for (i=1; i<bn1->num_data; i++)
        {
          fields = string_split(lines[i], ",", &num);
          free(lines[i]);
          bn1->data[i] = calloc(bn1->num_node, sizeof(int));
          for(j=0; j<bn1->num_node; j++)
            {
              tmp = atoi(fields[j]);
              free(fields[j]);
              bn1->data[i][j] = tmp;
            }
          free(fields);
        }
      bn1->edges = calloc(bn1->num_node * bn1->num_node , sizeof(struct edge *));
      for (i=0; i<bn1->num_node; i++)
        for (j=i+1; j<bn1->num_node; j++)
          {
            bn1->edges[i*bn1->num_node + j] = calloc(1, sizeof(struct edge));
            bn1->edges[i*bn1->num_node + j]->count[0] = 1;
            bn1->edges[i*bn1->num_node + j]->count[1] = 1;
            bn1->edges[i*bn1->num_node + j]->count[2] = 1;
            bn1->edges[i*bn1->num_node + j]->count[3] = 1;
            bn1->edges[i*bn1->num_node + j]->sum = 4; /* add-one laplace smoothing */
            bn1->edges[i*bn1->num_node + j]->from = i;
            bn1->edges[i*bn1->num_node + j]->to = j;
          }
    }
  free(lines);
  return bn1;
}


void
free_bns(struct bn **bns, int k)
{
  int i;
  for (i=0; i<k; i++)
    {
      free(bns[i]->gamma);
      free_bn_node(bns[i]);
      free_bn_edge(bns[i]);
      free(bns[i]);
    }
  free(bns);
}


void
free_bn(struct bn *bn1)
{
  if(bn1->data != NULL)
    free_bn_data(bn1);
  if(bn1->nodes != NULL)
    free_bn_node(bn1);
  if(bn1->edges != NULL)
    free_bn_edge(bn1);
  if(bn1->gamma != NULL)
    free(bn1->gamma);
  free(bn1);
}

void
free_bn_data(struct bn *bn1)
{
  int j;
  for(j=0;j<bn1->num_data;j++)
    free(bn1->data[j]);
  free(bn1->data);
  bn1->data = NULL;
}

void
free_bn_node(struct bn *bn1)
{
  int i;
  struct list_edge *e, *f;
  for(i=0; i<bn1->num_node; i++)
    {
      e = bn1->nodes[i]->list_edge;
      if (e != NULL){
        while(e->next != NULL)
          {
            f = e->next;
            e->next = e->next->next;
            free(f);
          }
        free(e);
      }
      free(bn1->nodes[i]);
    }
  free(bn1->nodes);
  bn1->nodes = NULL;
}

void
free_bn_edge(struct bn *bn1)
{
  int i,j;
  for (i=0; i<bn1->num_node; i++)
    for (j=i+1; j<bn1->num_node; j++)
      free(bn1->edges[i*bn1->num_node+j]);
  free(bn1->edges);
  bn1->edges = NULL;
}
