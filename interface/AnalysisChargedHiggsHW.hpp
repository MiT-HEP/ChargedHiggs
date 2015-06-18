#ifndef CHARGEDHIGGSHW_H
#define CHARGEDHIGGSHW_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsHW:  virtual public AnalysisBase
{
    public:
        virtual void Init();
        virtual int analyze(Event*,string systname);
        virtual const string name(){return "ChargedHiggsHW";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
