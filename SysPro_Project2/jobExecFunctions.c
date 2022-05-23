#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobheader.h"

#define BUFSIZE 128

int CommandNotValid(char *command){
	char *temp = malloc(strlen(command)+1);
	strcpy(temp,command);
	
	temp = strtok(temp," ");

	if(strcmp(temp,"search")==0)
		return 0;
	if(strcmp(temp,"maxcount")==0)
		return 0;
	if(strcmp(temp,"mincount")==0)
		return 0;
	if(strcmp(temp,"wc")==0)
		return 0;
	if(strcmp(temp,"exit")==0)
		return 0;
	printf("%s not valid command\n",temp);
	return 1;
}

int countPaths(FILE *docfile){

	int count_paths=0;
	char *str = malloc(BUFSIZE);

	while(fgets(str,BUFSIZE,docfile)){
		str = strtok(str,"\n");
		if(str==NULL||strcmp(str,"\n")==0)
			continue;
		count_paths++;
	}
	fseek(docfile,0,SEEK_SET);
	free(str);
	return count_paths;
}
