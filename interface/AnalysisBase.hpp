#ifndef ANALYSIS_BASE_H
#define ANALYSIS_BASE_H

#define EVENT_USED 1
#define EVENT_NOT_USED 0


#include "interface/Event.hpp"
#include "interface/Output.hpp"

class AnalysisBase
{
    Output *output_;
    public:
    AnalysisBase(){ labels.push_back("Data"); labels.push_back("Other");};
    virtual ~AnalysisBase(){};
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
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
