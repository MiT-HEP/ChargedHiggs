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
   }
   // Fill Electrons
   // sort leptons!
   // Fill Jets

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
