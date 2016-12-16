#include "interface/AnalysisChargedHiggsTopBottom.hpp"

void ChargedHiggsTopBottom::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetPtCut(10);
    l->SetIsoCut(-1.); // absolute isolation
    l->SetIsoRelCut(0.10); // relative isolation
    l->SetEtaCut(2.4);
    l->SetTightCut(false); // use the loose selection for now
}

void ChargedHiggsTopBottom::SetJetCuts(Jet *j){
    // https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80X
    j->SetBCut(0.800); //0.800 medium // 0.460 loose
    //    j->SetBCut(0.460); //0.800 medium // 0.460 loose
    j->SetEtaCut(4.7);
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(40);
    //j->SetPuIdCut(0.5);
}

void ChargedHiggsTopBottom::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(20);
    t->SetEtaCut(2.4);
    t->SetMuRej(false);
    t->SetEleRej(false);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(2.5);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(0);  // TauDecayModeFindingNewDMs
}

void ChargedHiggsTopBottom::setTree(Event*e, string label, string category )
{

    SetTreeVar("run",e->runNum());
    SetTreeVar("lumi",e->lumiNum());
    SetTreeVar("evt",e->eventNum());
    SetTreeVar("isRealData",e->IsRealData());

    SetTreeVar("weight",e->weight());
    SetTreeVar("npv",e->Npv());
    SetTreeVar("weight_pu",e->GetWeight()->GetBarePUWeight());
    SetTreeVar("weight_bTag",e->GetWeight()->GetSF("btag")->get());
    SetTreeVar("weight_topPt",e->GetWeight()->GetSF("topreweight")->get());
    SetTreeVar("weight_nEv",e->GetWeight()->GetBareNevents());
    SetTreeVar("xsec",e->GetWeight()->GetBareMCXsec());

    SetTreeVar("lep1_pt",leadLep->Pt());
    SetTreeVar("lep1_phi",leadLep->Phi());
    SetTreeVar("lep1_eta",leadLep->Eta());
    SetTreeVar("lep1_charge",leadLep->Charge());
    SetTreeVar("lep1_isolation",leadLep->Isolation());
    if(leadLep->IsMuon()) SetTreeVar("lep1_id",13);
    if(leadLep->IsElectron()) SetTreeVar("lep1_id",11);

    if(trailLep!=NULL) SetTreeVar("lep2_pt",trailLep->Pt());
    if(trailLep!=NULL) SetTreeVar("lep2_phi",trailLep->Phi());
    if(trailLep!=NULL) SetTreeVar("lep2_eta",trailLep->Eta());
    if(trailLep!=NULL) SetTreeVar("lep2_charge",trailLep->Charge());
    if(trailLep!=NULL) SetTreeVar("lep2_isolation",trailLep->Isolation());
    if(trailLep!=NULL && trailLep->IsMuon()) SetTreeVar("lep2_id",13);
    if(trailLep!=NULL && trailLep->IsElectron()) SetTreeVar("lep2_id",11);

    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NcentralJets",e->NcentralJets());
    SetTreeVar("NBJets",e->Bjets());

    for(int i=0;i!=min(e->Njets(),10);++i) {
    //    for(int i=0;i!=10;++i) { // fill only the first 10 jets
        SetTreeVar("jet_pt",i,e->GetJet(i)->GetP4().Pt());
        SetTreeVar("jet_eta",i,e->GetJet(i)->GetP4().Eta());
        SetTreeVar("jet_phi",i,e->GetJet(i)->GetP4().Phi());
        SetTreeVar("jet_discr",i,e->GetJet(i)->bdiscr);
    }

    for(int i=0;i!=min(e->Bjets(),10);++i) {
    //    for(int i=0;i!=10;++i) { // fill only the first 10 jets
        SetTreeVar("bjet_pt",i,e->GetBjet(i)->GetP4().Pt());
        SetTreeVar("bjet_eta",i,e->GetBjet(i)->GetP4().Eta());
        SetTreeVar("bjet_phi",i,e->GetBjet(i)->GetP4().Phi());
        SetTreeVar("bjet_discr",i,e->GetBjet(i)->bdiscr);
    }

    SetTreeVar("met_pt",e->GetMet().Pt());
    SetTreeVar("met_phi",e->GetMet().Phi());
    SetTreeVar("ht",evt_HT);
    SetTreeVar("DRl1b1",evt_DRl1b1);
    SetTreeVar("DRl2b1",evt_DRl2b1);
    SetTreeVar("DRbbmin",evt_minDRbb);

    SetTreeVar("mt",evt_MT);
    SetTreeVar("mt2ll",evt_MT2ll);
    SetTreeVar("mt2bb",evt_MT2bb);
    if(bdt.size()>0) { SetTreeVar("bdt",bdt[0]); } else {SetTreeVar("bdt",-1);}
    SetTreeVar("mtMin",evt_MTmin);
    SetTreeVar("mtMax",evt_MTmax);
    SetTreeVar("mtTot",evt_MTtot);

    //////
    //
    // Fill various event categories
    //

    if (category.find("_1Mu")    !=string::npos) SetTreeVar("lep_category",1);
    if (category.find("_1Ele")   !=string::npos) SetTreeVar("lep_category",2);
    if (category.find("_2Mu")    !=string::npos) SetTreeVar("lep_category",3);
    if (category.find("_2Ele")   !=string::npos) SetTreeVar("lep_category",4);
    if (category.find("_1Mu1Ele")!=string::npos) SetTreeVar("lep_category",5);


    if (label.find("Hplus") !=string::npos)  // SIG
        {
            SetTreeVar("sig",1);
        }
    else{ // BKG
        SetTreeVar("sig",0);
    }

    int mc=0;
    if (label.find("HplusToTB") !=string::npos)  //sig
        {
            mc = 0;
            if (label.find("M-300") !=string::npos) mc += 1;
            if (label.find("M-400") !=string::npos) mc += 2;
            if (label.find("M-500") !=string::npos) mc += 3;
            if (label.find("M-800") !=string::npos) mc += 4;
            if (label.find("M-1000")!=string::npos) mc += 5;
            if (label.find("M-2000")!=string::npos) mc += 6;
            if (label.find("M-3000")!=string::npos) mc += 7;

        } else if (label.find("HplusToTauNu") !=string::npos) //sig TauNu
        {
            mc = 50;
            if (label.find("M-300") !=string::npos) mc += 1;
            if (label.find("M-500") !=string::npos) mc += 3;
            if (label.find("M-800") !=string::npos) mc += 4;
            if (label.find("M-1000")!=string::npos) mc += 5;
            if (label.find("M-2000")!=string::npos) mc += 6;
            if (label.find("M-3000")!=string::npos) mc += 7;

        } else  // bkg
        {
            // ttbar + single top + ttV
            //            mc = 100;
            if(label.find("TTJets_DiLept") !=string::npos) mc =101 ;
            if(label.find("TTJets_SingleLeptFromT") !=string::npos) mc =102 ;
            if(label.find("TTJets_SingleLeptFromTbar") !=string::npos) mc =103 ;

            if(label.find("ST_tW_top") !=string::npos) mc =111 ;
            if(label.find("ST_tW_antitop") !=string::npos) mc =112 ;
            if(label.find("ST_t-channel_top") !=string::npos) mc =113 ;
            if(label.find("ST_t-channel_antitop") !=string::npos) mc =114 ;
            // s channel
            if(label.find("tZq_ll_4f") !=string::npos) mc =115 ;
            if(label.find("TTTT") !=string::npos) mc =116 ;

            // ttH
            if(label.find("TTZToQQ") !=string::npos) mc = 121;
            if(label.find("TTZToLLNuNu") !=string::npos) mc =122 ;
            if(label.find("TTWJetsToQQ") !=string::npos) mc =123 ;
            if(label.find("TTWJetsToLNu") !=string::npos) mc =124 ;
            if(label.find("ttHJetTobb") !=string::npos) mc =125 ;
            if(label.find("ttHJetToNonbb") !=string::npos) mc =126 ;

            // V+jets
            //            mc = 200;
            if(label.find("DYJets-madgraph") !=string::npos) mc =221 ;
            if(label.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8")!=string::npos) mc =222;

            // EWK
            // missing tribosons
            //            mc = 300;
            if(label.find("WWTo2L2Nu") !=string::npos) mc =331 ;
            if(label.find("WWToLNuQQ") !=string::npos) mc =332 ;
            if(label.find("WZTo1L1Nu2Q") !=string::npos) mc =333 ;
            if(label.find("WZTo1L3Nu") !=string::npos) mc =334 ;
            if(label.find("WZTo2L2Q") !=string::npos) mc =335 ;
            if(label.find("WZTo3LNu") !=string::npos) mc =336 ;
            if(label.find("ZZTo2L2Nu") !=string::npos) mc =337 ;
            if(label.find("ZZTo2L2Q") !=string::npos) mc =338 ;
            if(label.find("ZZTo4L") !=string::npos) mc =339 ;
        }

    SetTreeVar("mc",mc);

    SetTreeVar("nGenB",nGenB);
    if(bFromH!= NULL) {
        SetTreeVar("bFromH_pt",bFromH->Pt());
        SetTreeVar("bFromH_eta",bFromH->Eta());
        SetTreeVar("bFromH_phi",bFromH->Phi());
    }

    if(bFromTopH!= NULL) {
        SetTreeVar("bFromTopH_pt",bFromTopH->Pt());
        SetTreeVar("bFromTopH_eta",bFromTopH->Eta());
        SetTreeVar("bFromTopH_phi",bFromTopH->Phi());
    }

    if(leptonFromTopH!= NULL) {
        SetTreeVar("leptonFromTopH_pt",leptonFromTopH->Pt());
        SetTreeVar("leptonFromTopH_eta",leptonFromTopH->Eta());
        SetTreeVar("leptonFromTopH_phi",leptonFromTopH->Phi());
    }


}


void ChargedHiggsTopBottom::AddVariable( string name, char type){
    cout<<"[TmvaAnalysis]::[AddVariable]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddVariable]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }
}//end add variable


void ChargedHiggsTopBottom::Init()
{

    Log(__FUNCTION__,"INFO",Form("do1lAnalysis=%d",do1lAnalysis));
    Log(__FUNCTION__,"INFO",Form("do2lAnalysis=%d",do2lAnalysis));

    Preselection();

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader"<<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    AddVariable("lep1_pt",'F');
    AddVariable("lep2_pt",'F');
    AddVariable("DRbbmin",'F');
    AddVariable("bjet_pt",'F');

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
        {
            cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
            readers_[i]->BookMVA("BDT",weights[i].c_str());
        }
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;

    /////
    /////
    /////


    // fill variables for miniTREE

    InitTree("tree_tb");

    Branch("tree_tb","sig",'I');
    Branch("tree_tb","mc",'I'); // to distinguish between the different mc
    Branch("tree_tb","lep_category",'I'); // to distinguish between the different leptonCategory

    Branch("tree_tb","run",'I');
    Branch("tree_tb","lumi",'I');
    Branch("tree_tb","evt",'I');
    Branch("tree_tb","npv",'I');
    Branch("tree_tb","weight",'D');
    Branch("tree_tb","isRealData",'I');

    Branch("tree_tb","weight_pu",'D');
    Branch("tree_tb","weight_bTag",'D');
    Branch("tree_tb","weight_nEv",'D');
    Branch("tree_tb","weight_topPt",'D');
    Branch("tree_tb","xsec",'F');

    // fill lepton quantities
    Branch("tree_tb","lep1_pt",'F');
    Branch("tree_tb","lep1_phi",'F');
    Branch("tree_tb","lep1_eta",'F');
    Branch("tree_tb","lep1_charge",'F');
    Branch("tree_tb","lep1_isolation",'F');
    Branch("tree_tb","lep1_id",'F');

    Branch("tree_tb","lep2_pt",'F');
    Branch("tree_tb","lep2_phi",'F');
    Branch("tree_tb","lep2_eta",'F');
    Branch("tree_tb","lep2_charge",'F');
    Branch("tree_tb","lep2_isolation",'F');
    Branch("tree_tb","lep2_id",'F');

    // fill counter and scalar
    Branch("tree_tb","NcentralJets",'I');
    Branch("tree_tb","NJets",'I');
    Branch("tree_tb","NBJets",'I');

    // fill event variables
    Branch("tree_tb","met_pt",'F');
    Branch("tree_tb","met_phi",'F');
    Branch("tree_tb","ht",'F');
    Branch("tree_tb","DRl1b1",'F');
    Branch("tree_tb","DRl2b1",'F');
    Branch("tree_tb","DRbbmin",'F');

    // various masses
    Branch("tree_tb","bdt",'F');
    Branch("tree_tb","mt",'F');
    Branch("tree_tb","mt2ll",'F');
    Branch("tree_tb","mt2bb",'F');
    Branch("tree_tb","mtMin",'F');
    Branch("tree_tb","mtMax",'F');
    Branch("tree_tb","mtTot",'F');

    // fill all the object vector
    Branch("tree_tb","jet_pt",'d',10,"NJets");
    Branch("tree_tb","jet_eta",'d',10,"NJets");
    Branch("tree_tb","jet_phi",'d',10,"NJets");
    Branch("tree_tb","jet_discr",'d',10,"NJets");

    // fill all the object vector
    Branch("tree_tb","bjet_pt",'d',10,"NBJets");
    Branch("tree_tb","bjet_eta",'d',10,"NBJets");
    Branch("tree_tb","bjet_phi",'d',10,"NBJets");
    Branch("tree_tb","bjet_discr",'d',10,"NBJets");

    //// VARIOUS gen  INFO

    Branch("tree_tb","nGenB",'I');

    Branch("tree_tb","bFromH_pt",'F');
    Branch("tree_tb","bFromH_phi",'F');
    Branch("tree_tb","bFromH_eta",'F');

    Branch("tree_tb","bFromTopH_pt",'F');
    Branch("tree_tb","bFromTopH_phi",'F');
    Branch("tree_tb","bFromTopH_eta",'F');

    Branch("tree_tb","leptonFromTopH_pt",'F');
    Branch("tree_tb","leptonFromTopH_phi",'F');
    Branch("tree_tb","leptonFromTopH_eta",'F');

}

void ChargedHiggsTopBottom::BookCutFlow(string l, string category)
{

        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Histo CutFlow/Baseline" <<l<<endl;


        // do1lAnalysis
        if(do1lAnalysis && (category.find("_1Mu") || category.find("_1Ele"))) {
            Book(    "ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 1");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 1");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{taus} = 0");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT < 120");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJetsCR+1,"N_{jets} #geq 4");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NBCR+1,"numB>=1");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 5");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");
            //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");

            Book(    "ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 1");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 1");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{taus} = 0");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT < 120");
            //            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NJetsCR+1,"N_{jets} #geq 4");
            //            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NBCR+1,"numB>=1");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 5");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");
            //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");
        }

        // do2lAnalysis
        if(do2lAnalysis && (category.find("_2Mu") || category.find("_2Ele") || category.find("_1Mu1Ele")))  {
            Book(    "ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 2");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 2");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{tau} ??");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT ????");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJetsCR+1,"N_{jets} #eq 2");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NBCR+1,"numB>=1");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 3");
            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");

            //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");
            Book(    "ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(OneLep+1,"N_{lep} >= 2");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NoSecondLep+1,"N_{lep} == 2");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{tau} ??");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Met+1,"MET > 30 GeV");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(Mt+1,"MT ????");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NJetsCR+1,"N_{jets} #eq 2");
            //            GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NBCR+1,"numB>=1");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NJets+1,"N_{jets} #geq 3");
            GetHisto("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+l,"")->GetXaxis()->SetBinLabel(NB+1,"numB>=2");
            //        GetHisto("ChargedHiggsTopBottom/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(B1Pt+1,"B1 p_{T} #gt 100");
        }

        ////Histograms for N-1
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Mt_"+l,"Mt "+l+";M_{T} [GeV]",60,0,300);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/EtMiss_"+l,"EtMiss "+l+";E_{T}^{miss} [GeV]",30,0,300);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 [GeV]",15,0,15);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 [GeV] |#eta| <2.4",10,0,10);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 [GeV] |#eta| >2.4",10,0,10);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 [GeV]",10,0,10);

}


