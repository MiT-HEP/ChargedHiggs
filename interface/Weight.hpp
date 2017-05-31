#ifndef WEIGHT_H
#define WEIGHT_H

#include "interface/SF.hpp"
#include "interface/MC.hpp"
#include "interface/PU.hpp"
#include "interface/Smearable.hpp"

class SmearBase;
class Event;

class Weight : virtual public SmearableBase {
    friend class Event;
    friend class SmearBase;

    string mcName_;
    double mcXsec_;
    double nEvents_;
    double lumi_;
    double sf_;
    double mcWeight_; // this is set in FillEvent

    double scalesWeights_[MC_MAX_SCALES];
    double scalesNeventReweight_[MC_MAX_SCALES];
    bool scales_{false};
    bool pdfs_{false};
    double pdfsWeights_[MC_MAX_PDFS];
    double pdfsNeventReweight_[MC_MAX_PDFS];

    // syst here will have the values of MC::SCALES
    int systPdf {-1}; // 0 ... MAX_MC_MAX_PDFS


    protected:


    map<string, MC*> mc_db;
    map<string, SF*> sf_db;


    PU pu_; // handler for PU
    public:
    Weight(){ clear(); }
    ~Weight(){}

    void PrintInfo();

    inline void SetMcWeight(double w){mcWeight_= w; scales_=false; pdfs_=false;}
    inline void SetScaleWeight(double w, MC::SCALES pos){scalesWeights_[pos]=w; scales_=true;}
    inline void SetPdfWeight(double w, unsigned pos){pdfsWeights_[pos]=w; pdfs_=true;}
    inline void SetSyst( MC::SCALES val) { syst = val;}
    inline void SetSystPdf( int val=-1) { systPdf = val;}

    void clear(){ mcName_= "";
        mcXsec_ = 1.0; 
        mcWeight_=1.0; 
        nEvents_=1000. ; 
        lumi_=1.0;  // for RD MC this number should be the sum of the partial in the PUReweight
        sf_ = 1.0;
        syst = MC::none;
        systPdf = -1;
    }
    //
    string GetMC(){ return mcName_; }
    // ---
    void SetLumi(double l) {lumi_= l;}
    void AddMC( string label, string dir, double xsec, double nevents);
    inline void AddMCScale( string label , MC::SCALES x, double rw) { mc_db[label]->scalesNeventsReweight[x] = rw; }
    inline void AddMCPdf( string label , unsigned x, double rw) { mc_db[label]->pdfsNeventsReweight[x] = rw; }
    string LoadMC( string label) ;	 // return "" if failed otherwise dir
    string LoadMCbyDir( string dir ) ;	 // return "" if failed otherwise label

    SF* GetSF(string label){ if (sf_db.find(label) == sf_db.end() ) return NULL; return sf_db[label]; } // avoid to create label if not there
    void AddSF( string label, double sf, double err);
    void AddWG1SF( string label);
    void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err);
    void AddTh2fSF(string label, string filename);
    void AddTh2fSF(string label, string filename,string effData, string effMc, string errData, string errMc);
    void AddSplineSF(string label, double pt, double sf, double err);
    void AddCSVSF(string label, string filename);
    void AddCSVReweightSF(string label);

    void AddTF1SF(string label, string formula,string errFormula);
    void AddTF2SF(string label, string formula,string errFormula);

    void clearSF( ){ sf_ =1.0; for(auto& s :sf_db) s.second->clearEvent(); }
    void clearPU( ){ pu_ . clearTarget() ;};
    void SetPUTarget(const string& name) { pu_ . SetTarget( name ) ; } 
    void SetSystSF(const string& label, int s );// { sf_db[label] -> syst = s;}
    void resetSystSF( ) ;
    void SetPtEtaSF(const string& label,double pt , double eta);
    void SetWPSF(const string& label, int wp);
    void SetJetFlavorSF(const string& label, int flavor);
    void ApplySF(const string& label);
    inline bool ExistSF(const string& label){ if (sf_db.find(label) != sf_db.end() ) return true; else return false; }

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
    inline int GetPU() const { return puInt_ ; } 

    // SYNC -- WARNING, NOT implemented the Theory Unc in these function. Use for sync purposes
    inline double GetBareMCWeight() const { return mcWeight_ ;} // internal mcWeight
    inline double GetBarePUWeight()  { return pu_.GetPUWeight(mcName_,puInt_,runNum_);} // not const because pu is caching
    inline double GetBareMCXsec() const { return mcXsec_;}
    inline double GetBareNevents() const{ return nEvents_;}
    inline double GetBareSF() const {return sf_;}
    inline double GetBareLumi() const { return lumi_;}
    // --------

    // ---  check what happen with data, TODO CHECK LUMI
    inline double doWeight() { 
        //Log(__FUNCTION__,"DEBUG",Form("Weight: Mc=%lf mcXsec=%lf sf=%lf pu=%lf nevents=%lf",mcWeight_, mcXsec_ ,sf_,pu_.GetPUWeight(mcName_,puInt_,runNum_), nEvents_));
        if (syst == MC::none and systPdf <0)
        return mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        else if (systPdf<0) 
        { // SCALES
        if (not scales_) {Log(__FUNCTION__,"ERROR","Scales reweighting uncorrect!!!");PrintInfo();}
        return scalesWeights_[syst] * scalesNeventReweight_[syst] * mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }
        else { // pdfs
        if (not pdfs_) {Log(__FUNCTION__,"ERROR","Pdfs reweighting uncorrect!!!"); PrintInfo();}
        return pdfsWeights_[systPdf] * pdfsNeventReweight_[systPdf] * mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }
    }
    double weight(){
        double R=doWeight();
        if (R==0)PrintInfo();
        return R;
        }

    // 
	void Log(const string& function, const string& level, const string& message) const;
};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
