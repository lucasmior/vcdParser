#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uthash.h"

char* getLine(FILE* vcd);
char* getWord(char* line);


struct my_struct {
	int id;
    int frequence;            /* we'll use this field as the key */
    char module[30];             
    UT_hash_handle hh; /* makes this structure hashable */
};

struct my_struct *users = NULL;

void add_user(struct my_struct *s) {
    HASH_ADD_INT( users, id, s );    
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
	const char end[] = "$dump";
	const char s[2] = " ";
	
	printf("Getting variables\n");
	do{
		strcpy(line, getLine(vcd));
		strcpy(word, getWord(line));
		if( !strcmp(word, (char*)"$scope") )
		{
			strtok(NULL, s);
			module = strtok(NULL, s);
			printf("Scope:: %s\n", module);
		}
		else if( !strcmp(word, (char*)"$var") )
		{
			strtok(NULL, s);
			strtok(NULL, s);
			word = strtok(NULL, s);
			printf("Var:: %s\n", word);
			struct my_struct* s;
			//s->frequence = 0;
			//strcpy(s->module, module);
			//stdcpy(s.key, word);
			//s->id = 1;
			//add_user(s); 
		}
		//else skip
	}while( strcmp(word, end) );
	
	printf("Counting occurences\n");
	//TODO
	printf("Showing results\n");
	//TODO

	return 0;
}

char* getWord(char* line)
{
	char *word;
	char *l = malloc(sizeof(char)*100);
	const char s[2] = " ";

	if(line != NULL)
	{
		strcpy(l, line);
		word = strtok(l, s);
		// TODO Verify if it is needed.
		if(word == NULL)
		{
			strcpy(word, l);
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
