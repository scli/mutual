#ifndef _DI_GAMMA_
#define _DI_GAMMA_
class DiGamma
{
   
    static double gamma;
    static double * harmonicPreSum;
    static int      harmonicArraySize;
    static void   calculateHarmonicPreSum(int n);
   
     public: 
     DiGamma() {}
     ~DiGamma() {}
     static double digamma(int n);

};
#endif
