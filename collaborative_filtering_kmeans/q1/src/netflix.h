#ifndef NETFLIX_H
#define NETFLIX_H

typedef struct vecs{
  char **vecs;
  float *means;
  int *user_id_dict;
  short *reverse_user_id_dict;
  short *reverse_movie_id_dict;
  int *movie_id_dict;
  int user_len; /* row */
  int movie_len; /* col */
} *ptr_vecs;

ptr_vecs prepare_data(char **examples, int num_lines);
int cal_correlation(ptr_vecs p_vecs, float **cors);
static void * pthread_cal_correlation(void *args);
int measure(ptr_vecs p_vecs, float **cors, char **test_concepts, int total);
static void * pthread_measure(void *args);
#endif /* NETFLIX_H */
