#ifndef ANALYSIS_BASE_H
#define ANALYSIS_BASE_H


#include "interface/Event.hpp"
#include "interface/Output.hpp"

class AnalysisBase
{
    Output *output_;
    public:
    AnalysisBase(){ labels.push_back("Data"); labels.push_back("Other");};
    ~AnalysisBase(){};
    // 
    virtual void inline SetOutput( Output *o ) { output_ = o ;}
    virtual int analyze(Event*,string systname){return 0;}
    virtual void Init(){}
    virtual const string name(){return "AnalysisBase";}

    // call output_->Book, but add something to name
    void Book(string name, string title,int nBins, double xmin, double xmax);
    void Book(string name, string title,int nBins, double *xbound);
    void Fill(string name, string syst , double value, double weight=1);
    TH1D* GetHisto(string name, string systname);

    vector<string> labels;
    inline vector<string>& AllLabel(){return labels;}
    string GetLabel(Event *e);
    inline void AddLabel(string s) {labels.push_back(s);}
    inline void AddLabels(vector<string> &v) { for(string &s : v ) labels.push_back(s);}
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
