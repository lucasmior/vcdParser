#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uthash.h"

struct my_struct* find_user(char* key);
void add_user(struct my_struct *s);
char* getLine(FILE* vcd);
char* getWord(char* line);

struct my_struct {
  char key[10];					/* we'll use this field as the key */
  char state;
  int oneTime;
  int zeroTime;
  int currentTime;
  int frequence;            
  char module[30];             
  UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;

int main( )
{
  FILE *vcd;
  vcd = fopen("kriti.vcd", "rt");
  if(vcd == NULL)
  {
    printf("Problem to open the file\n");
    return -1;
  } 
  
  char *line = malloc(sizeof(char)*100);
  char *word = malloc(sizeof(char)*100);
  char *module = malloc(sizeof(char)*30);
  const char end[] = "$dumpvars";
  const char end2[] = "$end";
  // TODO change to dumpall
  const char space[] = " ";
  struct my_struct* su;
  
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
        strcpy(s->key, word);
        s->state = 'z'; //maybe x here..
        add_user(s);
      }
      //else printf("%d : %s\n", (int)strlen(word), word);
    }
  }while( strcmp(word, (char*)end) && !feof(vcd) );

  printf("\n");
  printf("Counting occurences\n");

  //line = getLine(vcd);
  //printf("%s\n", line);

  while(!feof(vcd))
  {	
  	//printf("new line\n");
    line = getLine(vcd);
    //printf("Got new line\n");
    if(line != NULL)
    {
		  //printf("####################\n");
      //printf("strlen: %ld | %s\n", strlen(line), line);
      //if( !strcmp(line, (char*)end2) ){printf("OH NO!\n"); break;} 
      
    	//char key = line[strlen(line)-1];
    	char* key = malloc(2);
		  char value;
		  
    	//TODO fix it, gambiarra rules!
    	strcpy(key, line+1);
		  value = line[0];
		  
		  //printf("key:: %s | ", key);
		  //printf("value: %c\n", value);

		  su = find_user(key);
		  if( su != NULL)
		  {
				if(su->state == 'z')
				{
					//printf("STATE NULL \n");
				 	su->state = value;
				 	//printf("New state: %d\n", su->state);
				}
				else if(su->state != value && value != 'x')
  /**
				else
	  **/
				{
					//printf("STATE !NULL\n");
					su->frequence = su->frequence + 1;
					su->state = value;
				 	//printf("New state: %d\n", su->state);
				}
	  	}
	  	else
	  	{
	  		printf("TIME CHANGING\n");
        for(s=users; s != NULL; s=s->hh.next)
        {
          if state == 0 and last == 0 or last == x
            increase time0;
          else if state == 1 and last == 1 or last == x
            increase time1;
          else state == 0 and last == 1
            if timecurr > time0 then 
              replace..
            time0 = 1;
          else time1 = 1;
        }			
	  	} 
	  	//printf("...END...\n");			
   	}
  }
  printf("\n");
  printf("Showing results\n");

	struct my_struct* s;
  /**
  for(s=users; s != NULL; s=s->hh.next)
  {
    printf("key: %s: frequence: %d\n", s->key, s->frequence);
	}
	**/
	char* key;
	int freq = 0;
	char* clk = "\"";
	for(s=users; s != NULL; s=s->hh.next)
  {
  	if(strcmp(s->key,clk) && s->frequence > freq)
  	{
  		freq = s->frequence;
  		printf("new key is: %s\n", s->key );
  		key = s->key;
  	}
	}
	printf("More changes was: %s with: %d changes\n", key, freq );
	printf("\n");
  return 0;
}



/*
    #### METHDS ######
*/
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
void add_user(struct my_struct *s) {
  HASH_ADD_STR( users, key, s );    
}

struct my_struct* find_user(char* key) {
  struct my_struct *s;

  HASH_FIND_STR( users, key, s );
  return s;
}