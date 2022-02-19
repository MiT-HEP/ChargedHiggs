#include "interface/AnalysisVBShad.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TPython.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"

//2018  https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18#Supported_Algorithms_and_Operati
//2017  https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17#Supported_Algorithms_and_Operati
//2016pre  https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
//2016post https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16postVFP
//#DEEP-CSV
#define DEEP_B_LOOSE ((year==12016)?0.2027:(year==2016)?0.1918:(year==2017)?0.1355:0.1208)
#define DEEP_B_MEDIUM ((year==12016)?0.6001:(year==2016)?0.5847:(year==2017)?0.4506:0.4148)
#define DEEP_B_TIGHT ((year==12016)?0.8819:(year==2016)?0.8767:(year==2017)?0.7738:.7665)
//#DeepJet
#define DEEP_Jet_LOOSE ((year==12016)?0.0508:(year==2016)?0.0480:(year==2017)?0.0532:0.0490)
#define DEEP_Jet_MEDIUM ((year==12016)?0.2598:(year==2016)?0.2489:(year==2017)?0.3040:0.2783)
#define DEEP_Jet_TIGHT ((year==12016)?0.6502:(year==2016)?0.6377:(year==2017)?0.7476:0.7100)

// CvsL
#define DEEP_C_LOOSE ((year==2016 or year==12016)?1.:(year==2017)?0.04:0.064)
#define DEEP_C_MEDIUM ((year==2016 or year==12016)?1.:(year==2017)?0.144:0.153)
#define DEEP_C_TIGHT ((year==2016 or year==12016)?1.:(year==2017)?0.73:0.405)

//https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepAK8Tagging2018WPsSFs#Working_Points
#define DEEP_AK8_W_MD_50 ((year==2016 or year==12016)?0.274:(year==2017)?0.258:0.245)
#define DEEP_AK8_W_MD_25 ((year==2016 or year==12016)?0.506:(year==2017)?0.506:0.479)
#define DEEP_AK8_W_MD_1 ((year==2016 or year==12016)?0.731:(year==2017)?0.739:0.704)
#define DEEP_AK8_W_MD_05 ((year==2016 or year==12016)?0.828:(year==2017)?0.838:0.806)

//slide4 of https://indico.cern.ch/event/853828/contributions/3723593/attachments/1977626/3292045/lg-btv-deepak8v2-sf-20200127.pdf
#define DEEP_AK8_ZHbb_MD_50 ((year==2016 or year==12016)?0.6795:(year==2017)?0.5845:0.5165)
#define DEEP_AK8_ZHbb_MD_25 ((year==2016 or year==12016)?0.8945:(year==2017)?0.8695:0.8365)
//#define DEEP_AK8_ZHbb_MD_25 0.80
#define DEEP_AK8_ZHbb_MD_1 0.97

// from Loukas
//#define ParticleNet_AK8_ZHbb_MD_loose 0.90
//#define ParticleNet_AK8_ZHbb_MD_medium 0.94
//#define ParticleNet_AK8_ZHbb_MD_tight 0.985

// from Miao
#define ParticleNet_loose 0.90
//#define ParticleNet_MEDIUM 0.94
#define ParticleNet_MEDIUM 0.75 //0.80
#define ParticleNet_tight 0.98
#define ParticleNet_Xbb 0.96
#define ParticleNet_Xcc 0.96

void VBShadAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10); 
    l->SetIsoRelCut(0.25); // check the 0.25-->0.4 as done for chargedHIggs
    l->SetEtaCut(2.4);
    //l->SetTightCut(true);
    //l->SetMediumCut(false);
    l->SetVetoCut();
    l->SetTightCut(false);
    l->SetLooseCut(true);
    l->SetMediumCut(false);
    l->SetTrackerMuonCut(false);
    l->SetGlobalMuonCut(false);
}

void VBShadAnalysis::SetJetCuts(Jet *j) { 
    j->SetBCut(-100); //L=0.5426 , M=  0.8484, T0.9535
    //    j->SetDeepBCut(DEEP_B_LOOSE);
    // might want to switch to the loose for the noBnoMET
    //    j->SetDeepBCut(DEEP_B_MEDIUM);
    j->SetDeepFlavBCut(DEEP_Jet_MEDIUM);
    //    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535
    //    j->SetBCut(0.5426); //L=0.5426 , M=  0.8484, T0.9535
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(20);
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100); // 100=loose
}

void VBShadAnalysis::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(18); // as in nanoAOD
    t->SetEtaCut(2.3);
    t->SetMuRej(false);
    t->SetEleRej(false);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // NOW byVLooseIsolationMVArun2v1DBoldDMwLT, BEFORE LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(-1);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(-1);
}

void VBShadAnalysis::SetPhotonCuts(Photon *p){ // Already selecting Medium ided photons
    // I'm removing jets that look really likes photons.
    p->SetIsoCut(-1);
    p->SetIsoRelCut(0.15);
    p->SetPtCut(15);
    p->SetEtaCut(2.5);
    // TODO CHeck for nano: electronVeto (ok) and medium ID
}

void VBShadAnalysis::SetFatJetCuts(FatJet *f){
    f->SetEtaCut(2.5);
    f->SetPtCut(300);
    f->SetSDMassCut(30);
}

bool VBShadAnalysis::checkSignalLabel(string l) {

    if(//NANOV9 only
       l.find("ZNUNUWPMJJjj_EWK") !=string::npos  ||
       l.find("ZNUNUWPMJJjj_QCD") !=string::npos  ||
       l.find("ZNUNUWPMJJjj_EWK_QCD") !=string::npos  ||
       l.find("ZNUNUZJJjj_EWK") !=string::npos  ||
       l.find("ZNUNUZJJjj_QCD") !=string::npos  ||
       l.find("ZNUNUZJJjj_EWK_QCD") !=string::npos  ||
       l.find("ZJJNOBWPMJJjj_EWK_LO") !=string::npos  ||
       l.find("ZJJNOBWPMJJjj_QCD_LO") !=string::npos  ||
       l.find("ZJJNOBWPMJJjj_EWK_QCD_LO") !=string::npos  ||
       //NANOV8 + somev9
       l.find("ZNuNuWPMJJjj_EWK") !=string::npos  ||
       l.find("ZNuNuWPMJJjj_QCD") !=string::npos  ||
       l.find("ZNuNuWPMJJjj_EWK_QCD") !=string::npos  ||
       l.find("ZBBWPMJJjj_EWK") !=string::npos  ||
       l.find("ZBBWPMJJjj_QCD") !=string::npos  ||
       l.find("ZBBWPMJJjj_EWK_QCD") !=string::npos  ||
       l.find("ZJJZJJjj_EWK_LO") !=string::npos  ||
       l.find("ZJJZJJjj_EWK_QCD_LO") !=string::npos  ||
       l.find("ZJJZJJjj_QCD_LO") !=string::npos  ||
       l.find("WPMJJWPMJJjj_EWK") !=string::npos  ||
       l.find("WPMJJWPMJJjj_QCD") !=string::npos  ||
       l.find("WPMJJWPMJJjj_EWK_QCD") !=string::npos  ||
       l.find("WPJJWMJJjj_EWK") !=string::npos  ||
       l.find("WPJJWMJJjj_QCD") !=string::npos  ||
       l.find("WPJJWMJJjj_EWK_QCD") !=string::npos  ||
       //
       l.find("WPHADWMLEPjj_EWK_LO") !=string::npos  ||
       l.find("WPLEPWMHADjj_EWK_LO") !=string::npos  ||
       l.find("WPLEPWPHADjj_EWK_LO") !=string::npos  ||
       l.find("WMLEPWMHADjj_EWK_LO") !=string::npos  ||
       l.find("WPLEPZHADjj_EWK_LO") !=string::npos  ||
       l.find("WMLEPZHADjj_EWK_LO") !=string::npos  ||
       l.find("WPHADWMLEPjj_QCD_LO") !=string::npos  ||
       l.find("WPLEPWMHADjj_QCD_LO") !=string::npos  ||
       l.find("WPLEPWPHADjj_QCD_LO") !=string::npos  ||
       l.find("WMLEPWMHADjj_QCD_LO") !=string::npos  ||
       l.find("WPLEPZHADjj_QCD_LO") !=string::npos  ||
       l.find("WMLEPZHADjj_QCD_LO") !=string::npos  ||
       // OLD labels
       l.find("ZnnZhadJJ_EWK") !=string::npos  ||
       l.find("ZbbZhadJJ_EWK")!=string::npos  ||
       l.find("WPhadWPhadJJ_EWK") !=string::npos ||
       l.find("ZNuNuWPMJJjj_EWK") !=string::npos ||
       l.find("ZNuNuWPMJJjj_QCD") !=string::npos ||
       l.find("ZbbWpmJJ_EWK") !=string::npos ||
       l.find("ZbbWpmJJ_QCD") !=string::npos ||
       l.find("WWjj_SS_ll") !=string::npos ||
       l.find("WWjj_SS_lt") !=string::npos ||
       l.find("WWjj_SS_tt") !=string::npos ||
       //
       l.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1000") !=string::npos ||
       l.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1500") !=string::npos ||
       l.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M2000") !=string::npos ||
       l.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1000") !=string::npos ||
       l.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1500") !=string::npos ||
       l.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M2000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M3000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_semilep_M2000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_semilep_M1000") !=string::npos
       ) {
        return true;
    } return false;
}

