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
    
    const string name() const override {return "VBShadAnalysis";}
    void SetLeptonCuts(Lepton *l) override ; 
    void SetJetCuts(Jet *j) override ;
    void SetFatJetCuts(FatJet *f) override;

    double resolvedtagger(Event*e, double Mw, string label, string systname);
    void genStudies(Event*e, string label);
    void getObjects(Event*e, string label, string systname);

    bool doMETAnalysis=false;
    bool doBAnalysis=false;
    bool doHADAnalysis=false;

private:

    GenParticle *genVp = NULL;
    GenParticle *genVp2 = NULL;

    // selected Objects
    vector<Jet*> selectedJets;
    vector<FatJet*> selectedFatJets;

    vector<Jet*> forwardJets;
    vector<Jet*> bosonJets;

    double evt_Mjj=-1;
    double evt_Detajj=-1;
    double evt_Dphijj=-1;
    double evt_MVV=-1;
    double evt_DetaVV=-1;
    double evt_MVV_gen=-1;

    double evt_zepVB=-1;
    double evt_zepV2=-1;

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
