#ifndef PU_H
#define PU_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

#include "TH1D.h"
#include "TH1F.h"

class PUunit
{
    // base class to be able to perform PU Reweighting
    // take control of the histograms
    public:
        PUunit();
        ~PUunit();
        int runMin;
        int runMax;
        TH1* hist;
        double lumi; // run dependent, only for target
};

// the structure is
// map<string,PU*>
// target/data -> PU
//
//
class PU
{
    double ltot ; // total luminosity seen
    map< string , vector< PUunit* >* > container;
    // mc, target, run
    map< string , vector<double>* > norm;

    void AddToContainer( string label, TH1*, int runMin, int runMax,double lumi=-1);
    bool IsInRange(int run, int runMin,int runMax);

    // --- limit max 
    inline double Ratio(double num, double den){ 
            if (std::isnan(num) or std::isinf(den)) return 0  ;
            if (std::isnan(den) or std::isinf(num)) return 10;
            if (den <=0 ) return 0 ; 
            if (num>10*den) return 10; 
            return num/den;
            };

    public:
    PU();
    int syst;

    void clear();
    void clearSyst(){ syst=0;}
    void AddTarget( TH1*, int runMin=-1, int runMax =-1,double lumi=-1);
    void AddTarget( TH1*, string systName="Up",int runMin=-1, int runMax =-1,double lumi=-1);
    void AddMC( string label, TH1*, int runMin=-1, int runMax =-1);

    // 
    double GetPUWeight(string label, float x, int run = -1);
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
