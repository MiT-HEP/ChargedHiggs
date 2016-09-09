#include "interface/AnalysisChargedHiggsTopBottom.hpp"

void ChargedHiggsTopBottom::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetIsoCut(10);
    l->SetPtCut(10);
    l->SetIsoCut(-1.); // absolute isolation
    l->SetIsoRelCut(0.25); // relative isolation
    l->SetEtaCut(2.4);
    l->SetTightCut(false); // use the loose selection for now
}

void ChargedHiggsTopBottom::SetJetCuts(Jet *j){
    j->SetBCut(0.800); //0.800 medium // 0.460 loose
    j->SetEtaCut(4.7);
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(50);
    //j->SetPuIdCut(0.5);
}

void ChargedHiggsTopBottom::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(20);
    t->SetEtaCut(2.4);
    t->SetMuRej(false);
    t->SetEleRej(false);
}

void ChargedHiggsTopBottom::Init()
{

    Log(__FUNCTION__,"INFO",Form("do1lAnalysis=%d",do1lAnalysis));
    Log(__FUNCTION__,"INFO",Form("do2lAnalysis=%d",do2lAnalysis));

    Preselection();

}

void ChargedHiggsTopBottom::Preselection()
{

    for ( string l : AllLabel()  ) {
        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
        //        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);

        Book(    "ChargedHiggsTopBottom/CutFlow/SROnelep_"+ l  , "" ,10,-.5,10-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/SRDilep_"+ l  , "" ,10,-.5,10-.5);

        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Filling Histo CutFlow" <<l<<endl;

        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");

        // do1lAnalysis
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{taus} = 0");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT < 120");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 5");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");
        //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");

        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(1,"numB==1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(2,"numB==2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(3,"numB>=3");


        // do2lAnalysis
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{tau} ??");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT ????");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 3");
        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");
        //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");

        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(1,"numB==1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(2,"numB==2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(3,"numB>=3");
        //
        ////Histograms for N-1
        Book("ChargedHiggsTopBottom/PreselectionN1/EtMiss_"+l,"EtMiss "+l+";E_{T}^{miss} [GeV]",60,0,300);
        Book("ChargedHiggsTopBottom/PreselectionN1/Njets_"+l,"Njets "+l + ";Number of selected jets P_{T}>50",15,0,15);
        Book("ChargedHiggsTopBottom/PreselectionN1/NBjets_"+l,"NBjets "+l + ";Number of selected b jets",10,0,10);

        Book("ChargedHiggsTopBottom/Preselection1Lep/mT_"+l,"mT "+l+";mT_{l,met}",40,0,200);
        Book("ChargedHiggsTopBottom/Preselection1Lep/TauPt_"+l,"TauPt "+l+";P_{T}^{#tau} [GeV]",100,0,200);
        Book("ChargedHiggsTopBottom/Preselection1Lep/RLepTau_"+l,"Rlb "+l+";dR_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/RMuTau_"+l,"RMub "+l+";dR_{#mu,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/REleTau_"+l,"REleb "+l+";dR_{e,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/PhiLepTau_"+l,"dPhilb "+l+";dPhi_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/EtaLepTau_"+l,"dEtalb "+l+";dEta_{lb}",100,0,10);

        /////
        Book("ChargedHiggsTopBottom/Baseline/HT_"+l,"HT "+l+"; HT (P_{T}^{jet}> 50)[GeV]",30,0,1500); // bins of 50 GeV
        // VARIOUS qb
        Book("ChargedHiggsTopBottom/Baseline/LeadingBPt_"+l,"LeadingBPt "+l+";P_{T} (leading B) [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/Baseline/DEtaMax_"+l,"DEtaMax "+l+";dEta_{j^{i},b^{1}}^{max}",100,0,10);
        // min DRbb
        Book("ChargedHiggsTopBottom/Baseline/minDRbb_"+l,"minDRbb"+l+";dR_{bb}^{min}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/minDRbb_mass_"+l,"minDRbb_mass "+l+";m(bb)^{dR^{min}})",100,0,300);
        // b1b2
        Book("ChargedHiggsTopBottom/Baseline/DRb1b2_"+l,"DRb1b2"+l+";dR_{b^{1}b^{2}}",100,0,10);
        Book2D( "ChargedHiggsTopBottom/Baseline/DRVsMassb1b2_"+l,"DRVsMassb1b2"+l+";mass(b^{1},b^{2});dR(b^{1},b^{2})",100,0,1000,100,0,10);
        // min DRqq
        Book("ChargedHiggsTopBottom/Baseline/minDRqq_"+l,"minDRqq"+l+";dR_{qq}^{min}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/minDRqq_mass_"+l,"minDRqq_mass "+l+";m(qq)^{dR^{min}})",100,0,300);
        // min central/forward
        Book("ChargedHiggsTopBottom/Baseline/Ncentraljets_"+l,"Njets "+l + ";Number of selected jets P_{T}>50 |#eta|<=2.4",20,0,20);
        Book("ChargedHiggsTopBottom/Baseline/Nforwardjets_"+l,"Njets "+l + ";Number of selected jets P_{T}>50 |#eta|>2.4",20,0,20);

        /////
        // lb
        Book("ChargedHiggsTopBottom/Baseline/minDRlb_"+l,"minDRlb"+l+";dR_{lb}^{min}",100,0,10);
        Book("ChargedHiggsTopBottom/Baseline/minDRlb_mass_"+l,"minDRlb_mass "+l+";m(lb)^{dR^{min}})",100,0,300);
        //        Book("ChargedHiggsTopBottom/Baseline/Rlb_"+l,"Rlb "+l+";R_{lb}",100,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/Baseline/dEtalb_"+l,"dEtalb "+l+";dEta_{l^{1},b^{1}}",50,0,10);
        Book("ChargedHiggsTopBottom/Baseline/dPhilb_"+l,"dPhilb "+l+";dPhi_{l^{1},b^{1}}",50,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/dRlb_"+l,"dRlb "+l+";dR_{l^{1},b^{1}}",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/Baseline/dEtal2b_"+l,"dEtal2b "+l+";dEta_{l^{2},b^{1}}",50,0,10);
        Book("ChargedHiggsTopBottom/Baseline/dPhil2b_"+l,"dPhil2b "+l+";abs(dPhi_{l^{2},b^{1}})",50,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/dRl2b_"+l,"dRl2b "+l+";dR_{l^{2},b^{1}}",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/Baseline/ptW_"+l,"ptW "+l+";pT_{l,MET}",100,0,500);
        Book("ChargedHiggsTopBottom/Baseline/ptWb_"+l,"ptWb "+l+";pT_{W,b}",100,0,500);
        Book("ChargedHiggsTopBottom/Baseline/PhiWb_"+l,"dPhiWb "+l+";abs(dPhi_{Wb})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/Baseline/mtWb1_"+l,"mtWb1 "+l+";mT_{b1,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/Baseline/ptWb1_"+l,"ptWb1 "+l+";pT_{b1,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/Baseline/minMT_"+l,"minMT "+l+";minMT [ (b1,(l1,MET)),(b1,(l2,MET)) ] ",50,0,500);
        Book("ChargedHiggsTopBottom/Baseline/maxMT_"+l,"maxMT "+l+";maxMT [ (b1,(l1,MET)),(b1,(l2,MET)) ]",50,0,500);

        /// various higgs candidate

        // trueComb
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_trueComb_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_trueComb_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_trueComb_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_trueComb_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_trueComb_"+l,"dPhiWb2 "+l+";abs(dPhi_{W,b2})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_trueComb_"+l,"dPhiWb2b1 "+l+";abs(dPhi_{top,b1})",100,0,TMath::Pi());

        /// b closest in DR
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_closeDRb_rightComb_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_closeDRb_rightComb_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_closeDRb_rightComb_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_closeDRb_rightComb_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_closeDRb_rightComb_"+l,"dPhiWb2 "+l+";abs(dPhi_{W,b2})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_closeDRb_rightComb_"+l,"dPhiWb2b1 "+l+";abs(dPhi_{top,b1})",100,0,TMath::Pi());

        /// leading/trailing b
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBtrailB_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBtrailB_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBtrailB_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBtrailB_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBtrailB_"+l,"dPhiWb2 "+l+";abs(dPhi_{W,b2})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBtrailB_"+l,"dPhiWb2b1 "+l+";abs(dPhi_{top,b1})",100,0,TMath::Pi());

        /// right combination
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBtrailB_rightComb_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBtrailB_rightComb_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBtrailB_rightComb_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBtrailB_rightComb_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBtrailB_rightComb_"+l,"dPhiWb2 "+l+";dPhi_{W,b2}",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBtrailB_rightComb_"+l,"dPhiWb2b1 "+l+";dPhi_{top,b1}",100,0,TMath::Pi());

        /// maxDR
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBmaxDRB_rightComb_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBmaxDRB_rightComb_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBmaxDRB_rightComb_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBmaxDRB_rightComb_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBmaxDRB_rightComb_"+l,"dPhiWb2 "+l+";dPhi_{W,b2}",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBmaxDRB_rightComb_"+l,"dPhiWb2b1 "+l+";dPhi_{top,b1}",100,0,TMath::Pi());

        /// closeToLept
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBtrailB_rightComb_minDRl_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBtrailB_rightComb_minDRl_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBtrailB_rightComb_minDRl_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBtrailB_rightComb_minDRl_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBtrailB_rightComb_minDRl_"+l,"dPhiWb2 "+l+";dPhi_{W,b2}",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBtrailB_rightComb_minDRl_"+l,"dPhiWb2b1 "+l+";dPhi_{top,b1}",100,0,TMath::Pi());

        // eq2b
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBtrailB_rightComb_eq2b_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBtrailB_rightComb_eq2b_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBtrailB_rightComb_eq2b_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBtrailB_rightComb_eq2b_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBtrailB_rightComb_eq2b_"+l,"dPhiWb2 "+l+";dPhi_{W,b2}",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBtrailB_rightComb_eq2b_"+l,"dPhiWb2b1 "+l+";dPhi_{top,b1}",100,0,TMath::Pi());

        // ge3b
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2_leadBtrailB_rightComb_ge3b_"+l,"mtWb2 "+l+";mT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/mtWb2b1_leadBtrailB_rightComb_ge3b_"+l,"mtWb2b1 "+l+";mT_{b1,(b2,l,MET)}",50,0,2000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2_leadBtrailB_rightComb_ge3b_"+l,"ptWb2 "+l+";pT_{b2,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/ptWb2b1_leadBtrailB_rightComb_ge3b_"+l,"ptWb2b1 "+l+";pT_{b1,(b2,l,MET)}",50,0,1000);
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2_leadBtrailB_rightComb_ge3b_"+l,"dPhiWb2 "+l+";dPhi_{W,b2}",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_leadBtrailB_rightComb_ge3b_"+l,"dPhiWb2b1 "+l+";dPhi_{top,b1}",100,0,TMath::Pi());

        ///// GENERATION
        Book("ChargedHiggsTopBottom/GENERATOR/ptbAss_"+l,"bAss"+l+"; p_{T}(associated b)",50,0,500); // bins of 10 GeV
        Book("ChargedHiggsTopBottom/GENERATOR/ptbFromTopH_"+l,"bFromTopH"+l+"; p_{T}(from top from the Higgs)",50,0,500); // bins of 10 GeV
        Book("ChargedHiggsTopBottom/GENERATOR/ptbFromTopAss_"+l,"bFromTopAss"+l+"; p_{T}(from associated top)",50,0,500); // bins of 10 GeV
        Book("ChargedHiggsTopBottom/GENERATOR/ptbFromH_"+l,"bFromH"+l+"; p_{T}( form Higgs Decay)",50,0,500); // bins of 10 GeV

        Book("ChargedHiggsTopBottom/GENERATOR/etabAss_"+l,"bAss"+l+"; p_{T}(associated b)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromTopH_"+l,"bFromTopH"+l+"; p_{T}(from top from the Higgs)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromTopAss_"+l,"bFromTopAss"+l+"; p_{T}(from associated top)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromH_"+l,"bFromH"+l+"; p_{T}( form Higgs Decay)",50,0,10);

    }

}


