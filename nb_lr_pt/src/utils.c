#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define TOKENSIZE 5000

char **
string_split (char *str1, char *delim1)
{
  char **ptr_strs;
  int tokensize = TOKENSIZE;
  ptr_strs = calloc(tokensize, sizeof(char *));
  char *str_buf;
  int i = 0;
  str_buf = strtok(str1, delim1);
  while (str_buf != NULL)
    {
      ptr_strs[i] = calloc(strlen(str_buf)+1, sizeof(char));
      strcpy(ptr_strs[i++], str_buf);
      str_buf = strtok(NULL, delim1);
      if (i>=tokensize)
        {
          ptr_strs = realloc(ptr_strs, (tokensize+TOKENSIZE)*sizeof(char*));
          tokensize += TOKENSIZE;
        }
    }
  ptr_strs[i] = calloc(1, sizeof(char));
  strcpy(ptr_strs[i], "");
  return ptr_strs;
}

int
indexof(int size, void **a, void *b, int (*cmp)(void *, void *))
{
  int i = 0;
  while (i < size)
    {
      if ((*cmp)(a[i], b) == 0)
        {
          return i;
        }
      i++;
    }
  return -1;
}

void
search(int *size, char ***a, char *b, int (*cmp)(void *, void *))
{
  int i = 0;
  while (i < *size)
    {
      if ((*cmp)((*a)[i], b) == 0)
        {
        }
      i++;
    }
  *size += 1;
  *a = realloc(*a, sizeof(b) * (*size));
  strcpy1((*a)[*size-1], b);
}
