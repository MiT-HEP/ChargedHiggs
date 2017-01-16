#ifndef CHARGEDHIGGSTAUNU_H
#define CHARGEDHIGGSTAUNU_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class ChargedHiggsTauNu:  virtual public AnalysisBase
{
    public:
        ChargedHiggsTauNu() : AnalysisBase() {}
        enum CutFlow{ Total=0, 
            OneTau ,
            NoLep ,
            ThreeJets ,
            OneBjet ,
            Trigger ,
            Met ,
            AngColl ,
            AngRbb ,
            MaxCut
            };
        // for the n-1 plots
        CutSelector cut;
        
        /*Object definition*/
        inline void SetLeptonCuts(Lepton *l) override { l->SetIsoCut(-1); l->SetPtCut(10);l->SetIsoRelCut(0.15);l->SetEtaCut(2.4); l->SetTightCut(false);}
        inline void SetPhotonCuts(Photon *p) override {p->SetIsoCut(-1); p->SetPtCut(30);}
        inline void SetTauCuts(Tau *t) override { t->SetIsoCut(2.5); t->SetEtaCut(2.1); t->SetPtCut(20); t->SetMuRej(true); t->SetEleRej(true);t->SetTrackPtCut(30.);t->SetProngsCut(1); t->SetDecayMode(1);}
        inline void SetJetCuts(Jet *j) override { j->SetBCut(0.460);j->SetEtaCut(4.7); j->SetEtaCutCentral(2.5);j->SetPtCut(30); j->SetPuIdCut(-999);}
        void SetGenCuts(GenParticle *g)override {};
        /**/

        void Init() override;
        int analyze(Event*,string systname) override;
        // *return the cut flow
        //  direct tau or inv tau
        unsigned Selection(Event*, bool direct=true,bool muon=false) ;
        const string name() const override {return "ChargedHiggsTauNu";}
        // define blind region: Data; Mt> 50
        bool Unblind(Event *e) { if (e->IsRealData() and e->Mt() > 50) return unblind; return true;} // if is not data, no need to return something else

        bool is80X{false};
        bool isLightMass{false};

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
