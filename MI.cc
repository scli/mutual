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
      sum+=DiGamma::digamma(Nx[i]+1)+DiGamma::digamma(Ny[i]+1);
  }
  
  return DiGamma::digamma(k)-1.0/mNumElement*sum+DiGamma::digamma(mNumElement);
}



int rankCmp(const void *_a, const void *_b)
{
    struct RankItem * c1 = (struct RankItem *) _a;
    struct RankItem * c2 = (struct RankItem *) _b;

    if(c1->value<c2->value) return -1;
    return 1;
}


double
MutualInfo::estimateMI04(int k, const char* output)
{
   double ret = 0;
   if(strlen(output)==0)
   {
    if(mReplications->numReplications()==2)
    { 
        ret = estimateMI04(0, 1, k);
        cout<< ret <<endl;
        return ret;
    }
    for(int i=0; i<mReplications->numReplications(); i++)
     { //if there are many replicates, may consider reusing the qsort: not implemented yet.  
      for(int j=0; j<mReplications->numReplications(); j++)
      {
        ret = estimateMI04(i, j, k);
        cout << ret << " \t";
      }
      cout<<endl;
       
    }
    return ret;
   }

   ofstream out(output);
   if(!output)
   {
      cerr<<"Can not open the output file "<<output<<endl;
      exit(0);
   }
  
   if(mReplications->numReplications()==2)
   {
       ret = estimateMI04(0, 1, k);
       out << ret << endl;
       return ret;
   } 
  for(int i=0; i<mReplications->numReplications(); i++)
  {
      for(int j=i+1; j<mReplications->numReplications(); j++)
      {
        ret = estimateMI04(i, j, k);
        out << ret << " \t";
      }
       out<<endl;
  }
  out.close();
  return ret;

}




double
MutualInfo::estimateMI14(int k, const char* output)
{
   //cout<<"I reach here"<<endl;
   double ret = 0;
   if(strlen(output)==0)
   {
    if(mReplications->numReplications()==2)
    { 
        ret = estimateMI14(0, 1, k);   
        cout << ret << endl;
        return ret;
    }
   }

   ofstream out(output);
   if(!output)
   {
      cerr<<"Can not open the output file "<<output<<endl;
      exit(0);
   }
  
    if(mReplications->numReplications()==2)
    {
        ret = estimateMI14(0, 1, k);
        out << ret << endl;
        out.close();
        return ret;
    } 
    return ret;

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
     

     //below codes are for debugging 
      int counter=0;
      for(int j=0; j<mNumElement; j++) 
         if(mPoints[j]->dist(mPoints[i], 2)<=mResult[i]) counter++;
      if(k+1!=counter)
      {
         cerr<<"Got bugs in calculating the top k threshold"<<endl;
      }
     //above codes are for debugging

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
 
    /*  
   //below are the code for debugs purpose only
   for(int i=0; i<n; i++)
   {
      cout<<items[i].i<<" \t/// "<<items[i].value<<" \t/// "<<vect[items[i].i]<<endl;
   
   }




   for(int k=0; k<n; k++)
   { 
//	int counterSame =0;
        int counterAll  =0;
	for(int j=0; j<n; j++)
	{
	   if(fabs(items[k].value-items[j].value)<threshold[items[k].i])
	   {
	      counterAll++;
	   }
	}
	cout<<items[k].i<<"\t"<<counterAll<<"\t "<<"\t +++"<<endl;
   }*/
   //end of debug 



   int lower=0;
   int upper=0;
  // for(int i=0; i< n; i++)
   //  cout<<i<<" :\t" <<items[i].value<<endl;

   for(int i=0; i<n; i++)
   {

     if(threshold[items[i].i]>0)
     {
        lower = lowerBound(items, items[i].value-threshold[items[i].i], 0, n-1);
        upper = upperBound(items, items[i].value+threshold[items[i].i], 0, n-1);
     }
     else
     {
        lower=items[i].i;
        upper=items[i].i;
        cout<<"ever reach here?"<<endl;
     }
     
     //cout<<i<<"\t"<<items[i].value<<" "<<lower<<" "<<upper<<" "<<(items[i].value-threshold[items[i].i])<<" \t" <<(items[i].value+threshold[items[i].i])<<endl;
     rev[items[i].i] = upper-lower;//exclude the element itself
    
     //below codes are for debugging
     /*int counter=0;
     for(int j=0; j<n; j++)
        if(fabs(items[j].value-items[i].value) < threshold[items[i].i])
          counter++;
     if(counter!=rev[items[i].i]+1)
     {
       cout<<"bugs in calculating the subspace neighbors   "<<counter<<" "<<rev[items[i].i]+1<<endl;
       cout<<lower<<" "<<upper<<" "<<i<<endl;
       for(int j=0; j<n; j++)
        if(fabs(items[j].value-items[i].value) < threshold[items[i].i])
           cout<<j<<" ";
        cout<<endl;

	cout<<items[lower].value<<" "<<items[i].value-threshold[items[i].i]<<"\t tttt\t" <<items[upper].value<<" "<<items[i].value+threshold[items[i].i]<<endl;
     }*/
       //the above codes are for debugging
    
    //cout<<"+++++++"<<i<<" "<<threshold[items[i].i]<<" "<<rev[items[i].i]<<endl;
     
   }//for

 
   delete [] items; 
   //exit(0);
   return rev;
}



