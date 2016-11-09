#ifndef CHARGEDHIGGSTOPBOTTOM_H
#define CHARGEDHIGGSTOPBOTTOM_H
#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class ChargedHiggsTopBottom:  virtual public AnalysisBase
{
public:

    ChargedHiggsTopBottom() : AnalysisBase () {}
    virtual ~ChargedHiggsTopBottom () {}

    bool do1lAnalysis=false;
    bool do2lAnalysis=false;

    void Init() override;

    void SetLeptonCuts(Lepton *l) override ;
    void SetJetCuts(Jet*j) override;
    void SetTauCuts(Tau*t) override;

    void BookCutFlow(string l, string category);
    void BookHisto(string l, string category, string phasespace);
    void Preselection();

    // function with various plots
    bool genInfo(Event*e, GenParticle * & bAss, GenParticle * & bFromTopH , GenParticle * & bFromTopAss, GenParticle * & bFromH);
    void jetPlot(Event*e, string label, string category, string systname, string jetname);
    void leptonicHiggs(Event*e, string label, string systname, TLorentzVector b1, TLorentzVector b2, TLorentzVector p4W, string combination);

    int analyze(Event*,string systname) override;
    const string name() const override {return "ChargedHiggsTopBottom";}

private:

    CutSelector cut;

    enum CutFlow{ Total=0,
                  OneLep,
                  NoSecondLep,
                  NoTau,
                  Met,
                  Mt,
                  NJets,
                  NB,
                  MaxCut
    };

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
