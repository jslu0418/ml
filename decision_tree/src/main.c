/* main.c: main program for homework1 of CS6375.502

 */


#include "decision_tree.h"

static struct feature *p_feature;
static int to_print;

/* main function */
int
main (int argc, char **argv)
{
  int L = atoi(argv[1]);
  int K = atoi(argv[2]);
  char *filename1 = argv[3]; /* training set */
  char *filename2 = argv[4]; /* validation set */
  char *filename3 = argv[5]; /* test set */
  char *to_print_c = argv[6];
  to_print = 0;
  if (strcmp(to_print_c, "yes") == 0)
    to_print = 1;
  ptr_array_element *array1;
  ptr_feature feature1;
  int training_array_size, i, option;
  ptr_ret_load_data ret1;
  /* training set */
  ret1 = load_data(filename1);
  training_array_size = ret1->a;
  array1 = ret1->b;
  feature1 = ret1->c;
  p_feature = ret1->c;
  /* validation set */
  ptr_array_element *validation_array1;
  int validation_array_size;
  ret1 = load_data(filename2);
  validation_array_size = ret1->a;
  validation_array1 = ret1->b;
  /* test set */
  ptr_array_element *test_array1;
  int test_array_size;
  ret1 = load_data(filename3);
  test_array_size = ret1->a;
  test_array1 = ret1->b;
  ptr_node root = calloc(1, sizeof(struct node));
  for (option=0; option<2; option++){
    printf("Constructing tree with algorithm %d.", option+1);
    root->nt0 = 0;
    root->nt1 = 0;
    int t0_size = 10;
    int t1_size = 10;
    root->t0 = calloc(t0_size, sizeof(ptr_array_element));
    root->t1 = calloc(t1_size, sizeof(ptr_array_element));
    for (i=0; i<training_array_size; i++)
      {
        /* printf("%d\n", array1[i]->c); */
        if(array1[i]->c == 0)
          {
            if (root->nt0 >= t0_size)
              {
                root->t0 = (ptr_array_element *)(array_extend(root->t0, t0_size, ARRAY_SIZE_INC, sizeof(ptr_array_element)));
                t0_size = t0_size + ARRAY_SIZE_INC;
              }
            root->t0[root->nt0]=array1[i];
            root->nt0++;
          }
        else
          {
            if (root->nt1 >= t1_size)
              {
                root->t1 = (ptr_array_element *)(array_extend(root->t1, t1_size, ARRAY_SIZE_INC, sizeof(ptr_array_element)));
                t1_size = t1_size + ARRAY_SIZE_INC;
              }
            root->t1[root->nt1]=array1[i];
            root->nt1++;
          }
      }
    /* printf("Total number of each class, 0:%d, 1:%d\n", root->nt0, root->nt1); */
    root->dep = 0;
    root = growtree(root, option);
    if (to_print == 1)
      {
        printf("\nOriginal Decision Tree before Pruning");
        print_tree(root, p_feature);
        printf("\n");
      }
    ptr_node D_best = pruning_tree_with_dataset(root, 100, 10, validation_array1, validation_array_size, p_feature, to_print);
    double test_acc;
    test_acc = valid_array_by_tree(test_array1, test_array_size, root);
    printf("\nAccuracy of original tree on test set is %lf", test_acc);
    test_acc = valid_array_by_tree(test_array1, test_array_size, D_best);
    printf("\nAfter pruning, accuracy on test set is %lf\n", test_acc);
  }
}
