#ifndef GEN_PARTICLE_H
#define GEN_PARTICLE_H

#include "interface/Object.hpp"
class GenParticle : virtual public Object
{
    protected:
    public:
        int pdgid_;
        int GetPdgId(){ return pdgid_;}
        void SetPdgId(int pdg){pdgid_= pdg;}
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
