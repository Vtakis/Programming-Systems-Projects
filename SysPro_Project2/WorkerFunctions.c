/*FILE: WorkerFunctions.c */

#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>

//////////////////////
//PROJECT 2 - K24   //
//VLASSIS PANAGIOTIS//
//AM: 1115201400022 //
//////////////////////


double searchDeadline(char *str){
	double deadline;
	str = strtok(str," ");
	while(str!=NULL){
		if(strcmp(str,"-d")==0){
			str = strtok(NULL," ");

			deadline = atof(str);
			return deadline;
		}
	str = strtok(NULL," ");
	}
	return 0.0;
}

void FreeWorkerFiles(WorkerFilesIndex **WF){

	if(*WF==NULL)
		return;
	
	if((*WF)->next!=NULL){
		FreeWorkerFiles(&(*WF)->next);
	}

	free((*WF)->fileName);
	for(int k=0;k<(*WF)->numLines;k++)
		free((*WF)->fileMap[k]);
	free((*WF)->fileMap);
	//free(*WF);
	//*WF=NULL;
	return;

}

int countFileLines(char *name){
	FILE *tempFile = fopen(name,"r");
	char *str1 = malloc(BUFSIZE);
	int count=0;
	
	while(fgets(str1,BUFSIZE,tempFile))
		count++;
	fseek(tempFile,0,SEEK_SET);	
	free(str1);
	//free(name);
	//str1=NULL;
	fclose(tempFile);
	return count;
}

int searchExistingFile(char *name,WorkerFilesIndex *WF){
	WorkerFilesIndex *temp;
	temp = WF;

	if(WF==NULL)
		return 0;
	while(temp!=NULL){
		if(strcmp(temp->fileName,name)==0)
			return 1;
		temp = temp->next;
	}
	return 0;
}

//Dhmiourgia rizas tou Trie
int CreateRootTrie(Trie **Root){
	*Root = CreateNodeTrie('\0');
}

//Dhmiourgia neou komvou sto Trie
Trie *CreateNodeTrie(char letter){
	Trie *newNode;
	newNode = malloc(sizeof(Trie));

	newNode->letter=letter;
	newNode->LetterCurLvl=NULL;
	newNode->LetterNextLvl=NULL;
	newNode->PostIndex=NULL;
	
	return newNode;
}

PostingList *SearchWorker(Trie *Root,char *word){
	int j;
	int max=0;
	if(word!=NULL){
		j=0;	
		Trie *tempNode = Root->LetterNextLvl;
		while(1){
			if(tempNode==NULL)
				break;
			if(tempNode->letter==word[j]){
				
				if(j==strlen(word)-1){
					if(tempNode->PostIndex!=NULL)
						return tempNode->PostIndex->PostList;
				}
				j++;	
				tempNode=tempNode->LetterNextLvl;//pame sto paidi
			}
			else
				tempNode=tempNode->LetterCurLvl;//pame ston geitona
		}
	}
	return NULL;
}

char* MaxCount(Trie *Root,char *word,int *max){
	int j;
	char *name = malloc(4096);
	if(word!=NULL){
		j=0;	
		Trie *tempNode = Root->LetterNextLvl;
		while(1){
			if(tempNode==NULL)
				break;
			if(tempNode->letter==word[j]){
				if(j==strlen(word)-1){
					if(tempNode->PostIndex!=NULL){
						PostingList *PS_List = tempNode->PostIndex->PostList;
						//printf("%s %d\n",temp,tempNode->PostIndex->DocFreq);
						*max = PS_List->WordFreq;
						strcpy(name,PS_List->fileName);
						
						PS_List=PS_List->next;
						while(PS_List!=NULL){
							if(PS_List->WordFreq > *max){
								*max = PS_List->WordFreq;
								strcpy(name,PS_List->fileName);
							}
							PS_List=PS_List->next;
						}
					}
				}
				j++;	
				tempNode=tempNode->LetterNextLvl;//pame sto paidi
			}
			else
				tempNode=tempNode->LetterCurLvl;//pame ston geitona
		}
	}
	if(*max==0)
		return NULL;
	else
		return name;
}

