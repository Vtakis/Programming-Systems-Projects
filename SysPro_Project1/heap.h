/*FILE: heap.h */

#ifndef HEAP_H
#define HEAP_H

typedef struct MaxHeapInfo MaxHeapInfo;
typedef struct ScoreData ScoreData;

struct ScoreData{
	int id;
	double score;
};

struct MaxHeapInfo{
	ScoreData *data;
};
void InsertMaxHeap(MaxHeapInfo*,int,double);
void InitHeapSize();
void swap(ScoreData *,ScoreData *);
void HeapifyMaxHeap(MaxHeapInfo *MaxHeap,int);
ScoreData *ExtractMaxHeap(MaxHeapInfo *MaxHeap);
int GetHeapSize();
#endif
