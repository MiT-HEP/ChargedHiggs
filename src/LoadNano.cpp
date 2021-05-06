#include "interface/LoadNano.hpp"
#include "interface/Event.hpp"
#include "interface/nanov8.hpp"

#include "TTreeCache.h"
#include "TLeaf.h"

// flags enum
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
//#define VERBOSE 1

void LoadNano::SetYear(int y)
{
    Loader::SetYear(y); 
    if(nano) nano->year=y;
}

void LoadNano::SetData(bool x)
{
    Loader::SetData(x); 
    if(nano) nano->data=x;
}

int LoadNano::InitTree(){
    if (tree_ == nullptr) {
        return 0;
    }
    //tree_->SetBranchAddress("run",&run);
    nano.reset(new nanov8(tree_,year,isData)); // call nano::Init -> SetBranchAddress
    
    // cache settings for AAA opening    
    unsigned long cachesize = 3145728U ;   //30 MBytes
    TTreeCache::SetLearnEntries(5);  //<<< we can take the decision after 5 entry
    tree_->SetCacheSize(cachesize);
    tree_->AddBranchToCache("*", true);
    tree_->AddBranchToCache("L1_*", false);
    return 0;
}

int LoadNano::FillEvent(){

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Begin");
#endif

   // Fill Met
    {
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Met");
#endif
        TLorentzVector met; met.SetPtEtaPhiM(nano->MET_pt,0,nano->MET_phi,0);
        event_ -> met_ .SetP4(met);
        event_->met_.SetSignificance(nano->MET_significance);

        TLorentzVector met_delta; met_delta.SetXYZT(nano->MET_MetUnclustEnUpDeltaX,nano->MET_MetUnclustEnUpDeltaY,0,0);
        event_ -> met_ . SetValueUp  (Smearer::UNCLUSTER , (met+met_delta).Pt() );
        event_ -> met_ . SetValueDown(Smearer::UNCLUSTER , (met-met_delta).Pt() );
        event_-> met_ . SetFilled(Smearer::UNCLUSTER);

        event_->met_.SetGenPt(nano->MET_pt);
        event_->met_.SetGenPhi(nano->MET_phi);

#warning Check met filter recommendation
        //event_->met_->filterbadPFMuon =  // now everything in full recommendation
        //event_->met_->filterbadChHadrons = 
        event_->met_ . setFullRecommendation(nano->Flag_METFilters);//??
        //if (year == 2016){
        //event_->met_. setFullRecommendation(
        //            nano->Flag_HBHENoiseFilter and
        //            nano->Flag_HBHENoiseIsoFilter and
        //            //nano->Flag_CSCTightHaloFilter and
        //            //nano->Flag_CSCTightHaloTrkMuUnvetoFilter and
        //            //nano->Flag_CSCTightHalo2015Filter and
        //            //nano->Flag_globalTightHalo2016Filter and
        //            nano->Flag_globalSuperTightHalo2016Filter and
        //            //nano->Flag_HcalStripHaloFilter and
        //            //nano->Flag_hcalLaserEventFilter and
        //            nano->Flag_EcalDeadCellTriggerPrimitiveFilter and
        //            //nano->Flag_EcalDeadCellBoundaryEnergyFilter and
        //            //nano->Flag_ecalBadCalibFilter and
        //            nano->Flag_goodVertices and
        //            //nano->Flag_eeBadScFilter and
        //            //nano->Flag_ecalLaserCorrFilter and
        //            //nano->Flag_trkPOGFilters and
        //            //nano->Flag_chargedHadronTrackResolutionFilter and
        //            //nano->Flag_muonBadTrackFilter and
        //            //nano->Flag_BadChargedCandidateFilter and
        //            nano->Flag_BadPFMuonFilter 
        //            //nano->Flag_BadChargedCandidateSummer16Filter and
        //            //nano->Flag_BadPFMuonSummer16Filter and
        //            //nano->Flag_trkPOG_manystripclus53X and
        //            //nano->Flag_trkPOG_toomanystripclus53X and
        //            //nano->Flag_trkPOG_logErrorTooManyClusters and
        //            //nano->Flag_METFilters
        //        );
        //}
        //if (year == 2017){
        //event_->met_. setFullRecommendation(
        //            nano->Flag_HBHENoiseFilter and
        //            nano->Flag_HBHENoiseIsoFilter and
        //            //nano->Flag_CSCTightHaloFilter and
        //            //nano->Flag_CSCTightHaloTrkMuUnvetoFilter and
        //            //nano->Flag_CSCTightHalo2015Filter and
        //            //nano->Flag_globalTightHalo2016Filter and
        //            nano->Flag_globalSuperTightHalo2016Filter and
        //            //nano->Flag_HcalStripHaloFilter and
        //            //nano->Flag_hcalLaserEventFilter and
        //            nano->Flag_EcalDeadCellTriggerPrimitiveFilter and
        //            //nano->Flag_EcalDeadCellBoundaryEnergyFilter and
        //            nano->Flag_ecalBadCalibFilter and
        //            nano->Flag_goodVertices and
        //            //nano->Flag_eeBadScFilter and
        //            //nano->Flag_ecalLaserCorrFilter and
        //            //nano->Flag_trkPOGFilters and
        //            //nano->Flag_chargedHadronTrackResolutionFilter and
        //            //nano->Flag_muonBadTrackFilter and
        //            //nano->Flag_BadChargedCandidateFilter and
        //            nano->Flag_BadPFMuonFilter 
        //            //nano->Flag_BadChargedCandidateSummer16Filter and
        //            //nano->Flag_BadPFMuonSummer16Filter and
        //            //nano->Flag_trkPOG_manystripclus53X and
        //            //nano->Flag_trkPOG_toomanystripclus53X and
        //            //nano->Flag_trkPOG_logErrorTooManyClusters and
        //            //nano->Flag_METFilters
        //        );
        //}
        //if (year == 2018){
        //event_->met_. setFullRecommendation(
        //            nano->Flag_HBHENoiseFilter and
        //            nano->Flag_HBHENoiseIsoFilter and
        //            //nano->Flag_CSCTightHaloFilter and
        //            //nano->Flag_CSCTightHaloTrkMuUnvetoFilter and
        //            //nano->Flag_CSCTightHalo2015Filter and
        //            //nano->Flag_globalTightHalo2016Filter and
        //            nano->Flag_globalSuperTightHalo2016Filter and
        //            //nano->Flag_HcalStripHaloFilter and
        //            //nano->Flag_hcalLaserEventFilter and
        //            nano->Flag_EcalDeadCellTriggerPrimitiveFilter and
        //            //nano->Flag_EcalDeadCellBoundaryEnergyFilter and
        //            nano->Flag_ecalBadCalibFilter and
        //            nano->Flag_goodVertices and
        //            //nano->Flag_eeBadScFilter and
        //            //nano->Flag_ecalLaserCorrFilter and
        //            //nano->Flag_trkPOGFilters and
        //            //nano->Flag_chargedHadronTrackResolutionFilter and
        //            //nano->Flag_muonBadTrackFilter and
        //            //nano->Flag_BadChargedCandidateFilter and
        //            nano->Flag_BadPFMuonFilter 
        //            //nano->Flag_BadChargedCandidateSummer16Filter and
        //            //nano->Flag_BadPFMuonSummer16Filter and
        //            //nano->Flag_trkPOG_manystripclus53X and
        //            //nano->Flag_trkPOG_toomanystripclus53X and
        //            //nano->Flag_trkPOG_logErrorTooManyClusters and
        //            //nano->Flag_METFilters
        //        );
        //}
    }

    // puppi met
    {
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Puppi Met");
#endif
        TLorentzVector puppimet; puppimet.SetPtEtaPhiM(nano->PuppiMET_pt,0,nano->PuppiMET_phi,0);
        event_->met_ . SetPuppiMetP4(puppimet);

        TLorentzVector up; 
        TLorentzVector down; 
        up.SetPtEtaPhiM(nano->PuppiMET_ptJESUp,0,nano->PuppiMET_phiJESUp,0);
        down.SetPtEtaPhiM(nano->PuppiMET_ptJESDown,0,nano->PuppiMET_phiJESDown,0);
        event_->met_ . puppiMetSyst . SetFilled(Smearer::JES);
        event_->met_ . puppiMetSyst . SetValueUp(Smearer::JES,up);
        event_->met_ . puppiMetSyst . SetValueDown(Smearer::JES,down);

        up.SetPtEtaPhiM(nano->PuppiMET_ptJERUp,0,nano->PuppiMET_phiJERUp,0);
        down.SetPtEtaPhiM(nano->PuppiMET_ptJERDown,0,nano->PuppiMET_phiJERDown,0);
        event_->met_ . puppiMetSyst . SetFilled(Smearer::JER);
        event_->met_ . puppiMetSyst . SetValueUp(Smearer::JER,up);
        event_->met_ . puppiMetSyst . SetValueDown(Smearer::JER,down);

        up.SetPtEtaPhiM(nano->PuppiMET_ptUnclusteredUp,0,nano->PuppiMET_phiUnclusteredUp,0);
        down.SetPtEtaPhiM(nano->PuppiMET_ptUnclusteredDown,0,nano->PuppiMET_phiUnclusteredDown,0);
        event_->met_ . puppiMetSyst . SetFilled(Smearer::UNCLUSTER);
        event_->met_ . puppiMetSyst . SetValueUp(Smearer::UNCLUSTER,up);
        event_->met_ . puppiMetSyst . SetValueDown(Smearer::UNCLUSTER,down);
    }

    // raw met
    {
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Raw");
#endif
        TLorentzVector rawmet; rawmet.SetPtEtaPhiM(nano->RawMET_pt,0,nano->RawMET_phi,0);
        event_->met_ . SetRawMetP4(rawmet);
    }

    // track met
    {
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Track Met");
#endif
        TLorentzVector trackmet; trackmet.SetPtEtaPhiM(nano->TkMET_pt,0,nano->TkMET_phi,0);
        event_->met_ . SetTrackMetP4(trackmet);
    }

   // Fill Muon
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Track Muon");
#endif
   for(int i=0;i<nano->nMuon;++i)
   {
        //if (not Muon_isPFcand[i] ) continue; // loose
        Lepton *l =new Lepton();
        TLorentzVector p4;
        p4.SetPtEtaPhiM(nano->Muon_pt[i],nano->Muon_eta[i],nano->Muon_phi[i],nano->Muon_mass[i]);
        l->SetP4(p4);
        l->SetType(13);
        l->SetCharge(nano->Muon_charge[i]);
        l->SetMiniIso(nano->Muon_miniPFRelIso_all[i]);
        l->SetIso(nano->Muon_pfRelIso04_all[i]*nano->Muon_pt[i]);
        l->SetCharge( nano->Muon_charge[i]);
        l->SetTightId( nano->Muon_tightId[i]);
        l->SetMediumId( nano->Muon_mediumId[i]);
        l->SetLooseId( nano->Muon_looseId[i]);
        l->SetNLayers( nano->Muon_nTrackerLayers[i]);
        l->SetDxy(nano->Muon_dxy[i]);

        TLorentzVector fsrP4(0,0,0,0);
        for(int k=0;k<=nano->nFsrPhoton;++k)
        {
            if ( i!=nano->FsrPhoton_muonIdx[k]) continue; //not this muon
            double fsrDrEt2Cut = 0.012;
            double fsrIsoCut = 1.8;

            if( nano->FsrPhoton_dROverEt2[k] >=fsrDrEt2Cut) continue;
            if( nano->FsrPhoton_relIso03[k] >=fsrIsoCut) continue;
            fsrP4.SetPtEtaPhiM(nano->FsrPhoton_pt[k],nano->FsrPhoton_eta[k],nano->FsrPhoton_phi[k],0.);
            l-> SetFsrP4( fsrP4 ); // assume 1 per muon
        }

        // REMEMBER TO CLEN THE LEPTONS IFF NOT USED
        event_ -> leps_ . push_back(l);
   }

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Track Electron");
#endif
   // Fill Electrons.
   for(int i=0;i<nano->nElectron;++i)
   {
        Lepton *l =new Lepton();
        TLorentzVector p4;
        p4.SetPtEtaPhiM(nano->Electron_pt[i],nano->Electron_eta[i],nano->Electron_phi[i],nano->Electron_mass[i]);
        l->SetP4(p4);
        l->SetType(11);
        l->SetCharge(nano->Electron_charge[i]);
        l->SetDxy(nano->Electron_dxy[i]);

        l->SetMiniIso(nano->Electron_miniPFRelIso_all[i]);
        l->SetIso(nano->Electron_pfRelIso03_all[i]*nano->Electron_pt[i]);
        l->SetCharge( nano->Electron_charge[i]);
        l->SetTightId( nano->Electron_mvaFall17V1noIso_WP80[i]);
        l->SetMediumId( nano->Electron_mvaFall17V1noIso_WP90[i]);
        l->SetLooseId( nano->Electron_mvaFall17V1Iso_WPL[i]); 

        l->SetR9(nano->Electron_r9[i]);
        l->SetSieie(nano->Electron_sieie[i]);

	// REMEMBER TO CLEN THE LEPTONS IFF NOT USED
	event_ -> leps_ . push_back(l);
   }

   // re-sort leptons
   sort(event_ -> leps_.begin(),event_ -> leps_.end(),[](Lepton*l1, Lepton*l2){ 
           if (l1->GetP4().Pt() > l2->GetP4().Pt()) return true;  // sorting by grt Pt
           if (l1->GetP4().Pt() < l2->GetP4().Pt()) return false;
           if (l1->GetP4().Eta() > l2->GetP4().Eta()) return true; 
           if (l1->GetP4().Eta() < l2->GetP4().Eta()) return false;
           return l1->GetP4().Phi() > l2->GetP4().Phi(); // if pt, eta, phi are identical, then they are the same object
           } );

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Jets");
#endif
   // Fill Jets
   for(int i=0;i<nano->nJet;++i)
   {
        bool id =  nano->Jet_jetId[i];
        if (not id) continue;
        Jet *j = new Jet() ;
        TLorentzVector p4;
        p4.SetPtEtaPhiM(nano->Jet_pt[i],nano->Jet_eta[i],nano->Jet_phi[i],nano->Jet_mass[i]);

        j->SetP4(p4);

        j->SetNEMF(nano->Jet_neEmEF[i] );
        j->SetCEMF(nano->Jet_chEmEF[i] );
	j->SetArea(nano->Jet_area[i]);
#warning MISSING NANO JES JER
        // TODO-- JES
        //j->SetValueUp  (Smearer::JES , (1. + bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetValueDown(Smearer::JES , (1. - bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        //j->SetFilled(Smearer::JES);
        //
        //TODO -- JER
        j->bdiscr = nano->Jet_btagCSVV2[i];
        j->SetDeepB ( nano->Jet_btagDeepB[i] );
        j->SetDeepC ( nano->Jet_btagDeepC[i] );
	// temporary, there are many nan in v8
	j->SetQGL(std::isnan(nano->Jet_qgl[i])? 0.8 : nano->Jet_qgl[i]);

        //j->SetPuId(nano->Jet_puId[i]); //80X flags
        j->SetPuId(nano->Jet_puIdDisc[i]);//91x discr
        j->SetBCorrection(nano->Jet_bRegCorr[i], nano->Jet_bRegRes[i]); 
        j->SetHadFlavor(nano->Jet_hadronFlavour[i]); 
        j->pdgId = nano->Jet_partonFlavour[i] ;

#warning MISSING NANO_JET INFO
        j->motherPdgId = 0; // FIXME do we use them?
        j->grMotherPdgId =  0 ;// FIXME
        //Jet_genJetIdx

        event_ -> jets_ . push_back(j);
   }

#warning CHECK_MASKS
   //UChar_t         Electron_cleanmask[6];   //[nElectron]
   //UChar_t         Jet_cleanmask[25];   //[nJet]
   //UChar_t         Muon_cleanmask[7];   //[nMuon]
   //UChar_t         Photon_cleanmask[8];   //[nPhoton]
   //UChar_t         Tau_cleanmask[4];   //[nTau]
   
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling FatJets");
#endif
   //Fill Fatjets
   for(int i=0;i<nano->nFatJet;++i)
   {
       bool id =  nano->FatJet_jetId[i];
       if (not id) continue;

       FatJet *j = new FatJet() ;
       TLorentzVector p4;
       p4.SetPtEtaPhiM(nano->FatJet_pt[i],nano->FatJet_eta[i],nano->FatJet_phi[i],nano->FatJet_mass[i]);
       j->SetP4(p4);

       j->tau1 = nano->FatJet_tau1[i];
       j->tau2 = nano->FatJet_tau2[i];
       j->tau3 = nano->FatJet_tau3[i];

       // sf mass
       j->softdropMass = nano->FatJet_msoftdrop[i];
       //
       j->TvsQCDMD =nano->FatJet_deepTagMD_TvsQCD[i];
       j->WvsQCDMD =nano->FatJet_deepTagMD_WvsQCD[i];
       j->ZHbbvsQCDMD =nano->FatJet_deepTagMD_ZHbbvsQCD[i];
       j->ZHccvsQCDMD =nano->FatJet_deepTagMD_ZHccvsQCD[i];
       j->ZbbvsQCDMD =nano->FatJet_deepTagMD_ZbbvsQCD[i];
       j->ZvsQCDMD =nano->FatJet_deepTagMD_ZvsQCD[i];

       j->nSubjets=0;
       // Subjets
       int idx=nano->FatJet_subJetIdx1[i];
       if (idx >=0 and idx < nano->nSubJet){
           j->nSubjets+=1;
           j->subjet_btag= nano->SubJet_btagCSVV2[idx];
           j->subjet_btagdeep= nano->SubJet_btagDeepB[idx];
       }

       idx=nano->FatJet_subJetIdx2[i];
       if (idx >=0 and idx < nano->nSubJet){
           j->nSubjets+=1;
           j->subjet_btag= std::max(nano->SubJet_btagCSVV2[idx],j->subjet_btag);
           j->subjet_btagdeep= std::max(nano->SubJet_btagDeepB[idx],j->subjet_btagdeep);
       }

       event_ -> fat_ . push_back(j);
   }

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling TrackJets");
#endif
   // TrackJets -- Soft Activity
   for(int i =0 ;i<nano->nSoftActivityJet;++i){
        TrackJet *j =new TrackJet();
        TLorentzVector p4; p4.SetPtEtaPhiM(nano->SoftActivityJet_pt[i],nano->SoftActivityJet_eta[i],nano->SoftActivityJet_phi[i],0.) ;
        j->SetP4(p4);
        event_ -> tracks_ . push_back(j);
   
   }
   

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Taus");
#endif
   //FILL Tau
   for (int i=0;i<nano->nTau;++i)
   {
       Tau *t =new Tau();
       TLorentzVector p4; p4.SetPtEtaPhiM(nano->Tau_pt[i],nano->Tau_eta[i],nano->Tau_phi[i],nano->Tau_mass[i]) ;
       t -> SetP4( p4);
       t -> SetType( 15 );
       t -> SetCharge( nano->Tau_charge[i] );
       t -> SetId ( nano->Tau_idDecayModeNewDMs[i]) ;
        // probably not DB corrected: TOCHECK
       t -> SetIso2( nano->Tau_chargedIso[i]+nano->Tau_neutralIso[i]);
       t -> SetIdEle (nano->Tau_idAntiEle[i]); //2018? FIXME
       t -> SetIdMu (nano->Tau_idAntiMu[i]); // 2018 FIXME
       //decay mode
   }

#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Photons");
#endif
   //Fill Photons
   for (int i=0;i<=nano->nPhoton;++i)
   {
        // electron veto
        if (not nano->Photon_electronVeto[i]) continue;

        Photon *p = new Photon();
        TLorentzVector p4; p4.SetPtEtaPhiM(nano->Photon_pt[i],nano->Photon_eta[i],nano->Photon_phi[i],0.);
        p -> SetP4(p4);

        p -> SetIso(nano->Photon_pfRelIso03_all[i]) ;
        p -> SetId (nano->Photon_mvaID_WP80[i] ) ;
        p -> SetR9 ( nano->Photon_r9[i]) ;
        p -> SetSieie ( nano->Photon_sieie[i]) ;

        event_ -> phos_ . push_back(p);
   
   }


#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Trigger");
#endif
   //Fill Trigger and TriggerObjects
   {
       event_ -> triggerFired_ . clear();

       //event_ -> triggerNames_ . clear();
       //event_-> triggerFired_.push_back(nano->HLT_IsoMu24);

       for(const auto & s : event_->triggerNames_)
       {
           TLeaf *l = tree_->GetLeaf(s.c_str());
           if ( l == nullptr) {
               event_->triggerFired_.push_back(0);
            LogN(__FUNCTION__,"WARNING","Trigger ("+s+") Not FILLED",10);
           }
           else {
               event_->triggerFired_.push_back( *((bool*)l->GetValuePointer () ) );
           }

       }


   }


#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","Filling Gen");
#endif
   //Gen
   if ( tree_ ->GetBranchStatus("Generator_weight") != 0 ){
       // nano: genWeight ; Generator_weight ; LHEWeight_originalXWGTUP
       event_ -> GetWeight() -> SetMcWeight( nano->Generator_weight);
       event_ -> GetWeight() -> SetPU( nano->Pileup_nTrueInt ,  event_ -> runNum_);
       event_->SetGenTtbarId(nano-> genTtbarId) ;
       event_->SetLHEHT(nano-> LHE_HT) ;

       // l1 prefiring incorporated in weights
       if (year==2016 or year==2017){
           event_ -> GetWeight() -> SetL1Prefiring(nano->L1PreFiringWeight_Nom,0);
           event_ -> GetWeight() -> SetL1Prefiring(nano->L1PreFiringWeight_Up,1);
           event_ -> GetWeight() -> SetL1Prefiring(nano->L1PreFiringWeight_Dn,-1);
       }
    
        //     LHE scale variation weights (w_var / w_nominal); 
        //     [0] is renscfact=0.5d0 facscfact=0.5d0 ; 
        //     [1] is renscfact=0.5d0 facscfact=1d0 ; 
        //     [2] is renscfact=0.5d0 facscfact=2d0 ; 
        //     [3] is renscfact=1d0 facscfact=0.5d0 ; 
        //     [4] is renscfact=1d0 facscfact=1d0 ;
        //     [5] is renscfact=1d0 facscfact=2d0 ; 
        //     [6] is renscfact=2d0 facscfact=0.5d0 ; 
        //     [7] is renscfact=2d0 facscfact=1d0 ; 
        //     [8] is renscfact=2d0 facscfact=2d0
       
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[5]*nano->Generator_weight , MC::r1f2 ) ;
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[3]*nano->Generator_weight , MC::r1f5 ) ;
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[7]*nano->Generator_weight , MC::r2f1 ) ;
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[8]*nano->Generator_weight , MC::r2f2 ) ;
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[1]*nano->Generator_weight , MC::r5f1 ) ;
        event_ -> GetWeight() -> SetScaleWeight( nano->LHEScaleWeight[0]*nano->Generator_weight , MC::r5f5 ) ;
    //
        if ( tree_ ->GetBranchStatus("nLHEPdfWeight") != 0 ){  
            for (unsigned i=0 ; i< nano->nLHEPdfWeight;++i){
                event_->GetWeight()->SetPdfWeight( nano->LHEPdfWeight[i] * nano->Generator_weight, i);
            }
        }

        if (tree_->GetLeaf(MC::aqgc_names[0].c_str()) != nullptr){ // aQGC
           for (const auto& name : MC::aqgc_names) {
               event_->GetWeight()->SetAQGCWeight(
                       *(double*)tree_->GetLeaf(name.c_str())->GetValuePointer()
                       , name);
           }
        }

        // DEBUG AQGC
        //if (fname.find("aQGC") != string::npos) { // AQGC MC
        //    if (tree_->GetLeaf(MC::aqgc_names[0].c_str()) == nullptr)
        //    {Log(__FUNCTION__,"ERROR",string("file ")+ fname +" is aQGC but has no AQGC weight "+MC::aqgc_names[0]);}
        //    if (not event_->GetWeight()->HasAQGC())
        //    {Log(__FUNCTION__,"ERROR","AQGC weights not propagated correctly to the event weight");}
        //} 
        // END

       event_ -> SetPdfId(1,nano->Generator_id1);
       event_ -> SetPdfId(2,nano->Generator_id2);
       //Float_t         Generator_scalePDF;
       //Float_t         Generator_weight;
       //Float_t         Generator_x1;
       //Float_t         Generator_x2;
       //Float_t         Generator_xpdf1;
       //Float_t         Generator_xpdf2;
       //Int_t           Generator_id1;
       //Int_t           Generator_id2;
       for(int i=0;i<nano->nGenJet;++i){
            GenJet *g =new GenJet();
            TLorentzVector p4; p4.SetPtEtaPhiM(nano->GenJet_pt[i],nano->GenJet_eta[i],nano->GenJet_phi[i],nano->GenJet_mass[i]);
            g->SetP4( p4 );
            event_ -> genjets_ . push_back(g);
       }

       for(int i=0;i< nano->nGenPart ; ++i){
            GenParticle *g =new GenParticle();
            TLorentzVector p4; p4.SetPtEtaPhiM(nano->GenPart_pt[i],nano->GenPart_eta[i],nano->GenPart_phi[i],nano->GenPart_mass[i]);
            g->SetP4( p4 );
            g->SetPdgId( nano->GenPart_pdgId[i]);
            
            g->SetParentIdx(nano->GenPart_genPartIdxMother[i]);
            if (nano->GenPart_genPartIdxMother[i]>=0 and nano->GenPart_genPartIdxMother[i] < nano->nGenPart )
            {
                g->SetParentPdgId( nano->GenPart_pdgId[ nano->GenPart_genPartIdxMother[i]] ); 
            }
            else { g->SetParentPdgId(0);}
            int flags=0;
            //g->SetFlags( mc ->flags ->at(iGP) ); // rematch flags
            //Int_t           GenPart_status[126];   //[nGenPart]
            //Int_t           GenPart_statusFlags[126];   //[nGenPart]
            event_ -> genparticles_ . push_back(g);
       }

       for (int i=0;i<nano->nLHEPart;++i){
            GenParticle *g =new GenParticle();
            if (nano->LHEPart_status[i] != 1) continue; // incoming partons have status = -1
            TLorentzVector p4; p4.SetPtEtaPhiM(nano->LHEPart_pt[i],nano->LHEPart_eta[i],nano->LHEPart_phi[i],nano->LHEPart_mass[i]);
            g->SetP4( p4 );
            g->SetPdgId( nano->LHEPart_pdgId[i]);
            int flags=0; 
            flags |= BareMonteCarlo::LHE;
            g->SetFlags(flags);
            // no navigation saved for LHE part
            event_ -> genparticles_ . push_back(g);
       }
   }
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DEBUG","End");
#endif

}

