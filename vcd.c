#include<stdio.h>
#include<string.h>

char* getWord(FILE* vcd);
main()
{
	FILE *vcd;
	vcd = fopen("test.vcd", "rt");
	if(vcd == NULL)
	{
		printf("Problem to open the file\n");
		return -1;
	} 
	
	char word[100];
	char start[] = "$scope";
	
	printf("Reading header\n");
	do{
		strcpy(word, getWord(vcd));
		printf("%s\n",word );
	}while( strcmp(word, start) );
	
	printf("Reading $var\n");
	int ret = 0;
	do{
		strcpy(word, getWord(vcd));
		printf("OPA\n");
		if( !strcmp(word, (char*)"$var") )
		{
			printf("HELLO\n");
			//;//add here
		}
		printf("WORLD\n");
		else ret = 1; //break
	}while(!ret);
	
}

char* getWord(FILE* vcd)
{
	char line[100];
	char *word;
	//printf("Get word\n");
	if(fgets(line, 100, vcd) != NULL)
	{
		const char s[2] = " ";
		//printf("line: %s\n", line);
		word = strtok(line, s);
		if (word == NULL)
		{
			//printf("here:\n");
			strcpy(word, line);
			return (char*)word;
		}
		else{
			//printf("word: %s \n", word);
			return (char*)word;
		}
	}
	return (char*)'c';
}

