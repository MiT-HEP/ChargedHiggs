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

void VBShadAnalysis::BookHisto(string l, string category, string signalLabel)
{

    Book ("VBShadAnalysis/BOSON/ZepBosBVar"+category+"_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
    Book ("VBShadAnalysis/BOSON/ZepBosVVar"+category+"_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
    Book ("VBShadAnalysis/BOSON/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 100,0,2500);
    Book ("VBShadAnalysis/MVV"+category+"_"+l, "MVV ; MVV [GeV]; Events", 100,0,2500);
    Book ("VBShadAnalysis/MVV"+category+"_low_"+l, "MVV (low Deta JJ) ; MVV_{reco}; Events", 100, 0, 2500);
    Book ("VBShadAnalysis/MVV"+category+"_high_"+l, "MVV (high Deta JJ) ; MVV_{reco}; Events", 100, 0, 2500);

    Book ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+l, "BDT noBnoMET ; BDT noBnoMET [GeV]; Events", 200,-1.,1.);

    if(l.find("ZnnZhadJJ") !=string::npos  ||
       l.find("ZbbZhadJJ")!=string::npos  ||
       l.find("WPhadWPhadJJ") !=string::npos ||
       l.find("WWjj_SS_ll") !=string::npos ||
       l.find("WWjj_SS_lt") !=string::npos ||
       l.find("WWjj_SS_tt") !=string::npos ||
       l.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ) {
        Book ("VBShadAnalysis/MVV"+category+"_good_"+l, "MVV (good) ; MVV_{reco}; Events", 100, 0, 2500);
        Book ("VBShadAnalysis/MVV"+category+"_bad_"+l, "MVV (bad) ; MVV_{reco}; Events", 100, 0, 2500);
        Book ("VBShadAnalysis/MVVres"+category+"_"+l, "MVVres ; ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100, -5., 5.);
    }

    Book ("VBShadAnalysis/FWJETS/Mjj"+category+"_"+l, "Mjj (BB); M(j,j) [GeV]; Events", 35,0,3500.);
    Book ("VBShadAnalysis/FWJETS/Dphijj"+category+"_"+l, "Dphi jj (BB); #Delta#Phi(j,j) ; Events", 100,0,6.28);

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

void VBShadAnalysis::AddVariable( string name, char type, int r){
    cout<<"[TmvaAnalysis]::[AddVariable]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    cout<<"[TmvaAnalysis]::[DEBUG] AddVariables of type F to reader "<<r <<" and name "<<name<<endl;
    //    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    //    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    if ( type == 'I') readers_[r] -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') readers_[r] -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddVariable]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }
}

void VBShadAnalysis::ReadTmva(){

    bdt.clear();

    SetVariable("varMjj",evt_Mjj); //0
    SetVariable("varDetajj",evt_Detajj); //1
    //    SetVariable("abs(varDphijj)",evt_Dphijj); //2
    //    SetVariable("abs(varJet2Eta)",evt_Jet2Eta); //3
    SetVariable("varJet2Pt",evt_Jet2Pt); //4
    SetVariable("varMVV",evt_MVV); //5
    SetVariable("varPTVV",evt_PTVV); //6
    //    SetVariable("varDetaVV",evt_DetaVV); //7
    SetVariable("varPetaVV",evt_PetaVV); //7
    SetVariable("varCen",evt_cenEta); //8
    SetVariable("varnormPTVVjj",evt_normPTVVjj); //9

    //    vector<float> bdt;
    for(unsigned i =0 ;i< readers_.size() ; ++i) {
        bdt.push_back(readers_[i]->EvaluateMVA("BDT_VBSHad") );
    }

}

void VBShadAnalysis::InitTmva() {

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader: " << weights.size() <<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    cout << "---------------------------------------------" << endl;
    cout << " GOING TO BDTG " << endl;

    for (int i=0; i<1; i++) {
        AddVariable("varMjj",'F',i); //0
        AddVariable("varDetajj",'F',i); //1
        //        AddVariable("abs(varDphijj)",'F',i); //2
        //        AddVariable("abs(varJet2Eta)",'F',i); //3
        AddVariable("varJet2Pt",'F',i); //4
        AddVariable("varMVV",'F',i); //5
        AddVariable("varPTVV",'F',i); //6
        //        AddVariable("varDetaVV",'F',i); //7
        AddVariable("varPetaVV",'F',i); //7
        AddVariable("varCen",'F',i); //8
        AddVariable("varnormPTVVjj",'F',i); //9
    }

    cout << " GOING loop over weights" << weights.size() << endl;

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
        {
            cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
            readers_[i]->BookMVA("BDT_VBSHad",weights[i].c_str());
        }
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;

}

void VBShadAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

    Log(__FUNCTION__,"INFO",Form("doMETAnalysis=%d",doMETAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAnalysis=%d",doBAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAnalysis=%d",doHADAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAntiAnalysis=%d",doHADAntiAnalysis));
    Log(__FUNCTION__,"INFO",Form("doMETAntiAnalysis=%d",doMETAntiAnalysis));

    if(doMETAnalysis or doBAnalysis) doTMVA=false;

    if(doTMVA) InitTmva();

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
        //cutflow
        Book ("VBShadAnalysis/Cutflow_"+l, "cutflow; bit; Events", 12,0,12);

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
        Book ("VBShadAnalysis/Baseline/ResBosonChi2_"+l, "ResBosonChi2; Chi2 [GeV]; Events", 100, 0, 1000.);

        // RESONANT CASE

        Book ("VBShadAnalysis/OUT1500/MVV_"+l, "MVV-OUT (unclassified); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/IN1500/MVV_"+l, "MVV-IN (unclassified); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/OUT1500/MVV_BB_"+l, "MVV-OUT (BB); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/IN1500/MVV_BB_"+l, "MVV-IN (BB); MVV [GeV]; Events", 100,0,2500);

        Book ("VBShadAnalysis/OUT1500/Mjj_"+l, "Mjj-OUT (unclassified); Mjj [GeV]; Events", 100,0,3500);
        Book ("VBShadAnalysis/IN1500/Mjj_"+l, "Mjj-IN (unclassified); Mjj [GeV]; Events", 100,0,3500);
        Book ("VBShadAnalysis/OUT1500/Mjj_BB_"+l, "Mjj-OUT (BB); Mjj [GeV]; Events", 35,0,3500);
        Book ("VBShadAnalysis/IN1500/Mjj_BB_"+l, "Mjj-IN (BB); Mjj [GeV]; Events", 35,0,3500);

        BookHisto(l, "", "");
        BookHisto(l, "_BB", "");
        BookHisto(l, "_RB", "");
        BookHisto(l, "_BMET", "");
        BookHisto(l, "_RMET", "");
        BookHisto(l, "_BBtag", "");
        BookHisto(l, "_RBtag", "");

    } //end label loop

    if(writeTree) {
        // fill variables for miniTREE

        InitTree("tree_vbs");

        Branch("tree_vbs","run",'I');
        Branch("tree_vbs","lumi",'I');
        Branch("tree_vbs","evt",'I');
        Branch("tree_vbs","npv",'I');
        Branch("tree_vbs","weight",'D');
        Branch("tree_vbs","isRealData",'I');
        Branch("tree_vbs","xsec",'F');

        Branch("tree_vbs","NJets",'I');
        Branch("tree_vbs","NBJets",'I');
        Branch("tree_vbs","met_pt",'F');
        Branch("tree_vbs","met_phi",'F');

        Branch("tree_vbs","mc",'I'); // to distinguish between the different mc
        Branch("tree_vbs","ana_category",'I');

        // JJ
        Branch("tree_vbs","varMjj",'F');
        Branch("tree_vbs","varDetajj",'F');
        Branch("tree_vbs","varDphijj",'F');
        Branch("tree_vbs","varJet2Eta",'F');
        Branch("tree_vbs","varJet2Pt",'F');

        // VV
        Branch("tree_vbs","genMVV",'F');
        Branch("tree_vbs","varMVV",'F');
        Branch("tree_vbs","varPTVV",'F');
        Branch("tree_vbs","varPTV1",'F');
        Branch("tree_vbs","varPTV2",'F');
        Branch("tree_vbs","varDetaVV",'F');
        Branch("tree_vbs","varPetaVV",'F');
        Branch("tree_vbs","varEtaMinV",'F');
        Branch("tree_vbs","varEtaMaxV",'F');

        // MIX
        Branch("tree_vbs","varCen",'F');
        Branch("tree_vbs","varzepVB",'F');
        Branch("tree_vbs","varzepVV",'F');
        Branch("tree_vbs","varDRVj",'F');
        Branch("tree_vbs","varnormPTVVjj",'F');
        Branch("tree_vbs","varcenPTVVjj",'F');
        Branch("tree_vbs","varFW2j",'F');

        // bosonDECAY
        Branch("tree_vbs","dauRatioV1",'F');
        Branch("tree_vbs","dauRatioV2",'F');
        Branch("tree_vbs","cosThetaV1",'F');
        Branch("tree_vbs","cosThetaV2",'F');

        // bosonProperties
        Branch("tree_vbs","bosV1mass",'F');
        Branch("tree_vbs","bosV1discr",'F');
        Branch("tree_vbs","bosV1tdiscr",'F');
        Branch("tree_vbs","bosV2mass",'F');
        Branch("tree_vbs","bosV2discr",'F');
        Branch("tree_vbs","bosV2tdiscr",'F');
        Branch("tree_vbs","bosV2chi2",'F');

        //MVA
        Branch("tree_vbs","BDTnoBnoMET",'F');

    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

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


float VBShadAnalysis::resolvedtagger(Event*e, float MV, string label, string systname, float etaV1) {

    bosonJets.clear();
    forwardJets.clear();

    float const norm = 1000*1000; // 0.5TeV^2
    float const MVres = 20*20; // 20 GeV

    double DRij = 0; //Wjets
    double PTij = 0; //Wjets
    double Mij = 0; //Wjets
    double Mkl = 0; //forward jets
    evt_chi2_ = 999999;

    int index_i=-1;
    int index_j=-1;
    int index_k=-1;
    int index_l=-1;

    for(unsigned i=0; i<selectedJets.size(); ++i) {
        for(unsigned j=0; j<i; ++j) {

            Mij = selectedJets[i]->InvMass(selectedJets[j]);
            DRij = selectedJets[i]->DeltaR(selectedJets[j]);
            PTij = (selectedJets[i]->GetP4()+selectedJets[j]->GetP4()).Pt();

            float minEtaV = std::min(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());
            float maxEtaV = std::max(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());

            for(unsigned k=0; k<j; ++k) {
                for(unsigned l=0; l<k; ++l) {

                    if( selectedJets[k]->Pt()<50 ) continue;
                    if( selectedJets[l]->Pt()<50 ) continue;

                    // remove and cut later to reduce combinatorics
                    //                    if( selectedJets[k]->Eta() * selectedJets[l]->Eta() >=0 ) continue;

                    if( selectedJets[k]->Eta() <  minEtaV or selectedJets[k]->Eta() > maxEtaV ) {
                        if( selectedJets[l]->Eta() <  minEtaV or selectedJets[l]->Eta() > maxEtaV ) {
                            Mkl = selectedJets[k]->InvMass(selectedJets[l]);
                            //                            double chi2 = (norm / (Mkl*Mkl)) + (Mij*Mij - MV*MV)/MVres;
                            // DR ~ 2M/PT
                            double chi2 = (norm / (Mkl*Mkl)) + (0.5*DRij*PTij - MV) * (0.5*DRij*PTij - MV) / MVres;
                            if(chi2<evt_chi2_) { evt_chi2_=chi2; index_i=i; index_j=j; index_k=k; index_l=l; }
                        }
                    }
                }
            }
        }
    }

    for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
        if(iter==index_i) bosonJets.push_back(selectedJets[index_i]);
        if(iter==index_j) bosonJets.push_back(selectedJets[index_j]);
        if(iter==index_k) forwardJets.push_back(selectedJets[index_k]);
        if(iter==index_l) forwardJets.push_back(selectedJets[index_l]);
    }

    //    return  Mij;
    if(bosonJets.size()>1) return  (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
    else return 0;

}


double VBShadAnalysis::genMtt(Event*e)
{

    GenParticle * genT = NULL;
    GenParticle * genTbar = NULL;
    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        if(genpar->GetPdgId() == 6) if(genT==NULL) { genT = genpar; }
        if(genpar->GetPdgId() == -6) if(genTbar==NULL) { genTbar = genpar; }
    }

    double Mtt= (genT->GetP4()+genTbar->GetP4()).M();

    return Mtt;

}


void VBShadAnalysis::genStudies(Event*e, string label )
{

    int pdgID1=24;
    int pdgID2=24;

    if((label.find("ZnnZhadJJ") !=string::npos )
       || (label.find("ZbbZhadJJ")!=string::npos )) {
        pdgID1=23;
        pdgID2=23;
    } else if (label.find("WPhadWPhadJJ") !=string::npos ||
               label.find("WWjj_SS_ll") !=string::npos ||
               label.find("WWjj_SS_lt") !=string::npos ||
               label.find("WWjj_SS_tt") !=string::npos ||
               label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ||
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
               label.find("MULTIBOSON") !=string::npos
               ) {
        pdgID1=23;
        pdgID2=24;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

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

        // ANTILOOSE
        //        if(f->IsZbbJetMirror() and doBAnalysis) {
        if(f->IsZbbJet() and doBAnalysis) {
            selectedFatZbb.push_back(f);
            bosonBBDiscr.push_back(f->ZHbbvsQCD());
            bosonBBMass.push_back(f->SDMass());
            Fill("VBShadAnalysis/Baseline/pT_FatZbbJet_" +label, systname, f->Pt(), e->weight() );
        }

        bool isWJet=false;
        if(!doHADAntiAnalysis and !doMETAntiAnalysis and f->IsWJet()) isWJet=true;
        if((doHADAntiAnalysis or doMETAntiAnalysis) and f->IsWJetMirror()) isWJet=true;
        if(doMETAntiAnalysis)  doMETAnalysis=true;

        if(isWJet) {
            if(doMETAnalysis and dPhiFatMet<0.4) continue;
            if(doBAnalysis and f->IsZbbJet()) continue;
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
            if(doBAnalysis and selectedFatZbb.size()>0) {
                if( j->DeltaR(selectedFatZbb[0]) < 1.2 ) continue;
            }
            if(doBAnalysis and selectedFatZbb.size()>1) {
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
        if(doBAnalysis and selectedFatZbb.size()>0) {
            if( j->DeltaR(selectedFatZbb[0]) < 1.2 ) continue;
        }
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
    if(label.find("WPhadWPhadJJ") !=string::npos ) mc = 1 ;
    if(label.find("ZbbZhadJJ") !=string::npos ) mc = 2 ;
    if(label.find("ZnnZhadJJ") !=string::npos ) mc = 3 ;

    if(label.find("WWjj_SS_ll") !=string::npos ) mc = 8 ;
    if(label.find("WWjj_SS_lt") !=string::npos ) mc = 9 ;
    if(label.find("WWjj_SS_tt") !=string::npos ) mc = 10 ;

    if(label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos ) mc = 11 ;
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

    if(label.find("QCD_HT") !=string::npos) mc =500 ;
    if(label.find("QCD_Inclusive") !=string::npos) mc =501 ;

    SetTreeVar("mc",mc);

    // $$$$ analysis variable
    SetTreeVar("varMjj",evt_Mjj);
    SetTreeVar("varDetajj",evt_Detajj);
    SetTreeVar("varDphijj",evt_Dphijj);
    SetTreeVar("varJet2Eta",evt_Jet2Eta);
    SetTreeVar("varJet2Pt",evt_Jet2Pt);

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

    // boson Decay
    SetTreeVar("dauRatioV1",dauRatioV1);
    SetTreeVar("dauRatioV2",dauRatioV2);
    SetTreeVar("cosThetaV1",cosThetaV1);
    SetTreeVar("cosThetaV2",cosThetaV2);

    // boson Properties
    SetTreeVar("bosV1mass",evt_bosV1mass);
    SetTreeVar("bosV1discr",evt_bosV1discr);
    SetTreeVar("bosV1tdiscr",evt_bosV1tdiscr);
    SetTreeVar("bosV2mass",evt_bosV2mass);
    SetTreeVar("bosV2discr",evt_bosV2discr);
    SetTreeVar("bosV2tdiscr",evt_bosV2tdiscr);
    SetTreeVar("bosV2chi2",evt_chi2_);

    // MVA
    SetTreeVar("BDTnoBnoMET",BDTnoBnoMET);

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
    if ( label == "WWW") label = "TRIBOSON";
    if ( label == "WWZ") label = "TRIBOSON";
    if ( label == "WZZ") label = "TRIBOSON";
    if ( label == "ZZZ") label = "TRIBOSON";
    if ( label == "WWTo") label = "DIBOSON";
    if ( label == "WZTo") label = "DIBOSON";
    if ( label == "ZZTo") label = "DIBOSON";

    //    if ( label == "ST") label = "TT";
    if ( label == "ttZ") label = "TTX";

    //$$$$$$$$$
    //$$$$$$$$$ Merge TTbar
    //$$$$$$$$$

    if((label.find("TT_TuneCUETP8M2T4") !=string::npos) or (label.find("TT_Mtt") !=string::npos) ) {
        double Mtt = genMtt(e);
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
    //    bool passtriggerHAD = (e->IsTriggered("HLT_PFHT_800_v") || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v"));
    if (doHADAnalysis or doHADAntiAnalysis) {
        bool passtriggerHAD = (e->IsTriggered("HLT_PFHT_800_v") ||
                               e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") ||
                               e->IsTriggered("HLT_AK8PFJet450_v") ||
                               e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") ||
                               e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v"));

        if(!passtriggerHAD) return EVENT_NOT_USED;
    }
    if(doMETAnalysis) {
        bool passtriggerMET = (e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        //    bool passtriggerMET = (e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        if(!passtriggerMET) return EVENT_NOT_USED;
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
        if ( doMETAnalysis and e->GetMet().GetPuppiMetP4().Pt() < 250 ) return EVENT_NOT_USED;
        if ( !doMETAnalysis and e->GetMet().GetPuppiMetP4().Pt() > 200 ) return EVENT_NOT_USED;
    } else {
        // events with MET in separate category
        if ( doMETAnalysis and e->GetMet().Pt() < 250 ) return EVENT_NOT_USED;
        if ( !doMETAnalysis and e->GetMet().Pt() > 200 ) return EVENT_NOT_USED;
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 3, e->weight() );  //3--veto MET

    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and boson/forward jets
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
    if ( (doHADAnalysis or doHADAntiAnalysis or doBAnalysis) and counterExtrabToVeto_>0) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/NBJet_" +label, systname, e->Bjets(), e->weight() );
    Fill("VBShadAnalysis/Cutflow_" +label, systname, 4, e->weight() );  //4--veto b


    if ( doMETAnalysis and minDPhi<0.4) return EVENT_NOT_USED;;
    if ( doMETAnalysis and TMath::Pi()-minDPhi<0.4) return EVENT_NOT_USED;;

    if ( doMETAnalysis and selectedFatJets.size()<0 ) return EVENT_NOT_USED;
    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatJets.size()<1 ) return EVENT_NOT_USED;
    if ( (doHADAnalysis or doHADAntiAnalysis) and selectedFatZbb.size()>0 ) return EVENT_NOT_USED;
    if ( doBAnalysis and selectedFatZbb.size()<1 ) return EVENT_NOT_USED;


    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build categories below
    //$$$$$$$$$
    //$$$$$$$$$

    string category="";
    string signalLabel="";
    // for forwardJets pick the one with the largest mass

    if(!doMETAnalysis and !doBAnalysis) {
        forwardJets.clear();
        p4VV*=0;
        evt_MVV=0;
        evt_PTV1=0;
        evt_PTV2=0;
        evt_DetaVV=-100;
        evt_PetaVV=-100;

        if(selectedFatJets.size()>1 and selectedJets.size()>1) {
            category="_BB";
            signalLabel="";
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

            if(genVp==NULL or genVp2==NULL) signalLabel="_bad";
            if(genVp!=NULL and genVp2!=NULL) {
                if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and selectedFatJets[1]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
                if(selectedFatJets[1]->DeltaR(*genVp) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
            }
        }

        if(selectedFatJets.size()==1 and selectedJets.size()>3) {
            category="";
            double mBoson=80.;
            double MV = resolvedtagger(e, mBoson, label, systname, selectedFatJets[0]->Eta());
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, evt_chi2_, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1 and evt_chi2_<10.) {
                category="_RB";
                signalLabel="";
                evt_bosV1discr = bosonVDiscr[0];
                evt_bosV1tdiscr = bosonTDiscr[0];
                evt_bosV1mass = bosonMass[0];
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                p4VV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4() );
                evt_MVV = p4VV.M();
                evt_PTVV = p4VV.Pt();
                evt_PTV1 = selectedFatJets[0]->GetP4().Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
                evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_DetaVV = fabs(selectedFatJets[0]->GetP4().Eta() - (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta());
                evt_PetaVV = selectedFatJets[0]->GetP4().Eta() * (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta();

                if(genVp==NULL or genVp2==NULL) signalLabel="_bad";
                if(genVp!=NULL and genVp2!=NULL) {
                    if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).DeltaR(genVp2->GetP4()) < 0.2 ) signalLabel="_good";
                    if((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).DeltaR(genVp->GetP4()) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
                }
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

        category="";
        if(selectedFatZbb.size()>0 and selectedFatJets.size()>0 and selectedJets.size()>1) {
            category="_BBtag";
            // add cases with two Zbb Zbb most pures
            signalLabel="";

            Fill("VBShadAnalysis/Baseline/pT_BJet_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );

            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
            p4VV = (selectedFatJets[0]->GetP4()+selectedFatZbb[0]->GetP4());

            evt_MVV = p4VV.M();
            evt_PTVV = p4VV.Pt();
            evt_PTV1 = selectedFatZbb[0]->GetP4().Pt();
            evt_PTV2 = selectedFatJets[0]->GetP4().Pt();
            evt_bosV1discr = bosonBBDiscr[0];
            evt_bosV1mass = bosonBBMass[0];
            evt_bosV2discr = bosonVDiscr[0];
            evt_bosV2tdiscr = bosonTDiscr[0];
            evt_bosV2mass = bosonMass[0];
            evt_DetaVV = fabs(selectedFatJets[0]->DeltaEta(selectedFatZbb[0]));
            evt_PetaVV = selectedFatJets[0]->GetP4().Eta() * selectedFatZbb[0]->GetP4().Eta();
            evt_EtaMinV = std::min(selectedFatJets[0]->Eta(),selectedFatZbb[0]->Eta());
            evt_EtaMaxV = std::max(selectedFatJets[0]->Eta(),selectedFatZbb[0]->Eta());
            //            float Mjj=jettagForBoosted(e, label, systname, evt_EtaMinV, evt_EtaMaxV);
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }

            if(genVp==NULL or genVp2==NULL) signalLabel="_bad";
            if(genVp!=NULL and genVp2!=NULL) {
                if(selectedFatJets[0]->DeltaR(*genVp) < 0.2 and selectedFatZbb[0]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
                if(selectedFatZbb[0]->DeltaR(*genVp) < 0.2 and selectedFatJets[0]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
            }

        }

        if(selectedFatZbb.size()>0 and selectedFatJets.size()==0 and selectedJets.size()>3) {
            category="";

            // target the ZbbZqq + ZbbWqq
            double mBoson=90.;
            double MV = resolvedtagger(e, mBoson, label, systname, selectedFatZbb[0]->Eta());
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, evt_chi2_, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1 and evt_chi2_<10.) {
                category="_RBtag";
                signalLabel="";

                p4VV = ( selectedFatZbb[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                evt_MVV = p4VV.M();
                evt_PTVV = p4VV.Pt();
                evt_PTV1 = selectedFatZbb[0]->GetP4().Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
                evt_bosV1discr = bosonBBDiscr[0];
                evt_bosV1mass = bosonBBMass[0];
                evt_bosV2mass = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_EtaMinV = std::min(selectedFatZbb[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_EtaMaxV = std::max(selectedFatZbb[0]->Eta(),float((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Eta()));
                evt_DetaVV = fabs(selectedFatZbb[0]->GetP4().Eta() - (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta());
                evt_PetaVV = selectedFatZbb[0]->GetP4().Eta() * (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta();

                if(genVp==NULL or genVp2==NULL) signalLabel="_bad";
                if(genVp!=NULL and genVp2!=NULL) {
                    if((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).DeltaR(genVp->GetP4()) < 0.2 and selectedFatZbb[0]->DeltaR(*genVp2) < 0.2 ) signalLabel="_good";
                    if(selectedFatZbb[0]->DeltaR(*genVp) < 0.2 and (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).DeltaR(genVp2->GetP4()) < 0.2 ) signalLabel="_good";
                }
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

        if(selectedFatJets.size()>0 and selectedJets.size()>1) {
            category="_BMET";
            signalLabel="";

            //        Current: (ET1+ET2)^2 - (PT1+PT2)^2    (also as shown in the current ch-higgs code you sent to me)
            //        New:      m^2 + PT2                                 (In code this is very easy to obtained: directly take      (p1+p2).Mt()     )

            TLorentzVector jetP4;
            jetP4.SetPtEtaPhiM(selectedFatJets[0]->Pt(),selectedFatJets[0]->Eta(),selectedFatJets[0]->Phi(),selectedFatJets[0]->SDMass());
            if(usePuppi) {
                p4VV = (e->GetMet().GetPuppiMetP4() + selectedFatJets[0]->GetP4());
                evt_PTV1 = e->GetMet().GetPuppiMetP4().Pt();
                TLorentzVector metP4;
                metP4.SetPtEtaPhiM(e->GetMet().GetPuppiMetP4().Pt(),0.,e->GetMet().GetPuppiMetP4().Phi(),91);
                evt_MVV = ChargedHiggs::mtMassive(jetP4,metP4);
                //                evt_MVV = (jetP4+metP4).Mt();
                //                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().GetPuppiMetP4().Pt(), selectedFatJets[0]->Phi(), e->GetMet().GetPuppiMetP4().Phi());
            } else {
                p4VV = (e->GetMet().GetP4() + selectedFatJets[0]->GetP4());
                evt_PTV1 = e->GetMet().GetP4().Pt();
                TLorentzVector metP4;
                metP4.SetPtEtaPhiM(e->GetMet().GetP4().Pt(),0.,e->GetMet().GetP4().Phi(),91);
                evt_MVV = ChargedHiggs::mtMassive(jetP4,metP4);
                //                evt_MVV = (jetP4+metP4).Mt();
                //                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().Pt(), selectedFatJets[0]->Phi(), e->GetMet().Phi());
            }
            evt_PTVV = p4VV.Pt();
            evt_PTV2 = selectedFatJets[0]->GetP4().Pt();
            evt_bosV2discr = bosonVDiscr[0];
            evt_bosV2tdiscr = bosonTDiscr[0];
            evt_bosV2mass = bosonMass[0];
            //            float Mjj=jettagForBoosted(e, label, systname, selectedFatJets[0]->Eta(),selectedFatJets[0]->Eta());
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }
        }

        // target the ZnnZqq + ZnnWqq
        if(selectedFatJets.size()==0 and selectedJets.size()>3) {
            category="";
            double mBoson=90.;
            // MARIA: dummy use of the centrality for now
            double MV = resolvedtagger(e, mBoson, label, systname, 0.);
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonChi2_"+label, systname, evt_chi2_, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1 and evt_chi2_<10.) {
                category="_RMET";
                signalLabel="";

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

    if(label.find("ZnnZhadJJ") !=string::npos  ||
       label.find("ZbbZhadJJ")!=string::npos  ||
       label.find("WPhadWPhadJJ") !=string::npos ||
       label.find("WWjj_SS_ll") !=string::npos ||
       label.find("WWjj_SS_lt") !=string::npos ||
       label.find("WWjj_SS_tt") !=string::npos ||
       label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos) {
        if(evt_MVV_gen!=0) Fill("VBShadAnalysis/MVVres" +category+"_"+label, systname, (evt_MVV-evt_MVV_gen)/evt_MVV_gen, e->weight() );
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 6, e->weight() );  //InvMFatjet cut

    Fill("VBShadAnalysis/Baseline/NJet_" +label, systname, forwardJets.size(), e->weight() );

    if( forwardJets.size() < 2 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 7, e->weight() );  //NJet cut

    // this is already applied when choosing resolved
    if( forwardJets[0]->Eta() * forwardJets[1]->Eta() >=0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 8, e->weight() );  //Jet seperate cut

    if(!doMETAnalysis) {
        bool centrality0 = (forwardJets[0]->Eta() <  evt_EtaMinV or forwardJets[0]->Eta() > evt_EtaMaxV);
        bool centrality1 = (forwardJets[1]->Eta() <  evt_EtaMinV or forwardJets[1]->Eta() > evt_EtaMaxV);

        // this is already applied when choosing resolved
        if(!centrality0) return EVENT_NOT_USED;
        if(!centrality1) return EVENT_NOT_USED;
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 9, e->weight() ); //Centrality cut

    evt_Detajj = fabs(forwardJets[0]->DeltaEta(forwardJets[1]));

    if( evt_Detajj < 3. ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 10, e->weight() );  //Delta Eta cut

    p4jj = forwardJets[0]->GetP4() + forwardJets[1]->GetP4();

    evt_Mjj = p4jj.M();

    Fill("VBShadAnalysis/FWJETS/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 11, e->weight() ); //InvMjet cut

    //////
    //$$$ VARIOUS plots below for tree
    //////

    evt_Jet2Eta=forwardJets[1]->Eta();
    evt_Jet2Pt=forwardJets[1]->Pt();

    evt_Dphijj = ChargedHiggs::deltaPhi(forwardJets[0]->Phi(), forwardJets[1]->Phi());
    Fill("VBShadAnalysis/FWJETS/Dphijj" +category+"_"+label, systname, evt_Dphijj, e->weight() );

    p4VVjj = p4VV + p4jj;

    evt_normPTVVjj = fabs(p4VVjj.Pt())/(evt_PTV2 + evt_PTV1 + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());
    //    evt_normPTVVjj = fabs(p4VVjj.Pt())/(p4VV.Pt() + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());

    evt_cenPTVVjj = fabs( p4VV.Pt() - 0.5 * p4jj.Pt() ) / fabs(forwardJets[0]->GetP4().Pt() - forwardJets[1]->GetP4().Pt());

    double averageJJeta= 0.5 * (forwardJets[0]->Eta()+forwardJets[1]->Eta());

    if(!doMETAnalysis) {
        evt_zepVV = fabs(p4VV.Eta() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/BOSON/ZepBosVVar" +category+"_"+label, systname, evt_zepVV, e->weight() );
        evt_cenEta = std::min(
                           evt_EtaMinV - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - evt_EtaMaxV
                           ) ;
    }

    if(selectedFatJets.size()>0) {
        evt_zepVB = fabs(selectedFatJets[0]->Eta() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepV2 = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepVV = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/BOSON/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );
        evt_DRV1j = std::min(selectedFatJets[0]->DeltaR(forwardJets[0]), selectedFatJets[0]->DeltaR(forwardJets[1]));
    }

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

    //////
    //$$$ MONEY plots below
    //////

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );

    if(label.find("ZnnZhadJJ") !=string::npos  ||
       label.find("ZbbZhadJJ")!=string::npos  ||
       label.find("WPhadWPhadJJ") !=string::npos ||
       label.find("WWjj_SS_ll") !=string::npos ||
       label.find("WWjj_SS_lt") !=string::npos ||
       label.find("WWjj_SS_tt") !=string::npos ||
       label.find("DoublyChargedHiggsGMmodel_HWW_M1500") !=string::npos) {
        Fill("VBShadAnalysis/MVV" +category+signalLabel+"_"+label, systname, evt_MVV, e->weight() );
    }

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

    if(doTMVA) BDTnoBnoMET = bdt[0];

    if(doTMVA and !doBAnalysis and !doMETAnalysis) Fill ("VBShadAnalysis/BDTnoBnoMET"+category+"_"+label, systname, BDTnoBnoMET, e->weight() );

    if(writeTree) setTree(e,label,category);
    if(writeTree) FillTree("tree_vbs");

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