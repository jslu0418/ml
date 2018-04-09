#include "common.h"

#define INC 10000
#define NUM_THREADS 15
#define NUM2_THREADS 15

struct p_thread_info {
  pthread_t t_id;
  int t_num;
  ptr_vecs p_vecs;
  float **cors;
};

struct m_thread_info {
  pthread_t t_id;
  int t_num;
  char **test_concepts;
  int total;
  ptr_vecs p_vecs;
  double abs_error;
  double root_error;
  float **cors;
};

ptr_vecs
prepare_data(char **examples, int num_lines)
{
  int i;
  ptr_vecs p_vec = calloc(1, sizeof(struct vecs));
  int NF;
  char strbuf[256];
  strcpy(strbuf, examples[num_lines-1]);
  char **fields = string_split(strbuf, ",", &NF);
  int max_movie_id = atoi(fields[0]);
  int max_user_id = atoi(fields[1]);
  short *movie_ids = calloc(max_movie_id+1, sizeof(short));
  short *user_ids = calloc(max_user_id+1, sizeof(short));
  int movie_id;
  int user_id;
  char rating = 0;
  int movies_len = INC;
  int users_len = INC;
  p_vec->movie_id_dict = calloc(movies_len, sizeof(int));
  p_vec->user_id_dict = calloc(users_len, sizeof(int));
  int j=0,k=0;
  for (i=0; i<num_lines; i++)
    {
      strcpy(strbuf, examples[i]);
      fields = string_split(strbuf, ",", &NF);
      movie_id = atoi(fields[0]);
      user_id = atoi(fields[1]);
      /* direct mapping for checking new user id */
      if(user_id > max_user_id)
        {
          user_ids = realloc(user_ids, (user_id + 1) * sizeof(short));
          memset(&user_ids[max_user_id], 0, (user_id - max_user_id) * sizeof(short));
          max_user_id = user_id;
        }
      /* direct mapping for checking new movie id */
      if(movie_id > max_movie_id)
        {
          movie_ids = realloc(movie_ids, (movie_id + 1) * sizeof(short));
          memset(&movie_ids[max_movie_id], 0, (movie_id - max_movie_id) * sizeof(short));
          max_movie_id = movie_id;
        }
      /* add to dict if new user id */
      if(user_ids[user_id] == 0)
        {
          if(k >= users_len)
            {
              users_len = users_len + INC;
              p_vec->user_id_dict = realloc(p_vec->user_id_dict, users_len * sizeof(int));
            }
          user_ids[user_id] = k;
          p_vec->user_id_dict[k] = user_id;
          k++;
        }
      /* add to dict if new movie id */
      if(movie_ids[movie_id] == 0)
        {
          if(j >= movies_len)
            {
              movies_len = movies_len + INC;
              p_vec->movie_id_dict = realloc(p_vec->movie_id_dict, movies_len * sizeof(int));
            }
          movie_ids[movie_id] = j;
          p_vec->movie_id_dict[j] = movie_id;
          j++;
        }
    }
  p_vec->user_len = k;
  p_vec->user_id_dict = realloc(p_vec->user_id_dict, k * sizeof(int));
  p_vec->movie_len = j;
  p_vec->movie_id_dict = realloc(p_vec->movie_id_dict, j * sizeof(int));
  p_vec->vecs = calloc(p_vec->user_len, sizeof(char *));
  for (i=0; i<p_vec->user_len; i++)
    {
      p_vec->vecs[i] = calloc(p_vec->movie_len, sizeof(char));
    }
  for (i=0; i<num_lines; i++)
    {
      fields = string_split(examples[i], ",", &NF);
      movie_id = atoi(fields[0]);
      user_id = atoi(fields[1]);
      rating = (char) atoi(fields[2]);
      p_vec->vecs[user_ids[user_id]][movie_ids[movie_id]] = rating;
    }
  p_vec->reverse_user_id_dict = user_ids;
  p_vec->reverse_movie_id_dict = movie_ids;
  return p_vec;
}

int
cal_correlation(ptr_vecs p_vecs, float **cors)
{
  int i,j,k,l;
  float sum;
  float sum_numerator;
  p_vecs->means = calloc(p_vecs->user_len, sizeof(float));
  for(i=0; i<p_vecs->user_len; i++)
    {
      k = 0;
      sum = 0.0;
      for(j=0; j<p_vecs->movie_len; j++)
        {
          if(p_vecs->vecs[i][j]==0)
            continue;
          sum += p_vecs->vecs[i][j];
          k++;
        }
      if(k!=0)
        {
          p_vecs->means[i] = sum/k;
        }
      else
        p_vecs->means[i] = (float) 0.0;
    }
  /* pthread version */
  struct p_thread_info *infos = calloc(NUM_THREADS, sizeof(struct p_thread_info));
  for(i=0; i<NUM_THREADS; i++)
    {
      infos[i].t_num = i;
      infos[i].p_vecs = p_vecs;
      infos[i].cors = cors;
      pthread_create(&infos[i].t_id, NULL, &pthread_cal_correlation, &infos[i]);
    }
  for(i=0; i<NUM_THREADS; i++)
    {
      pthread_join(infos[i].t_id, NULL);
    }
  return 0;
}

