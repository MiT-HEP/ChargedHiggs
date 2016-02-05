#ifndef CHARGEDHIGGSZW_H
#define CHARGEDHIGGSZW_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsZW:  virtual public AnalysisBase
{
    public:
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override {return "ChargedHiggsZW";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
