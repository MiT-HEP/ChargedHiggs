#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"
#include "interface/GenParticle.hpp"

class Tau: virtual public Object,
    virtual public Lepton
{
    float idcut_;

    bool match_ ; // is matched with a gen tau
    bool run_matching_; // run the algorithm.

    public:
    Tau() : Lepton() { idcut_ = 0.5;match_ = false; run_matching_= false;}
    float id;
    virtual int IsTau() ;
    virtual inline int IsObject(){ return IsTau(); }

    virtual int IsMatch( ) { if (not run_matching_ ) return -1; if (match_) return 1; else return 0;}

    virtual void clearSyst(){
        Lepton::clearSyst(); 
        Object::clearSyst();
        run_matching_= false;
        match_=false; }

    inline virtual void SetMatch(bool m=true){match_=m;}
    inline virtual void SetRunMatching(bool m=true){run_matching_=m;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
