#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include "webserver.h"

int MaxBuffer=0;
int bufferSize=0;
int shutDown=0;
int served_pages=0;
size_t total_bytes=0;

bufferIndex *fdBuffer=NULL;

pthread_mutex_t mutex;
pthread_cond_t condNonFull;

char *PathToFile;
int servingfd,commandfd;
struct pollfd fd_array[2];

int main(int argc, char **argv){

	int fd,fdd,i,numThreads,ret;
	int hours,min,sec;

	char *servingPort = malloc(10);
	char *commandPort = malloc(10);
	PathToFile = malloc(256);

	int flag_p=0,flag_c=0,flag_t=0,flag_d=0;

	for(i=0;i<argc;i++){
		if(strcmp(argv[i],"-p")==0){
			strcpy(servingPort,argv[i+1]);
			flag_p=1;
		}
		else if(strcmp(argv[i],"-c")==0){
			strcpy(commandPort,argv[i+1]);
			flag_c=1;
		}
		else if(strcmp(argv[i],"-t")==0){
			numThreads = atoi(argv[i+1]);
			flag_t=1;
		}
		else if(strcmp(argv[i],"-d")==0){
			strcpy(PathToFile,argv[i+1]);
			flag_d=1;
		}
	}

	if(flag_p==0){
		free(PathToFile);
		free(servingPort);
		free(commandPort);
		printf("Error! Missing serving port!\n");
		return -1;
	}
	else if(flag_c==0){
		free(PathToFile);
		free(servingPort);
		free(commandPort);
		printf("Error! Missing command port!\n");
		return -1;
	}
	else if(flag_t==0)
		numThreads=5;
	else if(flag_d==0){
		free(PathToFile);
		free(servingPort);
		free(commandPort);
		printf("Error! Missing root directory!\n");
		return -1;
	}

	pthread_mutex_init(&mutex,0);
	pthread_cond_init(&condNonFull,0);

	pthread_t tid[numThreads];

	for(i=0;i<numThreads;i++)
		pthread_create(&tid[i],0,GetRequest,0);

	char *word=malloc(32);
	char *upTimeStats = malloc(64);

	clock_t startTime = time(NULL);
	clock_t curTime,upTime;

	CreateServer(servingPort,0);
	CreateServer(commandPort,1);

	fd_array[0].fd=commandfd;
	fd_array[0].events=POLLIN;
	fd_array[1].fd=servingfd;
	fd_array[1].events=POLLIN;

	struct sockaddr_in server;
	socklen_t serverlen = sizeof(server);
	struct sockaddr *serverptr=(struct sockaddr *)&server;

	while (1){

		if(shutDown==1)
			break;

		ret = poll(fd_array,2,0);

		if (ret<0)
			printf("accept() error\n");
		else if(!ret)
			continue;
		else{
			if(fd_array[0].revents & POLLIN){//command port activity
				fdd = accept(commandfd,serverptr, &serverlen);
				memset(word,'\0',32);
				read(fdd,word,32);
				if(strcmp(word,"STATS\r\n")==0){
					curTime = time(NULL);
					upTime = curTime - startTime;
					hours = upTime/3600;
					min = (upTime-3600*hours)/60;
					sec = upTime-3600*hours-60*min;
					if(min>=10){
						if(hours>=10)
							sprintf(upTimeStats,"Server up for %d:%d.%d, served %d pages, %ld bytes\n",hours,min,sec,served_pages,total_bytes);
						else
							sprintf(upTimeStats,"Server up for 0%d:%d.%d, served %d pages, %ld bytes\n",hours,min,sec,served_pages,total_bytes);
					}
					else{
						if(hours>=10)
							sprintf(upTimeStats,"Server up for %d:0%d.%d, served %d pages, %ld bytes\n",hours,min,sec,served_pages,total_bytes);
						else
							sprintf(upTimeStats,"Server up for 0%d:0%d.%d, served %d pages, %ld bytes\n",hours,min,sec,served_pages,total_bytes);
					}
					write(fdd, upTimeStats, strlen(upTimeStats));
				}
				else if(strcmp(word,"SHUTDOWN\r\n")==0){
					pthread_mutex_lock(&mutex);
					shutDown=1;
					pthread_mutex_unlock(&mutex);
				}
				else
					write(fdd, "Wrong Command\n",15);
				close(fdd);
			}
			if(fd_array[1].revents & POLLIN){//serving port activity
				fd = accept(servingfd,serverptr,&serverlen);
				
				pthread_mutex_lock(&mutex);

				if(shutDown==1){
					close(fd);
					pthread_mutex_unlock(&mutex);
					break;
				}
				while(MaxBuffer==1)//Buffer is full
					pthread_cond_wait(&condNonFull,&mutex);//wait for signal to continue
				InsertBuffer(fd);
				pthread_mutex_unlock(&mutex);
			}
		}
	}

	for(i=0;i<numThreads;i++)
		pthread_join(tid[i],NULL);
	pthread_cond_destroy(&condNonFull);
	pthread_mutex_destroy(&mutex);

	free(fdBuffer);
	free(PathToFile);
	free(servingPort);
	free(commandPort);
	free(upTimeStats);
	free(word);

	return 0;
}
