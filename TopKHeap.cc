#include "TopKHeap.h"
#include <float.h>
#include <iostream>
using namespace std;


HeapNode::HeapNode(MIPoint* p, double dist)
{
    mPoint = p;
    mDist  = dist;
}


TopKHeap::TopKHeap(int k)
{
    mK    =k;
    mSize =0;
    mHeapArray=new HeapNode* [mK+1];
    
    for(int i=0; i< mK+1; i++)
    {
	mHeapArray[i] = NULL;    
    }
}

TopKHeap::~TopKHeap()
{
   delete [] mHeapArray;
}

int 
TopKHeap::parent(int which)
{
   return which/2;
}

int
TopKHeap::left(int which)
{
   return which*2;
}

int 
TopKHeap::right(int which)
{
   return which*2+1;
}

void
TopKHeap::swap(int a, int b)
{
   HeapNode* temp; 
   temp          = mHeapArray[a]; 
   mHeapArray[a] = mHeapArray[b]; 
   mHeapArray[b] = temp; 
}

bool
TopKHeap::leaf(int which) 
{ 
   if (which > (mSize / 2) && which <= mSize)  
            return true;  
   return false; 
}

bool
TopKHeap::hasRight(int which)
{
  return which*2+1<=mSize;
}


double
TopKHeap::threshold()
{
    if(mHeapArray[1]==NULL || mSize < mK)
	return DBL_MAX;
    return mHeapArray[1]->dist();
}


void 
TopKHeap::heapify(int which) 
{ 
   if (leaf(which)) return; 
   
   if (mHeapArray[which]->dist() < mHeapArray[ left(which)]->dist())
   {
       if(!hasRight(which) || mHeapArray[left(which)]->dist() >= mHeapArray[right(which)]->dist())  {
          swap   (which, left(which)); 
          heapify(       left(which));
       }
       //otherwise, has right; it right is better
       else {
          swap(which, right(which)); 
          heapify(    right(which)); 
       }
       return;
   }
   //left is less, now check the right
   if(hasRight(which) && mHeapArray[which]->dist() <= mHeapArray[right(which)]->dist())
   {
        swap(which, right(which)); 
        heapify(    right(which)); 
   }
} 

void
TopKHeap::insert(MIPoint* p, double dist) 
{
   //cout<<dist <<endl;	
   if(mSize<mK)	
   {
      mSize++;
      mHeapArray[mSize]=new HeapNode(p, dist);
      int index = mSize; 
      while (index>1 && mHeapArray[index]->dist() > mHeapArray[parent(index)]->dist()) { 
            swap(index, parent(index)); 
            index = parent(index); 
      } 
      return;
   }
  
   if(dist>=mHeapArray[1]->dist())//do nothing!
      return;

   delete mHeapArray[1]; 
   mHeapArray[1]=new HeapNode(p, dist); 
   heapify(1); 
} 
