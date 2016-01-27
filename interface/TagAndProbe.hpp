#ifndef TAG_AND_PROBE_H
#define TAG_AND_PROBE_H

#include "interface/AnalysisBase.hpp"
#include "TChain.h"
#include "TTree.h"

class TagAndProbe : public AnalysisBase {

public:

        void Init() override;
        int analyze(Event*,string systname) override;
        const string name()override{return "TagAndProbe";}

        // loose tau selection
        void SetTauCuts(Tau*t) override;

        bool doTree = true;
        string treename ="tagprobe";

private:

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 

