#ifndef BARE_EVENT_H
#define BARE_EVENT_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareEvent : virtual public BareCollection
{
    public:
    
        enum Selection {
            FullRecommendation     = 1UL << 0,
            HBHENoiseFilter        = 1UL << 1,
            HBHENoiseIsoFilter     = 1UL << 2,
            EcalDeadCellTriggerPrimitiveFilter = 1UL << 3,
            goodVertices           = 1UL << 4,
            eeBadScFilter          = 1UL << 5,
            GlobalTightHalo2016    = 1UL << 6,
            Unknown                = 1UL << 31 // if matching do not work, put it here, but keep the full correct
        };

        BareEvent();
        ~BareEvent();
        void clear() override;
        void defineBranches(TTree*) override;
        void setBranchAddresses(TTree*) override;
        inline string name() override { return "BareEvent"; }
        inline unsigned size() const override { return 1; }
    

        // -- variables
        int isRealData;
        int runNum;
        int lumiNum;
        ULong64_t eventNum;

        float rho;	
        //
    
        vector<string> *metfilterNames;
        unsigned selBits{0};
        bool filterbadChCandidate;
        bool filterbadPFMuon;

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
