#include <string.h>
#include <stdio.h>     /* for printf */
#include <stdlib.h>
#include <iostream>
#include <getopt.h>    /* for getopt_long; POSIX standard getopt is in unistd.h */
#include <string>
#include <string.h>

#ifdef _PYMODULE_
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
namespace py = pybind11;
#endif

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

#ifndef _PYMODULE_

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
        mi->estimateMI14(atoi(mik.c_str()), mioutput.c_str());//the 2014 plos one paper. 
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

#else /* _PYMODULE_ defined */

int k = 3, algo = 0;
int setk(int x) { k = x; return k; }
int setalgo(int a) { algo = a; return algo; }
//double mi(py::array_t<double> x)
double mi(py::array_t<double> x, py::array_t<double> y)
{
    py::buffer_info buf1 = x.request(), buf2 = y.request();
    double *ptr1 = (double *) buf1.ptr, *ptr2 = (double *) buf2.ptr;

    int numRows = buf1.shape[0];
    int numCols = 2;
    ReplicationParser* rp = new ReplicationParser();
    rp->mReplications = new double *[numCols];
    rp->mReplications[0] = new double[numRows];
    rp->mReplications[1] = new double[numRows];
    for (int j=0; j < numRows; j++)
    {
        double xval = ptr1[j];
        double yval = ptr2[j];
        rp->mReplications[0][j] = xval;
        rp->mReplications[1][j] = yval;
    }
    cout << "x vals: ";
    for (int j=0; j < numRows; j++)
        cout << rp->mReplications[0][j] << ", ";
    cout << endl;
    cout << "y vals: ";
    for (int j=0; j < numRows; j++)
        cout << rp->mReplications[1][j] << ", ";
    cout << endl;

    rp->mNumReplicates = numCols;
    rp->mNumPoints = numRows;

    MutualInfo* mut = new MutualInfo(rp);
    double ret;
    if (algo == 0)
    {
        cout << "algo=2004, k=" << k << endl;
        ret = mut->estimateMI04(k, mioutput.c_str());
    }
    else
    {
        cout << "algo=2014, k=" << k << endl;
        ret = mut->estimateMI14(k, mioutput.c_str());
    }
    return ret;
}

PYBIND11_MODULE(mi, m){
    m.doc() = "MI";
    m.def("mi", &mi, "Compute MI");
    m.def("setk", &setk, "Set k");
    m.def("setalgo", &setalgo, "Set algo");
}
#endif
