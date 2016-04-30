#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uthash.h"

char* getLine(FILE* vcd);
char* getWord(char* line);

struct my_struct {
  int id;
  int key;
  int frequence;            /* we'll use this field as the key */
  char module[30];             
  UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;

void add_user(struct my_struct *s) {
  HASH_ADD_INT( users, key, s );    
}

struct my_struct* find_user(int user_id) {
  struct my_struct *s;

  HASH_FIND_INT( users, &user_id, s );
  return s;
}

int main( )
{
  FILE *vcd;
  vcd = fopen("test.vcd", "rt");
  if(vcd == NULL)
  {
    printf("Problem to open the file\n");
    return -1;
  } 
  
  char *line = malloc(sizeof(char)*100);
  char *word = malloc(sizeof(char)*100);
  char *module = malloc(sizeof(char)*30);
  const char end[] = "$dumpvars";
  const char space[2] = " ";
  
  printf("Getting variables\n");
  do{
    line = getLine(vcd);
    if(line != NULL)
    {

      strcpy(word, getWord(line));
      if( !strcmp(word, (char*)"$scope") )
      {
        strtok(NULL, space);
        module = strtok(NULL, space);
        printf("Scope:: %s\n", module);
      }
      else if( !strcmp(word, (char*)"$var") )
      {
        strtok(NULL, space);
        strtok(NULL, space);
        word = strtok(NULL, space);
        printf("Var:: %s\n", word);
        struct my_struct* s;
        s = malloc(sizeof(struct my_struct));
        s->frequence = 0;
        strcpy(s->module, module);
        s->key = *word;
        s->id = (int)*word;
        add_user(s); 
      }
    }
    //else { printf("!VARIABLE> %s %d\n", word, strlen(word)); printf("strcpm %d\n", strcmp(word, end));}// printf("%s %d\n", end, strlen(end));}
    //else skip
    //printf("%s\n",word );
    //printf("%d\n", !feof(vcd));
  }while( strcmp(word, (char*)end) && !feof(vcd) );

  printf("HERE\n");
/*
  struct my_struct* su;
  su = malloc(sizeof(struct my_struct));
  //strcpy( su, find_user('#'));
  su = find_user((int)'#');
  printf("HEREE\n");
  if (su == NULL)
  {
    printf("NIL\n");
  }
  else{ 
    su->frequence = 43;
    printf("%s\n", su->module );
    su = find_user((int)'#');
    printf("%d\n", su->frequence );
    //TODO
  }*/
  printf("Counting occurences\n");
  while(!feof(vcd))
  {
    line = getLine(vcd);
    if(line != NULL)
    {
      line = strtok(line, "\n");
      printf("strlen: %ld %s\n", strlen(line), line);
    }
  }
  printf("Showing results\n");
  //TODO

  return 0;
}

char* getWord(char* line)
{
  char *word;
  char *l = malloc(sizeof(char)*100);
  const char space[2] = " ";

  if(line != NULL)
  {
    strcpy(l, line);
    word = strtok(l, space);
    // TODO Verify if it is needed.
    //printf("strlen line: %d %s\n", strlen(line), line );
    //printf("strlen word: %d %s\n", strlen(word), word );
    if(strlen(line) == strlen(word) )
    {
      const char k[]="\n";
      word[strlen(word)-2] = '\0';
    //  word = strtok(word, k);
      //printf("strlen word: %d %s\n", strlen(word), word );
    }
    return (char*)word;
  }
  return NULL;
}
char* getLine(FILE* vcd)
{
  char *line = malloc(sizeof(char)*100);
  if(fgets(line, 100, vcd) != NULL)
  {
    return line;
  }
  return NULL;
}
