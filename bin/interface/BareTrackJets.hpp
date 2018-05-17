#ifndef BARE_TRACK_JETS_H
#define BARE_TRACK_JETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"
#include <map>

class BareTrackJets : virtual public BareP4
{
    public:


        BareTrackJets();
        ~BareTrackJets();
        void init() override;
        void clear() override;
        void defineBranches(TTree* t) override;
        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;
        inline string name() override { return "BareTrackJets"; }

        // -- variables
        //TClonesArray  *p4{0};
        
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
