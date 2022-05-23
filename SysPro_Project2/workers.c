#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "header.h"

#define BUFSIZE 128
#define DEFAULT_DEADLINE 2.0

//Worker's main
int main(int argc, char **argv){
	
	int fd_read = open(argv[1],O_RDONLY);
	int fd_write = open(argv[2],O_RDWR|O_NONBLOCK);

	int found_strs=0;

	//dhmiourgia domhs gia Files tou Worker
	WorkerFilesIndex *WorkerFiles = NULL;
	WorkerFilesIndex *curWF = NULL;

	char *workerLog = malloc(16);
	sprintf(workerLog,"./log/worker_%d",atoi(argv[3]));
	FILE *Log = fopen(workerLog,"w");

	DIR* dir;
  	struct dirent *direntp;

	char* temp = malloc(BUFSIZE);
	char* str1 = malloc(BUFSIZE);
	char* word = malloc(BUFSIZE);
	char *toWrite = malloc(BUFSIZE);
	char *fileMax = malloc(BUFSIZE);
	char *fileMin = malloc(BUFSIZE);
	char *cur_time;

	int k,count=0;
	int cur_path;

	Trie *Root;
	Root = NULL;
	CreateRootTrie(&Root);

	size_t bufsize = BUFSIZE;

	/*https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program*/
	struct tm *tm;
	time_t t;


	for(;;){
		time(NULL);
		while(read(fd_read,temp,BUFSIZE)<0);
		str1 = strtok(temp," ");

		//Anoigma twn paths kai twn arxeiwn tous
		if(strcmp(str1,"Path")==0){

			temp = strtok(NULL,"\n");

			//printf("path = %s\n",temp);

			dir = opendir(temp);

			char *name = malloc(100);
			strcpy(toWrite,temp);

			while( (direntp = readdir(dir) ) !=NULL ){

				sprintf(name,"%s/%s",toWrite,direntp->d_name);
				if(strcmp(direntp->d_name,"..")==0||strcmp(direntp->d_name,".")==0){
					continue;
				}

				FILE *tempFile = fopen(name,"r");
				if(tempFile==NULL)
					printf("error in opening file\n");
				else{
					int countLines = countFileLines(name);
					if(WorkerFiles==NULL){
						WorkerFiles = malloc(sizeof(WorkerFilesIndex));
						WorkerFiles->fileName = malloc(strlen(name)+1);
						strcpy(WorkerFiles->fileName,name);
						WorkerFiles->numLines = countLines;
						WorkerFiles->numWords = 0;
						WorkerFiles->numBytes = 0;
						WorkerFiles->fileMap = malloc(countLines*BUFSIZE);
						for(k=0;k<countLines;k++)
							WorkerFiles->fileMap[k] = malloc(BUFSIZE);
						WorkerFiles->next=NULL;
						curWF = WorkerFiles;
					}
					else{
						if(searchExistingFile(name,WorkerFiles)==1)
							continue;
						else{
							WorkerFilesIndex *newWF = malloc(sizeof(WorkerFilesIndex));
							newWF->fileName = malloc(strlen(name)+1);
							strcpy(newWF->fileName,name);
							newWF->numLines = countLines;
							newWF->numWords = 0;
							newWF->numBytes = 0;
							newWF->fileMap = malloc(countLines*BUFSIZE);
							for(k=0;k<countLines;k++)
								newWF->fileMap[k] = malloc(BUFSIZE);
							newWF->next = NULL;

							WorkerFilesIndex *tempWF=NULL;
							tempWF = WorkerFiles;
							while(tempWF->next!=NULL)
								tempWF=tempWF->next;
							tempWF->next = newWF;
							curWF = newWF;
						}
					}
					
					int curLine = 0;
					//printf("Count Lines = %d\n",countLines);
					while(fgets(str1,BUFSIZE,tempFile)){

						if(strcmp(str1,"\t")==0 || strcmp(str1,"\n")==0){
							strcpy(curWF->fileMap[curLine],"\0");
							curLine++;
							continue;
						}
						if(str1==NULL){
							strcpy(curWF->fileMap[curLine],"\0");
							curLine++;
							continue;
						}

						strcpy(curWF->fileMap[curLine],str1);

						//eisagwgh sto trie
						word = strtok(str1," ");
						while(word!=NULL){
							curWF->numWords++;
							curWF->numBytes += strlen(word)+1;
							TrieInsert(&Root,word,name,curLine);
							word = strtok(NULL," ");
						}	
						curLine++;	
					}
				}
				fclose(tempFile);
			}
			write(fd_write,"ok",3);
			closedir(dir);
			free(name);
			continue;
		}
		else if(strcmp(str1,"search")==0){


			temp = strtok(NULL,"\n");
			char *tempStr = malloc(BUFSIZE);
			strcpy(tempStr,temp);

			double deadline;
			deadline = searchDeadline(temp);
			if(deadline==0.0)
				deadline = DEFAULT_DEADLINE;

			clock_t begin = clock();

			FILE *searchFile = NULL;
			searchFile = fopen("searchResults.txt","w");

			word = strtok(tempStr," ");
			PostingList *PS_List;
			t = time(&t);
			while(word!=NULL){
				cur_path=1;
				if(strcmp(word,"-d")==0)
					break;
				PS_List = SearchWorker(Root,word);

    			tm = localtime(&t);
				cur_time = asctime(tm);

				cur_time[strlen(cur_time)-1] = '\0';

				fprintf(Log,"Time of query arrival: %s Query type: %s ",cur_time,str1);

				if(PS_List!=NULL){
					fprintf(Log,"string: %s ",word);
					found_strs++;
				}
				while(PS_List!=NULL){
					fprintf(Log,"PathName%d: %s ",cur_path++,PS_List->fileName);
					while(PS_List->LineList!=NULL){
						fprintf(searchFile,"file = %s ||",PS_List->fileName);
						fprintf(searchFile," line %d ||",PS_List->LineList->numLine+1);

						WorkerFilesIndex *WF = WorkerFiles;
						while(strcmp(WF->fileName,PS_List->fileName)!=0)
							WF = WF->next;
						fprintf(searchFile," %s",WF->fileMap[PS_List->LineList->numLine]);

						PS_List->LineList = PS_List->LineList->next;
						fprintf(searchFile,"\n");
					}
					PS_List = PS_List->next;
					fprintf(Log,"\n");
				}

				word = strtok(NULL," ");
			}
			free(tempStr);
			fseek(searchFile,0,SEEK_SET);	

			clock_t end = clock();
			double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

			if(time_spent<deadline){//eimai entws deadline
				write(fd_write,"searchResults.txt",BUFSIZE);
				fclose(searchFile);
			}
			else
				write(fd_write,"slow",5);
			continue;
		}
		else if(strcmp(str1,"maxcount")==0){
			cur_path=1;
			time(&t);
			word = strtok(NULL,"\n");
			int max=0;
			fileMax = MaxCount(Root,word,&max);
			if(fileMax==NULL)
				write(fd_write,"notfound",9);
			else{
				tm = localtime(&t);
				cur_time = asctime(tm);

				cur_time[strlen(cur_time)-1] = '\0';

				fprintf(Log,"Time of query arrival: %s Query type: %s ",cur_time,str1);
				fprintf(Log,"string: %s pathname%d: %s\n",word,cur_path++,fileMax);
				found_strs++;

				sprintf(toWrite,"%d %s",max,fileMax);
				write(fd_write,toWrite,strlen(toWrite)+1);
			}
			free(fileMax);
			continue;


		}
		else if(strcmp(str1,"mincount")==0){
			cur_path=1;
			time(&t);
			word = strtok(NULL,"\n");

			int min=0;
			fileMin = MinCount(Root,word,&min);
			if(fileMin==NULL)
				write(fd_write,"notfound",9);
			else{
				tm = localtime(&t);
				cur_time = asctime(tm);

				cur_time[strlen(cur_time)-1] = '\0';

				fprintf(Log,"Time of query arrival: %s Query type: %s ",cur_time,str1);
				fprintf(Log,"string: %s pathname%d: %s\n",word,cur_path++,fileMin);

				found_strs++;
				sprintf(toWrite,"%d %s",min,fileMin);
				write(fd_write,toWrite,strlen(toWrite)+1);
			}
			fileMin=NULL;
			
			continue;
		}
		else if(strcmp(str1,"wc")==0){
				
			int countBytes=0;
			int countWords=0;
			int countLines=0;
			WorkerFilesIndex *WF = WorkerFiles;

			while(WF!=NULL){
				countBytes+=WF->numBytes;
				countWords+=WF->numWords;
				countLines+=WF->numLines;
				WF=WF->next;
			}
			sprintf(temp,"%d %d %d",countBytes,countWords,countLines);
			write(fd_write,temp,strlen(temp)+1);
			continue;

		}
		else if(strcmp(str1,"exit")==0){
				FreeWorkerFiles(&WorkerFiles);
				WorkerFiles=NULL;
				word=NULL;
				free(toWrite);
				TrieDestroy(&Root);
				fclose(Log);
				sprintf(str1,"%d",found_strs);
				write(fd_write,str1,strlen(str1)+1);
				str1=NULL;
				break;
		}
		else{
			write(fd_write,"error",5);
		}
	}
	exit(EXIT_SUCCESS);
}
