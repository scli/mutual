#include "MICMath.h"
#include <stdlib.h>

#include <iostream>
using namespace std;
int min(int x, int y)
{
   if(x<y) return x;
   return y;
}
int max(int x, int y)
{
   if(x>y) return x;
   return y;

}

int * permutation(int n)
{
  int* rev=new int[n];
  
  for(int i=0; i<n; i++)
    rev[i]=i;

  for(int i=0; i<n; i++)
  {
     int which  = i+(int)rand()%(n-i);
     int temp   = rev[i];
     rev[i]     = rev[which];
     rev[which] = temp;  
  }
  //for(int i=0; i<n; i++)
  // cout<<rev[i]<<" "<<endl;
  //cout<<endl;
  return rev;

}
