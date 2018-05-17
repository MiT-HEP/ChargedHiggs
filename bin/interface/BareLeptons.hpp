#ifndef BARE_LEPTONS_H
#define BARE_LEPTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareLeptons : virtual public BareP4
{
    public:
        enum Selection {
          LepBaseline = 1UL<<0,
          LepVeto     = 1UL<<1,
          LepFake     = 1UL<<2,
          LepSoft     = 1UL<<3,
          LepLoose    = 1UL<<4,
          LepMedium   = 1UL<<5,
          LepTight    = 1UL<<6,
          // 1 -- 6 POG
          LepEBEE     = 1UL<<7, // lepton (electron) is not in the EB-EE crack
          LepMediumIP = 1UL<<8,
          LepTightIP  = 1UL<<9,
          LepSoftIP   = 1UL<<10,
          MuStandalone = 1UL<<11,
          MuGlobal     = 1UL<<12,
          MuTracker    = 1UL<<13,
          MuMediumB2F    = 1UL<<14,
          EleTripleCharge = 1UL<<15, // charge is in agreement between different measurements
          EleNoMissingHits = 1UL<<16,
          LepIP    = 1UL<<17,
          LepHighPt=1UL<<18
        };

        BareLeptons();
        ~BareLeptons();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BareLeptons"; }

        bool inline passSelection(const unsigned &idx, const Selection &sel) const  { return (selBits->at(idx) & sel) != 0; }

        // ----
        vector<int>      *pdgId{0};	
        vector<float>    *iso{0};
        vector<unsigned> *selBits{0};
        vector<float>    *lepPfPt{0};

        // for electrons
        vector<float>    *etaSC{0}; 
        vector<float>    *sieie{0}; 
        vector<float>    *sipip{0};
        vector<float>    *sieip{0};
        vector<float>    *r9{0};   

        vector<float>    *mva{0};
        vector<float>    *chIso{0};
        vector<float>    *nhIso{0};
        vector<float>    *phoIso{0};
        vector<float>    *puIso{0};

        //add miniIsolation
        vector<float>    *miniIso{0};

        // muon
        vector<float>    *resolution{0};
        vector<int >     *nLayers{0};

        // ele scale and smear corrections
        vector<float>    *corr{0};
    

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
