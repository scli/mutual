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
  int numCols = r1->size();
  double* firstRow=new double[numCols];
  for(int i=0; i < numCols; i++)
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
    double* row=new double[numCols];
    int index=0;
    
    for(int index=0; index<numCols; index++)
    {
      row[index]=atof(token);
      token=strtok(NULL, ", \t\n");
    }
    replications->push_back(row);
  }


  //store the data into a 2D array
  mReplications=new double *[numCols];
  for(int i=0; i<numCols; i++)
  {
     mReplications[i]=new double[replications->size()];
     for(int j=0; j< replications->size(); j++)
     {
         mReplications[i][j] =(*replications)[j][i];
     }

  }

  for(int i=0; i<replications->size(); i++)
     delete (*replications)[i];
   
  mNumReplicates=numCols;
  mNumPoints      =replications->size();
  
  delete replications;
}

int
ReplicationParser::numPoints()
{
   return mNumPoints;
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
