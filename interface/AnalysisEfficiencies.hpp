#ifndef ANALYSIS_Efficiencies_H
#define ANALYSIS_Efficiencies_H

#include "interface/AnalysisBase.hpp"
#include "interface/Output.hpp" // DataStore

class EfficienciesAnalysis: virtual public AnalysisBase
{
    public:
        EfficienciesAnalysis() : AnalysisBase () {}
        virtual ~EfficienciesAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        void EndEvent() override;

        const string name() const override {return "EfficienciesAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet *j) override ;
        void SetTauCuts(Tau *t) override;
        void SetPhotonCuts(Photon*p) override;

        bool Unblind(Event *e) override { return true;} 

    private:
        
        Object Hmm;
        Lepton *mu0{NULL},*mu1{NULL};
        vector<Jet*> selectedJets; // final that match to the one above

        Lepton *mu0_mini{NULL},*mu1_mini{NULL};
        vector<Lepton*> miniIsoLeptons;
        vector<Jet*> selectedJetsMiniIso; 

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