bool ChargedHiggsTopBottom::genInfo(Event*e, GenParticle * & bAss, GenParticle * & bFromTopH , GenParticle * & bFromTopAss, GenParticle * & bFromH) {

    bool verbose=false;

    if(verbose) cout << "====================================================" << endl;

    GenParticle *genW1 = 0, *genW2 = 0, *genH = 0, *genCH = 0, *lep;
    GenParticle *topFromH = 0;

    bool rightComb=false;

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);

        /// Charged Higgs
        if(abs(genpar->GetPdgId()) == 37){
            if(!genCH) genCH = genpar;
            if(genCH and verbose) cout << "found Higgs: pt= " << genCH->GetP4().Pt() << " charge=" << genCH->GetPdgId() << " Idx = "<< i << endl;
        } else if(abs(genpar->GetPdgId()) == 6){
        /// t
            if(verbose) cout << " found TOP: pt= " << genpar->GetP4().Pt() << " charge=" << genpar->GetPdgId() << " moPdgId=" << genpar->GetParentPdgId() << endl;
            if(genpar->GetParentPdgId()==37) {
                if(verbose) cout << "   =$$$$= TOP from Higgs" << endl;
                topFromH = genpar;
            } else {
                if(verbose) cout << "   ==> prompt top" << endl;
            }
        } else if(abs(genpar->GetPdgId()) == 5){
        /// b
            if(genpar->GetParentPdgId()==37)  {
                bFromH = genpar;
            } else if(abs(genpar->GetParentPdgId())==6) {
                if(genpar->GetGrandParentPdgId()==37) {
                    bFromTopH = genpar;
                } else {
                    bFromTopAss=genpar;
                }
            } else {
                bAss = genpar;
            }

        }

    } // loop over GenParticle


    // need to find the leptonic Higgs
    for(Int_t i = 0; i < e->NGenPar(); i++){
        //FIXME: logic ?!?
        GenParticle *genpar = e->GetGenParticle(i);

        if(abs(genpar->GetPdgId()) == 11 or abs(genpar->GetPdgId()) == 13) {
            // lepton in acceptance
            if(genpar->Pt()<20) continue;
            if(abs(genpar->Eta())>2.4) continue;

            if(topFromH) {
                if ( abs(genpar->GetParentPdgId()) == 24 and abs(genpar->GetGrandParentPdgId()) == 6 and topFromH->GetPdgId()*genpar->GetPdgId()) {
                    if(verbose) cout << "H->top->W->l candidate (there is a second one in the acceptance)" << endl;
                    rightComb=true;
                }
            }
        }
    }

    //    if(verbose) cout << " found B: pt= " << genpar->GetP4().Pt() << " charge=" << genpar->GetPdgId() << " moPdgId=" << genpar->GetParentPdgId() << endl;
    if(verbose and bFromH) cout << "   ==> B from Higgs " << bFromH->GetP4().Pt() << endl;
    if(verbose and bFromTopH) cout << "   ==> B from TOP from Higgs " << bFromTopH->GetP4().Pt() << endl;
    if(verbose and bFromTopAss) cout << "   ==> B from associated TOP " << bFromTopAss->GetP4().Pt() << endl;
    if(verbose and bAss) cout << "    ==> prompt b " << bAss->GetP4().Pt() << endl;

    return rightComb;

}