static void *
pthread_cal_correlation(void *args)
{
  struct p_thread_info *info = args;
  ptr_vecs p_vecs = info->p_vecs;
  float **cors = info->cors;
  float square_dis_aj, square_dis_ij;
  int i,j,k;
  float sum_numerator;
  for(i=info->t_num; i<p_vecs->user_len; i+=NUM_THREADS)
    {
      for(j=i+1; j<p_vecs->user_len; j++)
        {
          sum_numerator = 0.0;
          square_dis_aj = 0.0;
          square_dis_ij = 0.0;
          for(k=0; k<p_vecs->movie_len; k++)
            {
              if(p_vecs->vecs[i][k] == 0 || p_vecs->vecs[j][k] == 0)
                  continue;
              sum_numerator += (p_vecs->vecs[i][k] - p_vecs->means[i]) * (p_vecs->vecs[j][k] - p_vecs->means[j]);
              square_dis_aj += (p_vecs->vecs[i][k] - p_vecs->means[i]) * (p_vecs->vecs[i][k] - p_vecs->means[i]);
              square_dis_ij += (p_vecs->vecs[j][k] - p_vecs->means[j]) * (p_vecs->vecs[j][k] - p_vecs->means[j]);
            }
          cors[i][j] = sum_numerator/sqrt(square_dis_aj * square_dis_ij);
        }
    }
  return NULL;
}

int
measure(ptr_vecs p_vecs, float **cors, char **test_concepts, int total)
{
  int i;
  double abs_error = 0.0;
  double root_error = 0.0;
  struct m_thread_info *infos = calloc(NUM2_THREADS, sizeof(struct m_thread_info));
  for(i=0; i<NUM2_THREADS; i++)
    {
      infos[i].t_num = i;
      infos[i].p_vecs = p_vecs;
      infos[i].cors = cors;
      infos[i].test_concepts = test_concepts;
      infos[i].total = total;
      infos[i].abs_error = 0.0;
      infos[i].root_error = 0.0;
      pthread_create(&infos[i].t_id, NULL, &pthread_measure, &infos[i]);
    }
  for(i=0; i<NUM2_THREADS; i++)
    {
      pthread_join(infos[i].t_id, NULL);
      abs_error += infos[i].abs_error;
      root_error += infos[i].root_error;
    }
  abs_error = abs_error / total;
  root_error = sqrt(root_error / total);
  printf("MEAN ABSOLUTE ERROR: %lf\n", abs_error);
  printf("ROOT MEAN SQUARE ERROR: %lf\n", root_error);
  return 0;
}

static void *
pthread_measure(void *args)
{
  struct m_thread_info *info = args;
  ptr_vecs p_vecs = info->p_vecs;
  float **cors = info->cors;
  int total = info->total;
  int t_num = info->t_num;
  char **examples = info->test_concepts;
  int i,j,k;
  double sum_numerator;
  double normal_K;
  int user_id;
  int movie_id;
  int rating;
  char **fields;
  int NF;
  for(i=t_num; i<total; i+=NUM2_THREADS)
    {
      fields = string_split(examples[i], ",", &NF);
      movie_id = atoi(fields[0]);
      user_id = atoi(fields[1]);
      rating = atoi(fields[2]);
      user_id = p_vecs->reverse_user_id_dict[user_id];
      sum_numerator = 0.0;
      normal_K = 0.0;
      k = p_vecs->reverse_movie_id_dict[movie_id];
      for(j=0; j<p_vecs->user_len; j++)
        {
          if(j == user_id)
            continue;
          if(p_vecs->vecs[j][k] == 0)
            continue;
          if(user_id<j)
            {
              if(isnan(cors[user_id][j]) != 0)
                continue;
              sum_numerator += cors[user_id][j] * (p_vecs->vecs[j][k] - p_vecs->means[j]);
              normal_K += (cors[user_id][j]>0?cors[user_id][j]:cors[user_id][j]*(-1));
            }
          else
            {
              if(isnan(cors[j][user_id]) != 0)
                continue;
              sum_numerator += cors[j][user_id] * (p_vecs->vecs[j][k] - p_vecs->means[j]);
              normal_K += (cors[j][user_id]>0?cors[j][user_id]:cors[j][user_id]*(-1));
            }
        }
      sum_numerator = sum_numerator/normal_K + p_vecs->means[user_id];
      if(isnan(sum_numerator) != 0 )
        continue;
      info->abs_error += sum_numerator > rating ? sum_numerator - rating : rating - sum_numerator;
      info->root_error += (sum_numerator - rating) * (sum_numerator - rating);
    }
  return NULL;
}
