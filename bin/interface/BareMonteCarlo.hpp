#ifndef BARE_MONTECARLO_H
#define BARE_MONTECARLO_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareMonteCarlo : virtual public BareP4
{
    public:
        BareMonteCarlo();
        ~BareMonteCarlo();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BareMonteCarlo"; }
        void compress() override;

        // -- variables
        //TClonesArray *p4{0}; // gen particles
        vector<int>  *pdgId{0};

        enum StatusFlags{
            // https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/DataFormats/HepMCCandidate/interface/GenParticle.h#L41-L104
            // ROBUST
            PromptFinalState = 1UL<<0, //isPromptFinalState()
            PromptDecayed = 1UL<<1, //isPromptDecayed()
            DirectPromptTauDecayProductFinalState = 1UL<<2,//isDirectPromptTauDecayProductFinalState()
            // HISTORY DEPENDENT
            HardProcess = 1UL<<3, // isHardProcess
            HardProcessBeforeFSR   = 1UL<<4, // fromHardProcessBeforeFSR
            HardProcessDecayed = 1UL<<5 , //fromHardProcessDecayed() -- like a tau from the hard process
            LastCopy  = 1UL<<6, // isLastCopy()
            LastCopyBeforeFSR = 1UL<<7, //isLastCopyBeforeFSR()
            Dressed = 1UL<<8, // dressed PromptFinal Lepton, cone 0.1
        };
        vector<unsigned> *flags{0}; // status flags of the gen particles

        // genjets
        TClonesArray *jetP4{0};
        TClonesArray *jetAK8P4{0};

        //
        int puTrueInt;
        //
        float mcWeight ;
        float qScale   ;
        float alphaQED ;
        float alphaQCD ;
        float x1       ;
        float x2       ;
        int   pdf1Id   ;
        int   pdf2Id   ;
        float scalePdf ;

        // QCD scale reweights
        float r2f1;
        float r5f1;
        float r1f2;
        float r2f2;
        float r1f5;
        float r5f5;

        // nBHadrons
        int nBHadronsHiggs{0},nBHadronsTop{0},nBHadrons{0};
        int nCHadrons{0};
        int genTtbarId;
    
        // PDF reweights
        std::vector<float> *pdfRwgt{0};

        // Iso and Iso Frixione
        std::vector<float> *genIso{0};

        // Parent index
        std::vector<int> *parent{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
