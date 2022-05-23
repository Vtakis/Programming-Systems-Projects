#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include "webserver.h"
#define MAX_BUFFER_SIZE 50

void CreateServer(char *port,int i)
{
	int fd;
	int portNum=atoi(port);
	struct sockaddr_in server;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
	
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
		exit(EXIT_FAILURE);
	}
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portNum);      /* The given port */
    /* Bind socket to address */
    if (bind(fd, serverptr, sizeof(server)) < 0){
		perror("bind");
		exit(EXIT_FAILURE);
	}
    /* Listen for connections */
    if (listen(fd, 50) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if(i==0){
		servingfd=fd;
		printf("Listening connections for serving pages through port: %d\n", portNum);
	}
	else{
		commandfd=fd;
    	printf("Listening connections for commands through port: %d\n", portNum);
	}
}

//client connection
void RespondRequest(int fd){

	char *bufferRead = malloc(256);
	char *prev = malloc(256);
	char *path = malloc(256);
	char *data;

	int bytesRead,flag=0;
	FILE *filefd;

	while(1){
		memset(bufferRead,'\0',256);
		bytesRead = read(fd,bufferRead,256);
		if(bytesRead>0){
			if(flag==0){
				data = strtok (bufferRead, " \t\n");
				if ( strcmp(data, "GET")==0 ){
					data = strtok (NULL, " \t");
					if(data==NULL){
						write(fd,"Wrong get request!\r\n",20);
						close(fd);
						free(bufferRead);
						free(prev);
						free(path);
						return;
					}
					strcpy(path, PathToFile);
					strcpy(&path[strlen(PathToFile)], data);
					data = strtok (NULL, " \t\n");
					if(data==NULL){
						write(fd,"Wrong get request!\r\n",20);
						close(fd);
						free(bufferRead);
						free(prev);
						free(path);
						return;
					}
					data[strlen(data)-1]='\0';
					if( strcmp(data,"HTTP/1.1")!=0 ){
						write(fd,"Wrong get request!\r\n",20);
						close(fd);
						free(bufferRead);
						free(prev);
						free(path);
						return;
					}
				}
				else{
					write(fd,"Wrong get request!\r\n",20);
					close(fd);
					return;
				}
				flag=1;
			}
			else{
				if(strcmp(bufferRead,"\r\n")==0)
					break;
				if(strcmp(prev,"\r\n")==0 && strcmp(bufferRead,"\r\n")==0)
					break;
			}
			strcpy(prev,bufferRead);
		}
		if(bytesRead<=256){
			if(strstr(bufferRead,"\r\n\r\n")!=NULL)
				break;
		}
	}

//https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
	char buf[1000];
	char *date = malloc(1024);
	char *temp = malloc(256);
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	sprintf(date,"Date: %s\r\n",buf);

	if ( (filefd=fopen(path, "r"))!=NULL ){//file found and is readable
		served_pages++;
		fseek(filefd,0,SEEK_END);
		total_bytes+=ftell(filefd);
		sprintf(temp,"Content-Length: %ld\r\n",ftell(filefd));
		fseek(filefd,0,SEEK_SET);
		write(fd,"HTTP/1.1 200 OK\r\n", 17);
		write(fd,date,strlen(date));
		write(fd,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n",34);
		write(fd,temp,strlen(temp));
		write(fd,"Content-Type: text/html\r\n",23);
		write(fd,"\r\nConnection: Closed\r\n",20);
		write(fd,"\r\n\r\n",4);

		while (fgets(buf, sizeof(buf), filefd) != NULL)
			write (fd, buf, strlen(buf));
		fclose(filefd);
	}
	else{
		if ( (access(path,F_OK)==0) ){//file exists but we don't have permissions to read requested file
			write(fd,"HTTP/1.1 403 Forbidden\r\n", 25); 
			write(fd,date,strlen(date));
			write(fd,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n",34);
			write(fd,"Content-Length: 124\r\n",22);
			write(fd,"Content-Type: text/html\r\n",23);
			write(fd,"\r\nConnection: Closed\r\n",20);
			write(fd,"\r\n\r\n",4);
			sprintf(temp,"<html>Trying to access this file but don't think I can make it.</html>\r\n");
			write(fd,temp,strlen(temp)+1);
		}
		else{//files doesn't exists
			write(fd,"HTTP/1.1 404 Not Found\r\n", 25); 
			write(fd,date,strlen(date));
			write(fd,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n",34);
			write(fd,"Content-Length: 124\r\n",22);
			write(fd,"Content-Type: text/html\r\n",23);
			write(fd,"\r\nConnection: Closed\r\n",20);
			write(fd,"\r\n\r\n",4);
			sprintf(temp,"<html>Sorry dude, couldn't find this file.</html>\r\n");
			write(fd,temp,strlen(temp)+1);
		}
	}
	close(fd);
	free(bufferRead);
	free(prev);
	free(path);
	free(date);
	free(temp);
}

int SearchBuffer(int fd){
	bufferIndex *temp = fdBuffer;
	while(temp!=NULL){

		if(temp->fd==fd){
			free(temp);
			return 1;
		}
		temp=temp->next;
	}
	free(temp);
	return 0;
}

void InsertBuffer(int fd){

		if(fdBuffer==NULL){
			if(shutDown==1)
				return;
			fdBuffer = malloc(sizeof(bufferIndex));
			fdBuffer->fd = fd;
			fdBuffer->next=NULL;
		}//uparxei hdh ston buffer
		else if(SearchBuffer(fd)==1)
			return;
		else{
			bufferIndex *temp = fdBuffer;
			bufferIndex *newNode = malloc(sizeof(bufferIndex));
			newNode->fd=fd;
			newNode->next=NULL; 
			while(temp->next!=NULL)
				temp=temp->next;
			temp->next=newNode;
		}
		bufferSize++;
		if(bufferSize==MAX_BUFFER_SIZE)
			MaxBuffer=1;
}


void *GetRequest(void *ptr){

	bufferIndex *cur_fd;
	int flag=0;

	while(1){

		pthread_mutex_lock(&mutex);
		if(flag==1){
			pthread_mutex_unlock(&mutex);
			break;
		}

		if(fdBuffer==NULL){
			if(shutDown==1)
				flag=1;
			pthread_mutex_unlock(&mutex);
			continue;
		}

		cur_fd=fdBuffer;
		fdBuffer=fdBuffer->next;
		RespondRequest(cur_fd->fd);
		close(cur_fd->fd);
		bufferSize--;
		pthread_cond_signal(&condNonFull);
		pthread_mutex_unlock(&mutex);
	}
	//return NULL;
	pthread_exit(NULL);
}