void LoadNano::NewFile(){
    // read some how the triggers -> this is for Nero, probably not necessary.
    // grep -r IsTriggered src/AnalysisChargedHiggsTauNu.cpp |  tr '>' '\n' | grep IsTriggered | sed 's/).*//' | sed 's/,.*//' | sed 's/.*(//' | sort  | uniq
    //string fname = tree_->GetFile()->GetName();

    //current tree start from N
#ifndef NO_TCHAIN
    int fNumber = (isTreeActive_) ? tree_ -> GetTreeNumber(): -1;
    string fname = tree_->GetListOfFiles()->At(fNumber+1)->GetTitle(); 
#endif
    Log(__FUNCTION__,"DEBUG-NTC",string("Loading file")+fname);

    int year=2016;
    isData=false;

    if (fname.find("mc2016") != string::npos) {year=2016;} // prefer string like mcYYYY
    else if (fname.find("mc2017") != string::npos) {year=2017;}
    else if (fname.find("mc2018") != string::npos) {year=2018;}
    else if (fname.find("2016") != string::npos) {year=2016;} // fall back to YYYY
    else if (fname.find("2017") != string::npos) {year=2017;}
    else if (fname.find("2018") != string::npos) {year=2018;}
    else if (fname.find("UL16") != string::npos) {year=2016;} // fall back to YYYY
    else if (fname.find("UL17") != string::npos) {year=2017;}
    else if (fname.find("UL18") != string::npos) {year=2018;}
    else Log(__FUNCTION__,"WARNING","Unable to identify year. Using 2016 as default");
    
    if (fname.find("Run2016") != string::npos) {isData=true;} 
    else if (fname.find("Run2017") != string::npos) {isData=true;} 
    else if (fname.find("Run2018") != string::npos) {isData=true;} 

    // Figure out year
    SetYear(year);
    SetData(isData);
    
    InitTree();
    
    event_->triggerNames_.clear(); // possibly also not necessary by file

    
    if (year==2016){
      //https://dmytro.web.cern.ch/dmytro/trigger/2016/triggerEvolution_all.html
        event_->triggerNames_.push_back("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200");
        event_->triggerNames_.push_back("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240");
        event_->triggerNames_.push_back("HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6");
        event_->triggerNames_.push_back("HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160");
        event_->triggerNames_.push_back("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087");
        event_->triggerNames_.push_back("HLT_PFHT450_SixJet40_BTagCSV_p056");
        event_->triggerNames_.push_back("HLT_PFHT400_SixJet30_DoubleBTagCSV_p056");
        //event_->triggerNames_.push_back("");
        event_->triggerNames_.push_back("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20");
        event_->triggerNames_.push_back("HLT_Photon90_CaloIdL_PFHT600");
	//below from jetHT
        event_->triggerNames_.push_back("HLT_PFHT900");
        event_->triggerNames_.push_back("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50");
        event_->triggerNames_.push_back("HLT_PFJet450");
        event_->triggerNames_.push_back("HLT_AK8PFJet450");
	event_->triggerNames_.push_back("HLT_AK8DiPFJet300_200_TrimMass30");
        event_->triggerNames_.push_back("HLT_AK8PFJet360_TrimMass30");
        event_->triggerNames_.push_back("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5");
        event_->triggerNames_.push_back("HLT_PFHT650_WideJetMJJ950DEtaJJ1p5");
	//below from MET
	event_->triggerNames_.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
    }else if (year==2017){
      //    https://dmytro.web.cern.ch/dmytro/trigger/2017/triggerEvolution_all.html
      // below from MET
      event_->triggerNames_.push_back("HLT_PFMET120_PFMHT120_IDTight_PFHT60");
      event_->triggerNames_.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
      event_->triggerNames_.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60");
      // below from JetHT
      event_->triggerNames_.push_back("HLT_PFHT1050");
      event_->triggerNames_.push_back("HLT_AK8PFHT800_TrimMass50");
      event_->triggerNames_.push_back("HLT_AK8PFJet500");
      event_->triggerNames_.push_back("HLT_AK8PFJet400_TrimMass30");
      event_->triggerNames_.push_back("HLT_AK8PFJetFwd400");
      event_->triggerNames_.push_back("HLT_PFJet500");
      //HLT_CaloJet500_NoJetID
      //HLT_DiPFJetAve160_HFJEC_v13
      //HLT_DiPFJetAve60_HFJEC_v13
      event_->triggerNames_.push_back("HLT_PFJetFwd400");
      event_->triggerNames_.push_back("HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2");
      event_->triggerNames_.push_back("HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2");
      event_->triggerNames_.push_back("HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5");
      // below in Dataset BTagCSV
      event_->triggerNames_.push_back("HLT_AK8PFJet330_PFAK8BTagCSV_p1");
      event_->triggerNames_.push_back("HLT_DoublePFJets100MaxDeta1p6_DoubleCaloBTagCSV_p33");
      event_->triggerNames_.push_back("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0");
      event_->triggerNames_.push_back("HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2");
      event_->triggerNames_.push_back("HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1");
    } else if (year==2018){
      // https://dmytro.web.cern.ch/dmytro/trigger/triggerEvolution_all.html
      // below from MET
      event_->triggerNames_.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
      event_->triggerNames_.push_back("HLT_PFMET120_PFMHT120_IDTight_PFHT60");
      event_->triggerNames_.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60");
      // below from JetHT
      event_->triggerNames_.push_back("HLT_PFHT1050");
      event_->triggerNames_.push_back("HLT_AK8PFJet500");
      event_->triggerNames_.push_back("HLT_AK8PFHT800_TrimMass50");
      event_->triggerNames_.push_back("HLT_AK8PFJet400_TrimMass30");
      event_->triggerNames_.push_back("HLT_AK8PFJetFwd200");
      event_->triggerNames_.push_back("HLT_PFJetFwd140");
      event_->triggerNames_.push_back("HLT_PFJet500");
      event_->triggerNames_.push_back("HLT_IsoMu27");
      event_->triggerNames_.push_back("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np2");
      event_->triggerNames_.push_back("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_p02");
      event_->triggerNames_.push_back("HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94");
      event_->triggerNames_.push_back("HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59");
      event_->triggerNames_.push_back("HLT_DoublePFJets200_CaloBTagDeepCSV_p71");
      event_->triggerNames_.push_back("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5");
      event_->triggerNames_.push_back("HLT_QuadPFJet103_88_75_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1");
      event_->triggerNames_.push_back("HLT_QuadPFJet103_88_75_15_PFBTagDeepCSV_1p3_VBF2");
      event_->triggerNames_.push_back("HLT_DiPFJetAve80");
    }

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
