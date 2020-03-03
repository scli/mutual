#ifndef _KD_TREE_
#define _KD_TREE_
#include "MI.h"
#include "TopKHeap.h"
class KDNode 
{
    MIPoint* mPoint; 
    KDNode * mLeft;
    KDNode * mRight;
   
   public:  
     KDNode(MIPoint * p);
     KDNode(MIPoint * p, KDNode* l, KDNode* r);
     ~KDNode()  { ; }
     double coord(int i) { return mPoint->coord(i); }
     KDNode* left()  { return mLeft;  }
     KDNode* right() { return mRight; }
     void  left(KDNode * n) { mLeft  = n; }
     void right(KDNode * n) { mRight = n; }
     MIPoint* point()  { return mPoint; }
};

class KDTree
{
   int mNumPoint; 
   MIPoint** mPoint;
   int mDimension;
   KDNode* mRoot;

   void createTree();
   void insert(MIPoint* aPoint);
   void insert(KDNode* parent, MIPoint* aPoint, int depth);
   void KNN(MIPoint* aPoint, KDNode* parent, TopKHeap* heap, int depth);

   public: 
      KDTree(MIPoint** pts, int n, int d);
     
      TopKHeap* KNN(MIPoint* aPoint, int k);

};

#endif
