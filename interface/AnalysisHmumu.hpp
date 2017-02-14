#ifndef ANALYSIS_Hmumu_H
#define ANALYSIS_Hmumu_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class HmumuAnalysis: virtual public AnalysisBase
{
    public:
        HmumuAnalysis() : AnalysisBase () {}
        virtual ~HmumuAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override {return "HmumuAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        inline void SetJetCuts(Jet *j) override { j->SetBCut(0.460); j->SetEtaCut(4.7); j->SetEtaCutCentral(2.5); j->SetPtCut(30); j->SetPuIdCut(-100);}

    private:
        CutSelector cut;

        enum CutFlow{ Total=0, 
            Leptons,
            Trigger,
            Mass,
            MaxCut // do not remove
        };
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
