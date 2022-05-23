/*FILE: minisearch.c */

#include "minisearch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>

//////////////////////
//PROJECT 1 - K24   //
//VLASSIS PANAGIOTIS//
//AM: 1115201400022 //
//////////////////////

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

//Eisagwgh word sto Trie
int TrieInsert(Trie **Root,char *word,int doc_id){

	Trie *tempNode = *Root;

	while(*word!='\0'){
		if(tempNode->LetterNextLvl==NULL){//den exei paidi
			tempNode->LetterNextLvl=CreateNodeTrie(*word);
			tempNode=tempNode->LetterNextLvl;
			word++;
			if(*word=='\0'){//end of word
				if(tempNode->PostIndex==NULL){//dhmiourgia PostIndex kai PostList
					tempNode->PostIndex=malloc(sizeof(PostingIndex));;
					tempNode->PostIndex->total_freq=1;
					tempNode->PostIndex->DocFreq=1;

					tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
					tempNode->PostIndex->PostList->next=NULL;
					tempNode->PostIndex->PostList->doc_id=doc_id;
					tempNode->PostIndex->PostList->freq_word=1;
				}
				else{
					int found=0;
					PostingList *PS_List = tempNode->PostIndex->PostList;
					while(1){
						if(PS_List->doc_id==doc_id){
							PS_List->freq_word++;
							found=1;
							tempNode->PostIndex->total_freq++;
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
						PS_List->doc_id=doc_id;
						PS_List->freq_word=1;
						tempNode->PostIndex->DocFreq++;
						tempNode->PostIndex->total_freq++;
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

					tempNode->PostIndex=malloc(sizeof(PostingIndex));;
					tempNode->PostIndex->total_freq=1;
					tempNode->PostIndex->DocFreq=1;

					tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
					tempNode->PostIndex->PostList->next=NULL;
					tempNode->PostIndex->PostList->doc_id=doc_id;
					tempNode->PostIndex->PostList->freq_word=1;
				}
				else{
					int found=0;
					PostingList *PS_List = tempNode->PostIndex->PostList;
					while(1){
						if(PS_List->doc_id==doc_id){
							PS_List->freq_word++;
							found=1;
							tempNode->PostIndex->total_freq++;
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
						PS_List->doc_id=doc_id;
						PS_List->freq_word=1;
						tempNode->PostIndex->DocFreq++;
						tempNode->PostIndex->total_freq++;
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
						tempNode->PostIndex=malloc(sizeof(PostingIndex));;
						tempNode->PostIndex->total_freq=1;
						tempNode->PostIndex->DocFreq=1;

						tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
						tempNode->PostIndex->PostList->next=NULL;
						tempNode->PostIndex->PostList->doc_id=doc_id;
						tempNode->PostIndex->PostList->freq_word=1;
					}
					else{
						int found=0;
						PostingList *PS_List = tempNode->PostIndex->PostList;
						while(1){
							if(PS_List->doc_id==doc_id){
								PS_List->freq_word++;
								found=1;
								tempNode->PostIndex->total_freq++;
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
							PS_List->doc_id=doc_id;
							PS_List->freq_word=1;
							tempNode->PostIndex->DocFreq++;
							tempNode->PostIndex->total_freq++;
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
							tempNode->PostIndex=malloc(sizeof(PostingIndex));;
							tempNode->PostIndex->total_freq=1;
							tempNode->PostIndex->DocFreq=1;
	
							tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
							tempNode->PostIndex->PostList->next=NULL;
							tempNode->PostIndex->PostList->doc_id=doc_id;
							tempNode->PostIndex->PostList->freq_word=1;
						}
						else{
							int found=0;
							PostingList *PS_List = tempNode->PostIndex->PostList;
							while(1){
								if(PS_List->doc_id==doc_id){
									PS_List->freq_word++;
									found=1;
									tempNode->PostIndex->total_freq++;
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
								PS_List->doc_id=doc_id;
								PS_List->freq_word=1;
								tempNode->PostIndex->DocFreq++;
								tempNode->PostIndex->total_freq++;
							}
						}
					}
					continue;
				}
				else{//vrikame to gramma ston geitona
					word++;
					if(*word=='\0'){//end of word
						if(tempNode->PostIndex==NULL){
							tempNode->PostIndex=malloc(sizeof(PostingIndex));;
							tempNode->PostIndex->total_freq=1;
							tempNode->PostIndex->DocFreq=1;
	
							tempNode->PostIndex->PostList=malloc(sizeof(PostingList));
							tempNode->PostIndex->PostList->next=NULL;
							tempNode->PostIndex->PostList->doc_id=doc_id;
							tempNode->PostIndex->PostList->freq_word=1;
						}
						else{
							int found=0;
							PostingList *PS_List = tempNode->PostIndex->PostList;
							while(1){
								if(PS_List->doc_id==doc_id){
									PS_List->freq_word++;
									found=1;
									tempNode->PostIndex->total_freq++;
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
								PS_List->doc_id=doc_id;
								PS_List->freq_word=1;
								tempNode->PostIndex->DocFreq++;
								tempNode->PostIndex->total_freq++;
							}
						}
					}
				}
			}
		}
	}
}

//Katastrofh toy Posting List
void PostDestroy(PostingList *PostList){
	if(PostList==NULL)
		return;

	PostDestroy(PostList->next);
	free(PostList);
	return;
}

//Katastrofh tou Trie
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

//Sunartisi TF
int TF_Trie(Trie *Root,int doc_id,char *word){
	int j=0;
	Trie *tempNode = Root->LetterNextLvl;

	while(1){
		if(tempNode==NULL)
			break;
		if(tempNode->letter==word[j]){
			if(j==strlen(word)-1){
				if(tempNode->PostIndex!=NULL){
					PostingList *PS_List = tempNode->PostIndex->PostList;
					while(PS_List->doc_id!=doc_id){
						if(PS_List->next==NULL)
							return 0;
						PS_List = PS_List->next;
					}
					printf("%d %s %d\n",doc_id,word,PS_List->freq_word);
					return 0;
				}	
			}
			j++;
			tempNode=tempNode->LetterNextLvl;//pame sto paidi
		}
		else{
			tempNode=tempNode->LetterCurLvl;//pame sto geitona
		}
	}
}

//Sunartisi DF
int DF_Trie(Trie *Root,char *word){
	int j;
	if(word!=NULL){
		char *temp=strtok(word," ");
		while(temp!=NULL){//oso exoume lekseis pros anazitisi
			j=0;	
			Trie *tempNode = Root->LetterNextLvl;
			while(1){
				if(tempNode==NULL)
					break;
				if(tempNode->letter==temp[j]){
					if(j==strlen(temp)-1){
						if(tempNode->PostIndex!=NULL){
							printf("%s %d\n",temp,tempNode->PostIndex->DocFreq);
							break;
						}
					}
					j++;	
					tempNode=tempNode->LetterNextLvl;//pame sto paidi
				}
				else
					tempNode=tempNode->LetterCurLvl;//pame ston geitona
			}
			temp=strtok(NULL," ");
		}
	}
	else{//theloume oles tis lekseis tou Trie
		int depth=0;
		char *buffer=malloc(500);
		TrieRecTrav(Root->LetterNextLvl,buffer,depth);
		free(buffer);
	}
}

//Anadromiki Diasxisi olou tou dentrou gia thn DF
void TrieRecTrav(Trie *Root,char *buffer,int depth){
	int i;

	if(Root==NULL){
		return;
	}
	buffer[depth] = Root->letter;

	TrieRecTrav(Root->LetterNextLvl,buffer,depth+1);
	TrieRecTrav(Root->LetterCurLvl,buffer,depth);

	if(Root->PostIndex!=NULL){
		for(i=0;i<=depth;i++)
			printf("%c",buffer[i]);
		printf(" %d\n",Root->PostIndex->DocFreq);
	}
}

//Eisagwgh se lista tis lekseis pou do8ikan kai vriskontai sto trexon document
WordListIndex *SearchForWordList(Trie *Root,int doc_id,char *data){
	int i,j;
	int flag=0;
	int found=0;
	char* temp,*data1 = data;
	WordListIndex *head = NULL;
	WordListIndex *new;
	WordListIndex *P;

	if(Root->LetterNextLvl==NULL)
		return NULL;

	for(i=0;i<10;i++){
		if(i==0)
			temp=strtok(data1," ");
		if(temp==NULL)
			break;
		j=0;
		Trie *tempNode = Root->LetterNextLvl;

		while(1){
			if(tempNode==NULL)
				break;
			if(tempNode->letter==temp[j]){
				if(j==strlen(temp)-1){
					if(tempNode->PostIndex!=NULL){
						PostingList *PS_List = tempNode->PostIndex->PostList;
						while(1){
							if(PS_List->doc_id==doc_id){
								found=1;
								break;
							}
							if(PS_List->next==NULL){
								found=0;
								break;
							}
							PS_List=PS_List->next;
						}
						if(found==1){						
												
							new=malloc(sizeof(WordListIndex));
							new->word=malloc(strlen(temp)+1);
							strcpy(new->word,temp);
							new->next = NULL;
							
							if(head==NULL)
								head=new;
							else{
								P=head;
								while(P->next!=NULL)
									P=P->next;
								P->next = new;					
							}
							break;
						}				
					}
				}
				j++;	
				tempNode=tempNode->LetterNextLvl;
			}
			else
				tempNode=tempNode->LetterCurLvl;
		}
		temp=strtok(NULL," ");
	}
	return head;
	
}

//Anazitisi leksis sth lista
int FindWord(WordListIndex *WordList,char* word){

	WordListIndex *List;
	List = WordList;

	if(WordList==NULL)
		return 0;
	
	while(List!=NULL){
		if(strcmp(List->word,word)==0)
			return 1;
		else
			List=List->next;
	}
	return 0;
}

//Katastrofh listas leksewn
void FreeWordList(WordListIndex **WordList){
	if(*WordList==NULL)
		return;
	
	if((*WordList)->next!=NULL){
		FreeWordList(&(*WordList)->next);
	}
	free(&(*WordList)->word);
	return;
}

//euresh a8roismatos score olwn twn leksewn pou do8ikan
int TrieSearchScore(Trie *Root,DocContext *DocMap,int id,int NumOfIds,char *data,int D,double avgdl,double *score){

	int i,j;
	int flag=0;
	int check=0;
	char* temp,*data1 = data;
	*score=0.0;

	if(Root->LetterNextLvl==NULL)
		return 0;

	for(i=0;i<10;i++){
		if(i==0)
			temp=strtok(data1," ");
		if(temp==NULL)
			break;
		j=0;
		Trie *tempNode = Root->LetterNextLvl;

		while(1){
			if(tempNode==NULL)
				break;
			if(tempNode->letter==temp[j]){
				if(j==strlen(temp)-1){
					if(tempNode->PostIndex!=NULL){
						*score+=CalcScore(id,NumOfIds,avgdl,D,tempNode->PostIndex);
						flag=1;
						break;
					}
				}
				j++;	
				tempNode=tempNode->LetterNextLvl;
			}
			else
				tempNode=tempNode->LetterCurLvl;
		}
		temp=strtok(NULL," ");
	}

	if(flag==1)
		return 1;
	else
		return -1;

}

//upologismos tou score leksis
double CalcScore(int doc_id,int NumOfIds,int avgdl,int D,PostingIndex *PostIndex){

	int found=0;
	double sum=0.0;
	double b=0.75;
	double k=1.2;
	int nq_i=PostIndex->DocFreq;

	PostingList *PS_List = PostIndex->PostList;
	while(1){
		if(PS_List->doc_id==doc_id){
			found=1;
			break;
		}
		if(PS_List->next==NULL){
			found=0;
			break;
		}
		PS_List=PS_List->next;
	}
	if(found==1){
		sum = (log10((NumOfIds-nq_i+0.5)/(nq_i+0.5)))*((PS_List->freq_word+(k+1))/(PS_List->freq_word+k*(1-b+b*D/avgdl)));
		return sum;
	}
	return 0.0;
}

//ektupwsh gia ta top-k score sto search
void PrintTopK(int doc_id,double score,char* str,WordListIndex *WordList){

	char *buffer=malloc(25);
	printf("(%d) [%.5f] ",doc_id,score);
	sprintf(buffer,"(%d) [%.5f] ",doc_id,score);

	struct winsize w;
	ioctl(0,TIOCGWINSZ, &w);
	int width = w.ws_col - strlen(buffer);
	int i,j,cur=0;

	char *underline=malloc(width);
	for(i=0;i<width;i++)
			underline[i]=' ';

	char *str1=str;
	char *temp;
	int temp_w;
	char* bufferLine=malloc(width);
	cur=0;

	temp = strtok(str1," ");
	while(1){
		if(temp==NULL)
			break;
		j=0;

		if(FindWord(WordList,temp)==1){

			if(cur+strlen(temp)<width){
				for(i=0;i<strlen(temp);i++){
					bufferLine[cur++]='^';
				}
			}
			else{
				printf("\n");
				for(i=0;i<strlen(buffer);i++)
					printf(" ");
				//printf("%s",bufferLine);
				for(i=0;i<width;i++)
					bufferLine[i]=' ';

				cur=0;
				for(i=0;i<strlen(temp);i++){
					bufferLine[cur++]='^';
				}
			}
			printf("%s ",temp);
		}
		else{
			if(cur+strlen(temp)>width){
				printf("\n");
				for(i=0;i<strlen(buffer);i++)
					printf(" ");
				//printf("%s\n",bufferLine);
				for(i=0;i<width;i++)
					bufferLine[i]=' ';
				cur=0;
			}
			else
				cur+=strlen(temp);
			printf("%s ",temp);		
		}
		temp=strtok(NULL," ");
	}
	free(bufferLine);
	free(buffer);
	free(underline);
	printf("\n");
	return;
	
}

