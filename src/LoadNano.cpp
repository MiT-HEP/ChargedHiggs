#include "interface/LoadNano.hpp"
#include "interface/Event.hpp"
#include "interface/nano.hpp"

int LoadNano::InitTree(){

    //tree_->SetBranchAddress("run",&run);
    nano.reset(new Nano(tree_)); // call nano::Init -> SetBranchAddress
}

int LoadNano::FillEvent(){

   // Fill Met
   //TLorentzVector met; met.SetPtEtaPhiM(met_pt,0,met_phi,0);
   //event_ -> met_ .SetP4(met);

   // Fill Muon
   //for(int i=0;i<nMuon;++i)
   //{
   //     //if (not Muon_isPFcand[i] ) continue; // loose
   //     Lepton *l =new Lepton();
   //     TLorentzVector p4;
   //     p4.SetPtEtaPhiM(Muon_pt[i],Muon_eta[i],Muon_phi[i],Muon_mass[i]);
   //     l->SetP4(p4);
   //     l->SetType(13);
   //     l->SetMiniIso(Muon_miniPFRelIso_all[i]);
   //     l->SetIso(Muon_pfRelIso04_all[i]);
   //     l->SetCharge( Muon_charge[i]);
   //     l->SetMediumId( Muon_mediumId[i]);
   //     l->SetLooseId( Muon_isPFcand[i]);
   //     l->SetNLayers(Muon_nTrackerLayers[i]);

   //     // REMEMBER TO CLEN THE LEPTONS IFF NOT USED
   //     event_ -> leps_ . push_back(l);
   //}
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
   for(int i=0;i<nano->nJet;++i)
   {
        bool id =  nano->Jet_jetId[i];
        if (not id) continue;
        Jet *j = new Jet() ;
        TLorentzVector p4;
        p4.SetPtEtaPhiM(nano->Jet_pt[i],nano->Jet_eta[i],nano->Jet_phi[i],nano->Jet_mass[i]);

        j->SetNEMF(nano->Jet_neEmEF[i] );
        j->SetCEMF(nano->Jet_chEmEF[i] );

        // TODO-- JES
        //j->SetValueUp  (Smearer::JES , (1. + bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetValueDown(Smearer::JES , (1. - bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetFilled(Smearer::JES);
        //
        //TODO -- JER
        j->bdiscr = nano->Jet_btagCSVV2[i];
        j->SetDeepB ( nano->Jet_btagDeepB[i] );
        j->SetQGL(nano->Jet_qgl[i]);
        j->SetPuId(nano->Jet_puId[i]);
        j->SetBCorrection(nano->Jet_bRegCorr[i], nano->Jet_bRegRes[i]); //TODO FIXME
#warning MISSING NANO_JET INFO
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
   event_->runNum_ = nano->run ;
   event_->lumiNum_ = nano->luminosityBlock;
   event_->eventNum_ = nano->event;

   event_->isRealData_ = (nano->run >10); //FIXME
   event_->rho_ = nano->fixedGridRhoFastjetAll;
   event_->npv_ = nano->PV_npvsGood;
}


void LoadNano::Clear(){
    // TODO
    //run=-1;
    //lumi=-1;
    //event=-1;
}

REGISTER(LoadNano);
