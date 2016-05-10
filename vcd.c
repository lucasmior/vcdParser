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
  char last;
  int time1;
  int time0;
  int count;
  int timecurr;
  int frequence;            
  char module[30];             
  UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;
long double timing = 0;

int main()
{
  FILE *vcd;
  vcd = fopen("kriti.vcd", "rt");
  if(vcd == NULL)
  {
    printf("Problem to open the file\n");
    return -1;
  } 
  getTiming(vcd);

  printf("Getting variables\n");
  getVariables(vcd);

  printf("\n");
  printf("Counting occurrences\n");
  countingOccurrences(vcd);

  printf("\n");
  printf("Showing results\n");
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
    printf("|  %s\t|t1 %d\t|\n", s->key, s->time1);
  }
  printf("+-------+-------+\n");
  **/
  char* key;
  char* time1;
  int t0 = 0, t1 = 0;
  char* time0;
  int freq = 0;
  char* clk = "\"";

  for(s=users; s != NULL; s=s->hh.next)
  {
    if(strcmp(s->key,clk) && s->frequence > freq)
    {
      freq = s->frequence;
      //printf("new key is: %s\n", s->key );
      key = s->key;
    }
    if(s->time0 > t0)
    {
      t0 = s->time0;
      //printf("new key is: %s\n", s->key );
      time0 = s->key;
    }
    if(s->time1 > t1)
    {
      t1 = s->time1;
      //printf("new key is: %s\n", s->key );
      time1 = s->key;
    }
  }
  printf("Most changes was: %s with: %d changes\n", key, freq );
  printf("Most time0 was: %s with: %d changes\n", time0, t0 );
  printf("Most time1 was: %s with: %d changes\n", time1, t1 );
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
  const char end[] = "$end";
  // dumpvars
  do{
    line = getLine(vcd);
    if(line != NULL)
    {
      char* key = malloc(4);
      char value;
      
      //TODO fix it, gambiarra rules!
      // Just work for bits values
      // array doesnt work
      strcpy(key, line+1);
      key[strlen(key)] = '\0';
      value = line[0];
      su = find_user(key);
      if(su != NULL)
      {
        su->state = value;
      }
    }
  }while( strcmp(line, (char*)end) && !feof(vcd) );
  calculateFrequence(vcd);

  // changing values
  while(!feof(vcd))
  { 
    line = getLine(vcd);
    if(line != NULL)
    {
      char* key = malloc(4);
      char value;
      
      //TODO fix it, gambiarra rules!
      // Just work for bits values
      // array doesnt work
      strcpy(key, line+1);
      key[strlen(key)] = '\0';
      value = line[0];
      
      //printf("key:: %s | ", key);
      //printf("value: %c\n", value);

      su = find_user(key);
      if(su != NULL)
      {
        su->last = su->state;
        // dumpvars
        if(su->state == 'z')
        {
          su->state = value;
        }
        else if(su->state != value && value != 'x')
        {
          //printf("STATE !NULL\n");
          su->frequence = su->frequence + 1;
          su->state = value;
        }
      }
      else
      {
        //printf("TIME CHANGING\n");
        for(su=users; su != NULL; su=su->hh.next)
        {
          //printf("state %c last %c\n",su->state, su->last );
          if( su->state == '0' && (su->last == '0' || su->last == 'x' || su->last == 'z'))
          {
            su->timecurr = su->timecurr + 1;
            //printf("ADD TIMECURR0 %d\n", su->timecurr);
          }
          else if( su->state == '1' && (su->last == '1' || su->last == 'x' || su->last == 'z'))
          {
            su->timecurr = su->timecurr + 1;
            //printf("ADD TIMECURR1 %d\n", su->timecurr);
          }
          else if(su->state == '0' && su->last == '1')
          { 
            //printf("HERE3\n");
            if (su->timecurr > su->time0)
            {
              su->time1 = su->timecurr;
            }
            su->timecurr = 1;
          } 
          else if( su->state == '1' && su->last == '0')
          {
            //printf("HERE4\n");
            if (su->timecurr > su->time1)    
            {
              su->time0 = su->timecurr;
            }
            su->timecurr = 1;
          }
        }     
      }     
    }
  }
  
  for(su=users; su != NULL; su=su->hh.next)
  {
    //printf("%d > %d ?", su->timecurr, su->time0);
    if((su->timecurr > su->time0) && (su->state == '0'))
    {
      su->time0 = su->timecurr;
    }
    else if((su->timecurr > su->time1) && (su->state == '1'))
    {
      su->time1 = su->timecurr;
    }
  }
}

void getVariables(FILE* vcd)
{
  char *line = malloc(sizeof(char)*100);
  char *word = malloc(sizeof(char)*100);
  char *module = malloc(sizeof(char)*30);
  const char end[] = "$dumpvars";
  //  const char end2[] = "$end";
  // TODO change to dumpall
  const char space[] = " ";
  //struct my_struct* su;


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
        s->time1 = 0;
        strcpy(s->module, module);
        strcpy(s->key, word);
        printf("New variable:: %s | %s\n", s->key, s->module);
        s->state = 'z'; //maybe x here..
        add_user(s);
      }
      //else printf("%d : %s\n", (int)strlen(word), word);
    }
  }while( strcmp(word, (char*)end) && !feof(vcd) );
}

char* getWord(char* line)
{
  char *word;
  //char *l = (char*)malloc(sizeof(char)*100);

  if(line != NULL)
  {
    //strcpy(l, line);
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
  // If print a blank line, it seg fault
  if(n==0)
  {
  	return NULL;
  }
  line = strtok(line, "\n");
  //printf("tam: %d | line: %s | \n", (int)strlen(line),line);
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