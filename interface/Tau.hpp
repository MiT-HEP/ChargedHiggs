#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"

class Tau: virtual public Object,
    virtual public Lepton
{
    float idcut_;
    public:
    Tau() : Lepton() { idcut_ = 0.5;}
    float id;
    virtual int IsTau() ;
    virtual inline int IsObject(){ return IsTau(); }
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