char* MinCount(Trie *Root,char *word,int *min){
	int j;
	char *name = malloc(4096);
	*min=0;
	if(word!=NULL){
		j=0;	
		Trie *tempNode = Root->LetterNextLvl;
		while(1){
			if(tempNode==NULL)
				break;
			if(tempNode->letter==word[j]){
				if(j==strlen(word)-1){
					if(tempNode->PostIndex!=NULL){
						PostingList *PS_List = tempNode->PostIndex->PostList;
						*min = PS_List->WordFreq;
						strcpy(name,PS_List->fileName);
						
						PS_List=PS_List->next;
						while(PS_List!=NULL){
							if( (PS_List->WordFreq < *min && PS_List->WordFreq>=1) || *min<1){
								*min = PS_List->WordFreq;
								strcpy(name,PS_List->fileName);
							}
							PS_List=PS_List->next;
						}
					}
				}
				j++;	
				tempNode=tempNode->LetterNextLvl;//pame sto paidi
			}
			else
				tempNode=tempNode->LetterCurLvl;//pame ston geitona
		}
	}
	if(*min==0)
		return NULL;
	else
		return name;
}

int SearchLineList(LineListIndex *List,int numLine){
	while(List!=NULL){

		if(List->numLine==numLine)
			return 1;
		List=List->next;
	}
	return 0;

}

LineListIndex *InsertInLineList(LineListIndex *List,int numLine){

		if(List==NULL){
			List = malloc(sizeof(LineListIndex));
			List->numLine = numLine;
			List->next=NULL;
			return List;
		}
		else if(SearchLineList(List,numLine)==1)
			return List;
		else{
			LineListIndex *temp = List;
			LineListIndex *newNode = malloc(sizeof(LineListIndex));
			newNode->numLine=numLine;
			newNode->next=NULL; 
			while(temp->next!=NULL)
				temp=temp->next;
			temp->next=newNode;
			return List;
		}
}

void FreeLineList(LineListIndex *List){
	if(List==NULL)
		return;

	if(List->next!=NULL)
		FreeLineList(List->next);
	free(List);
}


