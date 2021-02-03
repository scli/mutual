#include <string.h>
#include <stdio.h>     /* for printf */
#include <stdlib.h>
#include <iostream>
#include <getopt.h>    /* for getopt_long; POSIX standard getopt is in unistd.h */
#include <string>
#include <string.h>

using namespace std;
#include "ReplicationParser.h"
#include "MI.h"

void printMIHelp(char** argv)
{
    std::cout<<argv[0]<<endl;	
    std::cout <<
            "--input      <filename> :   input file\n"
            "--output     [filename] :   output file name, optional, default to screen.\n"
            "--k          [k, optional]: a positive integer, default value is 3\n"
            "--algo       [algo, c2c or d2c, default is c2c]\n"
            "--help:                     Show help\n";
    exit(1);
}

string  miinput     = "";
string  mioutput    = "";
string  mik         = "3";
string  mialgo      = "c2c";
int processMIArgs(int argc, char** argv)
{
    const char* const short_opts = "i:o:e:d:a:h";
    const option long_opts[] = {
            {"input",       required_argument,   nullptr, 'i'},
            {"output",      required_argument,   nullptr, 'o'},
            {"k",           required_argument,   nullptr, 'k'},
            {"algo",        required_argument,   nullptr, 'a'},
            {"help",        no_argument,         nullptr, 'h'},
            {nullptr,       no_argument,         nullptr,  0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
          case 'i':
            miinput  = optarg;
            std::cout << "input set to: " << miinput.c_str() << std::endl;
            break;

          case 'o':
            mioutput = optarg;
            std::cout << "output set to: " << mioutput.c_str() << std::endl;
            break;

          case 'k':
            mik    = optarg;
            std::cout << "k set to: " << mik.c_str() << std::endl;
            break;

          case  'a':
            mialgo = optarg;
            std::cout << "algo set to: " << mialgo.c_str() << std::endl;
            break; 
	  case 'h': // -h or --help
          case '?': // Unrecognized option
          default:
              printMIHelp(argv);
             break;
        }
    }//while
    return 0;
}


int 
main(int argc, char** argv)
{
  
   if(argc==1)
   {
      printMIHelp(argv);
	   exit(0);
   }
   processMIArgs(argc, argv);
   if(miinput.length()==0 || mik.length()==0)
   {
	  printMIHelp(argv);
	  exit(0);
   }
   ReplicationParser* replications=new ReplicationParser(miinput.c_str());
   //, atoi(mik.c_str()));

   MutualInfo* mi=new MutualInfo(replications);
   if(strcmp(mialgo.c_str(), "c2c")==0)
        mi->estimateMI04(atoi(mik.c_str()), mioutput.c_str());
   else if(strcmp(mialgo.c_str(), "d2c")==0)
        mi->estimateMI14(atoi(mik.c_str()), mioutput.c_str());
   else
   {  
        cerr<<"Uknown algorithm option "<<mialgo.c_str()<<endl;
   }
//   ReplicationParser* replications=new ReplicationParser(argv[1]);
   //, atoi(mik.c_str()));

//   MutualInfo* mi=new MutualInfo(replications);
//   cout<<mi->estimateMI04(0, 1, atoi(argv[2])) <<endl;;




   return 0;
}
