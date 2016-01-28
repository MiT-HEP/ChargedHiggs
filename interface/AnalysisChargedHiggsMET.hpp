#ifndef CHARGEDHIGGSMET_H
#define CHARGEDHIGGSMET_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsMET:  virtual public AnalysisBase
{
    public:
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() override {return "ChargedHiggsMET";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
