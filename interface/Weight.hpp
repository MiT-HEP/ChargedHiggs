#ifndef WEIGHT_H
#define WEIGHT_H

#include "interface/SF.hpp"
#include "interface/MC.hpp"
#include "interface/PU.hpp"

class SmearBase;
class Event;

class Weight{
    friend class Event;
    friend class SmearBase;

    string mcName_;
    double mcXsec_;
    double nEvents_;
    double lumi_;
    double sf_;
    protected:

    map<string, MC*> mc_db;
    map<string, SF*> sf_db;

    PU pu_; // handler for PU
    public:
    Weight(){ clear(); }
    ~Weight(){}

    double mcWeight_; // this is set in FillEvent

    void clear(){ mcName_= "";
        mcXsec_ = 1.0; 
        mcWeight_=1.0; 
        nEvents_=1000. ; 
        lumi_=1.0;  // for RD MC this number should be the sum of the partial in the PUReweight
        sf_ = 1.0;
    }
    //
    string GetMC(){ return mcName_; }
    // ---
    void SetLumi(double l) {lumi_= l;}
    void AddMC( string label, string dir, double xsec, double nevents);
    string LoadMC( string label) ;	 // return "" if failed otherwise dir
    string LoadMCbyDir( string dir ) ;	 // return "" if failed otherwise label

    void AddSF( string label, double sf, double err);
    void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err);
    void AddSplineSF(string label, double pt, double sf, double err);

    void clearSF( ){ sf_ =1.0;}
    void SetSystSF(string label, int s ) { sf_db[label] -> syst = s;}
    void resetSystSF( ) ;
    void SetPtEtaSF(string label,double pt , double eta);
    void ApplySF(string label){ sf_ *= sf_db[label] -> get(); }
    inline bool ExistSF(string label){ if (sf_db.find(label) != sf_db.end() ) return true; else return false; }

    // --- PU Reweight
    inline void AddTarget( TH1*h, int runMin=-1, int runMax =-1,double lumi=-1)
    { pu_ . AddTarget( h, runMin,runMax,lumi); }
    inline void AddTarget( TH1*h, string systName, int runMin=-1, int runMax =-1,double lumi=-1)
    { pu_ . AddTarget( h, systName,runMin,runMax,lumi); }
    inline void AddMC( string label, TH1*h, int runMin=-1, int runMax =-1){ pu_. AddMC(  label, h, runMin, runMax ); }


    float puInt_;	
    int runNum_;
    inline void SetPU(float pu, int run){puInt_ = pu; runNum_=run; }
    inline void SetSystPU(int syst){pu_.syst=syst;};
    inline void clearSystPU(){ pu_ .clearSyst();}

    // ---  check what happen with data, TODO CHECK LUMI
    double weight(){ 
        //Log(__FUNCTION__,"DEBUG",Form("Weight: Mc=%lf mcXsec=%lf sf=%lf pu=%lf nevents=%lf",mcWeight_, mcXsec_ ,sf_,pu_.GetPUWeight(mcName_,puInt_,runNum_), nEvents_));
        return mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }

    // 
	void Log(const string& function, const string& level, const string& message);
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