void VBShadAnalysis::BookHisto(string l, string category)
{

    Book ("VBShadAnalysis/DRV1j"+category+"_"+l, " ; #Delta #R (V1,jet) ; Events", 125,0,6.28);
    Book ("VBShadAnalysis/DRV2j"+category+"_"+l, " ; #Delta #R (V2,jet) ; Events", 125,0,6.28);
    Book ("VBShadAnalysis/ZepBosBVar"+category+"_"+l, " ; |#eta_{V} - 0.5*(#eta_{j1} + #eta_{j2})| / #Delta #eta(jj) ; Events", 250,0,2);
    Book ("VBShadAnalysis/ZepBosVVar"+category+"_"+l, " ; |#eta_{VV} - 0.5*(#eta_{j1} + #eta_{j2})| / #Delta #eta(jj) ; Events", 250,0,2);
    Book ("VBShadAnalysis/BOSON/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
    Book ("VBShadAnalysis/FW2"+category+"_"+l, " ; FW2 ; Events", 100,0,1.);

    AddFinalHisto("VBShadAnalysis/MVV"+category+"_"+l);
    Book ("VBShadAnalysis/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 200,0,5000); // should be 200,0,5000 -- 25 GeV bin

    if(checkSignalLabel(l)) {
        Book ("VBShadAnalysis/MVV"+category+"_match_"+l, "MVV (match) ; MVV [GeV]; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
        Book ("VBShadAnalysis/MVV"+category+"_unMatch_"+l, "MVV (unmatch) ; MVV [GeV]; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
        Book ("VBShadAnalysis/MVV"+category+"_central_"+l, "MVV (central) ; MVV [GeV]; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
        Book ("VBShadAnalysis/MVV"+category+"_CE_EE_"+l, "MVV (CE+EE) ; MVV [GeV]; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
    }
    Book ("VBShadAnalysis/MVV"+category+"_low_"+l, "MVV (low Deta JJ) ; MVV_{reco}; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin
    Book ("VBShadAnalysis/MVV"+category+"_high_"+l, "MVV (high Deta JJ) ; MVV_{reco}; Events", 120,0,3000); // should be 120,0,3000 -- 25 GeV bin

    if(doTMVA) {

        AddFinalHisto("VBShadAnalysis/BDTnoBnoMET"+category+"_"+l);
        AddFinalHisto("VBShadAnalysis/BDTwithMET"+category+"_"+l);
        if(!doBAnalysis and !doMETAnalysis) Book ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+l, "BDT noBnoMET ; BDT noBnoMET; Events", 200,-1.,1.);

        if(doBAnalysis or doBAntiAnalysis) {
            Book ("VBShadAnalysis/BDTbtag"+category+"_"+l, "BDT with Btag ; BDT with Btag; Events", 200,-1.,1.);
            if(category.find("RBtag")   !=string::npos ) {
                Book ("VBShadAnalysis/DNNMultiRBtagEWK"+category+"_"+l, "DNN MultiClass RBtag (response for ZZ); DNN Multi RBtag [GeV]; Events", 200,0.,1.);
                Book ("VBShadAnalysis/DNNMultiRBtagQCD"+category+"_"+l, "DNN MultiClass RBtag (response for WZ); DNN Multi RBtag [GeV]; Events", 200,0.,1.);
                Book ("VBShadAnalysis/DNNMultiRBtagBKG"+category+"_"+l, "DNN MultiClass RBtag (response for BKG); DNN Multi RBtag [GeV]; Events", 200,0.,1.);
            }
        }


        if(doMETAnalysis or doMETAntiAnalysis) {
            //            AddFinalHisto("VBShadAnalysis/BDTMultiEWKchi2"+category+"_"+l);
            //            AddFinalHisto("VBShadAnalysis/BDTMultiQCDchi2"+category+"_"+l);
            //            AddFinalHisto("VBShadAnalysis/BDTMultiBKGchi2"+category+"_"+l);

            AddFinalHisto("VBShadAnalysis/DNNMultiEWKdnn"+category+"_"+l);
            AddFinalHisto("VBShadAnalysis/DNNMultiQCDdnn"+category+"_"+l);
            AddFinalHisto("VBShadAnalysis/DNNMultiBKGdnn"+category+"_"+l);


            Book ("VBShadAnalysis/BDTwithMET"+category+"_"+l, "DNN withMET ; DNNwithMET; Events", 100,0.,1.);

            Book ("VBShadAnalysis/BDTMultiwithMETzz"+category+"_"+l, "BDT Multi withMET (response for ZZ); BDT Multi withMET [GeV]; Events", 200,0.,1.);
            Book ("VBShadAnalysis/BDTMultiwithMETwz"+category+"_"+l, "BDT Multi withMET (response for WZ); BDT Multi withMET [GeV]; Events", 200,0.,1.);
            Book ("VBShadAnalysis/BDTMultiwithMETbkg"+category+"_"+l, "BDT Multi withMET (response for BKG); BDT Multi withMET [GeV]; Events", 200,0.,1.);

            //            Book ("VBShadAnalysis/BDTMultiEWKchi2"+category+"_"+l, "BDT Multi withMET (response for WZ ewk); BDT Multi ewk vs qcd [GeV]; Events", 200,0.,1.);
            //            Book ("VBShadAnalysis/BDTMultiQCDchi2"+category+"_"+l, "BDT Multi withMET (response for WZ qcd); BDT Multi ewk vs qcd [GeV]; Events", 200,0.,1.);
            //            Book ("VBShadAnalysis/BDTMultiBKGchi2"+category+"_"+l, "BDT Multi withMET (response for BKG ); BDT Multi ewk vs qcd [GeV]; Events", 200,0.,1.);

            Book ("VBShadAnalysis/DNNMultiEWKdnn"+category+"_"+l, "DNN Multi withMET (response for WZ ewk); DNN Multi ewk vs qcd [GeV]; Events", 200,0.,1.);
            Book ("VBShadAnalysis/DNNMultiQCDdnn"+category+"_"+l, "DNN Multi withMET (response for WZ qcd); DNN Multi ewk vs qcd [GeV]; Events", 200,0.,1.);
            Book ("VBShadAnalysis/DNNMultiBKGdnn"+category+"_"+l, "DNN Multi withMET (response for BKG ); DNN Multi ewk vs qcd [GeV]; Events", 200,0.,1.);

        }
    }

    if(checkSignalLabel(l)) Book ("VBShadAnalysis/MVVres"+category+"_"+l, "MVVres ; ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100, -5., 5.);

    Book ("VBShadAnalysis/FWJETS/Mjj"+category+"_"+l, "Mjj ; M(j,j) [GeV]; Events", 35,0,3500.); // 140,0,3500.
    Book ("VBShadAnalysis/FWJETS/Dphijj"+category+"_"+l, "Dphi jj ; #Delta#Phi(j,j) ; Events", 100,0,6.28);

    AddFinalHisto("VBShadAnalysis/Mjj"+category+"_"+l);
    Book ("VBShadAnalysis/Mjj"+category+"_"+l, "Mjj ; M(j,j) [GeV]; Events", 200,0,5000.);

    // more plots for data/MC
    Book ("VBShadAnalysis/Detajj"+category+"_"+l, "Deta jj ; #Delta#eta(j,j) ; Events", 100,0,10.);

    Book ("VBShadAnalysis/normPTVVjj"+category+"_"+l, "normPTVVjj; |#vec{p_{T}^{tot}}| / #Sigma p_{T} ; Events", 100,0,1.);
    Book ("VBShadAnalysis/Dphimin"+category+"_"+l, "Dphimin;  min#Delta#Phi(Met,j)", 100,0,6.28);
    Book ("VBShadAnalysis/Met"+category+"_"+l, "Met; Met [GeV]; Events", 100,0,1000);
    Book ("VBShadAnalysis/MetPhi"+category+"_"+l, "Met #phi; Met #phi; Events", 100,-TMath::Pi(),TMath::Pi());
    Book ("VBShadAnalysis/PTVV"+category+"_"+l, "PTVV; PTVV [GeV]; Events", 100,0,1000);
    Book ("VBShadAnalysis/Centrality"+category+"_"+l, "Centrality; Centrality; Events", 100,0,5);

    // fwd jets
    Book ("VBShadAnalysis/pT1_Jet"+category+"_"+l, "pT_Jet; p_{T} [GeV] (leading); Events", 160,0,1600);
    Book ("VBShadAnalysis/pT2_Jet"+category+"_"+l, "pT_Jet; p_{T} [GeV] (subleading) ; Events", 100,0,1000);

    Book ("VBShadAnalysis/eta1_Jet"+category+"_"+l, "eta_Jet; #eta (leading); Events", 100,0.,5.);
    Book ("VBShadAnalysis/eta2_Jet"+category+"_"+l, "eta_Jet; #eta (subleading) ; Events", 100,0.,5.);

    Book ("VBShadAnalysis/qgl1_Jet"+category+"_"+l, "QGL1_Jet; QGL j1(leading); Events", 100,0.,1.);
    Book ("VBShadAnalysis/qgl2_Jet"+category+"_"+l, "QGL2_Jet; QGL j2(subleading) ; Events", 100,0.,1.);
    //    Book ("VBShadAnalysis/nemf1_Jet"+category+"_"+l, "nemf1_Jet; nemf j1 (leading); Events", 100,0.,1.);
    //    Book ("VBShadAnalysis/nemf2_Jet"+category+"_"+l, "nemf2_Jet; nemf j2 (subleading) ; Events", 100,0.,1.);
    //    Book ("VBShadAnalysis/nhf1_Jet"+category+"_"+l, "nhf1_Jet; nhf j1 (leading); Events", 100,0.,1.);
    //    Book ("VBShadAnalysis/nhf2_Jet"+category+"_"+l, "nhf2_Jet; nhf j2 (subleading) ; Events", 100,0.,1.);

    // boosted jets
    Book ("VBShadAnalysis/pTV1_Jet"+category+"_"+l, "pT_V1_Jet; p_{T} [GeV] (leading); Events", 160,0,1600);
    Book ("VBShadAnalysis/pTV2_Jet"+category+"_"+l, "pT_V2_Jet; p_{T} [GeV] (subleading) ; Events", 160,0,1600);

    Book ("VBShadAnalysis/massV1"+category+"_"+l, "VMass_leading; MassV [GeV]; Events", 200,0,200);
    Book ("VBShadAnalysis/massV2"+category+"_"+l, "VMass_trailing; MassV [GeV]; Events", 200,0,200);

    Book ("VBShadAnalysis/bosV2discr"+category+"_"+l, "V2tagger; V2Tagger score; Events", 100,0,1);
    Book ("VBShadAnalysis/bosV2discr2nd"+category+"_"+l, "V2tagger; V2Tagger 2ndscore; Events", 100,0,1);

    Book ("VBShadAnalysis/etaV1_Jet"+category+"_"+l, "eta_V1_Jet; #eta (leading); Events", 100,0.,5.);
    Book ("VBShadAnalysis/etaV2_Jet"+category+"_"+l, "eta_V2_Jet; #eta (subleading) ; Events", 100,0.,5.);

    Book ("VBShadAnalysis/phiV1_Jet"+category+"_"+l, "#phi V1; #phi V1; Events", 100,-TMath::Pi(),TMath::Pi());
    Book ("VBShadAnalysis/phiV2_Jet"+category+"_"+l, "#phi V2; #phi V2; Events", 100,-TMath::Pi(),TMath::Pi());
    /*
    // more
    Book ("VBShadAnalysis/pT_FatJet"+category+"_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
    Book ("VBShadAnalysis/WvsQCD_FatJet"+category+"_"+l, "WvsQCD_FatJet; WvsQCD; Events", 50,0,1.0);
    Book ("VBShadAnalysis/ZvsQCD_FatJet"+category+"_"+l, "ZvsQCD_FatJet; ZvsQCD; Events", 50,0,1.0);
    Book ("VBShadAnalysis/SDMass_FatJet"+category+"_"+l, "SDMass_FatJet; SDMass [GeV]; Events", 100,0,200.);
    //    Book ("VBShadAnalysis/SDMass_FatZbbJet"+category+"_"+l, "SDMass_FatZbbJet; SDMass [GeV]; Events", 100,0,200.);

    */

}


void VBShadAnalysis::AddSpectator( string name, char type, int r){

    cout<<"[TmvaAnalysis]::[AddSpectator]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    cout<<"[TmvaAnalysis]::[DEBUG] AddSpectator of type F to reader "<<r <<" and name "<<name<<endl;
    if ( type == 'I') readers_[r] -> AddSpectator(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') readers_[r] -> AddSpectator(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddSpectator]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }

}

void VBShadAnalysis::AddVariable( string name, char type, TMVA::Reader* i_readers){
    cout<<"[TmvaAnalysis]::[AddVariable]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    cout<<"[TmvaAnalysis]::[DEBUG] AddVariables of type F to reader "<< i_readers <<" and name "<<name<<endl;
    //    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    //    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    if ( type == 'I') i_readers -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') i_readers -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddVariable]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }
}

void VBShadAnalysis::ReadTmva(Event*e){

    bdt.clear();
    bdt_multi.clear();

    SetVariable("j1QGL",evt_j1QGL); //0
    SetVariable("j2QGL",evt_j2QGL); //1

    SetVariable("varMjj",evt_Mjj); //2
    SetVariable("varDetajj",evt_Detajj); //3
    SetVariable("varJet1Pt",evt_Jet1Pt); //4
    SetVariable("abs(varJet1Eta)", abs(evt_Jet1Eta)); //5
    SetVariable("varJet2Pt",evt_Jet2Pt); //6
    SetVariable("varMVV",evt_MVV); //7
    SetVariable("varMTVV",evt_MTVV); //8
    SetVariable("varnormPTVVjj",evt_normPTVVjj); //9

    SetVariable("varPTVV",evt_PTVV); //10
    SetVariable("varPTV2",evt_PTV2); //11
    SetVariable("varPTV1",evt_PTV1); //12

    SetVariable("varCen",evt_cenEta); //13
    SetVariable("varzepVB",evt_zepVB); //14
    SetVariable("varzepVV",evt_zepVV); //15   
    SetVariable("bosV2chi2",evt_chi2_); //16
    SetVariable("bosV2mass",evt_bosV2mass); //17
    SetVariable("varFW2j",evt_FW2); //18
    SetVariable("varmtop",evt_mtop); //19

    SetVariable("varDetaVV",evt_DetaVV);//20 
    SetVariable("varDRVj",evt_DRV1j);//21
    SetVariable("min(varDRV1j,varDRV2j)",std::min(evt_DRV1j,evt_DRV2j)); //22

    //for RBtag multi-DNN in particular
    SetVariable("varCenRes",evt_cenEtaRes); //23
    SetVariable("varnormPTVVjj",evt_normPTVVjj);//24
    SetVariable("bosV1mass",evt_bosV1mass); //25
    SetVariable("bosV2discr",evt_bosV2discr); //26
    SetVariable("bosV2discr2nd",evt_bosV2discr2nd);//27
    SetVariable("varDphiVV",evt_DphiVV);//28
    SetVariable("NBJets",e->Bjets()); //29

    //    vector<float> bdt;
    /*
    ---- all binary classes here -----
    0, 1, 2 - BB    -- BDT
    3, 4, 5 - BMET  -- DNN
    6, 7, 8 - BBtag -- BDT (athough not used now)
    9       - RBTag -- BDT (athough not used now)
    10,11,12- RMET  -- DNN
    -----------------------------------
    */
    for(unsigned i =0 ;i< readers_.size() ; ++i) {
        if(i==0 || i==1 || i==2) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSHad") );
        if(i==3 || i==4 || i==5) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSMet") );
        if(i==6 || i==7 || i==8) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSBBtag") );
        if(i==9 ) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSRBtag") );
        if(i==10 || i==11 || i==12) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSRMET") );
    }

    /*
    ---- all multi classes here -----
    0     : all 0,1,2                    - RMET  -- BDT W vs Z from Will
    1,2,3 : all 3,4,5/6,7,8/9,10,11      - RMET  -- BDT EWK vs QCD with dnn tagger (athough not used now, switch to DNN)
    4     : all 12,13,14                 - RMET  -- DNN EWK vs QCD with dnn tagger from Ava
    5     : all 15,16,17                 - RBTag -- DNN EWK vs QCD with dnn tagger from Ava (athough not used now)
    -----------------------------------
    */

    int NClass=3;
    for(unsigned i =0 ;i< readers_multi_.size() ; ++i) {
        for(unsigned j =0 ;j< NClass ; ++j) {

            if(doMETAnalysis or doMETAntiAnalysis) {
                if(i==0) bdt_multi.push_back( (readers_multi_[i]->EvaluateMulticlass("BDTG"))[ j]); //W vs Z (old chi2)
                if(i==1 || i==2 || i==3) bdt_multi.push_back( (readers_multi_[i]->EvaluateMulticlass("BDTG"))[ j]); // EWK vs QCD (new chi2)
                //                std::cout << " EvaluateMulticlass i = " << i << " j = " << j << " index = " << bdt_multi[NClass*i + j] << std::endl;
                if(i==4) bdt_multi.push_back( (readers_multi_[i]->EvaluateMulticlass("DNN_RMETmulti"))[ j]); //DNN RMET -- dnn tagger
            }

            if(doBAnalysis or doBAntiAnalysis) {
                if(i==5) bdt_multi.push_back( (readers_multi_[i]->EvaluateMulticlass("DNN_RBtagmulti"))[ j]); //W vs Z (DNN) RBTAG
            }
        } // loop nClass
    } //loop readers multi

}

void VBShadAnalysis::InitTmva() {

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader: " << weights.size() <<endl;


    if(doTMVA) {

        for( size_t i=0;i<weights.size() ;++i)
            readers_ . push_back( new TMVA::Reader() );

        cout << "---------------------------------------------" << endl;
        cout << " GOING TO BDTG - HAD " << endl;

        for (int i=0; i<3; i++) {
            AddVariable("j1QGL",'F',readers_[i]);  //0
            AddVariable("j2QGL" ,'F',readers_[i]); //1
            AddVariable("varMjj",'F',readers_[i]); //2
            AddVariable("varJet1Pt",'F',readers_[i]); //4
            AddVariable("varJet2Pt",'F',readers_[i]); //6
            AddVariable("varPTV1",'F',readers_[i]); //12
            AddVariable("varCen",'F',readers_[i]); //13
            AddVariable("varzepVV",'F',readers_[i]); //15
            AddVariable("min(varDRV1j,varDRV2j)",'F',readers_[i]); //22
            AddVariable("varnormPTVVjj",'F',readers_[i]); //9
        }

        cout << "---------------------------------------------" << endl;
        cout << " GOING TO BDTG - MET " << endl;

        for (int i=3; i<6; i++) {
            AddVariable("j1QGL",'F',readers_[i]);  //0
            AddVariable("j2QGL" ,'F',readers_[i]); //1
            AddVariable("varMjj",'F',readers_[i]); //2
            AddVariable("varDetajj",'F',readers_[i]); //3
            AddVariable("varJet1Pt",'F',readers_[i]); //4
            AddVariable("varJet2Pt",'F',readers_[i]); //6
            AddVariable("varMTVV",'F',readers_[i]); //8
            AddVariable("varCen",'F',readers_[i]); //13
            AddVariable("varnormPTVVjj",'F',readers_[i]); //9
        }


        //BBtag-Nano
        cout << "---------------------------------------------" << endl;
        cout << " GOING TO BBtag-Nano   " << endl;

        for (int i=6; i<9; i++) {
            AddVariable("varMjj",'F',readers_[i]); //0
            AddVariable("varDetajj",'F',readers_[i]); //1
            AddVariable("varJet2Pt",'F',readers_[i]); //2
            AddVariable("varMVV",'F',readers_[i]); //3
            AddVariable("varDetaVV",'F',readers_[i]); //14
            AddVariable("varzepVB",'F',readers_[i]); //5
            AddVariable("varDRVj",'F',readers_[i]); //15
            AddVariable("varnormPTVVjj",'F',readers_[i]); //6
            //AddVariable("bosV2mass",'F',readers_[i]); //8
            AddVariable("varFW2j",'F',readers_[i]); //9
        }


        // RBTAG
        cout << "---------------------------------------------" << endl;
        cout << " GOING TO RBtag   " << endl;
        //old training

        for (int i=9; i<10; i++) {
            AddVariable("varMjj",'F',readers_[i]); //0
            AddVariable("varJet2Pt",'F',readers_[i]); //2
            //        AddVariable("varDetajj",'F',i); //1
            //        AddVariable("varMVV",'F',i); //3
            AddVariable("varPTV2",'F',readers_[i]); //4
            AddVariable("varCen",'F',readers_[i]); //5
            //        AddVariable("varzepVB",'F',i); //6
            AddVariable("varnormPTVVjj",'F',readers_[i]); //7
            AddVariable("varmtop",'F',readers_[i]); //8
            AddVariable("bosV2mass",'F',readers_[i]); //9
            AddVariable("bosV2chi2",'F',readers_[i]); //10

            AddSpectator("mc",'F',i); AddSpectator("weight",'F',i); AddSpectator("ana_category",'F',i); AddSpectator("bosGen",'F',i); AddSpectator("evt",'F',i);
        }

        // RMET
        cout << "---------------------------------------------" << endl;
        cout << " GOING TO RMET-Nano   " << endl;
        //new binary class for RMET!

        for (int i=10; i<13; i++) {
            AddVariable("j1QGL",'F',readers_[i]);  //0
            AddVariable("j2QGL" ,'F',readers_[i]); //1
            AddVariable("varMjj",'F',readers_[i]); //2
            AddVariable("varDetajj",'F',readers_[i]); //3
            //AddVariable("varJet1Pt",'F',readers_[i]); //4
            AddVariable("varJet2Pt",'F',readers_[i]); //6
            AddVariable("varMVV",'F',readers_[i]); //8
            AddVariable("varPTV1",'F',readers_[i]); //12
            AddVariable("varCenRes",'F',readers_[i]); //23
            AddVariable("varnormPTVVjj",'F',readers_[i]); //9
            AddVariable("bosV2discr",'F',readers_[i]); //26
        }

        cout << "---------------------------------------------" << endl;

        cout << " GOING loop over weights" << weights.size() << endl;

        // load weights
        for( size_t i=0;i<weights.size() ;++i) {
            cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
            if(i==0 or i==1 or i==2) readers_[i]->BookMVA("BDT_VBSHad",weights[i].c_str());
            if(i==3 or i==4 or i==5) readers_[i]->BookMVA("BDT_VBSMet",weights[i].c_str());
            if(i==6 or i==7 or i==8) readers_[i]->BookMVA("BDT_VBSBBtag",weights[i].c_str());
            if(i==9 ) readers_[i]->BookMVA("BDT_VBSRBtag",weights[i].c_str());
            if(i==10 or i==11 or i==12) readers_[i]->BookMVA("BDT_VBSRMET",weights[i].c_str());
        }
    }

    //$$$$$$$$$
    //$$$$$$$$$ multiclass
    //$$$$$$$$$

    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) {

        for( size_t i=0;i<weights_multi.size() ;++i)
            readers_multi_ . push_back( new TMVA::Reader() );

        cout << "---------------------------------------------" << endl;
        cout << " GOING TO RMet multiclass final " << endl;

        //multiclass RMET - BDT - (WvsZ)
        for (int i=0; i<1; i++) {
            AddVariable("varMjj",'F',readers_multi_[i]); //0
            AddVariable("varDetajj",'F',readers_multi_[i]); //1
            AddVariable("varJet2Pt",'F',readers_multi_[i]); //2
            AddVariable("varMVV",'F',readers_multi_[i]); //3
            AddVariable("varPTV2",'F',readers_multi_[i]); //4
            AddVariable("varzepVB",'F',readers_multi_[i]); //5
            AddVariable("varnormPTVVjj",'F',readers_multi_[i]); //6
            AddVariable("varFW2j",'F',readers_multi_[i]); //7
            AddVariable("bosV2mass",'F',readers_multi_[i]); //8
            AddVariable("bosV2chi2",'F',readers_multi_[i]); //9
        }

        //multiclass RMET _ EWK vs QCD _ WZ chi2
        for (int i=1; i<2; i++) {
            AddVariable("varMjj",'F',readers_multi_[i]); //0
            AddVariable("varDetajj",'F',readers_multi_[i]); //1 (not used with dnn cut)
            AddVariable("varJet2Eta",'F',readers_multi_[i]); //2
            AddVariable("varPTV2",'F',readers_multi_[i]); //3 (PTV1 in with the dnn cut)
            AddVariable("varzepVB",'F',readers_multi_[i]); //4
            AddVariable("varnormPTVVjj",'F',readers_multi_[i]); //5
            AddVariable("varFW2j",'F',readers_multi_[i]); //6
            AddVariable("bosV2mass",'F',readers_multi_[i]); //7
            AddVariable("bosV2chi2",'F',readers_multi_[i]); //8
            AddVariable("NJets",'F',readers_multi_[i]); //9
        }
        //multiclass RMET _ EWK vs QCD _ WZ dnn(cut)
        for (int i=2; i<4; i++) {

            AddVariable("varMjj",'F',readers_multi_[i]); //0
            AddVariable("varJet2Eta",'F',readers_multi_[i]); //1
            AddVariable("varJet2Pt",'F',readers_multi_[i]); //2 (not used with chi2)
            AddVariable("varPTV1",'F',readers_multi_[i]); //3
            AddVariable("varzepVB",'F',readers_multi_[i]); //4
            AddVariable("varnormPTVVjj",'F',readers_multi_[i]); //5
            AddVariable("varFW2j",'F',readers_multi_[i]); //6
            AddVariable("bosV2mass",'F',readers_multi_[i]); //7
            AddVariable("bosV2discr",'F',readers_multi_[i]); //8
            AddVariable("NJets",'F',readers_multi_[i]); //9
        }

        
        //multiclass RMET - DNN
        for (int i=4; i<5; i++) {
            AddVariable("varMjj",'F',readers_multi_[i]); //2
            AddVariable("varDphiVV",'F',readers_multi_[i]); //28
            AddVariable("varMVV",'F',readers_multi_[i]); //7
            AddVariable("varzepVB",'F',readers_multi_[i]); //14
            AddVariable("bosV2discr",'F',readers_multi_[i]); //26
            AddVariable("j2QGL",'F',readers_multi_[i]); //1
        }
        


        for( size_t i=0;i<weights_multi.size() ;++i) {
            if(i==0) readers_multi_[i]->BookMVA("BDTG",weights_multi[i].c_str());
            if(i==1 || i==2 || i==3) readers_multi_[i]->BookMVA("BDTG",weights_multi[i].c_str());
            if(i==4) readers_multi_[i]->BookMVA("DNN_RMETmulti",weights_multi[i].c_str());
        }
    }


    if(doTMVA and (doBAnalysis or doBAntiAnalysis)) {

        for( size_t i=0;i<weights_multi.size() ;++i)
            readers_multi_ . push_back( new TMVA::Reader() );

        cout << "---------------------------------------------" << endl;
        cout << " GOING TO RBtag multiclass final " << endl;

        //multiclass RBtag - DNN
        for (int i=5; i<6; i++) {
            AddVariable("varMjj",'F',readers_multi_[i]); //0
            AddVariable("varJet2Pt",'F',readers_multi_[i]); //1
            AddVariable("varPTV1",'F',readers_multi_[i]); //2
            AddVariable("varnormPTVVjj",'F',readers_multi_[i]); //3
            AddVariable("bosV2discr",'F',readers_multi_[i]); //4
            AddVariable("NBJets",'F',readers_multi_[i]); //5
        }

        for( size_t i=0;i<weights_multi.size() ;++i) {
            if(i==5) readers_multi_[i]->BookMVA("DNN_RBtagmulti",weights_multi[i].c_str());
        }
    }


    //$$$$$$$$$
    //$$$$$$$$$ ResolvedTagger
    //$$$$$$$$$

    if(doResTagTMVA) {

        cout << "---------------------------------------------" << endl;
        cout << " GOING TO Resolve Tagger multiclass final " << endl;

        for( size_t i=0;i<weights_dnn.size() ;++i)
            readers_dnn_ . push_back( new TMVA::Reader() );

        //resTagger TMVA
        //4 jet comb binary 
        for (int i=0; i<2; i++) {

            AddVariable("j1_pT", 'F',readers_dnn_[i]);
            AddVariable("abs(j1_Eta)",'F',readers_dnn_[i]);
            //AddVariable("j1_M",'F',readers_dnn_[i]);
            //AddVariable("j1_QGL",'F',readers_dnn_[i]);

            AddVariable("j2_pT", 'F',readers_dnn_[i]);
            AddVariable("abs(j2_Eta)", 'F',readers_dnn_[i]);
            //AddVariable("j2_M",'F',readers_dnn_[i]);
            //AddVariable("j2_QGL",'F',readers_dnn_[i]);
            //AddVariable("j2_PUDiscr",'F',readers_dnn_[i]);

            AddVariable("j3_pT", 'F',readers_dnn_[i]);
            AddVariable("abs(j3_Eta)", 'F',readers_dnn_[i]);
            //AddVariable("j3_QGL",'F',readers_dnn_[i]);

            AddVariable("j4_pT", 'F',readers_dnn_[i]);
            AddVariable("abs(j4_Eta)", 'F',readers_dnn_[i]);
            //AddVariable("j4_QGL",'F',readers_dnn_[i]);

            AddVariable("abs(j3_Eta-(j1_Eta+j2_Eta)/2.)/abs(DEta_f12)", 'F',readers_dnn_[i]);
            AddVariable("abs(j4_Eta-(j1_Eta+j2_Eta)/2.)/abs(DEta_f12)", 'F',readers_dnn_[i]);
            AddVariable("min(min(j3_Eta,j4_Eta)-min(j1_Eta,j2_Eta),max(j1_Eta,j2_Eta)-max(j3_Eta,j4_Eta))", 'F',readers_dnn_[i]);


            //AddVariable("abs(DEta_f12)", 'F',readers_dnn_[i]);
            AddVariable("abs(DEta_v34)", 'F',readers_dnn_[i]); 
            AddVariable("abs(DPhi_f12)", 'F',readers_dnn_[i]);
            AddVariable("abs(DPhi_v34)", 'F',readers_dnn_[i]);
            //AddVariable("DR_f12",   'F',readers_dnn_[i]);
            //AddVariable("DR_v34",   'F',readers_dnn_[i]);
            //AddVariable("M_f12",    'F',readers_dnn_[i]);
            //AddVariable("M_v34",    'F',readers_dnn_[i]);

        }

        //2 V jet comb, binary
        for (int i=4; i<6; i++) {

            AddVariable("j3_pT", 'F',readers_dnn_[i]);
            AddVariable("j3_Eta", 'F',readers_dnn_[i]);
            AddVariable("j3_QGL", 'F',readers_dnn_[i]);

            AddVariable("j4_pT", 'F',readers_dnn_[i]);
            AddVariable("j4_Eta", 'F',readers_dnn_[i]);
            AddVariable("j4_QGL", 'F',readers_dnn_[i]);

            AddVariable("DR_v34",   'F',readers_dnn_[i]);
            AddVariable("M_v34",    'F',readers_dnn_[i]);

        }
       

        for( size_t i=0;i<weights_dnn.size() ;++i) {
            //Comb for RBtag and RMet.
            //W and Z should be two separate tagger, the one with higher score is chosen. Currently no Zbb(nn)Zjj samples, put W xml file there.
            if(i==0) readers_dnn_[i]->BookMVA("DNN_ResTag_W",weights_dnn[i].c_str());
            if(i==1) readers_dnn_[i]->BookMVA("DNN_ResTag_Z",weights_dnn[i].c_str());
        
            //Later to be cleaned. If multi-class and V tagger can be completely abandoned.
            //if(i==2) readers_dnn_[i]->BookMVA("DNN_ResTag_RBTA_ml",weights_dnn[i].c_str());
            //if(i==3) readers_dnn_[i]->BookMVA("DNN_ResTag_RMET_ml",weights_dnn[i].c_str());

            if(i==4) readers_dnn_[i]->BookMVA("DNN_VTag_RBTA",weights_dnn[i].c_str());
            if(i==5) readers_dnn_[i]->BookMVA("DNN_VTag_RMET",weights_dnn[i].c_str());

        }
    }

    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;

}


void VBShadAnalysis::InitScikit(){


    py . reset(new TPython);

    py -> Exec("import numpy as np");
    py -> Exec("import keras");
    //
    //py -> Exec("from sklearn.ensemble import RandomForestClassifier");
    //py -> Exec("from sklearn.calibration import CalibratedClassifierCV");
    //py -> Exec("from sklearn.metrics import log_loss");
    //
    //py -> Exec("from sklearn.externals import joblib ");
    if(doBAnalysis)     py -> Exec("kmodel=keras.models.load_model('aux/BDTvbs/model_ResTag0915_RBTA.hd')");
    if(doMETAnalysis)   py -> Exec("kmodel=keras.models.load_model('aux/BDTvbs/model_ResTag0915_RMET.hd')");

    py -> Exec("kmodel.summary()");


    py -> Exec("x = ROOT.vector(float)()");

    x = (std::vector<float>*) py->Eval("x");

    /* no longer used as TPython not Python
    //PyObject* pyx = py->ObjectProxy_FromVoidPtr(&x, "std::vector<float>");

    //PyObject* pymain = PyImport_ImportModule("__main__");

    //PyModule_AddObject(pymain, "x", pyx);

    //Py_DECREF(pymain);
    */

}

void VBShadAnalysis::writeTree(string name, int purp){   //purp = 0: main; purp = 1: restag

        // fill variables for miniTREE
    InitTree(name);

    if(purp == 0){
    Branch(name,"run",'I');
    Branch(name,"lumi",'I');
    Branch(name,"evt",'I');
    Branch(name,"npv",'I');
    Branch(name,"weight",'D');
    Branch(name,"isRealData",'I');
    Branch(name,"xsec",'F');

    Branch(name,"NJets",'I');
    Branch(name,"NBJets",'I');
    Branch(name,"NVetoJets",'I');
    Branch(name,"met_pt",'F');
    Branch(name,"met_phi",'F');

    Branch(name,"mc",'I'); // to distinguish between the different mc
    Branch(name,"ana_category",'I');

    // JJ
    Branch(name,"varMjj",'F');
    Branch(name,"varDetajj",'F');
    Branch(name,"varDphijj",'F');
    Branch(name,"varJet1Eta",'F');
    Branch(name,"varJet1Pt",'F');
    Branch(name,"varJet2Eta",'F');
    Branch(name,"varJet2Pt",'F');

    Branch(name,"j1Unc",'F');
    Branch(name,"j2Unc",'F');
    Branch(name,"j1QGL",'F');
    Branch(name,"j2QGL",'F');


    // VV
    Branch(name,"genMVV",'F');
    Branch(name,"varMVV",'F');
    Branch(name,"varMTVV",'F');
    Branch(name,"varPTVV",'F');
    Branch(name,"varPTV1",'F');
    Branch(name,"varPTV2",'F');
    Branch(name,"varDphiVV",'F');
    Branch(name,"varDetaVV",'F');
    Branch(name,"varPetaVV",'F');
    Branch(name,"varEtaMinV",'F');
    Branch(name,"varEtaMaxV",'F');

    // MIX
    Branch(name,"varCen",'F');
    Branch(name,"varCenRes",'F');
    Branch(name,"varzepVB",'F');
    Branch(name,"varzepVB1",'F');
    Branch(name,"varzepVB2",'F');
    Branch(name,"varzepVV",'F');
    Branch(name,"varzepV2j1",'F');
    Branch(name,"varzepV2j2",'F');
    Branch(name,"varDRV1j",'F');
    Branch(name,"varDRV2j",'F');
    Branch(name,"varnormPTVVjj",'F');
    Branch(name,"varcenPTVVjj",'F');
    Branch(name,"varFW2j",'F');

    //VV-rest Frame
    Branch(name,"varVVCen",'F');
    Branch(name,"varVVDRVj",'F');
    Branch(name,"varVVnormPTVVjj",'F');

    // BKG
    Branch(name,"varmtop",'F');

    // bosonDECAY
    Branch(name,"dauRatioV1",'F');
    Branch(name,"dauRatioV2",'F');
    Branch(name,"cosThetaV1",'F');
    Branch(name,"cosThetaV2",'F');

    Branch(name,"bosV2j1Pt",'F');
    Branch(name,"bosV2j2Pt",'F');

    // bosonProperties
    Branch(name,"bosV1mass",'F');
    Branch(name,"bosV1Eta",'F');
    Branch(name,"bosV1discr",'F');
    Branch(name,"bosV1tdiscr",'F');
    Branch(name,"bosV1bdiscr",'F');
    Branch(name,"bosV2mass",'F');
    Branch(name,"bosV2Eta",'F');
    Branch(name,"bosV2discr",'F');
    Branch(name,"bosV2tdiscr",'F');
    Branch(name,"bosV2bdiscr",'F');
    Branch(name,"bosV2discr2nd",'F');
    Branch(name,"bosV2chi2",'F');
    Branch(name,"bosV2dR",'F');

    Branch(name,"bosGen",'I');
    Branch(name,"bosV1Unc",'F');
    Branch(name,"bosV2Unc",'F');

    Branch(name,"index_f1",'I');
    Branch(name,"index_f2",'I');
    Branch(name,"index_v1",'I');
    Branch(name,"index_v2",'I');

    //MVA
    Branch(name,"BDTnoBnoMET",'F');
    Branch(name,"BDTwithMET",'F');
    Branch(name,"BDTbtag",'F');

    Branch(name,"DNNMultiRBtagEWK",'F');
    Branch(name,"DNNMultiRBtagQCD",'F');
    Branch(name,"DNNMultiRBtagBKG",'F');

    Branch(name,"BDTMultiwithMETzz",'F');
    Branch(name,"BDTMultiwithMETwz",'F');
    Branch(name,"BDTMultiwithMETbkg",'F');

    //    Branch(name,"BDTMultiEWKchi2",'F');
    //    Branch(name,"BDTMultiQCDchi2",'F');
    //    Branch(name,"BDTMultiBKGchi2",'F');

    Branch(name,"BDTMultiEWKdnnC",'F');
    Branch(name,"BDTMultiQCDdnnC",'F');
    Branch(name,"BDTMultiBKGdnnC",'F');

    Branch(name,"BDTMultiEWKdnnM",'F');
    Branch(name,"BDTMultiQCDdnnM",'F');
    Branch(name,"BDTMultiBKGdnnM",'F');

    Branch(name,"DNNMultiEWKdnn",'F');
    Branch(name,"DNNMultiQCDdnn",'F');
    Branch(name,"DNNMultiBKGdnn",'F');

    }

    else if (purp == 1){


        Branch(name,"j1_pT",'F');
        Branch(name,"j1_Eta",'F');
        Branch(name,"j1_Phi",'F');
        Branch(name,"j1_M",'F');
        Branch(name,"j1_Unc",'F');
        Branch(name,"j1_QGL",'F');
        Branch(name,"j1_DeepB",'F');
        Branch(name,"j1_DeepC",'F');
        Branch(name,"j1_PUDiscr",'F');
        Branch(name,"j1_Area",'F');
        Branch(name,"j1_NEMF",'F');
        Branch(name,"j1_CEMF",'F');
        Branch(name,"j1_NHF",'F');
        Branch(name,"j1_CHF",'F');


        Branch(name,"j2_pT",'F');
        Branch(name,"j2_Eta",'F');
        Branch(name,"j2_Phi",'F');
        Branch(name,"j2_M",'F');
        Branch(name,"j2_Unc",'F');
        Branch(name,"j2_QGL",'F');
        Branch(name,"j2_DeepB",'F');
        Branch(name,"j2_DeepC",'F');
        Branch(name,"j2_PUDiscr",'F');
        Branch(name,"j2_Area",'F');
        Branch(name,"j2_NEMF",'F');
        Branch(name,"j2_CEMF",'F');
        Branch(name,"j2_NHF",'F');
        Branch(name,"j2_CHF",'F');


        Branch(name,"j3_pT",'F');
        Branch(name,"j3_Eta",'F');
        Branch(name,"j3_Phi",'F');
        Branch(name,"j3_M",'F');
        Branch(name,"j3_Unc",'F');
        Branch(name,"j3_QGL",'F');
        Branch(name,"j3_DeepB",'F');
        Branch(name,"j3_DeepC",'F');
        Branch(name,"j3_PUDiscr",'F');
        Branch(name,"j3_Area",'F');
        Branch(name,"j3_NEMF",'F');
        Branch(name,"j3_CEMF",'F');
        Branch(name,"j3_NHF",'F');
        Branch(name,"j3_CHF",'F');


        Branch(name,"j4_pT",'F');
        Branch(name,"j4_Eta",'F');
        Branch(name,"j4_Phi",'F');
        Branch(name,"j4_M",'F');
        Branch(name,"j4_Unc",'F');
        Branch(name,"j4_QGL",'F');
        Branch(name,"j4_DeepB",'F');
        Branch(name,"j4_DeepC",'F');
        Branch(name,"j4_PUDiscr",'F');
        Branch(name,"j4_Area",'F');
        Branch(name,"j4_NEMF",'F');
        Branch(name,"j4_CEMF",'F');
        Branch(name,"j4_NHF",'F');
        Branch(name,"j4_CHF",'F');

 
        Branch(name,"j1_V1j",'F');
        Branch(name,"j1_V2j",'F');
        Branch(name,"j2_V1j",'F');
        Branch(name,"j2_V2j",'F');
        Branch(name,"j3_V1j",'F');
        Branch(name,"j3_V2j",'F');
        Branch(name,"j4_V1j",'F');
        Branch(name,"j4_V2j",'F');

        Branch(name,"j1_f1jLHE",'F');
        Branch(name,"j1_f2jLHE",'F');
        Branch(name,"j2_f1jLHE",'F');
        Branch(name,"j2_f2jLHE",'F');
        Branch(name,"j3_f1jLHE",'F');
        Branch(name,"j3_f2jLHE",'F');
        Branch(name,"j4_f1jLHE",'F');
        Branch(name,"j4_f2jLHE",'F');


        Branch(name,"DEta_f12",'F');
        Branch(name,"DEta_v34",'F');
        Branch(name,"DPhi_f12",'F');
        Branch(name,"DPhi_v34",'F');
        Branch(name,"DR_f12",'F');
        Branch(name,"DR_v34",'F');
        Branch(name,"M_f12", 'F');
        Branch(name,"M_v34", 'F');
        Branch(name,"pT_f12", 'F');
        Branch(name,"pT_v34", 'F');


        Branch(name,"fmatch",'I');
        Branch(name,"Vmatch",'I');
        Branch(name,"Allmatch",'I');
        Branch(name,"EvtNum",'I');
        Branch(name,"Weights",'D');
        Branch(name,"MC",'I');

    }


}


void VBShadAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

    Log(__FUNCTION__,"INFO",Form("doMETAnalysis=%d",doMETAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAnalysis=%d",doBAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAnalysis=%d",doHADAnalysis));

    Log(__FUNCTION__,"INFO",Form("doMETAntiAnalysis=%d",doMETAntiAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAntiAnalysis=%d",doBAntiAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAntiAnalysis=%d",doHADAntiAnalysis));

    Log(__FUNCTION__,"INFO",Form("doSideBand=%d",doSideBand));

    if(doResonant) doTMVA=false;

    if (not jet_resolution)
    { // init jet resolution
        // FIXME: if we want to use these we need to have those for the UL and specific for 17-18-16
        Log(__FUNCTION__,"INFO","Init JER");
        jet_resolution.reset( new JME::JetResolutionObject("aux/jer/Summer16_25nsV1/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt") ) ;
    }

    if(doTMVA or doResTagTMVA) InitTmva();
    if(doResTagKeras) InitScikit();

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {

        //cutflow
        Book ("VBShadAnalysis/GENERAL/Cutflow_"+l, "cutflow; bit; Events", 15,0,15);
        Book ("VBShadAnalysis/GENERAL/CutflowNoW_"+l, "cutflow, no weights; bit; Events", 15,0,15);
        Book ("VBShadAnalysis/GENERAL/Mtt_"+l, "Mtt (unclassified); Mtt [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/GENERAL/LHEht_"+l, "LHE; LHEht", 500,0,5000);

        //Trigger
        if(doTrigger) {

        Book("VBShadAnalysis/Baseline/mVV_MET_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_Base_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_Base_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_Base_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_Base_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);


        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_Base_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_Base_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_Base_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_Base_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_Base_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_Base_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_Base_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_Base_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);



        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad1_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad2_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad3_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad4_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad5_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad6_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHad7_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_triggerHadOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad1_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad2_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad3_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad4_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad5_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad6_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHad7_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RB_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);

        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad1_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad2_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad3_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad4_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad5_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad6_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHad7_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RB_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);

        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad1_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad2_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad3_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad4_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad5_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad6_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHad7_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RB_triggerHadOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);


        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad1_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad2_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad3_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad4_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad5_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad6_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHad7_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_triggerHadOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);


        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad1_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad2_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad3_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad4_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad5_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad6_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHad7_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BB_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);

        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad1_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad2_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad3_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad4_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad5_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad6_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHad7_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BB_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);

        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad1_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad2_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad3_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad4_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad5_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad6_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHad7_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BB_triggerHadOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);


        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag1_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag2_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag3_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag4_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag5_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag6_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag7_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtag8_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerBtagOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_triggerHadOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag1_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag2_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag3_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag4_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag5_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag6_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag7_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtag8_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerBtagOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_RBtag_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);

        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag1_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag2_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag3_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag4_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag5_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag6_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag7_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtag8_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerBtagOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_RBtag_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);


        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag1_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag2_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag3_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag4_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag5_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag6_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag7_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtag8_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerBtagOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_RBtag_triggerHadOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);


        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag1_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag2_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag3_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag4_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag5_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag6_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag7_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtag8_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerBtagOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_triggerHadOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag1_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag2_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag3_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag4_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag5_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag6_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag7_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtag8_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerBtagOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);
        Book("VBShadAnalysis/Baseline/pTV1_BBtag_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; Events", 160,0,1600);


        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag1_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag2_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag3_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag4_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag5_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag6_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag7_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtag8_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerBtagOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);
        Book2D("VBShadAnalysis/Baseline/pTV2V1_BBtag_triggerHadOr_" +l, "pTV1; pTV1 [GeV]; pTV2 [GeV]", 160,0,1600,160,0,1600);


        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag1_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag2_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag3_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag4_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag5_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag6_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag7_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtag8_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerBtagOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);
        Book2D("VBShadAnalysis/Baseline/mJJvsmVV_BBtag_triggerHadOr_" +l, "mVV; mVV [GeV]; mjj [GeV]", 250,0,2500,300,0,3000);


        Book("VBShadAnalysis/Baseline/mVV_MET_triggerMet1_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_MET_triggerMet2_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_MET_triggerMet3_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_MET_triggerMetOr_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        }

        if( (l.find("TT_TuneCP5") !=string::npos) or (l.find("WJetsToLNu_HT") !=string::npos) ) {
        //GenLepton
        Book ("VBShadAnalysis/Baseline/genElePt_"+l, "genElePt; p_{T} (e); Events", 200,0,200);
        Book ("VBShadAnalysis/Baseline/genEleEta_"+l, "genEleEta; #eta (e); Events", 100,-5.,5.);
        Book ("VBShadAnalysis/Baseline/genMuoPt_"+l, "genMuoPt; p_{T} (#mu); Events", 200,0,200);
        Book ("VBShadAnalysis/Baseline/genMuoEta_"+l, "genMuoEta; #eta (#mu); Events", 100,-5.,5.);
        Book ("VBShadAnalysis/Baseline/genTauPt_"+l, "genTauPt; p_{T} (#tau); Events", 200,0,200);
        Book ("VBShadAnalysis/Baseline/genTauEta_"+l, "genTauEta; #eta (#tau); Events", 100,-5.,5.);
        Book ("VBShadAnalysis/Baseline/genTauPt_all_"+l, "genTauPt; p_{T} (#tau); Events", 200,0,200);
        Book ("VBShadAnalysis/Baseline/genTauEta_all_"+l, "genTauEta; #eta (#tau); Events", 100,-5.,5.);
        }

        //FatJet
        Book ("VBShadAnalysis/Baseline/NFatJet_"+l, "NFatJet; NFatJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/eta_FatJet_"+l, "eta_FatJet; #eta; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatZbbJet_"+l, "pT_FatZbbJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_BB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RR_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/Tau21_FatJet_"+l, "Tau21_FatJet; tau21; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/PNetMass_FatJet_Inclusive_"+l, "PNetMass_FatJet; PNetMass [GeV]; Events", 200,0,200.);

        //        if(doStudySFfat) Book ("VBShadAnalysis/Baseline/SF_FatJet_"+l, "SF_FatJet; bit; Events", 36,0,36);
        if(doStudySFfat) Book ("VBShadAnalysis/Baseline/SF_FatJet_"+l, "SF_FatJet; bit; Events", 100,0,100);
        if(doStudySFfat) Book ("VBShadAnalysis/Baseline/SF_FatJet_Mjj_"+l, "SF_FatJet (after mjj cut); bit; Events", 100,0,100);

        if(doStudyMass){

        // mass studies
        Book ("VBShadAnalysis/Baseline/PNetMass_FatJet_"+l, "PNetMass_FatJet; PNetMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/PNetMass_FatZbbJet_"+l, "PNetMass_FatZbbJet; PNetMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_"+l, "SDMass_FatJet; SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatZbbJet_"+l, "SDMass_FatZbbJet; SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_lowDiff_"+l, "SDMass_FatJet (low diff); SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_largeDiff_"+l, "SDMass_FatJet (large diff); SDMass [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_"+l, "SubJetsMass_FatJet; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/FatJetsMass_FatJet_"+l, "FatJetsMass_FatJet; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsDeepbTag_FatJet_"+l, "SubJetsDeepbTag_FatJet; SubJet Mass [GeV]; Events", 100,0,1.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin1_"+l, "SubJetsMass_FatJet barrel_bin1 ; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin1_"+l, "SDMass_FatJet barrel_bin1; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin2_"+l, "SubJetsMass_FatJet barrel_bin2; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin2_"+l, "SDMass_FatJet barrel_bin2; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin3_"+l, "SubJetsMass_FatJet barrel_bin3; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin3_"+l, "SDMass_FatJet barrel_bin3; FatJet Mass [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin1_"+l, "SubJetsMass_FatJet endcap_bin1; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin1_"+l, "SDMass_FatJet endcap_bin1; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin2_"+l, "SubJetsMass_FatJet endcap_bin2; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin2_"+l, "SDMass_FatJet endcap_bin2; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin3_"+l, "SubJetsMass_FatJet endcap_bin3; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin3_"+l, "SDMass_FatJet endcap_bin3; FatJet Mass [GeV]; Events", 100,0,200.);
        //

        //Mass corr
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_200_"+l, "SubJetsMass END 2-300; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_300_"+l, "SubJetsMass END 3-400; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_400_"+l, "SubJetsMass END 4-500; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_500_"+l, "SubJetsMass END 5-600; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_600_"+l, "SubJetsMass END 6-700; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_700_"+l, "SubJetsMass END 7-800; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_END_800plus_"+l, "SubJetsMass END 800p; SubJet Mass [GeV]; Events", 200,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_200_"+l, "SubJetsMass END 2-300; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_300_"+l, "SubJetsMass END 3-400; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_400_"+l, "SubJetsMass END 4-500; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_500_"+l, "SubJetsMass END 5-600; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_600_"+l, "SubJetsMass END 6-700; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_700_"+l, "SubJetsMass END 7-800; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_END_800plus_"+l, "SubJetsMass END 800p; SubJet Mass [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_200_"+l, "SubJetsMass BAR 2-300; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_300_"+l, "SubJetsMass BAR 3-400; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_400_"+l, "SubJetsMass BAR 4-500; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_500_"+l, "SubJetsMass BAR 5-600; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_600_"+l, "SubJetsMass BAR 6-700; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_700_"+l, "SubJetsMass BAR 7-800; SubJet Mass [GeV]; Events", 200,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass1_BAR_800plus_"+l, "SubJetsMass BAR 800p; SubJet Mass [GeV]; Events", 200,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_200_"+l, "SubJetsMass BAR 2-300; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_300_"+l, "SubJetsMass BAR 3-400; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_400_"+l, "SubJetsMass BAR 4-500; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_500_"+l, "SubJetsMass BAR 5-600; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_600_"+l, "SubJetsMass BAR 6-700; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_700_"+l, "SubJetsMass BAR 7-800; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsMass2_BAR_800plus_"+l, "SubJetsMass BAR 800p; SubJet Mass [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJet_"+l, "ZHbbvsQCD_FatJet; ZHbbvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJetFake_"+l, "ZHbbvsQCD_FatJetFake; ZHbbvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsQCD_FatJet_"+l, "WvsQCD_FatJet; WvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsQCD_FatJetFake_"+l, "WvsQCD_FatJetFake; WvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZvsQCD_FatJet_"+l, "ZvsQCD_FatJet; ZvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZvsQCD_FatJetFake_"+l, "ZvsQCD_FatJetFake; ZvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/TvsQCD_FatJet_"+l, "TvsQCD_FatJet; TvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/TvsQCD_FatJetFake_"+l, "TvsQCD_FatJetFake; TvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsT_FatJet_"+l, "WvsT_FatJet; WvsT; Events", 50,0,2.0);
        Book ("VBShadAnalysis/Baseline/ZHccvsQCD_FatJet_"+l, "ZHccvsQCD_FatJet; ZHccvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_"+l, "ZHccvsQCD_FatJetFake; ZHccvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/Tau21_FatJetFake_"+l, "Tau21_FatJet Fake; tau21; Events", 50,0,1.0);

        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_"+l, "SDMass_FatJetFake; SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMassOverPT_FatJetFake_"+l, "SDMassOverPT_FatJetFake; SDMass [GeV]; Events", 100,0,1.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_barrel_"+l, "SDMass_FatJetFake (barrel); SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_endcap_"+l, "SDMass_FatJetFake (endcap); SDMass [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/Baseline/SubJetsMass_FatJetFake_"+l, "SDMass_FatJetFake; SubJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/FatJetsMass_FatJetFake_"+l, "SDMass_FatJetFake; FatJet Mass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SubJetsDeepbTag_FatJetFake_"+l, "SubJetsDeepbTag_FatJetFake; SubJet Mass [GeV]; Events", 100,0,1.);

        Book ("VBShadAnalysis/Baseline/NSubJets_FatJetFake_"+l, "NSubJets; NSubJets; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_lowDiff_"+l, "SDMass_FatJetFake (low diff); SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_largeDiff_"+l, "SDMass_FatJetFake (large diff); SDMass [GeV]; Events", 100,0,200.);
        }// end mass study

        //Jet
        Book ("VBShadAnalysis/Baseline/NJet_"+l, "NJet; FatJet; Events", 10,0,10);
        Book ("VBShadAnalysis/Baseline/pT_Jet_"+l, "pT_Jet; pT [GeV]; Events", 80,0,1600.);

        Book ("VBShadAnalysis/Baseline/NBJet_"+l, "NBJet; NBJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/pT_BJet_"+l, "pT_BJet; pT [GeV]; Events", 120,0,2400.);

        //MET
        Book ("VBShadAnalysis/Baseline/PuppiMet_"+l, "MetPuppi; Met [GeV]; Events", 100,0,1000);
        Book ("VBShadAnalysis/Baseline/Met_"+l, "Met; Met [GeV]; Events", 100,0,1000);
        Book ("VBShadAnalysis/Baseline/MetPhi_"+l, "Met #phi; Met #phi; Events", 100,-TMath::Pi(),TMath::Pi());
        Book ("VBShadAnalysis/Baseline/DphiMETFat_"+l, "Dphi(Met,AK8); #Delta#Phi(Met,AK8) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/Baseline/Dphimin_"+l, "Dphi(Met,AK4); #Delta#Phi(Met,AK4) ; Events", 100,0,6.28);

        // OTHERS
        Book ("VBShadAnalysis/BOSON/BosonDecayRatio_"+l, " ; J2/J1; Events", 200,0,1);
        Book ("VBShadAnalysis/BOSON/CosThetaStar_"+l, " ; CosThetaStar; Events", 200,-1,1);

        // resolved
        Book ("VBShadAnalysis/Baseline/ResBosonMassChi2_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMassChi2_right_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMassChi2_wrong_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);

        Book ("VBShadAnalysis/Baseline/ResBosonMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMass_right_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMass_wrong_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMassClean_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMassClean_right_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResBosonMassClean_wrong_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);


        Book ("VBShadAnalysis/Baseline/ResZMass_right_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResZMass_wrong_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResWMass_right_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);
        Book ("VBShadAnalysis/Baseline/ResWMass_wrong_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);

        Book2D ("VBShadAnalysis/Baseline/ResWMassChi2_"+l, "ResBosonMass; V(i,j) [GeV]; Chi2", 100, 0, 200, 100, 0, 100);
        Book2D ("VBShadAnalysis/Baseline/ResZMassChi2_"+l, "ResBosonMass; V(i,j) [GeV]; Chi2", 100, 0, 200, 100, 0, 100);

        Book ("VBShadAnalysis/Baseline/ResBosonChi2_"+l, "ResBosonChi2; Chi2 [GeV]; Events", 100, 0, 1000.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2_right_"+l, "ResBosonChi2; Chi2; Events", 100, 0, 1000.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2_wrong_"+l, "ResBosonChi2; Chi2; Events", 100, 0, 1000.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_W_"+l, "ResBosonChi2Diff_W; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_Z_"+l, "ResBosonChi2Diff_Z; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_W_right_"+l, "ResBosonChi2Diff_W; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_Z_right_"+l, "ResBosonChi2Diff_Z; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_W_wrong_"+l, "ResBosonChi2Diff_W; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonChi2Diff_Z_wrong_"+l, "ResBosonChi2Diff_Z; Chi2; Events", 100, -25, 25.);
        Book ("VBShadAnalysis/Baseline/ResBosonKeras_"+l, "ResBosonKeras; Keras; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonKeras_right_"+l, "ResBosonKeras; Keras; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonKeras_wrong_"+l, "ResBosonKeras; Keras; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonTMVA_"+l, "ResBosonTMVA; TMVA; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonTMVA_right_"+l, "ResBosonTMVA; TMVA; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonTMVA_wrong_"+l, "ResBosonTMVA; TMVA; Events", 100, 0, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonTMVA2V_right_"+l, "ResBosonTMVA; TMVA; Events", 50, 0.5, 1.);
        Book ("VBShadAnalysis/Baseline/ResBosonTMVA2V_wrong_"+l, "ResBosonTMVA; TMVA; Events", 50, 0.5, 1.);


        Book ("VBShadAnalysis/Baseline/DR_genVj1_"+l, "DeltaR(gen,j1); DeltaR(gen,j1); Events", 100, 0, 4.);
        Book ("VBShadAnalysis/Baseline/DR_genVj2_"+l, "DeltaR(gen,j2); DeltaR(gen,j2); Events", 100, 0, 4.);
        Book ("VBShadAnalysis/Baseline/Eta_genLHEW_"+l, "Eta_V; eta; Events", 120, -3., 3.);
        Book ("VBShadAnalysis/Baseline/Eta_genLHEZ_"+l, "Eta_V; eta; Events", 120, -3., 3.);
        Book ("VBShadAnalysis/Baseline/JetUnc_right_"+l, "unc; jesUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetUnc_wrong_"+l, "unc; jesUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetRes_right_"+l, "res; jerUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetRes_wrong_"+l, "res; jerUnc; Events", 500, 0., 0.25);

        Book ("VBShadAnalysis/Baseline/ResBosonGenMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);

        if(doResonant) {
            Book ("VBShadAnalysis/OUT1500/MVV_"+l, "MVV-OUT (unclassified); MVV [GeV]; Events", 100,0,2500);
            Book ("VBShadAnalysis/IN1500/MVV_"+l, "MVV-IN (unclassified); MVV [GeV]; Events", 100,0,2500);
            Book ("VBShadAnalysis/OUT1500/MVV_BB_"+l, "MVV-OUT (BB); MVV [GeV]; Events", 100,0,2500);
            Book ("VBShadAnalysis/IN1500/MVV_BB_"+l, "MVV-IN (BB); MVV [GeV]; Events", 100,0,2500);

            Book ("VBShadAnalysis/OUT1500/Mjj_"+l, "Mjj-OUT (unclassified); Mjj [GeV]; Events", 100,0,3500);
            Book ("VBShadAnalysis/IN1500/Mjj_"+l, "Mjj-IN (unclassified); Mjj [GeV]; Events", 100,0,3500);
            Book ("VBShadAnalysis/OUT1500/Mjj_BB_"+l, "Mjj-OUT (BB); Mjj [GeV]; Events", 35,0,3500);
            Book ("VBShadAnalysis/IN1500/Mjj_BB_"+l, "Mjj-IN (BB); Mjj [GeV]; Events", 35,0,3500);
        }

        BookHisto(l, "");
        BookHisto(l, "_BB");
        BookHisto(l, "_RB");
        BookHisto(l, "_BMET");
        BookHisto(l, "_RMET");
        BookHisto(l, "_BBtag");
        BookHisto(l, "_RBtag");

    } //end label loop

    if(doWriteTree) { writeTree("tree_vbs",0); writeTree("tree_vbs_JES_TotalDown",0); writeTree("tree_vbs_JES_TotalUp",0); }
    if(writeTrainTree) writeTree("tree_resTag",1);

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}



float VBShadAnalysis::Getjetres(Jet* ajet) {

    JME::JetParameters bins1,vars1;
    vars1.setJetPt( ajet->GetP4().Pt() );
    bins1.setJetEta( ajet->GetP4().Eta() );
    bins1.setRho( std::min(e->Rho(),float(40.)) ); // corrections up to rho 40.

    float sigma1=1.0;

    if ( jet_resolution->getRecord(bins1)==NULL){ sigma1=1.0;}
    else{ sigma1 = jet_resolution->evaluateFormula( *jet_resolution->getRecord(bins1),vars1); }

    return sigma1/(ajet->GetP4().Pt());
}



float VBShadAnalysis::jettagForBoosted(Event*e, string label, string systname, float minEtaV, float maxEtaV) {

    forwardJets.clear();

    float const norm = 1000*1000; // 1TeV^2
    double Mkl = 0; //forward jets
    double chi2_ = 999999.;

    int index_k=-1;
    int index_l=-1;

    for(unsigned k=0; k<selectedJets.size(); ++k) {
        for(unsigned l=0; l<k; ++l) {

            if(selectedJets[k]->Pt()<50 ) continue;
            if(selectedJets[l]->Pt()<50 ) continue;

            if( selectedJets[k]->Eta() * selectedJets[l]->Eta() >=0 ) continue;

            if(selectedJets[k]->Eta() <  minEtaV or selectedJets[k]->Eta() > maxEtaV) {
                if(selectedJets[l]->Eta() <  minEtaV or selectedJets[l]->Eta() > maxEtaV) {
                    Mkl = selectedJets[k]->InvMass(selectedJets[l]);
                    double chi2 = norm / (Mkl*Mkl);
                    if(chi2<chi2_) { chi2_=chi2; index_k= k; index_l=l; }
                }
            }
        }
    }

    for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
        if(iter==index_k) forwardJets.push_back(selectedJets[index_k]);
        if(iter==index_l) forwardJets.push_back(selectedJets[index_l]);
    }

    return  Mkl;
}



std::pair<float, int> VBShadAnalysis::resolvedDNN(Event*e, string label, string systname){

    bosonJets.clear();
    forwardJets.clear();

    evt_maxDnn = 0.;
    evt_2ndmaxDnn = 0;
    evt_maxkeras = -999.;

    evt_v_score = 0;
    int index_i=-1;
    int index_j=-1;
    int index_k=-1;
    int index_l=-1;
    int VTarget = -1; //0 -- W; 1 -- Z


    index_f1 = -1;
    index_f2 = -1;
    index_v1 = -1;
    index_v2 = -1;

    int NjetsToUse = selectedJets.size();
    if(!writeTrainTree and selectedJets.size()>4 and doMETAnalysis) NjetsToUse=4;
    if(!writeTrainTree and selectedJets.size()>5 and doBAnalysis) NjetsToUse = 5;


    for(unsigned i=0; i<NjetsToUse; ++i) {

        if(selectedJets[i]->QGL()<0) continue;

        for(unsigned j=0; j<i; ++j) {

            if(selectedJets[j]->QGL()<0) continue;

            for(unsigned k=0; k<j; ++k) {

                if(selectedJets[k]->QGL()<0) continue;

                for(unsigned l=0; l<k; ++l) {

                    if(selectedJets[l]->QGL()<0) continue;

                    int fi  = -1;
                    int fj  = -1;
                    int vk  = -1;
                    int vl  = -1;
                    int tmpTar = -1; //0 -- W; 1 -- Z
                    //int tmpTar = 0;
                    /*
                    double Mij[2] = {fabs(selectedJets[i]->InvMass(selectedJets[j]) - 80.)/80. , fabs(selectedJets[i]->InvMass(selectedJets[j]) - 91.)/91.};
                    double Mjk[2] = {fabs(selectedJets[j]->InvMass(selectedJets[k]) - 80.)/80. , fabs(selectedJets[j]->InvMass(selectedJets[k]) - 91.)/91.};
                    double Mkl[2] = {fabs(selectedJets[k]->InvMass(selectedJets[l]) - 80.)/80. , fabs(selectedJets[k]->InvMass(selectedJets[l]) - 91.)/91.};
                    double Mik[2] = {fabs(selectedJets[i]->InvMass(selectedJets[k]) - 80.)/80. , fabs(selectedJets[i]->InvMass(selectedJets[k]) - 91.)/91.};
                    double Mil[2] = {fabs(selectedJets[i]->InvMass(selectedJets[l]) - 80.)/80. , fabs(selectedJets[i]->InvMass(selectedJets[l]) - 91.)/91.};
                    double Mjl[2] = {fabs(selectedJets[l]->InvMass(selectedJets[j]) - 80.)/80. , fabs(selectedJets[l]->InvMass(selectedJets[j]) - 91.)/91.};

                    double minM_W = std::min({Mij[0], Mjk[0], Mkl[0], Mik[0], Mil[0], Mjl[0]});
                    double minM_Z = std::min({Mij[1], Mjk[1], Mkl[1], Mik[1], Mil[1], Mjl[1]});
                    double minM   = std::min(minM_W,minM_Z);

                    if(minM_W < minM_Z) {tmpTar = 0;} else {tmpTar = 1;}
                    if(tmpTar < 0) continue;

                    if(minM == Mij[tmpTar]) {fi = k; fj = l; vk = i; vl = j;}
                    else if(minM == Mjk[tmpTar]) {fi = i; fj = l; vk = j; vl = k; }
                    else if(minM == Mkl[tmpTar]) {fi = i; fj = j; vk = k; vl = l; }
                    else if(minM == Mik[tmpTar]) {fi = j; fj = l; vk = i; vl = k; }
                    else if(minM == Mil[tmpTar]) {fi = j; fj = k; vk = i; vl = l; }
                    else if(minM == Mjl[tmpTar]) {fi = i; fj = k; vk = j; vl = l; }

                    if(fi<0 || fj<0 || vk<0 || vl<0) continue;
                    */

                    //// new assignment in training and application
                    vector<vector<unsigned> > assign{ { l, k, j, i }, 
                                                    { j, i, l, k }, 
                                                    { l, j, k, i },
                                                    { k, i, l, j },
                                                    { l, i, k, j },
                                                    { k, j, l, i } }; 
            
                    for(int comb = 0; comb < 6; comb++){

                        fi = assign[comb][0];  fj = assign[comb][1]; vk = assign[comb][2]; vl = assign[comb][3];

                        if(fi<0 || fj<0 || vk<0 || vl<0) continue;

                        if(writeTrainTree) {
                            setTrainingTree(e,label, fi, fj, vk,vl);
                            FillTree("tree_resTag");
                        } // end traintree


                        if(selectedJets[fi]->GetP4().Pt() < 50) continue;
                        if(selectedJets[fj]->GetP4().Pt() < 50) continue;

                        if( (selectedJets[fj]->GetNEMF() == 0 || selectedJets[fj]->GetNHF() == 0) and (fabs(selectedJets[fj]->Eta()) > 2.4 && fabs(selectedJets[fj]->Eta()) < 3.0 ) ) continue;
                        if(fabs(selectedJets[vk]->Eta())>3.0) continue;
                        if(fabs(selectedJets[vl]->Eta())>3.0) continue;
                    
                        float zepV3 = fabs(selectedJets[vk]->Eta() - (selectedJets[fi]->Eta()+selectedJets[fj]->Eta())/2.0)/fabs(selectedJets[fi]->DeltaEta(selectedJets[fj]));
                        float zepV4 = fabs(selectedJets[vl]->Eta() - (selectedJets[fi]->Eta()+selectedJets[fj]->Eta())/2.0)/fabs(selectedJets[fi]->DeltaEta(selectedJets[fj]));        
                        float centralV2 = std::min( std::min(selectedJets[vk]->Eta(),selectedJets[vl]->Eta())-std::min(selectedJets[fi]->Eta(),selectedJets[fj]->Eta()), std::max(selectedJets[fi]->Eta(),selectedJets[fj]->Eta())-std::max(selectedJets[vk]->Eta(),selectedJets[vl]->Eta()) );
                    
                        if(zepV3>25 || zepV4>25) continue;


                        if(doResTagTMVA){


                            SetVariable("j1_pT",  selectedJets[fi]->GetP4().Pt());
                            SetVariable("abs(j1_Eta)", fabs(selectedJets[fi]->Eta()));
                            //SetVariable("j1_QGL", selectedJets[fi]->QGL()<0?0:selectedJets[fi]->QGL()); 


                            SetVariable("j2_pT",  selectedJets[fj]->GetP4().Pt());
                            SetVariable("abs(j2_Eta)", fabs(selectedJets[fj]->Eta()));
                            //SetVariable("j2_QGL", selectedJets[fj]->QGL()<0?0:selectedJets[fj]->QGL());
                            //SetVariable("j2_PUDiscr",selectedJets[fj]->GetPuId());


                            SetVariable("j3_pT",  selectedJets[vk]->GetP4().Pt());
                            SetVariable("abs(j3_Eta)", fabs(selectedJets[vk]->Eta()));
                            //SetVariable("j3_QGL", selectedJets[vk]->QGL()<0?0:selectedJets[vk]->QGL());


                            SetVariable("j4_pT",  selectedJets[vl]->GetP4().Pt());
                            SetVariable("abs(j4_Eta)", fabs(selectedJets[vl]->Eta()));
                            //SetVariable("j4_QGL", selectedJets[vl]->QGL()<0?0:selectedJets[vl]->QGL());

                            SetVariable("abs(j3_Eta-(j1_Eta+j2_Eta)/2.)/abs(DEta_f12)", zepV3);
                            SetVariable("abs(j4_Eta-(j1_Eta+j2_Eta)/2.)/abs(DEta_f12)", zepV4);
                            SetVariable("min(min(j3_Eta,j4_Eta)-min(j1_Eta,j2_Eta),max(j1_Eta,j2_Eta)-max(j3_Eta,j4_Eta))", centralV2);

                            //SetVariable("abs(DEta_f12)", selectedJets[fi]->DeltaEta(selectedJets[fj]));
                            SetVariable("abs(DEta_v34)", selectedJets[vk]->DeltaEta(selectedJets[vl]));
                            SetVariable("abs(DPhi_f12)", ChargedHiggs::deltaPhi(selectedJets[fi]->Phi(), selectedJets[fj]->Phi()));
                            SetVariable("abs(DPhi_v34)", ChargedHiggs::deltaPhi(selectedJets[vk]->Phi(), selectedJets[vl]->Phi()));
                            //SetVariable("DR_f12", selectedJets[fi]->DeltaR(selectedJets[fj]));
                            //SetVariable("DR_v34", selectedJets[vk]->DeltaR(selectedJets[vl]));
                            //SetVariable("M_f12",  selectedJets[fi]->InvMass(selectedJets[fj]));
                            //SetVariable("M_v34",  selectedJets[vk]->InvMass(selectedJets[vl]));


                            float dnn = 0;

                            float dnn_ml_0 = 0;
                            float dnn_ml_2 = 0;   //F yes V not
                            float dnn_ml_3 = 0;   // both not
                            float V_dnn = 0;
                            float V_cut = 0;

                            float dnn_W = (float)readers_dnn_[0]->EvaluateMVA("DNN_ResTag_W");
                            float dnn_Z = (float)readers_dnn_[1]->EvaluateMVA("DNN_ResTag_Z");
           
                            dnn = dnn_W; tmpTar = 0;
                            if(dnn_Z>dnn_W) {dnn = dnn_Z; tmpTar = 1;} 

                            if(doBAnalysis){
                                //dnn_ml_0 = (float)readers_dnn_[2]->EvaluateMulticlass("DNN_ResTag_RBTA_ml")[0];
                                //dnn_ml_2 = (float)readers_dnn_[2]->EvaluateMulticlass("DNN_ResTag_RBTA_ml")[2];
                                //dnn_ml_3 = (float)readers_dnn_[2]->EvaluateMulticlass("DNN_ResTag_RBTA_ml")[3];
                                // for 4 + 2 (2DNN)
                                V_dnn = (float)readers_dnn_[4]->EvaluateMVA("DNN_VTag_RBTA");
                                V_cut = 0.75;
                            }
                            if(doMETAnalysis){
                                //dnn_ml_0 = (float)readers_dnn_[3]->EvaluateMulticlass("DNN_ResTag_RMET_ml")[0];
                                //dnn_ml_2 = (float)readers_dnn_[3]->EvaluateMulticlass("DNN_ResTag_RMET_ml")[2];
                                //dnn_ml_3 = (float)readers_dnn_[3]->EvaluateMulticlass("DNN_ResTag_RMET_ml")[3];
                                // for 4 + 2 (2DNN)
                                V_dnn = (float)readers_dnn_[5]->EvaluateMVA("DNN_VTag_RMET");
                                V_cut = 0.7;
                            }

                            if(doMultiTagger){
                                dnn = dnn_ml_0;
                                if(dnn_ml_2 > 0.4) continue;
                                if(dnn_ml_3 > 0.2 ) continue;
                            }else if(do2DNN){
                                if(V_dnn < V_cut) continue;
                            }

                            if(dnn > evt_maxDnn){

                                evt_2ndmaxDnn = evt_maxDnn;
                                evt_maxDnn = dnn;
                                index_i=vk;
                                index_j=vl;
                                index_k=fi;
                                index_l=fj;
                                VTarget = tmpTar;
                                evt_v_score = V_dnn;
                            }
                            if(dnn > evt_2ndmaxDnn and dnn < evt_maxDnn) evt_2ndmaxDnn = dnn;

                        }//end TMVA

                    

                        if(doResTagKeras){
                            (*x).clear();
                            (*x).push_back(selectedJets[fi]->GetP4().Pt());
                            (*x).push_back(selectedJets[fi]->Eta());
                            //(*x).push_back(selectedJets[fi]->Phi());
                            (*x).push_back(selectedJets[fi]->M());
                            (*x).push_back(sqrt((selectedJets[fi]->GetJESUnc())*(selectedJets[fi]->GetJESUnc()) + Getjetres(selectedJets[fi]) * Getjetres(selectedJets[fi])));

                            (*x).push_back(selectedJets[fj]->GetP4().Pt());
                            //(*x).push_back(selectedJets[fj]->Eta());
                            //(*x).push_back(selectedJets[fj]->Phi());
                            (*x).push_back(selectedJets[fj]->M());
                            (*x).push_back(sqrt((selectedJets[fj]->GetJESUnc())*(selectedJets[fj]->GetJESUnc()) + Getjetres(selectedJets[fj]) * Getjetres(selectedJets[fj])));

                            (*x).push_back(selectedJets[vk]->GetP4().Pt());
                            (*x).push_back(selectedJets[vk]->Eta());
                            //(*x).push_back(selectedJets[vk]->Phi());
                            //(*x).push_back(selectedJets[vk]->M());
                            (*x).push_back(sqrt((selectedJets[vk]->GetJESUnc())*(selectedJets[vk]->GetJESUnc()) + Getjetres(selectedJets[vk]) * Getjetres(selectedJets[vk])));

                            (*x).push_back(selectedJets[vl]->GetP4().Pt());
                            (*x).push_back(selectedJets[vl]->Eta());
                            //(*x).push_back(selectedJets[vl]->Phi());
                            //(*x).push_back(selectedJets[vl]->M());
                            (*x).push_back(sqrt((selectedJets[vl]->GetJESUnc())*(selectedJets[vl]->GetJESUnc()) + Getjetres(selectedJets[vl]) * Getjetres(selectedJets[vl])));

                            //float kerasDiscr = py->Eval("kmodel.predict(np.array([ x ] ))[0][0]");
                            float kerasDiscr = 0.;
                            if(doBAnalysis) kerasDiscr = py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16],x[17],x[18],x[19],x[20],x[21],x[22] ] ] ))[0][0]");
                            if(doMETAnalysis)   kerasDiscr  = py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16],x[17],x[18],x[19],x[20],x[21] ] ] ))[0][0]");
                        //float kerasDiscr = py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16],x[17],x[18],x[19],x[20],x[21],x[22],x[23],x[24],x[25],x[26],x[27],x[28],x[29],x[30],x[31] ] ] ))[0][0]");

                            if(kerasDiscr > evt_maxkeras){
                            //                            std::cout << "kerasDiscr " << kerasDiscr << std::endl;
                                evt_maxkeras = kerasDiscr;
                                index_i=vk;
                                index_j=vl;
                                index_k=fi;
                                index_l=fj;
                                VTarget = tmpTar;
                            }
                        }//end keras

                    } // end assignment

                }//l
            }//k
        }//j
    }//i

    if(index_i>=0 && index_j>=0 && index_k>=0 && index_l>=0){
        bosonJets.push_back(selectedJets[index_i]);
        bosonJets.push_back(selectedJets[index_j]);
        if(selectedJets[index_k]->Pt() > fjPtCut ) forwardJets.push_back(selectedJets[index_k]);
        if(selectedJets[index_l]->Pt() > fjPtCut ) forwardJets.push_back(selectedJets[index_l]);

        index_f1 = index_k;
        index_f2 = index_l;
        index_v1 = index_i;
        index_v2 = index_j;
    }

    std::pair<float, int> pairResDNN = std::make_pair(0., -1);
    if(bosonJets.size()>1 && VTarget>=0) pairResDNN  = std::make_pair((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M() , VTarget); 

    return pairResDNN;


}


