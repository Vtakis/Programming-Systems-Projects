/*FILE: header.h */

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

typedef struct PostingList PostingList;
typedef struct PostingIndex PostingIndex;
typedef struct Trie Trie;
typedef struct WordListIndex WordListIndex;
typedef struct LineListIndex LineListIndex;
typedef struct WorkerFilesIndex WorkerFilesIndex;

struct WorkerFilesIndex{
	char *fileName;
	int numLines;//ari8mos grammwn arxeiou
	int numWords;//ari8mos leksewn arxeiou
	int numBytes;//ari8mos bytes arxeiou
	char **fileMap;//periexomeno ka8e grammhs tou arxeiou

	WorkerFilesIndex *next;
};

struct LineListIndex{
	int numLine;//ari8mos grammhs
	LineListIndex *next;
};

struct PostingList{
	char *fileName;
	LineListIndex *LineList; //lista me tous ari8mous grammwn pou vrisketai h leksi sto arxeio
	int WordFreq;
	PostingList *next;
};

struct PostingIndex{
	PostingList *PostList;
};

struct Trie{
	char letter;
	PostingIndex *PostIndex;
	Trie *LetterCurLvl;//geitonas
	Trie *LetterNextLvl;//paidi
};

int CommandNotValid(char *);
int countPaths(FILE *);
PostingList *SearchWorker(Trie *,char *);
char* MaxCount(Trie *,char *,int *);
char* MinCount(Trie *,char *,int *);
int CreateRootTrie(Trie **);
void TrieDestroy(Trie **);
void TrieDestroy(Trie **);
Trie *CreateNodeTrie(char);
int TrieInsert(Trie **,char*,char*,int);
WordListIndex *SearchForWordList(Trie *,int,char*);
int FindWord(WordListIndex *,char*);
void FreeWordList(WordListIndex **);

double searchDeadline(char *);
void FreeWorkerFiles(WorkerFilesIndex **);
int countFileLines(char *);
int searchExistingFile(char *name,WorkerFilesIndex *);

#endif
