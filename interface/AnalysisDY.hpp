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
        int analyzeEE(Event*,string systname);
        int analyzeEM(Event*,string systname);
        int analyzeLLL(Event*,string systname);
        const string name() override {return "DYAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 

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
