#ifndef _MUTUAL_INFORMATION_
#define _MUTUAL_INFORMATION_

#include "ReplicationParser.h"

using namespace std;


class MIPoint
{
   double* mCoord;

   public:
     MIPoint(double* c);
     MIPoint(double x, double y);
     ~MIPoint();
     double coord(int i) { return mCoord[i]; }
     double dist(MIPoint* p, int dim);
};



struct RankItem
{
   int i;
   double value;
};

class MutualInfo
{
   
      ReplicationParser* mReplications; 
      int mNumElement; 
      MIPoint** mPoints;


      int lowerInclusiveBound(RankItem* items, double lower, int from, int to);
      int upperInclusiveBound(RankItem* items, double upper, int from, int to);
      int * calculateM(double *vect, int n, double* threshold);


      int lowerBound(RankItem* items, double lower, int from, int to);
      int upperBound(RankItem* items, double upper, int from, int to);
      int * calculateN(double *vect, int n, double* threshold);

      double* calculateE2(int r1, int r2, int k);
      struct RankItem *countNx(double *v, int n, int* from, int* Nx);
      double* calculateE2(struct RankItem * items, int r1, int r2, int k, int* from, int* Nx);

   public:

     //assume it is row major. 
      MutualInfo(ReplicationParser* replication);	   
     
      double estimateMI04(int r1, int r2, int k); 
      double estimateMI04(int k, const char* filename); 
      
      //assume the vector indexed by r1 is discrete and by r2 is continuous.
      double estimateMI14(int r1, int r2, int k);      
      double estimateMI14(int k, const char* filename); 
      
      //implement the method of
      //PHYSICAL REVIEW E 69, 066138 (2004)
      //Estimating mutual information
      //Alexander Kraskov, Harald Stögbauer, and Peter Grassberger 
      double estimateMI04(double* v1, double* v2, int n, int);



};
#endif
