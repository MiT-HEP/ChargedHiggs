#define DEEP_B_LOOSE 0.1522
#define DEEP_B_MEDIUM 0.4941
#define DEEP_B_TIGHT 0.8001

#include "interface/AnalysisEfficiencies.hpp"
void EfficienciesAnalysis::SetLeptonCuts(Lepton *l){ 
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

void EfficienciesAnalysis::SetJetCuts(Jet *j) { 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100);
    // medium CSV
    //j->SetBCut(0.8484); //L=0.5803 , M=  0.8838, T=9693
    //j->SetDeepBCut(-100); // L = 0.1522, M=0.4941, T=0.8001
    j->SetBCut(-100); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetDeepBCut(DEEP_B_MEDIUM); 
}

void EfficienciesAnalysis::SetTauCuts(Tau *t){ 
    t->SetIsoCut(2.5); 
    t->SetEtaCut(2.1); 
    t->SetPtCut(8000);  // remove taus!!! 
    t->SetMuRej(true); 
    t->SetEleRej(true);
    t->SetTrackPtCut(30.);
    t->SetProngsCut(1); 
    t->SetDecayMode(1);
}

void EfficienciesAnalysis::SetPhotonCuts(Photon *p){
    p->SetIsoCut(-1); 
    p->SetPtCut(8000);
}

void EfficienciesAnalysis::EndEvent(){}

