#ifndef ANALYSIS_BASE_H
#define ANALYSIS_BASE_H

#define EVENT_USED 1
#define EVENT_NOT_USED 0


#include "interface/Event.hpp"
#include "interface/Output.hpp"

class AnalysisBase
{
    private:
    Output *output_; // set automatically

    protected:
    // --- this are the default values. override these functions
    virtual inline void SetLeptonCuts(Lepton *l){ l->SetIsoCut(10); l->SetPtCut(15);l->SetIsoRelCut(-1);l->SetEtaCut(2.4);}
    virtual inline void SetPhotonCuts(Photon *p){p->SetIsoCut(-1); p->SetPtCut(30);}
    virtual inline void SetTauCuts(Tau *t){ t->SetIsoCut(1.5); t->SetEtaCut(2.1); t->SetPtCut(20); t->SetMuRej(true); t->SetEleRej(true);}
    virtual inline void SetJetCuts(Jet *j){j->SetBCut(0.5);j->SetEtaCut(4.7); j->SetEtaCutCentral(2.4);j->SetPtCut(30);};
    virtual void SetGenCuts(GenParticle *g){};
    
    void SetCuts(Event *e);

    public:
    AnalysisBase(){ labels.push_back("Data"); labels.push_back("Other");};
    virtual ~AnalysisBase(){};
    //--
    void doInit() {Init();}
    void doEnd() { End();} 
    int doAnalyze(Event*e,string systname){ SetCuts(e); return analyze(e,systname);}
    // 
    virtual void inline SetOutput( Output *o ) { output_ = o ;}
    virtual int analyze(Event*,string systname){return EVENT_NOT_USED;}
    virtual void Init(){}
    virtual void End(){} // before closing files and writing
    virtual const string name(){return "AnalysisBase";}

    // call output_->Book, but add something to name
    void Book(string name, string title,int nBins, double xmin, double xmax);
    void Book2D(string name, string title,int nBins, double xmin, double xmax,int nBins2,double ymin,double ymax);
    void Book(string name, string title,int nBins, double *xbound);
    void Fill(string name, string syst , double value, double weight=1);
    void Fill2D(string name, string syst , double valueX,double valueY, double weight=1);
    TH1D* GetHisto(string name, string systname);
    TH2D* GetHisto2D(string name, string systname);

    vector<string> labels;
    inline vector<string>& AllLabel(){return labels;}
    string GetLabel(Event *e);
    inline void AddLabel(string s) {labels.push_back(s);}
    inline void AddLabels(vector<string> &v) { for(string &s : v ) labels.push_back(s);}

    inline TFile* GetOutputFile(){ return output_->GetFile() ;} // TMVA wants the file pointer
    // Tree Operations ---- 
    inline void InitTree(string name){ output_->InitTree(name);}
    inline void Branch(string tree,string name,char type){ output_->Branch(tree,name,type);}
    template<class T>
    inline void SetTreeVar(string name, T value) { output_->SetTreeVar(name,value);}
    inline void PrintTreeVar(){output_->PrintTreeVar() ;}
    inline void FillTree(string tree){output_->FillTree(tree);}
    inline void PrintTree(string tree){output_->PrintTree(tree);}
    inline TTree* GetTree(string tree){ return output_->GetTree(tree);}
    bool unblind{false}; // use this flag to protect histograms or region that should be blind
    bool Unblind(Event *e) {if (e->IsRealData() ) return unblind; return true;} // if is not data, no need to return something else
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
