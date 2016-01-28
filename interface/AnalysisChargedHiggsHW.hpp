#ifndef CHARGEDHIGGSHW_H
#define CHARGEDHIGGSHW_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsHW:  virtual public AnalysisBase
{
    public:
        void Init() override;
        int analyze(Event*,string) override;
        virtual int VetoFirst(Event*,string);
        virtual int VetoSecond(Event*,string);
        //Double_t deltaR(Double_t, Double_t, Double_t, Double_t);
        Double_t neutrinoPz(TLorentzVector, TLorentzVector, Int_t);
        const string name() const override {return "ChargedHiggsHW";}


        inline void SetLeptonCuts(Lepton *l) override { l->SetIsoCut(0.4); l->SetPtCut(15);l->SetIsoRelCut(-1);l->SetEtaCut(2.4); l->SetTightCut(true);}

        static constexpr float Mw = 80.385;
        static constexpr float Mh = 125.;
        static constexpr float Mtop = 173.34;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