void EfficienciesAnalysis::Init(){
    double ptbins[]={0,10,20,50,8000};
    int nbins=sizeof(ptbins)/sizeof(double)-1;
    //double ebins[]={2.4, -2.1, -1.6, -1.2, -0.9, -0.3, -0.2, 0.2, 0.3, 0.9, 1.2, 1.6, 2.1, 2.4};
    double ebins[]={0,0.5,1.0,1.5,2.0,2.4};
    int nebins=sizeof(ebins)/sizeof(double)-1;

    for ( string l : AllLabel()  ) {
        //Book ("EfficienciesAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
        for( string type:{"","_miniiso"}){
            for ( string what:{"pass","fail"})
            {
                Book ("EfficienciesAnalysis/Vars/"+what+type+"_pt2nd_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nbins,ptbins);
                Book ("EfficienciesAnalysis/Vars/"+what+type+"_pt1st_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nbins,ptbins);

                Book ("EfficienciesAnalysis/Vars/"+what+type+"_eta2nd_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nebins,ebins);
                Book ("EfficienciesAnalysis/Vars/"+what+type+"_eta1st_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nebins,ebins);

                Book2D ("EfficienciesAnalysis/Vars/"+what+type+"_pt1st_eta1st_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nbins,ptbins,nebins,ebins);
                Book2D ("EfficienciesAnalysis/Vars/"+what+type+"_pt2nd_eta2nd_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", nbins,ptbins,nebins,ebins);
            }
        }
    }

}


int EfficienciesAnalysis::analyze(Event *event, string systname)
{
    bool refTrigger = true; // Run On HLT ZeroBias or JetHT
    string label = GetLabel(e);
    //if (label.find("SingleMuon") !=string::npos) {isSingleMuon=true; label="Data";} else { isSingleMuon=false;}
    //if (label.find("DoubleMuon") !=string::npos) {isDoubleMuon=true; label="Data";} else {isDoubleMuon=false;}
    // search for muons
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );
    //
    mu0 = e->GetMuon(0);
    mu1 = e->GetMuon(1);
    int il2=1;
    while (mu0 != NULL and mu1!= NULL and mu1->Charge() * mu0->Charge() != -1) { mu1= e->GetMuon(++il2);} // fully combinatorics on the second muon
    bool recoMuons= mu0 != NULL and mu1 !=NULL; 
    if (recoMuons and mu0->Charge() * mu1->Charge() != -1 ) recoMuons=false; // 

    if (recoMuons)
    {
        TLorentzVector zero(0,0,0,0);
        Hmm.SetP4(zero); // make sure it is 0.
        Hmm += *mu0;
        Hmm += *mu1;
    }

    /************************************
     *** Compute MiniIsolation muons  ***
     ************************************/
    mu0_mini=NULL;
    mu1_mini=NULL;
    miniIsoLeptons.clear();
    for(unsigned il=0 ; ;++il)
    {
        Lepton *l = e->GetBareLepton(il);
        if (l == NULL) break;  // exit strategy

        // selection
        if (l->Pt() <10 ) continue;
        //l->SetIsoRelCut(0.25);
        if (abs(l->Eta()) >2.4) continue;
        //medium id
        if( not l->GetMediumId() ) continue;

        //MINI-ISO
        if( l->MiniIsolation() >0.4 ) continue;//loose

        // for muons require tracker and global
        if (l->IsMuonDirty() and not l->GetTrackerMuon())  continue;
        if (l->IsMuonDirty() and not l->GetGlobalMuon())  continue;

        // selected leptons
        miniIsoLeptons.push_back(l);
    }
    // sort miniIsoLeptons by pt
    std::sort(miniIsoLeptons.begin(),miniIsoLeptons.end(),[](Lepton const *a, Lepton const *b ){return a->Pt() > b->Pt();});

    for( auto& l : miniIsoLeptons)
    {
        if (mu0_mini == NULL and l->IsMuonDirty() ) { mu0_mini = l;}
        if (mu1_mini == NULL and (mu0_mini != l ) and l->IsMuonDirty() and mu0_mini->Charge()*l->Charge() == -1)  mu1_mini = l;
    }

    bool  passAsymmPtCuts= (recoMuons and mu0->Pt() >26 and mu1->Pt() >20);
    bool passTrigger1{false}, passTrigger2{false}, passTrigger{false};

    if (recoMuons) // normal flow
    {
        // ISO MU 27 is Fully Efficient
        passTrigger1 = (e->IsTriggered("HLT_IsoMu27_v",mu0) or e->IsTriggered("HLT_IsoTkMu27_v",mu0)) ;
        passTrigger2 = (e->IsTriggered("HLT_IsoMu27_v",mu1) or e->IsTriggered("HLT_IsoTkMu27_v",mu1)) ;

        // Prescale at some point
        passTrigger1 = passTrigger1 || (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;
        passTrigger2 = passTrigger2 || (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
        passTrigger = passTrigger1 || passTrigger2;
        if (fabs(mu0->Eta())<2.1 and fabs(mu1->Eta())<2.1){
            passTrigger = passTrigger|| e->IsTriggered("HLT_DoubleIsoMu20_eta2p1_v");
        }

        // 
        if (Hmm.M() >70 and Hmm.M() <110)
        {
            // Fill passtrigger and fail Trigger
            string what= (passTrigger)?"pass":"fail";
            Fill("EfficienciesAnalysis/Vars/"+what+"_pt2nd_"+ label,systname,mu1->Pt(),e->weight());
            Fill("EfficienciesAnalysis/Vars/"+what+"_pt1st_"+ label,systname,mu0->Pt(),e->weight());

            Fill("EfficienciesAnalysis/Vars/"+what+"_eta2nd_"+ label,systname,mu1->Eta(),e->weight());
            Fill("EfficienciesAnalysis/Vars/"+what+"_eta1st_"+ label,systname,mu0->Eta(),e->weight());

            Fill2D("EfficienciesAnalysis/Vars/"+what+"_pt1st_eta1st_"+ label,systname,mu1->Pt(),mu1->Eta(),e->weight());
            Fill2D("EfficienciesAnalysis/Vars/"+what+"_pt2nd_eta2nd_"+ label,systname,mu0->Pt(),mu0->Eta(),e->weight());
            
        }
    }

    passTrigger=false; passTrigger1=false; passTrigger2=false;
    if (mu0_mini != NULL and mu1_mini != NULL)
    {
        passTrigger1 = (e->IsTriggered("HLT_IsoMu27_v",mu0_mini) or e->IsTriggered("HLT_IsoTkMu27_v",mu0_mini)) ;
        passTrigger2 = (e->IsTriggered("HLT_IsoMu27_v",mu1_mini) or e->IsTriggered("HLT_IsoTkMu27_v",mu1_mini)) ;

        // Prescale at some point
        passTrigger1 = passTrigger1 || (e->IsTriggered("HLT_IsoMu24_v",mu0_mini) or e->IsTriggered("HLT_IsoTkMu24_v",mu0_mini)) ;
        passTrigger2 = passTrigger2 || (e->IsTriggered("HLT_IsoMu24_v",mu1_mini) or e->IsTriggered("HLT_IsoTkMu24_v",mu1_mini)) ;
        passTrigger = passTrigger1 || passTrigger2;
        if (fabs(mu0_mini->Eta())<2.1 and fabs(mu1_mini->Eta())<2.1){
            passTrigger = passTrigger|| e->IsTriggered("HLT_DoubleIsoMu20_eta2p1_v");
        }

        TLorentzVector zero(0,0,0,0);
        Hmm.SetP4(zero); // make sure it is 0.
        Hmm += *mu0_mini;
        Hmm += *mu1_mini;

        if (Hmm.M() >70 and Hmm.M() <110)
        {
            // Fill passtrigger and fail Trigger
            string what= (passTrigger)?"pass":"fail";
            Fill("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_pt2nd_"+ label,systname,mu1_mini->Pt(),e->weight());
            Fill("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_pt1st_"+ label,systname,mu0_mini->Pt(),e->weight());

            Fill("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_eta2nd_"+ label,systname,mu1_mini->Eta(),e->weight());
            Fill("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_eta1st_"+ label,systname,mu0_mini->Eta(),e->weight());

            Fill2D("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_pt1st_eta1st_"+ label,systname,mu1_mini->Pt(),mu1_mini->Eta(),e->weight());
            Fill2D("EfficienciesAnalysis/Vars/"+what+"_miniiso"+"_pt2nd_eta2nd_"+ label,systname,mu0_mini->Pt(),mu0_mini->Eta(),e->weight());

        }

    }

    return 0;

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
