#include "MI.h"
#include <math.h>
#include "KDTree.h"
#include "DiGamma.h"
#include <iostream>
using namespace std;
MIPoint::MIPoint(double* c)
{
   mCoord=c;
}
MIPoint::MIPoint(double x, double y)
{
   mCoord=new double[2];

   mCoord[0] =x ;
   mCoord[1] =y;
}
MIPoint::~MIPoint()
{
   delete mCoord;
}

double 
MIPoint::dist(MIPoint* p, int dim)
{
   double max=0;
   for(int i=0; i<dim; i++)
      if(fabs(p->coord(i)-coord(i))>max)
	  max=fabs(p->coord(i)-coord(i));
   return max;

}


MutualInfo::MutualInfo(ReplicationParser* r)
{
   mReplications=r;
   mNumElement=r->numCols();
}



double
MutualInfo::estimateMI04(int r1, int r2, int k)
{

   //first we need to get the ei/2
  double* e2values = calculateE2(r1, r2, k);//
  int*    Nx       = calculateN(mReplications->replication(r1), mNumElement, e2values);
  int*    Ny       = calculateN(mReplications->replication(r2), mNumElement, e2values);
  
  double sum=0;
  for(int i=0; i<mNumElement; i++)
  {
      sum+=DiGamma::digamma(Nx[i])+DiGamma::digamma(Ny[i]);
  }
  
  return DiGamma::digamma(k)-1/k-1.0/mNumElement*sum+DiGamma::digamma(mNumElement);
}



int rankCmp(const void *_a, const void *_b)
{
    struct RankItem * c1 = (struct RankItem *) _a;
    struct RankItem * c2 = (struct RankItem *) _b;

    if(c1->value<c2->value) return -1;
    return 1;
}



//first we need to get the ei/2
//it will calculate ei/2. the ei/2 will be stored in an array

double* 
MutualInfo::calculateE2(int r1, int r2, int k)
{
    double* mResult=new double[mNumElement];
    //first, we build n points
    mPoints = new MIPoint *[mNumElement];
    for(int i=0; i< mNumElement; i++)
       mPoints[i]=new MIPoint(mReplications->value(r1, i), mReplications->value(r2, i));

    //then, we build a KDTree this these points
    KDTree* kdTree =new KDTree(mPoints, mNumElement, 2);

    //now, build a TopKTree for each point, according to the heap
    //
    for(int i=0; i< mNumElement; i++)
    {
      TopKHeap* topK=kdTree->KNN(mPoints[i], k+1);
      mResult[i] = topK->threshold();
      //cout<<topK->threshold()<<endl; 
      delete topK;
    } 

    delete kdTree;
    //delete the point as well?!

    return mResult;
}


//then we need to calculte nx according to ei/2

int*
MutualInfo::calculateN(double *vect, int n, double* threshold)
{
   int* rev=new int[n];

   struct RankItem * items =new struct RankItem[n];
   for(int i=0; i< n; i++)
   { 
      items[i].value=vect[i];
      items[i].i    =i;
   }//for 
   
   qsort(items, n, sizeof(struct RankItem), rankCmp);


   int lower=0;
   int upper=0;
   for(int i=0; i<n; i++)
   {

     int lower = lowerBound(items, items[i].value-threshold[items[i].i], 0, n-1);
     int upper = upperBound(items, items[i].value+threshold[items[i].i], 0, n-1);
     
     if(upper-lower-1<1)
	 rev[items[i].i]=1;// upper-lower-1;
     else rev[items[i].i] = upper-lower-1;

    cout<<threshold[items[i].i]<<" "<<rev[items[i].i]<<endl;
     
   }//for

   //exit(0);
   return rev;
}


//find the lower bound, such the value of the element is strictly larger than lower; binary search of course
int
MutualInfo::lowerBound(RankItem* items, double lower, int from, int to)
{
  //cout<<from<<":"<<items[from].value<<" "<< "----------"<<lower<<"------"<<to<<":"<<items[to].value<<""<<endl;
  if(items[to].value<lower) return -1;//this should never happen

  if(items[from].value>lower) 
	  return from;
  int mid = (to+from)/2; 
  
  if(to-from<=1)
      return to; 

  //the below line will give a loglogn resuls on averge, maybe turn it on when the dataset is large
  //int mid=(int)(to+(to-from)*(lower-items[from])/(items[to]-items[from])) 
  //

  if(items[mid].value <=lower)
     return lowerBound(items, lower, mid, to);
  else return lowerBound(items, lower, from, mid);
}


//find the upper bound, such the value of the element is strictly less than upper; binary search of course
int
MutualInfo::upperBound(RankItem* items, double upper, int from, int to)
{
 // cout<<from<<":"<<items[from].value<<" "<< "----------"<<upper<<"------"<<to<<":"<<items[to].value<<""<<endl;
  if(items[from].value>upper) return -1;//this should never happen

  if(items[to].value < upper) 
	  return to;
  int mid = (to+from)/2;
  if(to-from<=1)
      return to; 
  
  //the below line will give a loglogn resuls on averge, maybe turn it on when the dataset is large
  //int mid=(int)(to+(to-from)*(lower-items[from])/(items[to]-items[from])) 
  //

  if(items[mid].value >=upper)
     return upperBound(items, upper, from, mid);
  else return upperBound(items, upper, mid, to);
}





/*
int[]
MutualInfo::calculateNx(double *v, int n, int k)
{
   int* rev=new int[n];

   struct RankItem * items =new struct RankItem[n];
   for(int i=0; i< n; i++)
   { 
      item[i].value=v[i];
      item[i].i    =i;
   }//for 
   
   qsort(items, n, sizeof(struct RankItem), rankCmp);

   for(int i=0; i<n; i++)
   {
     double ek2=items[n-1].value, items[0].value;	   
     for(int from=max(0, i-k); from<=min(i, i-k); from++)
     {
        double left  = items[i     ].value - items[from].value;  
        double right = items[min(k+from, n-1)].value - items[i   ].value;
        if(ek2<max(left, right))
	  ek2=max(left, right);
     } //for
   
   
   }//for

}
*/
