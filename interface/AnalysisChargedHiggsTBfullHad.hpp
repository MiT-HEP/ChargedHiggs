#ifndef CHARGEDHIGGSTOPBOTTOMFULLHAD_H
#define CHARGEDHIGGSTOPBOTTOMFULLHAD_H
//#ifndef CHARGEDHIGGSTBFULLHAD_H
//#define CHARGEDHIGGSTBFULLHAD_H
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
    void BookFatjetPro(string l, string category, string phasespace, string pile);
    void BookGenTTBar(string l, string category, string phasespace, string labelHF);
    void BookHisto(string l, string category, string phasespace);
    void BookFlavor(string l, string category, string phasespace, string flavor, string SR);
    void BookGenMatch(string l, string category, string phasespace, string cut321, string state);
    void BookEnCorr(string category, string phasespace);
    void Preselection();

    // function with various plots
    void jetPlot(Event*e, string label, string category, string systname, string jetname);
    void fatjetPlot(Event*e, string label, string systname, string phasespace);
    void higgsPlot(Event*e, string label, string category, string systname, string phasespace);
    void leptonPlot(Event*e, string label, string category, string systname, string phasespace);
    void eventShapePlot(Event*e, string label, string category, string systname, string phasespace);
    void classifyHF(Event*e, string label, string category, string systname, string jetname, string SR);
    void leptonicHiggs(Event*e, string label, string systname, TLorentzVector b1, TLorentzVector b2, TLorentzVector p4W, string combination);

    void classifyLabelGenEv(Event*e, string label, string systname, string phasespace);
    void getCandidate(Event*e, string label, string systname, string phasespace, bool mirror);
    void PlotAss(Event*e, string label, string systname, string phasespace);


    void computeVar(Event*e);

    void printSynch(Event*e);

    bool genInfoForSignal(Event*e);
    void Encorrection(Event*e, string systname, string phasespace);


    int analyze(Event*,string systname) override;
    //    const string name() const override {return "ChargedHiggsTBfullHad";}
    const string name() const override {return "ChargedHiggsTopBottomFullHad";}

    // Tree
    bool writeTree = false;
    void setTree(Event*e, string label, string  category);

    // Variables for MVA

    template<class T>
    void SetVariable( string name, T value){ varValues_.Set(name, value); }
    void AddVariable( string name, char type, int r);
    void AddSpectator( string name, char type, int r);

    vector<string> weights;


private:

    CutSelector cut;

    enum CutFlow{ Total=0,
                  NoTau,
                  NoLep,
                  HTcut,
                  OneBOneFat,
                  OneBOneFatMirror,
                  MaxCut
    };

    bool doSig = 1; 
    bool doMirror = 0;
    bool doLep = 0;

    double evt_ST=-1;
    double evt_HT=-1;
    double evt_minDRbb=-1;
    double evt_minDRbb_invMass=-1;
    double evt_DEtaMaxBB=-1;

    double evt_C=0;

    /////
    ///// higgs candidates

    double evt_MH_tb=-1;
    double evt_MH_t1b=-1;
    double evt_MH_t0b=-1;
    double evt_MH_Wbb=-1;
    double evt_MH_wbj=-1;

    double evt_PtH_tb =-1;
    double evt_PtH_t1b=-1;
    double evt_PtH_t0b=-1;
    double evt_PtH_Wbb=-1;
    double evt_PtH_wbj=-1;

    
    //bjets
    Jet* leadingb=NULL;     // tb,wbb
    Jet* leadingbWBJ = NULL;//wbj
    Jet* secondb=NULL;      //wbj
    Jet* secondbwbb=NULL;   //wbb

    //fatjets
    int numtop=0;
    FatJet* topJet=NULL;    //tb
    FatJet* wJet=NULL;      //wbb
    FatJet* wJetwbj=NULL;   //wbj

    //make top
    TLorentzVector topFromHOpenCand;//wbb
    TLorentzVector topFromHwbj;     //wbj

    //associated
    int num_otherfj = 0;
    int num_otherbj = 0;
    int num_otherj = 0;

    /////
    /////
    GenParticle * topFromH=NULL;
    GenParticle * topAss =NULL;
    GenParticle * topBKGplus = NULL;
    GenParticle * topBKGminus = NULL;
    int topFromH_lep = 0;
    int topAss_lep = 0;
    int topBKGplus_lep = 0;
    int topBKGminus_lep = 0;
    int siglep = 0;

    GenParticle * WFromTopH=NULL;
    GenParticle * WFromTopAss=NULL;
    GenParticle * WBKGplus=NULL;
    GenParticle * WBKGminus=NULL;
    int WFromTopH_lep = 0;
    int WFromTopAss_lep = 0;
    int WBKGplus_lep = 0;
    int WBKGminus_lep = 0;


    GenParticle * bFromH=NULL;
    GenParticle * bFromTopH=NULL;
    GenParticle * bFromTopAss=NULL;
    GenParticle * bBKGplus=NULL;
    GenParticle * bBKGminus=NULL;



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
