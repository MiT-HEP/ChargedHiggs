#ifndef ANALYSIS_VBSHAD_H
#define ANALYSIS_VBSHAD_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"
#include "interface/SF_CSVReweight.hpp" // REMOVEME AFTER MERGE OF MR PR
#include "interface/Output.hpp" // DataStore

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class TRandom;
#include "TPython.h"
#include <memory>

#include "interface/JetResolutionObject.hpp"

class VBShadAnalysis: virtual public AnalysisBase
{
public:
    VBShadAnalysis() : AnalysisBase () {}
    virtual ~VBShadAnalysis (){}

    int year=2016; // master switch for year configuration
    
    void Init() override;
    int analyze(Event*,string systname) override;
    void EndEvent() override;
    void setTree(Event*e, string label, string  category);

    void BookHisto(string l, string category, string signalLabel);

    const string name() const override {return "VBShadAnalysis";}
    void SetLeptonCuts(Lepton *l) override ; 
    void SetTauCuts(Tau*t) override;
    void SetJetCuts(Jet *j) override ;
    void SetFatJetCuts(FatJet *f) override;

    float resolvedtagger(Event*e, float MV, string label, string systname, float etaV1);
    float jettagForBoosted(Event*e, string label, string systname, float minEtaV, float maxEtaV);
    void genStudies(Event*e, string label);
    void getObjects(Event*e, string label, string systname);
    void studyTriggers(Event*e, string category, string label, string systname);
    double genMtt(Event*e);
    bool genMatchResolved(Event*e, string systname, string label);

    bool doMETAnalysis=false;
    bool doBAnalysis=false;
    bool doHADAnalysis=false;
    bool doHADAntiAnalysis=false;
    bool doMETAntiAnalysis=false;
    bool writeTree = true;
    bool usePuppi=false;
    bool doTrigger=false;

    bool doTMVA=false;

private:

    // for genStudies
    GenParticle *genVp = NULL;
    GenParticle *genVp2 = NULL;

    GenParticle * dauV1a = NULL;
    GenParticle * dauV1b = NULL;
    GenParticle * dauV2a = NULL;
    GenParticle * dauV2b = NULL;

    double dauRatioV1=-1;
    double dauRatioV2=-1;
    double cosThetaV1=-10;
    double cosThetaV2=-10;

    bool V1isZbb=false;
    bool V2isZbb=false;

    double minDPhi=999.;

    // selected Objects
    vector<Jet*> selectedJets;
    vector<FatJet*> selectedFatJets;
    vector<FatJet*> selectedFatZbb;

    vector<float> bosonVDiscr;
    vector<float> bosonTDiscr;
    vector<float> bosonMass;
    vector<float> bosonBBDiscr;
    vector<float> bosonBBMass;

    vector<Jet*> forwardJets;
    vector<Jet*> bosonJets;

    TLorentzVector p4VV;
    TLorentzVector p4jj;
    TLorentzVector p4VVjj;

    float evt_Mjj=-100;
    float evt_Detajj=-100;
    float evt_Dphijj=-100;
    float evt_Jet2Eta=-100;
    float evt_Jet2Pt=-100;

    float evt_MVV=-100;
    float evt_PetaVV=-100;
    float evt_DetaVV=-100;
    float evt_MVV_gen=-100;
    float evt_PTVV=0;
    float evt_PTV1=0;
    float evt_PTV2=0;
    float evt_EtaMinV=-100;
    float evt_EtaMaxV=-100;

    float evt_normPTVVjj=0;
    float evt_cenPTVVjj=0;

    float evt_zepVB=-100;
    float evt_zepV2=-100;
    float evt_cenEta=-100;
    float evt_zepVV=-100;
    float evt_DRV1j=-100;
    float evt_FW2=-100;

    float evt_bosV1mass=-1;
    float evt_bosV1discr=-1;
    float evt_bosV1tdiscr=-1;
    float evt_bosV1unc = 0;
    float evt_bosV2mass=-1;
    float evt_bosV2discr=-1;
    float evt_bosV2tdiscr=-1;
    float evt_bosV2unc = 0;
    float evt_chi2_= -1;

    bool evt_genmatch = 0;
    float evt_j1unc = 0;
    float evt_j2unc = 0;

    float BDTnoBnoMET = -100;
    float BDTwithMET = -100;
    int counterExtrabToVeto_=0;

    /************
     *   TMVA   *
     ************/

    vector<float> bdt;  // score
    DataStore varValues_;
    vector<TMVA::Reader*> readers_;
    void InitTmva();
    void ReadTmva();

public:
    // Variables for MVA
    template<class T>
    void SetVariable( string name, T value){ varValues_.Set(name, value); }
    void AddVariable( string name, char type, int r);
    void AddSpectator( string name, char type, int r);

    vector<string> weights;
   
protected:
    

    std::unique_ptr<JME::JetResolutionObject> jet_resolution;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
