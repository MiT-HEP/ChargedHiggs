#ifndef BARE_MET_H
#define BARE_MET_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareMet : virtual public BareP4
{
    public:
        BareMet();
        ~BareMet();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BareMet"; }
        void compress() override;

        // -- variables
        //TClonesArray *p4;
    
        // Should not stay here, but in MC -> we will esculed it if run on onlyMc
        TClonesArray *genP4{0};

        TLorentzVector *metNoMu{0};
        TLorentzVector *pfMet_e3p0{0}; // raw
        TLorentzVector *trackMet{0};
        TLorentzVector *neutralMet{0};
        TLorentzVector *photonMet{0};
        TLorentzVector *HFMet{0};
        TLorentzVector *CaloMet{0};
        TLorentzVector *RawMet{0};
        TLorentzVector *SmearedMet{0};
    
        TLorentzVector *metPuppi{0};
        TLorentzVector *metCleanMu{0};
        TLorentzVector *metCleanEG{0};
        TLorentzVector *metUnClean{0};
        // --- SYSTEMATICS -- //
        enum Syst{ JesUp= 0 , JesDown, JerUp,JerDown,UnclusterUp, UnclusterDown, TauUp, TauDown, PhotonUp, PhotonDown , ElectronUp, ElectronDown, MuonUp, MuonDown,  MaxSyst};

        // for PF MET
        TClonesArray *metSyst{0};
        // for Puppi MET
        TClonesArray *metPuppiSyst{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
