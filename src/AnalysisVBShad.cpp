#include "interface/AnalysisVBShad.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TPython.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"

#define VERBOSE 0

//#warning Hmumu ANALYSIS NON ISO

#define DEEP_B_LOOSE ((year==2016)?0.2217:(year==2017)?0.1522:0.1241)
#define DEEP_B_MEDIUM ((year==2016)?0.6321:(year==2017)?0.4941:0.4148)
#define DEEP_B_TIGHT ((year==2016)?0.8953:(year==2017)?.8001:.7527)

void VBShadAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10); 
    l->SetIsoRelCut(0.25);
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
    j->SetDeepBCut(DEEP_B_MEDIUM);
    //    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535
    //    j->SetBCut(0.5426); //L=0.5426 , M=  0.8484, T0.9535
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(20);
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100);
}

void VBShadAnalysis::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(20);
    t->SetEtaCut(2.3);
    t->SetMuRej(false);
    t->SetEleRej(false);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // NOW byVLooseIsolationMVArun2v1DBoldDMwLT, BEFORE LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(-1);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(1);  // 0=TauDecayModeFindingNewDMs 1=TauDecayModeFinding
}


void VBShadAnalysis::SetFatJetCuts(FatJet *f){
    f->SetEtaCut(2.5);
    f->SetPtCut(250);
    f->SetSDMassCut(30);
}

