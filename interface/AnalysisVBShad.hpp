#ifndef ANALYSIS_VBSHAD_H
#define ANALYSIS_VBSHAD_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"
#include "interface/SF_CSVReweight.hpp" // REMOVEME AFTER MERGE OF MR PR
#include "interface/Output.hpp" // DataStore
#include "interface/KinematicFit.hpp"

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
    bool doFinal = false;
    int VERBOSE=false;

    void Init() override;
    int analyze(Event*,string systname) override;
    void EndEvent() override;
    void setTree(Event*e, string label, string  category);
    void setTrainingTree(Event*e, string label, int fi, int fj, int vk, int vl);

    void writeTree(string name, int purp);

    void BookHisto(string l, string category);

    bool checkSignalLabel(string l);

    const string name() const override {return "VBShadAnalysis";}
    void SetLeptonCuts(Lepton *l) override ; 
    void SetTauCuts(Tau*t) override;
    void SetJetCuts(Jet *j) override ;
    void SetFatJetCuts(FatJet *f) override;

    float Getjetres(Jet* ajet);
    std::pair<float, int> resolvedDNN(Event*e, string label, string systname);
    std::pair<float, float> resolvedtagger(Event*e, float MV, string label, string systname, float etaV1);
    float jettagForBoosted(Event*e, string label, string systname, float minEtaV, float maxEtaV);
    void genStudies(Event*e, string label);
    void getObjects(Event*e, string label, string systname);
    void VVRestObj(Event*e);
    void studyTriggers(Event*e, string category, string label, string systname);
    float genMtt(Event*e);
    bool genMatchResolved(Event*e, string systname, string label);
    bool genMatchResonant(Event*e, string label, string category);

    bool doMETAnalysis=false;
    bool doBAnalysis=false;
    bool doHADAnalysis=false;
    bool doMETAntiAnalysis=false;
    bool doBAntiAnalysis=false;
    bool doHADAntiAnalysis=false;

    bool doResonant = false;

    bool doWriteTree = true;
    bool usePuppi=false;
    bool doTrigger=false;
    bool writeTrainTree=false;

    bool doTMVA=false;
    bool doResTagKeras = false;
    bool doResTagTMVA = false;
    // those two below are to prune the combination
    bool doMultiTagger = false;
    bool do2DNN = false;

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

    float VjPtCut_W = 35.;
    float VjPtCut_Z = 40.;
    float fjPtCut   = 50.;

    bool V1isZbb=false;
    bool V2isZbb=false;

    float minDPhi=999.;

    // selected Objects
    vector<Jet*> selectedJets;
    vector<FatJet*> selectedFatJets;
    vector<FatJet*> selectedMirrorFatJets;
    vector<FatJet*> selectedFatZbb;

    vector<float> bosonVDiscr;
    vector<float> bosonTDiscr;
    vector<float> bosonMass;
    vector<float> bosonBBDiscr;
    vector<float> bosonBBMass;
    vector<float> bosonBBTDiscr;

    vector<Jet*> forwardJets;
    vector<Jet*> bosonJets;
    vector<Jet*> vetoJets;

    TLorentzVector p4VV;
    TLorentzVector p4jj;
    TLorentzVector p4VVjj;

    float evt_Mjj=-100;
    float evt_Detajj=-100;
    float evt_Dphijj=-100;
    float evt_Jet2Eta=-100;
    float evt_Jet2Pt=-100;

    float bosV2j2Pt=-100;
    float bosV2j1Pt=-100;

    float evt_MVV=-100;
    float evt_DRV2=-100;
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

    float evt_mtop=0;

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
    float evt_maxDnn = 0.;
    float evt_v_score = 0.;
    float evt_maxkeras = -999.;
    int    index_f1 = -1;
    int    index_f2 = -1;
    int    index_v1 = -1;
    int    index_v2 = -1;


    bool evt_genmatch = 0;
    float evt_j1unc = 0;
    float evt_j2unc = 0;

    //VV-rest Frame
    float evt_VVcenEta = -100;
    float evt_VVDRV1j = -100;
    float evt_VVnormPTVVjj = 0;

    TVector3 boostVV;


    float BDTnoBnoMET = -100;
    float BDTwithMET = -100;
    float BDTbtag = -100;
    float MultiBDTwithMET = -100;
    int counterExtrabToVeto_=0;

    /************
     *   TMVA   *
     ************/

    vector<float> bdt;  // score
    vector<float> bdt_multi;
    DataStore varValues_;
    vector<TMVA::Reader*> readers_;
    vector<TMVA::Reader*> readers_multi_;
    vector<TMVA::Reader*> readers_dnn_;

    void InitTmva();
    void ReadTmva();

    /**********************************
     *          SCIKIT                *
     **********************************/

    std::unique_ptr<TPython> py;
    vector<float> *x;

    void InitScikit();
    //void ReadScikit(Event*e);


public:
    // Variables for MVA
    template<class T>
    void SetVariable( string name, T value){ varValues_.Set(name, value); }
    void AddVariable( string name, char type, TMVA::Reader* i_readers);
    void AddSpectator( string name, char type, int r);

    vector<string> weights;
    vector<string> weights_multi;
    vector<string> weights_dnn;   
protected:
    

    std::unique_ptr<JME::JetResolutionObject> jet_resolution;
    //std::unique_ptr<KinematicFit2> kf; 
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
