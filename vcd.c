/////////////////////////////////////
// Lucas Mior e Pedro Henrique
// May, 11 2016
//
// Parser to vcd file
// VLSI II
/////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uthash.h"

struct my_struct* find_user(char* key);
void add_user(struct my_struct *s);
char* getLine(FILE* vcd);
char* getWord(char* line);
void getVariables(FILE* vcd);
void countingOccurrences(FILE* vcd);
void showingResults();
void getTiming(FILE* vcd);

struct my_struct {
  char key[10];         /* we'll use this field as the key */
  char state;
  int time0;
  int timecurr;
  int detected0;
  int frequence;            
  char module[30];             
  UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;
long double timing = 0;

int main()
{
  FILE *vcd;
  vcd = fopen("switch.vcd", "rt");
  if(vcd == NULL)
  {
    printf("Problem to open the file\n");
    return -1;
  } 
  getTiming(vcd);

  printf("Getting variables\n");
  getVariables(vcd);

  printf("\n");
  printf("Counting occurrences..\n");
  countingOccurrences(vcd);

  printf("\n");
  printf("Showing results:\n");
  showingResults();

  return 0;
}

/*
    #### METHDS ######
*/
void getTiming(FILE* vcd)
{

  // TODO can add strtok here.. instead line[1]..
  char *line = malloc(sizeof(char)*100);
  const char end[] = "$timescale";
  
  do{
    line = getLine(vcd);
  }while( strcmp(line, (char*)end) && !feof(vcd) );

  line = getLine(vcd);
  //printf("%s\n", line);
  int size = strlen(line);
  char* unit = malloc(2);
  strcpy(unit, &line[size-2]);

  if(!strcmp(unit,"ms"))
  {
    timing = (line[1]-0x30) * 1000;
  }
  else if(!strcmp(unit,"us"))
  {
    timing = (line[1]-0x30) * 1000 * 1000;
  }
  else if(!strcmp(unit,"ns"))
  {
    timing = (line[1]-0x30) * 1000 * 1000 * 1000;
  }  
}

void showingResults()
{
  struct my_struct* s;
  /**
  printf("+-------+------+\n");
  for(s=users; s != NULL; s=s->hh.next)
  {
    printf("|  %s\t|   %d\t|\n", s->key, s->frequence);
    printf("|  %s\t|t0 %d\t|\n", s->key, s->time0);
  }
  printf("+-------+-------+\n");
  **/
  char* key;
  char* time1;
  int t0 = 0, t1 = 9999;
  char* time0;
  int freq = 0;
  char* clk = "\"";

  for(s=users; s != NULL; s=s->hh.next)
  {
    // most changes
    if(strcmp(s->key,clk) && s->frequence > freq)
    {
      freq = s->frequence;
      key = s->key;
    }
    // most idle
    if(s->time0 > t0)
    {
      t0 = s->time0;
      time0 = s->key;
    }
    // less idle
    if(strcmp(s->key,clk) && s->time0 < t1)
    {
      t1 = s->time0;
      time1 = s->key;
    }
  }
  printf("Most changes was: %s with: %d changes\n", key, freq );
  printf("Most idle time was: %s with: %d changes\n", time0, t0 );
  printf("Less idle time was: %s with: %d changes\n", time1, t1 );
  printf("\n");

}
void calculateFrequence(FILE* vcd)
{
  char *line = malloc(sizeof(char)*100);
  line = getLine(vcd);
  //printf("%s\n",line );
  char* frequence = malloc(6);
  strcpy(frequence, line+1);
  frequence[strlen(frequence)] = '\0';
  int freq = atoi(frequence);
  float f = ((float)1/(float)freq)*timing;
  //printf("Frequence is %d\n", freq);
  printf("Frequence is %.0fHz\n", f);

}

void countingOccurrences(FILE* vcd)
{

  char *line = malloc(sizeof(char)*100);
  struct my_struct* su;
  char* key = malloc(4);
  char value;

  // dumpvars
  do{
    line = getLine(vcd);
    if(line != NULL)
    {
      strcpy(key, line+1);
      key[strlen(key)] = '\0';
      value = line[0];
      su = find_user(key);
      if(su != NULL)
      {
        su->state = value;
        if(su->state == '0')
        {
          su->detected0 = 1;
        }
      }
    }
  }while( strcmp(line, (char*)"$end") && !feof(vcd) );
  calculateFrequence(vcd);

  // changing values
  while(!feof(vcd))
  { 
    line = getLine(vcd);
    if(line != NULL)
    {
      //dumpall
      if(!strcmp(line,(char*)"$dumpall"))
      {
        break;
      }
      strcpy(key, line+1);
      key[strlen(key)] = '\0';
      value = line[0];
      
      su = find_user(key);
      if(su != NULL)
      {
        if(su->state != value && value != 'x')
        {
          su->frequence = su->frequence + 1;
          su->state = value;
          if(su->state == '0')
          {
            su->detected0 = 1;
          }
          else if(su->state == '1')
          {
            su->detected0 = 0;
          }
        }
      }
      else
      {
        for(su=users; su != NULL; su=su->hh.next)
        {
          if(su->detected0)
          {
            su->timecurr = su->timecurr + 1;
          }
          else
          {
            if(su->timecurr > su->time0)
            {
              su->time0 = su->timecurr;
            }
            su->timecurr = 0;
          }
        }     
      }     
    }
  }
  for(su=users; su != NULL; su=su->hh.next)
  {
    if((su->timecurr > su->time0) && (su->state == '0'))
    {
      su->time0 = su->timecurr;
    }
  }
}

void getVariables(FILE* vcd)
{
  char *line = malloc(sizeof(char)*100);
  char *word = malloc(sizeof(char)*100);
  char *module = malloc(sizeof(char)*30);
  const char space[] = " ";

  do{
    line = getLine(vcd);

    if(line != NULL)
    {
      strcpy(word, getWord(line));
      if( !strcmp(word, (char*)"$scope") )
      {
        strtok(NULL, space);
        module = strtok(NULL, space);
        //printf("Scope:: %s\n", module);
      }
      else if( !strcmp(word, (char*)"$var") )
      {
        strtok(NULL, space);
        strtok(NULL, space);
        word = strtok(NULL, space);
        struct my_struct* s;
        s = malloc(sizeof(struct my_struct));
        s->frequence = 0;
        s->time0 = 0;
        s->timecurr = 0;
        strcpy(s->module, module);
        strcpy(s->key, word);
        printf("New variable:: %s | %s\n", s->key, s->module);
        s->state = 'z';
        add_user(s);
      }
    }
  }while( strcmp(word, (char*)"$dumpvars") && !feof(vcd) );
}

char* getWord(char* line)
{
  char *word;
  if(line != NULL)
  {
    word = strtok(line, " ");
    return (char*)word;
  }
  return NULL;
}

char* getLine(FILE* vcd)
{
  char *line = malloc(sizeof(char)*100);
  size_t tam = 100;
  int n;

  n = getline(&line, &tam, vcd);
  if(n==0)
  {
  	return NULL;
  }
  line = strtok(line, "\n");
  return line;
}

/*
    #### UTHASH METHDS ######
*/
struct my_struct* find_user(char* key) {
  struct my_struct *s;

  HASH_FIND_STR( users, key, s );
  return s;
}

void add_user(struct my_struct *s) {
  HASH_ADD_STR( users, key, s );
}