void VBShadAnalysis::BookHisto(string l, string category)
{

    Book ("VBShadAnalysis/DRV1j"+category+"_"+l, " ; #Delta #R (V,jet) ; Events", 125,0,6.28);
    Book ("VBShadAnalysis/ZepBosBVar"+category+"_"+l, " ; |#eta_{V} - 0.5*(#eta_{j1} + #eta_{j2})| / #Delta #eta(jj) ; Events", 250,0,2);
    Book ("VBShadAnalysis/BOSON/ZepBosVVar"+category+"_"+l, " ; |#eta_{VV} - 0.5*(#eta_{j1} + #eta_{j2})| / #Delta #eta(jj) ; Events", 250,0,2);
    Book ("VBShadAnalysis/BOSON/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 100,0,2500);
    Book ("VBShadAnalysis/FW2"+category+"_"+l, " ; FW2 ; Events", 100,0,1.);

    AddFinalHisto("VBShadAnalysis/MVV"+category+"_"+l);
    Book ("VBShadAnalysis/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 100,0,2500);
    Book ("VBShadAnalysis/MVV"+category+"_low_"+l, "MVV (low Deta JJ) ; MVV_{reco}; Events", 100, 0, 2500);
    Book ("VBShadAnalysis/MVV"+category+"_high_"+l, "MVV (high Deta JJ) ; MVV_{reco}; Events", 100, 0, 2500);

    if(doTMVA and !doBAnalysis and !doMETAnalysis) Book ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+l, "BDT noBnoMET ; BDT noBnoMET; Events", 200,-1.,1.);
    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) Book ("VBShadAnalysis/BDTwithMET"+category+"_"+l, "BDT withMET ; BDT withMET; Events", 200,-1.,1.);
    if(doTMVA and (doBAnalysis or doBAntiAnalysis)) Book ("VBShadAnalysis/BDTbtag"+category+"_"+l, "BDT with Btag ; BDT with Btag; Events", 200,-1.,1.);

    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) Book ("VBShadAnalysis/BDTMultiwithMETzz"+category+"_"+l, "BDT Multi withMET (response for ZZ); BDT Multi withMET [GeV]; Events", 200,0.,1.);
    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) Book ("VBShadAnalysis/BDTMultiwithMETwz"+category+"_"+l, "BDT Multi withMET (response for WZ); BDT Multi withMET [GeV]; Events", 200,0.,1.);
    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) Book ("VBShadAnalysis/BDTMultiwithMETbkg"+category+"_"+l, "BDT Multi withMET (response for BKG); BDT Multi withMET [GeV]; Events", 200,0.,1.);

    if(l.find("ZnnZhadJJ_EWK") !=string::npos  ||
       l.find("ZbbZhadJJ_EWK")!=string::npos  ||
       l.find("WPhadWPhadJJ_EWK") !=string::npos ||
       l.find("ZNuNuWPMJJjj_EWK") !=string::npos ||
       l.find("ZNuNuWPMJJjj_QCD") !=string::npos ||
       l.find("ZbbWpmJJ_EWK") !=string::npos ||
       l.find("ZbbWpmJJ_QCD") !=string::npos ||
       l.find("WWjj_SS_ll") !=string::npos ||
       l.find("WWjj_SS_lt") !=string::npos ||
       l.find("WWjj_SS_tt") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ) {
        Book ("VBShadAnalysis/MVVres"+category+"_"+l, "MVVres ; ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100, -5., 5.);
    }

    AddFinalHisto("VBShadAnalysis/FWJETS/Mjj"+category+"_"+l);
    Book ("VBShadAnalysis/FWJETS/Mjj"+category+"_"+l, "Mjj ; M(j,j) [GeV]; Events", 35,0,3500.);
    Book ("VBShadAnalysis/FWJETS/Dphijj"+category+"_"+l, "Dphi jj ; #Delta#Phi(j,j) ; Events", 100,0,6.28);

    // more plots for data/MC
    Book ("VBShadAnalysis/Detajj"+category+"_"+l, "Deta jj ; #Delta#eta(j,j) ; Events", 100,0,10.);
    Book ("VBShadAnalysis/pT_FatJet"+category+"_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
    Book ("VBShadAnalysis/WvsQCD_FatJet"+category+"_"+l, "WvsQCD_FatJet; WvsQCD; Events", 50,0,1.0);
    Book ("VBShadAnalysis/SDMass_FatJet"+category+"_"+l, "SDMass_FatJet; SDMass [GeV]; Events", 100,0,200.);
    Book ("VBShadAnalysis/normPTVVjj"+category+"_"+l, "normPTVVjj; |#vec{p_{T}^{tot}}| / #Sigma p_{T} ; Events", 100,0,1.);
    Book ("VBShadAnalysis/Dphimin"+category+"_"+l, "Dphimin", 100,0,6.28);
    Book ("VBShadAnalysis/Met"+category+"_"+l, "Met; Met [GeV]; Events", 100,0,6.28);
    Book ("VBShadAnalysis/PTVV"+category+"_"+l, "PTVV; PTVV [GeV]; Events", 100,0,1000);

    // fwd jets
    Book ("VBShadAnalysis/pT1_Jet"+category+"_"+l, "pT_Jet; p_{T} [GeV] (leading); Events", 80,0,1600);
    Book ("VBShadAnalysis/pT2_Jet"+category+"_"+l, "pT_Jet; p_{T} [GeV] (subleading) ; Events", 80,0,1600);

    Book ("VBShadAnalysis/eta1_Jet"+category+"_"+l, "eta_Jet; #eta (leading); Events", 100,0.,5.);
    Book ("VBShadAnalysis/eta2_Jet"+category+"_"+l, "eta_Jet; #eta (subleading) ; Events", 100,0.,5.);

    // boosted jets
    Book ("VBShadAnalysis/pTV1_Jet"+category+"_"+l, "pT_V1_Jet; p_{T} [GeV] (leading); Events", 80,0,1600);
    Book ("VBShadAnalysis/pTV2_Jet"+category+"_"+l, "pT_V2_Jet; p_{T} [GeV] (subleading) ; Events", 80,0,1600);

    Book ("VBShadAnalysis/etaV1_Jet"+category+"_"+l, "eta_V1_Jet; #eta (leading); Events", 100,0.,5.);
    Book ("VBShadAnalysis/etaV2_Jet"+category+"_"+l, "eta_V2_Jet; #eta (subleading) ; Events", 100,0.,5.);

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

void VBShadAnalysis::ReadTmva(){

    bdt.clear();
    bdt_multi.clear();

    SetVariable("varMjj",evt_Mjj); //0
    SetVariable("varDetajj",evt_Detajj); //1
    SetVariable("varJet2Pt",evt_Jet2Pt); //2
    SetVariable("varMVV",evt_MVV); //3
    SetVariable("varnormPTVVjj",evt_normPTVVjj); //4

    SetVariable("varPTVV",evt_PTVV); //5
    SetVariable("varPTV2",evt_PTV2); //6
    SetVariable("varPTV1",evt_PTV1); //7

    SetVariable("varCen",evt_cenEta); //8
    SetVariable("varzepVB",evt_zepVB); //9
    SetVariable("bosV2chi2",evt_chi2_); //10
    SetVariable("bosV2mass",evt_bosV2mass); //11
    SetVariable("varFW2j",evt_FW2); //12
    SetVariable("varmtop",evt_mtop); //13

    //    vector<float> bdt;
    for(unsigned i =0 ;i< readers_.size() ; ++i) {
        if(i==0 || i==1 || i==2) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSHad") );
        if(i==3 || i==4 || i==5) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSMet") );
        if(i==6 ) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSRBtag") );
        if(i==7 ) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSRMET") );
        if(i==8 ) bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSBHad") );
    }

    int NClass=3;
    for(unsigned i =0 ;i< readers_multi_.size() ; ++i) {
        for(unsigned j =0 ;j< NClass ; ++j) {
            bdt_multi.push_back( (readers_multi_[i]->EvaluateMulticlass("BDTG"))[j]);
        }
    }

}

void VBShadAnalysis::InitTmva() {

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader: " << weights.size() <<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    for( size_t i=0;i<weights_multi.size() ;++i)
        readers_multi_ . push_back( new TMVA::Reader() );

    cout << "---------------------------------------------" << endl;
    cout << " GOING TO BDTG - HAD " << endl;

    for (int i=0; i<3; i++) {
        AddVariable("varMjj",'F',readers_[i]); //0
        AddVariable("varDetajj",'F',readers_[i]); //1
        AddVariable("varJet2Pt",'F',readers_[i]); //2
        AddVariable("varMVV",'F',readers_[i]); //3
        AddVariable("varPTV2",'F',readers_[i]); //4
        AddVariable("varCen",'F',readers_[i]); //6
        AddVariable("varnormPTVVjj",'F',readers_[i]); //7
    }

    cout << "---------------------------------------------" << endl;
    cout << " GOING TO BDTG - MET " << endl;

    for (int i=3; i<4; i++) {
        AddVariable("varMjj",'F',readers_[i]); //0
        AddVariable("varDetajj",'F',readers_[i]); //1
        AddVariable("varJet2Pt",'F',readers_[i]); //2
        AddVariable("varPTV1",'F',readers_[i]); //4
        //        AddVariable("varMVV",'F',i); //3
        //        AddVariable("varPTVV",'F',i); //4
        AddVariable("varzepVB",'F',readers_[i]); //5
        AddVariable("varnormPTVVjj",'F',readers_[i]); //6
    }

    // old training with the top included
    for (int i=4; i<6; i++) {
        AddVariable("varMjj",'F',readers_[i]); //0
        AddVariable("varDetajj",'F',readers_[i]); //1
        AddVariable("varJet2Pt",'F',readers_[i]); //2
        AddVariable("varMVV",'F',readers_[i]); //3
        AddVariable("varPTVV",'F',readers_[i]); //4
        AddVariable("varzepVB",'F',readers_[i]); //5
        AddVariable("varnormPTVVjj",'F',readers_[i]); //6
    }

    // RBTAG
    for (int i=6; i<7; i++) {
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
    for (int i=7; i<8; i++) {
        AddVariable("varMjj",'F',readers_[i]); //0
        AddVariable("varDetajj",'F',readers_[i]); //1
        AddVariable("varJet2Pt",'F',readers_[i]); //2
        AddVariable("varMVV",'F',readers_[i]); //3
        AddVariable("varPTV2",'F',readers_[i]); //4
        AddVariable("varzepVB",'F',readers_[i]); //5
        AddVariable("varnormPTVVjj",'F',readers_[i]); //6
        AddVariable("bosV2chi2",'F',readers_[i]); //7
        AddVariable("bosV2mass",'F',readers_[i]); //8
        AddVariable("varFW2j",'F',readers_[i]); //9
    }

    // BBtag
    for (int i=8; i<9; i++) {
        AddVariable("varMjj",'F',readers_[i]); //0
        AddVariable("varJet2Pt",'F',readers_[i]); //1
        AddVariable("varPTV2",'F',readers_[i]); //2
        AddVariable("varCen",'F',readers_[i]); //3
        AddVariable("varnormPTVVjj",'F',readers_[i]); //4
        AddVariable("bosV2mass",'F',readers_[i]); //5
        AddVariable("varmtop",'F',readers_[i]); //6
    }


    //multiclass RMET
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

    cout << " GOING loop over weights" << weights.size() << endl;

    // load weights
    for( size_t i=0;i<weights.size() ;++i) {
        cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
        if(i==0 or i==1 or i==2) readers_[i]->BookMVA("BDT_VBSHad",weights[i].c_str());
        if(i==3 or i==4 or i==5) readers_[i]->BookMVA("BDT_VBSMet",weights[i].c_str());
        if(i==6 ) readers_[i]->BookMVA("BDT_VBSRBtag",weights[i].c_str());
        if(i==7 ) readers_[i]->BookMVA("BDT_VBSRMET",weights[i].c_str());
        if(i==8 ) readers_[i]->BookMVA("BDT_VBSBHad",weights[i].c_str());
    }

    for( size_t i=0;i<weights_multi.size() ;++i) {
        if(i==0) readers_multi_[i]->BookMVA("BDTG",weights_multi[i].c_str());
    }

    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;

}


void VBShadAnalysis::writeTree(string name){

        // fill variables for miniTREE
    InitTree(name);

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
    Branch(name,"varJet2Eta",'F');
    Branch(name,"varJet2Pt",'F');

    Branch(name,"j1Unc",'F');
    Branch(name,"j2Unc",'F');

    // VV
    Branch(name,"genMVV",'F');
    Branch(name,"varMVV",'F');
    Branch(name,"varPTVV",'F');
    Branch(name,"varPTV1",'F');
    Branch(name,"varPTV2",'F');
    Branch(name,"varDetaVV",'F');
    Branch(name,"varPetaVV",'F');
    Branch(name,"varEtaMinV",'F');
    Branch(name,"varEtaMaxV",'F');

    // MIX
    Branch(name,"varCen",'F');
    Branch(name,"varzepVB",'F');
    Branch(name,"varzepVV",'F');
    Branch(name,"varDRVj",'F');
    Branch(name,"varnormPTVVjj",'F');
    Branch(name,"varcenPTVVjj",'F');
    Branch(name,"varFW2j",'F');

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
    Branch(name,"bosV1discr",'F');
    Branch(name,"bosV1tdiscr",'F');
    Branch(name,"bosV2mass",'F');
    Branch(name,"bosV2discr",'F');
    Branch(name,"bosV2tdiscr",'F');
    Branch(name,"bosV2chi2",'F');
    Branch(name,"bosV2dR",'F');

    Branch(name,"bosGen",'I');
    Branch(name,"bosV1Unc",'F');
    Branch(name,"bosV2Unc",'F');

    //MVA
    Branch(name,"BDTnoBnoMET",'F');
    Branch(name,"BDTwithMET",'F');
    Branch(name,"BDTbtag",'F');

    Branch(name,"BDTMultiwithMETzz",'F');
    Branch(name,"BDTMultiwithMETwz",'F');
    Branch(name,"BDTMultiwithMETbkg",'F');

}


void VBShadAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

    Log(__FUNCTION__,"INFO",Form("doMETAnalysis=%d",doMETAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAnalysis=%d",doBAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAnalysis=%d",doHADAnalysis));

    Log(__FUNCTION__,"INFO",Form("doMETAntiAnalysis=%d",doMETAntiAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAntiAnalysis=%d",doBAntiAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAntiAnalysis=%d",doHADAntiAnalysis));


    if (not jet_resolution)
    { // init jet resolution
        Log(__FUNCTION__,"INFO","Init JER");
        jet_resolution.reset( new JME::JetResolutionObject("aux/jer/Summer16_25nsV1/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt") ) ;
    }


    if(doTMVA) InitTmva();

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
        //cutflow
        Book ("VBShadAnalysis/Cutflow_"+l, "cutflow; bit; Events", 12,0,12);

        //Trigger
        Book("VBShadAnalysis/Baseline/mVV_MET_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/mVV_RB_PFHT900_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_AK8PFJet450_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_AK8PFHT700_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_AK8PFJet360_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_AK8DiPFJet300_200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_PFHT650_Wide_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RB_had_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/mVV_BB_PFHT900_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_AK8PFJet450_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_AK8PFHT700_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_AK8PFJet360_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_AK8DiPFJet300_200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_PFHT650_Wide_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BB_had_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/mVV_RBtag_C100_DoubleBTagCSV_p014_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_C100_DoubleBTagCSV_p026_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_QuadPFJet_BTagCSV_Mqq200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_had_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_RBtag_btag_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/mVV_BBtag_C100_DoubleBTagCSV_p014_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_C100_DoubleBTagCSV_p026_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_QuadPFJet_BTagCSV_Mqq200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_had_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_BBtag_btag_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);


        Book("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_NoiseCleaned_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_PFMET120_PFHT60_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_PFHT60_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/mVV_met_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        //FatJet
        Book ("VBShadAnalysis/Baseline/NFatJet_"+l, "NFatJet; NFatJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatZbbJet_"+l, "pT_FatZbbJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_BB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RR_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_"+l, "SDMass_FatJet; SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/Tau21_FatJet_"+l, "Tau21_FatJet; tau21; Events", 50,0,1.0);

        Book ("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJet_"+l, "ZHbbvsQCD_FatJet; ZHbbvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJetFake_"+l, "ZHbbvsQCD_FatJetFake; ZHbbvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsQCD_FatJet_"+l, "WvsQCD_FatJet; WvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsQCD_FatJetFake_"+l, "WvsQCD_FatJetFake; WvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/TvsQCD_FatJet_"+l, "TvsQCD_FatJet; TvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/TvsQCD_FatJetFake_"+l, "TvsQCD_FatJetFake; TvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/WvsT_FatJet_"+l, "WvsT_FatJet; WvsT; Events", 50,0,2.0);
        Book ("VBShadAnalysis/Baseline/ZHccvsQCD_FatJet_"+l, "ZHccvsQCD_FatJet; ZHccvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_"+l, "ZHccvsQCD_FatJetFake; ZHccvsQCD; Events", 50,0,1.0);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJetFake_"+l, "SDMass_FatJetFake; SDMass [GeV]; Events", 100,0,200.);

        //Jet
        Book ("VBShadAnalysis/Baseline/NJet_"+l, "NJet; FatJet; Events", 10,0,10);
        Book ("VBShadAnalysis/Baseline/pT_Jet_"+l, "pT_Jet; pT [GeV]; Events", 80,0,1600.);

        Book ("VBShadAnalysis/Baseline/NBJet_"+l, "NBJet; NBJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/pT_BJet_"+l, "pT_BJet; pT [GeV]; Events", 120,0,2400.);

        //MET
        Book ("VBShadAnalysis/Baseline/PuppiMet_"+l, "MetPuppi; Met [GeV]; Events", 100,0,1000);
        Book ("VBShadAnalysis/Baseline/Met_"+l, "Met; Met [GeV]; Events", 100,0,1000);
        Book ("VBShadAnalysis/Baseline/DphiMETFat_"+l, "Dphi(Met,AK8); #Delta#Phi(Met,AK8) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/Baseline/Dphimin_"+l, "Dphi(Met,AK4); #Delta#Phi(Met,AK4) ; Events", 100,0,6.28);

        // OTHERS
        Book ("VBShadAnalysis/BOSON/BosonDecayRatio_"+l, " ; J2/J1; Events", 200,0,1);
        Book ("VBShadAnalysis/BOSON/CosThetaStar_"+l, " ; CosThetaStar; Events", 200,-1,1);

        Book ("VBShadAnalysis/BOSON/Mtt_"+l, "Mtt (unclassified); Mtt [GeV]; Events", 100,0,2500);

        // resolved
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

        Book ("VBShadAnalysis/Baseline/DR_genVj1_"+l, "DeltaR(gen,j1); DeltaR(gen,j1); Events", 100, 0, 4.);
        Book ("VBShadAnalysis/Baseline/DR_genVj2_"+l, "DeltaR(gen,j2); DeltaR(gen,j2); Events", 100, 0, 4.);
        Book ("VBShadAnalysis/Baseline/Eta_genLHEW_"+l, "Eta_V; eta; Events", 120, -3., 3.);
        Book ("VBShadAnalysis/Baseline/Eta_genLHEZ_"+l, "Eta_V; eta; Events", 120, -3., 3.);
        Book ("VBShadAnalysis/Baseline/JetUnc_right_"+l, "unc; jesUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetUnc_wrong_"+l, "unc; jesUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetRes_right_"+l, "res; jerUnc; Events", 500, 0., 0.25);
        Book ("VBShadAnalysis/Baseline/JetRes_wrong_"+l, "res; jerUnc; Events", 500, 0., 0.25);

        Book ("VBShadAnalysis/Baseline/ResBosonGenMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100, 0, 200.);

        // RESONANT CASE
        Book ("VBShadAnalysis/OUT1500/MVV_"+l, "MVV-OUT (unclassified); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/IN1500/MVV_"+l, "MVV-IN (unclassified); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/OUT1500/MVV_BB_"+l, "MVV-OUT (BB); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/IN1500/MVV_BB_"+l, "MVV-IN (BB); MVV [GeV]; Events", 100,0,2500);

        Book ("VBShadAnalysis/OUT1500/Mjj_"+l, "Mjj-OUT (unclassified); Mjj [GeV]; Events", 100,0,3500);
        Book ("VBShadAnalysis/IN1500/Mjj_"+l, "Mjj-IN (unclassified); Mjj [GeV]; Events", 100,0,3500);
        Book ("VBShadAnalysis/OUT1500/Mjj_BB_"+l, "Mjj-OUT (BB); Mjj [GeV]; Events", 35,0,3500);
        Book ("VBShadAnalysis/IN1500/Mjj_BB_"+l, "Mjj-IN (BB); Mjj [GeV]; Events", 35,0,3500);

        BookHisto(l, "");
        BookHisto(l, "_BB");
        BookHisto(l, "_RB");
        BookHisto(l, "_BMET");
        BookHisto(l, "_RMET");
        BookHisto(l, "_BBtag");
        BookHisto(l, "_RBtag");

    } //end label loop

    if(doWriteTree) { writeTree("tree_vbs"); writeTree("tree_vbs_JESUp"); writeTree("tree_vbs_JESDown"); }

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


std::pair<float, float> VBShadAnalysis::resolvedtagger(Event*e, float MV, string label, string systname, float etaV1) {

    bosonJets.clear();
    forwardJets.clear();
    vetoJets.clear();

    //    float const norm = 1000*1000; // 0.5TeV^2
    float const norm = 500*500; // 0.5TeV^2
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

    for(unsigned i=0; i<selectedJets.size(); ++i) {
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

            //V_term = (MRij - MV) * (MRij - MV)  / MVres ;
            //optimized chi2
            V_term = (MRij - MV) * (MRij - MV)  / MVres * (1 + 100*(PTij>0?(DRij/PTij):0))  + (Mij - MV) * (Mij - MV) / MVres;


            for(unsigned k=0; k<selectedJets.size(); ++k) {
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

    for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
        if(iter==index_i) bosonJets.push_back(selectedJets[index_i]);
        if(iter==index_j) bosonJets.push_back(selectedJets[index_j]);
        if(iter==index_k and selectedJets[index_k]->Pt()>50) forwardJets.push_back(selectedJets[index_k]);
        if(iter==index_l and selectedJets[index_l]->Pt()>50) forwardJets.push_back(selectedJets[index_l]);
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

    GenParticle * genT = NULL;
    GenParticle * genTbar = NULL;
    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        if(genpar->GetPdgId() == 6) if(genT==NULL) { genT = genpar; }
        if(genpar->GetPdgId() == -6) if(genTbar==NULL) { genTbar = genpar; }
    }

    float Mtt= (genT->GetP4()+genTbar->GetP4()).M();

    return Mtt;

}


bool VBShadAnalysis::genMatchResolved(Event*e, string systname, string label){
    if(bosonJets.size()<2) return false;

    bool match_1 = false;
    bool match_2 = false;

    evt_genmatch = 0;

    float bosjer1=Getjetres(bosonJets[0]);
    float bosjer2=Getjetres(bosonJets[1]);


    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        //cout <<  "debug LHE: " << genpar->IsLHE() << "pa id: " <<  genpar->GetParentPdgId() << endl;

        if( ! genpar->IsLHE()) continue;

        Fill("VBShadAnalysis/Baseline/DR_genVj1_"+label, systname, (bosonJets[0]->GetP4()).DeltaR(genpar->GetP4()), e->weight() );
        Fill("VBShadAnalysis/Baseline/DR_genVj2_"+label, systname, (bosonJets[1]->GetP4()).DeltaR(genpar->GetP4()), e->weight() );

        //if( fabs(genpar->GetParentPdgId())!= 23 && fabs(genpar->GetParentPdgId())!= 24 ) continue;

        if(fabs(genpar->GetPdgId()) == 23) Fill("VBShadAnalysis/Baseline/Eta_genLHEZ_"+label, systname, genpar->Eta(), e->weight() );
        if(fabs(genpar->GetPdgId()) == 24) Fill("VBShadAnalysis/Baseline/Eta_genLHEW_"+label, systname, genpar->Eta(), e->weight() );

        if( (bosonJets[0]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_1 = true;
        if( (bosonJets[1]->GetP4()).DeltaR(genpar->GetP4()) < 0.2 ) match_2 = true;

    }

    evt_genmatch = match_1 && match_2;

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

                if( ! genpar->IsLHE()) continue;
                if( (selectedJets[i]->GetP4()).DeltaR(genpar->GetP4()) > 0.2 ) continue;

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




void VBShadAnalysis::genStudies(Event*e, string label )
{

    int pdgID1=24;
    int pdgID2=24;

    if((label.find("ZnnZhadJJ_EWK") !=string::npos )
       || (label.find("ZbbZhadJJ_EWK")!=string::npos )) {
        pdgID1=23;
        pdgID2=23;
    } else if (label.find("WPhadWPhadJJ_EWK") !=string::npos ||
               label.find("WWjj_SS_ll") !=string::npos ||
               label.find("WWjj_SS_lt") !=string::npos ||
               label.find("WWjj_SS_tt") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ||
               label.find("ST") !=string::npos ||
               label.find("TTX") !=string::npos ||
               label.find("TTJets") !=string::npos ||
               label.find("TT_TuneCUETP8M2T4") !=string::npos ||
               label.find("TT_Mtt") !=string::npos
               ) {
        pdgID1=24;
        pdgID2=24;
    } else if (label.find("DIBOSON") !=string::npos ||
               label.find("TRIBOSON") !=string::npos ||
               label.find("MULTIBOSON") !=string::npos ||
               label.find("ZNuNuWPMJJjj_EWK")!=string::npos ||
               label.find("ZNuNuWPMJJjj_QCD")!=string::npos ||
               label.find("ZbbWpmJJ_EWK")!=string::npos ||
               label.find("ZbbWpmJJ_QCD")!=string::npos
               ) {
        pdgID1=23;
        pdgID2=24;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);
        if( ! genpar->IsLHE()) continue;

        // ** BOSON
        if(fabs(genpar->GetPdgId()) == pdgID1) if(genVp==NULL) { genVp = genpar; /*cout << "found W1 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }
        if(fabs(genpar->GetPdgId()) == pdgID2) if(genVp2==NULL and genpar!=genVp) { genVp2 = genpar; /*cout << "found W2 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }

        // ** DECAY PRODUCT
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID1) if(dauV1a==NULL) { dauV1a = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID1) if(dauV1b==NULL and genpar!=dauV1a) { dauV1b = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID2) if(dauV2a==NULL) { dauV2a = genpar; }
        if(fabs(genpar->GetPdgId()) < 5 and fabs(genpar->GetParentPdgId())==pdgID2) if(dauV2b==NULL and genpar!=dauV2a) { dauV2b = genpar; }

        // ** for ZbbZhadJJ
        if(fabs(genpar->GetPdgId()) == 5 and fabs(genpar->GetParentPdgId())==pdgID1) V1isZbb=true;
        if(fabs(genpar->GetPdgId()) == 5 and fabs(genpar->GetParentPdgId())==pdgID2) V2isZbb=true;

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

    int topology=0;
    if(genVp!=NULL and genVp2!=NULL) {
        if(genVp->Pt() > 200 and genVp2->Pt() > 200) topology=1;
        if((genVp->Pt() > 200 and genVp2->Pt() < 200) || (genVp->Pt() < 200 and genVp2->Pt() > 200) ) topology=2;
    }

    // at least one boosted
    //    if(genWp==NULL or genWp2==NULL) return EVENT_NOT_USED;
    //    if(genWp->Pt() < 200 and genWp2->Pt() < 200) return EVENT_NOT_USED;

    if(genVp!=NULL and genVp2!=NULL) {
        if(doMETAnalysis) {
            //            evt_MVV_gen = ChargedHiggs::mt(genVp->GetP4().Pt(),genVp2->GetP4().Pt(),genVp->GetP4().Phi(),genVp2->GetP4().Phi());
            evt_MVV_gen = ChargedHiggs::mtMassive(genVp->GetP4(),genVp2->GetP4());
            //            evt_MVV_gen = (genVp->GetP4()+genVp2->GetP4()).Mt();
        } else {
            evt_MVV_gen = (genVp->GetP4()+genVp2->GetP4()).M();
        }
    }

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

    bool passtriggerbtag1 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6_v");
    bool passtriggerbtag2 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160_v");
    bool passtriggerbtag3 = e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200_v");

    bool passtriggerHad1 = e->IsTriggered("HLT_PFHT900_v");
    bool passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450_v");
    bool passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v");
    bool passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v");
    bool passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v");
    bool passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");
    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6;

    bool passtriggerbtagOR = passtriggerbtag1 || passtriggerbtag2 || passtriggerbtag3 || passtriggerHadOR;

    if(category.find("RBtag")   !=string::npos){

        Fill("VBShadAnalysis/Baseline/mVV_RBtag_Base_" +label, systname, evt_MVV, e->weight() );

        if(passtriggerbtag1) Fill("VBShadAnalysis/Baseline/mVV_RBtag_C100_DoubleBTagCSV_p014_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtag2) Fill("VBShadAnalysis/Baseline/mVV_RBtag_C100_DoubleBTagCSV_p026_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtag3) Fill("VBShadAnalysis/Baseline/mVV_RBtag_QuadPFJet_BTagCSV_Mqq200_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/mVV_RBtag_had_OR_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtagOR) Fill("VBShadAnalysis/Baseline/mVV_RBtag_btag_OR_" +label, systname, evt_MVV, e->weight() );
    }

    if(category.find("BBtag")   !=string::npos){

        Fill("VBShadAnalysis/Baseline/mVV_BBtag_Base_" +label, systname, evt_MVV, e->weight() );

        if(passtriggerbtag1) Fill("VBShadAnalysis/Baseline/mVV_BBtag_C100_DoubleBTagCSV_p014_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtag2) Fill("VBShadAnalysis/Baseline/mVV_BBtag_C100_DoubleBTagCSV_p026_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtag3) Fill("VBShadAnalysis/Baseline/mVV_BBtag_QuadPFJet_BTagCSV_Mqq200_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/mVV_BBtag_had_OR_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerbtagOR) Fill("VBShadAnalysis/Baseline/mVV_BBtag_btag_OR_" +label, systname, evt_MVV, e->weight() );
    }

    // - HAD

    if(category.find("RB")   !=string::npos){
        Fill("VBShadAnalysis/Baseline/mVV_RB_Base_" +label, systname, evt_MVV, e->weight() );

        if(passtriggerHad1) Fill("VBShadAnalysis/Baseline/mVV_RB_PFHT900_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad2) Fill("VBShadAnalysis/Baseline/mVV_RB_AK8PFJet450_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad3) Fill("VBShadAnalysis/Baseline/mVV_RB_AK8PFHT700_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad4) Fill("VBShadAnalysis/Baseline/mVV_RB_AK8PFJet360_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad5) Fill("VBShadAnalysis/Baseline/mVV_RB_AK8DiPFJet300_200_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad6) Fill("VBShadAnalysis/Baseline/mVV_RB_PFHT650_Wide_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/mVV_RB_had_OR_" +label, systname, evt_MVV, e->weight() );
    }

    if(category.find("BB")   !=string::npos){
        Fill("VBShadAnalysis/Baseline/mVV_BB_Base_" +label, systname, evt_MVV, e->weight() );

        if(passtriggerHad1) Fill("VBShadAnalysis/Baseline/mVV_BB_PFHT900_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad2) Fill("VBShadAnalysis/Baseline/mVV_BB_AK8PFJet450_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad3) Fill("VBShadAnalysis/Baseline/mVV_BB_AK8PFHT700_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad4) Fill("VBShadAnalysis/Baseline/mVV_BB_AK8PFJet360_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad5) Fill("VBShadAnalysis/Baseline/mVV_BB_AK8DiPFJet300_200_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHad6) Fill("VBShadAnalysis/Baseline/mVV_BB_PFHT650_Wide_" +label, systname, evt_MVV, e->weight() );
        if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/mVV_BB_had_OR_" +label, systname, evt_MVV, e->weight() );
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
    selectedFatJets.clear();
    selectedFatZbb.clear();
    for(unsigned i=0;i<e->NFatJets() ; ++i)
    {

        FatJet *f=e->GetFatJet(i);

        //        std::cout << " Pt=" << f->Pt() << " SDMass=" << f->SDMass() << " IsZbbJet = " << f->IsZbbJet() << "  IsWJet = " << f->IsWJet()  << std::endl;

        Double_t minDR1=9999;
        Double_t minDR2=9999;
        if(genVp!=NULL and genVp2!=NULL) {
            Double_t delW = f->DeltaR(*genVp);
            if (delW < minDR1){ minDR1 = delW;}
            Double_t delW2 = f->DeltaR(*genVp2);
            if (delW2 < minDR2){ minDR2 = delW2;}
        }

        if(minDR1<0.8 or minDR2<0.8) {
            // for both W and Zbb not necessarity is matched with one jet
            Fill("VBShadAnalysis/Baseline/SDMass_FatJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJet_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJet_" +label, systname, f->WvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/TvsQCD_FatJet_" +label, systname, f->TvsQCD(), e->weight() );
            if(f->WvsQCD()>0 and f->TvsQCD()>0){
                double WvsT = f->WvsQCD()/ (f->WvsQCD() + f->TvsQCD());
                Fill("VBShadAnalysis/Baseline/WvsT_FatJet_" +label, systname, WvsT, e->weight() );
            }
            //            if(topology==1) Fill("VBShadAnalysis/Baseline/pT_FatJet_BB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==2) Fill("VBShadAnalysis/Baseline/pT_FatJet_RB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==0) Fill("VBShadAnalysis/Baseline/pT_FatJet_RR_" +label, systname, f->Pt(), e->weight() );
        } else {
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJetFake_" +label, systname, f->WvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/TvsQCD_FatJetFake_" +label, systname, f->TvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_" +label, systname, f->ZHccvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_" +label, systname, f->SDMass(), e->weight() );
        }

        if((minDR1<0.8 and V1isZbb) || (minDR2<0.8 and V2isZbb)) {
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJetFake_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_" +label, systname, f->ZHccvsQCD(), e->weight() );
        } else {
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJet_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJet_" +label, systname, f->ZHccvsQCD(), e->weight() );
        }

        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().Phi()));
        //        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().GetPuppiMetP4().Phi()));

        bool isZbbJet=false;
        if(f->IsZbbJet()) isZbbJet=true;
        //        if(doBAntiAnalysis and f->IsZbbJet()) isZbbJet=true;

        if(isZbbJet) {
            selectedFatZbb.push_back(f);
            bosonBBDiscr.push_back(f->ZHbbvsQCD());
            bosonBBMass.push_back(f->SDMass());
            bosonBBTDiscr.push_back(f->TvsQCD());
            Fill("VBShadAnalysis/Baseline/pT_FatZbbJet_" +label, systname, f->Pt(), e->weight() );
        }

        bool isWJet=false;
        if(!doHADAntiAnalysis and !doMETAntiAnalysis and !doBAntiAnalysis and f->IsWJet()) isWJet=true;
        if((doHADAntiAnalysis or doMETAntiAnalysis or doBAntiAnalysis) and f->IsWJetMirror()) isWJet=true;

        if(doMETAntiAnalysis)  doMETAnalysis=true;
        if(doBAntiAnalysis) doBAnalysis=true;

        if(isWJet) {
            if(doMETAnalysis and dPhiFatMet<0.4) continue;
            if(f->IsZbbJet()) continue;
            selectedFatJets.push_back(f);
            bosonVDiscr.push_back(f->WvsQCD());
            bosonTDiscr.push_back(f->TvsQCD());
            bosonMass.push_back(f->SDMass());
            Fill("VBShadAnalysis/Baseline/pT_FatJet_" +label, systname, f->Pt(), e->weight() );
            Fill("VBShadAnalysis/Baseline/DphiMETFat_" +label, systname, dPhiFatMet, e->weight() );

        }
    }

    Fill("VBShadAnalysis/Baseline/NFatJet_" +label, systname, selectedFatJets.size(), e->weight() );

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 5, e->weight() );  //NFatjet cut

    minDPhi=999;

    //AK4
    selectedJets.clear();

    counterExtrabToVeto_=0;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);

        // COUNT additional b-veto (20 GeV-Medium)
        if (j->GetDeepB() > DEEP_B_MEDIUM) {
            if(selectedFatZbb.size()>0) {
                if( j->DeltaR(selectedFatZbb[0]) < 1.2 ) continue;
            }
            // second Zbb eventually only on the BBtag
            if(doBAnalysis && selectedFatZbb.size()>1) {
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
        if (j->GetDeepB() > DEEP_B_MEDIUM) continue;
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
        // second Zbb eventually only on the BBtag
        if(doBAnalysis and selectedFatZbb.size()>1) {
            if( j->DeltaR(selectedFatZbb[1]) < 1.2 ) continue;
        }

        counter++;
        double dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().Phi()));
        //        double dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().GetPuppiMetP4().Phi()));

        // first 2,4 jets in the BMET,RMET
        if(selectedFatJets.size()>0 and counter<3 and (dphi < minDPhi)) minDPhi = dphi;
        if(selectedFatJets.size()==0 and counter<5 and (dphi < minDPhi)) minDPhi = dphi;

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

void VBShadAnalysis::setTree(Event*e, string label, string category )
{

    SetTreeVar("run",e->runNum());
    SetTreeVar("lumi",e->lumiNum());
    SetTreeVar("evt",e->eventNum());
    SetTreeVar("isRealData",e->IsRealData());

    SetTreeVar("weight",e->weight());
    SetTreeVar("xsec",e->GetWeight()->GetBareMCXsec());
    SetTreeVar("npv",e->Npv());

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

    if(label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ) mc = 11 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ) mc = 12 ;
    if(label.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos ) mc = 13 ;
    if(label.find("aQGC_ZJJZJJjj") !=string::npos ) mc = 20 ;

    // multiboson
    if(label.find("MULTIBOSON") !=string::npos) mc = 100 ;
    if(label.find("TRIBOSON") !=string::npos) mc = 110 ;
    if(label.find("DIBOSON") !=string::npos) mc = 120 ;
    // with Top
    if(label.find("TT_TuneCUETP8M2T4") !=string::npos) mc =200 ;
    if(label.find("TT_Mtt") !=string::npos) mc =201 ;
    if(label.find("TTJets") !=string::npos) mc =202 ;

    if(label.find("TTX") !=string::npos) mc =205 ;
    if(label.find("ST") !=string::npos) mc =210 ;
    // V+jets
    if(label.find("ZJetsToNuNu") !=string::npos) mc = 300 ;
    if(label.find("WJetsToLNu") !=string::npos) mc = 310 ;
    if(label.find("DY") !=string::npos) mc = 320 ;
    if(label.find("ZJetsToQQ") !=string::npos) mc = 330 ;
    if(label.find("WJetsToQQ") !=string::npos) mc = 340 ;

    if(label.find("QCD_HT") !=string::npos) mc =500 ;
    if(label.find("QCD_Inclusive") !=string::npos) mc =501 ;
    if(label.find("QCD_Pt") !=string::npos) mc =502 ;

    SetTreeVar("mc",mc);

    // $$$$ analysis variable
    SetTreeVar("varMjj",evt_Mjj);
    SetTreeVar("varDetajj",evt_Detajj);
    SetTreeVar("varDphijj",evt_Dphijj);
    SetTreeVar("varJet2Eta",evt_Jet2Eta);
    SetTreeVar("varJet2Pt",evt_Jet2Pt);
    SetTreeVar("j1Unc",evt_j1unc);
    SetTreeVar("j2Unc",evt_j2unc);


    SetTreeVar("genMVV",evt_MVV_gen);
    SetTreeVar("varMVV",evt_MVV);
    SetTreeVar("varPTVV",evt_PTVV);
    SetTreeVar("varPTV1",evt_PTV1);
    SetTreeVar("varPTV2",evt_PTV2);
    SetTreeVar("varDetaVV",evt_DetaVV);
    SetTreeVar("varPetaVV",evt_PetaVV);
    SetTreeVar("varEtaMinV",evt_EtaMinV);
    SetTreeVar("varEtaMaxV",evt_EtaMaxV);
    // Add varDphi VV

    SetTreeVar("varCen",evt_cenEta);
    SetTreeVar("varzepVB",evt_zepVB);
    SetTreeVar("varzepVV",evt_zepVV);
    SetTreeVar("varDRVj",evt_DRV1j);
    SetTreeVar("varnormPTVVjj",evt_normPTVVjj);
    SetTreeVar("varcenPTVVjj",evt_cenPTVVjj);
    SetTreeVar("varFW2j",evt_FW2);

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
    SetTreeVar("bosV1discr",evt_bosV1discr);
    SetTreeVar("bosV1tdiscr",evt_bosV1tdiscr);
    SetTreeVar("bosV1Unc", evt_bosV1unc);
    SetTreeVar("bosV2mass",evt_bosV2mass);
    SetTreeVar("bosV2discr",evt_bosV2discr);
    SetTreeVar("bosV2tdiscr",evt_bosV2tdiscr);
    SetTreeVar("bosV2Unc", evt_bosV2unc);
    SetTreeVar("bosV2chi2",evt_chi2_);
    SetTreeVar("bosV2dR",evt_DRV2);

    SetTreeVar("bosGen",int(evt_genmatch));

    // MVA
    SetTreeVar("BDTnoBnoMET",BDTnoBnoMET);
    SetTreeVar("BDTwithMET",BDTwithMET);
    SetTreeVar("BDTbtag",BDTbtag);

    SetTreeVar("BDTMultiwithMETzz",bdt_multi[0]);
    SetTreeVar("BDTMultiwithMETwz",bdt_multi[1]);
    SetTreeVar("BDTMultiwithMETbkg",bdt_multi[2]);

}


int VBShadAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );

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
    if ( label.find("WWW") !=string::npos) label = "TRIBOSON";
    if ( label.find("WWZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("WZZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("ZZZ") !=string::npos) label = "TRIBOSON";
    if ( label.find("WWTo") !=string::npos) label = "DIBOSON";
    if ( label.find("WZTo") !=string::npos) label = "DIBOSON";
    if ( label.find("ZZTo") !=string::npos) label = "DIBOSON";

    //    if ( label == "ST") label = "TT";
    if ( label.find("ttZ") !=string::npos) label = "TTX";
    if ( label.find("TTTT") !=string::npos) label = "TTX";
    if ( label.find("TTW") !=string::npos) label = "TTX";
    if ( label.find("TTG") !=string::npos) label = "TTX";
    if ( label.find("ttH") !=string::npos) label = "TTX";

    if ( label.find("EWKZ2Jets_ZToNuNu") !=string::npos) label = "ZJetsToNuNu";
    //    if ( label == "EWK_LNuJJ_MJJ-120") label = "WJetsToLNu";
    if ( label.find("EWK_LLJJ_MLL-50_MJJ-120") !=string::npos) label = "DY";
    if ( label.find("EWKWMinus2Jets") !=string::npos) label = "WJetsToLNu";
    if ( label.find("EWKWPlus2Jets") !=string::npos) label = "WJetsToLNu";

    //    if(label.find("ZJetsToQQ") !=string::npos) label = "DY";
    //    if(label.find("WJetsToQQ") !=string::npos) label = "WJetsToLNu"; ;

    //$$$$$$$$$
    //$$$$$$$$$ Merge TTbar
    //$$$$$$$$$

    if((label.find("TT_TuneCUETP8M2T4") !=string::npos) or (label.find("TT_Mtt") !=string::npos) ) {
        float Mtt = genMtt(e);
        Fill("VBShadAnalysis/BOSON/Mtt_" +label, systname, Mtt, e->weight() );
    }

    if (not e->IsRealData()
        and (label.find("TT_TuneCUETP8M2T4") != string::npos )
        and not e->ApplyMttReweight()) return EVENT_NOT_USED;

    if ( label == "TT_Mtt-1000toInf_TuneCUETP8M2T4" ) label = "TT_TuneCUETP8M2T4";
    if ( label == "TT_Mtt-700to1000_TuneCUETP8M2T4" ) label = "TT_TuneCUETP8M2T4";

    //$$$$$$$$$
    //$$$$$$$$$ genStudies
    //$$$$$$$$$

    genVp = NULL;
    genVp2 = NULL;
    dauV1a = NULL;
    dauV1b = NULL;
    dauV2a = NULL;
    dauV2b = NULL;

    genStudies(e, label );

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build cutflow
    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 0, e->weight() );

    // TRIGGER STORY
 
    bool passtriggerHad = e->IsTriggered("HLT_PFHT900_v")
                        || e->IsTriggered("HLT_AK8PFJet450_v")
                        || e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v")
                        || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v")
                        || e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v")
                        || e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");

    bool passtriggerMET = (e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));


    bool passtriggerBtag = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6_v")
                        || e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160_v")
                        || e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200_v") || passtriggerHad;   
    if(!doTrigger){
      //    bool passtriggerHAD = (e->IsTriggered("HLT_PFHT_800_v") || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v"));
      if (doHADAnalysis or doHADAntiAnalysis) {
        if(!passtriggerHad) return EVENT_NOT_USED;
      }

      if(doMETAnalysis or doMETAntiAnalysis) {
        //    bool passtriggerMET = (e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        if(!passtriggerMET) return EVENT_NOT_USED;
      }

      if(doBAnalysis) {
        if(!passtriggerBtag) return EVENT_NOT_USED;
      }
    }
    Fill("VBShadAnalysis/Cutflow_" +label, systname, 1, e->weight() );  //1--trigger

    // kill Top/W/Z
    if ( e->Nleps() > 0 ) return EVENT_NOT_USED;
    if ( e->Ntaus() > 0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 2, e->weight() );  //2--lep veto

    Fill("VBShadAnalysis/Baseline/Met_" +label, systname, e->GetMet().Pt(), e->weight() );
    Fill("VBShadAnalysis/Baseline/PuppiMet_" +label, systname, e->GetMet().GetPuppiMetP4().Pt(), e->weight() );

    if(usePuppi) {
        // events with MET in separate category
        if ( (doMETAnalysis or doMETAntiAnalysis) and e->GetMet().GetPuppiMetP4().Pt() < 250 ) return EVENT_NOT_USED;
        if ( (!doMETAnalysis and !doMETAntiAnalysis) and e->GetMet().GetPuppiMetP4().Pt() > 200 ) return EVENT_NOT_USED;
    } else {
        // events with MET in separate category
        if ( (doMETAnalysis or doMETAntiAnalysis) and e->GetMet().Pt() < 250 ) return EVENT_NOT_USED;
        if ( (!doMETAnalysis and !doMETAntiAnalysis) and e->GetMet().Pt() > 200 ) return EVENT_NOT_USED;
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 3, e->weight() );  //3--veto MET

    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and boson/forward jets
    //$$$$$$$$$ After this point doMETAntiAnalysis is set if doMETAnalysis
    getObjects(e, label , systname);

    //$$$$$$$$$
    // events with B in another category
    // withMET
    // noMET with B
    // noMET noB

    // note: e->Bjets() contains the pt>20: can raise at pt>30 for category with MET and B
    //    if ( ( doHADAnalysis or doHADAntiAnalysis ) and e->Bjets() > 0 ) return EVENT_NOT_USED;
    //    if ( doBAnalysis and (e->Bjets() == 0 or e->Bjets()>2) ) return EVENT_NOT_USED;
    if ( doMETAnalysis and e->Bjets()>2 ) return EVENT_NOT_USED;
    if ( !doBAntiAnalysis and counterExtrabToVeto_>0) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/NBJet_" +label, systname, e->Bjets(), e->weight() );
    Fill("VBShadAnalysis/Cutflow_" +label, systname, 4, e->weight() );  //4--veto b

    if ( doMETAnalysis and minDPhi<0.4) return EVENT_NOT_USED;
    if ( doMETAnalysis and TMath::Pi()-minDPhi<0.4) return EVENT_NOT_USED;

    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatJets.size()<1 ) return EVENT_NOT_USED;
    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatZbb.size()>0 ) return EVENT_NOT_USED;
    if ( doBAnalysis and selectedFatZbb.size()<1 ) return EVENT_NOT_USED;

    // THIS IS NOT OK FOR THE RMET analysis
    //    if ( doMETAnalysis and selectedFatJets.size()<0 and selectedFatZbb.size()<0) return EVENT_NOT_USED;


    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build categories below
    //$$$$$$$$$
    //$$$$$$$$$

    string category="";
    // for forwardJets pick the one with the largest mass

    if(!doMETAnalysis and !doBAnalysis) {
        forwardJets.clear();
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

        if(selectedFatJets.size()>1 and selectedJets.size()>1) {
            category="_BB";
            evt_genmatch = false;
            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
            evt_bosV1discr = bosonVDiscr[0];
            evt_bosV1tdiscr = bosonTDiscr[0];
            evt_bosV1mass = bosonMass[0];
            evt_bosV2discr = bosonVDiscr[1];
            evt_bosV2tdiscr = bosonTDiscr[1];
            evt_bosV2mass = bosonMass[1];

            p4VV = (selectedFatJets[0]->GetP4()+selectedFatJets[1]->GetP4());
            evt_MVV = p4VV.M();
            evt_PTVV = p4VV.Pt();
            evt_PTV1 = selectedFatJets[0]->GetP4().Pt();
            evt_PTV2 = selectedFatJets[1]->GetP4().Pt();
            evt_DetaVV = fabs(selectedFatJets[0]->DeltaEta(selectedFatJets[1]));
            evt_PetaVV = selectedFatJets[0]->GetP4().Eta() * selectedFatJets[1]->GetP4().Eta();
            evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),selectedFatJets[1]->Eta());
            evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),selectedFatJets[1]->Eta());
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }

            if(genVp==NULL or genVp2==NULL) evt_genmatch = false;
            if(genVp!=NULL and genVp2!=NULL) {
                bool match1 = false;
                bool match2 = false;
                if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and selectedFatJets[1]->DeltaR(*genVp2) < 0.2 ) match1 = true;
                if(selectedFatJets[1]->DeltaR(*genVp) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) match2 = true;
                if(match1 or match2) evt_genmatch = true;
            }
        }

        if(selectedFatJets.size()==1 and selectedJets.size()>3) {
            category="";
            double mBoson = 80.;
            double chi2Cut = 6.;
            float mWidth = 20.;

            // MARIA: RB
            float MV, chi2;
            std::tie(MV,chi2) = resolvedtagger(e, mBoson, label, systname, selectedFatJets[0]->Eta());

            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
            if( fabs(MV-mBoson) < mWidth and bosonJets.size()>1 and chi2<chi2Cut) {
                category="_RB";
                evt_bosV1discr = bosonVDiscr[0];
                evt_bosV1tdiscr = bosonTDiscr[0];
                evt_bosV1mass = bosonMass[0];
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_chi2_ = chi2;
                p4VV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4() );
                evt_MVV = p4VV.M();
                evt_PTVV = p4VV.Pt();
                evt_PTV1 = selectedFatJets[0]->GetP4().Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
                evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_DetaVV = fabs(selectedFatJets[0]->GetP4().Eta() - (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta());
                evt_PetaVV = selectedFatJets[0]->GetP4().Eta() * (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta();

            }
        }
    }


    //////////
    // with B

    if(doBAnalysis) {
        forwardJets.clear();
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
        if(selectedFatZbb.size()>0 and selectedFatJets.size()>0 and selectedJets.size()>1) {
            category="_BBtag";
            // add cases with two Zbb Zbb most pures
            evt_genmatch = false;

            Fill("VBShadAnalysis/Baseline/pT_BJet_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );

            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
            p4VV = (selectedFatJets[0]->GetP4()+selectedFatZbb[0]->GetP4());

            evt_MVV = p4VV.M();
            evt_PTVV = p4VV.Pt();
            evt_PTV1 = selectedFatZbb[0]->GetP4().Pt();
            evt_PTV2 = selectedFatJets[0]->GetP4().Pt();
            evt_bosV1discr = bosonBBDiscr[0];
            evt_bosV1mass = bosonBBMass[0];
            evt_bosV1unc = -1;
            evt_bosV1tdiscr = -1;
            evt_bosV2discr = bosonVDiscr[0];
            evt_bosV2tdiscr = bosonTDiscr[0];
            evt_bosV2mass = bosonMass[0];
            evt_bosV2unc = 0;
            evt_DetaVV = fabs(selectedFatJets[0]->DeltaEta(selectedFatZbb[0]));
            evt_PetaVV = selectedFatJets[0]->GetP4().Eta() * selectedFatZbb[0]->GetP4().Eta();
            evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),selectedFatZbb[0]->Eta());
            evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),selectedFatZbb[0]->Eta());
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }

            if(genVp==NULL or genVp2==NULL) evt_genmatch = false;
            if(genVp!=NULL and genVp2!=NULL) {
                bool match1 = false;
                bool match2 = false;
                if(selectedFatZbb[0]->DeltaR(*genVp) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) match1 = true;
                if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and selectedFatZbb[0]->DeltaR(*genVp2) < 0.2 ) match2 = true;
                if(match1 or match2) evt_genmatch = true;
            }
        }

        if(selectedFatZbb.size()>0 and selectedFatJets.size()==0 and selectedJets.size()>3) {
            category="";

            // target the ZbbZqq + ZbbWqq

            ///////$$$$$$$
            ///////$$$$$$$ evaluate both W and Z hypothesis
            ///////$$$$$$$

            string genWmat = "wrong_";
            string genZmat = "wrong_";
            double mBoson_W=80.;
            double mBoson_Z=91.;

            float MV_W, mW_chi2;
            std::tie(MV_W,mW_chi2) = resolvedtagger(e, mBoson_W, label, systname, selectedFatZbb[0]->Eta()); if(genMatchResolved(e,systname,label)) genWmat = "right_";
            if(bosonJets.size()>1){
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

            //// decide

            double chi2Cut=6.;
            string genmatch = "wrong_";
            float mBoson = mBoson_Z;
            float mWidth = 20.;

            double MV = MV_Z;
            double chi2 = mZ_chi2;
            genmatch = genZmat;

            if(mW_chi2 < mZ_chi2){
                mBoson = mBoson_W;
                genmatch = genWmat;
                std::tie(MV,chi2) = resolvedtagger(e, mBoson_W, label, systname, selectedFatZbb[0]->Eta());
                if(genMatchResolved(e,systname,label)) genWmat = "right_";
                // need to redo since due to  the jet assignement
            }

            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+genmatch+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+genmatch+label, systname, chi2, e->weight() );

            if(bosonJets.size()>1 && chi2<chi2Cut){
                Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+label, systname, MV, e->weight() );
                Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+genmatch+label, systname, MV, e->weight() );
            }

            ///////$$$$$
            ///////$$$$$

            if( fabs(MV-mBoson) < mWidth and bosonJets.size()>1 and chi2<chi2Cut) {
                category="_RBtag";
                p4VV = ( selectedFatZbb[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                evt_chi2_ = chi2;
                evt_MVV = p4VV.M();
                evt_PTVV = p4VV.Pt();
                evt_PTV1 = selectedFatZbb[0]->GetP4().Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_bosV1discr = bosonBBDiscr[0];
                evt_bosV1mass = bosonBBMass[0];
                evt_bosV1tdiscr = bosonBBTDiscr[0];
                evt_bosV1unc = 0;
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_bosV2discr = -1;
                evt_bosV2tdiscr = -1;
                evt_bosV2unc = 0;
                evt_EtaMinV = std::min(selectedFatZbb[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_EtaMaxV = std::max(selectedFatZbb[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_DetaVV = fabs(selectedFatZbb[0]->GetP4().Eta() - (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta());
                evt_PetaVV = selectedFatZbb[0]->GetP4().Eta() * (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta();

            }
        }
    }

    //////////
    // with MET

    if(doMETAnalysis) {
        forwardJets.clear();
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

        if((selectedFatJets.size()>0 or selectedFatZbb.size()>0) and selectedJets.size()>1) {
            category="_BMET";

            //        Current: (ET1+ET2)^2 - (PT1+PT2)^2    (also as shown in the current ch-higgs code you sent to me)
            //        New:      m^2 + PT2                                 (In code this is very easy to obtained: directly take      (p1+p2).Mt()     )

            TLorentzVector jetP4;
            //            cout << "is the selectedFatJets->SDMass() ok ?? "<< endl;
            if(selectedFatZbb.size()>0)
                jetP4.SetPtEtaPhiM(selectedFatZbb[0]->Pt(),selectedFatZbb[0]->Eta(),selectedFatZbb[0]->Phi(),selectedFatZbb[0]->SDMass());
            else
                jetP4.SetPtEtaPhiM(selectedFatJets[0]->Pt(),selectedFatJets[0]->Eta(),selectedFatJets[0]->Phi(),selectedFatJets[0]->SDMass());
            if(usePuppi) {
                p4VV = (e->GetMet().GetPuppiMetP4() + jetP4);
                evt_PTV1 = e->GetMet().GetPuppiMetP4().Pt();
                TLorentzVector metP4;
                metP4.SetPtEtaPhiM(e->GetMet().GetPuppiMetP4().Pt(),0.,e->GetMet().GetPuppiMetP4().Phi(),91);
                evt_MVV = ChargedHiggs::mtMassive(jetP4,metP4);
                //                evt_MVV = (jetP4+metP4).Mt();
                //                evt_MVV = ChargedHiggs::mt(jetP4.Pt(), e->GetMet().GetPuppiMetP4().Pt(), jetP4.Phi(), e->GetMet().GetPuppiMetP4().Phi());
            } else {
                p4VV = (e->GetMet().GetP4() + jetP4);
                evt_PTV1 = e->GetMet().GetP4().Pt();
                TLorentzVector metP4;
                metP4.SetPtEtaPhiM(e->GetMet().GetP4().Pt(),0.,e->GetMet().GetP4().Phi(),91);
                evt_MVV = ChargedHiggs::mtMassive(jetP4,metP4);
                //                evt_MVV = (jetP4+metP4).Mt();
                //                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().Pt(), selectedFatJets[0]->Phi(), e->GetMet().Phi());
            }
            evt_PTVV = p4VV.Pt();
            evt_PTV2 = jetP4.Pt();

            evt_bosV2unc = 0;

            if(selectedFatZbb.size()>0) { evt_bosV2discr = bosonBBDiscr[0]; evt_bosV2mass = bosonBBMass[0]; evt_bosV2tdiscr = bosonBBTDiscr[0]; }
            else {
                evt_bosV2discr = bosonVDiscr[0]; evt_bosV2tdiscr = bosonTDiscr[0]; evt_bosV2mass = bosonMass[0];
            }
            //            float Mjj=jettagForBoosted(e, label, systname, jetP4.Eta(),jetP4.Eta());
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }
        }

        // target the ZnnZqq + ZnnWqq
        if(selectedFatJets.size()==0 and selectedFatZbb.size()==0 and selectedJets.size()>3) {
            category="";

            string genWmat = "wrong_";
            string genZmat = "wrong_";
            double mBoson_W=80.;
            double mBoson_Z=91.;

            float MV_W, mW_chi2;
            std::tie(MV_W,mW_chi2) = resolvedtagger(e, mBoson_W, label, systname, 0.); if(genMatchResolved(e,systname,label)) genWmat = "right_";

            if(bosonJets.size()>1){
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

            //// decide

            double chi2Cut=6.;
            string genmatch = "wrong_";
            float mBoson = mBoson_Z;
            float mWidth = 20;

            double MV = MV_Z;
            double chi2 = mZ_chi2;
            genmatch = genZmat;

            if(mW_chi2 < mZ_chi2){
                genmatch = genWmat;
                mBoson = mBoson_W;
                std::tie(MV,chi2) = resolvedtagger(e, mBoson_W, label, systname, 0.);
                if(genMatchResolved(e,systname,label)) genWmat = "right_";
                // need to redo since due to  the jet assignement
            }

            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+genmatch+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, chi2, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+genmatch+label, systname, chi2, e->weight() );

            if(bosonJets.size()>1 && chi2<chi2Cut){
                Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+label, systname, MV, e->weight() );
                Fill("VBShadAnalysis/Baseline/ResBosonMassClean_"+genmatch+label, systname, MV, e->weight() );
            }

            if( fabs(MV-mBoson) < mWidth and bosonJets.size()>1 and chi2<chi2Cut) {
                category="_RMET";

                if(usePuppi) {
                    p4VV = (e->GetMet().GetPuppiMetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                    evt_PTV1 = e->GetMet().GetPuppiMetP4().Pt();
                    TLorentzVector metP4;
                    metP4.SetPtEtaPhiM(e->GetMet().GetPuppiMetP4().Pt(),0.,e->GetMet().GetPuppiMetP4().Phi(),91);
                    evt_MVV = ChargedHiggs::mtMassive(bosonJets[0]->GetP4() + bosonJets[1]->GetP4(), metP4);
                    //                    evt_MVV = (bosonJets[0]->GetP4()+metP4).Mt();
                    //                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt(), e->GetMet().GetPuppiMetP4().Pt(),
                    //                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().GetPuppiMetP4().Phi());

                } else {
                    p4VV = (e->GetMet().GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                    evt_PTV1 = e->GetMet().Pt();
                    TLorentzVector metP4;
                    metP4.SetPtEtaPhiM(e->GetMet().GetP4().Pt(),0.,e->GetMet().GetP4().Phi(),91);
                    evt_MVV = ChargedHiggs::mtMassive(bosonJets[0]->GetP4() + bosonJets[1]->GetP4(), metP4);
                    //                    evt_MVV = (bosonJets[0]->GetP4()+metP4).Mt();
                    //                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()), e->GetMet().Pt(),
                    //                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().Phi());
                }
                evt_PTVV = p4VV.Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_DRV2 = bosonJets[0]->DeltaR(bosonJets[1]);
                evt_bosV2discr = -1;
                evt_bosV2tdiscr = -1;
                evt_bosV2unc = 0;
                evt_chi2_ = chi2;
            }
        }
    }

    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/BOSON/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
    Fill("VBShadAnalysis/BOSON/BosonDecayRatio_" +label, systname, dauRatioV1, e->weight() );
    Fill("VBShadAnalysis/BOSON/BosonDecayRatio_" +label, systname, dauRatioV2, e->weight() );
    Fill("VBShadAnalysis/BOSON/CosThetaStar_" +label, systname, cosThetaV1, e->weight() );
    Fill("VBShadAnalysis/BOSON/CosThetaStar_" +label, systname, cosThetaV2, e->weight() );

    //////
    //$$$ CUT FLOW
    //////

    double MVV_cut=500;
    // CHECK THIS: adjust with trigger turn ones especially on BB and Btag
    if((category.find("RBtag")   !=string::npos ) or (category.find("RMET")   !=string::npos )) MVV_cut=400;

    if( evt_MVV < MVV_cut ) return EVENT_NOT_USED;

    if(label.find("ZnnZhadJJ_EWK") !=string::npos  ||
       label.find("ZbbZhadJJ_EWK")!=string::npos  ||
       label.find("WPhadWPhadJJ_EWK") !=string::npos ||
       label.find("ZNuNuWPMJJjj_EWK") !=string::npos ||
       label.find("ZNuNuWPMJJjj_QCD") !=string::npos ||
       label.find("ZbbWpmJJ_EWK") !=string::npos ||
       label.find("ZbbWpmJJ_QCD") !=string::npos ||
       label.find("WWjj_SS_ll") !=string::npos ||
       label.find("WWjj_SS_lt") !=string::npos ||
       label.find("WWjj_SS_tt") !=string::npos ||
       label.find("DoublyChargedHiggsGMmodel_HWW_M1000") !=string::npos ||
       label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
       label.find("DoublyChargedHiggsGMmodel_HWW_M2000") !=string::npos) {
        if(evt_MVV_gen!=0) Fill("VBShadAnalysis/MVVres" +category+"_"+label, systname, (evt_MVV-evt_MVV_gen)/evt_MVV_gen, e->weight() );
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 6, e->weight() );  //InvMFatjet cut

    Fill("VBShadAnalysis/Baseline/NJet_" +label, systname, forwardJets.size(), e->weight() );


    if(doTrigger){
      if ((category.find("RMET")   !=string::npos) || (category.find("BMET")   !=string::npos)){

        Fill("VBShadAnalysis/Baseline/mVV_MET_Base_" +label, systname, evt_MVV, e->weight() );

        bool passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
        bool passtriggermet2 = e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight");
        bool passtriggermet3 = e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v");
        bool passtriggermet4 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v");

        bool passtriggermetOR = passtriggermet1 || passtriggermet2 || passtriggermet3 || passtriggermet4;

        if(passtriggermet1) Fill("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermet2) Fill("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_NoiseCleaned_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermet3) Fill("VBShadAnalysis/Baseline/mVV_PFMET120_PFHT60_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermet4) Fill("VBShadAnalysis/Baseline/mVV_PFMETNoMu120_PFHT60_" +label, systname, evt_MVV, e->weight() );
        if(passtriggermetOR) Fill("VBShadAnalysis/Baseline/mVV_met_OR_" +label, systname, evt_MVV, e->weight() );
      }
    }


    if( forwardJets.size() < 2 ) return EVENT_NOT_USED;

    //    if(doTrigger) studyTriggers(e, category, label, systname);

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 7, e->weight() );  //NJet cut

    if( forwardJets[0]->Eta() * forwardJets[1]->Eta() >=0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 8, e->weight() );  //Jet seperate cut

    evt_Detajj = fabs(forwardJets[0]->DeltaEta(forwardJets[1]));

    if( evt_Detajj < 3. ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 9, e->weight() );  //Delta Eta cut

    p4jj = forwardJets[0]->GetP4() + forwardJets[1]->GetP4();

    evt_Mjj = p4jj.M();

    Fill("VBShadAnalysis/FWJETS/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 10, e->weight() ); //InvMjet cut

    //////
    //$$$ VARIOUS plots below for tree
    //////

    evt_j1unc = sqrt((forwardJets[0]->GetJESUnc())*(forwardJets[0]->GetJESUnc()) + Getjetres(forwardJets[0]) * Getjetres(forwardJets[0]));
    evt_j2unc = sqrt((forwardJets[1]->GetJESUnc())*(forwardJets[1]->GetJESUnc()) + Getjetres(forwardJets[1]) * Getjetres(forwardJets[1]));

    evt_Jet2Eta=forwardJets[1]->Eta();
    evt_Jet2Pt=forwardJets[1]->Pt();

    evt_Dphijj = ChargedHiggs::deltaPhi(forwardJets[0]->Phi(), forwardJets[1]->Phi());
    Fill("VBShadAnalysis/FWJETS/Dphijj" +category+"_"+label, systname, fabs(evt_Dphijj), e->weight() );

    p4VVjj = p4VV + p4jj;

    evt_normPTVVjj = fabs(p4VVjj.Pt())/(evt_PTV2 + evt_PTV1 + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());
    //    evt_normPTVVjj = fabs(p4VVjj.Pt())/(p4VV.Pt() + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());

    evt_cenPTVVjj = fabs( p4VV.Pt() - 0.5 * p4jj.Pt() ) / fabs(forwardJets[0]->GetP4().Pt() - forwardJets[1]->GetP4().Pt());

    double averageJJeta= 0.5 * (forwardJets[0]->Eta()+forwardJets[1]->Eta());

    if(!doMETAnalysis) {
        bool centrality0 = (forwardJets[0]->Eta() <  evt_EtaMinV or forwardJets[0]->Eta() > evt_EtaMaxV);
        bool centrality1 = (forwardJets[1]->Eta() <  evt_EtaMinV or forwardJets[1]->Eta() > evt_EtaMaxV);

        // this is already applied when choosing resolved
        if(!centrality0) return EVENT_NOT_USED;
        if(!centrality1) return EVENT_NOT_USED;
    }

    if(!doMETAnalysis) {

        evt_zepVV = fabs(p4VV.Eta() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/BOSON/ZepBosVVar" +category+"_"+label, systname, evt_zepVV, e->weight() );

        evt_cenEta = std::min(
                           evt_EtaMinV - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - evt_EtaMaxV
                           ) ;
        // why this is not taken from previous condition ?
        if( evt_cenEta < 0. ) return EVENT_NOT_USED;
    }

    if(selectedFatZbb.size()>0) {
        evt_zepVB = fabs(selectedFatZbb[0]->Eta() - averageJJeta)/fabs(evt_Detajj);
        evt_DRV1j = std::min(selectedFatZbb[0]->DeltaR(forwardJets[0]), selectedFatZbb[0]->DeltaR(forwardJets[1]));
    } else if (selectedFatJets.size()>0) {
        evt_zepVB = fabs(selectedFatJets[0]->Eta() - averageJJeta)/fabs(evt_Detajj);
        evt_DRV1j = std::min(selectedFatJets[0]->DeltaR(forwardJets[0]), selectedFatJets[0]->DeltaR(forwardJets[1]));
    } else if (bosonJets.size()>1 ) {
        TLorentzVector vP4 = bosonJets[0]->GetP4() + bosonJets[1]->GetP4();
        evt_zepVB = fabs(vP4.Eta() - averageJJeta)/fabs(evt_Detajj);
        evt_DRV1j = std::min(vP4.DeltaR(forwardJets[0]->GetP4()), vP4.DeltaR(forwardJets[1]->GetP4()));
        //        evt_zepV2 = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepVV = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
    }

    Fill("VBShadAnalysis/DRV1j" +category+"_"+label, systname, evt_DRV1j, e->weight() );
    Fill("VBShadAnalysis/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );
    Fill("VBShadAnalysis/normPTVVjj" +category+"_"+label, systname, evt_normPTVVjj, e->weight() );
    Fill("VBShadAnalysis/Dphimin" +category+"_"+label, systname, minDPhi, e->weight() );

    if((
        (category.find("BMET")   !=string::npos )
        or (category.find("RMET")   !=string::npos )
        ) and
       (evt_normPTVVjj > 0.25) ) return EVENT_NOT_USED;

    std::vector<TLorentzVector> oP4;
    oP4.push_back(p4VV);
    oP4.push_back(forwardJets[0]->GetP4());
    oP4.push_back(forwardJets[1]->GetP4());

    evt_FW2 = ChargedHiggs::FW_momentum(oP4, 2);
    Fill("VBShadAnalysis/FW2" +category+"_"+label, systname, evt_FW2, e->weight() );

    if( category.find("BBtag")   !=string::npos  ) {

        evt_mtop = ( selectedFatJets[0]->GetP4() +  forwardJets[0]->GetP4() ) .M();
        float mtop2 = ( selectedFatJets[0]->GetP4() +  forwardJets[1]->GetP4() ) .M();
        if(fabs(mtop2-175) < fabs(evt_mtop-175)) evt_mtop = mtop2;

    }

    //////
    //$$$ STUDY triggers
    //////

    if(doTrigger) studyTriggers(e, category, label, systname);

    //////
    //$$$ MONEY plots below
    //////

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
    Fill("VBShadAnalysis/PTVV" +category+"_"+label, systname, evt_PTVV, e->weight() );
    Fill("VBShadAnalysis/Detajj" +category+"_"+label, systname, evt_Detajj, e->weight() );
    Fill("VBShadAnalysis/pT1_Jet" +category+"_"+label, systname, forwardJets[0]->GetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/pT2_Jet" +category+"_"+label, systname, forwardJets[1]->GetP4().Pt(), e->weight() );
    Fill("VBShadAnalysis/eta1_Jet" +category+"_"+label, systname, fabs(forwardJets[0]->GetP4().Eta()), e->weight() );
    Fill("VBShadAnalysis/eta2_Jet" +category+"_"+label, systname, fabs(forwardJets[1]->GetP4().Eta()), e->weight() );

    // 
    Fill("VBShadAnalysis/pTV1_Jet" +category+"_"+label, systname, evt_PTV1, e->weight() );
    Fill("VBShadAnalysis/pTV2_Jet" +category+"_"+label, systname, evt_PTV2, e->weight() );

    if( (category.find("BMET")   !=string::npos)  or (category.find("BB")   !=string::npos) or (category.find("BBtag")   !=string::npos) ) {
        if(selectedFatJets.size()>0) Fill("VBShadAnalysis/etaV1_Jet" +category+"_"+label, systname, fabs(selectedFatJets[0]->GetP4().Eta()), e->weight() );
        if(selectedFatZbb.size()>0) Fill("VBShadAnalysis/etaV1_Jet" +category+"_"+label, systname, fabs(selectedFatZbb[0]->GetP4().Eta()), e->weight() );
    }

    if( (category.find("BB")   !=string::npos) ) {
        Fill("VBShadAnalysis/etaV2_Jet" +category+"_"+label, systname, fabs(forwardJets[1]->GetP4().Eta()), e->weight() );
    }

    if( doMETAnalysis and (category.find("BMET")   !=string::npos) ) {
        if(selectedFatJets.size()>0) Fill("VBShadAnalysis/pT_FatJet" +category+"_"+label, systname, selectedFatJets[0]->GetP4().Pt(), e->weight() );
        if(selectedFatZbb.size()>0) Fill("VBShadAnalysis/pT_FatJet" +category+"_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );
        Fill("VBShadAnalysis/WvsQCD_FatJet" +category+"_"+label, systname, evt_bosV2discr, e->weight() );
        Fill("VBShadAnalysis/SDMass_FatJet" +category+"_"+label, systname, evt_bosV2mass, e->weight() );
    }

    if(doMETAnalysis) Fill("VBShadAnalysis/Met" +category+"_"+label, systname, evt_PTV1, e->weight() );

    if(evt_Detajj < 5.) Fill("VBShadAnalysis/MVV" +category+"_low_"+label, systname, evt_MVV, e->weight() );
    if(evt_Detajj >= 5.) Fill("VBShadAnalysis/MVV" +category+"_high_"+label, systname, evt_MVV, e->weight() );

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

    if(doTMVA) ReadTmva();

    if(doTMVA and (doHADAnalysis or doHADAntiAnalysis)) BDTnoBnoMET = bdt[0];
    if(doTMVA and doMETAnalysis and (category.find("BMET")   !=string::npos)) BDTwithMET = bdt[3];
    if(doTMVA and doMETAnalysis and (category.find("RMET")   !=string::npos)) BDTwithMET = bdt[7];
    if(doTMVA and doBAnalysis and (category.find("RBtag")   !=string::npos)) BDTbtag = bdt[6];
    if(doTMVA and doBAnalysis and (category.find("BBtag")   !=string::npos) ) BDTbtag = bdt[8];

    if(doTMVA and !doBAnalysis and !doMETAnalysis) Fill ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+label, systname, BDTnoBnoMET, e->weight() );
    if(doTMVA and (doBAnalysis or doBAntiAnalysis)) Fill ("VBShadAnalysis/BDTbtag"+category+"_"+label, systname, BDTbtag, e->weight() );
    if(doTMVA and (doMETAnalysis or doMETAntiAnalysis)) {
        Fill ("VBShadAnalysis/BDTwithMET"+category+"_"+label, systname, BDTwithMET, e->weight() );
        Fill ("VBShadAnalysis/BDTMultiwithMETzz"+category+"_"+label, systname, bdt_multi[0], e->weight() );
        Fill ("VBShadAnalysis/BDTMultiwithMETwz"+category+"_"+label, systname, bdt_multi[1], e->weight() );
        Fill ("VBShadAnalysis/BDTMultiwithMETbkg"+category+"_"+label, systname, bdt_multi[2], e->weight() );
    }

    if(doWriteTree) {

        if (systname.find("NONE")    !=string::npos) {
            setTree(e,label,category);
            FillTree("tree_vbs");
        }

        if (systname.find("JESUp")    !=string::npos) {
            setTree(e,label,category);
            FillTree("tree_vbs_JESUp");
        }

        if (systname.find("JESDown")    !=string::npos) {
            setTree(e,label,category);
            FillTree("tree_vbs_JESDown");
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