void ChargedHiggsTopBottom::BookHisto(string l, string category, string phasespace)
{

        // Control Plots leptons
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonEta_"+l,"LeptonEta "+l + ";#eta (lepton)",20,-5.,5.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonPt_"+l,"LeptonPt "+l + ";p_{T} (lepton)",50,0.,200.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonIso_"+l,"LeptonIso "+l + ";iso (lepton)",50,0.,100.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailEta_"+l,"LeptonTrailEta "+l + ";#eta (lepton)",20,-5.,5.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailPt_"+l,"LeptonTrailPt "+l + ";p_{T} (lepton)",50,0.,200.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailIso_"+l,"LeptonTrailIso "+l + ";iso (lepton)",50,0.,100.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Mt_"+l,"Mt "+l+";M_{T} [GeV]",60,0,300);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+l,"Met "+l+";MET [GeV]",50,0,1000);

        /// Vertices
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+l,"Vertices "+l + ";Number of vertices",50,0.,50.);

        /////
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 [GeV]",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 |#eta|<=2.4",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);

        /////
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",8000,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR1_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",8000,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR2_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",8000,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR3_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",8000,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR4_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",8000,0,8000);

        // VARIOUS qb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_"+l,"LeadingBPt "+l+";P_{T} (leading B) [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBDiscr_"+l,"LeadingBDiscr "+l+";discr (leading B) [GeV]",50,0,1.5);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBPt_"+l,"NextLeadingBPt "+l+";P_{T} (next leading B) [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBDiscr_"+l,"NextLeadingBDiscr "+l+";discr (next leading B) [GeV]",50,0,1.5);

        // jet ratio
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextOverLeadingBPt_"+l,"NextOverLeadingBPt "+l+";P_{T}^{B2}/P_{T}^{B1} [GeV]",50,0,1);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_OverHT_"+l,"LeadingBPt_OverHT "+l+";P_{T}^{B1}/HT [GeV]",50,0,1);

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMax_"+l,"DEtaMax "+l+";dEta_{j^{i},b^{1}}^{max}",100,0,10);
        // min DRbb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+l,"minDRbb"+l+";dR_{bb}^{min}",50,0,2*TMath::Pi()); // <-- this has signal discrimination
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+l,"minDRbb_mass "+l+";m(bb)^{dR^{min}})",100,0,300);
        // b1b2
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DRb1b2_"+l,"DRb1b2"+l+";dR_{b^{1}b^{2}}",100,0,10);
        //        Book2D( "ChargedHiggsTopBottom/Baseline"+category+"/DRVsMassb1b2_"+l,"DRVsMassb1b2"+l+";mass(b^{1},b^{2});dR(b^{1},b^{2})",100,0,1000,100,0,10);
        // min DRqq
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_"+l,"minDRqq"+l+";dR_{qq}^{min}",100,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_mass_"+l,"minDRqq_mass "+l+";m(qq)^{dR^{min}})",100,0,300);

        // l1 l2
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonM_"+l,"DiLeptonM "+l + ";m(l^{1},l^{2}) (lepton)",50,0.,200.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonPT_"+l,"DiLeptonPT "+l + ";p_{T}(l^{1},l^{2}) (lepton)",50,0.,200.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonDeltaR_"+l,"DiLeptonDeltaR "+l + ";dR_(l^{1},l^{2})",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/MT2ll_"+l,"MT2ll "+l + ";mT2(l^{1},l^{2},met)",100,0.,500.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb_"+l,"MT2bb "+l + ";mT2(b^{1},b^{2},met)",100,0.,500.);

        /////
        // lb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_"+l,"minDRlb"+l+";dR_{lb}^{min}",100,0,10);  // <-- this has signal discrimination
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_mass_"+l,"minDRlb_mass "+l+";m(lb)^{dR^{min}})",100,0,300);
        //        Book("ChargedHiggsTopBottom/Baseline/Rlb_"+l,"Rlb "+l+";R_{lb}",100,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dEtalb_"+l,"dEtalb "+l+";dEta_{l^{1},b^{1}}",50,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dPhilb_"+l,"dPhilb "+l+";dPhi_{l^{1},b^{1}}",50,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dRlb_"+l,"dRlb "+l+";dR_{l^{1},b^{1}}",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dEtal2b_"+l,"dEtal2b "+l+";dEta_{l^{2},b^{1}}",50,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dPhil2b_"+l,"dPhil2b "+l+";abs(dPhi_{l^{2},b^{1}})",50,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dRl2b_"+l,"dRl2b "+l+";dR_{l^{2},b^{1}}",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptW_"+l,"ptW "+l+";pT_{l,MET}",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptWb_"+l,"ptWb "+l+";pT_{W,b}",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/PhiWb_"+l,"dPhiWb "+l+";abs(dPhi_{Wb})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/mtWb1_"+l,"mtWb1 "+l+";mT_{b1,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptWb1_"+l,"ptWb1 "+l+";pT_{b1,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minMT_"+l,"minMT "+l+";minMT [ (b1,(l1,MET)),(b1,(l2,MET)) ] ",50,0,1000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/maxMT_"+l,"maxMT "+l+";maxMT [ (b1,(l1,MET)),(b1,(l2,MET)) ]",50,0,1000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/totMT_"+l,"totMT "+l+";totMT [ MT(b1,(l1,MET))^2 + MT(b1,(l2,MET))^2 ]",50,0,1000);

        //        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Filling Histo CutFlow/Baseline" <<l<<endl;

}

