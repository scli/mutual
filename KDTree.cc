#include "KDTree.h"
#include "MICMath.h"

KDNode::KDNode(MIPoint * p)
{
   mPoint = p;
   mLeft  = NULL;
   mRight = NULL;
}

KDNode::KDNode(MIPoint * p, KDNode* l, KDNode* r)
{
   mPoint =p;
   mLeft  =l;
   mRight =r;
}

KDTree::KDTree(MIPoint** pts, int n, int d)
{
   mNumPoint = n; 
   mPoint    = pts;
   mDimension= d;
   mRoot=NULL;
   createTree();
}

void
KDTree::createTree()
{
  //first, create a random permutations of 0 to n-1
  int* p=permutation(mNumPoint);

  //insert the points into the tree in random order;
  for(int i=0; i<mNumPoint; i++)
  {
     insert(mPoint[p[i]]);
  }

  delete [] p;
}

void
KDTree::insert(MIPoint* aPoint)
{
   if(mRoot == NULL)
   {
      mRoot=new KDNode(aPoint);
      return;
   }
   
   insert(mRoot, aPoint, 0);
}

void
KDTree::insert(KDNode* parent, MIPoint* aPoint, int depth)
{
   int currentDim = depth % mDimension;
  //if its less the current current coodinate of the current root, on the left, 
  //
   if(aPoint->coord(currentDim) < parent->coord(currentDim))
   {
      if(parent->left()==NULL)
	 parent->left(new KDNode(aPoint));
      else 
	 insert(parent->left(), aPoint, depth+1);
   }
   else//on the right
   {
      if(parent->right()==NULL)
	 parent->right(new KDNode(aPoint));
      else
	 insert(parent->right(), aPoint, depth+1);
   }
}

TopKHeap*
KDTree::KNN(MIPoint* aPoint, int k)
{
   TopKHeap* heap=new TopKHeap(k);
   KNN(aPoint, mRoot, heap, 0);
   return heap;
}

void
KDTree::KNN(MIPoint* aPoint, KDNode* parent, TopKHeap* heap, int depth)
{

    if(parent == NULL) return;

    heap->insert(parent->point(), aPoint->dist(parent->point(), mDimension));
  
    int currentDim = depth % mDimension;
  
  //see if you can prune the left: if you are at further enough right, then you can 
  if(aPoint->coord(currentDim) - parent->coord(currentDim)<heap->threshold())
      KNN(aPoint, parent->left(), heap, depth+1);

  //now check if you can prine the right: yes only if you are at the further enough left 
  if(parent->coord(currentDim) - aPoint->coord(currentDim)<heap->threshold())
     KNN(aPoint, parent->right(), heap, depth+1);
}

