#ifndef CHARGEDHIGGSTOPBOTTOMFULLHAD_H
#define CHARGEDHIGGSTOPBOTTOMFULLHAD_H
#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"


#include "interface/Output.hpp" // DataStore
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class ChargedHiggsTopBottomFullHad:  virtual public AnalysisBase
{
public:

    ChargedHiggsTopBottomFullHad() : AnalysisBase () {}
    virtual ~ChargedHiggsTopBottomFullHad () {}

    void Init() override;

    void SetLeptonCuts(Lepton *l) override ;
    void SetJetCuts(Jet*j) override;
    void SetTauCuts(Tau*t) override;

    void BookCutFlow(string l, string category);
    void BookHisto(string l, string category, string phasespace);
    void BookFlavor(string l, string category, string phasespace, string flavor, string SR);
    void Preselection();

    // function with various plots
    void jetPlot(Event*e, string label, string category, string systname, string jetname);
    void leptonPlot(Event*e, string label, string category, string systname, string phasespace);
    void eventShapePlot(Event*e, string label, string category, string systname, string phasespace);
    void classifyHF(Event*e, string label, string category, string systname, string jetname, string SR);
    void leptonicHiggs(Event*e, string label, string systname, TLorentzVector b1, TLorentzVector b2, TLorentzVector p4W, string combination);

    void computeVar(Event*e);

    void printSynch(Event*e);

    int analyze(Event*,string systname) override;
    const string name() const override {return "ChargedHiggsTopBottomFullHad";}

    vector<string> weights;


private:

    CutSelector cut;

    enum CutFlow{ Total=0,
                  NoLep,
                  NoTau,
                  MaxCut
    };

    double evt_HT=-1;

    /////
    /////

    DataStore varValues_;

    //
    //TMVA::Reader *reader_ ;
    vector<TMVA::Reader*> readers_;


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
