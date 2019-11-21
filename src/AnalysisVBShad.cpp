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

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
	    Book ("VBShadAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);

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

        // resolved
        Book ("VBShadAnalysis/Baseline/ResBosonMass_"+l, "ResBosonMass; V(i,j) [GeV]; Events", 100,0,200.);

        Book ("VBShadAnalysis/ZepBosBVar_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj; Events", 250,0,2);
        Book ("VBShadAnalysis/ZepBosBVar_BB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);
        Book ("VBShadAnalysis/ZepBosBVar_RB_"+l, " ; |Y_{V} - (eta_{j1} + eta_{j2})/2| / Detajj ; Events", 250,0,2);

        // Search Variavle
        Book ("VBShadAnalysis/MVV_"+l, "MVV (unclassified); MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_"+l, "MVVres (unclassified); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_BB_"+l, "MVV (BB) ; MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_BB_"+l, "MVVres (BB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        Book ("VBShadAnalysis/MVV_RB_"+l, "MVV (RB); MVV [GeV]; Events", 250,0,2500);
        Book ("VBShadAnalysis/MVVres_RB_"+l, "MVVres (RB); ( MVV_{reco} - MVV_{gen} ) / MVV_{gen}; Events", 100,-5.,5.);
        //
        Book ("VBShadAnalysis/Mjj_BB_"+l, "Mjj (BB); Mjj [GeV]; Events", 250,0,2500.);
        Book ("VBShadAnalysis/Mjj_RB_"+l, "Mjj (RB); Mjj [GeV]; Events", 250,0,2500.);
        Book ("VBShadAnalysis/Mjj_"+l, "Mjj; Mjj [GeV]; Events", 250,0,2500.);


    } //end label loop

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

double VBShadAnalysis::resolvedtagger(Event*e, double Mw, string label, string systname) {

    float const norm = 1000*1000; // 1TeV^2
    float const Mwres = 20*20; // 20 GeV

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

                    double chi2 = norm / (Mkl*Mkl) + (Mij*Mij - Mw*Mw)/Mwres;
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


void VBShadAnalysis::genStudies(Event*e, GenParticle* & genWp, GenParticle* & genWp2 )
{

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        if(genpar->GetPdgId() == 24) if(genWp==NULL) { genWp = genpar; /*cout << "found W1 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }
        if(genpar->GetPdgId() == 24) if(genWp2==NULL and genpar!=genWp) { genWp2 = genpar; /*cout << "found W2 pt= "<< genpar->Pt() << " eta=" << genpar->Eta()  << endl;*/ }

    }

    int topology=0;
    if(genWp!=NULL and genWp2!=NULL) {
        if(genWp->Pt() > 200 and genWp2->Pt() > 200) topology=1;
        if((genWp->Pt() > 200 and genWp2->Pt() < 200) || (genWp->Pt() < 200 and genWp2->Pt() > 200) ) topology=2;
    }

    // at least one boosted
    //    if(genWp==NULL or genWp2==NULL) return EVENT_NOT_USED;
    //    if(genWp->Pt() < 200 and genWp2->Pt() < 200) return EVENT_NOT_USED;

    evt_MVV_gen = (genWp->GetP4()+genWp2->GetP4()).M();

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

int VBShadAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );


    GenParticle *genWp = NULL;
    GenParticle *genWp2 = NULL;

    genStudies(e,genWp,genWp2);

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build cutflow
    //$$$$$$$$$
    //$$$$$$$$$

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 0, e->weight() );

    // TRIGGER STORY

    bool passtrigger = (e->IsTriggered("HLT_PFHT_800_v") || e->IsTriggered("HLT_AK8PFJet360_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v"));
    //    if(!passtrigger) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 1, e->weight() );  //1--trigger

    // kill Top/W/Z 
    if ( e->Nleps() > 0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 2, e->weight() );  //2--lep veto

    // events with MET in another category
    if ( e->GetMet().Pt() > 200 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 3, e->weight() );  //3--veto MET

    // events with B in another category   
    if ( e->Bjets() > 0 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 4, e->weight() );  //4--veto b

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build fatJets and jets
    //$$$$$$$$$
    //$$$$$$$$$

    //AK8 jet
    selectedFatJets.clear();
    for(unsigned i=0;i<e->NFatJets() ; ++i)
    {
        FatJet *f=e->GetFatJet(i);

        Double_t minDR=9999;
        if(genWp!=NULL and genWp2!=NULL) {
            Double_t delW = f->DeltaR(*genWp);
            if (delW < minDR){ minDR = delW;}
            Double_t delW2 = f->DeltaR(*genWp2);
            if (delW2 < minDR){ minDR = delW2;}
        }

        if(minDR<0.8) {
            Fill("VBShadAnalysis/Baseline/SDMass_FatJet_" +label, systname, f->SDMass(), e->weight() );
            Fill("VBShadAnalysis/Baseline/Tau21_FatJet_" +label, systname, f->Tau2()/f->Tau1(), e->weight() );
            //            if(topology==1) Fill("VBShadAnalysis/Baseline/pT_FatJet_BB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==2) Fill("VBShadAnalysis/Baseline/pT_FatJet_RB_" +label, systname, f->Pt(), e->weight() );
            //            if(topology==0) Fill("VBShadAnalysis/Baseline/pT_FatJet_RR_" +label, systname, f->Pt(), e->weight() );
            Fill("VBShadAnalysis/Baseline/pT_FatJet_" +label, systname, f->Pt(), e->weight() );
        }

        if(f->IsWJet()) selectedFatJets.push_back(f);

    }

    Fill("VBShadAnalysis/Baseline/NFatJet_" +label, systname, selectedFatJets.size(), e->weight() );

    if(selectedFatJets.size()<1) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 5, e->weight() );  //NFatjet cut

    //AK4
    selectedJets.clear();
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);

        Fill("VBShadAnalysis/Baseline/pT_Jet_" +label, systname, j->Pt(), e->weight() );

        if( j->DeltaR(selectedFatJets[0]) < 1.2 ) continue;
        if(selectedFatJets.size()>1) {
            if( j->DeltaR(selectedFatJets[1]) < 1.2 ) continue;
        }
        //        if( j->DeltaR(genWp) < 1.2 ) continue;
        //        if( j->DeltaR(genWp2) < 1.2 ) continue;

        selectedJets.push_back(j);
    }

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$ Build categories below
    //$$$$$$$$$
    //$$$$$$$$$

    forwardJets.clear();

    string category="";
    if(selectedFatJets.size()>1 and selectedJets.size()>1) {
        category="_BB";
        evt_MVV = selectedFatJets[0]->InvMass(selectedFatJets[1]);

        for(unsigned iter=0; iter<selectedJets.size(); ++iter) {
            forwardJets.push_back(selectedJets[iter]);
        }
    }

    bosonJets.clear();
    if(selectedFatJets.size()==1 and selectedJets.size()>3) {
        double MV = resolvedtagger(e, 80, label, systname);
        if(MV>(80-20) and MV<(80+20) and bosonJets.size()>1) { category="_RB"; evt_MVV = ( selectedFatJets[0]->GetP4() + bosonJets[0]->GetP4() + bosonJets[1]->GetP4()).M(); }
    }

    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$
    //$$$$$$$$$

    // CHECK THIS ?? 1050 ?? move to 500 for now
    if( evt_MVV < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/MVV" +category+"_"+label, systname, evt_MVV, e->weight() );
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

    Fill("VBShadAnalysis/Mjj" +category+"_"+label, systname, evt_Mjj, e->weight() );

    if( evt_Mjj < 500 ) return EVENT_NOT_USED;

    Fill("VBShadAnalysis/Cutflow_" +label, systname, 10, e->weight() ); //InvMjet cut

    evt_zepVB = (selectedFatJets[0]->Rapidity() - (forwardJets[0]->Eta()+forwardJets[1]->Eta())/2)/evt_Detajj;

    Fill("VBShadAnalysis/ZepBosBVar" +category+"_"+label, systname, evt_zepVB, e->weight() );

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
