#ifndef CHARGEDHIGGSTAUNU_H
#define CHARGEDHIGGSTAUNU_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class ChargedHiggsTauNu:  virtual public AnalysisBase
{
    public:
        enum CutFlow{ Total=0, 
            OneTau ,
            NoLep ,
            ThreeJets ,
            OneBjet ,
            Trigger ,
            Met ,
            AngColl ,
            AngRbb ,
            MaxCut
            };
        // for the n-1 plots
        CutSelector cut;

        void Init() override;
        int analyze(Event*,string systname) override;
        // *return the cut flow
        //  direct tau or inv tau
        static unsigned Selection(Event*, bool direct=true) ;
        const string name() override {return "ChargedHiggsTauNu";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
