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
    void reset();
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
    void SetPhotonCuts(Photon *p) override;

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

    int getIndex(int nFat) {
        auto pt = selectedFatJets[nFat]->Pt();
        auto etaAbs = fabs(selectedFatJets[nFat]->Eta());
        int index = (( pt >= 300 and pt < 400 ) and (etaAbs < 1.3) )? 0 : -1 ; // pt1Eta1
        index = ((pt >= 400 and pt < 500) and (etaAbs < 1.3) )? 1 : index ; // pt2Eta1
        index = ((pt >= 500 and pt < 600) and (etaAbs < 1.3) )? 2 : index ; // pt2Eta1
        index = ((pt >= 600 and pt < 750) and (etaAbs < 1.3) )? 3 : index ; // pt2Eta1
        index = ((pt >= 750) and (etaAbs < 1.3) )? 4 : index ; // pt3Eta1

        index = ((pt >= 300 and pt < 400) and (etaAbs >= 1.3) )? 5 : index ; // pt1Eta2
        index = ((pt >= 400 and pt < 500) and (etaAbs >= 1.3) )? 6 : index ; // pt1Eta2
        index = ((pt >= 500 and pt < 600) and (etaAbs >= 1.3) )? 7 : index ; // pt2Eta2
        index = ((pt >= 600 and pt < 750) and (etaAbs >= 1.3) )? 8 : index ; // pt2Eta2
        index = ((pt >= 750) and (etaAbs >= 1.3) )? 9 : index ; // pt3Eta2
        return index;
    }

    int getIndexB(int nFat) {
        auto pt = selectedFatZbb[nFat]->Pt();
        auto etaAbs = fabs(selectedFatZbb[nFat]->Eta());
        int index = (( pt >= 300 and pt < 400 ) and (etaAbs < 1.3) )? 0 : -1 ; // pt1Eta1
        index = ((pt >= 400 and pt < 500) and (etaAbs < 1.3) )? 1 : index ; // pt2Eta1
        index = ((pt >= 500 and pt < 600) and (etaAbs < 1.3) )? 2 : index ; // pt2Eta1
        index = ((pt >= 600 and pt < 750) and (etaAbs < 1.3) )? 3 : index ; // pt2Eta1
        index = ((pt >= 750) and (etaAbs < 1.3) )? 4 : index ; // pt3Eta1

        index = ((pt >= 300 and pt < 400) and (etaAbs >= 1.3) )? 5 : index ; // pt1Eta2
        index = ((pt >= 400 and pt < 500) and (etaAbs >= 1.3) )? 6 : index ; // pt1Eta2
        index = ((pt >= 500 and pt < 600) and (etaAbs >= 1.3) )? 7 : index ; // pt2Eta2
        index = ((pt >= 600 and pt < 750) and (etaAbs >= 1.3) )? 8 : index ; // pt2Eta2
        index = ((pt >= 750) and (etaAbs >= 1.3) )? 9 : index ; // pt3Eta2
        return index;
    }

    bool doMETAnalysis=false;
    bool doBAnalysis=false;
    bool doHADAnalysis=false;
    //
    bool doMETAntiAnalysis=false;
    bool doBAntiAnalysis=false;
    bool doHADAntiAnalysis=false;
    //
    bool doSideBand = false;
    
    bool doResonant = false;

    bool doWriteTree = true;
    bool writeTrainTree=false;
    bool usePuppi=false;
    bool doTrigger=false;
    bool doStudyMass=false; //inclusive SDMass, SubjetMass, W/ZvsQCD plots
    bool doStudySFfat=true;
    bool useParticleNet=true;

    bool doTMVA=true;
    bool doResTagKeras = false;
    bool doResTagTMVA = true;
    // those two below are to prune the combination
    bool doMultiTagger = false;
    bool do2DNN = false;

    bool computeGenPhaseSpace(){
        // fwd - bkw jets 
        GenParticle *q1=nullptr, *q2=nullptr;
        // find vector bosons
        GenParticle *q11=nullptr,*q12=nullptr,*q21=nullptr,*q22=nullptr;  // decay product
        vector<GenParticle*> q ;

        for(Int_t i = 0; i < e->NGenPar(); i++){
            GenParticle *genpar = e->GetGenParticle(i);
            if (genpar == nullptr) break;
            if( ! genpar->IsLHE()) continue; // loop only on the LHE particles

            int apdg= abs(genpar->GetPdgId());

            if (apdg< 6){ // do I also have the V decays?
                q.push_back(genpar);
            }
        }

        if (q.size() <6) return false;
        // find vector bosons
        // search the couples closer in mass to Z 91.18 and W =
        constexpr float mW = 80.34; 
        constexpr float mZ = 91.18; 
        
        float delta1 =1000.;  // find the best candidates
        for (int i=0;i<6;++i)
        for (int j=i+1;i<6;++i)
        {
            // Z
            if (abs(q[i]->GetPdgId()) == abs(q[j]->GetPdgId()) and q[i]->GetPdgId()*q[j]->GetPdgId()<0) {
                double m = q[j]->InvMass(q[i]);
                if ( fabs(m-mZ) < delta1) {
                    q11= q[i];
                    q12 = q[j];
                    delta1 = fabs(m-mZ);
                }
            } // ended Z loop
            // W 
            if (  
                (abs(q[i]->GetPdgId()) + abs(q[j]->GetPdgId()) )%2 ==1
                and q[i]->GetPdgId()*q[j]->GetPdgId()>0
                ) {
                double m = q[j]->InvMass(q[i]);
                if ( fabs(m-mW) < delta1) {
                    q11= q[i];
                    q12 = q[j];
                    delta1 = fabs(m-mW);
                }
            } // ended Z loop
        }
        
        //////// ----------- SECOND LOOP
        float delta2 =1000.;  // find the best candidates
        for (int i=0;i<6;++i)
        {
        if (q[i] == q11 or q[i] == q12) continue;
        for (int j=i+1;i<6;++i)
        {
            if (q[j] == q11 or q[j] == q12) continue;
            // Z
            if (abs(q[i]->GetPdgId()) == abs(q[j]->GetPdgId()) and q[i]->GetPdgId()*q[j]->GetPdgId()<0) {
                double m = q[j]->InvMass(q[i]);
                if ( fabs(m-mZ) < delta2) {
                    q21= q[i];
                    q22 = q[j];
                    delta2 = fabs(m-mZ);
                }
            } // ended Z loop
            // W 
            if (  
                (abs(q[i]->GetPdgId()) + abs(q[j]->GetPdgId()) )%2 ==1
                and q[i]->GetPdgId()*q[j]->GetPdgId()>0
                ) {
                double m = q[j]->InvMass(q[i]);
                if ( fabs(m-mW) < delta2) {
                    q21= q[i];
                    q22 = q[j];
                    delta2 = fabs(m-mW);
                }
            } // ended Z loop
        }
        }

        // end association
        TLorentzVector zero(0,0,0,0); // just to satisfy my paranoia
        Object V1, V2 ; V1.SetP4(zero); V2.SetP4(zero);
        V1 += *q11 ; V1 +=  *q12;
        V2 += *q21 ; V2 += *q22;

        for (int i=0;i<6;++i)
        {
            if (q[i] == q11 or q[i] == q12 or q[i]== q21 or q[i]==q22) continue;
            if (q1==nullptr) q1=q[i];
            else if (q2==nullptr) q2=q[i];
        }

        // find fwd bkw gen jets matching q1 and q2
        GenJet *j1 = nullptr, *j2 = nullptr;
        for (int i=0;;++i){
            GenJet *gj=e->GetGenJet(i);
            if (gj == nullptr) break;
            if (j1 == nullptr and gj->DeltaR(q1) <0.4) j1 =gj;
            if (j2 == nullptr and gj->DeltaR(q2) <0.4) j2 =gj;
        }

        if (j1 == nullptr or j2 == nullptr) return false;

        double mjj = j1->InvMass(j2);
        double mvv = V1.InvMass(V2);
        
        if (q11->Pt() <10) return false;
        if (q12->Pt() <10) return false;
        if (q21->Pt() <10) return false;
        if (q22->Pt() <10) return false;
        if (V1.Pt() <10 ) return false;
        if (V2.Pt() <10 ) return false;
        if (j1->Pt() <30) return false;
        if (j2->Pt() <30) return false;
        if (fabs(j1->Eta()) > 4.7) return false;
        if (fabs(j2->Eta()) > 4.7) return false;
        if (fabs(j1->Eta() -j2->Eta()) <2.5 ) return false;  //deltaetajj
        if (mjj <100) return false;
        if (mvv <100) return false;
        return true;
    }

    bool toClip(float mvvGeV, const string & aqgc_param)
    {   // true is above clipping limits
        // arXiv:2004.05174

        float mvv = mvvGeV/1000.; // in TeV
        // split the name  
        // fs0_21p00
        size_t split = aqgc_param.find('_');
        string param = aqgc_param.substr(0,split);
        string num = aqgc_param.substr(split+1);
        std::replace(num.begin(),num.end(),'p','.');
        std::replace(num.begin(),num.end(),'m','-');
        float aqgc_val = std::atof(num.c_str());

        // 1 operator
        float bound =0.;
             if (param =="fs0") { bound= 32.* M_PI / std::pow(mvv,4);}
        else if (param =="fs1") { bound= 96./7.*M_PI / std::pow(mvv,4);}
        else if (param =="fs2") { bound= 96./5.*M_PI / std::pow(mvv,4);}
        //--
        else if (param =="fm0") { bound= 32./std::sqrt(6.F) *M_PI / std::pow(mvv,4);}
        else if (param =="fm1") { bound= 128./std::sqrt(6.F)*M_PI/std::pow(mvv,4);}
        else if (param =="fm2") { bound= 16./std::sqrt(2.F)*M_PI/std::pow(mvv,4);}
        else if (param =="fm3") { bound= 64./std::sqrt(2.F)*M_PI/std::pow(mvv,4);} 
        else if (param =="fm4") { bound= 32.*M_PI/std::pow(mvv,4);} 
        else if (param =="fm5") { bound= 64.*M_PI/std::pow(mvv,4);}
        else if (param =="fm7") { bound= 256./std::sqrt(6.F)*M_PI / std::pow(mvv,4);}
        //--
        else if (param =="ft0") { bound= 12./5. *M_PI / std::pow(mvv,4);}
        else if (param =="ft1") { bound= 24./5. *M_PI / std::pow(mvv,4);}
        else if (param =="ft2") { bound= 96./13. *M_PI / std::pow(mvv,4);}
        else if (param =="ft3") { bound= 32./3 *M_PI / std::pow(mvv,4);}
        else if (param =="ft4") { bound= 16.*M_PI / std::pow(mvv,4);}
        else if (param =="ft5") { bound= 8./std::sqrt(3.F) *M_PI / std::pow(mvv,4);}
        else if (param =="ft6") { bound= 48./7. *M_PI / std::pow(mvv,4);}
        else if (param =="ft7") { bound= 32./std::sqrt(3.F) *M_PI / std::pow(mvv,4);}
        else if (param =="ft8") { bound= 3./2. *M_PI / std::pow(mvv,4);}
        else if (param =="ft9") { bound= 24./7. *M_PI / std::pow(mvv,4);}
        else { Log(__FUNCTION__,"ERROR","Unknown aqgc clipping for param" + param); throw abortException();}
        
        if (aqgc_val > bound) return true;
        else return false;

    }

