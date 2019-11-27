#include "interface/AnalysisVBShad.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TPython.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"

#define VERBOSE 0

//#warning Hmumu ANALYSIS NON ISO

void VBShadAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10); 
    l->SetIsoRelCut(0.25);
    l->SetEtaCut(2.4);
    //l->SetTightCut(true);
    //l->SetMediumCut(false);
    l->SetVetoCut();
    l->SetTightCut(false);
    l->SetLooseCut(false);
    l->SetMediumCut(true);
    l->SetTrackerMuonCut(true);
    l->SetGlobalMuonCut(true);
}

void VBShadAnalysis::SetJetCuts(Jet *j) { 
    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100);
}

void VBShadAnalysis::SetFatJetCuts(FatJet *f){
    f->SetEtaCut(2.5);
    f->SetPtCut(200);
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

        //FatJet
        Book ("VBShadAnalysis/Baseline/NFatJet_"+l, "NFatJet; NFatJet; Events", 5,0,5);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_BB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RB_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/pT_FatJet_RR_"+l, "pT_FatJet; pT [GeV]; Events", 120,0,2400.);
        Book ("VBShadAnalysis/Baseline/SDMass_FatJet_"+l, "SDMass_FatJet; SDMass [GeV]; Events", 100,0,200.);
        Book ("VBShadAnalysis/Baseline/Tau21_FatJet_"+l, "Tau21_FatJet; tau21; Events", 50,0,1.0);

        //Jet
        Book ("VBShadAnalysis/Baseline/NJet_"+l, "NJet; FatJet; Events", 10,0,10);
        Book ("VBShadAnalysis/Baseline/pT_Jet_"+l, "pT_Jet; pT [GeV]; Events", 80,0,1600.);

        Book ("VBShadAnalysis/Baseline/NBJet_"+l, "NBJet; NBJet; Events", 5,0,5);

        //MET
        Book ("VBShadAnalysis/Baseline/Met_"+l, "Met; Met [GeV]; Events", 100,0,1000);
        Book ("VBShadAnalysis/Baseline/DphiMETFat_"+l, "Dphi(Met,AK8); #Delta#Phi(Met,AK8) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/Baseline/Dphimin_"+l, "Dphi(Met,AK4); #Delta#Phi(Met,AK4) ; Events", 100,0,6.28);

        // resolved
        Book ("VBShadAnalysis/Baseline/ResBosonMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/BOSON/ZepBosBVar_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_BB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_RB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_BMET_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/BOSON/ZepBosBVar_RMET_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);

        // Search Variable
        Book ("VBShadAnalysis/MVV_"+l, "MVV (unclassified); MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_"+l, "MVVres (unclassified); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_BB_"+l, "MVV (BB) ; MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_BB_"+l, "MVVres (BB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RB_"+l, "MVV (RB); MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_RB_"+l, "MVVres (RB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);

        Book ("VBShadAnalysis/MVV_BMET_"+l, "MTVV (BMET) ; M_{T}(V,MET) [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_BMET_"+l, "MVMETres (BMET); ( M_{T}(V,MET) - MTVV_{gen} ) / MTVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RMET_"+l, "MVV (RMET); M_{T}(V,MET) [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_RMET_"+l, "MVVres (RMET); ( M_{T}(V,MET) - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);

        //
        Book ("VBShadAnalysis/FWJETS/Mjj_BB_"+l, "Mjj (BB); M(j,j) [GeV]; Events", 25,0,2500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_RB_"+l, "Mjj (RB); M(j,j) [GeV]; Events", 25,0,2500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_BMET_"+l, "Mjj (BMET); M(j,j) [GeV]; Events", 25,0,2500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_RMET_"+l, "Mjj (RMET); M(j,j) [GeV]; Events", 25,0,2500.);
        Book ("VBShadAnalysis/FWJETS/Mjj_"+l, "Mjj; Mjj [GeV]; Events", 25,0,2500.);

        Book ("VBShadAnalysis/FWJETS/Dphijj_"+l, "Dphi jj; #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_BB_"+l, "Dphi jj (BB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
        Book ("VBShadAnalysis/FWJETS/Dphijj_RB_"+l, "Dphi jj (RB); #Delta#Phi(j,j) ; Events", 100,0,6.28);
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

        Branch("tree_vbs","varMjj",'F');
        Branch("tree_vbs","varDetajj",'F');
        Branch("tree_vbs","varDphijj",'F');

        Branch("tree_vbs","varMVV",'F');
        Branch("tree_vbs","varDetaVV",'F');
        Branch("tree_vbs","varzepVB",'F');

    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

float VBShadAnalysis::resolvedtagger(Event*e, float MV, string label, string systname) {

    bosonJets.clear();

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

            for(unsigned k=0; k<j; ++k) {
                for(unsigned l=0; l<k; ++l) {

                    if( selectedJets[k]->Eta() * selectedJets[l]->Eta() >=0 ) continue;

                    Mkl = selectedJets[k]->InvMass(selectedJets[l]);

                    double chi2 = norm / (Mkl*Mkl) + (Mij*Mij - MV*MV)/MVres;
                    if(chi2<chi2_) { chi2_=chi2; index_i=i; index_j=j; index_k= k; index_l=l; }

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

    if(bosonJets.size()>1) Fill("VBShadAnalysis/Baseline/ResBosonMass_"+label, systname, Mij, e->weight() );

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
    } else if (label.find("WPhadWPhadJJ") !=string::npos ) {
        pdgID1=24;
        pdgID2=24;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        if(genpar->GetPdgId() == pdgID1) if(genVp==NULL) { genVp = genpar; /*cout << "found W1 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }
        if(genpar->GetPdgId() == pdgID2) if(genVp2==NULL and genpar!=genVp) { genVp2 = genpar; /*cout << "found W2 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }

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
    for(unsigned i=0;i<e->NFatJets() ; ++i)
    {
        FatJet *f=e->GetFatJet(i);

        Double_t minDR=9999;
        if(genVp!=NULL and genVp2!=NULL) {
            Double_t delW = f->DeltaR(*genVp);
            if (delW < minDR){ minDR = delW;}
            Double_t delW2 = f->DeltaR(*genVp2);
            if (delW2 < minDR){ minDR = delW2;}
        }

        if(minDR<0.8) {
            Fill("VBShadAnalysis/Baseline/SDMass_FatJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJet_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            //            if(topology==1) Fill("VBShadAnalysis/Baseline/pT_FatJet_BB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==2) Fill("VBShadAnalysis/Baseline/pT_FatJet_RB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==0) Fill("VBShadAnalysis/Baseline/pT_FatJet_RR_" +label, systname, f->Pt(), e->weight() );
        }

        double dPhiFatMet=fabs(ChargedHiggs::deltaPhi(f->Phi(), e->GetMet().Phi()));

        Fill("VBShadAnalysis/Baseline/DphiMETFat_" +label, systname, dPhiFatMet, e->weight() );

        if(f->IsWJet()) {
            if(doMETAnalysis and dPhiFatMet<0.2) continue;
            selectedFatJets.push_back(f);
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

        Fill("VBShadAnalysis/Baseline/pT_Jet_" +label, systname, j->Pt(), e->weight() );

        if(selectedFatJets.size()>0) {
            if( j->DeltaR(selectedFatJets[0]) < 1.2 ) continue;
        }
        if(selectedFatJets.size()>1) {
            if( j->DeltaR(selectedFatJets[1]) < 1.2 ) continue;
        }

        counter++;
        double dphi = fabs(ChargedHiggs::deltaPhi(j->Phi(), e->GetMet().Phi()));

        if(dphi < minDPhi) minDPhi = dphi;

        if(counter<5) Fill("VBShadAnalysis/Baseline/Dphimin_" +label, systname, dphi, e->weight() );

        /*
        for (auto const& fat : selectedFatJets) {
            if( j->DeltaR(fat) < 1.2 ) continue;
        }
        */
        //        if( j->DeltaR(genWp) < 1.2 ) continue;
        //        if( j->DeltaR(genWp2) < 1.2 ) continue;

        if(doMETAnalysis and minDPhi<0.2 and counter<5) continue;
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

    SetTreeVar("met_pt",e->GetMet().Pt());
    SetTreeVar("met_phi",e->GetMet().Phi());

    //$$$$

    if (category.find("BB")   !=string::npos) SetTreeVar("ana_category",1);
    if (category.find("RB")   !=string::npos) SetTreeVar("ana_category",2);
    if (category.find("BMET") !=string::npos) SetTreeVar("ana_category",3);
    if (category.find("RMET") !=string::npos) SetTreeVar("ana_category",4);

    int mc=0;
    if(label.find("WPhadWPhadJJ") !=string::npos ) mc = 1 ;
    if(label.find("ZbbZhadJJ") !=string::npos ) mc = 2 ;
    if(label.find("ZnnZhadJJ") !=string::npos ) mc = 3 ;

    if(label.find("QCD_HT") !=string::npos) mc =500 ;

    SetTreeVar("mc",mc);

    // $$$$ analysis variable
    SetTreeVar("varMjj",evt_Mjj);
    SetTreeVar("varDetajj",evt_Detajj);
    SetTreeVar("varDphijj",evt_Dphijj);
    SetTreeVar("varMVV",evt_MVV);
    SetTreeVar("varDetaVV",evt_DetaVV);
    SetTreeVar("varzepVB",evt_zepVB);

}


int VBShadAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );

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
    if(doMETAnalysis) {
        bool passtriggerMET = (e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        //    bool passtriggerMET = (e->IsTriggered("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v") || e->IsTriggered("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight") || e->IsTriggered("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"));
        if(!passtriggerMET) return EVENT_NOT_USED;
    }

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 1, e->weight() );  //1--trigger

    // kill Top/W/Z
    if ( e->Nleps() > 0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 2, e->weight() );  //2--lep veto

    // events with MET in separate category
    if ( doMETAnalysis and e->GetMet().Pt() < 200 ) return EVENT_NOT_USED;
    if ( !doMETAnalysis and e->GetMet().Pt() > 200 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Baseline/Met_" +label, systname, e->GetMet().Pt(), e->weight() );
    Fill("VBShadAnalysis/Cutflow_" +label, systname, 3, e->weight() );  //3--veto MET

    // events with B in another category
    // withMET
    // noMET with B
    // noMET noB
    if ( doHADAnalysis and e->Bjets() > 0 ) return EVENT_NOT_USED;
    if ( doBAnalysis and e->Bjets() == 0 ) return EVENT_NOT_USED;

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

    //    if(doMETAnalysis) BMET, RMET
    //    if(!doMETAnalysis) BB, RB

    if(!doMETAnalysis) {
        forwardJets.clear();

        if(selectedFatJets.size()>1 and selectedJets.size()>1) {
            category="_BB";
            evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);
            evt_DetaVV = fabs(selectedFatJets[0]->DeltaEta(selectedFatJets[1]));
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                forwardJets.push_back(selectedJets[iter]);
            }
        }

        double mBoson=80.;

        if(selectedFatJets.size()==1 and selectedJets.size()>3) {
            double MV = resolvedtagger(e, mBoson, label, systname);
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1) {
                category="_RB";
                evt_MVV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M();
                evt_DetaVV = fabs(selectedFatJets[0]->GetP4().Eta() - (bosonJets[1]->GetP4() + bosonJets[0]->GetP4()).Eta());
            }
        }
    }

    if(doMETAnalysis) {
        forwardJets.clear();

        if(selectedFatJets.size()>0 and selectedJets.size()>1) {
            category="_BMET";
            evt_MVV = ChargedHiggs::mt(selectedFatJets[0]->Pt(), e->GetMet().Pt(), selectedFatJets[0]->Phi(), e->GetMet().Phi());
            for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
                forwardJets.push_back(selectedJets[iter]);
            }
        }
        double mBoson=90.;
        if(selectedFatJets.size()==0 and selectedJets.size()>3) {
            double MV = resolvedtagger(e, mBoson, label, systname);
            if(MV>(mBoson-20) and MV<(mBoson+20) and bosonJets.size()>1) {
                category="_RMET";
                evt_MVV = ChargedHiggs::mt((bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Pt(), e->GetMet().Pt(),
                                           (bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).Phi(), e->GetMet().Phi());
            }
        }
    }


    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );

    // CHECK THIS ?? 1050 ?? move to 500 for now
    // resolved likely all below M=500
    if( evt_MVV < 500 ) return EVENT_NOT_USED;

    if(evt_MVV_gen!=0) Fill("VBShadAnalysis/MVVres" +category+"_"+label, systname, (evt_MVV-evt_MVV_gen)/evt_MVV_gen, e->weight() );

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 6, e->weight() );  //InvMFatjet cut

    Fill("VBShadAnalysis/Baseline/NJet_" +label, systname, forwardJets.size(), e->weight() );

    if( forwardJets.size() < 2 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 7, e->weight() );  //NJet cut

    if( forwardJets[0]->Eta() * forwardJets[1]->Eta() >=0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 8, e->weight() );  //Jet seperate cut

    evt_Detajj = fabs(forwardJets[0]->DeltaEta(forwardJets[1]));

    // CHECK THIS ?? 3 ?? move to 2.5 for now
    if( evt_Detajj < 2.5 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 9, e->weight() );  //Jet separate cut

    evt_Mjj= forwardJets[0]->InvMass(forwardJets[1]);
    evt_Dphijj = ChargedHiggs::deltaPhi(forwardJets[0]->Phi(), forwardJets[1]->Phi());

    Fill("VBShadAnalysis/FWJETS/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/FWJETS/Dphijj" +category+"_"+label, systname, evt_Dphijj, e->weight() );

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 10, e->weight() ); //InvMjet cut

    if(selectedFatJets.size()>0) {
        evt_zepVB = (selectedFatJets[0]->Rapidity() - (forwardJets[0]->Eta()+forwardJets[1]->Eta())/2)/evt_Detajj;
        Fill("VBShadAnalysis/BOSON/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );
    }

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
