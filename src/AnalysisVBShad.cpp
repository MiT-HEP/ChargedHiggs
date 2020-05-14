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
    j->SetDeepBCut(DEEP_B_MEDIUM);
    //    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535
    //    j->SetBCut(0.5426); //L=0.5426 , M=  0.8484, T0.9535
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
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

void VBShadAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

    Log(__FUNCTION__,"INFO",Form("doMETAnalysis=%d",doMETAnalysis));
    Log(__FUNCTION__,"INFO",Form("doBAnalysis=%d",doBAnalysis));
    Log(__FUNCTION__,"INFO",Form("doHADAnalysis=%d",doHADAnalysis));

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
        //cutflow
        Book ("VBShadAnalysis/Cutflow_"+l, "cutflow; bit; Events", 12,0,12);

        //Trigger
        Book("VBShadAnalysis/Baseline/HT_Base_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/HT_PFHT900_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_AK8PFJet450_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_AK8PFHT700_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_AK8PFJet360_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_AK8DiPFJet300_200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_PFHT650_Wide_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_had_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);


        Book("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p014_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p026_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_QuadPFJet_BTagCSV_Mqq200_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_btag_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);

        Book("VBShadAnalysis/Baseline/HT_PFMETNoMu120_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_PFMETNoMu120_NoiseCleaned_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_PFMET120_PFHT60_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_PFMETNoMu120_PFHT60_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);
        Book("VBShadAnalysis/Baseline/HT_met_OR_" +l, "mVV; mVV [GeV]; Events", 250,0,2500);


        //FatJet
        Book ("VBShadAnalysis/Baseline/NFatJet_"+l, "NFatJet; NFatJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
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

        // resolved
        Book ("VBShadAnalysis/Baseline/ResBosonMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/BOSON/ZepBosBVar_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_BB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_RB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_BBtag_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_RBtag_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_BMET_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_RMET_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);

        Book ("VBShadAnalysis/BOSON/ZepBosVVar_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_BB_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_RB_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_BBtag_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_RBtag_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_BMET_"+l, " ; |Y_{VV} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosVVar_RMET_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);

        // Search Variable
        Book ("VBShadAnalysis/MVV_"+l, "MVV (unclassified); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_"+l, "MVVres (unclassified); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_BB_"+l, "MVV (BB) ; MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_BB_"+l, "MVVres (BB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RB_"+l, "MVV (RB); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_RB_"+l, "MVVres (RB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);

        Book ("VBShadAnalysis/MVV_BBtag_"+l, "MVV (BB) ; MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_BBtag_"+l, "MVVres (BB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RBtag_"+l, "MVV (RB); MVV [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_RBtag_"+l, "MVVres (RB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);

        Book ("VBShadAnalysis/MVV_BMET_"+l, "MTVV (BMET) ; M_{T}(V,MET) [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_BMET_"+l, "MVMETres (BMET); ( M_{T}(V,MET) - MTVV_{gen} ) / MTVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RMET_"+l, "MVV (RMET); M_{T}(V,MET) [GeV]; Events", 100,0,2500);
        Book ("VBShadAnalysis/MVVres_RMET_"+l, "MVVres (RMET); ( M_{T}(V,MET) - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);

        //
        Book ("VBShadAnalysis/FWJETS/Mjj_BB_"+l, "Mjj (BB); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_RB_"+l, "Mjj (RB); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_BBtag_"+l, "Mjj (BB); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_RBtag_"+l, "Mjj (RB); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_BMET_"+l, "Mjj (BMET); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_RMET_"+l, "Mjj (RMET); M(j,j) [GeV]; Events", 35,0,3500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_"+l, "Mjj; Mjj [GeV]; Events", 35,0,3500.);

        Book ("VBShadAnalysis/FWJETS/Dphijj_"+l, "Dphi jj; #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_BB_"+l, "Dphi jj (BB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_RB_"+l, "Dphi jj (RB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_BBtag_"+l, "Dphi jj (BB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_RBtag_"+l, "Dphi jj (RB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_BMET_"+l, "Dphi jj (BMET); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_RMET_"+l, "Dphi jj (RMET); #Delta#Phi(j,j) ; Events", 100,0,6.28);

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
        Branch("tree_vbs","varMVVgen",'F');
        Branch("tree_vbs","varMVV",'F');
        Branch("tree_vbs","varPTVV",'F');
        Branch("tree_vbs","varPTV1",'F');
        Branch("tree_vbs","varPTV2",'F');
        Branch("tree_vbs","varDetaVV",'F');
        Branch("tree_vbs","varPetaVV",'F');
        Branch("tree_vbs","varEtaMinV",'F');
        Branch("tree_vbs","varEtaMaxV",'F');
        Branch("tree_vbs","varCen",'F');

        // MIX
        Branch("tree_vbs","varzepVB",'F');
        Branch("tree_vbs","varzepVV",'F');
        Branch("tree_vbs","varDRVj",'F');
        Branch("tree_vbs","varnormPTVVjj",'F');
        Branch("tree_vbs","varFW2j",'F');

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

    // Delta Eta of the two leading jets < 1.5

    bosonJets.clear();
    forwardJets.clear();

    float const norm = 1000*1000; // 1TeV^2
    float const MVres = 20*20; // 20 GeV

    double Mij = 0; //Wjets
    double Mkl = 0; //forward jets
    double chi2_ = 999999.;

    int index_i=-1;
    int index_j=-1;
    int index_k=-1;
    int index_l=-1;

    for(unsigned i=0; i<selectedJets.size(); ++i) {
        for(unsigned j=0; j<i; ++j) {

            Mij = selectedJets[i]->InvMass(selectedJets[j]);
            float minEtaV = std::min(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());
            float maxEtaV = std::max(etaV1,(float)(selectedJets[i]->GetP4() + selectedJets[j]->GetP4()).Eta());

            for(unsigned k=0; k<j; ++k) {
                for(unsigned l=0; l<k; ++l) {

                    if(selectedJets[k]->Pt()<50 ) continue;
                    if(selectedJets[l]->Pt()<50 ) continue;

                    if( selectedJets[k]->Eta() * selectedJets[l]->Eta() >=0 ) continue;

                    if(selectedJets[k]->Eta() <  minEtaV or selectedJets[k]->Eta() > maxEtaV) {
                        if(selectedJets[l]->Eta() <  minEtaV or selectedJets[l]->Eta() > maxEtaV) {
                            Mkl = selectedJets[k]->InvMass(selectedJets[l]);
                            double chi2 = norm / (Mkl*Mkl) + (Mij*Mij - MV*MV)/MVres;
                            if(chi2<chi2_) { chi2_=chi2; index_i=i; index_j=j; index_k= k; index_l=l; }
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

    return  Mij;
}


void VBShadAnalysis::genStudies(Event*e, string label )
{

    int pdgID1=24;
    int pdgID2=24;

    if((label.find("ZnnZhadJJ") !=string::npos )
       || (label.find("ZbbZhadJJ")!=string::npos )) {
        pdgID1=23;
        pdgID2=23;
    } else if (label.find("WPhadWPhadJJ") !=string::npos || label.find("TTX") !=string::npos || label.find("TT_TuneCUETP8M2T4") !=string::npos) {
        pdgID1=24;
        pdgID2=24;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        if(fabs(genpar->GetPdgId()) == pdgID1) if(genVp==NULL) { genVp = genpar; /*cout << "found W1 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }
        if(fabs(genpar->GetPdgId()) == pdgID2) if(genVp2==NULL and genpar!=genVp) { genVp2 = genpar; /*cout << "found W2 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }

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
            evt_MVV_gen = ChargedHiggs::mt(genVp->GetP4().Pt(),genVp2->GetP4().Pt(),genVp->GetP4().Phi(),genVp2->GetP4().Phi());
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
        //        std::cout << " pT " << f->Pt() << " ZHbbvsQCD " << f->ZHbbvsQCD() << std::endl;

        Double_t minDR=9999;
        if(genVp!=NULL and genVp2!=NULL) {
            Double_t delW = f->DeltaR(*genVp);
            if (delW < minDR){ minDR = delW;}
            Double_t delW2 = f->DeltaR(*genVp2);
            if (delW2 < minDR){ minDR = delW2;}
        }

        if(minDR<0.8) {
            // for both W and Zbb not necessarity is matched with one jet
            Fill("VBShadAnalysis/Baseline/SDMass_FatJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJet_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJet_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJet_" +label, systname, f->ZHccvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJet_" +label, systname, f->WvsQCD(), e->weight() );
            //            if(topology==1) Fill("VBShadAnalysis/Baseline/pT_FatJet_BB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==2) Fill("VBShadAnalysis/Baseline/pT_FatJet_RB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==0) Fill("VBShadAnalysis/Baseline/pT_FatJet_RR_" +label, systname, f->Pt(), e->weight() );
        } else {
            Fill("VBShadAnalysis/Baseline/ZHbbvsQCD_FatJetFake_" +label, systname, f->ZHbbvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/WvsQCD_FatJetFake_" +label, systname, f->WvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/TvsQCD_FatJetFake_" +label, systname, f->TvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/ZHccvsQCD_FatJetFake_" +label, systname, f->ZHccvsQCD(), e->weight() );
            Fill("VBShadAnalysis/Baseline/SDMass_FatJetFake_" +label, systname, f->SDMass(), e->weight() );
        }

        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().Phi()));
        //        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().GetPuppiMetP4().Phi()));

        Fill("VBShadAnalysis/Baseline/DphiMETFat_" +label, systname, dPhiFatMet, e->weight() );

        if(f->IsWJet()) {
            if(doMETAnalysis and dPhiFatMet<0.4) continue;
            selectedFatJets.push_back(f);
            Fill("VBShadAnalysis/Baseline/pT_FatJet_" +label, systname, f->Pt(), e->weight() );
        }

        if(f->IsZbbJet()) {
            selectedFatZbb.push_back(f);
            Fill("VBShadAnalysis/Baseline/pT_FatJet_" +label, systname, f->Pt(), e->weight() );
        }


    }

    Fill("VBShadAnalysis/Baseline/NFatJet_" +label, systname, selectedFatJets.size(), e->weight() );


    Fill("VBShadAnalysis/Cutflow_" +label, systname, 5, e->weight() );  //NFatjet cut


    double minDPhi=999;

    //AK4
    selectedJets.clear();

    int counter=0;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);
        if (j->GetDeepB() > DEEP_B_MEDIUM) continue;

        Fill("VBShadAnalysis/Baseline/pT_Jet_" +label, systname, j->Pt(), e->weight() );

        if(selectedFatJets.size()>0) {
            if( j->DeltaR(selectedFatJets[0]) < 1.2 ) continue;
        }
        if(selectedFatJets.size()>1) {
            if( j->DeltaR(selectedFatJets[1]) < 1.2 ) continue;
        }
        if(selectedFatJets.size()>2) {
            if( j->DeltaR(selectedFatJets[2]) < 1.2 ) continue;
        }

        counter++;
        double dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().Phi()));
        //        double dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().GetPuppiMetP4().Phi()));

        if(dphi < minDPhi) minDPhi = dphi;

        if(counter<5) Fill("VBShadAnalysis/Baseline/Dphimin_" +label, systname, dphi, e->weight() );

        /*
        for (auto const& fat : selectedFatJets) {
            if( j->DeltaR(fat) < 1.2 ) continue;
        }
        */
        //        if( j->DeltaR(genWp) < 1.2 ) continue;
        //        if( j->DeltaR(genWp2) < 1.2 ) continue;

        // counter should bs 4,2 for the RMET,BMET
        if(doMETAnalysis and minDPhi<0.2 and selectedFatJets.size()>0 and counter<3) continue;
        if(doMETAnalysis and minDPhi<0.2 and selectedFatJets.size()==0 and counter<5) continue;
        selectedJets.push_back(j);

    }

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

    if(label.find("MULTIBOSON") !=string::npos) mc = 100 ;
    if(label.find("TRIBOSON") !=string::npos) mc = 110 ;
    if(label.find("DIBOSON") !=string::npos) mc = 120 ;
    if(label.find("TT_TuneCUETP8M2T4") !=string::npos) mc =200 ;
    if(label.find("TTX") !=string::npos) mc =205 ;
    if(label.find("ST") !=string::npos) mc =210 ;
    if(label.find("ZJetsToNuNu") !=string::npos) mc = 300 ;

    if(label.find("QCD") !=string::npos) mc =500 ;

    SetTreeVar("mc",mc);

    // $$$$ analysis variable
    SetTreeVar("varMjj",evt_Mjj);
    SetTreeVar("varDetajj",evt_Detajj);
    SetTreeVar("varDphijj",evt_Dphijj);
    SetTreeVar("varJet2Eta",evt_Jet2Eta);
    SetTreeVar("varJet2Pt",evt_Jet2Pt);

    SetTreeVar("varMVVgen",evt_MVV_gen);
    SetTreeVar("varMVV",evt_MVV);
    SetTreeVar("varPTVV",evt_PTVV);
    SetTreeVar("varPTV1",evt_PTV1);
    SetTreeVar("varPTV2",evt_PTV2);
    SetTreeVar("varDetaVV",evt_DetaVV);
    SetTreeVar("varPetaVV",evt_PetaVV);
    SetTreeVar("varEtaMinV",evt_EtaMinV);
    SetTreeVar("varEtaMaxV",evt_EtaMaxV);
    SetTreeVar("varCen",evt_cen);
    // Add varDphi VV
    // Zep Var

    SetTreeVar("varzepVB",evt_zepVB);
    SetTreeVar("varzepVV",evt_zepVV);
    SetTreeVar("varDRVj",evt_DRV1j1);
    SetTreeVar("varnormPTVVjj",evt_normPTVVjj);
    SetTreeVar("varFW2j",evt_FW2);

}

int VBShadAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );

    /*
    // redefine plots
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

    genVp = NULL;
    genVp2 = NULL;

    genStudies(e, label );

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build cutflow
    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 0, e->weight() );

    // TRIGGER STORY

    bool passtriggerHAD = (e->IsTriggered("HLT_PFHT_800_v") || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v"));
//    if(doMETAnalysis) {
//        bool passtriggerMET = (e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        //    bool passtriggerMET = (e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
//        if(!passtriggerMET) return EVENT_NOT_USED;
//    }

    /*
    bool passtriggerHad1 = e->IsTriggered("HLT_PFHT900_v");
    bool passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450_v");
    bool passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v");
    bool passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v");
    bool passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v");
    bool passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");
    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6;

    //if(doHADAnalysis && !passtriggerHadOR) return EVENT_NOT_USED;
    */ 


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

    // events with B in another category
    // withMET
    // noMET with B
    // noMET noB
    if ( doHADAnalysis and e->Bjets() > 0 ) return EVENT_NOT_USED;
    if ( doBAnalysis and (e->Bjets() == 0 or e->Bjets()>2) ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/NBJet_" +label, systname, e->Bjets(), e->weight() );
    Fill("VBShadAnalysis/Cutflow_" +label, systname, 4, e->weight() );  //4--veto b

    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and boson/forward jets
    getObjects(e, label , systname);

    if ( doMETAnalysis and selectedFatJets.size()<0 ) return EVENT_NOT_USED;
    if ( !doMETAnalysis and selectedFatJets.size()<1 ) return EVENT_NOT_USED;

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
        //if(selectedFatJets[0]->GetP4().Pt() < 250) return EVENT_NOT_USED;

        if(selectedFatJets.size()>1 and selectedJets.size()>1) {
            category="_BB";
            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
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
        }

        if(selectedFatJets.size()==1 and selectedJets.size()>3) {
            category="";
            double mBoson=80.;
            double MV = resolvedtagger(e, mBoson, label, systname, selectedFatJets[0]->Eta());
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1) {
                category="_RB";
                p4VV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
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

    /*
    //We firstly tried whether works to put triggers here but seems no?
    if (!(category.find("BB")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );

    bool passtriggerHad1 = e->IsTriggered("HLT_PFHT900_v");
    bool passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450_v");
    bool passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v");
    bool passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v");
    bool passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v");
    bool passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");
    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6;

    if(passtriggerHad1) Fill("VBShadAnalysis/Baseline/HT_PFHT900_" +label, systname, evt_MVV, e->weight() );     
    if(passtriggerHad2) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet450_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad3) Fill("VBShadAnalysis/Baseline/HT_AK8PFHT700_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad4) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet360_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad5) Fill("VBShadAnalysis/Baseline/HT_AK8DiPFJet300_200_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad6) Fill("VBShadAnalysis/Baseline/HT_PFHT650_Wide_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/HT_OR_" +label, systname, evt_MVV, e->weight() );

    return EVENT_NOT_USED;
    // end the test of triggers
    */

    //////////
    // with B

    if(doBAnalysis) {
        forwardJets.clear();
        p4VV*=0;
        evt_MVV=0;
        evt_PTV1=0;
        evt_PTV2=0;
        evt_DetaVV=-100;
        evt_PetaVV=-100;

        if(selectedFatZbb.size()==1 and selectedFatJets.size()>1 and selectedJets.size()>1) {
            Fill("VBShadAnalysis/Baseline/pT_BJet_"+label, systname, selectedFatZbb[0]->GetP4().Pt(), e->weight() );

            // the Zbb should be one of the highest FatJet
            // to be checked if probability WvsQCDMD and ZHbbvsQCDMD make sense
            if(selectedFatZbb[0]->GetP4()!=selectedFatJets[0]->GetP4() and selectedFatZbb[0]->GetP4()!=selectedFatJets[1]->GetP4()) {
                category="";
            } else {
                category="_BBtag";
            }

            //            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
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
        }

        if(selectedFatZbb.size()==1 and selectedFatJets.size()==1 and selectedJets.size()>3) {
            category="";

            // target the ZbbZqq + ZbbWqq
            double mBoson=90.;
            double MV = resolvedtagger(e, mBoson, label, systname, selectedFatJets[0]->Eta());
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1) {
                category="_RBtag";
                p4VV = ( selectedFatZbb[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                evt_MVV = p4VV.M();
                evt_PTVV = p4VV.Pt();
                evt_PTV1 = selectedFatZbb[0]->GetP4().Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
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

        if(selectedFatJets.size()>0 and selectedJets.size()>1) {
            category="_BMET";
            if(usePuppi) {
                p4VV = (e->GetMet().GetPuppiMetP4() + selectedFatJets[0]->GetP4());
                evt_PTV1 = e->GetMet().Pt();
                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().GetPuppiMetP4().Pt(), selectedFatJets[0]->Phi(), e->GetMet().GetPuppiMetP4().Phi());
            } else {
                p4VV = (e->GetMet().GetP4() + selectedFatJets[0]->GetP4());
                evt_PTV1 = e->GetMet().GetP4().Pt();
                evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().Pt(), selectedFatJets[0]->Phi(), e->GetMet().Phi());
            }
            evt_PTVV = p4VV.Pt();
            evt_PTV2 = selectedFatJets[0]->GetP4().Pt();
            //            float Mjj=jettagForBoosted(e, label, systname, selectedFatJets[0]->Eta(),selectedFatJets[0]->Eta());
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                if(selectedJets[iter]->Pt()<50 ) continue;
                forwardJets.push_back(selectedJets[iter]);
            }
        }

        // target the ZbbZqq + ZbbWqq
        if(selectedFatJets.size()==0 and selectedJets.size()>3) {
            category="";
            double mBoson=90.;
            // MARIA: dummy use of the centrality for now
            double MV = resolvedtagger(e, mBoson, label, systname, 0.);
            if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, MV, e->weight() );
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1) {
                category="_RMET";
                if(usePuppi) {
                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt(), e->GetMet().GetPuppiMetP4().Pt(),
                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().GetPuppiMetP4().Phi());
                    p4VV = (e->GetMet().GetPuppiMetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                    evt_PTV1 = e->GetMet().GetPuppiMetP4().Pt();
                } else {
                    evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt(), e->GetMet().Pt(),
                                               (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().Phi());
                    p4VV = (e->GetMet().GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4());
                    evt_PTV1 = e->GetMet().Pt();

                }
                evt_PTVV = p4VV.Pt();
                evt_PTV2 = (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt();
            }
        }
    }




    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );

    double MVV_cut=500;
    if ((category.find("RMET") !=string::npos) || (category.find("RBtag")   !=string::npos)) MVV_cut=300;

    // CHECK THIS ?? 1050 ?? move to 500 for now
    // resolved likely all below M=500
    if( evt_MVV < MVV_cut ) return EVENT_NOT_USED;


    if (!(category.find("RMET")   !=string::npos) && !(category.find("BMET")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );


    bool passtriggermet1 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
    bool passtriggermet2 = e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight");
    bool passtriggermet3 = e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v");
    bool passtriggermet4 = e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v");

    bool passtriggermetOR = passtriggermet1 || passtriggermet2 || passtriggermet3 || passtriggermet4;

    if(passtriggermet1) Fill("VBShadAnalysis/Baseline/HT_PFMETNoMu120_" +label, systname, evt_MVV, e->weight() );
    if(passtriggermet2) Fill("VBShadAnalysis/Baseline/HT_PFMETNoMu120_NoiseCleaned_" +label, systname, evt_MVV, e->weight() );
    if(passtriggermet3) Fill("VBShadAnalysis/Baseline/HT_PFMET120_PFHT60_" +label, systname, evt_MVV, e->weight() );
    if(passtriggermet4) Fill("VBShadAnalysis/Baseline/HT_PFMETNoMu120_PFHT60_" +label, systname, evt_MVV, e->weight() );
    if(passtriggermetOR) Fill("VBShadAnalysis/Baseline/HT_met_OR_" +label, systname, evt_MVV, e->weight() );


    return EVENT_NOT_USED;






    if(evt_MVV_gen!=0) Fill("VBShadAnalysis/MVVres" +category+"_"+label, systname, (evt_MVV-evt_MVV_gen)/evt_MVV_gen, e->weight() );

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 6, e->weight() );  //InvMFatjet cut

    Fill("VBShadAnalysis/Baseline/NJet_" +label, systname, forwardJets.size(), e->weight() );

    if( forwardJets.size() < 2 ) return EVENT_NOT_USED;

    /*
    
    if (!(category.find("RBtag")   !=string::npos) && !(category.find("BBtag")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );

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

    if(passtriggerbtag1) Fill("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p014_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag2) Fill("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p026_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag3) Fill("VBShadAnalysis/Baseline/HT_QuadPFJet_BTagCSV_Mqq200_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/HT_had_OR_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtagOR) Fill("VBShadAnalysis/Baseline/HT_btag_OR_" +label, systname, evt_MVV, e->weight() );

    return EVENT_NOT_USED;

    
    //Let's see whether OK to put trigger after 2 forward jets selections
    if (!(category.find("BB")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );

    bool passtriggerHad1 = e->IsTriggered("HLT_PFHT900_v");
    bool passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450_v");
    bool passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v");
    bool passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v");
    bool passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v");
    bool passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");
    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6;

    if(passtriggerHad1) Fill("VBShadAnalysis/Baseline/HT_PFHT900_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad2) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet450_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad3) Fill("VBShadAnalysis/Baseline/HT_AK8PFHT700_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad4) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet360_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad5) Fill("VBShadAnalysis/Baseline/HT_AK8DiPFJet300_200_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad6) Fill("VBShadAnalysis/Baseline/HT_PFHT650_Wide_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/HT_OR_" +label, systname, evt_MVV, e->weight() );

    return EVENT_NOT_USED;

    //Here ends the test of the triggers    
    */

    evt_Jet2Eta=forwardJets[1]->Eta();
    evt_Jet2Pt=forwardJets[1]->Pt();

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 7, e->weight() );  //NJet cut

    // this is already applied when choosing
    if( forwardJets[0]->Eta() * forwardJets[1]->Eta() >=0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 8, e->weight() );  //Jet seperate cut

    evt_Detajj = fabs(forwardJets[0]->DeltaEta(forwardJets[1]));

    if( evt_Detajj < 3. ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 9, e->weight() );  //Delta Eta cut

    evt_Mjj= forwardJets[0]->InvMass(forwardJets[1]);
    evt_Dphijj = ChargedHiggs::deltaPhi(forwardJets[0]->Phi(), forwardJets[1]->Phi());

    bool centrality0 = (forwardJets[0]->Eta() <  evt_EtaMinV or forwardJets[0]->Eta() > evt_EtaMaxV);
    bool centrality1 = (forwardJets[1]->Eta() <  evt_EtaMinV or forwardJets[1]->Eta() > evt_EtaMaxV);

    if(!centrality0) return EVENT_NOT_USED;
    if(!centrality1) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 10, e->weight() ); //Centrality cut

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 11, e->weight() ); //InvMjet cut

    Fill("VBShadAnalysis/FWJETS/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    Fill("VBShadAnalysis/FWJETS/Dphijj" +category+"_"+label, systname, evt_Dphijj, e->weight() );

    p4VVjj = p4VV + forwardJets[0]->GetP4() + forwardJets[1]->GetP4();

    evt_normPTVVjj = fabs(p4VVjj.Pt())/(evt_PTV2 + evt_PTV1 + forwardJets[0]->GetP4().Pt() + forwardJets[1]->GetP4().Pt());

    double averageJJeta=(forwardJets[0]->Eta()+forwardJets[1]->Eta())/2;

    if(!doMETAnalysis) {
        evt_zepVV = fabs(p4VV.Eta() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/BOSON/ZepBosVVar" +category+"_"+label, systname, evt_zepVV, e->weight() );
        evt_cen = std::min(
                           evt_EtaMinV - std::min(forwardJets[0]->Eta(),forwardJets[1]->Eta()),
                           std::max(forwardJets[0]->Eta(),forwardJets[1]->Eta()) - evt_EtaMaxV
                           ) ;
    }

    if(selectedFatJets.size()>0) {
        evt_zepVB = fabs(selectedFatJets[0]->Eta() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepV2 = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        //        evt_zepVV = fabs(selectedFatJets[0]->Rapidity() - averageJJeta)/fabs(evt_Detajj);
        Fill("VBShadAnalysis/BOSON/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );
        evt_DRV1j1 = selectedFatJets[0]->DeltaR(forwardJets[0]);
    }

    std::vector<TLorentzVector> oP4;
    oP4.push_back(p4VV);
    oP4.push_back(forwardJets[0]->GetP4());
    oP4.push_back(forwardJets[1]->GetP4());

    evt_FW2 = ChargedHiggs::FW_momentum(oP4, 2);


/*
    //Let's see whether OK to put trigger after all VBS selections
 
    if (!(category.find("BBtag")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );

    bool passtriggerbtag1 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6_v");
    bool passtriggerbtag2 = e->IsTriggered("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160_v");
    bool passtriggerbtag3 = e->IsTriggered("HLT_DoubleJetsC112_DoubleBTagCSV_p014_DoublePFJetsC112MaxDeta1p6_v");
    bool passtriggerbtag4 = e->IsTriggered("HLT_DoubleJetsC112_DoubleBTagCSV_p026_DoublePFJetsC172_v");
    bool passtriggerbtag5 = e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq460_v");
    bool passtriggerbtag6 = e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200_v");


    bool passtriggerbtagOR = passtriggerbtag1 || passtriggerbtag2 || passtriggerbtag3 || passtriggerbtag4 || passtriggerbtag5 || passtriggerbtag6;


    if(passtriggerbtag1) Fill("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p014_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag2) Fill("VBShadAnalysis/Baseline/HT_C100_DoubleBTagCSV_p026_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag3) Fill("VBShadAnalysis/Baseline/HT_C112_DoubleBTagCSV_p014_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag4) Fill("VBShadAnalysis/Baseline/HT_C112_DoubleBTagCSV_p026_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag5) Fill("VBShadAnalysis/Baseline/HT_QuadPFJet_BTagCSV_Mqq460_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtag6) Fill("VBShadAnalysis/Baseline/HT_QuadPFJet_BTagCSV_Mqq200_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerbtagOR) Fill("VBShadAnalysis/Baseline/HT_btag_OR_" +label, systname, evt_MVV, e->weight() );

    return EVENT_NOT_USED;


    
    //Let's see whether OK to put trigger after all VBS selections
    if (!(category.find("BB")   !=string::npos)) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/HT_Base_" +label, systname, evt_MVV, e->weight() );

    bool passtriggerHad1 = e->IsTriggered("HLT_PFHT900_v");
    bool passtriggerHad2 = e->IsTriggered("HLT_AK8PFJet450_v");
    bool passtriggerHad3 = e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v");
    bool passtriggerHad4 = e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v");
    bool passtriggerHad5 = e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v");
    bool passtriggerHad6 = e->IsTriggered("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v");
    bool passtriggerHadOR = passtriggerHad1 || passtriggerHad2 || passtriggerHad3 || passtriggerHad4 || passtriggerHad5 || passtriggerHad6;

    if(passtriggerHad1) Fill("VBShadAnalysis/Baseline/HT_PFHT900_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad2) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet450_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad3) Fill("VBShadAnalysis/Baseline/HT_AK8PFHT700_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad4) Fill("VBShadAnalysis/Baseline/HT_AK8PFJet360_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad5) Fill("VBShadAnalysis/Baseline/HT_AK8DiPFJet300_200_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHad6) Fill("VBShadAnalysis/Baseline/HT_PFHT650_Wide_" +label, systname, evt_MVV, e->weight() );
    if(passtriggerHadOR) Fill("VBShadAnalysis/Baseline/HT_had_OR_" +label, systname, evt_MVV, e->weight() );

    return EVENT_NOT_USED;

    //Here ends the test of the triggers 
    */

    //////
    //$$$
    //////

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
