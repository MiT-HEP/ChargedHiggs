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
            Met ,
            AngColl ,
            AngRbb ,
            MaxCut
            };
        // for the n-1 plots
        CutSelector cut;

        virtual void Init();
        virtual int analyze(Event*,string systname);
        virtual const string name(){return "ChargedHiggsTauNu";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
