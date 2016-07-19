#include "interface/AnalysisChargedHiggsTopBottom.hpp"

void ChargedHiggsTopBottom::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetIsoCut(10);
    l->SetPtCut(10);
    l->SetIsoRelCut(-1);
    l->SetEtaCut(2.4);
}

void ChargedHiggsTopBottom::SetJetCuts(Jet *j){
    j->SetBCut(0.800);
    j->SetEtaCut(4.7);
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(50);
    //j->SetPuIdCut(0.5);
}

void ChargedHiggsTopBottom::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetIsoCut(1.5);
    t->SetPtCut(20);
    t->SetEtaCut(2.4);
    t->SetMuRej(false);
    t->SetEleRej(false);
}

void ChargedHiggsTopBottom::Init()
{

    Preselection();

}

void ChargedHiggsTopBottom::Preselection()
{

    for ( string l : AllLabel()  ) {
        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
        //        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);

        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Filling Histo CutFlow" <<l<<endl;

        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");

        // do1lAnalysis
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{taus} = 0");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT < 120");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(FourJets+1,"N_{jets} #geq 4");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=3");

        // do2lAnalysis
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{tau} ??");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT ????");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(FourJets+1,"N_{jets} #geq 4");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=3");

        //
        ////Histograms for N-1
        Book("ChargedHiggsTopBottom/PreselectionN1/EtMiss_"+l,"EtMiss "+l+";E_{T}^{miss} [GeV]",100,0,500);
        Book("ChargedHiggsTopBottom/PreselectionN1/Njets_"+l,"Njets "+l + ";Number of selected jets P_{T}>50",20,0,20);

        Book("ChargedHiggsTopBottom/Preselection1Lep/mT_"+l,"mT "+l+";mT_{l,met}",100,0,200);
        Book("ChargedHiggsTopBottom/Preselection1Lep/TauPt_"+l,"TauPt "+l+";P_{T}^{#tau} [GeV]",100,0,200);
        Book("ChargedHiggsTopBottom/Preselection1Lep/RLepTau_"+l,"Rlb "+l+";dR_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/RMuTau_"+l,"RMub "+l+";dR_{#muu,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/REleTau_"+l,"REleb "+l+";dR_{e,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/PhiLepTau_"+l,"dPhilb "+l+";dPhi_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/EtaLepTau_"+l,"dEtalb "+l+";dEta_{lb}",100,0,10);

        /////
        Book("ChargedHiggsTopBottom/Baseline/LeadingBPt_"+l,"LeadingBPt "+l+";P_{T} (leading B) [GeV]",100,0,500);
        Book("ChargedHiggsTopBottom/Baseline/NBjets_"+l,"NBjets "+l + ";Number of selected b jets",10,0,10);
        //        Book("ChargedHiggsTopBottom/Baseline/Rlb_"+l,"Rlb "+l+";R_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/dEtalb_"+l,"dEtalb "+l+";dEta_{lb}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/dPhilb_"+l,"dPhilb "+l+";dPhi_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/ptW_"+l,"ptW "+l+";pT_{l,MET}",100,0,500);
        Book("ChargedHiggsTopBottom/Baseline/ptWb_"+l,"ptWb "+l+";pT_{W,b}",100,0,500);
        Book("ChargedHiggsTopBottom/Baseline/PhiWb_"+l,"dPhiWb "+l+";dPhi_{Wb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/mtWb_"+l,"mtWb "+l+";mT_{b,l,MET}",100,0,1000);
        //
        Book("ChargedHiggsTopBottom/Baseline/DEtaMax_"+l,"DEtaMax "+l+";dEta_{jb}^{max}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/minDRbb_"+l,"minDRbb"+l+";dR_{bb}^{min}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/minDRbb_mass_"+l,"minDRbb_mass "+l+";m(bb)^{dR^{min}})",100,0,300);

    }

}



