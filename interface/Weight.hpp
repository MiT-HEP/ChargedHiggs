#ifndef WEIGHT_H
#define WEIGHT_H

#include "interface/SF.hpp"
#include "interface/SF_CSVReweight.hpp"
#include "interface/MC.hpp"
#include "interface/PU.hpp"
#include "interface/Smearable.hpp"

class SmearBase;
class Event;

class Weight : virtual public SmearableBase {
    enum L1PREFIRING{L1Nom=0,L1Down=1,L1Up=2};
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
    bool aqgcs_{false};
    double pdfsWeights_[MC_MAX_PDFS];
    double pdfsNeventReweight_[MC_MAX_PDFS];
    map<string,double> aqgcWeights_;

    // syst here will have the values of MC::SCALES
    int systPdf {-1}; // 0 ... MAX_MC_MAX_PDFS
    string systAQGC {""};

    double l1prefiring_[3] {1.,1.,1.}; // NOM, down, up // L1PREFIRING
    bool l1_ {false};
    int systL1{0}; // 0: nom, -1 down, 1 up
    MC* currentMC_{nullptr};

    protected:


    map<string, MC*> mc_db;
    map<string, SF*> sf_db;


    PU pu_; // handler for PU
    public:
    Weight(){ clear(); }
    ~Weight(){}

    void PrintInfo();
    //const string name() const override{ return "Weight";};

    void SetMcWeight(double w);
    void SetScaleWeight(double w, MC::SCALES pos);
    void SetPdfWeight(double w, unsigned pos);
    void SetAQGCWeight(double w, const string & name);
    inline void SetSyst( MC::SCALES val) { syst = val;}
    inline void SetSystPdf( int val=-1) { systPdf = val;}
    inline void SetSystAQGC( const string& val="") { systAQGC = val;}
    inline bool HasAQGC() const { return aqgcs_;}

    void SetL1Prefiring(double x, int sys=0){ l1prefiring_[(sys==-1)?L1Down:((sys==1)?L1Up:L1Nom)] =x; l1_=true;};

    void clear(){ mcName_= "";
        mcXsec_ = 1.0; 
        mcWeight_=1.0; 
        nEvents_=1000. ; 
        lumi_=1.0;  // for RD MC this number should be the sum of the partial in the PUReweight
        sf_ = 1.0;
        syst = MC::none;
        systPdf = -1;
        l1_=false;
        l1prefiring_[L1Nom]=1.;
        l1prefiring_[L1Down]=1.;
        l1prefiring_[L1Up]=1.;
        systL1=0;
        aqgcWeights_.clear();
        systAQGC = "";
        scales_=false;
        pdfs_=false;
        aqgcs_=false;
    }
    //
    string GetMC(){ return mcName_; }
    // ---
    void SetLumi(double l) {lumi_= l;}
    void AddMC( string label, string dir, double xsec, double nevents);
    inline void AddMCScale( string label , MC::SCALES x, double rw) { mc_db[label]->scalesNeventsReweight[x] = rw; }
    inline void AddMCPdf( string label , unsigned x, double rw) { mc_db[label]->pdfsNeventsReweight[x] = rw; }
    inline void AddMCAQGC(string label, const string& name, double rw){mc_db[label]->aQGCNeventsReweight[name]=rw; }
    string LoadMC( string label) ;	 // return "" if failed otherwise dir
    string LoadMCbyDir( string dir ) ;	 // return "" if failed otherwise label

    SF* GetSF(string label){ if (sf_db.find(label) == sf_db.end() ) return NULL; return sf_db[label]; } // avoid to create label if not there
    void AddSF( string label, double sf, double err);
    void AddWG1SF( string label);
    void AddNNLOPSSF( string label, string filename); 
    void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err);
    void AddPtEtaEff( string label, double pt1,double pt2 ,double eta1 ,double eta2,double dataEff, double mcEff,double dataErr,double mcErr);
    void AddTh2fSF(string label, string filename);
    void AddTh2fSF(string label, string filename,string effData, string effMc, string errData, string errMc);
    void AddSplineSF(string label, double pt, double sf, double err);
    void AddCSVSF(string label, string filename);
    void AddCSVReweightSF(string label);

    void AddTF1SF(string label, string formula,string errFormula);
    void AddTF2SF(string label, string formula,string errFormula);

    //Log(__FUNCTION__,"DEBUG", string("clear SF of:") + s.first);
    void clearSF( );

    void clearL1(){
        l1_=false;
        l1prefiring_[L1Nom]=1.;
        l1prefiring_[L1Down]=1.;
        l1prefiring_[L1Up]=1.;
        systL1=0;
    }
    void clearPU( ){ pu_ . clearTarget() ;};
    void SetPUTarget(const string& name) { pu_ . SetTarget( name ) ; } 
    void SetSystSF(const string& label, int s );// { sf_db[label] -> syst = s;}
    void SetSystL1(int sys){systL1=sys;}
    void resetSystSF( ) ;
    void SetPtEtaSF(const string& label,double pt , double eta);
    void SetWPSF(const string& label, int wp);
    void SetJetFlavorSF(const string& label, int flavor);
    void SetDiscrSF( const string& label, float discr);
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
        double l1=1.; if (l1_) l1=l1prefiring_[(systL1==0)?L1Nom:(systL1<0)?L1Down:L1Up];
        if (syst == MC::none and systPdf <0 and systAQGC=="")
        { // NOMINAL
            return l1*mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }
        else if (syst!=MC::none) 
        { // SCALES
            if (not scales_) {Log(__FUNCTION__,"ERROR","Scales reweighting uncorrect!!!");PrintInfo();}
            return l1*scalesWeights_[syst] * scalesNeventReweight_[syst] * mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }
        else if (systPdf>=0) 
        { // PDFS
            if (not pdfs_) {Log(__FUNCTION__,"ERROR","Pdfs reweighting uncorrect!!!"); PrintInfo();}
            return l1* pdfsWeights_[systPdf] * pdfsNeventReweight_[systPdf] * mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        }
        else if (systAQGC!="") 
        { // AQGC
            if (not aqgcs_) {Log(__FUNCTION__,"ERROR","AQGCs reweighting uncorrect!!!"); PrintInfo();}
            if (currentMC_==nullptr) return 1; // data?
            auto rwit = currentMC_->aQGCNeventsReweight.find(systAQGC);
            if (rwit == currentMC_->aQGCNeventsReweight.end()) return 0.;
            auto wit = aqgcWeights_.find(systAQGC);
            if (wit == aqgcWeights_.end()) return 0.;
            return l1*(wit->second) *(rwit->second) * mcXsec_*lumi_*sf_*pu_.GetPUWeight(mcName_,puInt_,runNum_)/nEvents_;
        }
    }
    double weight(){
        double R=doWeight();
        if (R==0)PrintInfo();
        return R;
        }

    double weight_aqgc(const string&name){
        //TODO
        if (systPdf >=0 or syst!=MC::none) return 0; // possible? do we need extras?
        systAQGC=name;
        double R=doWeight();
        systAQGC="";
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