void ChargedHiggsTopBottom::Preselection()
{

    for ( string l : AllLabel()  ) {

        BookCutFlow(l, "");
        if(do1lAnalysis) BookCutFlow(l, "_1Mu");
        if(do1lAnalysis) BookCutFlow(l, "_1Ele");
        if(do2lAnalysis) BookCutFlow(l, "_2Mu");
        if(do2lAnalysis) BookCutFlow(l, "_1Mu1Ele");
        if(do2lAnalysis) BookCutFlow(l, "_2Ele");

        BookHisto(l, "","Baseline"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","Baseline");
        if(do1lAnalysis) BookHisto(l, "_1Ele","Baseline");
        if(do2lAnalysis) BookHisto(l, "_2Mu","Baseline");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","Baseline");
        if(do2lAnalysis) BookHisto(l, "_2Ele","Baseline");

        BookHisto(l, "","topCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","topCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","topCR");
        if(do2lAnalysis) BookHisto(l, "_2Mu","topCR");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","topCR");
        if(do2lAnalysis) BookHisto(l, "_2Ele","topCR");

        BookHisto(l, "","extraRadCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","extraRadCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_2Mu","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_2Ele","extraRadCR");

        BookHisto(l, "","charmCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","charmCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","charmCR");
        if(do2lAnalysis) BookHisto(l, "_2Mu","charmCR");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","charmCR");
        if(do2lAnalysis) BookHisto(l, "_2Ele","charmCR");

        BookHisto(l, "","lowMTCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","lowMTCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Mu","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Ele","lowMTCR");

        BookHisto(l, "","highMTCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","highMTCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Mu","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Ele","highMTCR");

        /////

        /*
        Book(    "ChargedHiggsTopBottom/CutFlow/SROnelep_"+ l  , "" ,10,-.5,10-.5);
        Book(    "ChargedHiggsTopBottom/CutFlow/SRDilep_"+ l  , "" ,10,-.5,10-.5);
        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(1,"numB==1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(2,"numB==2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SROnelep_"+l,"")->GetXaxis()->SetBinLabel(3,"numB>=3");

        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(1,"numB==1");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(2,"numB==2");
        GetHisto("ChargedHiggsTopBottom/CutFlow/SRDilep_"+l,"")->GetXaxis()->SetBinLabel(3,"numB>=3");

        //
        Book("ChargedHiggsTopBottom/Preselection1Lep/TauPt_"+l,"TauPt "+l+";P_{T}^{#tau} [GeV]",100,0,200);
        Book("ChargedHiggsTopBottom/Preselection1Lep/RLepTau_"+l,"Rlb "+l+";dR_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/RMuTau_"+l,"RMub "+l+";dR_{#mu,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/REleTau_"+l,"REleb "+l+";dR_{e,b}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/PhiLepTau_"+l,"dPhilb "+l+";dPhi_{lb}",100,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/Preselection1Lep/EtaLepTau_"+l,"dEtalb "+l+";dEta_{lb}",100,0,10);
        */

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

int ChargedHiggsTopBottom::genInfoForBKG(Event*e) {

    int countB=0;

    for(Int_t i = 0; i < e->NGenPar(); i++) {
        GenParticle *genpar = e->GetGenParticle(i);
        if(abs(genpar->GetPdgId()) == 5) countB++;
    }

}


bool ChargedHiggsTopBottom::genInfoForSignal(Event*e) {

    // FIXIME:
    // this is tested for W->lep, need to update for W->tau->lep

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
                    leptonFromTopH=genpar;
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

void ChargedHiggsTopBottom::leptonPlot(Event*e, string label, string category, string systname, string phasespace) {

    bool filldo1l=(do1lAnalysis && ( (category.find("_1Mu")  !=string::npos ) || ( category.find("_1Ele") !=string::npos ) ));
    bool filldo2l=(do2lAnalysis && ( (category.find("_2Mu")  !=string::npos ) || ( category.find("_2Ele") !=string::npos ) || ( category.find("_1Mu1Ele")  !=string::npos ) ));

    if(not (filldo1l || filldo2l)) return;

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonPt_"+label,systname,leadLep->GetP4().Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonEta_"+label,systname,leadLep->GetP4().Eta(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonIso_"+label,systname,leadLep->Isolation(),e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ptW_"+label,systname, (e->GetMet().GetP4()+leadLep->GetP4()).Pt(), e->weight() );
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Mt_"+label,systname, evt_MT, e->weight() );
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+label,systname, e->GetMet().GetP4().Pt(), e->weight() );

    if(do1lAnalysis) return;
    if(trailLep==NULL) return;

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailPt_"+label,systname,trailLep->GetP4().Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailEta_"+label,systname,trailLep->GetP4().Eta(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailIso_"+label,systname,trailLep->Isolation(),e->weight());

    evt_MT2ll=ChargedHiggs::mt2(leadLep->GetP4(),trailLep->GetP4(),e->GetMet().GetP4());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MT2ll_"+label, systname, evt_MT2ll , e->weight() );

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonM_"+label,systname,(trailLep->GetP4() + leadLep->GetP4()).M(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonPT_"+label,systname,(trailLep->GetP4() + leadLep->GetP4()).Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonDeltaR_"+label,systname,leadLep->DeltaR(*trailLep),e->weight());

    if(e->Bjets()==0) return;

    TLorentzVector p4W = e->GetMet().GetP4()+leadLep->GetP4();
    TLorentzVector p4W2 = e->GetMet().GetP4()+trailLep->GetP4();

    evt_MTmin=min(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
    evt_MTmax=max(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
    evt_MTtot=sqrt(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4())*ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4())  +
                   ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4())*ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4())
                   );
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minMT_"+label,systname,evt_MTmin,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/maxMT_"+label,systname,evt_MTmax,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/totMT_"+label,systname,evt_MTtot,e->weight());

}

void ChargedHiggsTopBottom::jetPlot(Event*e, string label, string category, string systname, string phasespace) {

    bool filldo1l=(do1lAnalysis && ( (category.find("_1Mu")  !=string::npos ) || ( category.find("_1Ele") !=string::npos ) ));
    bool filldo2l=(do2lAnalysis && ( (category.find("_2Mu")  !=string::npos ) || ( category.find("_2Ele") !=string::npos ) || ( category.find("_1Mu1Ele")  !=string::npos ) ));

    if(not (filldo1l || filldo2l)) return;


    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+label,systname,e->Npv(),e->weight());


    ///////////
    //// STUDY various jets properties
    //// forward vs central

    evt_HT=0;

    for(int i=0;i!=e->Njets();++i) {
        evt_HT += e->GetJet(i)->GetP4().Pt();
    }

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+label,systname, e->NcentralJets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+label,systname, e->NforwardJets() ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+label,systname, e->Bjets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+label,systname, e->Njets() ,e->weight());

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
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_"+label, systname, minDRqq , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_mass_"+label, systname, minDRqq_invMass , e->weight() );
    }

    ///////////
    //// STUDY various jets properties
    //// B jets angular separation

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

    evt_minDRbb=minDRbb;

    if(e->Bjets()>1) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+label, systname, minDRbb , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+label, systname, minDRbb_invMass , e->weight() );
        evt_MT2bb=ChargedHiggs::mt2(e->GetBjet(0)->GetP4(),e->GetBjet(1)->GetP4(),e->GetMet().GetP4());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb_"+label, systname, evt_MT2bb , e->weight() );

        Jet* bjet1 = e->GetBjet(0);
        Jet* bjet2 = e->GetBjet(1);
        if(bjet1 && bjet2) {
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DRb1b2_"+label, systname, bjet1->DeltaR(bjet2) , e->weight() );
            //        Fill2D("ChargedHiggsTopBottom/Baseline/DRVsMassb1b2_"+label,systname,(bjet1->GetP4()+bjet2->GetP4()).M(), bjet1->DeltaR(bjet2) , e->weight() );
            //        Fill2D("ChargedHiggsTauNu/NOne/DPhiJet2MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet2,e->weight());
        }

    }

    ///////$$$$$$
    ///////$$$$$$
    ///////$$$$$$

    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_"+label,systname, e->GetBjet(0)->Pt() ,e->weight());
    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBDiscr_"+label,systname, e->GetBjet(0)->bdiscr ,e->weight());

    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBPt_"+label,systname, e->GetBjet(1)->Pt() ,e->weight());
    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBDiscr_"+label,systname, e->GetBjet(1)->bdiscr ,e->weight());

    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NextOverLeadingBPt_"+label,systname, e->GetBjet(1)->Pt()/e->GetBjet(0)->Pt() ,e->weight());
    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_OverHT_"+label,systname, e->GetBjet(0)->Pt()/evt_HT ,e->weight());

    if(e->Bjets()>0) {

        double DEtaMax=0.;

        for(int i=0;i!=e->Njets();++i) {
            Jet* jet = e->GetJet(i);
            Jet * bj1 = e->GetBjet(0);
            if(bj1->DeltaEta(*jet)>DEtaMax) DEtaMax=bj1->DeltaEta(*jet);
        }

        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMax_"+label, systname, DEtaMax , e->weight() );

    }



    ///////$$$$$$
    ///////$$$$$$
    ///////$$$$$$


    Jet * bj1 = e->GetBjet(0);
    if (bj1 != NULL) {
        float deltaEtalb= bj1->DeltaEta(*leadLep);
        float deltaPhilb= bj1->DeltaPhi(*leadLep);
        float deltaRlb = bj1->DeltaR(*leadLep);
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dRlb_"+label, systname, deltaRlb , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtalb_"+label, systname, deltaEtalb , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhilb_"+label, systname, abs(deltaPhilb) , e->weight() );

        evt_DRl1b1=deltaRlb;

        if(trailLep) {
            float deltaEtal2b= bj1->DeltaEta(*trailLep);
            float deltaPhil2b= bj1->DeltaPhi(*trailLep);
            float deltaRl2b = bj1->DeltaR(*trailLep);
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dRl2b_"+label, systname, deltaRl2b , e->weight() );
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtal2b_"+label, systname, deltaEtal2b , e->weight() );
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhil2b_"+label, systname, abs(deltaPhil2b) , e->weight() );

            evt_DRl2b1=deltaRl2b;
        }


    }



}


