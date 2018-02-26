#include "load_file.h"
#include "utils.h"

int
strcat1(ptr_doc p_doc1, char *str2)
{
  while (p_doc1->len < strlen(p_doc1->text) + strlen(str2) + 1)
    {
      p_doc1->text = realloc(p_doc1->text, p_doc1->len + TEXT_LENGTH);
      p_doc1->len += TEXT_LENGTH;
    }
  p_doc1->text = strcat(p_doc1->text, str2);
  return 0;
}

char **
readdir1(char *dirname)
{
  int fileno = FILENO;
  char **filenames = calloc(fileno, sizeof(char*));
  DIR *dir;
  struct dirent *file;
  dir = opendir(dirname);
  int i=0;
  int j;
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
  /* for (j=0; j<i; j++) */
  /*   { */
  /*     printf("%s\n", filenames[j]); */
  /*   } */
  return filenames;
}

ptr_doc
load_file (char *filename)
{
  FILE *data_txt;
  char str_buf[STRING_LENGTH];
  struct doc *doc1 = calloc(1, sizeof(struct doc));
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
          strcat1(doc1, " ");
          strcat1(doc1, str_buf);
        }
    }
  fclose(data_txt);
  char *tmptext;
  strcpy1(tmptext, doc1->text);
  doc1->tokens = string_split(tmptext, DELIM);
  return doc1;
}
