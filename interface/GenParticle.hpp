#ifndef GEN_PARTICLE_H
#define GEN_PARTICLE_H

#include "interface/Object.hpp"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"

class GenParticle : virtual public Object
{
    protected:
        int pdgid_;
        unsigned flags_; // from Nero
    public:

        int GetPdgId() const { return pdgid_;}
        void SetPdgId(int pdg){pdgid_= pdg;}
        unsigned GetFlag() const { return flags_;}
        void SetFlags(unsigned x) { flags_=x;}
        bool IsPromptFinalState() const { return flags_ & BareMonteCarlo::PromptFinalState ;}
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