void ChargedHiggsTopBottom::leptonicHiggs(Event*e,string label, string systname, TLorentzVector p4b1, TLorentzVector p4b2, TLorentzVector p4W, string combination) {

    p4b1.SetPz(0);
    p4b1.SetE(sqrt(p4b1.Px()*p4b1.Px() + p4b1.Py()*p4b1.Py()));

    p4b2.SetPz(0);
    p4b2.SetE(sqrt(p4b2.Px()*p4b2.Px() + p4b2.Py()*p4b2.Py()));

    TLorentzVector p4TOPlepcand,p4HIGlepcand;
    p4TOPlepcand=p4b2+p4W;
    p4HIGlepcand=p4b1+p4TOPlepcand;

    Fill("ChargedHiggsTopBottom/MASSreco/mtWb2_"+combination+"_"+label,systname, ChargedHiggs::mtMassive(p4W,p4b2) , e->weight());
    Fill("ChargedHiggsTopBottom/MASSreco/mtWb2b1_"+combination+"_"+label,systname, ChargedHiggs::mtMassive(p4TOPlepcand,p4b1) , e->weight());

    Fill("ChargedHiggsTopBottom/MASSreco/ptWb2_"+combination+"_"+label,systname, p4TOPlepcand.Pt() , e->weight());
    Fill("ChargedHiggsTopBottom/MASSreco/ptWb2b1_"+combination+"_"+label,systname, p4HIGlepcand.Pt() , e->weight());

    Fill("ChargedHiggsTopBottom/MASSreco/PhiWb2_"+combination+"_"+label,systname,abs(ChargedHiggs::deltaPhi(p4b2.Phi(), p4W.Phi())),e->weight());
    Fill("ChargedHiggsTopBottom/MASSreco/PhiWb2b1_"+combination+"_"+label,systname,abs(ChargedHiggs::deltaPhi(p4b1.Phi(), p4TOPlepcand.Phi())),e->weight());

}



