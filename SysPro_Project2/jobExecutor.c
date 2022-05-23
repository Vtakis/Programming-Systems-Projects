#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "header.h"

#define BUFSIZE 128

int main(int argc, char **argv){

	FILE* docfile=NULL;
	int i,numWorkers;
	
	int workers_flag=0;
	int doc_flag=0;

	for(i=0;i<argc;i++){
		if(strcmp(argv[i],"-d")==0){
			docfile = fopen(argv[i+1],"r");
			if(docfile==NULL){
				printf("Document file doesn't exists!\n");
				free(docfile);
				return -1;
			}
			doc_flag=1;
		}
		else if(strcmp(argv[i],"-w")==0){
			numWorkers = atoi(argv[i+1]);
			if(numWorkers<=0){
				printf("workers_flag must be positive!\n");
				return -1;
			}
			workers_flag=1;
		}
	}

	if(doc_flag==0){
		printf("Error! Missing document file!\n");
		return -1;
	}
	else if(workers_flag==0)//default w=5
		numWorkers = 5;

	DIR* dir = opendir("./log");
  	struct dirent *direntp;

	if (dir)
	{
   	 	/* Directory exists. */
		while ( ( direntp=readdir(dir) ) != NULL ){
			char *temp = malloc(7+strlen(direntp->d_name));
			sprintf(temp,"./log/%s",direntp->d_name);
			remove(temp);
			free(temp);
		}
    	closedir(dir);
	}
	else if (ENOENT == errno)
	{
		mkdir("./log",0777);
	}

	size_t bufsize=500;
	int count_paths;

	count_paths = countPaths(docfile);
	fseek(docfile,0,SEEK_SET);
	char *PathsMap[count_paths];
	for(i=0;i<count_paths;i++)
		PathsMap[i] = malloc(BUFSIZE);

	char *str = malloc(BUFSIZE);
	char *str1 = malloc(BUFSIZE);

	int id=0;
	while(fgets(str,BUFSIZE,docfile)){
		str = strtok(str,"\n");
		if(str==NULL || strcmp(str,"\n")==0)
			continue;
		strcpy(PathsMap[id],str);
		id++;
	}

	int *paths_per_worker;

	fseek(docfile,0,SEEK_SET);

	if(count_paths<numWorkers){//de xreiazontai oloi oi workers
		numWorkers = count_paths;
		printf("Using only %d workers\n",numWorkers);

		paths_per_worker = malloc(numWorkers*sizeof(int));
		for(i=0;i<numWorkers;i++)
			paths_per_worker[i]=1;
	}
	else{
		paths_per_worker = malloc(numWorkers*sizeof(int));
		int numPaths = count_paths/numWorkers;
		for(i=0;i<numWorkers;i++)
			paths_per_worker[i]=numPaths;
		int cur_paths = numPaths*numWorkers;
		if(cur_paths!=count_paths){
			for(i=0;i<numWorkers;i++){
				paths_per_worker[i]++;
				cur_paths++;
				if(cur_paths==count_paths)
					break;
			}
		}
	}

	pid_t worker[numWorkers];
	char *send_workers[numWorkers];
	int fd_send_workers[numWorkers];
	char *read_workers[numWorkers];
	int fd_read_workers[numWorkers];

	//create FIFO

	for(i=0;i<numWorkers;i++){
		send_workers[i] = malloc(10);
		read_workers[i] = malloc(10);
		sprintf(send_workers[i],"send_%d",i);
		sprintf(read_workers[i],"read_%d",i);
		mkfifo(send_workers[i],0666);
		mkfifo(read_workers[i],0666);
		fd_send_workers[i] = open(send_workers[i],O_RDWR| O_NONBLOCK);
		fd_read_workers[i] = open(read_workers[i],O_RDWR);
	}

	char *to_send[5];

	int status;

	char* input1=malloc(BUFSIZE);
	char* command = malloc(BUFSIZE);

	char *temp1 = malloc(BUFSIZE);
	int flag=0,flag1=0;

	int k,curPath=0;

//Create childrens

	for(i=0;i<numWorkers;i++){
		to_send[0] = malloc(10);
		strcpy(to_send[0],"./workers");
		to_send[1] = send_workers[i];
		to_send[2] = read_workers[i];
		to_send[3] = malloc(5);
		to_send[4] = NULL;

		worker[i] = fork();

		if(worker[i]==0){
			sprintf(to_send[3],"%d",getpid());

			if(execvp("./workers",to_send)<0)
				printf("Execvp failed\n");
		}
		else{
			continue;
		}
	}

//Dinw ta antistoixa paths sta paidia
	for(i=0;i<numWorkers;i++){
		for(k=0;k<paths_per_worker[i];k++){
				sprintf(input1,"Path %s",PathsMap[k+curPath]);
				write(fd_send_workers[i],input1,strlen(input1)+1);
				while(waitpid(worker[i], &status, WNOHANG));
				while(read(fd_read_workers[i],temp1,BUFSIZE)<0);
		}
		curPath+=paths_per_worker[i];
	}
	printf("\nCreation of workers completed! Ready for input!\n\n");

///////////////////////////////////////////////////////////////////////

	int RespondedWorkers = numWorkers;
	int FoundWorkers = numWorkers;
	int j,max,min,maxW,minW,pos;
	char *PathResult = malloc(BUFSIZE);

	char *PathsForCountsQuery[numWorkers];
	int *ResultsWC = malloc(3*numWorkers*sizeof(int));

	char *tempPath = malloc(BUFSIZE);
	for(k=0;k<numWorkers;k++)
		PathsForCountsQuery[i] = NULL;

	while(1){
		printf("/");
		getline(&input1,&bufsize,stdin);//perimenw entoli apo ton xrhstj
		command = strtok(input1,"\n");
		if(command==NULL || !strcmp(command,"\n"))
				continue;
		if(CommandNotValid(command)==1)
			continue;
		char *tempCom = malloc(BUFSIZE);
		strcpy(tempCom,command);
		tempCom = strtok(tempCom," ");
		if(strcmp(command,"exit")==0){//exiting...
			for(i=0;i<numWorkers;i++){
				if (write(fd_send_workers[i],command,strlen(command)+1)<0)
					printf("Write failed\n");
				while(read(fd_read_workers[i],temp1,BUFSIZE)<0);
				printf("Worker_%i exiting...strings found %s\n",worker[i],temp1);
			}
			break;
		}

		for(i=0;i<numWorkers;i++){
			if (write(fd_send_workers[i],command,strlen(command)+1)<0)//write to query sta paidia
				printf("Write failed\n");
			while(read(fd_read_workers[i],temp1,BUFSIZE)<0);
			while(waitpid(worker[i], &status, WNOHANG));
			if(strcmp(tempCom,"wc")==0){
				int off = 3*i;
				int d=0;
				str = strtok(temp1," ");	
				while(str!=NULL){
					ResultsWC[off+d] = atoi(str);
					str = strtok(NULL," ");
					d++;
				}
				printf("\n");
			}

			if(strcmp(temp1,"searchResults.txt")==0){//an parw auto, kserw pws einai gia to search query
				FILE *fp = fopen(temp1,"r");
				int flag_r = 0;
				printf("\n");
				while(fgets(str1,BUFSIZE,fp)){
					flag_r=1;
					printf("%s\n",str1);
				}
				if(flag_r==0)
					RespondedWorkers--;
				fclose(fp);
				unlink(temp1);
			}
			if(strcmp(tempCom,"maxcount")==0 || strcmp(tempCom,"mincount")==0){
				if(strcmp(temp1,"notfound")==0){
					PathsForCountsQuery[i] = malloc(2);
					strcpy(PathsForCountsQuery[i],"-");
					FoundWorkers--;
				}
				else if(strstr(temp1,"notfound")!=NULL){
					PathsForCountsQuery[i] = malloc(2);
					strcpy(PathsForCountsQuery[i],"-");
					FoundWorkers--;
				}
				else{
					PathsForCountsQuery[i] = malloc(strlen(temp1)+1);
					strcpy(PathsForCountsQuery[i],temp1);
				}
			}

			if(strcmp(temp1,"slow")==0)//an parw auto, kserw pws einai gia to search query
				RespondedWorkers--;

			if(i==numWorkers-1){
				command = strtok(command," ");
				if(strcmp(command,"wc")==0){
					int sum_words=0;
					int sum_lines=0;
					int sum_bytes=0;
					for(j=0;j<numWorkers;j++){
						sum_bytes+=ResultsWC[3*j];
						sum_words+=ResultsWC[3*j+1];
						sum_lines+=ResultsWC[3*j+2];
					}
					printf("Total bytes: %d || Total words: %d || Total lines: %d\n",sum_bytes,sum_words,sum_lines);
				}
				if(strcmp(command,"search")==0){
					printf("\n%d/%d Workers responded or found results for search query!\n\n",RespondedWorkers,numWorkers);
					RespondedWorkers=numWorkers;
				}
				else if(strcmp(command,"maxcount")==0 || strcmp(command,"mincount")==0){

					if(strcmp(command,"maxcount")==0){
						if(FoundWorkers==0)
							printf("\n0 Workers found results\n\n");
						else{
							max = -1;
							for(j=0;j<numWorkers;j++){
								if(strcmp(PathsForCountsQuery[j],"-")==0)
									continue;
								tempPath = strtok(PathsForCountsQuery[j]," \t");
								maxW = atoi(tempPath);
								tempPath = strtok(NULL,"\n");
								if(maxW>max){
									max = maxW;
									strcpy(PathResult,tempPath);
								}
							}
							printf("\n%s\n\n",PathResult);
						}
					}
					if(strcmp(command,"mincount")==0){
						if(FoundWorkers==0)
							printf("\n0 Workers found results\n\n");
						else{
							min = -1;
							int flag_min=0;
							for(j=0;j<numWorkers;j++){
								if(strcmp(PathsForCountsQuery[j],"-")==0)
									continue;
								else{
									if(flag_min==0){
										tempPath = strtok(PathsForCountsQuery[j]," \t");
										minW = atoi(tempPath);
										tempPath = strtok(NULL,"\n");
										strcpy(PathResult,tempPath);
										flag_min=1;
										continue;
									}
								}
								tempPath = strtok(PathsForCountsQuery[j]," \t");
								minW = atoi(tempPath);
								tempPath = strtok(NULL,"\n");
								if(minW<min){
									min = minW;
									strcpy(PathResult,tempPath);
								}
								if(minW=min){
									if(strcmp(PathResult,tempPath)>0)
										strcpy(PathResult,tempPath);
								}
							}
							printf("\n%s\n\n",PathResult);
						}

					}
					FoundWorkers=numWorkers;
					for(k=0;k<numWorkers;k++)
							free(PathsForCountsQuery[k]);
				}
			}
		}
	}

	free(paths_per_worker);
	free(PathResult);
	free(ResultsWC);

	for(i=0;i<numWorkers;i++){
		unlink(send_workers[i]);
		unlink(read_workers[i]);
	}

	remove("searchResults.txt");

	for(i=0;i<numWorkers;i++)
		kill(worker[i],SIGTERM);

	for(i=0;i<count_paths;i++)
		free(PathsMap[i]);
	fclose(docfile);
}
