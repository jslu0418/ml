#ifndef UTILS_H
#define UTILS_H
#include "common.h"

struct str_entry {
  SLIST_ENTRY(str_entry) ent;
  char *str;
};

char** string_split(char *str1, char *delim1);
int strcpy1(char *str1, char *str2);
void search(int *size, char ***a, char *b, int (*cmp)(void *, void *));

#define strcpy1(str1, str2) do {                                                 \
  str1 = calloc(strlen(str2)+10, sizeof(char));                                  \
  strcpy(str1, str2);                                                            \
} while (0)

#endif /* UTILS_H */