void ChargedHiggsTopBottom::jetPlot(Event*e,string label, string systname) {

    ///////////
    //// STUDY various jets properties
    //// forward vs central

    double HT=0;

    for(int i=0;i!=e->Njets();++i) {
        HT += e->GetJet(i)->GetP4().Pt();
    }

    Fill("ChargedHiggsTopBottom/Baseline/HT_"+label,systname, HT ,e->weight());

    Fill("ChargedHiggsTopBottom/Baseline/Ncentraljets_"+label,systname, e->NcentralJets() ,e->weight());
    Fill("ChargedHiggsTopBottom/Baseline/Nforwardjets_"+label,systname, e->NforwardJets() ,e->weight());

    ///////////
    //// STUDY various jets properties
    //// light jet to make the top hadronic top

    double minDRqq=99999;
    double minDRqq_invMass=-1;
    //    double indexI=-1;
    //    double indexJ=-1;

    for(int i=0;i!=e->Ljets();++i) {
        Jet* ljet = e->GetLjet(i);
        for(int j=0;j!=e->Ljets();++j) {
            if (j==i) continue;
            double dr = ljet->DeltaR(e->GetLjet(j));
            double mass = (ljet->GetP4() + e->GetLjet(j)->GetP4()).M();
            if(dr<minDRqq) { minDRqq=dr; minDRqq_invMass=mass; }
        }
    }

    if(e->Ljets()>1) {
        Fill("ChargedHiggsTopBottom/Baseline/minDRqq_"+label, systname, minDRqq , e->weight() );
        Fill("ChargedHiggsTopBottom/Baseline/minDRqq_mass_"+label, systname, minDRqq_invMass , e->weight() );
    }

    ///////////
    //// STUDY various jets properties
    //// B jets angular separation

    Jet* bjet1 = e->GetBjet(0);
    Jet* bjet2 = e->GetBjet(1);
    if(bjet1 && bjet2) {
        Fill("ChargedHiggsTopBottom/Baseline/DRb1b2_"+label, systname, bjet1->DeltaR(bjet2) , e->weight() );
        Fill2D("ChargedHiggsTopBottom/Baseline/DRVsMassb1b2_"+label,systname,(bjet1->GetP4()+bjet2->GetP4()).M(), bjet1->DeltaR(bjet2) , e->weight() );
        //        Fill2D("ChargedHiggsTauNu/NOne/DPhiJet2MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet2,e->weight());
    }


    ///////$$$$$$

    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/Baseline/LeadingBPt_"+label,systname, e->GetBjet(0)->Pt() ,e->weight());

    if(e->Bjets()>0) {

        double DEtaMax=0.;

        for(int i=0;i!=e->Njets();++i) {
            Jet* jet = e->GetJet(i);
            Jet * bj1 = e->GetBjet(0);
            if(bj1->DeltaEta(*jet)>DEtaMax) DEtaMax=bj1->DeltaEta(*jet);
        }

        Fill("ChargedHiggsTopBottom/Baseline/DEtaMax_"+label, systname, DEtaMax , e->weight() );

    }

}


