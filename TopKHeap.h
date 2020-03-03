#ifndef _TOP_K_HEAP_
#define _TOP_K_HEAP_

#include "MI.h"
//top k heap is heap of k element, which stores nearest top k points to the reference point

class HeapNode
{  
     MIPoint* mPoint; 
     double mDist;
   
   public: 
     HeapNode(MIPoint* p, double dist);
     ~HeapNode() {;}
     double dist() { return mDist; }

};

class TopKHeap
{
      int mK;
      int mSize; 
      HeapNode** mHeapArray;

      int parent(int i);
      int left  (int i);
      int right (int i);
      bool leaf (int i);
      void swap(int a, int b);
      bool hasRight(int i);

      void heapify(int i);
    public:
      TopKHeap(int k);
      ~TopKHeap();

      double threshold();
      void   insert(MIPoint* p, double dist);


};



#endif