std::pair<float, float> VBShadAnalysis::resolvedtagger(Event*e, float MV, string label, string systname, float etaV1) {

    bosonJets.clear();
    forwardJets.clear();
    vetoJets.clear();

    //    float const norm = 1000*1000; // 0.5TeV^2
    float const norm = 500*500; // 0.5TeV^2
    //    float const norm = 0.; // temporary remove the Mjj terms
    //    float const MVres = 20*20; // 20 GeV
    float const MVres = 10*10; // 20 GeV

    double DRij = 0; //Wjets
    double PTij = 0; //Wjets
    double Mij = 0; //Wjets
    double Mkl = 0; //forward jets
    double DPhiij = 0;
    double MRij = 0;
    double Unc_i = 0;
    double Unc_j = 0;
    double V_term = 0;

    float bestChi2_ = 999999.;
    evt_bosV2unc = 0;

    int index_i=-1;
    int index_j=-1;
    int index_k=-1;
    int index_l=-1;


    int NjetsToUse = selectedJets.size();
    if(!writeTrainTree and selectedJets.size()>4 and doMETAnalysis) NjetsToUse=4;
    if(!writeTrainTree and selectedJets.size()>5 and doBAnalysis) NjetsToUse = 5;

    for(unsigned i=0; i<NjetsToUse; ++i) {
        for(unsigned j=0; j<i; ++j) {

            if( fabs(selectedJets[i]->Eta())>2.4 ) continue;
            if( fabs(selectedJets[j]->Eta())>2.4 ) continue;

            Mij = selectedJets[i]->InvMass(selectedJets[j]);
            DRij = selectedJets[i]->DeltaR(selectedJets[j]);
            PTij = (selectedJets[i]->GetP4()+selectedJets[j]->GetP4()).Pt();
            DPhiij = fabs(ChargedHiggs::deltaPhi(selectedJets[i]->GetP4().Phi(), selectedJets[j]->GetP4().Phi()));
            //MRij = sqrt( 2 * (selectedJets[i]->GetP4().Pt()) * (selectedJets[j]->GetP4().Pt()) * cos(DPhiij) );
            MRij = 0.5*DRij*PTij;

            // requires resolved jets well separated (so that do not overlap with the boosted)
            //            if(DRij<0.8) continue;

            float minEtaV = std::min(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());
            float maxEtaV = std::max(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());

            float bosjer1=Getjetres(selectedJets[i]);
            float bosjer2=Getjetres(selectedJets[j]);

            Unc_i = (selectedJets[i]->GetJESUnc())*(selectedJets[i]->GetJESUnc()) + bosjer1 * bosjer1;
            Unc_j = (selectedJets[j]->GetJESUnc())*(selectedJets[j]->GetJESUnc()) + bosjer2 * bosjer2;
            //V_term = (MRij - MV) * (MRij - MV)  / ( MRij*MRij * ( sqrt(Unc_i+Unc_j+2.5*2.5/(MV*MV)) ) ); 
            //V_term = (MRij - MV) * (MRij - MV)  / MVres * (1+sqrt(Unc_i+Unc_j+2.5*2.5/(MV*MV)));
            //            V_term = (MRij - MV) * (MRij - MV)  / MVres * (Unc_i+Unc_j+2.5*2.5/(MV*MV));

            V_term = (MRij - MV) * (MRij - MV)  / MVres ;
            //optimized chi2 --- fancy one, overtunning, not a good baseline
            //V_term = (MRij - MV) * (MRij - MV)  / MVres * (1 + 100*(PTij>0?(DRij/PTij):0))  + (Mij - MV) * (Mij - MV) / MVres;

            for(unsigned k=0; k<NjetsToUse; ++k) {
                if(k == i || k == j) continue;
                for(unsigned l=0; l<k; ++l) {
                    if(l == i || l == j) continue;

                    // remove and cut later to reduce combinatorics
                    //                    if( selectedJets[k]->Eta() * selectedJets[l]->Eta() >=0 ) continue;
                    //                    if( (selectedJets[k]->Eta() <  minEtaV or selectedJets[k]->Eta() > maxEtaV)) {
                    //                        if( (selectedJets[l]->Eta() <  minEtaV or selectedJets[l]->Eta() > maxEtaV) and !doMETAnalysis ) {
                    //                        }
                    //                    }

                    Mkl = selectedJets[k]->InvMass(selectedJets[l]);
                    //                            double chi2 = (norm / (Mkl*Mkl)) + (Mij*Mij - MV*MV)/MVres;
                    // DR ~ 2M/PT
                    double chi2 = (norm / (Mkl*Mkl)) + V_term;
                    //double chi2 = (norm / (Mkl*Mkl)) + (0.5*DRij*PTij - MV) * (0.5*DRij*PTij - MV) / MVres;
                    if(chi2<bestChi2_) { evt_bosV2unc = sqrt(Unc_i+Unc_j); bestChi2_=chi2; index_i=i; index_j=j; index_k=k; index_l=l; }
                }
            }
        }
    }

    float VjPtCut = (MV > 85) ? VjPtCut_Z : VjPtCut_W;

    for(unsigned iter=0; iter<NjetsToUse; ++iter) {
        if(iter==index_i and selectedJets[index_i]->Pt()>VjPtCut) bosonJets.push_back(selectedJets[index_i]);
        if(iter==index_j and selectedJets[index_j]->Pt()>VjPtCut) bosonJets.push_back(selectedJets[index_j]);
        if(iter==index_k and selectedJets[index_k]->Pt()>fjPtCut) forwardJets.push_back(selectedJets[index_k]);
        if(iter==index_l and selectedJets[index_l]->Pt()>fjPtCut) forwardJets.push_back(selectedJets[index_l]);
    }

    if(bosonJets.size()>1 and forwardJets.size()>1) {
        
        for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
            if(iter!=index_i and iter!=index_j and iter!=index_k and iter!=index_l) {
                //                if(selectedJets[iter]->Pt()< std::min(bosonJets[0]->Pt(),bosonJets[1]->Pt())) continue;
                if((selectedJets[iter]->Eta() < std::max(selectedJets[index_k]->Eta(),selectedJets[index_l]->Eta())) and
                   (selectedJets[iter]->Eta() > std::min(selectedJets[index_k]->Eta(),selectedJets[index_l]->Eta())))
                    vetoJets.push_back(selectedJets[iter]);
            }
        }

        std::pair<float, float> pairResolved = std::make_pair((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M() , bestChi2_);
        return pairResolved;
    } else {
        std::make_pair(0.,999999.);
    }
}


