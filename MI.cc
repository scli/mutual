#include "MI.h"
#include <math.h>
#include "KDTree.h"
#include "DiGamma.h"
#include <iostream>
#include <fstream>
#include <string.h>
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
   mNumElement=r->numPoints();
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
  
  return DiGamma::digamma(k)-1.0/k-1.0/mNumElement*sum+DiGamma::digamma(mNumElement);
}



int rankCmp(const void *_a, const void *_b)
{
    struct RankItem * c1 = (struct RankItem *) _a;
    struct RankItem * c2 = (struct RankItem *) _b;

    if(c1->value<c2->value) return -1;
    return 1;
}


void
MutualInfo::estimateMI04(int k, const char* output)
{
   if(strlen(output)==0)
   {
    if(mReplications->numReplications()==2)
    { 
        cout<<estimateMI04(0, 1, k)<<endl;
        return;
    }
    for(int i=0; i<mReplications->numReplications(); i++)
     { //if there are many replicates, may consider reusing the qsort: not implemented yet.  
      for(int j=0; j<mReplications->numReplications(); j++)
        cout<<estimateMI04(i, j, k)<<" \t";
      cout<<endl;
       
     }
     return;
   }

   ofstream out(output);
   if(!output)
   {
      cerr<<"Can not open the output file "<<output<<endl;
      exit(0);
   }
  
       if(mReplications->numReplications()==2)
    {
        out<<estimateMI04(0, 1, k)<<endl;
        return;
    } 
  for(int i=0; i<mReplications->numReplications(); i++)
  {
      for(int j=i+1; j<mReplications->numReplications(); j++)
        out<<estimateMI04(i, j, k)<<" \t";
       out<<endl;
  }
  out.close();

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
      TopKHeap* topK=kdTree->KNN(mPoints[i], k+1);//plus 1 for itself
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
   //for(int i=0; i< n; i++)
   //  cout<<i<<" :\t" <<items[i].value<<endl;

   for(int i=0; i<n; i++)
   {

     int lower = lowerBound(items, items[i].value-threshold[items[i].i], 0, n-1);
     int upper = upperBound(items, items[i].value+threshold[items[i].i], 0, n-1);
     
     //cout<<i<<"\t"<<items[i].value<<" "<<lower<<" "<<upper<<" "<<(items[i].value-threshold[items[i].i])<<" \t" <<(items[i].value+threshold[items[i].i])<<endl;
     if(upper-lower-1<1)
	 rev[items[i].i]=1;// upper-lower-1;
     else rev[items[i].i] = upper-lower;

    //cout<<threshold[items[i].i]<<" "<<rev[items[i].i]<<endl;
     
   }//for
   
   delete [] items; 
   //exit(0);
   return rev;
}


//find the lower bound, such the value of the element is strictly larger than lower; binary search of course
int
MutualInfo::lowerBound(RankItem* items, double bound, int from, int to)
{
  int mid =(to+from)/2;
  //if(from==to) return to;
  if((mid-1 < 0 || items[mid-1].value < bound) &&  bound<= items[mid].value)
     return mid;

  if(items[mid].value < bound)
       return lowerBound(items, bound, mid+1,  to);
  else return lowerBound(items, bound, from, mid-1);
}


//find the upper bound, such the value of the element is strictly less than upper; binary search of course
int
MutualInfo::upperBound(RankItem* items, double bound, int from, int to)
{
 

 int mid = (to+from)/2;
 //if(from==to) return to
 // cout<<from<<":"<<items[from].value<<" "<< "----------"<<upper<<"------"<<to<<":"<<items[to].value<<""<<endl;
  //if(items[from].value>upper) return -1;//this should never happen
  

 if(items[mid].value <= bound && (mid+1==mNumElement || bound<items[mid+1].value))
     return mid;

  //if(items[to].value < upper) 
//	  return to;
  //int mid = (to+from)/2;
  //if(to-from<=1)
  //    return to; 
  
  //the below line will give a loglogn resuls on averge, maybe turn it on when the dataset is large
  //int mid=(int)(to+(to-from)*(lower-items[from])/(items[to]-items[from])) 
  //

  if(items[mid].value > bound)
       return upperBound(items, bound, from, mid-1);
  else return upperBound(items, bound, mid+1, to);
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
