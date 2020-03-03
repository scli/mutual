#include "DiGamma.h"
#include <iostream>
#include <string.h>
using namespace std;
double DiGamma::gamma = 0.577215664901;
double * DiGamma::harmonicPreSum = NULL;
int      DiGamma::harmonicArraySize=0;

double 
DiGamma::digamma(int n)
{
   if(n>harmonicArraySize)
   { 
      //extend the hormoic pre sum array to have size n+10; 
     double* preSumArray=new double[n+10];
     memcpy(preSumArray, harmonicPreSum, harmonicArraySize*sizeof(double));
     delete         [] harmonicPreSum; 
     harmonicPreSum =  preSumArray; 
     calculateHarmonicPreSum(n+10);
     harmonicArraySize = n+10;
     
   }
   if(n<=0)
   {
     cerr<<"error with calling digamma "<<n <<endl;
     exit(0);
   }
   return harmonicPreSum[n-1]-gamma;
}


void
DiGamma::calculateHarmonicPreSum(int n)
{
    if(n<=harmonicArraySize)
	 return;
    if(n==1)
    {
      harmonicPreSum[n-1] = 0;
      return;
    } 
    
    calculateHarmonicPreSum(n-1);
    harmonicPreSum[n-1]=1.0/(n-1)+harmonicPreSum[n-2];
}



