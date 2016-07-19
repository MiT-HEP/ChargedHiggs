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

    void Preselection();

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
                  FourJets,
                  B1Pt,
                  NB,
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
