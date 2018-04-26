#ifndef CHARGEDHIGGSTOPBOTTOM_H
#define CHARGEDHIGGSTOPBOTTOM_H
#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"
#include "interface/GetBinning.hpp"
#include "interface/SplitMC.hpp"
#include <memory>

#include "TRandom3.h"
#include "TPython.h"

#include "TStopwatch.h"

#include "interface/Output.hpp" // DataStore
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class ChargedHiggsTopBottom:  virtual public AnalysisBase
{
public:

    ChargedHiggsTopBottom() : AnalysisBase () {}
    virtual ~ChargedHiggsTopBottom () {}

    bool doJECSources = true;

    bool doSynch = false;
    bool doQCD = false;
    bool doBDTSyst = true;
    bool doFinal = true; // true for final ; no need to compute fancy stuff
    bool writeTree = false; // false for final
    bool doSplit = true; // true for final
    bool doMorePlot = false; // false for final

    // Analysis type
    // do1lAnalysis, do2lAnalysis, doTaulAnalysis from the config
    bool do1lAnalysis=false;
    bool do2lAnalysis=false;
    bool doTaulAnalysis=false;
    // doSplitLepCat HARDcoded
    bool doSplitLepCat = true; // true for final

    // trigger bits
    bool passTriggerMu=true;
    bool passTriggerEle=true;

    bool passTriggerMuEle=true;
    bool passTriggerMuMu=true;
    bool passTriggerEleEle=true;


    void Init() override;

    void SetLeptonCuts(Lepton *l) override ;
    void SetJetCuts(Jet*j) override;
    void SetTauCuts(Tau*t) override;

    void BookCutFlow(string l, string category);
    void BookHisto(string l, string category, string phasespace);
    void BookFlavor(string l, string category, string phasespace, string flavor, string SR);
    void BookSplit(string l, string category);
    unsigned findMC(string label);
    int FillSplit(Event*e, string l, string category, string systname);

    void Preselection();

    // function with various plots
    void jetPlot(Event*e, string label, string category, string systname, string jetname);
    void leptonPlot(Event*e, string label, string category, string systname, string phasespace);
    void eventShapePlot(Event*e, string label, string category, string systname, string phasespace);
    void classifyHF(Event*e, string label, string category, string systname, string jetname, string SR);
    void leptonicHiggs(Event*e, string label, string systname, TLorentzVector b1, TLorentzVector b2, TLorentzVector p4W, string combination);
    void fillMoneyPlot(Event*e, string category, string systname,string SRlabel, string label);

    void computeVar(Event*e);

    void printSynch(Event*e, string category, string systname);

    double genInfoForWZ(Event*e);
    int genInfoForBKG(Event*e);
    bool genInfoForSignal(Event*e);

    // function for the mini-tree
    void setTree(Event*e, string label, string  category);

    int analyze(Event*,string systname) override;
    const string name() const override {return "ChargedHiggsTopBottom";}

    // Variables for MVA
    template<class T>
    void SetVariable( string name, T value){ varValues_.Set(name, value); }
    void AddVariable( string name, char type, int r);
    void AddSpectator( string name, char type, int r);

    vector<string> weights;

    std::unique_ptr<SplitMCAnalysis> spliMC_ ;//( new SplitMCAnalysis());

private:


    std::unique_ptr<GetBinning> binLow_;
    std::unique_ptr<GetBinning> binMedium_;
    std::unique_ptr<GetBinning> binHigh_;

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

    Lepton* leadLep=NULL;
    Lepton* trailLep=NULL;

    double evt_HT=-1;
    double evt_ST=-1;
    double evt_DRlbmaxPt=-1;
    double evt_MlbmaxPt=-1;

    double evt_minDRbb=-1;
    double evt_minDRbb_invMass=-1;

    double evt_minDRlb_invMass=-1;
    double evt_minDRlb=-1;

    double evt_minMasslb=99999;

    double evt_avDRBB=-1;

    double evt_DEtaMaxBB_invMass=-1;
    double evt_DEtaMaxBB=-1;
    double evt_DEtaMaxJJ=-1;
    double evt_DEtaMaxJJ_invMass=-1;
    double evt_MJJJmaxPt=-1;
    double evt_AvDRJJJmaxPt=-1;
    double evt_AvCSVPt=0;

    double evt_MT=-1;
    double evt_MT2bb=-1;
    double evt_MT2bb1l=-1;
    double evt_DRl1b1=-1;
    double evt_Ml1b1=-1;

    double evt_DRl1j1=-1;
    double evt_DRl1j2=-1;
    double evt_DRl1j3=-1;

    // these are defined only for 2l
    double evt_DRl2b1=-1;
    double evt_MT2ll=-1;
    double evt_MTmin=-1;
    double evt_MTmax=-1;
    double evt_MTtot=-1;

    double evt_HemiMetOut=0;
    double evt_C=0;
    double evt_FW2=0;


    int nGenB = 0 ;
    int genLepSig = 0 ;

    int ev_forTrain = 0;

    /////
    /////

    GenParticle * bAss=NULL;
    GenParticle * bFromTopH=NULL;
    GenParticle * bFromTopAss=NULL;
    GenParticle * topFromH=NULL;
    GenParticle * bFromH=NULL;
    GenParticle * leptonFromTopH=NULL;
    GenParticle * WFromTopH=NULL;
    GenParticle * WFromTopAss=NULL;
    GenParticle * leptonTopAssH=NULL;

    bool doTMVA{false};
    bool doScikit{true};


    // TMVA
    //    int nbinsBDT=400;
    //    float binMIN=-1.;

    // Keras
    //    int nbinsBDT=200;
    int nbinsBDT=1000;
    float binMIN=0.;
    float binMAX=1.;


    /************
     *   TMVA   *
     ************/

    vector<float> bdt;  // score
    DataStore varValues_;
    vector<TMVA::Reader*> readers_;
    void InitTmva();
    void ReadTmva(Event*e);


    /**********************************
     *          SCIKIT                *
     **********************************/

    std::unique_ptr<TPython> py;
    vector<float> x;
    vector<string> discr;
    vector<float> scikit; // like bdt

    void InitScikit();
    void ReadScikit(Event*e);

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
