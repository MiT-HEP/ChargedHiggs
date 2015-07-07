#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"
#include "interface/GenParticle.hpp"

class Tau: virtual public Object,
    virtual public Lepton
{
    float idcut_;


    public:
    Tau() : Lepton() { idcut_ = 0.5; match = -999; ; isocut_=100; iso2 = -999; id_ele= -1; id_mu=-1;}
    float id;
    float iso2;
    int id_ele;
    int id_mu;
    int match ; // is matched with a gen tau

    virtual int IsTau() ;
    virtual inline int IsObject(){ return IsTau(); }

    virtual bool IsMatch( ) { if (match >= 0) return true; else return false;}

    virtual void clearSyst(){
        Lepton::clearSyst(); 
        Object::clearSyst();
        }

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