float VBShadAnalysis::genMtt(Event*e)
{

    float Mtt=-1;

    GenParticle * genT = NULL;
    GenParticle * genTbar = NULL;
    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        //        if( ! genpar->IsLHE()) continue;
        if(genpar->GetPdgId() == 6) if(genT==NULL) { genT = genpar; }
        if(genpar->GetPdgId() == -6) if(genTbar==NULL) { genTbar = genpar; }
    }


    if(genT!=NULL and genTbar!=NULL) Mtt= (genT->GetP4()+genTbar->GetP4()).M();

    return Mtt;

}

bool VBShadAnalysis::genMatchResolved(Event*e, string systname, string label){
    if(bosonJets.size()<2) return false;
    if(forwardJets.size()<2) return false;   //optional 2 or 4

    bool match_1 = false;
    bool match_2 = false;
    // for the forward jets, optional
    bool match_3 = false;
    bool match_4 = false;

    evt_genmatch = false;

    float bosjer1=Getjetres(bosonJets[0]);
    float bosjer2=Getjetres(bosonJets[1]);

    int MatchParentID = 24;
    //if((label.find("ZnnZhadJJ_EWK") !=string::npos) || (label.find("ZbbZhadJJ_EWK")!=string::npos )) MatchParentID = 23;
    if((label.find("ZNUNUZJJjj_EWK") !=string::npos) || (label.find("ZJJZJJjj_EWK")!=string::npos ) || (label.find("WPLEPZHADjj_EWK")!=string::npos ) || (label.find("WMLEPZHADjj_EWK")!=string::npos )) MatchParentID = 23;

    for(Int_t i = e->NGenPar()-1; i >= 0; i--){

        GenParticle *genpar = e->GetGenParticle(i);

        if(fabs(genpar->GetPdgId()) < 6 and fabs(genpar->GetParentPdgId()) == MatchParentID){
            if( (bosonJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.4  ) match_1 = true; 
            if( (bosonJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.4  ) match_2 = true;
        }

        //cout <<  "debug LHE: " << genpar->IsLHE() << "pa id: " <<  genpar->GetParentPdgId() << endl;
        if( ! genpar->IsLHE()) continue;

        Fill("VBShadAnalysis/Baseline/DR_genVj1_"+label, systname, (bosonJets[0]->GetP4()).DeltaR(genpar->GetP4()), e->weight() );
        Fill("VBShadAnalysis/Baseline/DR_genVj2_"+label, systname, (bosonJets[1]->GetP4()).DeltaR(genpar->GetP4()), e->weight() );

        //if( fabs(genpar->GetParentPdgId())!= 23 && fabs(genpar->GetParentPdgId())!= 24 ) continue;

        if(fabs(genpar->GetPdgId()) == 23 and fabs(genpar->GetParentPdgId())>6) Fill("VBShadAnalysis/Baseline/Eta_genLHEZ_"+label, systname, genpar->Eta(), e->weight() );
        if(fabs(genpar->GetPdgId()) == 24 and fabs(genpar->GetParentPdgId())>6) Fill("VBShadAnalysis/Baseline/Eta_genLHEW_"+label, systname, genpar->Eta(), e->weight() );

        //V jets. ParentID match better than LHE match
        //if( (i == e->NGenPar()-3 or i == e->NGenPar()-4) and (bosonJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.3 ) match_1 = true;
        //if( (i == e->NGenPar()-3 or i == e->NGenPar()-4) and (bosonJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.3 ) match_2 = true;

        //forward jets optional 2 or 4
        //if( (forwardJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_3 = true;
        //if( (forwardJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_4 = true;

    }

    evt_genmatch = match_1 && match_2;
    //evt_genmatch = match_1 && match_2 && match_3 && match_4;


    if(match_1){
        Fill("VBShadAnalysis/Baseline/JetUnc_right_"+label, systname, bosonJets[0]->GetJESUnc(), e->weight() );
        Fill("VBShadAnalysis/Baseline/JetRes_right_"+label, systname, bosjer1, e->weight() );
    }else{
        Fill("VBShadAnalysis/Baseline/JetUnc_wrong_"+label, systname, bosonJets[0]->GetJESUnc(), e->weight() );
        Fill("VBShadAnalysis/Baseline/JetRes_wrong_"+label, systname, bosjer1, e->weight() );
    }

    if(match_2){
        Fill("VBShadAnalysis/Baseline/JetUnc_right_"+label, systname, bosonJets[1]->GetJESUnc(), e->weight() );
        Fill("VBShadAnalysis/Baseline/JetRes_right_"+label, systname, bosjer2, e->weight() );
    }else{
        Fill("VBShadAnalysis/Baseline/JetUnc_wrong_"+label, systname, bosonJets[1]->GetJESUnc(), e->weight() );
        Fill("VBShadAnalysis/Baseline/JetRes_wrong_"+label, systname, bosjer2, e->weight() );
    }



    if(!match_1 || !match_2){

        Jet* gjet1 = NULL;
        Jet* gjet2 = NULL;

        for(unsigned i=0; i<selectedJets.size(); ++i) {

            if( selectedJets[i] == forwardJets[0] || selectedJets[i] == forwardJets[1]) continue;

            for(Int_t j = 0; j < e->NGenPar(); j++){

                GenParticle *genpar = e->GetGenParticle(j);

                if(!(fabs(genpar->GetPdgId()) < 6 and fabs(genpar->GetParentPdgId()) == MatchParentID)) continue;
                //if( ! genpar->IsLHE()) continue;
                //if( (selectedJets[i]->GetP4()).DeltaR(genpar->GetP4()) > 0.2 ) continue;

                if( !match_1 && match_2 && selectedJets[i] != bosonJets[1] ) {gjet1 = selectedJets[i]; gjet2 = bosonJets[1];}
                if( match_1 && !match_2 && selectedJets[i] != bosonJets[0] ) {gjet1 = bosonJets[0]; gjet2 = selectedJets[i];}
                if( !match_1 && !match_2) { if(gjet1 == NULL) gjet1 = selectedJets[i]; else gjet2 = selectedJets[i];}

            }
        }

        if(gjet1 != NULL && gjet2 != NULL){
            Fill("VBShadAnalysis/Baseline/ResBosonGenMass_"+label, systname, (gjet1->GetP4() + gjet2->GetP4()).M(), e->weight() );
            //double new_chi2 = (1000*1000 / (*Mkl)) + (0.5*DRij*PTij - MV) * (0.5*DRij*PTij - MV) / MVres;

        }
    }

    return (match_1 && match_2);

}

bool VBShadAnalysis::genMatchResonant(Event*e, string label, string category){

    if(!checkSignalLabel(label)) return false;

    /*
    if(genVp==NULL or genVp2==NULL) evt_genmatch = false;
    if(genVp!=NULL and genVp2!=NULL) {
        bool match1 = false;
        bool match2 = false;
        if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and selectedFatJets[1]->DeltaR(*genVp2) < 0.2 ) match1 = true;
        if(selectedFatJets[1]->DeltaR(*genVp) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) match2 = true;
        if(match1 or match2) evt_genmatch = true;
    }
    */

    bool match_1 = false;
    bool match_2 = false;
    evt_genmatch = false;

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);
        if((fabs(genpar->GetPdgId()) != 23) and  (fabs(genpar->GetPdgId()) != 24) ) continue;
        if(fabs(genpar->GetParentPdgId())<=6) continue;

        // target WjjZbb
        if( category.find("BBtag")   !=string::npos ) {
            if( (fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (selectedFatZbb[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_1 = true;
            if( (selectedFatJets.size()>0 and  (fabs(genpar->GetPdgId()) == 24) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2)  ||
                (selectedFatZbb.size()>1 and  (fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (selectedFatZbb[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.2)) match_2 = true;
        } else if ( category.find("BB")   !=string::npos ) {
            if(label.find("ZJJZJJjj_EWK_LO") !=string::npos  ||
               label.find("ZJJZJJjj_EWK_QCD_LO") !=string::npos  ||
               label.find("ZJJZJJjj_QCD_LO") !=string::npos) {
                // target ZjjZjj
                if( (fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_1 = true;
                if( (fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_2 = true;
            } else {
            // target WjjWjj
                if( (fabs(genpar->GetPdgId()) == 24) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_1 = true;
                if( (fabs(genpar->GetPdgId()) == 24) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_2 = true;
            }
        } else if( category.find("BMET")   !=string::npos ) {
            // target WjjZnn
            if( usePuppi ) {
                if((fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (e->GetMet().GetPuppiMetP4()).DeltaPhi(genpar->GetP4()) < 0.2 ) match_1 = true;
            } else {
                if((fabs(genpar->GetPdgId()) == 23) and fabs(genpar->GetParentPdgId())>6 and (e->GetMet().GetP4()).DeltaPhi(genpar->GetP4()) < 0.2 ) match_1 = true;
            }
            if(selectedFatZbb.size()>0) {
                if( (fabs(genpar->GetPdgId()) == 24) and fabs(genpar->GetParentPdgId())>6 and (selectedFatZbb[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_2 = true;
            } else {
                if( (fabs(genpar->GetPdgId()) == 24) and fabs(genpar->GetParentPdgId())>6 and (selectedFatJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_2 = true;
            }
        }
    }

    evt_genmatch = match_1 && match_2;
    return evt_genmatch;
}

void VBShadAnalysis::genStudies(Event*e, string label )
{

    int pdgID1=24;
    int pdgID2=24;

    if((label.find("ZnnZhadJJ_EWK") !=string::npos )
       || (label.find("ZbbZhadJJ_EWK")!=string::npos )
       || (label.find("ZJJZJJjj_EWK_LO")!=string::npos )
       || (label.find("ZJJZJJjj_QCD_LO")!=string::npos )
       || (label.find("ZJJZJJjj_EWK_QCD_LO")!=string::npos )
       || (label.find("ZNUNUZJJjj_EWK") !=string::npos)
       || (label.find("ZNUNUZJJjj_QCD") !=string::npos)
       || (label.find("ZNUNUZJJjj_EWK_QCD") !=string::npos)
       ) {
        pdgID1=23;
        pdgID2=23;
    } else if (label.find("WPhadWPhadJJ_EWK") !=string::npos ||
               label.find("WWjj_SS_ll") !=string::npos ||
               label.find("WWjj_SS_lt") !=string::npos ||
               label.find("WWjj_SS_tt") !=string::npos ||
               label.find("WPMJJWPMJJjj_EWK_LO") !=string::npos ||
               label.find("WPMJJWPMJJjj_QCD_LO") !=string::npos ||
               label.find("WPMJJWPMJJjj_EWK_QCD_LO") !=string::npos ||
               label.find("WPJJWMJJjj_EWK_LO") !=string::npos ||
               label.find("WPJJWMJJjj_QCD_LO") !=string::npos ||
               label.find("WPJJWMJJjj_EWK_QCD_LO") !=string::npos ||
               //
               label.find("WPHADWMLEPjj_EWK_LO") !=string::npos  ||
               label.find("WPLEPWMHADjj_EWK_LO") !=string::npos  ||
               label.find("WPLEPWPHADjj_EWK_LO") !=string::npos  ||
               label.find("WMLEPWMHADjj_EWK_LO") !=string::npos  ||
               label.find("WPHADWMLEPjj_QCD_LO") !=string::npos  ||
               label.find("WPLEPWMHADjj_QCD_LO") !=string::npos  ||
               label.find("WPLEPWPHADjj_QCD_LO") !=string::npos  ||
               label.find("WMLEPWMHADjj_QCD_LO") !=string::npos  ||
               //
               label.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M3000") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_semilep_M1000") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_semilep_M2000") !=string::npos ||
               label.find("ST") !=string::npos ||
               label.find("TTX") !=string::npos ||
               //
               label.find("TT_TuneCP5") !=string::npos ||
               //               label.find("TTTo2L2Nu") !=string::npos ||
               label.find("TTToSemiLeptonic") !=string::npos ||
               label.find("TTToHadronic") !=string::npos ||
               label.find("TTJets_TuneCP5") !=string::npos ||
               label.find("TT_Mtt") !=string::npos
               ) {
        pdgID1=24;
        pdgID2=24;
    } else if (label.find("DIBOSON") !=string::npos ||
               label.find("TRIBOSON") !=string::npos ||
               label.find("MULTIBOSON") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1000") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1500") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M2000") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1000") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1500") !=string::npos ||
               label.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M2000") !=string::npos ||
               //
               label.find("WPLEPZHADjj_EWK_LO") !=string::npos  ||
               label.find("WMLEPZHADjj_EWK_LO") !=string::npos  ||
               label.find("WPLEPZHADjj_QCD_LO") !=string::npos  ||
               label.find("WMLEPZHADjj_QCD_LO") !=string::npos  ||
               //
               label.find("ZNUNUWPMJJjj_EWK") !=string::npos  ||
               label.find("ZNUNUWPMJJjj_QCD") !=string::npos  ||
               label.find("ZNUNUWPMJJjj_EWK_QCD") !=string::npos  ||
               label.find("ZJJNOBWPMJJjj_EWK_LO") !=string::npos  ||
               label.find("ZJJNOBWPMJJjj_QCD_LO") !=string::npos  ||
               label.find("ZJJNOBWPMJJjj_EWK_QCD_LO") !=string::npos  ||
               label.find("ZNuNuWPMJJjj_EWK_LO")!=string::npos ||
               label.find("ZNuNuWPMJJjj_QCD_LO")!=string::npos ||
               label.find("ZNuNuWPMJJjj_EWK_QCD_LO")!=string::npos ||
               label.find("ZBBWPMJJjj_EWK_LO")!=string::npos ||
               label.find("ZBBWPMJJjj_QCD_LO")!=string::npos ||
               label.find("ZBBWPMJJjj_EWK_QCD_LO")!=string::npos ||
               label.find("ZbbWpmJJ_EWK")!=string::npos ||
               label.find("ZbbWpmJJ_EWK_QCD")!=string::npos ||
               label.find("ZbbWpmJJ_QCD")!=string::npos
               ) {
        pdgID1=23;
        pdgID2=24;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);
        //        if( ! genpar->IsLHE()) continue;

        // ** promptLeptons from W for Wjets and semileptonic ttbar in MET category
        if((fabs(genpar->GetPdgId()) == 11 ||  fabs(genpar->GetPdgId()) == 13 || fabs(genpar->GetPdgId()) == 15) and fabs(genpar->GetParentPdgId())==24) if(genLep==NULL) { genLep = genpar; }
        // ** prompt V for the ZJetsToNuNu_HT and WJetsToLNu_HT and pt-reweighting
        if( (fabs(genpar->GetPdgId()) == 23) and (label.find("ZJetsToNuNu_HT") !=string::npos or label.find("ZJetsToNuNuPt") !=string::npos ) ) if(genVp==NULL) { genVp = genpar; }
        if( (fabs(genpar->GetPdgId()) == 24) and (label.find("WJetsToLNu_HT") !=string::npos or label.find("WJetsToLNu_Pt") !=string::npos ) ) if(genVp==NULL) { genVp = genpar; }

        // ** BOSON
        if(fabs(genpar->GetPdgId()) == pdgID1 and fabs(genpar->GetParentPdgId())>6) if(genVp==NULL) { genVp = genpar; /*cout << "found W1 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }
        if(fabs(genpar->GetPdgId()) == pdgID2 and fabs(genpar->GetParentPdgId())>6) if(genVp2==NULL and genpar!=genVp) { genVp2 = genpar; /*cout << "found W2 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }

        // ** DECAY PRODUCT
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID1 and fabs(genpar->GetParentPdgId())>6) if(dauV1a==NULL) { dauV1a = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID1 and fabs(genpar->GetParentPdgId())>6) if(dauV1b==NULL and genpar!=dauV1a) { dauV1b = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID2 and fabs(genpar->GetParentPdgId())>6) if(dauV2a==NULL) { dauV2a = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID2 and fabs(genpar->GetParentPdgId())>6) if(dauV2b==NULL and genpar!=dauV2a) { dauV2b = genpar; }

        // ** for ZbbZhadJJ
        if(fabs(genpar->GetPdgId()) == 5 and fabs(genpar->GetParentPdgId())==pdgID1 and fabs(genpar->GetParentPdgId())>6) V1isZbb=true;
        if(fabs(genpar->GetPdgId()) == 5 and fabs(genpar->GetParentPdgId())==pdgID2 and fabs(genpar->GetParentPdgId())>6) V2isZbb=true;

    }

    if (label.find("WWjj_SS_ll") !=string::npos || label.find("WWjj_SS_lt") !=string::npos || label.find("WWjj_SS_tt") !=string::npos ) {
        if(dauV1a!=NULL and dauV1b!=NULL) {
            //            std::cout << " dauV1a->Pt()= " << dauV1a->Pt() << " dauV1b->Pt()= " << dauV1b->Pt() << std::endl;
            if(dauV1a->Pt()>0 and dauV1b->Pt()>0 ) dauRatioV1 = (dauV1a->Pt()<dauV1b->Pt()) ? dauV1a->Pt()/dauV1b->Pt() : dauV1b->Pt()/dauV1a->Pt();
            cosThetaV1 = ChargedHiggs::CosThetaStar(&dauV1a->GetP4(), &dauV1b->GetP4());
        }
        if(dauV2a!=NULL and dauV2b!=NULL) {
            //            std::cout << " dauV2a->Pt()= " << dauV2a->Pt() << " dauV2b->Pt()= " << dauV2b->Pt() <<std::endl;
            if(dauV2a->Pt()>0 and dauV2b->Pt()>0 )  dauRatioV2 = (dauV2a->Pt()<dauV2b->Pt()) ? dauV2a->Pt()/dauV2b->Pt() : dauV2b->Pt()/dauV2a->Pt();
            cosThetaV2 = ChargedHiggs::CosThetaStar(&dauV2a->GetP4(), &dauV2b->GetP4());
        }
    }

    if(genVp!=NULL and genVp2!=NULL) {
        if(doMETAnalysis) {
            //            evt_MVV_gen = ChargedHiggs::mt(genVp->GetP4().Pt(),genVp2->GetP4().Pt(),genVp->GetP4().Phi(),genVp2->GetP4().Phi());
            evt_MVV_gen = ChargedHiggs::mtMassive(genVp->GetP4(),genVp2->GetP4());
            //            evt_MVV_gen = (genVp->GetP4()+genVp2->GetP4()).Mt();
        } else {
            evt_MVV_gen = (genVp->GetP4()+genVp2->GetP4()).M();
        }
    }

    return;

    int topology=0;
    if(genVp!=NULL and genVp2!=NULL) {
        if(genVp->Pt() > 200 and genVp2->Pt() > 200) topology=1;
        if((genVp->Pt() > 200 and genVp2->Pt() < 200) || (genVp->Pt() < 200 and genVp2->Pt() > 200) ) topology=2;
    }

    // at least one boosted
    //    if(genWp==NULL or genWp2==NULL) return EVENT_NOT_USED;
    //    if(genWp->Pt() < 200 and genWp2->Pt() < 200) return EVENT_NOT_USED;

    // Get Njets 30
    vector<TLorentzVector> genJets;
    for(int ij =0; ;++ij)
        {
            GenJet *j = e->GetGenJet(ij);
            if (j==NULL) break;
            if (j->Pt() < 30 ) continue;
            if (abs(j->Eta()) > 5 ) continue;
            //            jets.push_back( j->GetP4() ) ;
        }

}

void VBShadAnalysis::studyTriggers(Event* e, string category, string label, string systname )
{
    //init
    bool passtriggerbtag1 = false;
    bool passtriggerbtag2 = false;
    bool passtriggerbtag3 = false;
    bool passtriggerbtag4 = false;
    bool passtriggerbtag5 = false;
    bool passtriggerbtag6 = false;
    bool passtriggerbtag7 = false;
    bool passtriggerbtag8 = false;
    bool passtriggerbtag9 = false;

    bool passtriggerHad1 = false;
    bool passtriggerHad2 = false;
    bool passtriggerHad3 = false;
    bool passtriggerHad4 = false;
    bool passtriggerHad5 = false;
    bool passtriggerHad6 = false;
    bool passtriggerHad7 = false;


    bool passtriggermet1 = false;
    bool passtriggermet2 = false;


    switch(year){
        case 2016:  passtriggerHad1 = e->IsTriggered("HLT_PFHT900");
                    passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450");
                    passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50");
                    passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30");
                    passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30");
                    passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5");
                    passtriggerHad7 = e->IsTriggered("HLT_PFJet450");
                    passtriggerbtag1 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6");
                    passtriggerbtag2 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160");
                    passtriggerbtag3 = e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240");
                    passtriggerbtag4 = e->IsTriggered("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087");
                    passtriggerbtag5 = e->IsTriggered("HLT_PFHT450_SixJet40_BTagCSV_p056");
                    passtriggerbtag6 = e->IsTriggered("HLT_PFHT400_SixJet30_DoubleBTagCSV_p056");
                    passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
                    break;

        case 12016:  passtriggerHad1 = e->IsTriggered("HLT_PFHT900");
                    passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450");
                    passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50");
                    passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30");
                    passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30");
                    passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5");
                    passtriggerHad7 = e->IsTriggered("HLT_PFJet450");
                    passtriggerbtag1 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6");
                    passtriggerbtag2 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160");
                    passtriggerbtag3 = e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240");
                    passtriggerbtag4 = e->IsTriggered("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087");
                    passtriggerbtag5 = e->IsTriggered("HLT_PFHT450_SixJet40_BTagCSV_p056");
                    passtriggerbtag6 = e->IsTriggered("HLT_PFHT400_SixJet30_DoubleBTagCSV_p056");
                    passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
                    break;

        case 2017:  passtriggerHad1 = e->IsTriggered("HLT_PFHT1050"); 
                    passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet500");
                    passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT800_TrimMass50");
                    passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet400_TrimMass30");
                    passtriggerHad5 = e->IsTriggered("HLT_AK8PFJetFwd400");
                    passtriggerHad6 = e->IsTriggered("HLT_PFJet500");
                    passtriggerHad7 = e->IsTriggered("HLT_PFJetFwd400");
                    passtriggerbtag1 = e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"); 
                    passtriggerbtag2 = e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2");
                    passtriggerbtag3 = e->IsTriggered("HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5");
                    passtriggerbtag4 = e->IsTriggered("HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2");
                    passtriggerbtag5 = e->IsTriggered("HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1");
                    passtriggerbtag6 = e->IsTriggered("HLT_PFHT380_SixJet32_DoubleBTagCSV_p075");
                    passtriggerbtag7 = e->IsTriggered("HLT_PFHT430_SixJet40_BTagCSV_p080");
                    passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
                    passtriggermet2 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60");
                    break;

        case 2018:  passtriggerHad1 = e->IsTriggered("HLT_PFHT1050");
                    passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet500");
                    passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT800_TrimMass50");
                    passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet400_TrimMass30");
                    passtriggerHad5 = e->IsTriggered("HLT_PFJet500");
                    passtriggerHad6 = e->IsTriggered("HLT_AK8PFJetFwd200");
                    passtriggerHad7 = e->IsTriggered("HLT_PFJetFwd140");
                    passtriggerbtag1 = e->IsTriggered("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np2");
                    passtriggerbtag2 = e->IsTriggered("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_p02");
                    passtriggerbtag3 = e->IsTriggered("HLT_DoublePFJets200_CaloBTagDeepCSV_p71");
                    //passtriggerbtag4 = e->IsTriggered("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5");
                    //passtriggerbtag4 = e->IsTriggered("HLT_DoublePFJets116MaxDeta1p6_DoubleCaloBTagDeepCSV_p71");
                    passtriggerbtag4 = e->IsTriggered("HLT_QuadPFJet103_88_75_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1");
                    passtriggerbtag5 = e->IsTriggered("HLT_QuadPFJet103_88_75_15_PFBTagDeepCSV_1p3_VBF2");
                    passtriggerbtag8 = e->IsTriggered("HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94");
                    passtriggerbtag9 = e->IsTriggered("HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59");
                    passtriggerbtag6 = e->IsTriggered("HLT_PFHT430_SixPFJet40_PFBTagDeepCSV_1p5");
                    passtriggerbtag7 = e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2");
                    passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
                    passtriggermet2 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60");
                    break;

        default:    cout << "year incorrect!" << endl;
                    break;
    }

    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6 || passtriggerHad7;

    bool passtriggerbtagOR = passtriggerbtag1 || passtriggerbtag2 || passtriggerbtag3 || passtriggerbtag4 || passtriggerbtag5 || passtriggerbtag6 || passtriggerbtag7 || passtriggerbtag8 || passtriggerbtag9 || passtriggerHadOR;

    bool passtriggermetOR = passtriggermet1 || passtriggermet2;


    //Fill the histos
    //btag

    if(category.find("BBtag")   !=string::npos or category.find("RBtag")   !=string::npos){

        Fill("VBShadAnalysis/Baseline/mVV"+category+"_Base_" +label, systname, evt_MVV, e->weight() );
        Fill("VBShadAnalysis/Baseline/pTV1"+category+"_Base_" +label, systname, evt_PTV1, e->weight() );
        Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_Base_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
        Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_Base_" +label, systname, evt_MVV, evt_Mjj, e->weight() ); 


        if(passtriggerbtag1){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag1_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag1_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag1_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag1_" +label, systname, evt_MVV, evt_Mjj, e->weight() );} 
        if(passtriggerbtag2){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag2_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag2_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag2_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag2_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtag3){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag3_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag3_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag3_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag3_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtag4){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag4_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag4_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag4_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag4_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtag5){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag5_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag5_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag5_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag5_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtag6){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag6_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag6_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag6_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag6_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtag7){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtag7_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtag7_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtag7_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtag7_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHadOR){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHadOr_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHadOr_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHadOr_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHadOr_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerbtagOR){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerBtagOr_" +label, systname, evt_MVV, e->weight() );
                               Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerBtagOr_" +label, systname, evt_PTV1, e->weight() );
                               Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerBtagOr_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                               Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerBtagOr_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
    }


    // - HAD

    if( (category.find("BB")   !=string::npos or category.find("RB")   !=string::npos) and not (category.find("BBtag")   !=string::npos or category.find("RBtag")   !=string::npos) ){

        Fill("VBShadAnalysis/Baseline/mVV"+category+"_Base_" +label, systname, evt_MVV, e->weight() );
        Fill("VBShadAnalysis/Baseline/pTV1"+category+"_Base_" +label, systname, evt_PTV1, e->weight() );
        Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_Base_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
        Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_Base_" +label, systname, evt_MVV, evt_Mjj, e->weight() );

        if(passtriggerHad1){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad1_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad1_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad1_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad1_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad2){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad2_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad2_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad2_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad2_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad3){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad3_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad3_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad3_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad3_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad4){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad4_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad4_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad4_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad4_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad5){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad5_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad5_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad5_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad5_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad6){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad6_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad6_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad6_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad6_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHad7){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHad7_" +label, systname, evt_MVV, e->weight() );
                             Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHad7_" +label, systname, evt_PTV1, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHad7_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                             Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHad7_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
        if(passtriggerHadOR){ Fill("VBShadAnalysis/Baseline/mVV"+category+"_triggerHadOr_" +label, systname, evt_MVV, e->weight() );
                              Fill("VBShadAnalysis/Baseline/pTV1"+category+"_triggerHadOr_" +label, systname, evt_PTV1, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/pTV2V1"+category+"_triggerHadOr_" +label, systname, evt_PTV1, evt_PTV2, e->weight() );
                              Fill2D("VBShadAnalysis/Baseline/mJJvsmVV"+category+"_triggerHadOr_" +label, systname, evt_MVV, evt_Mjj, e->weight() );}
    }

    //met
    if ((category.find("RMET")   !=string::npos) || (category.find("BMET")   !=string::npos)){

        Fill("VBShadAnalysis/Baseline/mVV_MET_Base_" +label, systname, evt_MVV, e->weight() );

        if(passtriggermet1) Fill("VBShadAnalysis/Baseline/mVV_MET_triggerMet1_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermet2) Fill("VBShadAnalysis/Baseline/mVV_MET_triggerMet2_" +label, systname, evt_MVV, e->weight() );
        //if(passtriggermet3) Fill("VBShadAnalysis/Baseline/mVV_PFMET120_PFHT60_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermetOR) Fill("VBShadAnalysis/Baseline/mVV_MET_triggerMetOr_" +label, systname, evt_MVV, e->weight() );
    }


}

void VBShadAnalysis::getObjects(Event* e, string label, string systname )
{
    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and boson/forward jets
    //$$$$$$$$$
    //$$$$$$$$$

    //AK8 jet
    for(unsigned i=0;i<e->NFatJets() ; ++i)
    {

        FatJet *f=e->GetFatJet(i);

        //        std::cout << " Pt=" << f->Pt() << " SDMass=" << f->SDMass() << " IsZbbJet = " << f->IsZbbJet() << "  IsWJet = " << f->IsWJet()  << std::endl;

        if(doStudyMass){
        // Do mass corrections
        string range_j = "";
        switch(int(f->Pt()/100.)){
            case 2: range_j = "200_"; break;
            case 3: range_j = "300_"; break;
            case 4: range_j = "400_"; break;
            case 5: range_j = "500_"; break;
            case 6: range_j = "600_"; break;
            case 7: range_j = "700_"; break;
            default: range_j = "800plus_";
        }
        string eta_j = "";
        if(fabs(f->Eta()) < 1.3) eta_j = "BAR_";
        else eta_j = "END_";


        Double_t minDR1=9999;
        Double_t minDR2=9999;
        if(genVp!=NULL and genVp2!=NULL) {
            Double_t delW = f->DeltaR(*genVp);
            if (delW < minDR1){ minDR1 = delW;}
            Double_t delW2 = f->DeltaR(*genVp2);
            if (delW2 < minDR2){ minDR2 = delW2;}
        }


        float massRaw = (f->subjet_lead_p4 + f->subjet_sublead_p4).M();

        if(minDR1<0.8) Fill("VBShadAnalysis/Baseline/SubJetsMass1_"+eta_j+range_j+label, systname, massRaw , e->weight() ); 
        else if(minDR2<0.8) Fill("VBShadAnalysis/Baseline/SubJetsMass2_"+eta_j+range_j+label, systname, massRaw , e->weight() );


        if(minDR1<0.8 or minDR2<0.8) {
            // for both W and Zbb not necessarity is matched with one jet
            Fill("VBShadAnalysis/Baseline/PNetMass_FatJet_" +label, systname, f->PNetMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/SDMass_FatJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJet_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJet_" +label, systname, f->WvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZvsQCD_FatJet_" +label, systname, f->ZvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/TvsQCD_FatJet_" +label, systname, f->TvsQCD(), e->weight() );

            float diff = (f->SDMass() - f->M())/(f->SDMass() + f->M());
            if(fabs(diff)<0.1) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_lowDiff_" +label, systname, f->SDMass(), e->weight() );
            if(fabs(diff)>=0.1) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_largeDiff_" +label, systname, f->SDMass(), e->weight() );

            if(f->WvsQCD()>0 and f->TvsQCD()>0){
                double WvsT = f->WvsQCD()/ (f->WvsQCD() + f->TvsQCD());
                Fill("VBShadAnalysis/Baseline/WvsT_FatJet_" +label, systname, WvsT, e->weight() );
            }
            Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_" +label, systname, massRaw , e->weight() );
            Fill("VBShadAnalysis/Baseline/FatJetsMass_FatJet_" +label, systname, f->M() , e->weight() );

            Fill("VBShadAnalysis/Baseline/SubJetsDeepbTag_FatJet_" +label, systname, f->subjet_btagdeep , e->weight() );

            if(fabs(f->Eta()) < 1.3) {

                if(f->Pt()<400) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin1_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()<400) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin1_" +label, systname, massRaw , e->weight() );

                if(f->Pt()<600 and f->Pt()>=400) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin2_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()<600 and f->Pt()>=400) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin2_" +label, systname, massRaw , e->weight() );

                if(f->Pt()>600) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_barrel_bin3_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()>600) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_barrel_bin3_" +label, systname, massRaw , e->weight() );

            } else {

                if(f->Pt()<400) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin1_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()<400) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin1_" +label, systname, massRaw , e->weight() );

                if(f->Pt()<600 and f->Pt()>=400) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin2_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()<600 and f->Pt()>=400) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin2_" +label, systname, massRaw , e->weight() );

                if(f->Pt()>600) Fill("VBShadAnalysis/Baseline/SDMass_FatJet_endcap_bin3_" +label, systname, f->SDMass(), e->weight() );
                if(f->Pt()>600) Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJet_endcap_bin3_" +label, systname, massRaw , e->weight() );

            }

            //            if(topology==1) Fill("VBShadAnalysis/Baseline/pT_FatJet_BB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==2) Fill("VBShadAnalysis/Baseline/pT_FatJet_RB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==0) Fill("VBShadAnalysis/Baseline/pT_FatJet_RR_" +label, systname, f->Pt(), e->weight() );
        } else {
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJetFake_" +label, systname, f->WvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZvsQCD_FatJetFake_" +label, systname, f->ZvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/TvsQCD_FatJetFake_" +label, systname, f->TvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_" +label, systname, f->ZHccvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJetFake_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/SDMassOverPT_FatJetFake_" +label, systname, f->SDMass()/f->Pt(), e->weight() );

            //*
            if(fabs(f->Eta())<1.4) Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_barrel_" +label, systname, f->SDMass(), e->weight() );
            if(fabs(f->Eta())>=1.4) Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_endcap_" +label, systname, f->SDMass(), e->weight() );
            float diff = (f->SDMass() - f->M())/(f->SDMass() + f->M());
            if(fabs(diff)<0.1) Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_lowDiff_" +label, systname, f->SDMass(), e->weight() );
            if(fabs(diff)>=0.1) Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_largeDiff_" +label, systname, f->SDMass(), e->weight() );
            //*
            float massRaw = (f->subjet_lead_p4 + f->subjet_sublead_p4).M();
            Fill("VBShadAnalysis/Baseline/SubJetsMass_FatJetFake_" +label, systname, massRaw , e->weight() );
            Fill("VBShadAnalysis/Baseline/FatJetsMass_FatJetFake_" +label, systname, f->M() , e->weight() );

            Fill("VBShadAnalysis/Baseline/NSubJets_FatJetFake_" +label, systname, f->nSubjets, e->weight() );

            Fill("VBShadAnalysis/Baseline/SubJetsDeepbTag_FatJetFake_" +label, systname, f->subjet_btagdeep , e->weight() );

        }

        if((minDR1<0.8 and V1isZbb) || (minDR2<0.8 and V2isZbb)) {
            Fill("VBShadAnalysis/Baseline/SDMass_FatZbbJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/PNetMass_FatZbbJet_" +label, systname, f->PNetMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJet_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJet_" +label, systname, f->ZHccvsQCD(), e->weight() );
        } else {
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJetFake_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_" +label, systname, f->ZHccvsQCD(), e->weight() );
        }

        } // massive AK8 -- mass/discr plots filling ends

        //some general mass plots
        Fill("VBShadAnalysis/Baseline/PNetMass_FatJet_Inclusive_" +label, systname, f->PNetMass(), e->weight() );


        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().GetP4().Phi()));
        if(usePuppi) dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().GetPuppiMetP4().Phi()));
        // dPhiFatMet in principle only for the final V candidates

        if((doMETAnalysis or doMETAntiAnalysis) and dPhiFatMet<0.4) continue; // should this be larger 0.8 ? or veto ?

        bool isZbbJet=false;
        bool isZbbJetWide=false;
        //        if(f->IsZbbJet(DEEP_AK8_ZHbb_MD_25, DEEP_AK8_ZHbb_MD_50)) isZbbJet=true;
        if(!useParticleNet and f->IsZbbJet(DEEP_AK8_ZHbb_MD_25, DEEP_AK8_ZHbb_MD_25)) isZbbJet=true;
        if(!useParticleNet and f->IsZbbJetWide(DEEP_AK8_ZHbb_MD_25, DEEP_AK8_ZHbb_MD_25)) isZbbJetWide=true;
        if(useParticleNet and f->IsZbbJet(ParticleNet_Xbb, ParticleNet_Xcc)) isZbbJet=true;
        if(useParticleNet and f->IsZbbJetWide(ParticleNet_Xbb, ParticleNet_Xcc)) isZbbJetWide=true;

        /****************** Logic ***************************
        if do HAD/HADanti/HADside:      selectedFatZbb  stores Zbbwide, for vetoing
        if do MET/METanti:              selectedFatZbb  stores Zbb_in,  for use
        if do METside:                  selectedFatZbb  stores Zbbwide, for use
        if do BHAD/BHADanti:            selectedFatZbb  stores Zbb_in,  for use
        if do BHADside/BHADantiside:    selectedFatZbb  stores Zbbwide, for use
        ***************************************************/

        //        bool cat_condition1 = !doHADAnalysis and !doHADAntiAnalysis and !doSideBand;
        //        if( (cat_condition1 and isZbbJet) or (!cat_condition1 and isZbbJetWide)  ) {
        if(isZbbJet) {
            selectedFatZbb.push_back(f);
            bosonBBDiscr.push_back(f->Xbb());
            bosonBBMass.push_back(f->rawMass(f->MASSTYPE,true));
            bosonBBTDiscr.push_back(f->TvsQCD());
            Fill("VBShadAnalysis/Baseline/pT_FatZbbJet_" +label, systname, f->Pt(), e->weight() );
        }

        if(isZbbJet) selectedFatZbbIn.push_back(f);
        if(isZbbJetWide) selectedFatZbbWide.push_back(f);

        bool isWJet=false;
        bool isWJetWide=false;
        bool isWJetOut=false;
        bool isWJetMirror=false;

        if(useParticleNet) {

            if(!doHADAntiAnalysis and !doMETAntiAnalysis and !doBAntiAnalysis) {
                if (f->IsWJet( ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM) or (!doResonant and f->IsZJet(ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM)) ) isWJet=true;
                if (f->IsWJetWide( ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM) or (!doResonant and f->IsZJetWide(ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM)) ) isWJetWide = true;

                // isWJetOut and isWJetMirror only for resolved ( veto bosted ABC when doing resolved)
                if (f->IsWJetOut( ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM) or (!doResonant and f->IsZJetOut(ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM))) isWJetOut = true;
                //isWJetMirror unused when doing the SR
                if (f->IsWJetMirror( ParticleNet_MEDIUM, 0.2 , ParticleNet_MEDIUM ) or (!doResonant and f->IsZJetMirror( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM ))) isWJetMirror=true;
            }

            if(doHADAntiAnalysis or doMETAntiAnalysis or doBAntiAnalysis) {
                if (f->IsWJetMirror( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM ) or (!doResonant and f->IsZJetMirror( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM ))) isWJet=true;
                if (f->IsWJetMirrorWide( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM) or (!doResonant and f->IsZJetMirrorWide(ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM)) ) isWJetWide = true;

                // regions B+D is excusive with A+C
                // Caution: When doing Anti, selectedMirrorFatJets stores SR fatjet, required to <2/<1(BB-anti/MET and B-anti) to be orthogonal
                if ( f->IsWJetWide( ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM ) or (!doResonant and f->IsZJetWide( ParticleNet_MEDIUM, ParticleNet_MEDIUM, ParticleNet_MEDIUM )) ) isWJetMirror=true;

                // IsWJetMirrorOut only for resolved
                if ( f->IsWJetMirrorOut( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM ) or (!doResonant and f->IsZJetMirrorOut( ParticleNet_MEDIUM, 0.2, ParticleNet_MEDIUM ))) isWJetOut=true;
            }
        } else {

            if(!doHADAntiAnalysis and !doMETAntiAnalysis and !doBAntiAnalysis) {
                if (f->IsWJet( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25) or (!doResonant and f->IsZJet(DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25)) ) isWJet=true;
                if (f->IsWJetWide( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25) or (!doResonant and f->IsZJetWide(DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25)) ) isWJetWide = true;

                // isWJetOut and isWJetMirror only for resolved ( veto bosted ABC when doing resolved)
                if (f->IsWJetOut( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25) or (!doResonant and f->IsZJetOut(DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25))) isWJetOut = true;
                //isWJetMirror unused when doing the SR
                if (f->IsWJetMirror( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ) or (!doResonant and f->IsZJetMirror( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ))) isWJetMirror=true;
            }

            if(doHADAntiAnalysis or doMETAntiAnalysis or doBAntiAnalysis) {
                if (f->IsWJetMirror( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ) or (!doResonant and f->IsZJetMirror( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ))) isWJet=true;
                if (f->IsWJetMirrorWide( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25) or (!doResonant and f->IsZJetMirrorWide(DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25)) ) isWJetWide = true;

                // regions B+D is excusive with A+C
                // Caution: When doing Anti, selectedMirrorFatJets stores SR fatjet, required to <2/<1(BB-anti/MET and B-anti) to be orthogonal
                if ( f->IsWJetWide( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25 ) or (!doResonant and f->IsZJetWide( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_1, DEEP_AK8_W_MD_25 )) ) isWJetMirror=true;

                // IsWJetMirrorOut only for resolved
                if ( f->IsWJetMirrorOut( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ) or (!doResonant and f->IsZJetMirrorOut( DEEP_AK8_W_MD_05, DEEP_AK8_W_MD_50, DEEP_AK8_W_MD_25 ))) isWJetOut=true;
            }
        }

        //////comments for above////////////////////////////////////        ////////////////////////////////////////////////////
        //// for SR-(like), i.e. discr > wp, in+out != wide ////////        //// for Anti, i.e. discr < wp, in+out == wide /////
        //// 50---------65------105---------155---  ////////////////        //// 50---------65------105---------155--- /////////
        ////  |-------------wide-------------|----  ////////////////        ////  |-------------wide-------------|---- /////////
        ////  |---out---|---in---|---out----------  ////////////////        ////  |---out---|---in---|---out-----|---- /////////
        //// i.e. wide cuts at 155 for sideband, out goes to inf////        //// while for Anti, no need to go to inf //////////
        //// out used solely for resolved regions to veto all fats//        //// so both cut at 155                   //////////
        ////////////////////////////////////////////////////////////        ////////////////////////////////////////////////////

        if(doMETAntiAnalysis)  doMETAnalysis=true;
        if(doBAntiAnalysis) doBAnalysis=true;
        if(doHADAntiAnalysis) doHADAnalysis=true;

        if( (!doSideBand and isWJet) or (doSideBand and isWJetWide) ) {
            //if(!doMETAnalysis and !doResonant and isZbbJet) continue;  //avoid selectedFatZbb except resonant; 
            // MARIA: the condition above we might need to add back
            //remove exceptions, as later in the category selection, already consider separately when Zbb>0 or fat>0, no need to count Zbb in fat TODO: to be condirmed with the resonant case 
            //            if(isZbbJetWide) continue;
            if(isZbbJet) continue;
            selectedFatJets.push_back(f);
            if(isWJet) selectedFatJetsIn.push_back(f);
            bosonBDiscr.push_back(f->subjet_btagdeep);
            // fixme: this need to be filled depending on the W vs Z. Think: what if pass both W and Z?
            //            bosonVDiscr.push_back(f->WvsQCD());
            bosonVDiscr.push_back(f->Xqq());
            bosonTDiscr.push_back(f->TvsQCD());
            bosonMass.push_back(f->rawMass(f->MASSTYPE));
            Fill("VBShadAnalysis/Baseline/pT_FatJet_" +label, systname, f->Pt(), e->weight() );
            Fill("VBShadAnalysis/Baseline/eta_FatJet_" +label, systname, f->Eta(), e->weight() );
            Fill("VBShadAnalysis/Baseline/DphiMETFat_" +label, systname, dPhiFatMet, e->weight() );

        }


        /**************************************************
        Select priority: discrimator goes before mass, i.e.
        Zbb (full) -> Vjj (full)
        **************************************************/
        //        if(isWJet and not isZbbJet) selectedFatJetsIn.push_back(f);
        //        if(isWJet and not isZbbJetWide) selectedFatJetsIn.push_back(f);
        //        if(isWJetWide and not isZbbJet) selectedFatJetsWide.push_back(f);
        //        if(isWJetOut and not isZbbJetWide) selectedFatJetsOut.push_back(f); //selectedFatJetsOut used for veto in the resolved
        //        if(isWJetMirror and not isZbbJetWide) selectedMirrorFatJets.push_back(f);

        if(isWJetOut and not isZbbJet) selectedFatJetsOut.push_back(f); //selectedFatJetsOut used for veto in the resolved
        if(isWJetMirror and not isZbbJet) selectedMirrorFatJets.push_back(f);
    }

    Fill("VBShadAnalysis/Baseline/NFatJet_" +label, systname, selectedFatJets.size(), e->weight() );

    //AK4
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);

        // COUNT additional b-veto (20 GeV-Medium)
        //        if (j->GetDeepB() > DEEP_B_MEDIUM) {
        if (j->GetDeepFlavB() > DEEP_Jet_MEDIUM) {
            if(selectedFatZbb.size()>0) {
                if( j->DeltaR(selectedFatZbb[0]) < 1.2 ) continue;
            }
            // second Zbb eventually only on the BBtag and for nonresonant
            if(!doResonant and doBAnalysis and selectedFatZbb.size()>1) {
                if( j->DeltaR(selectedFatZbb[1]) < 1.2 ) continue;
            }
            counterExtrabToVeto_++;
        }
    }

    int counter=0;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);
        // COUNT additional jets candidate for forward or resolved
        //        if (j->GetDeepB() > DEEP_B_MEDIUM) continue;
        if ( j->GetDeepFlavB() > DEEP_Jet_MEDIUM) continue;
        if ( j->Pt()<30 ) continue;

        Fill("VBShadAnalysis/Baseline/pT_Jet_" +label, systname, j->Pt(), e->weight() );

        if(selectedFatJets.size()>0) {
            if( j->DeltaR(selectedFatJets[0]) < 1.2 ) continue;
        }
        // the secondFatJet needed only for the BB
        if((doHADAnalysis or doHADAntiAnalysis) and selectedFatJets.size()>1) {
            if( j->DeltaR(selectedFatJets[1]) < 1.2 ) continue;
        }
        if(selectedFatZbb.size()>0) {
            if( j->DeltaR(selectedFatZbb[0]) < 1.2 ) continue;
        }
        // second Zbb eventually only on the BBtag and for nonresonant
        if(!doResonant and doBAnalysis and selectedFatZbb.size()>1) {
            if( j->DeltaR(selectedFatZbb[1]) < 1.2 ) continue;
        }

        counter++;
        float dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().GetP4().Phi()));
        if(usePuppi) dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().GetPuppiMetP4().Phi()));

        // first 2,4 jets in the BMET,RMET for met-jets aligned
        if(selectedFatJets.size()>0 and counter<3 and (dphi < minDPhi)) minDPhi = dphi;
        if(selectedFatJets.size()==0 and counter<5 and (dphi < minDPhi)) minDPhi = dphi;

        // do we want to do this for all jets or for the first 2 or 4 ?
        // HFjets event veto (anti aligned with MET)
        if( (TMath::Pi() - dphi) < 0.4 and fabs(j->Eta()) > 2.99) {
            //https://lathomas.web.cern.ch/lathomas/JetMETStuff/HFNoiseStudy/HFNoise_VBFHinv_Laurent.pdf
            //slide3
            if(j->hfcEtaEtaStripSize()>2) badHFjetVeto = true;

            if(fabs(j->Eta()) < 4) {
                //slide4 y = x - 0.02
                if(j->HFsigmaPhiPhi() < (j->HFsigmaEtaEta() - 0.02)) badHFjetVeto = true;
                if(j->HFsigmaPhiPhi() < 0.02 or j->HFsigmaEtaEta() < 0.02) badHFjetVeto = true;
            }

            // see also AN2021-094-v5 section 6
            if(fabs(j->Eta()) >= 4) {
                if(j->HFsigmaPhiPhi() < 0.2 or j->HFsigmaEtaEta() > 0.1) badHFjetVeto = true;
            }

        }

        /*
        for (auto const& fat : selectedFatJets) {
            if( j->DeltaR(fat) < 1.2 ) continue;
        }
        */
        //        if( j->DeltaR(genWp) < 1.2 ) continue;
        //        if( j->DeltaR(genWp2) < 1.2 ) continue;

        selectedJets.push_back(j);

    }

    Fill("VBShadAnalysis/Baseline/Dphimin_" +label, systname, minDPhi, e->weight() );
}


void VBShadAnalysis::VVRestObj(Event*e){

    evt_VVcenEta = -100;
    evt_VVDRV1j = -100;
    evt_VVnormPTVVjj = 0;

    boostVV*=0;
    TVector3 basezero(0.,0.,0.);

    TLorentzVector P4V1_VV, P4V2_VV, P4j1_VV, P4j2_VV;
    P4V1_VV*=0; P4V2_VV*=0; P4j1_VV*=0; P4j2_VV*=0;
    TLorentzVector basezero4(0.,0.,0.,0.);

    //BB
    if (doHADAnalysis or doHADAntiAnalysis){
        if(selectedFatJets.size()>1){
            boostVV = (selectedFatJets[0]->GetP4()+selectedFatJets[1]->GetP4()).BoostVector();
            //{  // To keep consistency, use reco VV uniformly.
            //if( (label.find("_EWK_") !=string::npos ) && genVp!=NULL and genVp2!=NULL) boostVV = (genVp->GetP4() + genVp2->GetP4()).BoostVector();        
            //else boostVV = (selectedFatJets[0]->GetP4()+selectedFatJets[1]->GetP4()).BoostVector();
            //}
            P4V1_VV = selectedFatJets[0]->GetP4();
            P4V2_VV = selectedFatJets[1]->GetP4();
            //evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),selectedFatJets[1]->Eta());
            //evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),selectedFatJets[1]->Eta());
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);

        }
    }
    //BBtag
    if (doBAnalysis or doBAntiAnalysis){
        if(selectedFatZbb.size()>0 and selectedFatJets.size()>0){
            boostVV = (selectedFatJets[0]->GetP4()+selectedFatZbb[0]->GetP4()).BoostVector();
                   
            P4V1_VV = selectedFatZbb[0]->GetP4();
            P4V2_VV = selectedFatJets[0]->GetP4();
        }
    }

    //BMET
    if (doMETAnalysis or doMETAntiAnalysis){
        if(selectedFatJets.size()>0 or selectedFatZbb.size()>0) {
            if(usePuppi) {
                if(selectedFatZbb.size()>0) boostVV = (e->GetMet().GetPuppiMetP4() + selectedFatZbb[0]->GetP4()).BoostVector();
                else if(selectedFatJets.size()>0) boostVV = (e->GetMet().GetPuppiMetP4() + selectedFatJets[0]->GetP4()).BoostVector();
                P4V1_VV = e->GetMet().GetPuppiMetP4();
                if(selectedFatZbb.size()>0) P4V2_VV = selectedFatZbb[0]->GetP4();
                else if(selectedFatJets.size()>0) P4V2_VV = selectedFatJets[0]->GetP4();
            } else {
                if(selectedFatZbb.size()>0) boostVV = (e->GetMet().GetP4() + selectedFatZbb[0]->GetP4()).BoostVector();
                else if(selectedFatJets.size()>0) boostVV = (e->GetMet().GetP4() + selectedFatJets[0]->GetP4()).BoostVector();
                P4V1_VV = e->GetMet().GetP4();
                if(selectedFatZbb.size()>0) P4V2_VV = selectedFatZbb[0]->GetP4();
                else if(selectedFatJets.size()>0) P4V2_VV = selectedFatJets[0]->GetP4();
            } 

        } 
    }

    if(forwardJets.size()>1){
        P4j1_VV = forwardJets[0]->GetP4();
        P4j2_VV = forwardJets[1]->GetP4();
    }


    if(boostVV != basezero && P4V1_VV != basezero4 && P4V2_VV != basezero4 && P4j1_VV != basezero4 && P4j2_VV != basezero4){

        boostVV = -boostVV;

        P4V1_VV.Boost(boostVV);
        P4V2_VV.Boost(boostVV);
        P4j1_VV.Boost(boostVV);
        P4j2_VV.Boost(boostVV);        

        float VV_EtaMinV = std::min(P4V1_VV.Eta(),P4V2_VV.Eta());
        float VV_EtaMaxV = std::max(P4V1_VV.Eta(),P4V2_VV.Eta());

        evt_VVcenEta = std::min(
                           VV_EtaMinV - std::min(P4j1_VV.Eta(),P4j2_VV.Eta()),
                           std::max(P4j1_VV.Eta(),P4j2_VV.Eta()) - VV_EtaMaxV
                           ) ;
        evt_VVDRV1j = std::min(P4V1_VV.DeltaR(P4j1_VV), P4V1_VV.DeltaR(P4j2_VV));

        evt_VVnormPTVVjj = fabs((P4V1_VV+P4V2_VV+P4j1_VV+P4j2_VV).Pt())/(P4V1_VV.Pt() + P4V2_VV.Pt() + P4j1_VV.Pt() + P4j2_VV.Pt());

    }   
        
}

void VBShadAnalysis::setTrainingTree(Event*e, string label, int fi, int fj, int vk, int vl) {

    SetTreeVar("j1_pT",  selectedJets[fi]->GetP4().Pt());
    SetTreeVar("j1_Eta", selectedJets[fi]->Eta());
    SetTreeVar("j1_Phi", selectedJets[fi]->Phi());
    SetTreeVar("j1_M",   selectedJets[fi]->M());
    SetTreeVar("j1_Unc",   sqrt((selectedJets[fi]->GetJESUnc())*(selectedJets[fi]->GetJESUnc()) + Getjetres(selectedJets[fi]) * Getjetres(selectedJets[fi])));
    SetTreeVar("j1_QGL",  selectedJets[fi]->QGL());
    SetTreeVar("j1_DeepB", selectedJets[fi]->GetDeepB()); // note: check the deepJet (also there is a cut on the mediumB already)
    SetTreeVar("j1_DeepC", selectedJets[fi]->GetDeepC());
    SetTreeVar("j1_PUDiscr", selectedJets[fi]->GetPuId());
    SetTreeVar("j1_Area", selectedJets[fi]->GetArea());
    SetTreeVar("j1_NEMF", selectedJets[fi]->GetNEMF());
    SetTreeVar("j1_CEMF", selectedJets[fi]->GetCEMF());
    SetTreeVar("j1_NHF", selectedJets[fi]->GetNHF());
    SetTreeVar("j1_CHF", selectedJets[fi]->GetCHF());


    SetTreeVar("j2_pT",  selectedJets[fj]->GetP4().Pt());
    SetTreeVar("j2_Eta", selectedJets[fj]->Eta());
    SetTreeVar("j2_Phi", selectedJets[fj]->Phi());
    SetTreeVar("j2_M",   selectedJets[fj]->M());
    SetTreeVar("j2_Unc",   sqrt((selectedJets[fj]->GetJESUnc())*(selectedJets[fj]->GetJESUnc()) + Getjetres(selectedJets[fj]) * Getjetres(selectedJets[fj])));
    SetTreeVar("j2_QGL",  selectedJets[fj]->QGL());
    SetTreeVar("j2_DeepB", selectedJets[fj]->GetDeepB());
    SetTreeVar("j2_DeepC", selectedJets[fj]->GetDeepC());
    SetTreeVar("j2_PUDiscr", selectedJets[fj]->GetPuId());
    SetTreeVar("j2_Area", selectedJets[fj]->GetArea());
    SetTreeVar("j2_NEMF", selectedJets[fj]->GetNEMF());
    SetTreeVar("j2_CEMF", selectedJets[fj]->GetCEMF());
    SetTreeVar("j2_NHF", selectedJets[fj]->GetNHF());
    SetTreeVar("j2_CHF", selectedJets[fj]->GetCHF());


    SetTreeVar("j3_pT",  selectedJets[vk]->GetP4().Pt());
    SetTreeVar("j3_Eta", selectedJets[vk]->Eta());
    SetTreeVar("j3_Phi", selectedJets[vk]->Phi());
    SetTreeVar("j3_M",   selectedJets[vk]->M());
    SetTreeVar("j3_Unc",   sqrt((selectedJets[vk]->GetJESUnc())*(selectedJets[vk]->GetJESUnc()) + Getjetres(selectedJets[vk]) * Getjetres(selectedJets[vk])));
    SetTreeVar("j3_QGL",  selectedJets[vk]->QGL());
    SetTreeVar("j3_DeepB", selectedJets[vk]->GetDeepB());
    SetTreeVar("j3_DeepC", selectedJets[vk]->GetDeepC());
    SetTreeVar("j3_PUDiscr", selectedJets[vk]->GetPuId());
    SetTreeVar("j3_Area", selectedJets[vk]->GetArea());
    SetTreeVar("j3_NEMF", selectedJets[vk]->GetNEMF());
    SetTreeVar("j3_CEMF", selectedJets[vk]->GetCEMF());
    SetTreeVar("j3_NHF", selectedJets[vk]->GetNHF());
    SetTreeVar("j3_CHF", selectedJets[vk]->GetCHF());


    SetTreeVar("j4_pT",  selectedJets[vl]->GetP4().Pt());
    SetTreeVar("j4_Eta", selectedJets[vl]->Eta());
    SetTreeVar("j4_Phi", selectedJets[vl]->Phi());
    SetTreeVar("j4_M",   selectedJets[vl]->M());
    SetTreeVar("j4_Unc",   sqrt((selectedJets[vl]->GetJESUnc())*(selectedJets[vl]->GetJESUnc()) + Getjetres(selectedJets[vl]) * Getjetres(selectedJets[vl])));
    SetTreeVar("j4_QGL",  selectedJets[vl]->QGL());
    SetTreeVar("j4_DeepB", selectedJets[vl]->GetDeepB());
    SetTreeVar("j4_DeepC", selectedJets[vl]->GetDeepC());
    SetTreeVar("j4_PUDiscr", selectedJets[vl]->GetPuId());
    SetTreeVar("j4_Area", selectedJets[vl]->GetArea());
    SetTreeVar("j4_NEMF", selectedJets[vl]->GetNEMF());
    SetTreeVar("j4_CEMF", selectedJets[vl]->GetCEMF());
    SetTreeVar("j4_NHF", selectedJets[vl]->GetNHF());
    SetTreeVar("j4_CHF", selectedJets[vl]->GetCHF());


    SetTreeVar("DEta_f12", selectedJets[fi]->DeltaEta(selectedJets[fj]));
    SetTreeVar("DEta_v34", selectedJets[vk]->DeltaEta(selectedJets[vl]));
    SetTreeVar("DPhi_f12", ChargedHiggs::deltaPhi(selectedJets[fi]->Phi(), selectedJets[fj]->Phi()));
    SetTreeVar("DPhi_v34", ChargedHiggs::deltaPhi(selectedJets[vk]->Phi(), selectedJets[vl]->Phi()));
    SetTreeVar("DR_f12", selectedJets[fi]->DeltaR(selectedJets[fj]));
    SetTreeVar("DR_v34", selectedJets[vk]->DeltaR(selectedJets[vl]));
    SetTreeVar("M_f12",  selectedJets[fi]->InvMass(selectedJets[fj]));
    SetTreeVar("M_v34",  selectedJets[vk]->InvMass(selectedJets[vl]));
    SetTreeVar("pT_f12", (selectedJets[fi]->GetP4()+selectedJets[fj]->GetP4()).Pt());
    SetTreeVar("pT_v34", (selectedJets[vk]->GetP4()+selectedJets[vl]->GetP4()).Pt());



    SetTreeVar("EvtNum",e->eventNum());
    SetTreeVar("Weights",e->weight());

    bool match_all = false;
    bool match_V = false;
    bool match_f = false;

    float dr1V1j = -1;
    float dr1V2j = -1;
    float dr2V1j = -1;
    float dr2V2j = -1;
    float dr3V1j = -1;
    float dr3V2j = -1;
    float dr4V1j = -1;
    float dr4V2j = -1;
    float dr1V1jLHE = -1;
    float dr1V2jLHE = -1;
    float dr2V1jLHE = -1;
    float dr2V2jLHE = -1;
    float dr3V1jLHE = -1;
    float dr3V2jLHE = -1;
    float dr4V1jLHE = -1;
    float dr4V2jLHE = -1;

    if(checkSignalLabel(label)) {

        bool matchi = false;
        bool matchj = false;
        bool matchk = false;
        bool matchl = false;

        int ind_i = -1;
        int ind_j = -1;
        int ind_k = -1;
        int ind_l = -1;

        GenParticle *V1j = NULL;
        GenParticle *V2j = NULL;
        GenParticle *V1jLHE = NULL;
        GenParticle *V2jLHE = NULL;
        //cout << "new event !!! " << e->NGenPar() << endl;

        int MatchParentID = 24;
        //if((label.find("ZnnZhadJJ_EWK") !=string::npos) || (label.find("ZbbZhadJJ_EWK")!=string::npos )) MatchParentID = 23;
        if((label.find("ZNUNUZJJjj_EWK") !=string::npos) || (label.find("ZJJZJJjj_EWK")!=string::npos ) || (label.find("WPLEPZHADjj_EWK")!=string::npos ) || (label.find("WMLEPZHADjj_EWK")!=string::npos )) MatchParentID = 23;

        for(Int_t g = 0; g < e->NGenPar(); g++){

            GenParticle *genpar = e->GetGenParticle(g);

            /*
            if(fabs(genpar->GetPdgId()) == 24 and fabs(genpar->GetParentPdgId()) < 6){

                cout << g << "  W: " << genpar->GetPdgId() << endl;
                cout << "W s'par: " << genpar->GetParentPdgId() << endl;
            }
             

            if(fabs(genpar->GetPdgId()) < 6 and genpar->GetParentPdgId() != 0 and fabs(genpar->GetParentPdgId()) < 5){
                cout << g << "  quark: " << genpar->GetPdgId() << endl;
                cout << "quark s'par: " << genpar->GetParentPdgId() << endl;
                 cout << "pT: " << genpar->Pt() << ", " << genpar->Eta() << endl;
            }
            */                        
    
            if(fabs(genpar->GetPdgId()) < 6 and fabs(genpar->GetParentPdgId()) == MatchParentID){
                //cout << "from W: " << endl;
                //cout << g << ": " << genpar->GetPdgId() << endl;
                //cout << "par: " << genpar->GetParentPdgId() << endl;
                //cout << "pT: " << genpar->Pt() << ", " << genpar->Eta() << endl;
                if(V1j == NULL) V1j = genpar;
                else if(V2j == NULL) V2j = genpar;
            }
               

            if( ! genpar->IsLHE()) continue;

            //Use ParentID match for V jets
            //if( (g == e->NGenPar() - 4 or g == e->NGenPar() - 3) and selectedJets[vk]->GetP4().DeltaR(genpar->GetP4()) < 0.3  ) matchk = true;
            //if( (g == e->NGenPar() - 4 or g == e->NGenPar() - 3) and selectedJets[vl]->GetP4().DeltaR(genpar->GetP4()) < 0.3  ) matchl = true;

            if( (g == e->NGenPar() - 2 or g == e->NGenPar() - 1) and selectedJets[fi]->GetP4().DeltaR(genpar->GetP4()) < 0.4  ) matchi = true;
            if( (g == e->NGenPar() - 2 or g == e->NGenPar() - 1) and selectedJets[fj]->GetP4().DeltaR(genpar->GetP4()) < 0.4  ) matchj = true;    

            if(g == e->NGenPar() - 2) V1jLHE = genpar;
            if(g == e->NGenPar() - 1) V2jLHE = genpar;

            /*
            cout <<  "debug LHE: " << endl;
            cout << g << ": " << genpar->GetPdgId() << endl;
            cout << "par: " << genpar->GetParentPdgId() << endl;
            cout << "pT: " << genpar->Pt() << ", " << genpar->Eta() << endl;
            //cout << "v1j: " << genpar->GetP4().DeltaR(V1j->GetP4()) << endl;
            //cout << "v2j: " << genpar->GetP4().DeltaR(V2j->GetP4()) << endl;
            //if(g == e->NGenPar() - 4 && genpar->GetP4().DeltaR(V1j->GetP4()) > 0.3) cout << "WRONG ORDER 1" << endl; 
            //if(g == e->NGenPar() - 3 && genpar->GetP4().DeltaR(V2j->GetP4()) > 0.3) cout << "WRONG ORDER 2" << endl;
            */
        }
   
        if(V1j != NULL && V2j != NULL){ 
            dr1V1j = selectedJets[fi]->GetP4().DeltaR(V1j->GetP4());
            dr1V2j = selectedJets[fi]->GetP4().DeltaR(V2j->GetP4());
            dr2V1j = selectedJets[fj]->GetP4().DeltaR(V1j->GetP4());
            dr2V2j = selectedJets[fj]->GetP4().DeltaR(V2j->GetP4());
            dr3V1j = selectedJets[vk]->GetP4().DeltaR(V1j->GetP4());
            dr3V2j = selectedJets[vk]->GetP4().DeltaR(V2j->GetP4());
            dr4V1j = selectedJets[vl]->GetP4().DeltaR(V1j->GetP4());
            dr4V2j = selectedJets[vl]->GetP4().DeltaR(V2j->GetP4());  
        }


        if(V1jLHE != NULL && V2jLHE != NULL){
            dr1V1jLHE = selectedJets[fi]->GetP4().DeltaR(V1jLHE->GetP4());
            dr1V2jLHE = selectedJets[fi]->GetP4().DeltaR(V2jLHE->GetP4());
            dr2V1jLHE = selectedJets[fj]->GetP4().DeltaR(V1jLHE->GetP4());
            dr2V2jLHE = selectedJets[fj]->GetP4().DeltaR(V2jLHE->GetP4());
            dr3V1jLHE = selectedJets[vk]->GetP4().DeltaR(V1jLHE->GetP4());
            dr3V2jLHE = selectedJets[vk]->GetP4().DeltaR(V2jLHE->GetP4());
            dr4V1jLHE = selectedJets[vl]->GetP4().DeltaR(V1jLHE->GetP4());
            dr4V2jLHE = selectedJets[vl]->GetP4().DeltaR(V2jLHE->GetP4());
        }

        if(dr1V1j < 0.4 or dr1V2j < 0.4) matchk = true;
        if(dr2V1j < 0.4 or dr2V2j < 0.4) matchl = true;


        match_all = matchi && matchj && matchk && matchl;
        match_V   = matchk && matchl;
        match_f   = matchi && matchj; 

    }

    SetTreeVar("j1_f1jLHE",dr1V1jLHE);
    SetTreeVar("j1_f2jLHE",dr1V2jLHE);
    SetTreeVar("j2_f1jLHE",dr2V1jLHE);
    SetTreeVar("j2_f2jLHE",dr2V2jLHE);
    SetTreeVar("j3_f1jLHE",dr3V1jLHE);
    SetTreeVar("j3_f2jLHE",dr3V2jLHE);
    SetTreeVar("j4_f1jLHE",dr4V1jLHE);
    SetTreeVar("j4_f2jLHE",dr4V2jLHE);

    SetTreeVar("j1_V1j",dr1V1j);
    SetTreeVar("j1_V2j",dr1V2j);
    SetTreeVar("j2_V1j",dr2V1j);
    SetTreeVar("j2_V2j",dr2V2j);
    SetTreeVar("j3_V1j",dr3V1j);
    SetTreeVar("j3_V2j",dr3V2j);
    SetTreeVar("j4_V1j",dr4V1j);
    SetTreeVar("j4_V2j",dr4V2j);

    SetTreeVar("fmatch",(int)match_f);
    SetTreeVar("Vmatch",(int)match_V); 
    SetTreeVar("Allmatch",(int)match_all);

    //cout << "Vmatch" << match_V << "Allmatch" << match_all << endl;

    int sigmc=0;
    /*
    // OLD FOR NERO
    if(label.find("WPhadWPhadJJ_EWK") !=string::npos ) sigmc = 1 ;
    if(label.find("ZbbZhadJJ_EWK") !=string::npos ) sigmc = 2 ;
    if(label.find("ZnnZhadJJ_EWK") !=string::npos ) sigmc = 3 ;
    if(label.find("ZNuNuWPMJJjj_EWK") !=string::npos ) sigmc = 4 ;
    if(label.find("ZNuNuWPMJJjj_QCD") !=string::npos ) sigmc = 5 ;
    if(label.find("ZbbWpmJJ_EWK") !=string::npos ) sigmc = 6 ;
    if(label.find("ZbbWpmJJ_QCD") !=string::npos ) sigmc = 7 ;
    */
    if(label.find("WPMJJWPMJJjj_EWK_LO") !=string::npos ) sigmc = 1 ;
    if(label.find("WPMJJWPMJJjj_QCD_LO") !=string::npos ) sigmc = 2 ;
    if(label.find("WPMJJWPMJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 3 ;
    if(label.find("WPJJWMJJjj_EWK_LO") !=string::npos ) sigmc = 4 ;
    if(label.find("WPJJWMJJjj_QCD_LO") !=string::npos ) sigmc = 5 ;
    if(label.find("WPJJWMJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 6 ;
    if(label.find("ZNuNuWPMJJjj_EWK_LO") !=string::npos ) sigmc = 7 ;
    if(label.find("ZNuNuWPMJJjj_QCD_LO") !=string::npos ) sigmc = 8 ;
    if(label.find("ZNuNuWPMJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 9 ;
    if(label.find("ZBBWPMJJjj_EWK_LO") !=string::npos ) sigmc = 10 ;
    if(label.find("ZBBWPMJJjj_QCD_LO") !=string::npos ) sigmc = 11 ;
    if(label.find("ZBBWPMJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 12 ;
    if(label.find("ZJJZJJjj_EWK_LO") !=string::npos ) sigmc = 13 ;
    if(label.find("ZJJZJJjj_QCD_LO") !=string::npos ) sigmc = 14 ;
    if(label.find("ZJJZJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 15 ;
    if(label.find("ZNUNUWPMJJjj_EWK") !=string::npos ) sigmc = 16 ;
    if(label.find("ZNUNUWPMJJjj_QCD") !=string::npos ) sigmc = 17 ;
    if(label.find("ZNUNUWPMJJjj_EWK_QCD") !=string::npos  ) sigmc = 18 ;
    if(label.find("ZNUNUZJJjj_EWK") !=string::npos ) sigmc = 19 ;
    if(label.find("ZNUNUZJJjj_QCD") !=string::npos ) sigmc = 20 ;
    if(label.find("ZNUNUZJJjj_EWK_QCD") !=string::npos ) sigmc = 21 ;
    if(label.find("ZJJNOBWPMJJjj_EWK_LO") !=string::npos ) sigmc = 22 ;
    if(label.find("ZJJNOBWPMJJjj_QCD_LO") !=string::npos ) sigmc = 23 ;
    if(label.find("ZJJNOBWPMJJjj_EWK_QCD_LO") !=string::npos ) sigmc = 24 ;

    if(label.find("WPHADWMLEPjj_EWK_LO") !=string::npos  ) sigmc = 25 ;
    if(label.find("WPLEPWMHADjj_EWK_LO") !=string::npos  ) sigmc = 26 ;
    if(label.find("WPLEPWPHADjj_EWK_LO") !=string::npos  ) sigmc = 27 ;
    if(label.find("WMLEPWMHADjj_EWK_LO") !=string::npos  ) sigmc = 28 ;
    if(label.find("WPLEPZHADjj_EWK_LO") !=string::npos  ) sigmc = 29 ;
    if(label.find("WMLEPZHADjj_EWK_LO") !=string::npos  ) sigmc = 30 ;
    if(label.find("WPHADWMLEPjj_QCD_LO") !=string::npos  ) sigmc = 31 ;
    if(label.find("WPLEPWMHADjj_QCD_LO") !=string::npos  ) sigmc = 32 ;
    if(label.find("WPLEPWPHADjj_QCD_LO") !=string::npos  ) sigmc = 33 ;
    if(label.find("WMLEPWMHADjj_QCD_LO") !=string::npos  ) sigmc = 34 ;
    if(label.find("WPLEPZHADjj_QCD_LO") !=string::npos  ) sigmc = 35 ;
    if(label.find("WMLEPZHADjj_QCD_LO") !=string::npos  ) sigmc = 36 ;

    SetTreeVar("MC",sigmc);

}


void VBShadAnalysis::setTree(Event*e, string label, string category )
{

    SetTreeVar("run",e->runNum());
    SetTreeVar("lumi",e->lumiNum());
    SetTreeVar("evt",e->eventNum());
    SetTreeVar("isRealData",e->IsRealData());
    SetTreeVar("year",year);

    SetTreeVar("weight",e->weight());
    SetTreeVar("xsec",e->GetWeight()->GetBareMCXsec());
    SetTreeVar("npv",e->Npv());

    // consider moving Njets to selectedJets.size() (remember to subtract 2 for the resolved)
    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NBJets",e->Bjets());
    SetTreeVar("NVetoJets",vetoJets.size());

    if(usePuppi) {
        SetTreeVar("met_pt",e->GetMet().GetPuppiMetP4().Pt());
        SetTreeVar("met_phi",e->GetMet().GetPuppiMetP4().Phi());
    } else {
        SetTreeVar("met_pt",e->GetMet().GetP4().Pt());
        SetTreeVar("met_phi",e->GetMet().GetP4().Phi());
    }
    //$$$$

    if (category.find("BB")   !=string::npos) SetTreeVar("ana_category",1);
    if (category.find("RB")   !=string::npos) SetTreeVar("ana_category",2);
    if (category.find("BMET") !=string::npos) SetTreeVar("ana_category",3);
    if (category.find("RMET") !=string::npos) SetTreeVar("ana_category",4);
    if (category.find("BBtag")   !=string::npos) SetTreeVar("ana_category",5);
    if (category.find("RBtag")   !=string::npos) SetTreeVar("ana_category",6);

    int mc=0;
    /* // OLD LABEL
    if(label.find("WPhadWPhadJJ_EWK") !=string::npos ) mc = 1 ;
    if(label.find("ZbbZhadJJ_EWK") !=string::npos ) mc = 2 ;
    if(label.find("ZnnZhadJJ_EWK") !=string::npos ) mc = 3 ;
    if(label.find("ZNuNuWPMJJjj_EWK") !=string::npos ) mc = 4 ;
    if(label.find("ZNuNuWPMJJjj_QCD") !=string::npos ) mc = 5 ;
    if(label.find("ZbbWpmJJ_EWK") !=string::npos ) mc = 6 ;
    if(label.find("ZbbWpmJJ_QCD") !=string::npos ) mc = 7 ;

    if(label.find("WWjj_SS_ll") !=string::npos ) mc = 8 ;
    if(label.find("WWjj_SS_lt") !=string::npos ) mc = 9 ;
    if(label.find("WWjj_SS_tt") !=string::npos ) mc = 10 ;
    */


    if(label.find("WPMJJWPMJJjj_EWK_LO") !=string::npos ) mc = 1 ;
    if(label.find("WPMJJWPMJJjj_QCD_LO") !=string::npos ) mc = 2 ;
    if(label.find("WPMJJWPMJJjj_EWK_QCD_LO") !=string::npos ) mc = 3 ;
    if(label.find("WPJJWMJJjj_EWK_LO") !=string::npos ) mc = 4 ;
    if(label.find("WPJJWMJJjj_QCD_LO") !=string::npos ) mc = 5 ;
    if(label.find("WPJJWMJJjj_EWK_QCD_LO") !=string::npos ) mc = 6 ;
    if(label.find("ZNuNuWPMJJjj_EWK_LO") !=string::npos ) mc = 7 ;
    if(label.find("ZNuNuWPMJJjj_QCD_LO") !=string::npos ) mc = 8 ;
    if(label.find("ZNuNuWPMJJjj_EWK_QCD_LO") !=string::npos ) mc = 9 ;
    if(label.find("ZBBWPMJJjj_EWK_LO") !=string::npos ) mc = 10 ;
    if(label.find("ZBBWPMJJjj_QCD_LO") !=string::npos ) mc = 11 ;
    if(label.find("ZBBWPMJJjj_EWK_QCD_LO") !=string::npos ) mc = 12 ;
    if(label.find("ZJJZJJjj_EWK_LO") !=string::npos ) mc = 13 ;
    if(label.find("ZJJZJJjj_QCD_LO") !=string::npos ) mc = 14 ;
    if(label.find("ZJJZJJjj_EWK_QCD_LO") !=string::npos ) mc = 15 ;
    if(label.find("ZNUNUWPMJJjj_EWK") !=string::npos ) mc = 16 ;
    if(label.find("ZNUNUWPMJJjj_QCD") !=string::npos ) mc = 17 ;
    if(label.find("ZNUNUWPMJJjj_EWK_QCD") !=string::npos  ) mc = 18 ;
    if(label.find("ZNUNUZJJjj_EWK") !=string::npos  ) mc = 19 ;
    if(label.find("ZNUNUZJJjj_QCD") !=string::npos   ) mc = 20 ;
    if(label.find("ZNUNUZJJjj_EWK_QCD") !=string::npos  ) mc = 21 ;
    if(label.find("ZJJNOBWPMJJjj_EWK_LO") !=string::npos  ) mc = 22 ;
    if(label.find("ZJJNOBWPMJJjj_QCD_LO") !=string::npos  ) mc = 23 ;
    if(label.find("ZJJNOBWPMJJjj_EWK_QCD_LO") !=string::npos  ) mc = 24 ;
    // semilep
    if(label.find("WPHADWMLEPjj_EWK_LO") !=string::npos  ) mc = 25 ;
    if(label.find("WPLEPWMHADjj_EWK_LO") !=string::npos  ) mc = 26 ;
    if(label.find("WPLEPWPHADjj_EWK_LO") !=string::npos  ) mc = 27 ;
    if(label.find("WMLEPWMHADjj_EWK_LO") !=string::npos  ) mc = 28 ;
    if(label.find("WPLEPZHADjj_EWK_LO") !=string::npos  ) mc = 29 ;
    if(label.find("WMLEPZHADjj_EWK_LO") !=string::npos  ) mc = 30 ;
    if(label.find("WPHADWMLEPjj_QCD_LO") !=string::npos  ) mc = 31 ;
    if(label.find("WPLEPWMHADjj_QCD_LO") !=string::npos  ) mc = 32 ;
    if(label.find("WPLEPWPHADjj_QCD_LO") !=string::npos  ) mc = 33 ;
    if(label.find("WMLEPWMHADjj_QCD_LO") !=string::npos  ) mc = 34 ;
    if(label.find("WPLEPZHADjj_QCD_LO") !=string::npos  ) mc = 35 ;
    if(label.find("WMLEPZHADjj_QCD_LO") !=string::npos  ) mc = 36 ;
    //
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ) mc = 51 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ) mc = 52 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ) mc = 53 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M3000") !=string::npos ) mc = 54 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_semilep_M1000") !=string::npos ) mc = 55 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_semilep_M2000") !=string::npos ) mc = 56 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1000") !=string::npos ) mc = 60 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Znn_M1500") !=string::npos ) mc = 61 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Znu_M2000") !=string::npos ) mc = 62 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1000") !=string::npos ) mc = 65 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M1500") !=string::npos ) mc = 66 ;
    if(label.find("SinglyChargedHiggsGMmodel_HWZ_Zbb_M2000") !=string::npos ) mc = 67 ;
    //
    if(label.find("aQGC_ZJJZJJjj") !=string::npos ) mc = 70 ;

    // multiboson
    if(label.find("MULTIBOSON") !=string::npos) mc = 100 ;
    if(label.find("TRIBOSON") !=string::npos) mc = 110 ;
    if(label.find("DIBOSON") !=string::npos) mc = 120 ;
    // with Top
    if(label.find("TT_TuneCP5") !=string::npos) mc =200 ;  //powheg
    //    if(label.find("TTTo2L2Nu") !=string::npos) mc =200 ; //powheg
    if(label.find("TTToHadronic") !=string::npos) mc =200 ;  //powheg
    if(label.find("TTToSemiLeptonic") !=string::npos) mc =200 ; //powheg 
    if(label.find("TT_Mtt") !=string::npos) mc =201 ; // powheg
    if(label.find("TTJets") !=string::npos) mc =202 ;  // amcatnloFXFX

    if(label.find("TTX") !=string::npos) mc =205 ;
    if(label.find("ST") !=string::npos) mc =210 ;
    // V+jets
    if(label.find("ZJetsToNuNu_HT") !=string::npos) mc = 300 ;
    if(label.find("WJetsToLNu_HT") !=string::npos) mc = 310 ;
    if(label.find("WJetsToLNu_Nj") !=string::npos) mc = 311 ;
    if(label.find("WJetsToLNu_Pt") !=string::npos) mc = 312 ;
    if(label.find("DY") !=string::npos) mc = 320 ;
    if(label.find("ZJetsToQQ") !=string::npos) mc = 330 ;
    if(label.find("WJetsToQQ") !=string::npos) mc = 340 ;

    if(label.find("QCD_HT") !=string::npos) mc =500 ;
    if(label.find("QCD_Inclusive") !=string::npos) mc =501 ;
    if(label.find("QCD_Pt") !=string::npos) mc =502 ;
    if(label.find("QCD_bEnriched_HT") !=string::npos) mc =503 ;
    if(label.find("BGenFilter") !=string::npos) mc =505 ;

    SetTreeVar("mc",mc);

    // $$$$ analysis variable
    SetTreeVar("varMjj",evt_Mjj);
    SetTreeVar("varDetajj",evt_Detajj);
    SetTreeVar("varDphijj",evt_Dphijj);
    SetTreeVar("varJet1Eta",evt_Jet1Eta);
    SetTreeVar("varJet1Pt",evt_Jet1Pt);
    SetTreeVar("varJet2Eta",evt_Jet2Eta);
    SetTreeVar("varJet2Pt",evt_Jet2Pt);
    SetTreeVar("j1Unc",evt_j1unc);
    SetTreeVar("j2Unc",evt_j2unc);
    SetTreeVar("j1QGL",evt_j1QGL);
    SetTreeVar("j2QGL",evt_j2QGL);



    SetTreeVar("genMVV",evt_MVV_gen);
    SetTreeVar("varMVV",evt_MVV);
    SetTreeVar("varMTVV",evt_MTVV);
    SetTreeVar("varPTVV",evt_PTVV);
    SetTreeVar("varPTV1",evt_PTV1);
    SetTreeVar("varPTV2",evt_PTV2);
    SetTreeVar("varDphiVV",evt_DphiVV);
    SetTreeVar("varDetaVV",evt_DetaVV);
    SetTreeVar("varPetaVV",evt_PetaVV);
    SetTreeVar("varEtaMinV",evt_EtaMinV);
    SetTreeVar("varEtaMaxV",evt_EtaMaxV);
    // Add varDphi VV

    SetTreeVar("varCen",evt_cenEta);
    SetTreeVar("varCenRes",evt_cenEtaRes);
    SetTreeVar("varzepVB",evt_zepVB);
    SetTreeVar("varzepVB1",evt_zepVB1);
    SetTreeVar("varzepVB2",evt_zepVB2);
    SetTreeVar("varzepVV",evt_zepVV);
    SetTreeVar("varzepV2j1",evt_zepV2j1);
    SetTreeVar("varzepV2j2",evt_zepV2j2);
    SetTreeVar("varDRV1j",evt_DRV1j);
    SetTreeVar("varDRV2j",evt_DRV2j);
    SetTreeVar("varnormPTVVjj",evt_normPTVVjj);
    SetTreeVar("varcenPTVVjj",evt_cenPTVVjj);
    SetTreeVar("varFW2j",evt_FW2);

    //VV -rest Frame
    SetTreeVar("varVVCen",evt_VVcenEta);
    SetTreeVar("varVVDRVj",evt_VVDRV1j);
    SetTreeVar("varVVnormPTVVjj",evt_VVnormPTVVjj);


    // BKG
    SetTreeVar("varmtop",evt_mtop);

    // boson Decay
    SetTreeVar("dauRatioV1",dauRatioV1);
    SetTreeVar("dauRatioV2",dauRatioV2);
    SetTreeVar("cosThetaV1",cosThetaV1);
    SetTreeVar("cosThetaV2",cosThetaV2);

    if(bosonJets.size()>1) SetTreeVar("bosV2j1Pt",bosonJets[0]->Pt());
    if(bosonJets.size()>1) SetTreeVar("bosV2j2Pt",bosonJets[1]->Pt());

    // boson Properties
    SetTreeVar("bosV1mass",evt_bosV1mass);
    SetTreeVar("bosV1Eta",evt_bosV1Eta);
    SetTreeVar("bosV1discr",evt_bosV1discr);
    SetTreeVar("bosV1tdiscr",evt_bosV1tdiscr);
    SetTreeVar("bosV1bdiscr",evt_bosV1bdiscr);
    SetTreeVar("bosV1Unc", evt_bosV1unc);
    SetTreeVar("bosV2mass",evt_bosV2mass);
    SetTreeVar("bosV2Eta",evt_bosV2Eta);
    SetTreeVar("bosV2discr",evt_bosV2discr);
    SetTreeVar("bosV2tdiscr",evt_bosV2tdiscr);
    SetTreeVar("bosV2bdiscr",evt_bosV2bdiscr);
    SetTreeVar("bosV2discr2nd",evt_bosV2discr2nd);
    SetTreeVar("bosV2Unc", evt_bosV2unc);
    SetTreeVar("bosV2chi2",evt_chi2_);
    SetTreeVar("bosV2dR",evt_DRV2);

    SetTreeVar("bosGen",int(evt_genmatch));

    SetTreeVar("index_f1", index_f1);
    SetTreeVar("index_f2", index_f2);
    SetTreeVar("index_v1", index_v1);
    SetTreeVar("index_v2", index_v2);

    // MVA
    SetTreeVar("BDTnoBnoMET",BDTnoBnoMET);
    SetTreeVar("BDTwithMET",BDTwithMET);
    SetTreeVar("BDTbtag",BDTbtag);

    if(doTMVA and (doBAnalysis or doBAntiAnalysis)) {
        SetTreeVar("DNNMultiRBtagEWK",bdt_multi[15]);
        SetTreeVar("DNNMultiRBtagQCD",bdt_multi[16]);
        SetTreeVar("DNNMultiRBtagBKG",bdt_multi[17]);
    }

   if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) {
        SetTreeVar("BDTMultiwithMETzz",bdt_multi[0]); // this crash ?? in BHAD and BMET
        SetTreeVar("BDTMultiwithMETwz",bdt_multi[1]);
        SetTreeVar("BDTMultiwithMETbkg",bdt_multi[2]);

        SetTreeVar("BDTMultiEWKchi2",bdt_multi[3]);
        SetTreeVar("BDTMultiQCDchi2",bdt_multi[4]);
        SetTreeVar("BDTMultiBKGchi2",bdt_multi[5]);

        SetTreeVar("BDTMultiEWKdnnC",bdt_multi[6]);
        SetTreeVar("BDTMultiQCDdnnC",bdt_multi[7]);
        SetTreeVar("BDTMultiBKGdnnC",bdt_multi[8]);

        SetTreeVar("BDTMultiEWKdnnM",bdt_multi[9]);
        SetTreeVar("BDTMultiQCDdnnM",bdt_multi[10]);
        SetTreeVar("BDTMultiBKGdnnM",bdt_multi[11]);

        SetTreeVar("DNNMultiEWKdnn",bdt_multi[12]);
        SetTreeVar("DNNMultiQCDdnn",bdt_multi[13]);
        SetTreeVar("DNNMultiBKGdnn",bdt_multi[14]);
    }

}


void VBShadAnalysis::reset() // reset private members
{
    selectedJets.clear();
    selectedFatJets.clear();
    //    selectedFatJetsWide.clear();
    selectedFatJetsIn.clear();
    selectedFatJetsOut.clear();
    selectedMirrorFatJets.clear();
    selectedFatZbb.clear();
    selectedFatZbbIn.clear();
    selectedFatZbbWide.clear();

    counterExtrabToVeto_=0;
    minDPhi=999.f;
    badHFjetVeto = false;

    genLep = NULL;
    genVp = NULL;
    genVp2 = NULL;
    dauV1a = NULL;
    dauV1b = NULL;
    dauV2a = NULL;
    dauV2b = NULL;

    /// MISSED BLOCK
    bosonVDiscr.clear();
    bosonTDiscr.clear();
    bosonBDiscr.clear();
    bosonMass.clear();
    bosonBBDiscr.clear();
    bosonBBMass.clear();
    bosonBBTDiscr.clear();

    V1isZbb=false;
    V2isZbb=false;
    evt_MVV_gen=-100;
    // END MISSED BLOCK

    bosonJets.clear();
    forwardJets.clear();
    vetoJets.clear();

    p4VV*=0;
    p4V1*=0;
    p4V2*=0;
    p4jj*=0;
    p4VVjj*=0;


    // below variables for the ntuples (REDUNDANT ??)

    evt_Mjj=-100;
    evt_Detajj=-100;
    evt_Dphijj=-100;
    evt_Jet1Eta=-100;
    evt_Jet1Pt=-100;
    evt_Jet2Eta=-100;
    evt_Jet2Pt=-100;

    evt_MTVV=-100;
    evt_MVV=-100;
    evt_DRV2=-100;
    evt_DphiVV=-100;
    evt_PetaVV=-100;
    evt_DetaVV=-100;
    evt_MVV_gen=-100;
    evt_PTVV=0;
    evt_PTV1=0;
    evt_PTV2=0;
    evt_EtaMinV=-100;
    evt_EtaMaxV=-100;

    evt_normPTVVjj=0;
    evt_cenPTVVjj=0;

    evt_mtop=0;

    evt_zepVB=-100;
    evt_zepVB1=-100;
    evt_zepVB2=-100;
    evt_zepV2=-100;
    evt_cenEta=-100;
    evt_cenEtaRes=-100;
    evt_zepVV=-100;
    evt_zepV2j1=-100;
    evt_zepV2j2=-100;
    evt_DRV1j=-100;
    evt_DRV2j=-100;
    evt_FW2=-100;

    evt_bosV1mass=-1;
    evt_bosV1Eta=-100;
    evt_bosV1discr=-1;
    evt_bosV1tdiscr=-1;
    evt_bosV1bdiscr=-1;
    evt_bosV1unc = 0;

    evt_bosV2mass=-1;
    evt_bosV2Eta=-100;
    evt_bosV2discr=-1;
    evt_bosV2tdiscr=-1;
    evt_bosV2discr2nd=-1;
    evt_bosV2unc = 0;
    evt_bosV2bdiscr=-1;

    evt_chi2_= -1;
    evt_maxDnn = 0.;
    evt_2ndmaxDnn = 0.;
    evt_v_score = 0.;
    evt_maxkeras = -999.;

    evt_j1unc = 0;
    evt_j2unc = 0;
    evt_j1QGL = -1;
    evt_j2QGL = -1;


    bosV2j2Pt=-100;
    bosV2j1Pt=-100;
    index_f1 = -1;
    index_f2 = -1;
    index_v1 = -1;
    index_v2 = -1;

    BDTnoBnoMET = -100;
    BDTwithMET = -100;
    BDTbtag = -100;
    MultiBDTwithMET = -100;

}


int VBShadAnalysis::analyze(Event *e, string systname)
{
    //** reset
    reset();

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );

    if( label == "QCD_HT" or label == "QCD_Pt" or label == "QCD_bEnriched_HT" or label == "BGenFilter" or
        label == "ZJetsToNuNu_HT" or label == "WJetsToLNu_HT" or label == "WJetsToLNu_Pt" or
        label == "Z1JetsToNuNu_M-50_LHEFilterPtZ" or label == "Z2JetsToNuNu_M-50_LHEFilterPtZ" or
        label == "WJetsToLNu_0J" or label == "WJetsToLNu_1J" or label == "WJetsToLNu_2J" or label == "WJetsToLNu_NJ" or
        label == "ZJetsToQQ" or label == "WJetsToQQ"
        ) Fill("VBShadAnalysis/GENERAL/LHEht_" +label, systname, e->GetLHEHT(), e->weight() );  //forQCDHT

    /*
    if ( label.find("EWKZ2Jets_ZToNuNu") !=string::npos) label = "ZJetsToNuNu";
    //    if ( label == "EWK_LNuJJ_MJJ-120") label = "WJetsToLNu";
    if ( label.find("EWKZ2Jets_ZToLL") !=string::npos) label = "DY";
    if ( label.find("EWKWMinus2Jets") !=string::npos) label = "WJetsToLNu";
    if ( label.find("EWKWPlus2Jets") !=string::npos) label = "WJetsToLNu";
    */

    if ( label.find("EWKWMinus2Jets") !=string::npos) label = "EWKW";
    if ( label.find("EWKWPlus2Jets") !=string::npos) label = "EWKW";

    /*
    // redefine labels
    if ( label == "WWW") label = "MULTIBOSON";
    if ( label == "WWZ") label = "MULTIBOSON";
    if ( label == "WZZ") label = "MULTIBOSON";
    if ( label == "ZZZ") label = "MULTIBOSON";
    if ( label == "WWTo") label = "MULTIBOSON";
    if ( label == "WZTo") label = "MULTIBOSON";
    if ( label == "ZZTo") label = "MULTIBOSON";
    */

    if( label.find("Z1JetsToNuNu_M-50_LHEFilterPtZ")  !=string::npos ) label = "ZJetsToNuNuPt";
    if( label.find("Z2JetsToNuNu_M-50_LHEFilterPtZ")  !=string::npos ) label = "ZJetsToNuNuPt";

    if( label.find("WJetsToLNu_0J")  !=string::npos) label = "WJetsToLNu_NJ";
    if( label.find("WJetsToLNu_1J")  !=string::npos) label = "WJetsToLNu_NJ";
    if( label.find("WJetsToLNu_2J")  !=string::npos) label = "WJetsToLNu_NJ";

    if ( label.find("WWW") !=string::npos) label = "TRIBOSON";
    if ( label.find("WWZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("WZZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("ZZZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("WWTo") !=string::npos) label = "DIBOSON";
    if ( label.find("WZTo") !=string::npos) label = "DIBOSON";
    if ( label.find("ZZTo") !=string::npos) label = "DIBOSON";
    if ( label.find("WW_TuneCP5") !=string::npos) label = "DIBOSON";
    if ( label.find("WZ_TuneCP5") !=string::npos) label = "DIBOSON";
    if ( label.find("ZZ_TuneCP5") !=string::npos) label = "DIBOSON";

    //    if ( label == "ST") label = "TT";
    if ( label.find("ttZ") !=string::npos) label = "TTX";
    if ( label.find("TTTT") !=string::npos) label = "TTX";
    if ( label.find("TTZ") !=string::npos) label = "TTX";
    if ( label.find("TTW") !=string::npos) label = "TTX";
    if ( label.find("TTG") !=string::npos) label = "TTX";
    if ( label.find("ttH") !=string::npos) label = "TTX";

    /* redefine labels
    if ( label.find("EWKZ2Jets_ZToNuNu") !=string::npos) label = "ZJetsToNuNu";
    //    if ( label == "EWK_LNuJJ_MJJ-120") label = "WJetsToLNu";
    if ( label.find("EWK_LLJJ_MLL-50_MJJ-120") !=string::npos) label = "DY";
    if ( label.find("EWKWMinus2Jets") !=string::npos) label = "WJetsToLNu";
    if ( label.find("EWKWPlus2Jets") !=string::npos) label = "WJetsToLNu";
    */
    //    if(label.find("ZJetsToQQ") !=string::npos) label = "DY";
    //    if(label.find("WJetsToQQ") !=string::npos) label = "WJetsToLNu"; ;

    //$$$$$$$$$
    //$$$$$$$$$ Merge and redefine TTbar
    //$$$$$$$$$

    // comment for now; need to evaluate the LEPTON veto

    if (not e->IsRealData() and (
        (label.find("TTTo2L2Nu") !=string::npos) or
        (label.find("TTToSemiLeptonic") !=string::npos) or
        (label.find("TTToHadronic") !=string::npos))
        and not e->ApplyMttReweight()) return EVENT_NOT_USED;

    if((label.find("TTTo2L2Nu") !=string::npos)
       or (label.find("TTToSemiLeptonic") !=string::npos)
       or (label.find("TTToHadronic") !=string::npos)
       or (label.find("TT_Mtt") !=string::npos)) {
        float Mtt = genMtt(e);
        Fill("VBShadAnalysis/GENERAL/Mtt_" +label, systname, Mtt, e->weight() );
    }

    if ( label.find("TT_Mtt-1000toInf_TuneCP5") !=string::npos) label = "TT_TuneCP5";
    if ( label.find("TT_Mtt-700to1000_TuneCP5") !=string::npos) label = "TT_TuneCP5";

    if ( label.find("TTTo2L2Nu") !=string::npos) label = "TT_TuneCP5";
    if ( label.find("TTToSemiLeptonic") !=string::npos) label = "TT_TuneCP5";
    if ( label.find("TTToHadronic") !=string::npos) label = "TT_TuneCP5";

    Fill("VBShadAnalysis/GENERAL/Mtt_" +label, systname, genMtt(e) , e->weight() );

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Final label is: " + label);

    //$$$$$$$$$
    //$$$$$$$$$ genStudies
    //$$$$$$$$$

    genStudies(e, label );

    /*
    // REMOVE TOP FOR THE OSWW or ZW - EWK
    bool foundTop = false;

    if(label.find("WPJJWMJJjj_EWK") !=string::npos ||
       label.find("ZNuNuWPMJJjj_EWK") !=string::npos ||
       label.find("ZBBWPMJJjj_EWK") !=string::npos ) {

        for(Int_t i = 0; i < e->NGenPar(); i++){
            GenParticle *genpar = e->GetGenParticle(i);
            //        if( ! genpar->IsLHE()) continue;
            if(abs(genpar->GetPdgId() == 6)) foundTop = true;
        }

        if (foundTop) return EVENT_NOT_USED;
    }
    */

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GenStudies. Done " );
    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Apply SF b-tag
    //$$$$$$$$$
    //$$$$$$$$$

    double btagsf=1;
    if (true) // deepJet-SF for passing loose,medium or tigth cuts
        {
            btagsf=e->ApplyBTagSF(3,year); //0 loose, 1 medium, 2 tight, 3 reshaping
            if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("BTag SF is %lf",btagsf));
        }

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build cutflow
    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 0, e->weight() );  //0--ALL
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 0, 1 );

    // TRIGGER STORY

    bool passtriggerMET = false;
    if  (year==2016 or year==12016) passtriggerMET = (e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight"));
    if  (year==2017 or year==2018) passtriggerMET = ( e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60"));
 
    bool passtriggerHad = false;

    if (doHADAnalysis or doHADAntiAnalysis) {
    if (year==2016 or year==12016) passtriggerHad = e->IsTriggered("HLT_PFHT900")
                        || e->IsTriggered("HLT_AK8PFJet450")
                        || e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50")
                        || e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30")
                        || e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5")
                        || e->IsTriggered("HLT_AK8PFJet360_TrimMass30")
                        || e->IsTriggered("HLT_PFJet450");

    if (year==2017 || year==2018) passtriggerHad = e->IsTriggered("HLT_PFHT1050")
                        || e->IsTriggered("HLT_AK8PFJet500")
                        || e->IsTriggered("HLT_AK8PFHT800_TrimMass50")
                        || e->IsTriggered("HLT_AK8PFJet400_TrimMass30")
                        || e->IsTriggered("HLT_PFJet500")
                        || e->IsTriggered("HLT_AK8PFHT750_TrimMass50")
                        || e->IsTriggered("HLT_AK8PFJet380_TrimMass30");
    }

    /*
      2017 - JetHT 
      HLT_DiPFJetAve160_HFJEC
      HLT_DiPFJetAve300_HFJEC 
      HLT_DiPFJetAve60_HFJEC_v13

      2018 - JetHT
      HLT_QuadPFJet103_88_75_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1
      HLT_QuadPFJet103_88_75_15_PFBTagDeepCSV_1p3_VBF2_v8
      HLT_DiPFJetAve80
    */

    bool passtriggerBtag = false;
    
    if(doBAnalysis or doBAntiAnalysis) {
    if (year==2016 or year==12016) passtriggerBtag = passtriggerHad
                         || e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6")
                         || e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160")
                         || e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240")
                         || e->IsTriggered("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087")
                         || e->IsTriggered("HLT_PFHT450_SixJet40_BTagCSV_p056")
                         || e->IsTriggered("HLT_PFHT400_SixJet30_DoubleBTagCSV_p056");

    if (year==2017) passtriggerBtag = passtriggerHad
                        || e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2")
                        || e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2")
                        || e->IsTriggered("HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5")
                        || e->IsTriggered("HLT_PFHT380_SixJet32_DoubleBTagCSV_p075")
                        || e->IsTriggered("HLT_PFHT430_SixJet40_BTagCSV_p080")
                        || e->IsTriggered("HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2")
                        || e->IsTriggered("HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1");

    if (year==2018) passtriggerBtag = passtriggerHad
                        || e->IsTriggered("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np2")
                        || e->IsTriggered("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_p02")
                        || e->IsTriggered("HLT_DoublePFJets200_CaloBTagDeepCSV_p71")
                        || e->IsTriggered("HLT_QuadPFJet103_88_75_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1")
                        || e->IsTriggered("HLT_QuadPFJet103_88_75_15_PFBTagDeepCSV_1p3_VBF2")
                        || e->IsTriggered("HLT_PFHT430_SixPFJet40_PFBTagDeepCSV_1p5")
                        || e->IsTriggered("HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2")
                        || e->IsTriggered("HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94")
                        || e->IsTriggered("HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59");
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Before tirgger" );
    
    
    if(!doTrigger){
      //    bool passtriggerHAD = (e->IsTriggered("HLT_PFHT_800_v") || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v"));
      if (doHADAnalysis or doHADAntiAnalysis) {
        if(!passtriggerHad) return EVENT_NOT_USED;
      }

      if(doMETAnalysis or doMETAntiAnalysis) {
      //    bool passtriggerMET = (e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
          if(!passtriggerMET) return EVENT_NOT_USED;
      }

      if(doBAnalysis or doBAntiAnalysis) {
        if(!passtriggerBtag) return EVENT_NOT_USED;
      }
    }
    
    //$$$$$$$$$$$
    //$$$$$$$$$$$

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","After tirgger" );
    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 1, e->weight() );  //1--trigger
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 1, 1 );

    // kill Top/W/Z
    if ( e->Nleps() > 0 ) return EVENT_NOT_USED;

    if( (label.find("TT_TuneCP5") !=string::npos) or (label.find("WJetsToLNu_HT") !=string::npos) ) {
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 15) Fill("VBShadAnalysis/Baseline/genTauPt_all_"+label, systname, genLep->Pt() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 15) Fill("VBShadAnalysis/Baseline/genTauEta_all_"+label, systname, genLep->Eta() , e->weight() );
    }

    if ( e->Ntaus() > 0 ) return EVENT_NOT_USED;

    if( (label.find("TT_TuneCP5") !=string::npos) or (label.find("WJetsToLNu_HT") !=string::npos) ) {
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 11) Fill("VBShadAnalysis/Baseline/genElePt_"+label, systname, genLep->Pt() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 11) Fill("VBShadAnalysis/Baseline/genEleEta_"+label, systname, genLep->Eta() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 13) Fill("VBShadAnalysis/Baseline/genMuoPt_"+label, systname, genLep->Pt() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 13) Fill("VBShadAnalysis/Baseline/genMuoEta_"+label, systname, genLep->Eta() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 15) Fill("VBShadAnalysis/Baseline/genTauPt_"+label, systname, genLep->Pt() , e->weight() );
        if(genLep!=NULL and fabs(genLep->GetPdgId()) == 15) Fill("VBShadAnalysis/Baseline/genTauEta_"+label, systname, genLep->Eta() , e->weight() );
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Lepton and taus veto" );

    //    if ( e->Nphotons() > 0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 2, e->weight() );  //2--lepVeto
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 2, 1 );

    Fill("VBShadAnalysis/Baseline/Met_" +label, systname, e->GetMet().GetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/Baseline/PuppiMet_" +label, systname, e->GetMet().GetPuppiMetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/Baseline/MetPhi_" +label, systname, e->GetMet().GetP4().Phi(), e->weight() );

    if(usePuppi) {
        // events with MET in separate category
        if ( (doMETAnalysis or doMETAntiAnalysis) and e->GetMet().GetPuppiMetP4().Pt() < 250 ) return EVENT_NOT_USED;
        if ( (!doMETAnalysis and !doMETAntiAnalysis) and e->GetMet().GetPuppiMetP4().Pt() > 200 ) return EVENT_NOT_USED;
    } else {
        // events with MET in separate category
        if ( (doMETAnalysis or doMETAntiAnalysis) and e->GetMet().GetP4().Pt() < 250 ) return EVENT_NOT_USED;
        if ( (!doMETAnalysis and !doMETAntiAnalysis) and e->GetMet().GetP4().Pt() > 200 ) return EVENT_NOT_USED;
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("Met Veto %f",e->GetMet().GetP4().Pt()) );
    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 3, e->weight() );  //3--vetoMET
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 3, 1 );

    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and boson/forward jets
    //$$$$$$$$$ After this point doMETAntiAnalysis is set if doMETAnalysis
    getObjects(e, label , systname);

    if (VERBOSE and doHADAnalysis)Log(__FUNCTION__,"DEBUG",Form("I have %d fatjets (>=1) and %d ZbbFat (0)",selectedFatJets.size(),selectedFatZbb.size()) );

    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatJets.size()<1 ) return EVENT_NOT_USED;
    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatZbb.size()>0 ) return EVENT_NOT_USED;
    if ( (doBAnalysis or doBAntiAnalysis) and selectedFatZbb.size()<1 ) return EVENT_NOT_USED;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","FatJet Selection" );
    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 4, e->weight() );  //4--NFatjet cut
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 4, 1 );

    // THIS IS NOT OK FOR THE RMET analysis
    //    if ( doMETAnalysis and selectedFatJets.size()<0 and selectedFatZbb.size()<0) return EVENT_NOT_USED;

    //$$$$$$$$$
    // events with B in another category
    // withMET
    // noMET with B
    // noMET noB

    // note: e->Bjets() contains the pt>20: can raise at pt>30 for category with MET and B
    //    if ( ( doHADAnalysis or doHADAntiAnalysis ) and e->Bjets() > 0 ) return EVENT_NOT_USED;
    //    if ( doBAnalysis and (e->Bjets() == 0 or e->Bjets()>2) ) return EVENT_NOT_USED;
    if ( doMETAnalysis and e->Bjets()>2 ) return EVENT_NOT_USED;
    if ( counterExtrabToVeto_>0 ) return EVENT_NOT_USED;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","b veto" );
    Fill("VBShadAnalysis/Baseline/NBJet_" +label, systname, e->Bjets(), e->weight() );

    if (VERBOSE and doMETAnalysis)Log(__FUNCTION__,"DEBUG",Form("MinDphi %f (0.4-2.74)",minDPhi ));

    if ( (doMETAnalysis or doMETAntiAnalysis) and minDPhi<0.4) return EVENT_NOT_USED;
    // to check if this anti-aligned is still needed for the generic jet, probably not on the resonant
    //    if ( doMETAnalysis and minDPhi!=999 and (TMath::Pi()-minDPhi)<0.4) return EVENT_NOT_USED;
    if ( (doMETAnalysis or doMETAntiAnalysis) and badHFjetVeto) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 5, e->weight() );  //5--vetoB+dPhiMET
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 5, 1 );

    if(doStudySFfat and selectedFatJets.size()>1 and selectedFatZbb.size()==0 and selectedJets.size()>1 ) {
        int indexij = 10*getIndex(0) + getIndex(1);
        Fill("VBShadAnalysis/Baseline/SF_FatJet_" +label, systname, indexij, e->weight() );  // fill to derive SF
        // Fill("VBShadAnalysis/Baseline/SF_FatJet_Mjj_" +label, systname, indexij, e->weight() );  // fill to derive SF
    }


    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build categories below
    //$$$$$$$$$
    //$$$$$$$$$

    string category="";
    // for forwardJets pick the one with the largest mass

    if(!doMETAnalysis and !doBAnalysis) {
        p4VV*=0;
        evt_MVV=0;
        evt_PTV1=0;
        evt_PTV2=0;
        evt_DetaVV=-100;
        evt_PetaVV=-100;
        // reset resolved
        evt_chi2_= -1;
        bosonJets.clear();
        forwardJets.clear();
        vetoJets.clear();


        //////////////////////////////////
        //50|---D---|---B---|---D---|155//
        //50|---C---|---A---|---C---|155//
        //////////////////////////////////

        // MARIA BB
        bool condition1_AvsC = doSideBand ? selectedFatJetsIn.size()<2 : selectedFatJetsIn.size()>1;
        bool condition2_AvsB = doHADAntiAnalysis ? selectedMirrorFatJets.size()<2 : 1; //when doing Anti, selectedMirrorFatJets stores SR fatjets, < 2 to be inclusive with SR

        if(selectedFatJets.size()>1 and selectedFatZbb.size()==0 and selectedJets.size()>1 and condition1_AvsC and condition2_AvsB) {
            category="_BB";
            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);

            TLorentzVector jet1P4;
            TLorentzVector jet2P4;

            evt_bosV1discr = bosonVDiscr[0];
            evt_bosV1tdiscr = bosonTDiscr[0];
            evt_bosV1mass = bosonMass[0];
            evt_bosV1bdiscr = bosonBDiscr[0];

            evt_bosV2discr = bosonVDiscr[1];
            evt_bosV2tdiscr = bosonTDiscr[1];
            evt_bosV2discr2nd = -1.;
            evt_bosV2mass = bosonMass[1];
            evt_bosV2bdiscr = bosonBDiscr[1];

            jet1P4.SetPtEtaPhiM(selectedFatJets[0]->Pt(),selectedFatJets[0]->Eta(),selectedFatJets[0]->Phi(),selectedFatJets[0]->rawMass(selectedFatJets[0]->MASSTYPE));
            jet2P4.SetPtEtaPhiM(selectedFatJets[1]->Pt(),selectedFatJets[1]->Eta(),selectedFatJets[1]->Phi(),selectedFatJets[1]->rawMass(selectedFatJets[1]->MASSTYPE));
            p4V1 = jet1P4;
            p4V2 = jet2P4;
            p4VV = jet1P4 + jet2P4;
            //p4VV = (selectedFatJets[0]->GetP4()+selectedFatJets[1]->GetP4());
            evt_MVV = p4VV.M();
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }

            evt_genmatch = genMatchResonant(e, label, category);
        }

        if(selectedFatJets.size()==1 and selectedJets.size()>3) {
            category="";
            double mBoson = 80.;
            double chi2Cut = 6.;
            float mWidth = 20.;

            float MV, chi2;
            std::tie(MV,chi2) = resolvedtagger(e, mBoson, label, systname, selectedFatJets[0]->Eta());

            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
            if( fabs(MV-mBoson) < mWidth and bosonJets.size()>1 and chi2<chi2Cut) {
                category="_RB";
                evt_bosV1discr = bosonVDiscr[0];
                evt_bosV1tdiscr = bosonTDiscr[0];
                evt_bosV1bdiscr = bosonBDiscr[0];
                evt_bosV1mass = bosonMass[0];
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_chi2_ = chi2;
                p4V1 = selectedFatJets[0]->GetP4();
                p4V2 = bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
                p4VV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4() );
                evt_MVV = p4VV.M();

            }
        }
    }


    //////////
    // with B

    if(doBAnalysis) {
        p4VV*=0;
        evt_MVV=-100;
        evt_PTV1=-100;
        evt_PTV2=-100;
        evt_DetaVV=-100;
        evt_PetaVV=-100;
        //reset the resolved
        evt_chi2_= -1;
        bosonJets.clear();
        forwardJets.clear();
        vetoJets.clear();

        category="";

        //////////////////////////////////
        //50|---D---|---B---|---D---|155//
        //50|---C---|---A---|---C---|155//
        //50|--Cres-|--Ares-|--Cres-|155//
        //////////////////////////////////

        //        bool condition1_AvsC = doSideBand ? ((selectedFatJetsIn.size()==0 or selectedFatZbbIn.size()==0) and selectedFatZbbIn.size()<2) : (selectedFatJetsIn.size()>0 or selectedFatZbbIn.size()>1);
        //        bool condition2_AvsB = doBAntiAnalysis ? (selectedMirrorFatJets.size()==0 and selectedFatZbbWide.size()==1) : 1; //when doing Anti, selectedMirrorFatJets stores SR Wide fatjets, ==0 to be inclusive with SR

        bool condition1_AvsC = doSideBand ? (selectedFatJetsIn.size()==0 and selectedFatZbb.size()==1): true;
        bool condition2_AvsB = doBAntiAnalysis ? (selectedMirrorFatJets.size()==0 and selectedFatZbb.size()==1): true;

        //MARIA BBtag
        if(  (selectedFatZbb.size()>1 or (selectedFatZbb.size()==1 and selectedFatJets.size()>0)) and selectedJets.size()>1 and condition1_AvsC and condition2_AvsB) {
            //        if( selectedFatZbb.size()==1 and selectedFatJets.size()>0 and selectedJets.size()>1 and condition1_AvsC and condition2_AvsB) {

            category="_BBtag";

            TLorentzVector jet1P4;
            TLorentzVector jet2P4;
            Fill("VBShadAnalysis/Baseline/pT_BJet_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );

            jet1P4.SetPtEtaPhiM(selectedFatZbb[0]->Pt(),selectedFatZbb[0]->Eta(),selectedFatZbb[0]->Phi(),selectedFatZbb[0]->rawMass(selectedFatZbb[0]->MASSTYPE,true));
            if(selectedFatZbb.size()>1) jet2P4.SetPtEtaPhiM(selectedFatZbb[1]->Pt(),selectedFatZbb[1]->Eta(),selectedFatZbb[1]->Phi(),selectedFatZbb[1]->rawMass(selectedFatZbb[1]->MASSTYPE,true));
            else jet2P4.SetPtEtaPhiM(selectedFatJets[0]->Pt(),selectedFatJets[0]->Eta(),selectedFatJets[0]->Phi(),selectedFatJets[0]->rawMass(selectedFatJets[0]->MASSTYPE));
        
            p4VV = jet1P4 + jet2P4;
            p4V1 = jet1P4;
            p4V2 = jet2P4;            
            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
            //p4VV = (selectedFatJets[0]->GetP4()+selectedFatZbb[0]->GetP4());

            evt_MVV = p4VV.M();

            evt_bosV1discr = bosonBBDiscr[0];
            evt_bosV1mass = bosonBBMass[0];
            evt_bosV1unc = 0;
            evt_bosV1tdiscr = bosonBBTDiscr[0];
            evt_bosV1bdiscr = -1;
            if(selectedFatZbb.size()>1){
                evt_bosV2discr = bosonBBDiscr[1];
                evt_bosV2mass = bosonBBMass[1];
                evt_bosV2tdiscr = bosonBBTDiscr[1];
                evt_bosV2bdiscr = -1.;
            }else{
                evt_bosV2discr = bosonVDiscr[0];
                evt_bosV2tdiscr = bosonTDiscr[0];
                evt_bosV2bdiscr = bosonBDiscr[0];
                evt_bosV2mass = bosonMass[0];
            }
            evt_bosV2discr2nd = -1.;
            evt_bosV2unc = 0;
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }

            evt_genmatch = genMatchResonant(e, label, category);
        }

        // target the ZbbZqq + ZbbWqq resolved
        if(selectedFatZbb.size()==1 and /*selectedFatZbbWide.size()>0 and*/ selectedFatJets.size()==0 and selectedFatJetsOut.size() == 0 and selectedMirrorFatJets.size()==0 and selectedJets.size()>3) {
        // Notice the selection here: the 2nd wide boosted object (can be Zbb_wide or Vjj_wide) is vetoed.    

            category="";

            ///////$$$$$$$
            ///////$$$$$$$ evaluate both W and Z hypothesis
            ///////$$$$$$$

            double MV = 0;
            double chi2 = 9999.;
            float mBoson = 0;
            int mBoson_ind = -1;

            float mBoson_W=80.;
            float mBoson_Z=91.;

            if (!doResTagTMVA && !doResTagKeras) {

                string genWmat = "wrong_";
                string genZmat = "wrong_";

                float MV_W, mW_chi2;
                std::tie(MV_W,mW_chi2) = resolvedtagger(e, mBoson_W, label, systname, selectedFatZbb[0]->Eta()); if(genMatchResolved(e,systname,label)) genWmat = "right_";

                if( (bosonJets.size()>1) && (forwardJets.size() > 1)  && ( forwardJets[0]->Eta() * forwardJets[1]->Eta() <0 ) && (fabs(forwardJets[0]->DeltaEta(forwardJets[1])) > 3.) && ((forwardJets[0]->GetP4() + forwardJets[1]->GetP4()).M() > 500) ){
                //if(bosonJets.size()>1){
                    Fill2D("VBShadAnalysis/Baseline/ResWMassChi2_"+label, systname, MV_W, mW_chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResWMass_"+genWmat+label, systname, MV_W, e->weight() );
                }
                  
                float MV_Z, mZ_chi2;
                std::tie(MV_Z,mZ_chi2) = resolvedtagger(e, mBoson_Z, label, systname, selectedFatZbb[0]->Eta()); if(genMatchResolved(e,systname,label)) genZmat = "right_";
                if(bosonJets.size()>1){
                    Fill2D("VBShadAnalysis/Baseline/ResZMassChi2_"+label, systname, MV_Z, mZ_chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResZMass_"+genZmat+label, systname, MV_Z, e->weight() );
                }

                if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2Diff_W_"+genWmat+label, systname, mW_chi2-mZ_chi2, e->weight() );
                if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2Diff_Z_"+genZmat+label, systname, mZ_chi2-mW_chi2, e->weight() );

                mBoson = mBoson_Z;
                MV = MV_Z;
                chi2 = mZ_chi2;

                if(mW_chi2 < mZ_chi2){
                    mBoson = mBoson_W;
                    std::tie(MV,chi2) = resolvedtagger(e, mBoson_W, label, systname, selectedFatZbb[0]->Eta());
                }
                
            }
            //******************//

            if(doResTagKeras or doResTagTMVA){
                std::tie(MV,mBoson_ind) = resolvedDNN(e,label, systname);
                if(mBoson_ind == 0) {mBoson = mBoson_W;} else if(mBoson_ind == 1) {mBoson = mBoson_Z;}
            }
            //******************//

            string genmatch = "wrong_";
            if(genMatchResolved(e,systname,label)) genmatch = "right_"; // need to redo since due to  the jet assignement

            //******************//
            //// decide
            //********cut********//
            float mWidth = 15.;
            float mWidthL = 15.;
            float mWidthH = 20.;
            double chi2Cut=6.;
            float tmvacut = 0.75; // 80% signal
            float kerascut = 0.5;
            //******************//

            if( (bosonJets.size()>1) && (forwardJets.size() > 1)  && ( forwardJets[0]->Eta() * forwardJets[1]->Eta() <0 ) && (fabs(forwardJets[0]->DeltaEta(forwardJets[1])) > 3.) && ((forwardJets[0]->GetP4() + forwardJets[1]->GetP4()).M() > 500) ){
                Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
                Fill("VBShadAnalysis/Baseline/ResBosonMass_"+genmatch+label, systname, MV, e->weight() );

                if((chi2<chi2Cut or (doResTagKeras and evt_maxkeras > kerascut) or (doResTagTMVA and evt_maxDnn > tmvacut))){
                  Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+label, systname, MV, e->weight() );
                  Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+genmatch+label, systname, MV, e->weight() );
                }

                if(doResTagKeras){
                    Fill("VBShadAnalysis/Baseline/ResBosonKeras_"+label, systname, evt_maxkeras, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonKeras_"+genmatch+label, systname, evt_maxkeras, e->weight() );
                }else if(doResTagTMVA){
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA_"+label, systname, evt_maxDnn, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA_"+genmatch+label, systname, evt_maxDnn, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA2V_"+genmatch+label, systname, evt_v_score, e->weight() );
                }else{
                    Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+genmatch+label, systname, chi2, e->weight() );
                }
            }

            //bool massWindow = doSideBand ? ( (fabs(MV-mBoson) >  mWidth) && MV < 155) : (fabs(MV-mBoson) < mWidth);
            bool massWindowAsy = ( (MV >= mBoson_W - mWidthL) and (MV <= mBoson_Z + mWidthH)  );
            bool massWindow = doSideBand ? ( (!massWindowAsy && MV < 155 && MV > 50) /*or (selectedFatZbbIn.size()==0)*/ ) : (massWindowAsy);
            if( massWindow and bosonJets.size()>1 and (chi2<chi2Cut or (doResTagKeras and evt_maxkeras > kerascut) or (doResTagTMVA and evt_maxDnn > tmvacut) ) ) {
                category="_RBtag";
    
                TLorentzVector jetP4;
                jetP4.SetPtEtaPhiM(selectedFatZbb[0]->Pt(),selectedFatZbb[0]->Eta(),selectedFatZbb[0]->Phi(),selectedFatZbb[0]->rawMass(selectedFatZbb[0]->MASSTYPE,true));

                p4VV = jetP4 + bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
                p4V1 = jetP4;
                p4V2 = bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
                //p4VV = ( selectedFatZbb[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                evt_chi2_ = chi2;
                evt_MVV = p4VV.M();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_bosV1discr = bosonBBDiscr[0];
                evt_bosV1mass = bosonBBMass[0];
                evt_bosV1tdiscr = bosonBBTDiscr[0];
                evt_bosV1unc = 0;
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_bosV2discr = (doResTagKeras) ? evt_maxkeras: (doResTagTMVA) ? evt_maxDnn: -1;
                evt_bosV2tdiscr = -1;
                evt_bosV2bdiscr = -1;
                evt_bosV2discr2nd = evt_2ndmaxDnn;
                evt_bosV2unc = 0;

            }
        }
    }

    //////////
    // with MET

    if(doMETAnalysis) {
        p4VV*=0;
        evt_MVV=0;
        evt_PTV1=0;
        evt_PTV2=0;
        evt_DetaVV=-100;
        // reset the first boson (now it's MET)
        evt_bosV1mass=-1;
        evt_bosV1discr=-1;
        evt_bosV1tdiscr=-1;
        evt_bosV1unc = 0;
        // reset resolved
        evt_chi2_= -1;
        bosonJets.clear();
        forwardJets.clear();
        vetoJets.clear();


        //////////////////////////////////
        //50|---D---|---B---|---D---|155//
        //50|---C---|---A---|---C---|155//
        //50|--Cres-|--Ares-|--Cres-|155//
        //////////////////////////////////

        bool condition1_AvsC = doSideBand ? (selectedFatJetsIn.size()==0 and selectedFatZbbIn.size()==0) : 1;
        bool condition2_AvsB = doMETAntiAnalysis ? (selectedMirrorFatJets.size()==0 and selectedFatZbbWide.size()==0) : 1; //when doing Anti, selectedMirrorFatJets stores SR fatjets, ==0 to be inclusive with SR

        if((selectedFatJets.size()>0 or (!doResonant and selectedFatZbb.size()==1)) and selectedJets.size()>1 and condition1_AvsC and condition2_AvsB) {
            category="_BMET";

            //        Current: (ET1+ET2)^2 - (PT1+PT2)^2    (also as shown in the current ch-higgs code you sent to me)
            //        New:      m^2 + PT2                                 (In code this is very easy to obtained: directly take      (p1+p2).Mt()     )

            TLorentzVector jetP4;
            TLorentzVector metP4;
            //            cout << "is the selectedFatJets->SDMass() ok ?? "<< endl;
            if(selectedFatZbb.size()>0){
                jetP4.SetPtEtaPhiM(selectedFatZbb[0]->Pt(),selectedFatZbb[0]->Eta(),selectedFatZbb[0]->Phi(),selectedFatZbb[0]->rawMass(selectedFatZbb[0]->MASSTYPE,true));
                evt_bosV2discr = bosonBBDiscr[0];
                evt_bosV2tdiscr = bosonBBTDiscr[0];
                evt_bosV2mass = bosonBBMass[0];
            }else{
                jetP4.SetPtEtaPhiM(selectedFatJets[0]->Pt(),selectedFatJets[0]->Eta(),selectedFatJets[0]->Phi(),selectedFatJets[0]->rawMass(selectedFatJets[0]->MASSTYPE));
                evt_bosV2discr = bosonVDiscr[0];
                evt_bosV2tdiscr = bosonTDiscr[0];
                evt_bosV2bdiscr = bosonBDiscr[0];
                evt_bosV2mass = bosonMass[0];
            }
            if(usePuppi) {
                metP4.SetPtEtaPhiM(e->GetMet().GetPuppiMetP4().Pt(),0.,e->GetMet().GetPuppiMetP4().Phi(),91);
                //                evt_MVV = ChargedHiggs::mt(jetP4.Pt(), e->GetMet().GetPuppiMetP4().Pt(), jetP4.Phi(), e->GetMet().GetPuppiMetP4().Phi());
            } else {
                metP4.SetPtEtaPhiM(e->GetMet().GetP4().Pt(),0.,e->GetMet().GetP4().Phi(),91);
                //                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().Pt(), selectedFatJets[0]->Phi(), e->GetMet().Phi());
            }
        
            p4V1 = metP4;
            p4V2 = jetP4;
            p4VV = metP4 + jetP4;
            evt_MVV = ChargedHiggs::mtMassive(jetP4,metP4); //this is ET^2-PT^2
            evt_MTVV = p4VV.Mt();                           //this ism^2 + PT2  

            evt_bosV2unc = 0;
            evt_bosV2discr2nd = -1.;


            //            float Mjj=jettagForBoosted(e, label, systname, jetP4.Eta(),jetP4.Eta());
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }


            evt_genmatch = genMatchResonant(e, label, category);

        }

        // target the ZnnZqq + ZnnWqq resolved
        if(selectedFatJets.size()==0 and selectedFatJetsOut.size()==0 and selectedFatZbbWide.size()==0 and selectedMirrorFatJets.size()==0 and selectedJets.size()>3) {
            category="";

            double MV = 0.;
            double chi2 = 9999.;
            float mBoson = 0;
            int mBoson_ind = -1;

            float mBoson_W=80.;
            float mBoson_Z=91.;

            if (!doResTagTMVA && !doResTagKeras) {

                string genWmat = "wrong_";
                string genZmat = "wrong_";

                float MV_W, mW_chi2;
                std::tie(MV_W,mW_chi2) = resolvedtagger(e, mBoson_W, label, systname, 0.); if(genMatchResolved(e,systname,label)) genWmat = "right_";

                //if(bosonJets.size()>1){
                if( (bosonJets.size()>1) && (forwardJets.size() > 1)  && ( forwardJets[0]->Eta() * forwardJets[1]->Eta() <0 ) && (fabs(forwardJets[0]->DeltaEta(forwardJets[1])) > 3.) && ((forwardJets[0]->GetP4() + forwardJets[1]->GetP4()).M() > 500) ){
                    Fill2D("VBShadAnalysis/Baseline/ResWMassChi2_"+label, systname, MV_W, mW_chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResWMass_"+genWmat+label, systname, MV_W, e->weight() );
                }
                  
                float MV_Z, mZ_chi2;
                std::tie(MV_Z,mZ_chi2) = resolvedtagger(e, mBoson_Z, label, systname, 0.); if(genMatchResolved(e,systname,label)) genZmat = "right_";
                if(bosonJets.size()>1){
                    Fill2D("VBShadAnalysis/Baseline/ResZMassChi2_"+label, systname, MV_Z, mZ_chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResZMass_"+genZmat+label, systname, MV_Z, e->weight() );
                }

                if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2Diff_W_"+genWmat+label, systname, mW_chi2-mZ_chi2, e->weight() );
                if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2Diff_Z_"+genZmat+label, systname, mZ_chi2-mW_chi2, e->weight() );

                mBoson = mBoson_Z;
                MV = MV_Z;
                chi2 = mZ_chi2;

                if(mW_chi2 < mZ_chi2){
                    mBoson = mBoson_W;
                    std::tie(MV,chi2) = resolvedtagger(e, mBoson_W, label, systname, 0.);
                }
                
            }

            //********cut********//

            if(doResTagKeras or doResTagTMVA){
                std::tie(MV,mBoson_ind) = resolvedDNN(e,label, systname);
                if(mBoson_ind == 0) {mBoson = mBoson_W;} else if(mBoson_ind == 1) {mBoson = mBoson_Z;}
            }
            //******************//

            string genmatch = "wrong_";
            if(genMatchResolved(e,systname,label)) genmatch = "right_"; // need to redo since due to  the jet assignement

            //// decide
            //********cut********//
            float mWidth = 15.;
            float mWidthL = 15.;
            float mWidthH = 20.;
            double chi2Cut=6.;
            float tmvacut = 0.75; // 80% cut
            float kerascut = 0.5;
            //******************//

            if( (bosonJets.size()>1) && (forwardJets.size() > 1)  && ( forwardJets[0]->Eta() * forwardJets[1]->Eta() <0 ) && (fabs(forwardJets[0]->DeltaEta(forwardJets[1])) > 3.) && ((forwardJets[0]->GetP4() + forwardJets[1]->GetP4()).M() > 500) ){
                Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
                Fill("VBShadAnalysis/Baseline/ResBosonMass_"+genmatch+label, systname, MV, e->weight() );

                if((chi2<chi2Cut or (doResTagKeras and evt_maxkeras > kerascut) or (doResTagTMVA and evt_maxDnn > tmvacut))){
                    Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+label, systname, MV, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+genmatch+label, systname, MV, e->weight() );
                }

                if(doResTagKeras){
                    Fill("VBShadAnalysis/Baseline/ResBosonKeras_"+label, systname, evt_maxkeras, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonKeras_"+genmatch+label, systname, evt_maxkeras, e->weight() );
                }else if(doResTagTMVA){
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA_"+label, systname, evt_maxDnn, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA_"+genmatch+label, systname, evt_maxDnn, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonTMVA2V_"+genmatch+label, systname, evt_v_score, e->weight() );
                }else{
                    Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
                    Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+genmatch+label, systname, chi2, e->weight() );
                }

            }

            //bool massWindow = doSideBand ? ( (fabs(MV-mBoson) >  mWidth) && MV < 155) : (fabs(MV-mBoson) < mWidth);
            bool massWindowAsy = ( (MV >= mBoson_W - mWidthL) and (MV <= mBoson_Z + mWidthH)  );
            bool massWindow = doSideBand ? ( !massWindowAsy && MV < 155 && MV > 50) : (massWindowAsy);
            if(massWindow and bosonJets.size()>1 and (chi2<chi2Cut or (doResTagKeras and evt_maxkeras > kerascut) or (doResTagTMVA and evt_maxDnn > tmvacut) ) ) {
                category="_RMET";

                TLorentzVector metP4;
                if(usePuppi) {
                    metP4.SetPtEtaPhiM(e->GetMet().GetPuppiMetP4().Pt(),0.,e->GetMet().GetPuppiMetP4().Phi(),91);
                    //                    evt_MVV = (bosonJets[0]->GetP4()+metP4).Mt();
                    //                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt(), e->GetMet().GetPuppiMetP4().Pt(),
                    //                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().GetPuppiMetP4().Phi());

                } else {
                    metP4.SetPtEtaPhiM(e->GetMet().GetP4().Pt(),0.,e->GetMet().GetP4().Phi(),91);
                    //                    evt_MVV = (bosonJets[0]->GetP4()+metP4).Mt();
                    //                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()), e->GetMet().Pt(),
                    //                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().Phi());
                }

                p4V1 = metP4;;
                p4V2 = bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
                p4VV = metP4 + bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
                evt_MVV = ChargedHiggs::mtMassive(bosonJets[0]->GetP4() + bosonJets[1]->GetP4(), metP4);
                evt_MTVV = p4VV.Mt();
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_bosV2discr = (doResTagKeras) ? evt_maxkeras: (doResTagTMVA) ? evt_maxDnn: -1;
                evt_bosV2tdiscr = -1;
                evt_bosV2discr2nd = evt_2ndmaxDnn;
                evt_bosV2unc = 0;
                evt_chi2_ = chi2;

            }
        }
    }

    evt_PTVV = p4VV.Pt();
    evt_PTV1 = p4V1.Pt();
    evt_PTV2 = p4V2.Pt();
    evt_bosV1Eta = p4V1.Eta();
    evt_bosV2Eta = p4V2.Eta();


    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/BOSON/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
    Fill("VBShadAnalysis/BOSON/BosonDecayRatio_" +label, systname, dauRatioV1, e->weight() );
    Fill("VBShadAnalysis/BOSON/BosonDecayRatio_" +label, systname, dauRatioV2, e->weight() );
    Fill("VBShadAnalysis/BOSON/CosThetaStar_" +label, systname, cosThetaV1, e->weight() );
    Fill("VBShadAnalysis/BOSON/CosThetaStar_" +label, systname, cosThetaV2, e->weight() );

    ////// Fill VV-rest frame variables
    VVRestObj(e);
    ///////////////////////////////////

    //////
    //$$$ APPLY WEIGHTS
    //////

    //////
    if((label.find("ZJetsToNuNu_HT") !=string::npos or label.find("ZJetsToNuNuPt")) and genVp!=NULL) {

        if( not e->ExistSF("ZNNLO_rwg") ){
            LogN(__FUNCTION__,"WARNING","SF: ZNNLO_rwg does not exist",10);
            return EVENT_NOT_USED;
        }
        auto pt = (genVp->Pt()<160)?160: (genVp->Pt()>1200)? 1200:genVp->Pt(); 
            e->SetPtEtaSF("ZNNLO_rwg", pt ,0.); // it is only pt dependent
        e->ApplySF("ZNNLO_rwg");
    }

    if ((label.find("WJetsToLNu_HT") !=string::npos or label.find("WJetsToLNu_Pt") !=string::npos) and genVp!=NULL) {

       if( not e->ExistSF("WNNLO_rwg") ){
           LogN(__FUNCTION__,"WARNING","SF: WNNLO_rwg does not exist",10);
           return EVENT_NOT_USED;
       }
       auto pt = (genVp->Pt()<160)?160: (genVp->Pt()>1200)? 1200:genVp->Pt(); 
       e->SetPtEtaSF("WNNLO_rwg", pt ,0.); // it is only pt dependent
       e->ApplySF("WNNLO_rwg");

    }


   //////
   if(label.find("QCD_HT") !=string::npos) {

       string sfname="";
       if (year==12016)  sfname = "FatJetV_12016";
       if (year==2016)  sfname = "FatJetV_2016";
       if (year==2017)  sfname = "FatJetV_2017";
       if (year==2018)  sfname = "FatJetV_2018";

       if((doHADAnalysis or doHADAntiAnalysis) and (category.find("BB") !=string::npos)) {
           e->SetPtEtaSF(sfname, evt_PTV1, fabs(evt_bosV1Eta));
           e->ApplySF(sfname);
           e->SetPtEtaSF(sfname, evt_PTV2, fabs(evt_bosV2Eta));
           e->ApplySF(sfname);
       }

       /*
       if((category.find("BBtag") !=string::npos) and selectedFatJets.size()>0) {
           e->SetPtEtaSF(sfname, evt_PTV2, fabs(evt_bosV2Eta));
           e->ApplySF(sfname);
       }

       if((category.find("BMET") !=string::npos) and selectedFatJets.size()>0) {
           e->SetPtEtaSF(sfname, evt_PTV2, fabs(evt_bosV2Eta));
           e->ApplySF(sfname);
       }
       */

   }

   /*
    //unc on mvv, use 'Smear=@SmearSF("QCDNonclosure_CAT"!"QCD_MVV_CAT_Nonclosure")' to run, where CAT = BB or BBtag
    // THIS IS OLD not needed anymore
    if( not e->IsRealData() and (label.find("QCD_HT")!=string::npos)){
        string sfname="";
        if(doHADAnalysis or doHADAntiAnalysis)  sfname="QCD_MVV_BB_Nonclosure";
        else if(doBAnalysis or doBAntiAnalysis) sfname="QCD_MVV_BBtag_Nonclosure";

        if( not e->ExistSF(sfname) ){
            LogN(__FUNCTION__,"WARNING","QCD MVV SF does not exist, check category",10);
            return EVENT_NOT_USED;
        }

        e->SetPtEtaSF(sfname,evt_MVV,0);
        e->ApplySF(sfname);
    }
   */

    //////
    //$$$ CUT FLOW
    //////

    double MVV_cut=500;
    // CHECK THIS: adjust with trigger turn ones especially on BB and Btag
    if((category.find("RMET")   !=string::npos )) MVV_cut=250;

    if( evt_MVV < MVV_cut ) return EVENT_NOT_USED;

    if(checkSignalLabel(label) and evt_MVV_gen!=0) Fill("VBShadAnalysis/MVVres" +category+"_"+label, systname, (evt_MVV-evt_MVV_gen)/evt_MVV_gen, e->weight() );

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 6, e->weight() );  //6--InvMFatjet cut
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 6, 1 );

    Fill("VBShadAnalysis/Baseline/NJet_" +label, systname, forwardJets.size(), e->weight() );

    if( forwardJets.size() < 2 ) return EVENT_NOT_USED;
    if( forwardJets[0]->Eta() * forwardJets[1]->Eta() >=0 ) return EVENT_NOT_USED;

    /*
    if(fabs(forwardJets[0]->GetP4().Eta())<3 and fabs(forwardJets[0]->GetP4().Eta())>2.4) Fill("VBShadAnalysis/nemf1_Jet" +category+"_"+label, systname, forwardJets[0]->GetNEMF(), e->weight());
    if(fabs(forwardJets[1]->GetP4().Eta())<3 and fabs(forwardJets[0]->GetP4().Eta())>2.4) Fill("VBShadAnalysis/nemf2_Jet" +category+"_"+label, systname, forwardJets[1]->GetNEMF(), e->weight());
    if(fabs(forwardJets[0]->GetP4().Eta())<3 and fabs(forwardJets[0]->GetP4().Eta())>2.4) Fill("VBShadAnalysis/nhf1_Jet" +category+"_"+label, systname, forwardJets[0]->GetNHF(), e->weight());
    if(fabs(forwardJets[1]->GetP4().Eta())<3 and fabs(forwardJets[0]->GetP4().Eta())>2.4) Fill("VBShadAnalysis/nhf2_Jet" +category+"_"+label, systname, forwardJets[1]->GetNHF(), e->weight());
    */

    // those are mainly wrong combination
    if(fabs(forwardJets[1]->GetP4().Eta())<3 and fabs(forwardJets[1]->GetP4().Eta())>2.4 and (forwardJets[1]->GetNEMF()==0 or forwardJets[1]->GetNHF()==0)) return EVENT_NOT_USED;

    //    if(doTrigger) studyTriggers(e, category, label, systname);

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 7, e->weight() );  //7--NJet cut opposite side
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 7, 1 );

    evt_Detajj = fabs(forwardJets[0]->DeltaEta(forwardJets[1]));

    float dEtaCut = 2.5;
    if(doResonant) dEtaCut = 4.;

    if( evt_Detajj < dEtaCut ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 8, e->weight() );  //8--DeltaEta cut
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 8, 1 );

    p4jj = forwardJets[0]->GetP4() + forwardJets[1]->GetP4();

    evt_Mjj = p4jj.M();

    Fill("VBShadAnalysis/FWJETS/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    if(doStudySFfat and selectedFatJets.size()>1 and selectedFatZbb.size()==0 and selectedJets.size()>1 ) {
        int indexij = 10*getIndex(0) + getIndex(1);
        //        Fill("VBShadAnalysis/Baseline/SF_FatJet_" +label, systname, indexij, e->weight() );  // fill to derive SF
        Fill("VBShadAnalysis/Baseline/SF_FatJet_Mjj_" +label, systname, indexij, e->weight() );  // fill to derive SF
    }

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 9, e->weight() ); //9--InvMjet cut
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 9, 1 );

    //////
    //$$$ VARIOUS plots below for tree
    //////


    evt_j1unc = sqrt((forwardJets[0]->GetJESUnc())*(forwardJets[0]->GetJESUnc()) + Getjetres(forwardJets[0]) * Getjetres(forwardJets[0]));
    evt_j2unc = sqrt((forwardJets[1]->GetJESUnc())*(forwardJets[1]->GetJESUnc()) + Getjetres(forwardJets[1]) * Getjetres(forwardJets[1]));

    evt_j1QGL = forwardJets[0]->QGL();
    evt_j2QGL = forwardJets[1]->QGL();


    evt_Jet1Eta=forwardJets[0]->Eta();
    evt_Jet1Pt=forwardJets[0]->Pt();
    evt_Jet2Eta=forwardJets[1]->Eta();
    evt_Jet2Pt=forwardJets[1]->Pt();

    evt_Dphijj = ChargedHiggs::deltaPhi(forwardJets[0]->Phi(), forwardJets[1]->Phi());
    Fill("VBShadAnalysis/FWJETS/Dphijj" +category+"_"+label, systname, fabs(evt_Dphijj), e->weight() );


    ////////////////////
    //

    /////// 
    //only after passing above all VBF selections, start to calculate VV properties below
    ////////

    evt_DphiVV = ChargedHiggs::deltaPhi(p4V1.Phi(), p4V2.Phi());

    if(!doMETAnalysis){
        evt_DetaVV = fabs(evt_bosV1Eta - evt_bosV2Eta);
        evt_PetaVV = evt_bosV1Eta * evt_bosV2Eta;
        evt_EtaMinV = std::min(evt_bosV1Eta,evt_bosV2Eta);
        evt_EtaMaxV = std::max(evt_bosV1Eta,evt_bosV2Eta);
    }


    p4VVjj = p4VV + p4jj;

    evt_normPTVVjj = fabs(p4VVjj.Pt())/(evt_PTV2 + evt_PTV1 + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());
    //    evt_normPTVVjj = fabs(p4VVjj.Pt())/(p4VV.Pt() + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());

    evt_cenPTVVjj = fabs( p4VV.Pt() - 0.5 * p4jj.Pt() ) / fabs(forwardJets[0]->GetP4().Pt() - forwardJets[1]->GetP4().Pt());

    double averageJJeta= 0.5 * (forwardJets[0]->Eta()+forwardJets[1]->Eta());

    if(!doMETAnalysis) {
        bool centrality0 = (forwardJets[0]->Eta() <  evt_EtaMinV or forwardJets[0]->Eta() > evt_EtaMaxV);
        bool centrality1 = (forwardJets[1]->Eta() <  evt_EtaMinV or forwardJets[1]->Eta() > evt_EtaMaxV);

        // this is already applied when choosing resolved
        //if(!doResonant and !centrality0) return EVENT_NOT_USED;
        //if(!doResonant and !centrality1) return EVENT_NOT_USED;
    }

    //Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 10, e->weight() ); //10--centrality
    //Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 10, 1 );

    if(!doMETAnalysis) {

        evt_zepVV = fabs(p4VV.Eta() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/ZepBosVVar" +category+"_"+label, systname, evt_zepVV, e->weight() );

        evt_cenEta = std::min(
                           evt_EtaMinV - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - evt_EtaMaxV
                           ) ;
    }else{

        evt_cenEta = std::min( evt_bosV2Eta - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - evt_bosV2Eta);

    } 


    if(bosonJets.size()>1){

        evt_cenEtaRes = std::min(
                           std::min(bosonJets[0]->Eta(),bosonJets[1]->Eta()) - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - std::max(bosonJets[0]->Eta(),bosonJets[1]->Eta())
                           ) ;
    }
    


    //only cut for btag cate, others used in MVA
    if( !doResonant and (category.find("BBtag")   !=string::npos or category.find("RBtag")   !=string::npos) and evt_cenEta < 0. ) return EVENT_NOT_USED;


    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 10, e->weight() ); //10--centrality
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 10, 1 );

    if(doHADAnalysis or doHADAntiAnalysis) { if(evt_PTV1<400) return EVENT_NOT_USED; }

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 11, e->weight() ); //11--V pt
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 11, 1 );


    //Zep and DR variables
    evt_zepVB1 = fabs(evt_bosV1Eta - averageJJeta)/fabs(evt_Detajj);
    evt_zepVB2 = fabs(evt_bosV2Eta - averageJJeta)/fabs(evt_Detajj);
    if(doMETAnalysis) evt_zepVB  = evt_zepVB2;
    else              evt_zepVB  = std::max(evt_zepVB1,evt_zepVB2);

    //zep for resolved cate, the two Vjets
    if(bosonJets.size()>1){
        evt_zepV2j1 = fabs(bosonJets[0]->Eta() - averageJJeta)/fabs(evt_Detajj);
        evt_zepV2j2 = fabs(bosonJets[1]->Eta() - averageJJeta)/fabs(evt_Detajj);
    }


    if(!doMETAnalysis){
        evt_DRV1j = std::min(p4V1.DeltaR(forwardJets[0]->GetP4()),p4V1.DeltaR(forwardJets[1]->GetP4()));
    }
    evt_DRV2j = std::min(p4V2.DeltaR(forwardJets[0]->GetP4()),p4V2.DeltaR(forwardJets[1]->GetP4()));


        //        evt_zepV2 = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepVV = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
    ////////////////////////


    Fill("VBShadAnalysis/DRV1j" +category+"_"+label, systname, evt_DRV1j, e->weight() );
    Fill("VBShadAnalysis/DRV2j" +category+"_"+label, systname, evt_DRV2j, e->weight() );
    Fill("VBShadAnalysis/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );
    Fill("VBShadAnalysis/normPTVVjj" +category+"_"+label, systname, evt_normPTVVjj, e->weight() );
    Fill("VBShadAnalysis/Dphimin" +category+"_"+label, systname, minDPhi, e->weight() );
    Fill("VBShadAnalysis/Centrality" +category+"_"+label, systname, evt_cenEta, e->weight() );

    if((
        (category.find("BMET")   !=string::npos )
        or (category.find("RMET")   !=string::npos )
        ) and
       (!doResonant and evt_normPTVVjj > 0.25) ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/GENERAL/Cutflow_" +label, systname, 12, e->weight() ); //12--normPtVV
    Fill("VBShadAnalysis/GENERAL/CutflowNoW_" +label, systname, 12, 1 );

    std::vector<TLorentzVector> oP4;
    oP4.push_back(p4VV);
    oP4.push_back(forwardJets[0]->GetP4());
    oP4.push_back(forwardJets[1]->GetP4());

    evt_FW2 = ChargedHiggs::FW_momentum(oP4, 2);
    Fill("VBShadAnalysis/FW2" +category+"_"+label, systname, evt_FW2, e->weight() );


    float mtop2 = ( p4V2 +  forwardJets[0]->GetP4() ) .M();;
    evt_mtop = ( p4V2 +  forwardJets[1]->GetP4() ) .M();
    if(fabs(mtop2-175) < fabs(evt_mtop-175)) evt_mtop = mtop2;    



    //////
    //$$$ STUDY triggers
    //////

    if(doTrigger) studyTriggers(e, category, label, systname);

    //////
    //$$$ MONEY plots below
    //////

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
    Fill("VBShadAnalysis/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );
    Fill("VBShadAnalysis/PTVV" +category+"_"+label, systname, evt_PTVV, e->weight() );
    Fill("VBShadAnalysis/Detajj" +category+"_"+label, systname, evt_Detajj, e->weight() );
    
    if (label.find("aQGC") != string::npos){  
        if (not e->GetWeight()->HasAQGC()) {  // SOMETHING WENT WRONG
            Log(__FUNCTION__,"ERROR","AQGC sample has no AQGC weights. File: " + e->GetName());
        }
        if ( (systname=="" or systname=="NONE") and (e->GetWeight()->HasAQGC())) // AQGC
        { // only on the money plots, when no other syst, and for MC with aqgc weights
            // prepare weights
            for (const auto & name : MC::aqgc_names)
            {
                double w=e->weight_aqgc(name);
                Fill("VBShadAnalysis/MVV" +category+"_"+label, string("AQGC_")+name, evt_MVV, w );
            }
        }
    }

    if(checkSignalLabel(label)) {
        if(evt_genmatch) Fill("VBShadAnalysis/MVV" + category+"_match_"+label, systname, evt_MVV, e->weight() );
        if(!evt_genmatch) Fill("VBShadAnalysis/MVV" + category+"_unMatch_"+label, systname, evt_MVV, e->weight() );

        if((category.find("BBtag") !=string::npos) or (category.find("BB") !=string::npos)) {
            bool isCC = (fabs(evt_bosV1Eta)<1.4) and (fabs(evt_bosV2Eta)<1.4);
            if(isCC) Fill("VBShadAnalysis/MVV" + category+"_central_"+label, systname, evt_MVV, e->weight() );
            if(!isCC) Fill("VBShadAnalysis/MVV" + category+"_CE_EE_"+label, systname, evt_MVV, e->weight() );
        }
    }


    Fill("VBShadAnalysis/pT1_Jet" +category+"_"+label, systname, forwardJets[0]->GetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/pT2_Jet" +category+"_"+label, systname, forwardJets[1]->GetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/eta1_Jet" +category+"_"+label, systname, fabs(forwardJets[0]->GetP4().Eta()), e->weight() );
    Fill("VBShadAnalysis/eta2_Jet" +category+"_"+label, systname, fabs(forwardJets[1]->GetP4().Eta()), e->weight() );
    Fill("VBShadAnalysis/qgl1_Jet" +category+"_"+label, systname, evt_j1QGL, e->weight() );
    Fill("VBShadAnalysis/qgl2_Jet" +category+"_"+label, systname, evt_j2QGL, e->weight() );
    // 
    Fill("VBShadAnalysis/pTV1_Jet" +category+"_"+label, systname, evt_PTV1, e->weight() );
    Fill("VBShadAnalysis/pTV2_Jet" +category+"_"+label, systname, evt_PTV2, e->weight() );

    Fill("VBShadAnalysis/massV1" +category+"_"+label, systname, evt_bosV1mass, e->weight() );
    Fill("VBShadAnalysis/massV2" +category+"_"+label, systname, evt_bosV2mass, e->weight() );

    Fill("VBShadAnalysis/bosV2discr"  +category+"_"+label, systname, evt_bosV2discr, e->weight() );
    Fill("VBShadAnalysis/bosV2discr2nd"  +category+"_"+label, systname, evt_bosV2discr2nd, e->weight() );
    
    Fill("VBShadAnalysis/Met" +category+"_"+label, systname, evt_PTV1, e->weight() );
    Fill("VBShadAnalysis/MetPhi"+category+"_"+label, systname, e->GetMet().GetP4().Phi(), e->weight() );

    Fill("VBShadAnalysis/etaV1_Jet" +category+"_"+label, systname, fabs(evt_bosV1Eta), e->weight() );
    Fill("VBShadAnalysis/etaV2_Jet" +category+"_"+label, systname, fabs(evt_bosV2Eta), e->weight() );

    Fill("VBShadAnalysis/phiV1_Jet" +category+"_"+label, systname, p4V1.Phi() , e->weight() );
    Fill("VBShadAnalysis/phiV2_Jet" +category+"_"+label, systname, p4V2.Phi() , e->weight() );

    /*
    if( doMETAnalysis and (category.find("BMET")   !=string::npos) ) {
        if(selectedFatJets.size()>0) Fill("VBShadAnalysis/pT_FatJet" +category+"_"+label, systname, selectedFatJets[0]->GetP4().Pt(), e->weight() );
        if(selectedFatZbb.size()>0) Fill("VBShadAnalysis/pT_FatJet" +category+"_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );
        Fill("VBShadAnalysis/WvsQCD_FatJet" +category+"_"+label, systname, evt_bosV2discr, e->weight() );
    }
    */

    if(evt_Detajj < 5.) Fill("VBShadAnalysis/MVV" +category+"_low_"+label, systname, evt_MVV, e->weight() );
    if(evt_Detajj >= 5.) Fill("VBShadAnalysis/MVV" +category+"_high_"+label, systname, evt_MVV, e->weight() );

    if(doResonant) {
        if( !doBAnalysis and !doMETAnalysis and (category.find("BB")   !=string::npos) ) {

            if( fabs(evt_MVV-1550) < 75 ) {
                Fill("VBShadAnalysis/IN1500/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
                Fill("VBShadAnalysis/IN1500/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );
            }
            if( fabs(evt_MVV-1550) > 75 ) {
                Fill("VBShadAnalysis/OUT1500/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
                Fill("VBShadAnalysis/OUT1500/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );
            }
        }
    }

    if(doTMVA) {
        ReadTmva(e);

        int xset = (e->eventNum()) % 3;

        if(doHADAnalysis or doHADAntiAnalysis) BDTnoBnoMET = bdt[xset];
        if(doMETAnalysis and (category.find("BMET")   !=string::npos)) BDTwithMET = bdt[xset+3];
        if(doMETAnalysis and (category.find("RMET")   !=string::npos)) BDTwithMET = bdt[xset+10];
        if(doBAnalysis and (category.find("RBtag")   !=string::npos)) BDTbtag = bdt[9]; // from Will
        if(doBAnalysis and (category.find("BBtag")   !=string::npos)) BDTbtag = bdt[xset+6]; // from Miao

        if(!doBAnalysis and !doMETAnalysis) Fill ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+label, systname, BDTnoBnoMET, e->weight() );
        if(doBAnalysis or doBAntiAnalysis) {
            Fill ("VBShadAnalysis/BDTbtag"+category+"_"+label, systname, BDTbtag, e->weight() );
            if((category.find("RBtag")   !=string::npos)) {
                Fill ("VBShadAnalysis/DNNMultiRBtagEWK"+category+"_"+label, systname, bdt_multi[15], e->weight() ); // from Ava
                Fill ("VBShadAnalysis/DNNMultiRBtagQCD"+category+"_"+label, systname, bdt_multi[16], e->weight() ); // from Ava
                Fill ("VBShadAnalysis/DNNMultiRBtagBKG"+category+"_"+label, systname, bdt_multi[17], e->weight() );// from Ava
            }
        }

        if(doMETAnalysis or doMETAntiAnalysis) {
            Fill ("VBShadAnalysis/BDTwithMET"+category+"_"+label, systname, BDTwithMET, e->weight() );

            Fill ("VBShadAnalysis/BDTMultiwithMETzz"+category+"_"+label, systname, bdt_multi[0], e->weight() );
            Fill ("VBShadAnalysis/BDTMultiwithMETwz"+category+"_"+label, systname, bdt_multi[1], e->weight() );
            Fill ("VBShadAnalysis/BDTMultiwithMETbkg"+category+"_"+label, systname, bdt_multi[2], e->weight() );
/*
            Fill ("VBShadAnalysis/BDTMultiEWKchi2"+category+"_"+label, systname, bdt_multi[3], e->weight() );
            Fill ("VBShadAnalysis/BDTMultiQCDchi2"+category+"_"+label, systname, bdt_multi[4], e->weight() );
            Fill ("VBShadAnalysis/BDTMultiBKGchi2"+category+"_"+label, systname, bdt_multi[5], e->weight() );
*/
            Fill ("VBShadAnalysis/DNNMultiEWKdnn"+category+"_"+label, systname, bdt_multi[12], e->weight() );
            Fill ("VBShadAnalysis/DNNMultiQCDdnn"+category+"_"+label, systname, bdt_multi[13], e->weight() );
            Fill ("VBShadAnalysis/DNNMultiBKGdnn"+category+"_"+label, systname, bdt_multi[14], e->weight() );

        }
    }

    if(doWriteTree) {

        if (systname.find("NONE")    !=string::npos) {
            setTree(e,label,category);
            FillTree("tree_vbs");
        }

        if (systname.find("JES_TotalUp")    !=string::npos) {
            std::cout << " writing tree systname = " << systname << std::endl;
            setTree(e,label,category);
            FillTree("tree_vbs_JES_TotalUp");
        }

        if (systname.find("JES_TotalDown")    !=string::npos) {
            std::cout << " writing tree systname = " << systname << std::endl;
            setTree(e,label,category);
            FillTree("tree_vbs_JES_TotalDown");
        }

    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}

void VBShadAnalysis::EndEvent(){
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