int*
MutualInfo::calculateM(double *vect, int n, double* threshold)
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

     if(threshold[items[i].i]>0)
     {
        lower = lowerInclusiveBound(items, items[i].value-threshold[items[i].i], 0, n-1);
        upper = upperInclusiveBound(items, items[i].value+threshold[items[i].i], 0, n-1);
     }
     else
     {
        lower=items[i].i;
        upper=items[i].i;
     }
     
     //cout<<i<<"\t"<<items[i].value<<" "<<lower<<" "<<upper<<" "<<(items[i].value-threshold[items[i].i])<<" \t" <<(items[i].value+threshold[items[i].i])<<endl;
     rev[items[i].i] = upper-lower+1;//include the element itself

    //cout<<threshold[items[i].i]<<" "<<rev[items[i].i]<<endl;
     
   }//for
   
   delete [] items; 
   //exit(0);
   return rev;
}






//find the index of the first element, such the value of the element is strictly larger than lower; binary search of course
int
MutualInfo::lowerBound(RankItem* items, double bound, int from, int to)
{
  if(items[from].value > bound) return from;
  
  int mid =(to+from)/2;
  while(items[mid].value==bound && mid<to) mid++; 
  
  if((mid-1 < 0 || items[mid-1].value <= bound) &&  bound < items[mid].value)
     return mid;

  if(items[to].value==bound)//it should not happen
  {
     cerr<<"entering an invalid case "<<to<<endl;
     return to+1;//should be to +1;
  }


  if(items[mid].value < bound)
       return lowerBound(items, bound, mid+1,  to);
  else return lowerBound(items, bound, from, mid-1);
}


//find the the index of the last element, such the value of the element is strictly less than buond; binary search of course
int
MutualInfo::upperBound(RankItem* items, double bound, int from, int to)
{
  if(items[to].value < bound) return to;

  int mid = (to+from)/2;
  while(items[mid].value==bound && from<mid) mid--; 

  if(items[mid].value < bound && (mid+1==mNumElement || bound<=items[mid+1].value))
     return mid;

  if(items[from].value==bound)//it should not happen
  {
     return from-1;//should be to +1;
  }

  //the below line will give a loglogn resuls on averge, maybe turn it on when the dataset is large
  //int mid=(int)(to+(to-from)*(lower-items[from])/(items[to]-items[from])) 
  //


  if(bound < items[mid].value )
       return upperBound(items, bound, from, mid-1);
  else return upperBound(items, bound, mid+1, to);
}




//find the index of the first element, such the value of the element is euqal to or larger than lower; binary search of course
int
MutualInfo::lowerInclusiveBound(RankItem* items, double bound, int from, int to)
{
  int mid =(to+from)/2;
  if((mid-1 < 0 || items[mid-1].value < bound) &&  bound <= items[mid].value)
     return mid;

  if(items[mid].value < bound)
       return lowerInclusiveBound(items, bound, mid+1,  to);
  else return lowerInclusiveBound(items, bound, from, mid-1);
}


int
MutualInfo::upperInclusiveBound(RankItem* items, double bound, int from, int to)
{
 

 int mid = (to+from)/2;

 if(items[mid].value <= bound && (mid+1==mNumElement || bound<items[mid+1].value))
     return mid;

  if(bound < items[mid].value )
       return upperInclusiveBound(items, bound, from, mid-1);
  else return upperInclusiveBound(items, bound, mid+1, to);
}