int ChargedHiggsTopBottom::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsTopBottom]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);
    cut.reset();

    if(e->weight() == 0. ) cout <<"[ChargedHiggsTopBottom]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    cut.SetCutBit(Total);
    Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,0,e->weight());
    Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,0,e->weight());

    //The event selection should be implemented here for details see src/AnalysisChargedHiggsTauNu.cpp

    Object* sub = NULL;

    int nGoodLepton=0;
    int nOSLepton=0;

    Lepton* leadLep=NULL;
    Lepton* trailLep=NULL;
    Lepton* mu=NULL; // first muon
    Lepton* ele=NULL; // first electrons
    Tau *t = e->GetTau(0);

    for(int i=0;i!=e->Nleps();++i) {
            Lepton *it = e->GetLepton(i);
            if(it->Pt()>20 and it->Isolation()<1.5) {

                nGoodLepton++;

                if(nGoodLepton==1) leadLep = it;
                if(nGoodLepton==1 and it->IsMuon()) mu = it;
                if(nGoodLepton==1 and it->IsElectron()) ele = it;

                // found a second lepton OS and mll>12 GeV and abs(mll-91)<15
                if(nGoodLepton>1 and nOSLepton==0 and
                   it->Charge()*leadLep->Charge()<0 and
                   std::abs((it->GetP4() + leadLep->GetP4()).M()-91) > 15 and (it->GetP4() + leadLep->GetP4()).M() > 12 )
                   { trailLep=it; nOSLepton++; }
            }
    }


    int nTau=0;
    if(nGoodLepton==1) {
        for(int i=0;i!=e->Ntaus();++i) {
            Tau *it = e->GetTau(i);
            if(it->DeltaR(*leadLep)>0.4) nTau++;
        }
    }

    //    bool do1lAnalysis=true;
    //    bool do2lAnalysis=false;

    ////// --> 1l analysis
    if(do1lAnalysis) {
        if ( nGoodLepton == 1 ) cut.SetCutBit(OneLep); // one lep
        if ( e->Nleps() == 1 ) cut.SetCutBit(NoSecondLep); // only one lep <--- kill ttbar dilepton ttbar
        if ( nTau == 0 ) cut.SetCutBit(NoTau); // tau Veto <--- dilepton killer
    }

    ////// --> 2l analysis
    if(do2lAnalysis) {
        if ( nOSLepton == 1) cut.SetCutBit(OneLep); // one lep
        if ( e->Nleps() == 2 ) cut.SetCutBit(NoSecondLep); // only two lep <--- kill multiboson/ttV
        if ( true ) cut.SetCutBit(NoTau); // tau Veto -- not needed
    }
    if ( e->GetMet().Pt() >= 30 ) cut.SetCutBit(Met);

    if(do1lAnalysis) {
        if ( nGoodLepton==1 && e->Mt(Event::MtLepton)<110) cut.SetCutBit(Mt); // dilepton Killer
    } else {
        cut.SetCutBit(Mt); // Mt -- not needed
    }
    if ( e->Njets() >= 4 ) cut.SetCutBit(FourJets);


    //// 1L CutFlow
    if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,OneLep,e->weight());
    if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoTau,e->weight());
    if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoSecondLep,e->weight());
    if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,Met,e->weight());
    if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,Mt,e->weight());
    if( cut.passAllUpTo(FourJets)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,FourJets,e->weight());

    //// 2L CutFlow
    if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,OneLep,e->weight());
    if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NoTau,e->weight());
    if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NoSecondLep,e->weight());
    if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,Met,e->weight());
    if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,Mt,e->weight());
    if( cut.passAllUpTo(FourJets)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,FourJets,e->weight());

    ///////
    ///////
    ///////

    if( cut.passAllExcept(Met) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1/EtMiss_"+label,systname, e->GetMet().Pt() ,e->weight());
    }

    if( cut.passAllExcept(FourJets) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1/Njets_"+label,systname, e->Njets() ,e->weight());
    }

    if(do1lAnalysis && nGoodLepton==1) {

        if( cut.passAllExcept(Mt)  && (nGoodLepton==1)) {
            Fill("ChargedHiggsTopBottom/Preselection1Lep/mT_"+label,systname,e->Mt(Event::MtLepton),e->weight());
        }

        if( cut.passAllExcept(NoTau) ) {
            if((t!=NULL) && leadLep != NULL) {
            ////
            Fill("ChargedHiggsTopBottom/Preselection1Lep/TauPt_"+label,systname,t->Pt(),e->weight());
            Fill("ChargedHiggsTopBottom/Preselection1Lep/RLepTau_"+label,systname,t->DeltaR(*leadLep),e->weight());

            if(mu!=NULL) Fill("ChargedHiggsTopBottom/Preselection1Lep/RMuTau_"+label,systname,t->DeltaR(*mu),e->weight());
            if(ele!=NULL) Fill("ChargedHiggsTopBottom/Preselection1Lep/REleTau_"+label,systname,t->DeltaR(*ele),e->weight());

            Fill("ChargedHiggsTopBottom/Preselection1Lep/PhiLepTau_"+label,systname,t->DeltaPhi(*leadLep),e->weight());
            Fill("ChargedHiggsTopBottom/Preselection1Lep/EtaLepTau_"+label,systname,t->DeltaEta(*leadLep),e->weight());
            ////
            }
        }

    }

    ////////
    ////   Baseline up to now
    ////
    ////


    if( cut.passAll()  && (leadLep != NULL)) {
        //        double p4W=e->GetMet().GetP4() + lep->GetP4();
        double ptWx=e->GetMet().GetP4().Px() + leadLep->GetP4().Px();
        double ptWy=e->GetMet().GetP4().Py() + leadLep->GetP4().Py();
        Fill("ChargedHiggsTopBottom/Baseline/ptW_"+label,systname, sqrt(ptWx*ptWx + ptWy*ptWy) ,e->weight());
        //        double ptWx=e->GetMet().GetP4().Px() + lep->GetP4().Px();
        //        double ptWy=e->GetMet().GetP4().Py() + lep->GetP4().Py();
        //        Fill("ChargedHiggsTopBottom/Baseline/ptW_"+label,systname, sqrt(ptWx*ptWx + ptWy*ptWy) ,e->weight());
        //        Fill("ChargedHiggsTopBottom/PreselectionN1/PhiLepTau_"+label,systname,t->DeltaPhi(*lep),e->weight());


    }


    //    cut.passAllExcept()

    //    if( cut.passAllUpTo(OneBjet)   ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,OneBjet,e->weight());
    //    if( cut.passAllUpTo(Trigger)       ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,Trigger,e->weight());

    //    Tau *t= e->GetTau(0);
    //    Jet *j0 = e->GetJet(0);
    //    Jet *j1 = e->GetJet(1);

    //    GenParticle * gp  = e->GetGenElectron(0,2.4);
    //    Jet * bj2 = e->GetBjet(1);

    if( cut.passAll() ) {

        Jet * bj1 = e->GetBjet(0);
        if (bj1 != NULL) {
            Fill("ChargedHiggsTopBottom/Baseline/LeadingBPt_"+label,systname, bj1->Pt() ,e->weight());

            if ( bj1->Pt() >= 100 ) cut.SetCutBit(B1Pt);
            if ( cut.passAllUpTo(B1Pt) )  Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,B1Pt,e->weight());
            if ( cut.passAllUpTo(B1Pt) )  Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,B1Pt,e->weight());

            if (leadLep != NULL) {
                TLorentzVector p4W=e->GetMet().GetP4() + leadLep->GetP4();

                ////                Fill("ChargedHiggsTopBottom/Basseline/PhiWb_"+label,systname,deltaPhi(bj1->Phi(), p4W.Phi()),e->weight());

                double ptWbx=e->GetMet().GetP4().Px() + leadLep->GetP4().Px() + bj1->GetP4().Px();
                double ptWby=e->GetMet().GetP4().Py() + leadLep->GetP4().Py() + bj1->GetP4().Py();

                Fill("ChargedHiggsTopBottom/Baseline/ptWb_"+label,systname, sqrt(ptWbx*ptWbx + ptWby*ptWby) ,e->weight() );
                Fill("ChargedHiggsTopBottom/Baseline/mtWb_"+label,systname, ChargedHiggs::mt(p4W.Pt(),bj1->Pt(),p4W.Phi(),bj1->Phi()) , e->weight());

            }

            if (leadLep != NULL) {
                float deltaEtalb= bj1->DeltaEta(*leadLep);
                float deltaPhilb= bj1->DeltaPhi(*leadLep);
                Fill("ChargedHiggsTopBottom/Baseline/dEtalb_"+label, systname, deltaEtalb , e->weight() );
                Fill("ChargedHiggsTopBottom/Baseline/dPhilb_"+label, systname, deltaPhilb , e->weight() );
            }

            double DEtaMax=0.;

            for(int i=0;i!=e->Njets();++i)
                {
                    Jet* jet = e->GetJet(i);
                    if(bj1->DeltaEta(*jet)>DEtaMax) DEtaMax=bj1->DeltaEta(*jet);
                    Fill("ChargedHiggsTopBottom/Baseline/DEtaMax_"+label, systname, DEtaMax , e->weight() );

                }
        }


        double minDRbb=99999;
        double minDRbb_invMass=-1;

        for(int i=0;i!=e->Bjets();++i) {
            Jet* bjet = e->GetBjet(i);
            for(int j=0;j!=e->Bjets();++j) {
                if (j==i) continue;
                double dr = bjet->DeltaR(e->GetBjet(j));
                double mass = (bjet->GetP4() + e->GetBjet(j)->GetP4()).M();
                if(dr<minDRbb) { minDRbb=dr; minDRbb_invMass=mass;}
            }
        }

        if(e->Bjets()>1) {
            Fill("ChargedHiggsTopBottom/Baseline/minDRbb_"+label, systname, minDRbb , e->weight() );
            Fill("ChargedHiggsTopBottom/Baseline/minDRbb_mass_"+label, systname, minDRbb_invMass , e->weight() );
        }

        /////

        Fill("ChargedHiggsTopBottom/Baseline/NBjets_"+label,systname, e->Bjets() ,e->weight());
        if ( e->Bjets()>=3 ) cut.SetCutBit(NB);
        if ( cut.passAllUpTo(NB) )  Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NB,e->weight());

    }


    return EVENT_NOT_USED;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
