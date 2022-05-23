/*FILE: minisearch.h */

#ifndef MINISEARCH_H
#define MINISEARCH_H

typedef struct DocContext DocContext;
typedef struct PostingList PostingList;
typedef struct PostingIndex PostingIndex;
typedef struct Trie Trie;
typedef struct WordListIndex WordListIndex;

struct WordListIndex{
	char* word;
	WordListIndex *next;
};

struct DocContext{
	int doc_id;//anagnwristiko keimenou
	int NumOfWords;//ari8mos leksewn keimenou
	char* docString;
};

struct PostingList{
	int doc_id;//anagnwristiko keimenou
	int freq_word;//poses fores emfanizetai sto trexon keimeno
	PostingList *next;
};

struct PostingIndex{
	int total_freq;//poses fores emfanizetai sunolika h leksi
	int DocFreq;//se posa keimena emfanizetai h leksi
	PostingList *PostList;
};

struct Trie{
	char letter;
	PostingIndex *PostIndex;
	Trie *LetterCurLvl;//geitonas
	Trie *LetterNextLvl;//paidi
};

int CreateRootTrie(Trie **);
void TrieDestroy(Trie **);
void TrieDestroy(Trie **);
Trie *CreateNodeTrie(char);
int TrieInsert(Trie **,char*,int);
WordListIndex *SearchForWordList(Trie *,int,char*);
int FindWord(WordListIndex *,char*);
void FreeWordList(WordListIndex **WordList);
int TrieSearchScore(Trie *,DocContext *,int,int,char*,int,double,double*);
double CalcScore(int,int,int,int,PostingIndex *);
int DF_Trie(Trie *,char *);
int TF_Trie(Trie *,int,char*);
void TrieRecTrav(Trie *,char *,int);
void PrintTopK(int,double,char*,WordListIndex*);
#endif
