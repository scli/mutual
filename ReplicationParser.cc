#include "ReplicationParser.h"
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;
ReplicationParser::ReplicationParser(const char* name)
{
  //at this moment, we assume no rows or cols, 	
  mColNames=new vector<string>(); 
  mRowNames=new vector<string>();
  readFile(name);
}

ReplicationParser::~ReplicationParser()
{
}

void
ReplicationParser::readFile(const char* filename)
{
  ifstream input(filename);

  if(!input)
  {
    cerr<<"Can not open file "<<filename<<endl;
    exit(0);
  }

  //parse the first line; 
  vector<double>* r1=new vector<double>();
  
  
  char buf[100000];
  char* token;

  while(!input.eof())
  {
    input.getline(buf, 100000);
    token=strtok(buf, ", \t\n");
    if(token==NULL) 
        continue;
    while(token!=NULL)
    {
       r1->push_back(atof(token));
       token=strtok(NULL, ", \t\n");
    }
    break;
  }
  mNumCols = r1->size();
  double* firstRow=new double[mNumCols];
  for(int i=0; i < mNumCols; i++)
     firstRow[i]=(*r1)[i];
  delete r1;

  vector<double*>* replications=new vector<double *>(); 
  replications->push_back(firstRow); 
  while(!input.eof())
  {
    input.getline(buf, 100000);
    token=strtok(buf, ", \t\n");
    if(token==NULL) 
        continue;
    double* replication=new double[mNumCols];
    int index=0;
    
    for(int index=0; index<mNumCols; index++)
    {
      replication[index]=atof(token);
      token=strtok(NULL, ", \t\n");
    }
    replications->push_back(replication);
  }


  //store the data into a 2D array
  mReplications=new double *[replications->size()];
  for(int i=0; i<replications->size(); i++)
  {
     mReplications[i]=(*replications)[i];
  }
  mNumReplicates=replications->size();
  delete replications;
}

int
ReplicationParser::numCols()
{
   return mNumCols;
}
int 
ReplicationParser::numReplications()
{
   return mNumReplicates;
}
double**
ReplicationParser::replications()
{
  return mReplications;
}
