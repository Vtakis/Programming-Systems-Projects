/*FILE: header.h */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int MaxBuffer;
extern int bufferSize;
extern int servingfd;
extern int commandfd;
extern int served_pages;
extern size_t total_bytes;
extern int shutDown;

extern pthread_mutex_t mutex;
extern pthread_cond_t condNonFull;

extern char *PathToFile;

struct pollfd fd_array[2];

typedef struct bufferIndex bufferIndex;

struct bufferIndex{
	int fd;
	bufferIndex *next;
};

extern bufferIndex *fdBuffer;

int SearchBuffer(int);
void InsertBuffer(int);
void CreateServer(char*,int);
void *GetRequest(void*);
void RespondRequest(int);

#endif

