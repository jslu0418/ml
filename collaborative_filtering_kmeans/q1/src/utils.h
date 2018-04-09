#ifndef UTILS_H
#define UTILS_H

typedef struct doc{
  char *text;
  int num_lines;
  int len;
} *ptr_doc;

char ** string_split (char *str1, char *delim1, int *num_lines);
int strcat1(ptr_doc p_doc1, char *str2);
char ** read_dir(char *dirname);
ptr_doc read_file1 (char *filename);
ptr_doc read_file2 (char *filename);

#endif /* UTILS_H */
