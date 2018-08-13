#include "interface/LoadNano.hpp"
#include "interface/Event.hpp"

int LoadNano::InitTree(){

    tree_->SetBranchAddress("run",&run);
    tree_->SetBranchAddress("luminosityBlock",&lumi);
    tree_->SetBranchAddress("event",&event);
    tree_->SetBranchAddress("fixedGridRhoFasdGridRhoFastjetAll",&rho);
    tree_->SetBranchAddress("PV_npvs",&npv);
    //
    tree_->SetBranchAddress("MET_pt",&met_pt);
    tree_->SetBranchAddress("MET_phi",&met_phi);
    // muon
    tree_->SetBranchAddress("nMuon",&nMuon);
    tree_->SetBranchAddress("Muon_eta",&Muon_eta);
    tree_->SetBranchAddress("Muon_pt",&Muon_pt);
    tree_->SetBranchAddress("Muon_phi",&Muon_phi);
    tree_->SetBranchAddress("Muon_mass",&Muon_mass);
    tree_->SetBranchAddress("Muon_charge",&Muon_charge);
    tree_->SetBranchAddress("Muon_nTrackerLayers",&Muon_nTrackerLayers);
    tree_->SetBranchAddress("Muon_miniPFRelIso_all",&Muon_miniPFRelIso_all);
    tree_->SetBranchAddress("Muon_pfRelIso04_all",&Muon_pfRelIso04_all);
    tree_->SetBranchAddress("Muon_mediumId",&Muon_mediumId);
    tree_->SetBranchAddress("Muon_tightId",&Muon_tightId);
    tree_->SetBranchAddress("Muon_isPFcand",&Muon_isPFcand);


    //
    tree_->SetBranchAddress("nJet",&nJet);
    tree_->SetBranchAddress("Jet_pt",&Jet_pt);
    tree_->SetBranchAddress("Jet_eta",&Jet_eta);
    tree_->SetBranchAddress("Jet_phi",&Jet_phi);
    tree_->SetBranchAddress("Jet_mass",&Jet_mass);
    tree_->SetBranchAddress("Jet_chEmEF",&Jet_chEmEF);
    tree_->SetBranchAddress("Jet_chHEF",&Jet_chHEF);
    tree_->SetBranchAddress("Jet_neEmEF",&Jet_neEmEF);
    tree_->SetBranchAddress("Jet_neHEF",&Jet_neHEF);
    tree_->SetBranchAddress("Jet_qgl",&Jet_qgl);
    tree_->SetBranchAddress("Jet_bReg",&Jet_bReg);
    tree_->SetBranchAddress("Jet_puId",&Jet_puId);
    tree_->SetBranchAddress("Jet_jetId",&Jet_jetId);
    tree_->SetBranchAddress("Jet_btagDeepB",&Jet_btagDeepB);
    tree_->SetBranchAddress("Jet_btagCSVV2",&Jet_btagCSVV2);

}

int LoadNano::FillEvent(){

   // Fill Met
   TLorentzVector met; met.SetPtEtaPhiM(met_pt,0,met_phi,0);
   event_ -> met_ .SetP4(met);

   // Fill Muon
   for(int i=0;i<nMuon;++i)
   {
        //if (not Muon_isPFcand[i] ) continue; // loose
        Lepton *l =new Lepton();
        TLorentzVector p4;
        p4.SetPtEtaPhiM(Muon_pt[i],Muon_eta[i],Muon_phi[i],Muon_mass[i]);
        l->SetP4(p4);
        l->SetType(13);
        l->SetMiniIso(Muon_miniPFRelIso_all[i]);
        l->SetIso(Muon_pfRelIso04_all[i]);
        l->SetCharge( Muon_charge[i]);
        l->SetMediumId( Muon_mediumId[i]);
        l->SetLooseId( Muon_isPFcand[i]);
        l->SetNLayers(Muon_nTrackerLayers[i]);

        // REMEMBER TO CLEN THE LEPTONS IFF NOT USED
        event_ -> leps_ . push_back(l);
   }
   // Fill Electrons. TODO
   // sort leptons
   sort(event_ -> leps_.begin(),event_ -> leps_.end(),[](Lepton*l1, Lepton*l2){ 
           if (l1->GetP4().Pt() > l2->GetP4().Pt()) return true;  // sorting by grt Pt
           if (l1->GetP4().Pt() < l2->GetP4().Pt()) return false;
           if (l1->GetP4().Eta() > l2->GetP4().Eta()) return true; 
           if (l1->GetP4().Eta() < l2->GetP4().Eta()) return false;
           return l1->GetP4().Phi() > l2->GetP4().Phi(); // if pt, eta, phi are identical, then they are the same object
           } );
   // Fill Jets
   for(int i=0;i<nJet;++i)
   {
        bool id =  Jet_jetId[i];
        if (not id) continue;
        Jet *j = new Jet() ;
        TLorentzVector p4;
        p4.SetPtEtaPhiM(Jet_pt[i],Jet_eta[i],Jet_phi[i],Jet_mass[i]);

        j->SetNEMF(Jet_neEmEF[i] );
        j->SetCEMF(Jet_chEmEF[i] );

        // TODO-- JES
        //j->SetValueUp  (Smearer::JES , (1. + bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetValueDown(Smearer::JES , (1. - bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetFilled(Smearer::JES);
        //
        //TODO -- JER
        j->bdiscr = Jet_btagCSVV2[i];
        j->SetDeepB ( Jet_btagDeepB[i] );
        j->SetQGL(Jet_qgl[i]);
        j->SetPuId(Jet_puId[i]);
#warning MISSING NANO_JET INFO
        j->SetBCorrection(Jet_bReg[i], 0.1); //TODO FIXME
        j->pdgId = 0 ; //FIXME
        j->motherPdgId = 0; // FIXME
        j->grMotherPdgId =  0 ;// FIXME
        j->SetHadFlavor(0); //FIXME
        event_ -> jets_ . push_back(j);

   }

}

void LoadNano::NewFile(){
    // read some how the triggers -> this is for Nero, probably not necessary.
    //TNamed * tn=  (TNamed*)tree_->GetFile()->Get("nero/triggerNames");
    //string fname = tree_->GetFile()->GetName();
    return;
}

void LoadNano::FillEventInfo(){
   event_->runNum_ = run ;
   event_->lumiNum_ = lumi;
   event_->eventNum_ = event;

   event_->isRealData_ = (run >10); //FIXME
   event_->rho_ = rho;
   event_->npv_ = npv;
}


void LoadNano::Clear(){
    run=-1;
    lumi=-1;
    event=-1;
}

REGISTER(LoadNano);
