#include "common.h"

#define FILENO 100
#define STRING_LENGTH 1024
#define TEXT_LENGTH 1024*1024
#define TOKENSIZE 5000

char **
string_split (char *str1, char *delim1, int *num_lines)
{
  char **ptr_strs;
  char *save_ptr;
  int tokensize = TOKENSIZE;
  ptr_strs = calloc(tokensize, sizeof(char *));
  char *str_buf;
  int i = 0;
  str_buf = strtok_r(str1, delim1, &save_ptr);
  while (str_buf != NULL)
    {
      ptr_strs[i] = calloc(strlen(str_buf)+1, sizeof(char));
      strcpy(ptr_strs[i], str_buf);
      i++;
      str_buf = strtok_r(NULL, delim1, &save_ptr);
      if (i>=tokensize)
        {
          ptr_strs = realloc(ptr_strs, (tokensize+TOKENSIZE)*sizeof(char*));
          tokensize += TOKENSIZE;
        }
    }
  ptr_strs = realloc(ptr_strs, (i+1) * sizeof(char*));
  ptr_strs[i] = calloc(1, sizeof(char));
  strcpy(ptr_strs[i], "");
  *num_lines = i;
  return ptr_strs;
}

int
strcat1(ptr_doc p_doc1, char *str2)
{
  while (p_doc1->len < (int) strlen(p_doc1->text) + (int) strlen(str2) + 1)
    {
      p_doc1->text = realloc(p_doc1->text, p_doc1->len + TEXT_LENGTH);
      p_doc1->len += TEXT_LENGTH;
    }
  p_doc1->text = strcat(p_doc1->text, str2);
  return 0;
}

char **
read_dir(char *dirname)
{
  int fileno = FILENO;
  char **filenames = calloc(fileno, sizeof(char*));
  DIR *dir;
  struct dirent *file;
  dir = opendir(dirname);
  int i=0;
  while((file = readdir(dir)) != NULL)
    {
      if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        continue;
      if(i>fileno)
        {
          filenames = realloc(filenames, (fileno+FILENO)*sizeof(char*));
        }
      filenames[i] = calloc(strlen(file->d_name) + 1, sizeof(char));
      strcpy(filenames[i], file->d_name);
      i++;
    }
  closedir(dir);
  filenames[i] = calloc(1, sizeof(char));
  strcpy(filenames[i],"");
  /* int j; */
  /* for (j=0; j<i; j++) */
  /*   { */
  /*     printf("%s\n", filenames[j]); */
  /*   } */
  return filenames;
}

ptr_doc
read_file1 (char *filename)
{
  FILE *data_txt;
  char str_buf[STRING_LENGTH];
  struct doc *doc1 = calloc(1, sizeof(struct doc));
  doc1->num_lines = 0;
  doc1->text = calloc(TEXT_LENGTH, sizeof(char));
  doc1->len = TEXT_LENGTH;
  strcpy(doc1->text, "");
  data_txt = fopen (filename, "r");
  if (data_txt == NULL)
    {
      printf ("File: %s could not be opened.\n", filename);
    }
  else
    {
      while (fgets(str_buf, STRING_LENGTH, data_txt) != NULL)
        {
          strcat1(doc1, str_buf);
          doc1->num_lines++;
          printf("# %d\n", doc1->num_lines);
        }
    }
  fclose(data_txt);
  return doc1;
}

ptr_doc
read_file2 (char *filename)
{
  FILE *data_txt;
  char str_buf[TEXT_LENGTH];
  size_t nsize = 0;
  struct doc *doc1 = calloc(1, sizeof(struct doc));
  doc1->num_lines = 0;
  doc1->len = TEXT_LENGTH;
  doc1->text = calloc(TEXT_LENGTH, sizeof(char));
  doc1->filename = calloc(strlen(filename)+1, sizeof(char));
  strcpy(doc1->filename, filename);
  strcpy(doc1->text, "");
  data_txt = fopen (filename, "r");
  if (data_txt == NULL)
    {
      printf ("File: %s could not be opened.\n", filename);
    }
  else
    {
      while((nsize = fread(str_buf, sizeof(char), TEXT_LENGTH, data_txt)) != 0)
        {
          if(nsize < TEXT_LENGTH)
            str_buf[nsize] = 0;
          strcat1(doc1, str_buf);
        }
    }
  doc1->text = realloc(doc1->text, (strlen(doc1->text) + 1 ) * sizeof(char));
  fclose(data_txt);
  return doc1;
}

int
free_doc(ptr_doc doc1)
{
  free(doc1->text);
  free(doc1->filename);
  free(doc1);
  return 0;
}