int ChargedHiggsTopBottom::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsTopBottom]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsTopBottom]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$
    ////$$$$$$$

    //The event selection should be implemented here for details see src/AnalysisChargedHiggsTauNu.cpp

    Object* sub = NULL;

    //    int nGoodLepton=0;
    int nOSLepPair=0;

    leadLep=NULL;
    trailLep=NULL;
    Lepton* mu=NULL; // first muon
    Lepton* ele=NULL; // first electrons
    Tau *t = e->GetTau(0);

    double LeadingLeptonPt_= 30; // singleLepton
    double NextLeadingLeptonPt_= 10; //

    for(int i=0;i!=e->Nleps();++i) {
        Lepton *it = e->GetLepton(i);

        //        if(it->Pt()<NextLeadingLeptonPt_) continue; // this is always >10
        if(it->Pt()>LeadingLeptonPt_ and it->IsTight() and leadLep==NULL ) {

            leadLep = it;
            if(it->IsMuon()) mu = it;
            if(it->IsElectron()) ele = it;

        }
        // found a second lepton OS and mll>12 GeV and abs(mll-91)<15
        // mll>50 since we do not have processed the MC DY M10-50
        if(leadLep!=NULL and nOSLepPair==0) {
            if( it->Charge()*leadLep->Charge()<0 and (it->GetP4() + leadLep->GetP4()).M() > 12 ) {
                if((it->IsMuon() && leadLep->IsMuon()) || (it->IsElectron() && leadLep->IsElectron()) ) {
                    // same flavour
                    if( (std::abs((it->GetP4() + leadLep->GetP4()).M()-91) > 15) && (it->GetP4() + leadLep->GetP4()).M()>50 ) { trailLep=it; nOSLepPair++; }
                } else { trailLep=it; nOSLepPair++; }
            } // and OS
        }
    }// end loop on lepton

    int nTau=0;
    if(leadLep!=NULL and nOSLepPair==0) {
        for(int i=0;i!=e->Ntaus();++i) {
            Tau *it = e->GetTau(i);
            if(it->DeltaR(*leadLep)>0.4) nTau++;
        }
    }


    ////$$$$$$$$$$$$$$$$$$$$$
    ////$$$$$$$$$$$$$$$$$$$$$
    ////$$$$$$$
    ////$$$$$$$ Building categories
    ////$$$$$$$

    bool passTriggerMu=true;
    bool passTriggerEle=true;

    if (e->IsRealData() and
        e->GetName().find("SingleMuon")!=string::npos) passTriggerMu=(e->IsTriggered("HLT_IsoMu27_v")
                                                                      //                                        or e->IsTriggered("HLT_IsoTkMu22_v")
                                                                      //                                        or e->IsTriggered("HLT_IsoMu22_v")
                                                                      or e->IsTriggered("HLT_IsoMu24_v")
                                                                      //                                        or e->IsTriggered("HLT_IsoTkMu24_v")
                                                                      );
    if (e->IsRealData() and
        e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPLoose_Gsf_v")
                                                                           //                                         or e->IsTriggered("HLT_Ele27_WPTight_Gsf_v")
                                                                           //                                         or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v")
                                                                           );


    CutSelector cut;
    cut.SetMask(MaxCut-1);
    cut.SetCutBit(Total);
    if(do1lAnalysis) {
        Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_1Mu/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_1Ele/CutFlow_"+label,systname,0,e->weight());
        ///
        if(!e->IsRealData()) {
            Fill("ChargedHiggsTopBottom/CutFlowNoWei/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_1Mu/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_1Ele/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
        }
    }
    if(do2lAnalysis) {
        Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_2Mu/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_1Mu1Ele/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_2Ele/CutFlow_"+label,systname,0,e->weight());
        ///
        if(!e->IsRealData()) {
            Fill("ChargedHiggsTopBottom/CutFlowNoWei/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_2Mu/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_1Mu1Ele/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_2Ele/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
        }
    }

    string category="";
    evt_MT=e->Mt(Event::MtLepton);

    ////// --> 1l analysis
    if(do1lAnalysis && leadLep!=NULL) {

        bool onemu=(leadLep->IsMuon() and passTriggerMu);
        bool oneele=(leadLep->IsElectron() and passTriggerEle);

        if ( leadLep->GetP4().Pt()>LeadingLeptonPt_ && (onemu or oneele)) {
            cut.SetCutBit(OneLep); // one lep
        }
        if( cut.passAllUpTo(OneLep) ) {
            if(onemu) {
                category="_1Mu";
                //                if (not e->IsRealData()) e->SetPtEtaSF("mureco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("mureco");
            }
            if(oneele) {
                category="_1Ele";
                //                if (not e->IsRealData()) e->SetPtEtaSF("elereco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("elereco");
            }
        }

        if ( e->Nleps() == 1 ) cut.SetCutBit(NoSecondLep); // only one lep <--- kill ttbar dilepton ttbar
        if ( nTau == 0 ) cut.SetCutBit(NoTau); // tau Veto <--- dilepton killer

    }

    ////// --> 2l analysis
    if(do2lAnalysis and leadLep!=NULL and trailLep!=NULL) {

        bool twomu=(leadLep->IsMuon() and trailLep->IsMuon() and passTriggerMu);
        bool twoele=(leadLep->IsElectron() and trailLep->IsElectron() and passTriggerEle);
        bool onemuoneele=(((leadLep->IsElectron() and trailLep->IsMuon() and trailLep->GetP4().Pt()>LeadingLeptonPt_) ||
                           (trailLep->IsElectron() and leadLep->IsMuon())) and passTriggerMu);


        if ( nOSLepPair == 1 and leadLep->GetP4().Pt()>LeadingLeptonPt_ and (twomu or twoele or onemuoneele) ) {
            cut.SetCutBit(OneLep); // one OS lepton Pair + leading above trigger threshould
        }
        if( cut.passAllUpTo(OneLep) ) {
            if(twomu) {
                category="_2Mu";
                //                if (not e->IsRealData()) e->SetPtEtaSF("mureco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("mureco");
                //                if (not e->IsRealData()) e->SetPtEtaSF("mureco",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("mureco");
            }
            if(twoele) {
                category="_2Ele";
                //                if (not e->IsRealData()) e->SetPtEtaSF("elereco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("elereco");
                //                if (not e->IsRealData()) e->SetPtEtaSF("elereco",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("elereco");
            }
            // this 1Mu1Ele muon above the trigger threshould otherwise bias in the turnon
            if(onemuoneele) {
                category="_1Mu1Ele";
                /*
                if(leadLep->IsElectron() and trailLep->IsMuon()) {
                    if (not e->IsRealData()) e->SetPtEtaSF("elereco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("elereco");
                    if (not e->IsRealData()) e->SetPtEtaSF("mureco",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("mureco");
                }
                if(leadLep->IsMuon() and trailLep->IsElectron()) {
                    if (not e->IsRealData()) e->SetPtEtaSF("mureco",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("mureco");
                    if (not e->IsRealData()) e->SetPtEtaSF("elereco",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("elereco");
                }
                */

            }
        }

        if ( e->Nleps() == 2 ) cut.SetCutBit(NoSecondLep); // only two lep <--- kill multiboson/ttV
        if ( true ) cut.SetCutBit(NoTau); // tau Veto -- not needed

    }

    ////// --> other cuts
    if ( e->GetMet().Pt() >= 30 ) cut.SetCutBit(Met);

    if(do1lAnalysis && leadLep!=NULL) {
        cut.SetCutBit(Mt); // Mt -- not applied put in the various lepton categories
        //        if ( e->Mt(evt_MT<120) cut.SetCutBit(Mt); // dilepton Killer
    } else {
        cut.SetCutBit(Mt); // Mt -- not needed
    }
    if(do1lAnalysis) {
        if ( e->NcentralJets() >= 5 ) cut.SetCutBit(NJets);
    } else {
        if ( e->NcentralJets() >= 3 ) cut.SetCutBit(NJets);
    }
    if ( e->Bjets() >= 2 )  cut.SetCutBit(NB);

    ////$$$$$$$
    ////$$$$$$$ Apply SF
    ////$$$$$$$

    if (not e->IsRealData() and
        ((label.find("TTJets_DiLept")!=string::npos) || (label.find("TTJets_SingleLeptFromT")!=string::npos) || (label.find("TTJets_SingleLeptFromTbar")!=string::npos))) {
            e->ApplyTopReweight();
    }

    ////$$$$$$$
    ////$$$$$$$ Fill Cut Flow
    ////$$$$$$$

    //// 1L CutFlow
    if(do1lAnalysis) {
        if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,OneLep,e->weight());
        if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NoSecondLep,e->weight());
        if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NoTau,e->weight());
        if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,Met,e->weight());
        if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,Mt,e->weight());
        if( cut.passAllUpTo(NJets)       ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NJets,e->weight());
        ///
        if(!e->IsRealData()) {
            if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,OneLep,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NoSecondLep,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NoTau,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,Met,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,Mt,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NJets)       ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NJets,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
        }
    }

    if(do2lAnalysis ) {
        //// 2L CutFlow
        if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,OneLep,e->weight());
        if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NoSecondLep,e->weight());
        if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NoTau,e->weight());
        if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,Met,e->weight());
        if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,Mt,e->weight());
        if( cut.passAllUpTo(NJets)       ) Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NJets,e->weight());
        ///
        if(!e->IsRealData()) {
            if( cut.passAllUpTo(OneLep)      ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,OneLep,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NoSecondLep) ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NoSecondLep,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NoTau)       ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NoTau,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(Met)         ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,Met,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(Mt)          ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,Mt,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            if( cut.passAllUpTo(NJets)       ) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NJets,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
        }
    }

    if (not e->IsRealData()) {
        e->ApplyBTagSF(1);// 0=loos wp  1=medium wp
        //        e->ApplyBTagSF(0);// 0=loos wp  1=medium wp
    }

    if( cut.passAllUpTo(NB) && do1lAnalysis ) {
        Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NB,e->weight());
        if(!e->IsRealData()) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NB,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
    }
    if( cut.passAllUpTo(NB) && do2lAnalysis ) {
        Fill("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+label,systname,NB,e->weight());
        if(!e->IsRealData()) Fill("ChargedHiggsTopBottom/CutFlowNoWei"+category+"/CutFlowNoWei_"+label,systname,NB,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
    }
    ////$$$$$$$
    ////$$$$$$$ pre-selection N-1
    ////$$$$$$$

    if( cut.passAllExcept(Mt) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/Mt_"+label,systname, e->Mt(Event::MtLepton) , e->weight());
    }

    if( cut.passAllExcept(Met) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/EtMiss_"+label,systname, e->GetMet().Pt() ,e->weight());
    }

    if( cut.passAllExcept(NJets) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/Njets_"+label,systname, e->Njets() ,e->weight());
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/Nforwardjets_"+label,systname, e->NforwardJets() ,e->weight());
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/Ncentraljets_"+label,systname, e->NcentralJets() ,e->weight());
    }

    if( cut.passAllExcept(NB) ) {
        Fill("ChargedHiggsTopBottom/PreselectionN1"+category+"/NBjets_"+label,systname, e->Bjets() ,e->weight());
    }

    /*
    if( cut.passAllExcept(NB) ) {
        if(e->Bjets()==1)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,0,e->weight());
        if(e->Bjets()==2)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,1,e->weight());
        if(e->Bjets()>=3)     Fill("ChargedHiggsTopBottom/CutFlow/SROnelep_"+label,systname,2,e->weight());

        if(e->Bjets()==1)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,0,e->weight());
        if(e->Bjets()==2)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,1,e->weight());
        if(e->Bjets()>=3)     Fill("ChargedHiggsTopBottom/CutFlow/SRDilep_"+label,systname,2,e->weight());
    }
    */

    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    ////////
    //// VARIOUS debugging plots

    /*
    if(do1lAnalysis && leadLep != NULL && cut.passAllExcept(NoTau)) {

        if(t!=NULL) {
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
    */


    ////////
    //// UP TO NOW: LEPTONS selection only
    ////

    if(!cut.passAllUpTo(Mt) )   return EVENT_NOT_USED;

    ////////
    ////
    //// Fill tree
    ////

    if( e->Bjets() > 0 && ( ( do1lAnalysis && e->NcentralJets() >3 ) || ( do2lAnalysis && e->NcentralJets() >1 ))) {
        setTree(e,label,category);
        FillTree("tree_tb");
    }

    ////////
    ////
    //// Control regions
    ////

    bool charmCR=false;
    bool extraRadCR=false;
    bool topCR=false;
    bool highMTCR=false;
    bool lowMTCR=false;

    // 4jets, ==3b
    if( do1lAnalysis && e->NcentralJets() >= 4 && e->Bjets() == 3 ) charmCR=true;
    if( do1lAnalysis && e->NcentralJets() >= 5 && e->Bjets() == 1 ) extraRadCR=true;
    if( do2lAnalysis && e->NcentralJets() >= 3 && e->Bjets() == 1 ) extraRadCR=true;

    if( do1lAnalysis && e->NcentralJets() == 4 && e->Bjets() > 0 ) topCR=true;
    if( do2lAnalysis && e->NcentralJets() == 2 && e->Bjets() > 0 ) topCR=true;

    if( do1lAnalysis && evt_MT>=120 ) highMTCR=true;
    if( do1lAnalysis && evt_MT<120 ) lowMTCR=true;

    if(topCR) jetPlot(e, label, category, systname,"topCR");
    if(topCR) leptonPlot(e,label, category, systname,"topCR");

    if(extraRadCR) jetPlot(e, label, category, systname,"extraRadCR");
    if(extraRadCR) leptonPlot(e, label, category, systname,"extraRadCR");
    if(charmCR) jetPlot(e, label, category, systname,"charmCR");
    if(charmCR) leptonPlot(e, label, category, systname,"charmCR");

    if(topCR && highMTCR) jetPlot(e, label, category, systname,"highMTCR");
    if(topCR && highMTCR) leptonPlot(e, label, category, systname,"highMTCR");
    if(topCR && lowMTCR) jetPlot(e, label, category, systname,"lowMTCR");
    if(topCR && lowMTCR) leptonPlot(e, label, category, systname,"lowMTCR");

    ////////
    ////
    //// UP TO NOW: Baseline selection
    ////

    if(!cut.passAll() )   return EVENT_NOT_USED;

    jetPlot(e, label, category, systname,"Baseline");
    leptonPlot(e, label, category, systname,"Baseline");

    if( do1lAnalysis and evt_MT<120  and e->Bjets()==2 and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT>=120 and e->Bjets()==2 and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT<120  and e->Bjets()>2  and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR3_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT>=120 and e->Bjets()>2  and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR4_"+label,systname, evt_HT ,e->weight());

    if( do2lAnalysis and e->Bjets()==2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
    if( do2lAnalysis and e->Bjets()>2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());

    /*
    std::cout << "=======================================" << endl;

    std::cout << "run=" << e->runNum() << " lumi=" << e->lumiNum() << " evt=" << e->eventNum();
    std::cout << " leadLep->Pt()=" << leadLep->Pt();
    std::cout << " leadLep->Eta()=" << leadLep->Eta();
    std::cout << " leadLep->Phi()=" << leadLep->Phi();
    std::cout << " leadLep->IsMuon()=" << leadLep->IsMuon() << " leadLep->IsElectron()=" << leadLep->IsElectron();
    std::cout << " nCentralJets=" << e->NcentralJets();
    std::cout << " nTaus=" << e->Ntaus();
    if(e->Ntaus()>0) std::cout << " tauPt=" << e->GetTau(0)->Pt() << " tauEta=" << e->GetTau(0)->Eta() << " tauPhi=" << e->GetTau(0)->Phi();
    if(e->Ntaus()>1) std::cout << " tauPt=" << e->GetTau(1)->Pt() << " tauEta=" << e->GetTau(1)->Eta() << " tauPhi=" << e->GetTau(1)->Phi();
    std::cout << " totalweight=" << e->weight() ;
    std::cout << " systname=" << systname << std::endl;

    for(int i=0;i!=min(e->Njets(),10);++i) {
        std::cout << "    pt[" <<i<<"]="<< e->GetJet(i)->GetP4().Pt() << " eta[" <<i<<"]="<< e->GetJet(i)->GetP4().Eta() << " phi[" <<i<<"]="<< e->GetJet(i)->GetP4().Phi();
    }

    std::cout << "=======================================" << std::endl;
    */
    ////////
    ////
    //// Read MVA
    ////

    if(do2lAnalysis)  {
        SetVariable("lep1_pt",leadLep->Pt());
        SetVariable("lep2_pt",trailLep->Pt());
        SetVariable("DRbbmin",evt_minDRbb);
        SetVariable("bjet_pt",e->GetBjet(0)->GetP4().Pt());

        //    vector<float> bdt;
        for(unsigned i =0 ;i< readers_.size() ; ++i)
            {
                bdt.push_back(readers_[i]->EvaluateMVA("BDT") );
            }

        if(bdt.size()>0) cout << " bdt[0]" << bdt[0] << endl;
    } else {
        bdt.push_back(-1);
    }


    ////
    ////

    if ( not e->IsRealData() ) {
        nGenB = genInfoForBKG(e);
    }

    bool rightCombination =true; // for all the bkg

    if ( not e->IsRealData() and (label.find("Higgs")  !=string::npos ) ){

        rightCombination=false; // reset for Higgs
        rightCombination=genInfoForSignal(e); // compute the right combination in the higgs case

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
    

    ///////////
    //// VARIOUS Higgs combination

    TLorentzVector p4b1,p4b2;
    TLorentzVector p4W = e->GetMet().GetP4()+leadLep->GetP4();


    //// ===> genInformation

    if ( not e->IsRealData() and (label.find("Higgs")  !=string::npos) ){

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

    Fill("ChargedHiggsTopBottom/Baseline"+category+"/minDRlb_"+label, systname, minDRbl1 , e->weight() );
    Fill("ChargedHiggsTopBottom/Baseline"+category+"/minDRlb_mass_"+label, systname, minDRlb_invMass , e->weight() );

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