//Eisagwgh word sto Trie
int TrieInsert(Trie **Root,char *word,char *fileName,int numLine){

	Trie *tempNode = *Root;

	while(*word!='\0'){
		if(tempNode->LetterNextLvl==NULL){//den exei paidi
			tempNode->LetterNextLvl=CreateNodeTrie(*word);
			tempNode=tempNode->LetterNextLvl;
			word++;
			if(*word=='\0'){//end of word
				if(tempNode->PostIndex==NULL){//dhmiourgia PostIndex kai PostList

					tempNode->PostIndex=malloc(sizeof(PostingIndex));
					tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
					tempNode->PostIndex->PostList->next=NULL;
					tempNode->PostIndex->PostList->fileName = malloc(strlen(fileName)+1);
					strcpy(tempNode->PostIndex->PostList->fileName,fileName);
					tempNode->PostIndex->PostList->WordFreq=1;
					tempNode->PostIndex->PostList->LineList = NULL;
					tempNode->PostIndex->PostList->LineList = InsertInLineList(tempNode->PostIndex->PostList->LineList,numLine);
				}
				else{
					int found=0;
					PostingList *PS_List = tempNode->PostIndex->PostList;
					while(1){
						if(strcmp(PS_List->fileName,fileName)==0){
							PS_List->WordFreq++;
							PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
							found=1;
							break;
						}
						if(PS_List->next==NULL){
							found=0;
							break;
						}
						PS_List=PS_List->next;
					}
					if(found==0){
						PS_List->next=malloc(sizeof(PostingList));
						PS_List=PS_List->next;
						PS_List->next=NULL;
						PS_List->fileName = malloc(strlen(fileName)+1);
						strcpy(PS_List->fileName,fileName);
						PS_List->LineList = NULL;
						PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
						PS_List->WordFreq=1;
					}
				}	
			}
			continue;	
		}
		if(tempNode->LetterNextLvl->letter==*word){//vrika to trexon gramma sto paidi

			tempNode=tempNode->LetterNextLvl;
			word++;
			if(*word=='\0'){//end of word
				if(tempNode->PostIndex==NULL){

					tempNode->PostIndex=malloc(sizeof(PostingIndex));
					tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
					tempNode->PostIndex->PostList->next=NULL;
					tempNode->PostIndex->PostList->fileName = malloc(strlen(fileName)+1);
					strcpy(tempNode->PostIndex->PostList->fileName,fileName);
					tempNode->PostIndex->PostList->WordFreq=1;
					tempNode->PostIndex->PostList->LineList = NULL;
					tempNode->PostIndex->PostList->LineList = InsertInLineList(tempNode->PostIndex->PostList->LineList,numLine);
				}
				else{
					int found=0;
					PostingList *PS_List = tempNode->PostIndex->PostList;
					while(1){
						if(strcmp(PS_List->fileName,fileName)==0){
							PS_List->WordFreq++;
							PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
							found=1;
							break;
						}
						if(PS_List->next==NULL){
							found=0;
							break;
						}
						PS_List=PS_List->next;
					}
					if(found==0){
						PS_List->next=malloc(sizeof(PostingList));
						PS_List=PS_List->next;
						PS_List->next=NULL;
						PS_List->fileName = malloc(strlen(fileName)+1);
						strcpy(PS_List->fileName,fileName);
						PS_List->LineList = NULL;
						PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
						PS_List->WordFreq=1;
					}
					return 1;
				}
			}
			continue;
		}
		else{//den einai idia ta grammata
			if(tempNode->LetterNextLvl==NULL){
				tempNode->LetterNextLvl = CreateNodeTrie(*word);
				word++;
				if(*word=='\0'){//end of word
					if(tempNode->PostIndex==NULL){

						tempNode->PostIndex=malloc(sizeof(PostingIndex));
						tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
						tempNode->PostIndex->PostList->next=NULL;
						tempNode->PostIndex->PostList->fileName = malloc(strlen(fileName)+1);
						strcpy(tempNode->PostIndex->PostList->fileName,fileName);
						tempNode->PostIndex->PostList->WordFreq=1;
						tempNode->PostIndex->PostList->LineList = NULL;
						tempNode->PostIndex->PostList->LineList = InsertInLineList(tempNode->PostIndex->PostList->LineList,numLine);
					}
					else{
						int found=0;
						PostingList *PS_List = tempNode->PostIndex->PostList;
						while(1){
							if(strcmp(PS_List->fileName,fileName)==0){
								PS_List->WordFreq++;
								PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
								found=1;
								break;
							}
							if(PS_List->next==NULL){
								found=0;
								break;
							}
							PS_List=PS_List->next;
						}
						if(found==0){
							PS_List->next=malloc(sizeof(PostingList));
							PS_List=PS_List->next;
							PS_List->next=NULL;
							PS_List->fileName = malloc(strlen(fileName)+1);
							strcpy(PS_List->fileName,fileName);
							PS_List->LineList = NULL;
							PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
							PS_List->WordFreq=1;
						}
					}
				}
				tempNode=tempNode->LetterNextLvl;
				continue;
			}
			else{//koitame ston geitona
				int flag=0;
				tempNode=tempNode->LetterNextLvl;
				while(1){
					if(tempNode->LetterCurLvl==NULL){
						flag=0;
						break;
					}
					tempNode=tempNode->LetterCurLvl;
					if(tempNode->letter==*word){
						flag=1;
						break;
					}
				}
				if(flag==0){//den exoume geitona
					tempNode->LetterCurLvl=CreateNodeTrie(*word);
					tempNode=tempNode->LetterCurLvl;
					word++;
					if(*word=='\0'){//end of word
						if(tempNode->PostIndex==NULL){

							tempNode->PostIndex=malloc(sizeof(PostingIndex));
							tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
							tempNode->PostIndex->PostList->next=NULL;
							tempNode->PostIndex->PostList->fileName = malloc(strlen(fileName)+1);
							strcpy(tempNode->PostIndex->PostList->fileName,fileName);
							tempNode->PostIndex->PostList->WordFreq=1;
							tempNode->PostIndex->PostList->LineList = NULL;
							tempNode->PostIndex->PostList->LineList = InsertInLineList(tempNode->PostIndex->PostList->LineList,numLine);
						}
						else{
							int found=0;
							PostingList *PS_List = tempNode->PostIndex->PostList;
							while(1){
								if(strcmp(PS_List->fileName,fileName)==0){
									PS_List->WordFreq++;
									PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
									found=1;
									break;
								}
								if(PS_List->next==NULL){
									found=0;
									break;
								}
								PS_List=PS_List->next;
							}
							if(found==0){
								PS_List->next=malloc(sizeof(PostingList));
								PS_List=PS_List->next;
								PS_List->next=NULL;
								PS_List->fileName = malloc(strlen(fileName)+1);
								strcpy(PS_List->fileName,fileName);
								PS_List->LineList = NULL;
								PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
								PS_List->WordFreq=1;
							}
						}
					}
					continue;
				}
				else{//vrikame to gramma ston geitona
					word++;
					if(*word=='\0'){//end of word
						if(tempNode->PostIndex==NULL){

							tempNode->PostIndex=malloc(sizeof(PostingIndex));
							tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
							tempNode->PostIndex->PostList->next=NULL;
							tempNode->PostIndex->PostList->fileName = malloc(strlen(fileName)+1);
							strcpy(tempNode->PostIndex->PostList->fileName,fileName);
							tempNode->PostIndex->PostList->WordFreq=1;
							tempNode->PostIndex->PostList->LineList = NULL;
							tempNode->PostIndex->PostList->LineList = InsertInLineList(tempNode->PostIndex->PostList->LineList,numLine);
						}
						else{
							int found=0;
							PostingList *PS_List = tempNode->PostIndex->PostList;
							while(1){
								if(strcmp(PS_List->fileName,fileName)==0){
									PS_List->WordFreq++;
									PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
									found=1;
									break;
								}
								if(PS_List->next==NULL){
									found=0;
									break;
								}
								PS_List=PS_List->next;
							}
							if(found==0){
								PS_List->next=malloc(sizeof(PostingList));
								PS_List=PS_List->next;
								PS_List->next=NULL;
								PS_List->fileName = malloc(strlen(fileName)+1);
								strcpy(PS_List->fileName,fileName);
								PS_List->LineList = NULL;
								PS_List->LineList = InsertInLineList(PS_List->LineList,numLine);
								PS_List->WordFreq=1;
							}
						}
					}
				}
			}
		}
	}
}

void PostDestroy(PostingList *PostList){
	if(PostList==NULL)
		return;

	PostDestroy(PostList->next);
	free(PostList->fileName);
	FreeLineList(PostList->LineList);
	//free(PostList->LineList);
	free(PostList);
	return;
}

void TrieDestroy(Trie **Root){
	
	if(*Root==NULL)
		return;

	if((*Root)->PostIndex!=NULL){
		PostDestroy((*Root)->PostIndex->PostList);
		free((*Root)->PostIndex);
	}
	
	TrieDestroy(&(*Root)->LetterNextLvl);
	TrieDestroy(&(*Root)->LetterCurLvl);

	free((*Root));
	return;
}


