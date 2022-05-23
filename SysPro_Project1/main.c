/*FILE: main.c */

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "minisearch.h"
#include "heap.h"

//////////////////////
//PROJECT 1 - K24   //
//VLASSIS PANAGIOTIS//
//AM: 1115201400022 //
//////////////////////

int main(int argc,char **argv){

	FILE* docfile=NULL;
	int i,k;
	
	int k_flag=0;
	int doc_flag=0;

	//elegxos gia ta orismata
	for(i=0;i<argc;i++){
		if(strcmp(argv[i],"-i")==0){
			docfile = fopen(argv[i+1],"r");
			if(docfile==NULL){
				printf("Document file doesn't exists!\n");
				free(docfile);
				return -1;
			}
			doc_flag=1;
		}
		else if(strcmp(argv[i],"-k")==0){
			k = atoi(argv[i+1]);
			if(k<=0){
				printf("K must be positive!\n");
				return -1;
			}
			k_flag=1;
		}
	}

	if(doc_flag==0){
		printf("Error! Missing document file!\n");
		return -1;
	}
	else if(k_flag==0)//default k=10
		k = 10;

	//1o diavasma tou arxeiou
	int ch,counter=0;
	int max_length=0,cur_length=0,max_words=0,cur_words=0;
	while((ch=fgetc(docfile))!=EOF){

		if(ch==' '){ //na vrw to length ths max leksis
			if(cur_length>max_length)
				max_length=cur_length;
			cur_length=0;
			cur_words++;
		}
		else if(ch=='\n'){
			if(cur_length>max_length)
				max_length=cur_length;
			cur_length=0;
			if(cur_words>max_words)
				max_words=cur_words;
			cur_words=0;
		}
		else
			cur_length++;

		if(ch == '\n')//posa lines exei to file
			counter++;
	}

	fseek(docfile,0,SEEK_SET);

	/*printf("Max length leksis = %d\n",max_length);
	printf("Lines of document = %d\n",counter);
	printf("Max words document = %d\n",max_words);*/

	char *str = malloc(max_words*max_length+1);
	char *word=NULL;
	char *str1=str;


	DocContext *DocMap = malloc(counter*sizeof(DocContext));
	for(i=0;i<counter;i++)
		DocMap[i].NumOfWords=0;

	Trie *Root;
	Root = NULL;
	CreateRootTrie(&Root);
	
	int id,cur_id=0;


	//Dhmiourgia Index
	while(fgets(str,max_words*max_length,docfile)){

		word = strtok(str," \t");
		id = atoi(word);
		if(cur_id!=0 && id==0)//diavastike keni grammi
			continue;
		if(cur_id!=id){//la8os id
			TrieDestroy(&Root);
			for(i=0;i<=cur_id;i++)
				free(DocMap[i].docString);		
			free(DocMap);
			free(str1);
			fclose(docfile);
			return 0;
			
		}
		cur_id++;
		str = strtok(NULL,"\n");

		DocMap[id].doc_id=id;
		DocMap[id].docString = malloc(max_words*max_length+1);
		strcpy(DocMap[id].docString,str);

		if(str==NULL){
			DocMap[id].NumOfWords=0;
			strcpy(DocMap[id].docString,"\0");
			continue;
		}

		//eisagwgh leksewn sto Trie
		word = strtok(str," ");
		while(word!=NULL){
			DocMap[id].NumOfWords++;
			TrieInsert(&Root,word,id);
			word = strtok(NULL," ");
		}
	}

	word=NULL;
	//telos dhmiourgias index

	char* input1;
	char* command;
	char* data;
	size_t bufsize=500;
	char* input;
	char* temp;
	//menu
	while(1){
		printf("/");
		input1 = malloc(500);
		getline(&input1,&bufsize,stdin);

		input=strtok(input1,"\n");
		command = strtok(input," ");
		if(command==NULL)
			continue;
		if(strcmp(command,"search")==0){

			MaxHeapInfo *MaxHeap = malloc(counter*sizeof(MaxHeapInfo));
			for(i=0;i<counter;i++){
				MaxHeap[i].data = malloc(sizeof(ScoreData));
			}

			char *data2=malloc(max_words*max_length+1);
			int D=0;
			float avgdl;
			double score;

			data = strtok(NULL,"");
			
			for(i=0;i<counter;i++){
				D += DocMap[i].NumOfWords;//sunolika words sto keimeno
			}
			avgdl = (float)D/counter;
			int search;

			for(id=0;id<counter;id++){
				strcpy(data2,data); 
				search = TrieSearchScore(Root,DocMap,id,counter,data2,DocMap[id].NumOfWords,avgdl,&score);
				if(search==1)//uparxei mia toulaxiston leksi sto keimeno
					InsertMaxHeap(MaxHeap,id,score);//eisagwgh sto max heap
			}
			ScoreData *max = malloc(sizeof(ScoreData));
			if(counter<k)
				k = counter;
				
			WordListIndex *WordList;
			for(i=0;i<k;i++){
				max = ExtractMaxHeap(MaxHeap);
				strcpy(data2,data); 

			 	WordList = NULL;
				WordList = SearchForWordList(Root,max->id,data2);//eisagwdh twn leksewn tou keimenou pou htan sto input
	
				PrintTopK(max->id,max->score,DocMap[max->id].docString,WordList);//ektupwsh
				FreeWordList(&WordList);
			}

			int heapSize = GetHeapSize();
			InitHeapSize();
			
			for(i=0;i<heapSize;i++)
				free(MaxHeap[i].data);
			free(MaxHeap);
			free(max);
			free(data2);
		}
		else if(strcmp(command,"df")==0){
			data = strtok(NULL,"");

			DF_Trie(Root,data);
			data=NULL;
		}
		else if(strcmp(command,"tf")==0){
			data = strtok(NULL,"");
			if(data==NULL)
				continue;
			temp=strtok(data," ");
			id = atoi(temp);
			temp=strtok(NULL," ");
			TF_Trie(Root,id,temp);
			data=NULL;
			temp=NULL;
		}
		else if(strcmp(command,"exit")==0){
			fclose(docfile);
			printf("Exiting...\n");
			free(input1);
			break;
		}
		else{
			printf("Den uparxei auto to command\n");
		}

	}
	TrieDestroy(&Root);
	for(i=0;i<counter;i++){
		free(DocMap[i].docString);
		DocMap[i].docString=NULL;
	}
	free(DocMap);
	free(str1);
}



































