
#ifndef CHARGEDHIGGS_EWKPURITY_H
#define CHARGEDHIGGS_EWKPURITY_H

#include "interface/AnalysisBase.hpp"
#include "interface/GeneralFunctions.hpp"

class ChargedHiggsEWKPurity:  virtual public AnalysisBase
{
    public:
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override{return "ChargedHiggsEWKPurity";}
        const string dir="ChargedHiggsEWKPurity/Vars/";
        const string none="ChargedHiggsEWKPurity/NOne/";
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
