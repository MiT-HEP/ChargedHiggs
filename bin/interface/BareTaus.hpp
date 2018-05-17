#ifndef BARE_TAUS_H
#define BARE_TAUS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareTaus : virtual public BareP4
{
    public:
        enum Selection{
            TauBaseline = 1UL << 0,
            TauDecayModeFindingNewDMs = 1UL << 1,
            TauDecayModeFinding = 1UL << 2,

            // 0--7 POG
            AgainstEleTight = 1UL <<7,
            AgainstEleLoose = 1UL <<8,
            AgainstEleMedium = 1UL<<9,
            AgainstMuLoose = 1UL<<10,
            AgainstMuTight = 1UL<<11,

            //--- MVA ISO
            byVLooseIsolationMVArun2v1DBoldDMwLT = 1UL <<12,
            byLooseIsolationMVArun2v1DBoldDMwLT = 1UL <<13,
            byMediumIsolationMVArun2v1DBoldDMwLT= 1UL <<14,
            byTightIsolationMVArun2v1DBoldDMwLT = 1UL <<15,
            byVTightIsolationMVArun2v1DBoldDMwLT= 1UL <<16,

            byVLooseIsolationMVArun2v1DBnewDMwLT = 1UL <<17,
            byLooseIsolationMVArun2v1DBnewDMwLT = 1UL <<18,
            byMediumIsolationMVArun2v1DBnewDMwLT= 1UL <<19,
            byTightIsolationMVArun2v1DBnewDMwLT = 1UL <<20, 
            byVTightIsolationMVArun2v1DBnewDMwLT= 1UL <<21,

            // --- DB ISO  WP 
            byLooseCombinedIsolationDeltaBetaCorr3Hits  = 1UL <<22,
            byMediumCombinedIsolationDeltaBetaCorr3Hits  = 1UL <<23,
            byTightCombinedIsolationDeltaBetaCorr3Hits  = 1UL <<24,

            // NProng
            OneProng = 1UL << 25,
            TwoProng = 1UL << 26,
            ThreeProng = 1UL << 27,
            ZeroPiZero = 1UL << 28,
            OnePiZero = 1UL << 29,
            TwoPiZero = 1UL << 30,
            ThreePiZero = 1UL << 31,

            /// NOTHING ABOVE 31 !!!

        };

        BareTaus();
        ~BareTaus();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BareTaus"; }

        // --  members
        //TClonesArray    *p4{0};
        vector<int>     *Q{0}; // charge
        vector<float>   *M{0}; // mass
        vector<float>   *iso{0};
        vector<unsigned> *selBits{0};

        // EXTENDED VARIABLES
        vector<float>   *chargedIsoPtSum{0};
        vector<float>   *neutralIsoPtSum{0};
        vector<float>   *isoDeltaBetaCorr{0};
        vector<float>   *leadTrackPt{0};
        
        // the ids are RecoTauTag/Configuration/python/HPSPFTaus_cff.py
        // VL=3.5 L=2.5 M=1.5 T=0.8
        vector<float>   *isoPileupWeightedRaw{0};//byPileupWeightedIsolationRaw3Hits{0};

        //mva isolation newDM raw
        vector<float>   *isoMva{0};
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
