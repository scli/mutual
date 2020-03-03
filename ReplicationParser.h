#ifndef _REPLICATION_PARSER_H_
#define _REPLICATION_PARSER_H_
#include <vector>
#include <string>
using namespace std;


class ReplicationParser
{

      vector<string >* mRowNames;
      vector<string> * mColNames; 

      int mNumReplicates;
      int mNumCols; 
      double** mReplications;

      void readFile(const char* file);
    public:
      ReplicationParser(const char* filename);
      ~ReplicationParser();
      int numCols();
      int numReplications();
      double** replications();
      double value(int r, int c) { return mReplications[r][c] ;}
      double* replication(int r) { return mReplications[r];}
};

#endif