double
MutualInfo::estimateMI14(int r1, int r2, int k)
{
   //first we calculate the Nx1, the number of element which is the 
   //same as the element indexed by 1
   int* from = new int[mNumElement];
   int*   Nx = new int[mNumElement];
   struct RankItem* items=countNx(mReplications->replication(r1), mNumElement, from, Nx);
  
   for(int i=0; i<mNumElement; i++)
   {
     if(Nx[i]<k+1)
     {
       cerr<<"k seems too large; it should not be larger than "<<Nx[i]-1<<" for this dataset."<<endl;
       exit(0);
     }
   }  


 
   double* thresholds =calculateE2(items, r1, r2, k, from, Nx);
   //among the elements same as element, we want to obtains the 
   //threshold
   //

   //then we need to count the number mi, according to the threshold
   int * m      =calculateN(mReplications->replication(r2), mNumElement, thresholds);
   
   
    //below are the code for debugs purpose only
   /*for(int i=0; i<mNumElement; i++)
   { 
	int counterSame =0;
        int counterAll  =0;
	for(int j=0; j<mNumElement; j++)
	{
	   if(fabs(mReplications->value(r2, i)-mReplications->value(r2, j))<thresholds[i])
	   {
	      counterAll++;
	      if(mReplications->value(r1, i)==mReplications->value(r1, j))
		  counterSame++;
	   }
	}
	cout<<i<<"\t"<<counterAll<<"\t +++\t"<<counterSame<<"\t +++\t"<<m[i]<<endl;
   }*/
   //end of debug 
   
   
   
   
   double sum=0; 
   for(int i=0; i<mNumElement; i++)
   {
                            //-1 here for the element itself
     //cout<<m[items[i].i]<<" "<<Nx[items[i].i]<<endl;
     ///cout.flush();
                          //plus one here is to count the boundary
     sum+=DiGamma::digamma(m[items[i].i]+1)+DiGamma::digamma(Nx[items[i].i]);
   }
   delete [] from;
   delete [] Nx;
   delete [] items; 
   delete [] m;
   delete [] thresholds;

   return DiGamma::digamma(mNumElement)+DiGamma::digamma(k)-1.0/mNumElement*sum;
}


//first we need to get the ei/2
//it will calculate ei/2. the ei/2 will be stored in an array

double* 
MutualInfo::calculateE2(RankItem * items, int r1, int r2, int k, int* from, int* Nx)
{
    double* result=new double[mNumElement];
   
    for(int i=0; i < mNumElement; )
    {
       MIPoint**  points =new MIPoint *[Nx[i]];
       for(int j=0; j<Nx[i]; j++)
       {      
 	  points[j]=new MIPoint(mReplications->value(r1, items[from[i]+j].i), 
			        mReplications->value(r2, items[from[i]+j].i));
          //cout<<items[from[i]+j].i<<"\t "<<mReplications->value(r1, items[from[i]+j].i)<<" \t"<<mReplications->value(r2, items[from[i]+j].i)<<endl;
       }
       KDTree* kdTree = new KDTree(points, Nx[i], 2);
       for(int j=0; j<Nx[i]; j++)
       {
          TopKHeap* topK              = kdTree->KNN(points[j], k+1);//plus 1 for itself
          result[items[from[i]+j].i]  = topK->threshold();
          //cout<<"-----"<<items[from[i]+j].i<<" \t"<<items[from[i]+j].value<<" \t"<<topK->threshold()<<endl; 
          
         int counter=0;
         for(int l=0; l<Nx[i]; l++)
           if(points[l]->dist(points[j], 2)<=result[items[from[i]+j].i]) counter++;
         if(k+1!=counter)
         {
         cerr<<"Got bugs in calculating the top k threshold"<<endl;
         }
        //cout<<counter<<endl;
     //above codes are for debugging
          delete topK;
       } 

       delete kdTree;
       delete [] points; //use the for loop to delete the points, 
       i = i+Nx[i];

    }//for

    //for(int i=0; i<mNumElement; i++)
    //    cout<<result[i]<<endl;

    return result;
}


struct RankItem *
MutualInfo::countNx(double *vect, int n, int* from, int* Nx)
{
   struct RankItem * items =new struct RankItem[n];
   for(int i=0; i< n; i++)
   { 
      items[i].value = vect[i];
      items[i].i     = i;
      
   }//for 
   
   qsort(items, n, sizeof(struct RankItem), rankCmp);
   
   from[0]  = 0; 
   for(int i = 1;  i < n;  i++)
   {
      if(items[i].value == items[i-1].value)
	 from[i] = from[i-1];     
      else
	 from[i] = i;
   } //from

   for(int i = 0;  i< n;     )
   {
       int to=i;
       while(to+1<n && from[to]==from[to + 1])
       {
	  to++;
       }
       for(int j=i; j<=to; j++)
       {
	   Nx[j] = to-from[i]+1;
       }
       i = to + 1;
  }
  //for(int i=0; i<n; i++)
  //{
    //  cout<<items[i].value<<" "<<from[i]<<" "<<Nx[i]<<endl; 
  //}
  
   return items; 
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

