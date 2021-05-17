#ifndef ANALYSIS_DY_H
#define ANALYSIS_DY_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class DYAnalysis: virtual public AnalysisBase
{
    public:
        DYAnalysis() : AnalysisBase () {}
        virtual ~DYAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        int analyzeMM(Event*,string systname);
        //int analyzeEE(Event*,string systname);
        //int analyzeEM(Event*,string systname);
        //int analyzeLLL(Event*,string systname);
        const string name() const override {return "DYAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet *j) override ; 
        void SetFatJetCuts(FatJet *f) override;


        int year=2016; // master switch for year configuration
    private:

        void ApplyMuonSF(Event*,string);
        Lepton*mu0 {NULL};
        Lepton*mu1 {NULL};

        Jet *j0 {NULL};
        Jet *j1 {NULL};
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
