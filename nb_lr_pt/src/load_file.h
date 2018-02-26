#ifndef LOAD_FILE_H
#define LOAD_FILE_H

#include "common.h"
#define DELIM " \n\t.,:\"<>`!~@#$%^&*()-_=+{}[];|\\/?"

#define STRING_LENGTH 200
#define TEXT_LENGTH 5000
#define FILENO 500

typedef struct doc {
  char* text;
  char** tokens;
  int len;
  int* indices;
} *ptr_doc;

typedef struct class_docs {
  char* class;
  ptr_doc *docs;
  int len;
} *ptr_class_docs;

int strcat1(ptr_doc p_doc1, char *str2);
ptr_doc load_file (char *filename);
char ** readdir1(char *dirname);

#endif /* LOAD_FILE_H */