int ChargedHiggsTopBottom::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsTopBottom]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsTopBottom]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    CutSelector cut;
    cut.SetMask(MaxCut-1);
    cut.SetCutBit(Total);
    Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,0,e->weight());
    Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,0,e->weight());

    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$

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
            //            if(it->Pt()>20 and it->Isolation()<1.5) {
            if(it->Pt()>20) {

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
    if(do1lAnalysis) {
        if ( e->Njets() >= 5 ) cut.SetCutBit(NJets);
    } else {
        if ( e->Njets() >= 3 ) cut.SetCutBit(NJets);
    }

    if ( e->Bjets() >= 2 )  cut.SetCutBit(NB);

    //// 1L CutFlow
    if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,OneLep,e->weight());
    if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoTau,e->weight());
    if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoSecondLep,e->weight());
    if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,Met,e->weight());
    if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,Mt,e->weight());
    if( cut.passAllUpTo(NJets)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NJets,e->weight());
    if( cut.passAllUpTo(NB)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NB,e->weight());

    //// 2L CutFlow
    if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,OneLep,e->weight());
    if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NoTau,e->weight());
    if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NoSecondLep,e->weight());
    if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,Met,e->weight());
    if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,Mt,e->weight());
    if( cut.passAllUpTo(NJets)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NJets,e->weight());
    if( cut.passAllUpTo(NB)    ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlowDilep_"+label,systname,NB,e->weight());

    //// pre-selection N-1

    if( cut.passAllExcept(Met) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1/EtMiss_"+label,systname, e->GetMet().Pt() ,e->weight());
    }

    if( cut.passAllExcept(NJets) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1/Njets_"+label,systname, e->Njets() ,e->weight());
    }

    if( cut.passAllExcept(NB) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1/NBjets_"+label,systname, e->Bjets() ,e->weight());
    }

    if( cut.passAllExcept(NB) ) {
        if(e->Bjets()==1)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,0,e->weight());
        if(e->Bjets()==2)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,1,e->weight());
        if(e->Bjets()>=3)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,2,e->weight());

        if(e->Bjets()==1)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,0,e->weight());
        if(e->Bjets()==2)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,1,e->weight());
        if(e->Bjets()>=3)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,2,e->weight());
    }

    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    //// VARIOUS debugging plots

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
    ////
    ////
    ////   Baseline up to now
    ////
    ////

    if(!cut.passAll() )   return EVENT_NOT_USED;


    bool rightCombination =true; // for all the bkg

    GenParticle *bAss=NULL;
    GenParticle *bFromTopH=NULL;
    GenParticle *bFromTopAss=NULL;
    GenParticle *bFromH=NULL;

    if ( not e->IsRealData() and label.find("Higgs")){

        rightCombination=false; // reset for Higgs
        rightCombination=genInfo(e,bAss,bFromTopH,bFromTopAss,bFromH); // compute the right combination in the higgs case

        ///// GENERATION
        if(bAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbAss_"+label,systname,bAss->GetP4().Pt(),e->weight());
        if(bFromTopH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromTopH_"+label,systname,bFromTopH->GetP4().Pt(),e->weight());
        if(bFromTopAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromTopAss_"+label,systname,bFromTopAss->GetP4().Pt(),e->weight());
        if(bFromH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromH_"+label,systname,bFromH->GetP4().Pt(),e->weight());

        if(bAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabAss_"+label,systname,bAss->GetP4().Eta(),e->weight());
        if(bFromTopH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromTopH_"+label,systname,bFromTopH->GetP4().Eta(),e->weight());
        if(bFromTopAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromTopAss_"+label,systname,bFromTopAss->GetP4().Eta(),e->weight());
        if(bFromH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromH_"+label,systname,bFromH->GetP4().Eta(),e->weight());

    }

    jetPlot(e,label, systname);

    ///////////
    //// TRY to reconstruct the Higgs mass
    ////
    ////

    TLorentzVector p4MET=e->GetMet().GetP4();
    p4MET.SetPz(0);
    p4MET.SetE(sqrt(p4MET.Px()*p4MET.Px() + p4MET.Py()*p4MET.Py()));
    TLorentzVector p4LEP=leadLep->GetP4();
    p4LEP.SetPz(0);
    p4LEP.SetE(sqrt(p4LEP.Px()*p4LEP.Px() + p4LEP.Py()*p4LEP.Py()));

    TLorentzVector p4W=p4MET+p4LEP;

    Fill("ChargedHiggsTopBottom/Baseline/ptW_"+label,systname, p4W.Pt() ,e->weight());

    ///////////
    //// TRY lepton to closest in DR
    ////
    ////

    double minDRbb=99999;
    double maxDRbb=0;
    double minDRbb_invMass=-1;
    double maxDRbb_invMass=-1;
    double indexI=-1;
    double indexJ=-1;

    double indexMaxJ=-1;
    
    for(int i=0;i!=e->Bjets();++i) {
        Jet* bjet = e->GetBjet(i);
        for(int j=0;j!=e->Bjets();++j) {
            if (j==i) continue;
            double dr = bjet->DeltaR(e->GetBjet(j));
            double mass = (bjet->GetP4() + e->GetBjet(j)->GetP4()).M();
            if(dr<minDRbb) { minDRbb=dr; minDRbb_invMass=mass; indexI=i; indexJ=j;}
            if(dr>maxDRbb and i==0) { maxDRbb=dr; maxDRbb_invMass=mass; indexMaxJ=j;}
        }
    }
    
    if(e->Bjets()>1) {
        Fill("ChargedHiggsTopBottom/Baseline/minDRbb_"+label, systname, minDRbb , e->weight() );
        Fill("ChargedHiggsTopBottom/Baseline/minDRbb_mass_"+label, systname, minDRbb_invMass , e->weight() );
    }

    ///////////
    //// VARIOUS Higgs combination

    TLorentzVector p4b1,p4b2;


    //// ===> genInformation

    if ( not e->IsRealData() and label.find("Higgs")){

        if(bFromTopH != NULL) p4b2 = bFromTopH->GetP4();
        if(bFromH != NULL) p4b1 = bFromH->GetP4();

        if(rightCombination) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"trueComb");
        //    GenParticle *bFromTopH=NULL;
        //    GenParticle *bFromH=NULL;

    }

    //// ===> RECO

    Jet * bj1 = e->GetBjet(0);
    Jet * bj2 = e->GetBjet(1);
    Jet * bjMax = e->GetBjet(indexMaxJ);

    if (bj1 != NULL) {
        p4b1=bj1->GetP4();

        //// ===> leadingB + trailing
        if (bj2 != NULL) {
            p4b2=bj2->GetP4();

            if(bj1 != NULL && bj2 != NULL) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBtrailB");
            if(rightCombination) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBtrailB_rightComb");
            if(rightCombination && e->Bjets()==2)  leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBtrailB_rightComb_eq2b");
            if(rightCombination && e->Bjets()>2)  leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBtrailB_rightComb_ge3b");
        }
        //// ===> leadingB + max in DR
        if (bjMax != NULL) {
            p4b2=bjMax->GetP4();
            if(rightCombination) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBmaxDRB_rightComb");
        }
    }

    //// ===> two b closest in DR
    if(indexI>indexJ) {
        Jet * bj1 = e->GetBjet(indexI);
        Jet * bj2 = e->GetBjet(indexJ);
        if (bj1 != NULL) p4b1=bj1->GetP4();
        if (bj2 != NULL) p4b2=bj2->GetP4();
    } else {
        Jet * bj1 = e->GetBjet(indexJ);
        Jet * bj2 = e->GetBjet(indexI);
        if (bj1 != NULL) p4b1=bj1->GetP4();
        if (bj2 != NULL) p4b2=bj2->GetP4();
    }

    if(rightCombination) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"closeDRb_rightComb");

    ///////////
    //// investgate the lepton close to b
    ////
    ////

    int indexMinDRbl1=-1;
    int indexMinDRbl2=-1;
    double minDRbl1=99999;
    double minDRbl2=99999;
    double minDRlb_invMass=-1;

    for(int i=0;i!=e->Bjets();++i) {
        Jet* bjet = e->GetBjet(i);

        if (leadLep != NULL) {
            double dr = leadLep->DeltaR(e->GetBjet(i));
            double mass = (bjet->GetP4() + leadLep->GetP4()).M();
            if(dr<minDRbl1) { minDRbl1=dr; minDRlb_invMass=mass; indexMinDRbl1=i;}
        }

        if (trailLep != NULL) {
            double drTrailLead = trailLep->DeltaR(e->GetBjet(i));
            if(drTrailLead<minDRbl2) { minDRbl2=drTrailLead; indexMinDRbl2=i;}
        }
    }

    Fill("ChargedHiggsTopBottom/Baseline/minDRlb_"+label, systname, minDRbl1 , e->weight() );
    Fill("ChargedHiggsTopBottom/Baseline/minDRlb_mass_"+label, systname, minDRlb_invMass , e->weight() );

    ////
    ////
    ////

    Jet * bjMinL = e->GetBjet(indexMinDRbl1);

    if (bjMinL != NULL) {
        Jet * bj1 = e->GetBjet(0);
        if(bj1 != NULL) p4b1 = bj1->GetP4();
        p4b2 = bjMinL->GetP4();
        if(rightCombination) leptonicHiggs(e,label,systname,p4b1,p4b2,p4W,"leadBtrailB_rightComb_minDRl");
    }


    //    Jet * bjetindexMinDRbl1= e->GetBjet(indexMinDRbl1);
    //    Jet * bjetindexMinDRbl2= e->GetBjet(indexMinDRbl2);

    //    Jet * bj1 = e->GetBjet(0);
    if (bj1 != NULL) {
        float deltaEtalb= bj1->DeltaEta(*leadLep);
        float deltaPhilb= bj1->DeltaPhi(*leadLep);
        float deltaRlb = bj1->DeltaR(*leadLep);
        Fill("ChargedHiggsTopBottom/Baseline/dRlb_"+label, systname, deltaRlb , e->weight() );
        Fill("ChargedHiggsTopBottom/Baseline/dEtalb_"+label, systname, deltaEtalb , e->weight() );
        Fill("ChargedHiggsTopBottom/Baseline/dPhilb_"+label, systname, abs(deltaPhilb) , e->weight() );

        if(trailLep) {
            float deltaEtal2b= bj1->DeltaEta(*trailLep);
            float deltaPhil2b= bj1->DeltaPhi(*trailLep);
            float deltaRl2b = bj1->DeltaR(*trailLep);
            Fill("ChargedHiggsTopBottom/Baseline/dRl2b_"+label, systname, deltaRl2b , e->weight() );
            Fill("ChargedHiggsTopBottom/Baseline/dEtal2b_"+label, systname, deltaEtal2b , e->weight() );
            Fill("ChargedHiggsTopBottom/Baseline/dPhil2b_"+label, systname, abs(deltaPhil2b) , e->weight() );
        }
    }    

    ///////////
    //// TRY to reconstruct the Higgs ---- dilepton
    ////
    ////

    if(trailLep and e->Bjets()>0) {

        TLorentzVector p4LEP2,p4W2;

        p4LEP2=trailLep->GetP4();
        p4LEP2.SetPz(0);
        p4LEP2.SetE(sqrt(p4LEP2.Px()*p4LEP2.Px() + p4LEP2.Py()*p4LEP2.Py()));
        p4W2=p4MET+p4LEP2;

        //// Dilepton
        double minMT=min(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
        double maxMT=max(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
        //            double minMT=min(ChargedHiggs::mtMassive(p4W,bjetindexMinDRl1b),ChargedHiggs::mtMassive(p4W2,p4b1));
        Fill("ChargedHiggsTopBottom/Baseline/minMT_"+label,systname,minMT,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline/maxMT_"+label,systname,maxMT,e->weight());

    }
    /////
    return EVENT_NOT_USED;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
