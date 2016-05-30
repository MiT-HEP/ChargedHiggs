#ifndef CHARGEDHIGGS_QCDPURITY_H
#define CHARGEDHIGGS_QCDPURITY_H

#include "interface/AnalysisBase.hpp"
#include "interface/GeneralFunctions.hpp"

class ChargedHiggsQCDPurity:  virtual public AnalysisBase
{
    public:
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override{return "ChargedHiggsQCDPurity";}

        // Configuration
        vector<float> PtBins;

        const string dir="ChargedHiggsQCDPurity/Vars/";
        const string none="ChargedHiggsQCDPurity/NOne/";
        int FindBin(float pt);
        string HistName(float pt, bool Direct=true, bool FullSelection=false,string var = "EtMiss");

    float Upar(Event*e,Tau *t ){ return e->GetMet().Pt() * TMath::Cos( e->GetMet().DeltaPhi( *t ) ) ; };
    float Uperp(Event*e, Tau*t){ return e->GetMet().Pt() * TMath::Sin( e->GetMet().DeltaPhi( *t) );}; // this is positive, deltaPhi>0
    
    bool Unblind(Event *e) { if (e->IsRealData() and e->Mt() > 50) return unblind; return true;} // if is not data, no need to return something else
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
