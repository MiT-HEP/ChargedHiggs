#ifndef ANALYSIS_GG_H
#define ANALYSIS_GG_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class GGAnalysis: virtual public AnalysisBase
{
    public:
        GGAnalysis() : AnalysisBase () {}
        virtual ~GGAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() override {return "GGAnalysis";}

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
