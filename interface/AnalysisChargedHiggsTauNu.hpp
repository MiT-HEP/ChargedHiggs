#ifndef CHARGEDHIGGSTAUNU_H
#define CHARGEDHIGGSTAUNU_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsTauNu:  virtual public AnalysisBase
{
    public:
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
