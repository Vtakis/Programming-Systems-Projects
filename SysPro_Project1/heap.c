/*FILE: heap.c */
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

//////////////////////
//PROJECT 1 - K24   //
//VLASSIS PANAGIOTIS//
//AM: 1115201400022 //
//////////////////////

int curHeapSize = 0;//trexon megethos tou min-heap

//arxikopoihsh megethous Heap
void InitHeapSize(){
	curHeapSize=0;
}

//epistrofh megethous heap
int GetHeapSize(){
	return curHeapSize;
}

void swap(ScoreData *x,ScoreData *y)
{
    ScoreData temp = *x;
    *x = *y;
    *y = temp;
}

//eisagwgh sto max heap
void InsertMaxHeap(MaxHeapInfo *MaxHeap,int id,double score){
	
	int pos;
	pos = curHeapSize;
	curHeapSize++;
	MaxHeap[pos].data->id = id;
	MaxHeap[pos].data->score = score;

	while( pos!=0 && MaxHeap[(pos-1)/2].data->score < MaxHeap[pos].data->score ){
		swap(MaxHeap[pos].data,MaxHeap[(pos-1)/2].data);
		pos = (pos-1)/2;
	}
}

//anasuntaksi tou max heap
void HeapifyMaxHeap(MaxHeapInfo *MaxHeap,int parent){
	int left,right,bigger;
	left = 2*parent+1;
	right = 2*parent+2;
	bigger=parent;

	if( left < curHeapSize && MaxHeap[left].data->score > MaxHeap[parent].data->score )
		bigger = left;
	if( right < curHeapSize && MaxHeap[right].data->score > MaxHeap[bigger].data->score )
		bigger = right;
	if( bigger!=parent ){
		swap(MaxHeap[parent].data,MaxHeap[bigger].data);
		HeapifyMaxHeap(MaxHeap,bigger);
	}
}

//epistrofh rizas tou max heap
ScoreData *ExtractMaxHeap(MaxHeapInfo *MaxHeap){
	if(curHeapSize==0)
		return NULL;

	if(curHeapSize==1){
		curHeapSize--;
		return MaxHeap[0].data;
	}
	
	ScoreData *max = MaxHeap[0].data;
	MaxHeap[0].data = MaxHeap[curHeapSize-1].data;
	curHeapSize--;
	HeapifyMaxHeap(MaxHeap,0);
	return max;
}