private:

    // for genStudies
    GenParticle *genLep = NULL;
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
    bool badHFjetVeto = false;

    // selected Objects
    vector<Jet*> selectedJets;
    vector<FatJet*> selectedFatJets;
    vector<FatJet*> selectedFatJetsIn;
    vector<FatJet*> selectedFatJetsOut;
    vector<FatJet*> selectedFatJetsWide;
    vector<FatJet*> selectedMirrorFatJets;
    vector<FatJet*> selectedFatZbb;
    vector<FatJet*> selectedFatZbbIn;
    vector<FatJet*> selectedFatZbbWide;

    vector<float> bosonVDiscr;
    vector<float> bosonTDiscr;
    vector<float> bosonBDiscr;
    vector<float> bosonMass;
    vector<float> bosonBBDiscr;
    vector<float> bosonBBMass;
    vector<float> bosonBBTDiscr;

    vector<Jet*> forwardJets;
    vector<Jet*> bosonJets;
    vector<Jet*> vetoJets;

    TLorentzVector p4VV;
    TLorentzVector p4V1;
    TLorentzVector p4V2;
    TLorentzVector p4jj;
    TLorentzVector p4VVjj;

    float evt_Mjj=-100;
    float evt_Detajj=-100;
    float evt_Dphijj=-100;
    float evt_Jet1Eta=-100;
    float evt_Jet1Pt=-100;
    float evt_Jet2Eta=-100;
    float evt_Jet2Pt=-100;

    float bosV2j2Pt=-100;
    float bosV2j1Pt=-100;

    float evt_MTVV=-100;
    float evt_MVV=-100;
    float evt_DRV2=-100;
    float evt_DphiVV=-100;
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
    float evt_zepVB1=-100;
    float evt_zepVB2=-100;
    float evt_zepV2=-100;
    float evt_cenEta=-100;
    float evt_cenEtaRes=-100;
    float evt_zepVV=-100;
    float evt_zepV2j1=-100;
    float evt_zepV2j2=-100;
    float evt_DRV1j=-100;
    float evt_DRV2j=-100;
    float evt_FW2=-100;

    float evt_bosV1mass=-1;
    float evt_bosV1Eta=-100;
    float evt_bosV1discr=-1;
    float evt_bosV1tdiscr=-1;
    float evt_bosV1bdiscr=-1;
    float evt_bosV1unc = 0;
    float evt_bosV2mass=-1;
    float evt_bosV2Eta=-100;
    float evt_bosV2discr=-1;
    float evt_bosV2tdiscr=-1;
    float evt_bosV2bdiscr=-1;
    float evt_bosV2discr2nd=-1;
    float evt_bosV2unc = 0;
    float evt_chi2_= -1;
    float evt_maxDnn = 0.;
    float evt_2ndmaxDnn = 0.;
    float evt_v_score = 0.;
    float evt_maxkeras = -999.;
    int    index_f1 = -1;
    int    index_f2 = -1;
    int    index_v1 = -1;
    int    index_v2 = -1;


    bool evt_genmatch = 0;
    float evt_j1unc = 0;
    float evt_j2unc = 0;
    float evt_j1QGL = -1;
    float evt_j2QGL = -1;

    //VV-rest Frame
    float evt_VVcenEta = -100;
    float evt_VVDRV1j = -100;
    float evt_VVnormPTVVjj = 0;

    TVector3 boostVV;


    float BDTnoBnoMET = -100;
    float BDTwithMET = -100;
    float BDTbtag = -100;
    float MultiBDTwithMETewk = -100;
    float MultiBDTwithMETqcd = -100;
    float MultiBDTwithMETbkg = -100;
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
    void ReadTmva(Event*e);

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
