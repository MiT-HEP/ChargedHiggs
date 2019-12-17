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

class VBShadAnalysis: virtual public AnalysisBase
{
public:
    VBShadAnalysis() : AnalysisBase () {}
    virtual ~VBShadAnalysis (){}
    
    void Init() override;
    int analyze(Event*,string systname) override;
    void EndEvent() override;
    void setTree(Event*e, string label, string  category);

    const string name() const override {return "VBShadAnalysis";}
    void SetLeptonCuts(Lepton *l) override ; 
    void SetTauCuts(Tau*t) override;
    void SetJetCuts(Jet *j) override ;
    void SetFatJetCuts(FatJet *f) override;

    float resolvedtagger(Event*e, float MV, string label, string systname);
    float jettagForBoosted(Event*e, string label, string systname);
    void genStudies(Event*e, string label);
    void getObjects(Event*e, string label, string systname);

    bool doMETAnalysis=false;
    bool doBAnalysis=false;
    bool doHADAnalysis=false;
    bool writeTree = true;

private:

    GenParticle *genVp = NULL;
    GenParticle *genVp2 = NULL;

    // selected Objects
    vector<Jet*> selectedJets;
    vector<FatJet*> selectedFatJets;

    vector<Jet*> forwardJets;
    vector<Jet*> bosonJets;

    TLorentzVector p4VV;
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

    float evt_zepVB=-100;
    float evt_zepV2=-100;
    float evt_cen=-100;
    float evt_zepVV=-100;
    float evt_DRV1j1=-100;
    float evt_FW2=-100;

public:
    vector<string> weights;
    
protected:
    
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
