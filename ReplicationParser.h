#ifndef _REPLICATION_PARSER_H_
#define _REPLICATION_PARSER_H_
#include <vector>
#include <string>
using namespace std;


class ReplicationParser
{

      vector<string >* mRowNames;
      vector<string> * mColNames; 


      void readFile(const char* file);
    public:
      double** mReplications;
      int mNumReplicates;
      int mNumPoints;

      ReplicationParser();
      ReplicationParser(const char* filename);
      ~ReplicationParser();
      void show(int i, int j);
      int numPoints();
      int numReplications();
      double** replications();
      double value(int r, int c) { return mReplications[r][c] ;}
      double* replication(int r) { return mReplications[r];}
};

#endif
