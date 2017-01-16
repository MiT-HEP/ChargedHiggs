#ifndef CHARGEDHIGGS_QCDPURITY_H
#define CHARGEDHIGGS_QCDPURITY_H

#include "interface/AnalysisBase.hpp"
#include "interface/GeneralFunctions.hpp"

#include "interface/AnalysisChargedHiggsTauNu.hpp"

class ChargedHiggsQCDPurity:  virtual public ChargedHiggsTauNu
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

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
