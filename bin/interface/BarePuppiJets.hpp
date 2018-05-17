#ifndef BARE_PUPPIJETS_H
#define BARE_PUPPIJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BarePuppiJets : virtual public BareP4
{
    public:
        enum Selection{
            // 0--7 POG
            JetBaseline = 1UL << 0,
            JetLoose  = 1UL << 3, 
            JetMedium = 1UL << 4,
            JetTight  = 1UL << 5,
            // 0--7 POG
            mjId = 1UL<< 8,
            mjIdLoose = 1UL << 9,
            mjId2015 = 1UL << 10
        };

        std::string cachedPrefix="";

        BarePuppiJets();
        ~BarePuppiJets();
        void init() override;
        void clear() override;
        void defineBranches(TTree* t) override; 

        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;

        inline string name() override { return "BarePuppiJets"; }

        // variables --
        vector<float> *rawPt{0};
        vector<float> *bDiscr{0};
        vector<unsigned> *selBits{0};
        vector<float> *Q{0};
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
