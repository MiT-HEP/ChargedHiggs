#include "interface/AnalysisChargedHiggsTopBottom.hpp"

void ChargedHiggsTopBottom::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetPtCut(10);
    l->SetIsoCut(-1.); // absolute isolation
    //https://indico.cern.ch/event/594396/contributions/2402538/attachments/1389409/2116003/20161215_MuonPOG_GeneralPPD.pdf
    //l->SetIsoRelCut(0.25); // relative isolation // for muon 0.25 is loose, 0.15 is tight
    //l->SetMiniIsoRelCut(-1); // relative mini-isolation
    l->SetIsoRelCut(-1); // relative isolation
    l->SetMiniIsoRelCut(0.40); // relative mini-isolation  // 0.1 is tight and 0.4 is for loose
    l->SetEtaCut(2.4);
    l->SetVetoCut(); // loosest selection of them all
    l->SetTightCut(false); // use the loose selection for now
    l->SetMvaLooseCut(true);
}

void ChargedHiggsTopBottom::SetJetCuts(Jet *j){
    // https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80X
    if(doICHEP) j->SetBCut(0.800); //0.800 medium // 0.460 loose
    //    j->SetBCut(0.460); //0.800 medium // 0.460 loose
    // https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80XReReco
    // doSynch
    if(!doICHEP) j->SetBCut(0.8484); //0.8484 medium // 0.5426 loose
    j->SetEtaCut(4.7);
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(40);
    //    j->SetPtCut(30); // for MIAO
    j->SetPuIdCut(-100);
}

void ChargedHiggsTopBottom::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(20);
    t->SetEtaCut(2.3);
    t->SetMuRej(false);
    t->SetEleRej(false);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // NOW byVLooseIsolationMVArun2v1DBoldDMwLT, BEFORE LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(-1);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(1);  // 0=TauDecayModeFindingNewDMs 1=TauDecayModeFinding
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
    SetTreeVar("weight_bTagCSV",e->GetWeight()->GetSF("btag-reweight")->get());
    SetTreeVar("weight_bTag",e->GetWeight()->GetSF("btag")->get());
    SetTreeVar("weight_topPt",e->GetWeight()->GetSF("topreweight")->get());
    SetTreeVar("weight_nEv",e->GetWeight()->GetBareNevents());
    SetTreeVar("xsec",e->GetWeight()->GetBareMCXsec());

    SetTreeVar("lep1_pt",leadLep->Pt());
    SetTreeVar("lep1_phi",leadLep->Phi());
    SetTreeVar("lep1_eta",leadLep->Eta());
    //    SetTreeVar("lep1_charge",leadLep->Charge());
    //    SetTreeVar("lep1_isolation",leadLep->Isolation());
    if(leadLep->IsMuon()) SetTreeVar("lep1_id",13*leadLep->Charge());
    if(leadLep->IsElectron()) SetTreeVar("lep1_id",11*leadLep->Charge());

    if(trailLep!=NULL) SetTreeVar("lep2_pt",trailLep->Pt());
    if(trailLep!=NULL) SetTreeVar("lep2_phi",trailLep->Phi());
    if(trailLep!=NULL) SetTreeVar("lep2_eta",trailLep->Eta());
    //    if(trailLep!=NULL) SetTreeVar("lep2_charge",trailLep->Charge());
    //    if(trailLep!=NULL) SetTreeVar("lep2_isolation",trailLep->Isolation());
    if(trailLep!=NULL && trailLep->IsMuon()) SetTreeVar("lep2_id",13*trailLep->Charge());
    if(trailLep!=NULL && trailLep->IsElectron()) SetTreeVar("lep2_id",11*trailLep->Charge());

    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NcentralJets",e->NcentralJets());
    SetTreeVar("NBJets",e->Bjets());

    for(int i=0;i!=min(e->Njets(),10);++i) {
    //    for(int i=0;i!=10;++i) { // fill only the first 10 jets
        SetTreeVar("jet_pt",i,e->GetJet(i)->Pt());
        SetTreeVar("jet_eta",i,e->GetJet(i)->Eta());
        SetTreeVar("jet_phi",i,e->GetJet(i)->Phi());
        SetTreeVar("jet_e",i,e->GetJet(i)->E());
        SetTreeVar("jet_discr",i,e->GetJet(i)->bdiscr);
    }

    for(int i=0;i!=min(e->Bjets(),10);++i) {
    //    for(int i=0;i!=10;++i) { // fill only the first 10 jets
        SetTreeVar("bjet_pt",i,e->GetBjet(i)->Pt());
        SetTreeVar("bjet_eta",i,e->GetBjet(i)->Eta());
        SetTreeVar("bjet_phi",i,e->GetBjet(i)->Phi());
        SetTreeVar("bjet_e",i,e->GetBjet(i)->E());
        SetTreeVar("bjet_discr",i,e->GetBjet(i)->bdiscr);
    }

    SetTreeVar("met_pt",e->GetMet().Pt());
    SetTreeVar("met_phi",e->GetMet().Phi());
    SetTreeVar("ht",evt_HT);
    SetTreeVar("st",evt_ST);
    SetTreeVar("DRl1b1",evt_DRl1b1);
    SetTreeVar("DRl2b1",evt_DRl2b1);
    SetTreeVar("DRbbmin",evt_minDRbb);
    SetTreeVar("MassDRbbmin",evt_minDRbb_invMass);
    SetTreeVar("MassDRlbmin",evt_minDRlb_invMass);
    SetTreeVar("DEtaMaxBB",evt_DEtaMaxBB);
    SetTreeVar("DRlbmaxPT",evt_DRlbmaxPt);
    SetTreeVar("MJJJmaxPt",evt_MJJJmaxPt);
    SetTreeVar("AvDRJJJmaxPt",evt_AvDRJJJmaxPt);
    SetTreeVar("AvCSVPt",evt_AvCSVPt);

    SetTreeVar("mt",evt_MT);
    SetTreeVar("mt2ll",evt_MT2ll);
    //    SetTreeVar("mt2bb",evt_MT2bb);
    SetTreeVar("mt2bb1l",evt_MT2bb1l);
    if(bdt.size()>0) { SetTreeVar("bdt1lh",bdt[0]); } else {SetTreeVar("bdt1lh",-1);}
    if(bdt.size()>1) { SetTreeVar("bdt1lm",bdt[1]); } else {SetTreeVar("bdt1lm",-1);}
    if(bdt.size()>2) { SetTreeVar("bdt1ll",bdt[2]); } else {SetTreeVar("bdt1ll",-1);}
    if(bdt.size()>3) { SetTreeVar("bdt2lh",bdt[3]); } else {SetTreeVar("bdt2lh",-1);}
    if(bdt.size()>4) { SetTreeVar("bdt2lm",bdt[4]); } else {SetTreeVar("bdt2lm",-1);}
    if(bdt.size()>5) { SetTreeVar("bdt2ll",bdt[5]); } else {SetTreeVar("bdt2ll",-1);}
    SetTreeVar("mtMin",evt_MTmin);
    SetTreeVar("mtMax",evt_MTmax);
    SetTreeVar("mtTot",evt_MTtot);

    SetTreeVar("Cen",evt_C);
    SetTreeVar("HemiOut",evt_HemiMetOut);

    //////
    //
    // Fill various event categories
    //

    if (category.find("_1Mu")    !=string::npos) SetTreeVar("lep_category",1);
    if (category.find("_1Ele")   !=string::npos) SetTreeVar("lep_category",2);
    if (category.find("_2Mu")    !=string::npos) SetTreeVar("lep_category",3);
    if (category.find("_2Ele")   !=string::npos) SetTreeVar("lep_category",4);
    if (category.find("_1Mu1Ele")!=string::npos) SetTreeVar("lep_category",5);

    int triggerMap=1*passTriggerMu + 10*passTriggerEle + 100*passTriggerMuMu + 1000*passTriggerMuEle + 10000*passTriggerEleEle ;

    SetTreeVar("passTrigger",triggerMap);

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
            //            mc = 0;
            // low mass
            if (label.find("M-180") !=string::npos) mc = 1;
            if (label.find("M-200") !=string::npos) mc = 2;
            if (label.find("M-220") !=string::npos) mc = 3;
            if (label.find("M-250") !=string::npos) mc = 4;
            if (label.find("M-300") !=string::npos) mc = 5;
            if (label.find("M-350") !=string::npos) mc = 6;
            if (label.find("M-400") !=string::npos) mc = 7;
            if (label.find("M-450") !=string::npos) mc = 8;
            if (label.find("M-500") !=string::npos) mc = 9;
            // high mass
            if (label.find("M-750") !=string::npos) mc = 10;
            if (label.find("M-800") !=string::npos) mc = 11;
            if (label.find("M-1000")!=string::npos) mc = 12;
            if (label.find("M-2000")!=string::npos) mc = 13;
            if (label.find("M-3000")!=string::npos) mc = 14;


        } else if (label.find("HplusToTauNu") !=string::npos) //sig TauNu
        {
            //            mc = 50;
            if (label.find("M-180") !=string::npos) mc = 51;
            if (label.find("M-200") !=string::npos) mc = 52;
            if (label.find("M-220") !=string::npos) mc = 53;
            if (label.find("M-250") !=string::npos) mc = 54;
            if (label.find("M-300") !=string::npos) mc = 55;
            if (label.find("M-400") !=string::npos) mc = 57;
            if (label.find("M-500") !=string::npos) mc = 59;
            if (label.find("M-800") !=string::npos) mc = 61;
            if (label.find("M-1000")!=string::npos) mc = 62;
            if (label.find("M-2000")!=string::npos) mc = 63;
            if (label.find("M-3000")!=string::npos) mc = 64;

        } else  // bkg
        {
            // ttbar + single top + ttV
            //            mc = 100;
            if(doICHEP) {
                if(label.find("TTJets_DiLept") !=string::npos) mc =101 ;
                if(label.find("TTJets_SingleLeptFrom") !=string::npos) mc =102 ;
            //            if(label.find("TTJets_SingleLeptFromT") !=string::npos) mc =102 ;
            //            if(label.find("TTJets_SingleLeptFromTbar") !=string::npos) mc =103 ;
            } else {
                if(label.find("TTTo2L2Nu_TuneCUETP8M2") !=string::npos) mc =101 ;
                if(label.find("TTToSemilepton_TuneCUETP8M2") !=string::npos) mc =102 ;
                if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) mc =103 ;

                if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrdown") !=string::npos) mc =104 ;
                if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrup") !=string::npos) mc =105 ;
                if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrdown") !=string::npos) mc =106 ;
                if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrup") !=string::npos) mc =107 ;

                if(label.find("TTJets") !=string::npos) mc =108 ;

                if(label.find("TTTo2L2Nu_ttbbFilter") !=string::npos) mc =109 ;
                if(label.find("TTToSemilepton_ttbbFilter") !=string::npos) mc =110 ;

            }

            if(label.find("ST") !=string::npos) mc =111 ;
            //            if(label.find("ST_tW_top") !=string::npos) mc =111 ;
            //            if(label.find("ST_tW_antitop") !=string::npos) mc =112 ;
            //            if(label.find("ST_t-channel_top") !=string::npos) mc =113 ;
            //            if(label.find("ST_t-channel_antitop") !=string::npos) mc =114 ;
            //            if(label.find("ST_s-channel_4f") !=string::npos) mc =115 ;
            if(label.find("tZq") !=string::npos) mc =116 ;
            //            if(label.find("tZq_ll_4f") !=string::npos) mc =116 ;
            //            if(label.find("tZq_nunu_4f") !=string::npos) mc =117 ;

            // tt+X with X=Z,W,H,TT
            if(label.find("TTZ") !=string::npos) mc = 121;
            //            if(label.find("TTZToQQ") !=string::npos) mc = 121;
            //            if(label.find("TTZToLLNuNu") !=string::npos) mc =122 ;
            if(label.find("TTW") !=string::npos) mc =123 ;
            //            if(label.find("TTWJetsToQQ") !=string::npos) mc =123 ;
            //            if(label.find("TTWJetsToLNu") !=string::npos) mc =124 ;
            if(label.find("ttH") !=string::npos) mc =125 ;
            //            if(label.find("ttHJetTobb") !=string::npos) mc =125 ;
            //            if(label.find("ttHJetToNonbb") !=string::npos) mc =126 ;
            if(label.find("TTTT") !=string::npos) mc =127 ;
            if(label.find("TTG") !=string::npos) mc =128 ;

            // V+jets
            //            mc = 200;
            if(doICHEP) {
                if(label.find("DYJets-madgraph") !=string::npos) mc =221 ;
                if(label.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8")!=string::npos) mc =222;
            } else {
                if(label.find("DYJetsToLL_M") !=string::npos) mc =221 ;
                if(label.find("WJetsToLNu")!=string::npos) mc =222;
            }
            // EWK
            // missing tribosons
            //            mc = 300;
            if(label.find("WWTo") !=string::npos) mc =331 ;
            //            if(label.find("WWTo2L2Nu") !=string::npos) mc =331 ;
            //            if(label.find("WWToLNuQQ") !=string::npos) mc =332 ;
            if(label.find("WZTo") !=string::npos) mc =333 ;
            //            if(label.find("WZTo1L1Nu2Q") !=string::npos) mc =333 ;
            //            if(label.find("WZTo1L3Nu") !=string::npos) mc =334 ;
            //            if(label.find("WZTo2L2Q") !=string::npos) mc =335 ;
            //            if(label.find("WZTo3LNu") !=string::npos) mc =336 ;
            if(label.find("ZZTo") !=string::npos) mc =337 ;
            //            if(label.find("ZZTo2L2Nu") !=string::npos) mc =337 ;
            //            if(label.find("ZZTo2L2Q") !=string::npos) mc =338 ;
            //            if(label.find("ZZTo4L") !=string::npos) mc =339 ;
            if(label.find("VHToNonbb_M125") !=string::npos) mc =340 ;
            if(label.find("WH_HToBB_WToLNu_M125") !=string::npos) mc =341 ;

        }

    SetTreeVar("mc",mc);

    SetTreeVar("nGenB",nGenB);
    SetTreeVar("genTTid",e->GetGenTtbarId());
    SetTreeVar("genLepSig",genLepSig);

    if ( not e->IsRealData() and (label.find("HplusToTB")  !=string::npos ) ){

        if(bFromH!= NULL) {
            SetTreeVar("bFromH_pt",bFromH->Pt());
            SetTreeVar("bFromH_eta",bFromH->Eta());
            SetTreeVar("bFromH_phi",bFromH->Phi());
        }

        if(bFromTopAss!= NULL) {
            SetTreeVar("bFromTopAss_pt",bFromTopAss->Pt());
            SetTreeVar("bFromTopAss_eta",bFromTopAss->Eta());
            SetTreeVar("bFromTopAss_phi",bFromTopAss->Phi());
        }

        if(bFromTopH!= NULL) {
            SetTreeVar("bFromTopH_pt",bFromTopH->Pt());
            SetTreeVar("bFromTopH_eta",bFromTopH->Eta());
            SetTreeVar("bFromTopH_phi",bFromTopH->Phi());
        }

        if(topFromH!= NULL) {
            SetTreeVar("topFromH_pt",topFromH->Pt());
            SetTreeVar("topFromH_eta",topFromH->Eta());
            SetTreeVar("topFromH_phi",topFromH->Phi());
        }

        if(leptonFromTopH!= NULL) {
            SetTreeVar("leptonFromTopH_pt",leptonFromTopH->Pt());
            SetTreeVar("leptonFromTopH_eta",leptonFromTopH->Eta());
            SetTreeVar("leptonFromTopH_phi",leptonFromTopH->Phi());
        }

        if(WFromTopH!= NULL) {
            SetTreeVar("WFromTopH_pt",WFromTopH->Pt());
            SetTreeVar("WFromTopH_eta",WFromTopH->Eta());
            SetTreeVar("WFromTopH_phi",WFromTopH->Phi());
        }

        if(WFromTopAss!= NULL) {
            SetTreeVar("WFromTopAss_pt",WFromTopAss->Pt());
            SetTreeVar("WFromTopAss_eta",WFromTopAss->Eta());
            SetTreeVar("WFromTopAss_phi",WFromTopAss->Phi());
        }


    }
}

void ChargedHiggsTopBottom::AddSpectator( string name, char type, int r){

    cout<<"[TmvaAnalysis]::[AddSpectator]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    cout<<"[TmvaAnalysis]::[DEBUG] AddSpectator of type F to reader "<<r <<" and name "<<name<<endl;
    if ( type == 'I') readers_[r] -> AddSpectator(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') readers_[r] -> AddSpectator(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddSpectator]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }

}


void ChargedHiggsTopBottom::AddVariable( string name, char type, int r){
    cout<<"[TmvaAnalysis]::[AddVariable]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type);
    cout<<"[TmvaAnalysis]::[DEBUG] AddVariables of type F to reader "<<r <<" and name "<<name<<endl;
    //    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    //    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    if ( type == 'I') readers_[r] -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') readers_[r] -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else {
        cout <<"[TmvaAnalysis]::[AddVariable]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }
}//end add variable


void ChargedHiggsTopBottom::Init()
{

    Log(__FUNCTION__,"INFO",Form("do1lAnalysis=%d",do1lAnalysis));
    Log(__FUNCTION__,"INFO",Form("do2lAnalysis=%d",do2lAnalysis));
    Log(__FUNCTION__,"INFO",Form("doTaulAnalysis=%d",doTaulAnalysis));
    Log(__FUNCTION__,"INFO",Form("doSynch=%d",doSynch));

    Preselection();
    bin_.reset(new GetBinning());

    if(doSynch) return;

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader"<<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    for (int i=0; i<3; i++) {
        // 0,1,2

        // 1l - high mass
        // 1l - medium mass
        // 1l - low mass

        AddVariable("ht",'F',i);
        AddVariable("met_pt",'F',i);
        AddVariable("lep1_pt",'F',i);
        AddVariable("bjet_pt[0]",'F',i);
        AddVariable("DRl1b1",'F',i);
        AddVariable("DRbbmin",'F',i);
        AddVariable("MassDRbbmin",'F',i);
        AddVariable("MassDRlbmin",'F',i);
        AddVariable("mt",'F',i);
        //    AddVariable("mt2bb",'F',i);
        AddVariable("mt2bb1l",'F',i);
        AddVariable("Cen",'F',i);
        AddVariable("HemiOut",'F',i);
        AddVariable("DEtaMaxBB",'F',i);
        AddVariable("DRlbmaxPT",'F',i);
        AddVariable("AvCSVPt",'F',i);
        AddVariable("st",'F',i);
        AddVariable("MJJJmaxPt",'F',i);

        AddSpectator("mc",'F',i); AddSpectator("run",'F',i); AddSpectator("lumi",'F',i); AddSpectator("evt",'F',i);

    }

    for (int i=3; i<3+3; i++) {
        // 0,1,2

        // 2l - high mass
        // 2l - medium mass
        // 2l - low mass
        AddVariable("ht",'F',i);
        AddVariable("met_pt",'F',i);
        AddVariable("lep1_pt",'F',i);
        AddVariable("bjet_pt[0]",'F',i);
        AddVariable("DRl1b1",'F',i);
        AddVariable("DRbbmin",'F',i);
        AddVariable("MassDRbbmin",'F',i);
        AddVariable("MassDRlbmin",'F',i);
        AddVariable("mt",'F',i);
        //    AddVariable("mt2bb",'F',3);
        AddVariable("mt2bb1l",'F',i);
        AddVariable("Cen",'F',i);
        AddVariable("HemiOut",'F',i);
        AddVariable("DEtaMaxBB",'F',i);
        AddVariable("DRlbmaxPT",'F',i);
        AddVariable("AvCSVPt",'F',i);
        AddVariable("st",'F',i);
        AddVariable("mtMin",'F',i);

        AddSpectator("mc",'F',i); AddSpectator("run",'F',i); AddSpectator("lumi",'F',i); AddSpectator("evt",'F',i);

    }

    //////////////
    ////  ------
    ////

    for (int i=6; i<6+6; i++) {
        // 0,1,2

        // 1l - high mass
        // 1l - medium mass
        // 1l - low mass

        AddVariable("ht",'F',i);
        AddVariable("met_pt",'F',i);
        AddVariable("lep1_pt",'F',i);
        AddVariable("bjet_pt[0]",'F',i);
        AddVariable("DRl1b1",'F',i);
        AddVariable("DRbbmin",'F',i);
        AddVariable("MassDRbbmin",'F',i);
        AddVariable("MassDRlbmin",'F',i);
        AddVariable("mt",'F',i);
        //    AddVariable("mt2bb",'F',i);
        AddVariable("mt2bb1l",'F',i);
        AddVariable("Cen",'F',i);
        AddVariable("HemiOut",'F',i);
        AddVariable("DEtaMaxBB",'F',i);
        AddVariable("DRlbmaxPT",'F',i);
        AddVariable("AvCSVPt",'F',i);
        AddVariable("st",'F',i);
        AddVariable("MJJJmaxPt",'F',i);

        AddSpectator("mc",'F',i); AddSpectator("run",'F',i); AddSpectator("lumi",'F',i); AddSpectator("evt",'F',i);

    }

    for (int i=12; i<12+6; i++) {
        // 0,1,2

        // 2l - high mass
        // 2l - medium mass
        // 2l - low mass
        AddVariable("ht",'F',i);
        AddVariable("met_pt",'F',i);
        AddVariable("lep1_pt",'F',i);
        AddVariable("bjet_pt[0]",'F',i);
        AddVariable("DRl1b1",'F',i);
        AddVariable("DRbbmin",'F',i);
        AddVariable("MassDRbbmin",'F',i);
        AddVariable("MassDRlbmin",'F',i);
        AddVariable("mt",'F',i);
        //    AddVariable("mt2bb",'F',3);
        AddVariable("mt2bb1l",'F',i);
        AddVariable("Cen",'F',i);
        AddVariable("HemiOut",'F',i);
        AddVariable("DEtaMaxBB",'F',i);
        AddVariable("DRlbmaxPT",'F',i);
        AddVariable("AvCSVPt",'F',i);
        AddVariable("st",'F',i);
        AddVariable("mtMin",'F',i);

        AddSpectator("mc",'F',i); AddSpectator("run",'F',i); AddSpectator("lumi",'F',i); AddSpectator("evt",'F',i);

    }



    // load weights
    for( size_t i=0;i<weights.size() ;++i)
        {
            cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
            readers_[i]->BookMVA("BDTG",weights[i].c_str());
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
    Branch("tree_tb","passTrigger",'I'); // to distinguish between the various trigger bits

    Branch("tree_tb","run",'I');
    Branch("tree_tb","lumi",'I');
    Branch("tree_tb","evt",'I');
    Branch("tree_tb","npv",'I');
    Branch("tree_tb","weight",'D');
    Branch("tree_tb","isRealData",'I');

    Branch("tree_tb","weight_pu",'D');
    Branch("tree_tb","weight_bTag",'D');
    Branch("tree_tb","weight_bTagCSV",'D');
    Branch("tree_tb","weight_nEv",'D');
    Branch("tree_tb","weight_topPt",'D');
    Branch("tree_tb","xsec",'F');

    // fill lepton quantities
    Branch("tree_tb","lep1_pt",'F');
    Branch("tree_tb","lep1_phi",'F');
    Branch("tree_tb","lep1_eta",'F');
    //    Branch("tree_tb","lep1_charge",'F');
    //    Branch("tree_tb","lep1_isolation",'F');
    Branch("tree_tb","lep1_id",'F');

    Branch("tree_tb","lep2_pt",'F');
    Branch("tree_tb","lep2_phi",'F');
    Branch("tree_tb","lep2_eta",'F');
    //    Branch("tree_tb","lep2_charge",'F');
    //    Branch("tree_tb","lep2_isolation",'F');
    Branch("tree_tb","lep2_id",'F');

    // fill counter and scalar
    Branch("tree_tb","NcentralJets",'I');
    Branch("tree_tb","NJets",'I');
    Branch("tree_tb","NBJets",'I');

    // fill event variables
    Branch("tree_tb","met_pt",'F');
    Branch("tree_tb","met_phi",'F');
    Branch("tree_tb","ht",'F');
    Branch("tree_tb","st",'F');
    Branch("tree_tb","DRl1b1",'F');
    Branch("tree_tb","DRl2b1",'F');
    Branch("tree_tb","DRbbmin",'F');
    Branch("tree_tb","MassDRbbmin",'F');
    Branch("tree_tb","MassDRlbmin",'F');
    Branch("tree_tb","DEtaMaxBB",'F');
    Branch("tree_tb","DRlbmaxPT",'F');
    Branch("tree_tb","MJJJmaxPt",'F');
    Branch("tree_tb","AvDRJJJmaxPt",'F');
    Branch("tree_tb","AvCSVPt",'F');

    // various masses
    Branch("tree_tb","bdt1lh",'F');
    Branch("tree_tb","bdt1lm",'F');
    Branch("tree_tb","bdt1ll",'F');
    Branch("tree_tb","bdt2lh",'F');
    Branch("tree_tb","bdt2lm",'F');
    Branch("tree_tb","bdt2ll",'F');
    Branch("tree_tb","mt",'F');
    Branch("tree_tb","mt2ll",'F');
    //    Branch("tree_tb","mt2bb",'F');
    Branch("tree_tb","mt2bb1l",'F');
    Branch("tree_tb","mtMin",'F');
    Branch("tree_tb","mtMax",'F');
    Branch("tree_tb","mtTot",'F');

    // Shape variables
    Branch("tree_tb","Cen",'F');
    Branch("tree_tb","HemiOut",'F');

    // fill all the object vector
    Branch("tree_tb","jet_pt",'d',10,"NJets");
    Branch("tree_tb","jet_eta",'d',10,"NJets");
    Branch("tree_tb","jet_phi",'d',10,"NJets");
    Branch("tree_tb","jet_e",'d',10,"NJets");
    Branch("tree_tb","jet_discr",'d',10,"NJets");

    // fill all the object vector
    Branch("tree_tb","bjet_pt",'d',10,"NBJets");
    Branch("tree_tb","bjet_eta",'d',10,"NBJets");
    Branch("tree_tb","bjet_e",'d',10,"NBJets");
    Branch("tree_tb","bjet_phi",'d',10,"NBJets");
    Branch("tree_tb","bjet_discr",'d',10,"NBJets");

    //// VARIOUS gen  INFO

    Branch("tree_tb","genTTid",'I');
    Branch("tree_tb","nGenB",'I');

    Branch("tree_tb","genLepSig",'I'); // to distinguish between the different leptonCategory

    Branch("tree_tb","bFromH_pt",'F');
    Branch("tree_tb","bFromH_phi",'F');
    Branch("tree_tb","bFromH_eta",'F');

    Branch("tree_tb","bFromTopH_pt",'F');
    Branch("tree_tb","bFromTopH_phi",'F');
    Branch("tree_tb","bFromTopH_eta",'F');

    Branch("tree_tb","leptonFromTopH_pt",'F');
    Branch("tree_tb","leptonFromTopH_phi",'F');
    Branch("tree_tb","leptonFromTopH_eta",'F');

    Branch("tree_tb","WFromTopH_pt",'F');
    Branch("tree_tb","WFromTopH_phi",'F');
    Branch("tree_tb","WFromTopH_eta",'F');

    Branch("tree_tb","topFromH_pt",'F');
    Branch("tree_tb","topFromH_phi",'F');
    Branch("tree_tb","topFromH_eta",'F');

    Branch("tree_tb","bFromTopAss_pt",'F');
    Branch("tree_tb","bFromTopAss_phi",'F');
    Branch("tree_tb","bFromTopAss_eta",'F');

    Branch("tree_tb","WFromTopAss_pt",'F');
    Branch("tree_tb","WFromTopAss_phi",'F');
    Branch("tree_tb","WFromTopAss_eta",'F');


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

        //$$$$$$
        //$$$$$$
        /*
        Book(    "ChargedHiggsTopBottom/Yield"+category+"/Yield_"+ l  , ("Yield "+ l).c_str(),10,-.5,10-.5);
        /// topCR
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(top1,"N_{jets} #eq 4; N_{b} #eq 1");
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(top1,"N_{jets} #eq 4; N_{b} #eq 2");
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(top2,"N_{jets} #eq 4; N_{b} #geq 3");
        // extraRadCR
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(eRad1,"N_{jets} #eq 5; N_{b} #eq 1");
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(eRad2,"N_{jets} #geq 6; N_{b} #geq 1");
        /// SR
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(SR1,"N_{jets} #eq 5; N_{b} #eq 2");
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(SR2,"N_{jets} #eq 5; N_{b} #geq 3");

        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(SR3,"N_{jets} #geq 6; N_{b} #eq 2");
        GetHisto("ChargedHiggsTopBottom/Yield"+category+"/Yield_"+l,"")->GetXaxis()->SetBinLabel(SR4,"N_{jets} #geq 6; N_{b} #geq 3");
        */

        /*
        ////Histograms for N-1
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Mt_"+l,"Mt "+l+";M_{T} [GeV]",50,0,250);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/EtMiss_"+l,"EtMiss "+l+";E_{T}^{miss} [GeV]",30,0,300);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 [GeV]",15,0,15);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 [GeV] |#eta| <2.4",10,0,10);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 [GeV] |#eta| >2.4",10,0,10);
        Book("ChargedHiggsTopBottom/PreselectionN1"+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 [GeV]",10,0,10);
        */

}

void ChargedHiggsTopBottom::BookFlavor(string l, string category, string phasespace, string flavor, string SR)
{
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+SR+flavor+l);

    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+SR+flavor+l);
    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+SR+flavor+l);
    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+SR+flavor+l);

    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1ll_"+SR+flavor+l);
    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lm_"+SR+flavor+l);
    if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lh_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2ll_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lm_"+SR+flavor+l);
    if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lh_"+SR+flavor+l);

    //////

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+SR+flavor+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+SR+flavor+l,"ST "+l+"; ST ( HT+met+lepsPt )",800,0,8000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+SR+flavor+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",40,0,2000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+SR+flavor+l,"ST "+l+"; ST ( HT+met+lepsPt )",50,0,2500);

    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1ll_"+SR+flavor+l,"bdt2D_1ll_ "+l+";bdt (2D low) 1l",   4,-0.5,3.5);
    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lm_"+SR+flavor+l,"bdt2D_1lm_ "+l+";bdt (2D medium) 1l",4,-0.5,3.5);
    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lh_"+SR+flavor+l,"bdt2D_1lh_ "+l+";bdt (2D high) 1l",  4,-0.5,3.5);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2ll_"+SR+flavor+l,"bdt2D_2ll_ "+l+";bdt (2D low) 2l",   4,-0.5,3.5);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lm_"+SR+flavor+l,"bdt2D_2lm_ "+l+";bdt (2D medium) 2l",4,-0.5,3.5);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lh_"+SR+flavor+l,"bdt2D_2lh_ "+l+";bdt (2D high) 2l",  4,-0.5,3.5);

    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+SR+flavor+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+SR+flavor+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
    if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+SR+flavor+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+SR+flavor+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+SR+flavor+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
    if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+SR+flavor+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

    if(!(SR.find("SR")!=string::npos)) {

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMaxBB_"+SR+flavor+l,"DEtaMaxBB "+l+";dEta_{b^{i},b^{j}}^{max}",100,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+SR+flavor+l,"NBjets "+l + ";Number of b jets P_{T}>40 ",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+SR+flavor+l,"thirdBDiscr "+l + "; discr (3rd b-ranked) ",50,0.,1.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+SR+flavor+l,"minDRbb"+l+";dR_{bb}^{min}",50,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+SR+flavor+l,"minDRbb_mass "+l+";m(bb)^{dR^{min}})",50,0,500);

    }

}


void ChargedHiggsTopBottom::BookHisto(string l, string category, string phasespace)
{

        cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Final Histo" <<l<<endl;

        /**********************************************
         *                   Final                     *
         **********************************************/

        AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+l);
        AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+l);

        AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+l);
        AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+l);

        //BDT plot
        if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+l);
        if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+l);
        if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+l);
        if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+l);
        if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+l);
        if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+l);

        if(phasespace.find("Baseline")!=string::npos) {

           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR1_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR2_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR3_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR4_"+l);

           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR1_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR2_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR3_"+l);
           AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR4_"+l);

           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR1_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR1_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR1_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR1_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR1_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR1_"+l);

           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR2_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR2_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR2_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR2_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR2_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR2_"+l);

           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR3_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR3_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR3_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR3_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR3_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR3_"+l);

           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR4_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR4_"+l);
           if(do1lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR4_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR4_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR4_"+l);
           if(do2lAnalysis) AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR4_"+l);

        }

        /**********************************************
         *        Book hist for datacard               *
         **********************************************/

        /////
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",40,0,2000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);

        /////
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+l,"ST "+l+"; ST ( HT+met+lepsPt )",50,0,2500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);

        //BDT plot
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

        if(phasespace.find("Baseline")!=string::npos) {

            Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR1_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR2_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR3_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_SR4_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);

            Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR1_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR2_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR3_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
            Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_SR4_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);

            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR1_"+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR1_"+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR1_"+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR1_"+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR1_"+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR1_"+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR2_"+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR2_"+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR2_"+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR2_"+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR2_"+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR2_"+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR3_"+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR3_"+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR3_"+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR3_"+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR3_"+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR3_"+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_SR4_"+l,"bdt1lh "+l+";bdt (1l high)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_SR4_"+l,"bdt1lm "+l+";bdt (1l medium)",200,-1.,1.);
            if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_SR4_"+l,"bdt1ll "+l+";bdt (1l low)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_SR4_"+l,"bdt2lh "+l+";bdt (2l high)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_SR4_"+l,"bdt2lm "+l+";bdt (2l medium)",200,-1.,1.);
            if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_SR4_"+l,"bdt2ll "+l+";bdt (2l low)",200,-1.,1.);

        }

        /**********************************************
         *          special HeavyFlavour               *
         **********************************************/


        if((l.find("TTTo2L2Nu_TuneCUETP8M2")!=string::npos) || (l.find("TTToSemilepton_TuneCUETP8M2")!=string::npos)
           || (l.find("TTTo2L2Nu_ttbbFilter")!=string::npos) || (l.find("TTToSemilepton_ttbbFilter")!=string::npos)
           || (l.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=string::npos)
           || (l.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrdown")!=string::npos)
           || (l.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrup")!=string::npos)
           || (l.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrdown")!=string::npos)
           || (l.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrup")!=string::npos)
           || (l.find("TTJets")!=string::npos)
           ) {

            //            BookFlavor(l, category, phasespace, "other_", "");
            BookFlavor(l, category, phasespace, "tt2bMerged_", "");
            BookFlavor(l, category, phasespace, "tt2b_", "");
            BookFlavor(l, category, phasespace, "tt1b_", "");
            BookFlavor(l, category, phasespace, "ttc_", "");
            BookFlavor(l, category, phasespace, "ttlight_", "");

            if(phasespace.find("Baseline")!=string::npos) {
                //                BookFlavor(l, category, phasespace, "other_", "SR1_");
                //                BookFlavor(l, category, phasespace, "other_", "SR2_");
                //                BookFlavor(l, category, phasespace, "other_", "SR3_");
                //                BookFlavor(l, category, phasespace, "other_", "SR4_");

                BookFlavor(l, category, phasespace, "tt2bMerged_", "SR1_");
                BookFlavor(l, category, phasespace, "tt2bMerged_", "SR2_");
                BookFlavor(l, category, phasespace, "tt2bMerged_", "SR3_");
                BookFlavor(l, category, phasespace, "tt2bMerged_", "SR4_");

                BookFlavor(l, category, phasespace, "tt2b_", "SR1_");
                BookFlavor(l, category, phasespace, "tt2b_", "SR2_");
                BookFlavor(l, category, phasespace, "tt2b_", "SR3_");
                BookFlavor(l, category, phasespace, "tt2b_", "SR4_");

                BookFlavor(l, category, phasespace, "tt1b_", "SR1_");
                BookFlavor(l, category, phasespace, "tt1b_", "SR2_");
                BookFlavor(l, category, phasespace, "tt1b_", "SR3_");
                BookFlavor(l, category, phasespace, "tt1b_", "SR4_");

                BookFlavor(l, category, phasespace, "ttc_", "SR1_");
                BookFlavor(l, category, phasespace, "ttc_", "SR2_");
                BookFlavor(l, category, phasespace, "ttc_", "SR3_");
                BookFlavor(l, category, phasespace, "ttc_", "SR4_");

                BookFlavor(l, category, phasespace, "ttlight_", "SR1_");
                BookFlavor(l, category, phasespace, "ttlight_", "SR2_");
                BookFlavor(l, category, phasespace, "ttlight_", "SR3_");
                BookFlavor(l, category, phasespace, "ttlight_", "SR4_");
            }

        }


        /////
        // Control Plots leptons
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonEta_"+l,"LeptonEta "+l + ";#eta (lepton)",20,-5.,5.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonPt_"+l,"LeptonPt "+l + ";p_{T}^{lepton} [GeV]",50,0.,250.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonIso_"+l,"LeptonIso "+l + ";iso (lepton) [GeV]",50,0.,50.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonMiniIso_"+l,"LeptonMiniIso "+l + ";mini iso (lepton) [GeV]",50,0.,0.5);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonMva_"+l,"LeptonMva "+l + ";mva (lepton)",50,-1,1.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailEta_"+l,"LeptonTrailEta "+l + ";#eta (trailing lepton)",20,-5.,5.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailPt_"+l,"LeptonTrailPt "+l + ";p_{T}^{trailing lepton} [GeV]",50,0.,200.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailIso_"+l,"LeptonTrailIso "+l + ";iso (trailing lepton) [GeV]",50,0.,50.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailMiniIso_"+l,"LeptonTrailMiniIso "+l + ";mini iso (trailing lepton) [GeV]",50,0.,0.5);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailMva_"+l,"LeptonTrailMva "+l + ";mva (trailing lepton)",50,-1.,1.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Mt_"+l,"Mt "+l+";M_{T} [GeV]",50,0.,250.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+l,"Met "+l+";MET [GeV]",50,0.,1000.);

        /// Vertices
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+l,"Vertices "+l + ";Number of vertices",50,0.,50.);

        /////
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 ",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 |#eta|<=2.4",10,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);

        // VARIOUS qb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_"+l,"LeadingBPt "+l+";P_{T} (leading B) [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBDiscr_"+l,"LeadingBDiscr "+l+";discr (leading B) [GeV]",50,0.,1.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBPt_"+l,"NextLeadingBPt "+l+";P_{T} (next leading B) [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextLeadingBDiscr_"+l,"NextLeadingBDiscr "+l+";discr (next leading B) [GeV]",50,0,1.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+l,"thirdBDiscr "+l+";discr (3rd b-ranked)",50,0,1.);

        // jet ratio
        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/NextOverLeadingBPt_"+l,"NextOverLeadingBPt "+l+";P_{T}^{B2}/P_{T}^{B1} [GeV]",50,0,1);
        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_OverHT_"+l,"LeadingBPt_OverHT "+l+";P_{T}^{B1}/HT [GeV]",50,0,1);

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMaxBB_"+l,"DEtaMaxBB "+l+";dEta_{b^{i},b^{j}}^{max}",100,0,10);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMax_"+l,"DEtaMax "+l+";dEta_{j^{i},j^{j}}^{max}",100,0,10);

        // min DRbb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+l,"minDRbb"+l+";dR_{bb}^{min}",50,0,2*TMath::Pi()); // <-- this has signal discrimination
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+l,"minDRbb_mass "+l+";m(bb)^{dR^{min}})",50,0,500);
        // b1b2
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DRb1b2_"+l,"DRb1b2"+l+";dR_{b^{1}b^{2}}",100,0,10);
        //        Book2D( "ChargedHiggsTopBottom/Baseline"+category+"/DRVsMassb1b2_"+l,"DRVsMassb1b2"+l+";mass(b^{1},b^{2});dR(b^{1},b^{2})",100,0,1000,100,0,10);
        // min DRqq
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_"+l,"minDRqq"+l+";dR_{qq}^{min}",100,0,10);
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRqq_mass_"+l,"minDRqq_mass "+l+";m(qq)^{dR^{min}})",50,0,500);

        // l1 l2
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonM_"+l,"DiLeptonM "+l + ";m(l^{1},l^{2}) (lepton) [GeV]",50,0.,200.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonPT_"+l,"DiLeptonPT "+l + ";p_{T}(l^{1},l^{2}) (lepton) [GeV]",50,0.,200.);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonDeltaR_"+l,"DiLeptonDeltaR "+l + ";dR_(l^{1},l^{2})",50,0,2*TMath::Pi());

        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/MT2ll_"+l,"MT2ll "+l + ";mT2(l^{1},l^{2},met) [GeV]",60,0.,300.);
        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb_"+l,"MT2bb "+l + ";mT2(b^{1},b^{2},met) [GeV]",50,0.,250.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb1l_"+l,"MT2bb1l "+l + ";mT2((b^{1}b^{2}), met+l^{1} ) [GeV]",50,0.,250.);

        /// Event Shapes
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/Centrality_"+l,"Centrality "+l + "; Sum Pt_{i} / Sum E_{i}",25, 0., 1.);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HemiOut_"+l,"HemiOut "+l + "; (Jet+Lepton) opposite to MET",50, 0., 500.);


        /////
        // JJJ
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/MJJJmaxPT_"+l,"MJJJmaxPT"+l+";M_{JJJ}^{maxP_{T}}",50,0,1000);
        if(do1lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/AvDRJJJmaxPT_"+l,"AvDRJJJmaxPT"+l+";AvDR_{JJJ}^{maxP_{T}}",50,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/AvCSVPt_"+l,"AvCSVPt"+l+";AvCSV",50,0.,1.);

        /////
        // lb
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/DRlbmaxPT_"+l,"DRlbmaxPT"+l+";dR_{lb}^{maxP_{T}}",50,0,2*TMath::Pi());

        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_"+l,"minDRlb"+l+";dR_{lb}^{min}",50,0,2*TMath::Pi());  // <-- this has signal discrimination
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_mass_"+l,"minDRlb_mass "+l+";m(lb)^{dR^{min}})",50,0,500);
        //        Book("ChargedHiggsTopBottom/Baseline/Rlb_"+l,"Rlb "+l+";R_{lb}",100,0,2*TMath::Pi());

        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dEtalb_"+l,"dEtalb "+l+";dEta_{l^{1},b^{1}}",50,0,10);
        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dPhilb_"+l,"dPhilb "+l+";dPhi_{l^{1},b^{1}}",50,0,2*TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dRlb_"+l,"dRlb "+l+";dR_{l^{1},b^{1}}",50,0,2*TMath::Pi());

        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dEtal2b_"+l,"dEtal2b "+l+";dEta_{l^{2},b^{1}}",50,0,10);
        //        Book("ChargedHiggsTopBottom/"+phasespace+category+"/dPhil2b_"+l,"dPhil2b "+l+";abs(dPhi_{l^{2},b^{1}})",50,0,TMath::Pi());
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/dRl2b_"+l,"dRl2b "+l+";dR_{l^{2},b^{1}}",50,0,2*TMath::Pi());

        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptW_"+l,"ptW "+l+";pT_{l,MET} [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptWb_"+l,"ptWb "+l+";pT_{W,b} [GeV]",50,0,500);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/PhiWb_"+l,"dPhiWb "+l+";abs(dPhi_{Wb})",100,0,TMath::Pi());
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/mtWb1_"+l,"mtWb1 "+l+";mT_{b1,l,MET}",50,0,1000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ptWb1_"+l,"ptWb1 "+l+";pT_{b1,l,MET} [GeV]",50,0,1000);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/minMT_"+l,"minMT "+l+";minMT [ (b1,(l1,MET)),(b1,(l2,MET)) ] [GeV] ",50,0,1000);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/maxMT_"+l,"maxMT "+l+";maxMT [ (b1,(l1,MET)),(b1,(l2,MET)) ] [GeV]",50,0,1000);
        if(do2lAnalysis) Book("ChargedHiggsTopBottom/"+phasespace+category+"/totMT_"+l,"totMT "+l+";totMT [ MT(b1,(l1,MET))^2 + MT(b1,(l2,MET))^2 ] [GeV]",50,0,1000);

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
        if(doTaulAnalysis) BookHisto(l, "_1Mu","Baseline");
        if(doTaulAnalysis) BookHisto(l, "_1Ele","Baseline");
        if(doTaulAnalysis) BookHisto(l, "_2Mu","Baseline");
        if(doTaulAnalysis) BookHisto(l, "_1Mu1Ele","Baseline");
        if(doTaulAnalysis) BookHisto(l, "_2Ele","Baseline");

        ///////
        ///////

        BookHisto(l, "","topCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","topCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","topCR");
        if(do2lAnalysis) BookHisto(l, "_2Mu","topCR");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","topCR");
        if(do2lAnalysis) BookHisto(l, "_2Ele","topCR");

        BookHisto(l, "","topCRR4"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","topCRR4");
        if(do1lAnalysis) BookHisto(l, "_1Ele","topCRR4");
        if(do2lAnalysis) BookHisto(l, "_2Mu","topCRR4");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","topCRR4");
        if(do2lAnalysis) BookHisto(l, "_2Ele","topCRR4");

        BookHisto(l, "","topCRR5"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","topCRR5");
        if(do1lAnalysis) BookHisto(l, "_1Ele","topCRR5");
        if(do2lAnalysis) BookHisto(l, "_2Mu","topCRR5");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","topCRR5");
        if(do2lAnalysis) BookHisto(l, "_2Ele","topCRR5");

        ///////
        ///////

        BookHisto(l, "","extraRadCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","extraRadCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_2Mu","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","extraRadCR");
        if(do2lAnalysis) BookHisto(l, "_2Ele","extraRadCR");

        BookHisto(l, "","extraRadCRR7"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","extraRadCRR7");
        if(do1lAnalysis) BookHisto(l, "_1Ele","extraRadCRR7");
        if(do2lAnalysis) BookHisto(l, "_2Mu","extraRadCRR7");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","extraRadCRR7");
        if(do2lAnalysis) BookHisto(l, "_2Ele","extraRadCRR7");

        BookHisto(l, "","extraRadCRR10"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","extraRadCRR10");
        if(do1lAnalysis) BookHisto(l, "_1Ele","extraRadCRR10");
        if(do2lAnalysis) BookHisto(l, "_2Mu","extraRadCRR10");
        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","extraRadCRR10");
        if(do2lAnalysis) BookHisto(l, "_2Ele","extraRadCRR10");

        ///////
        ///////

        if(do1lAnalysis) BookHisto(l, "","charmCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","charmCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","charmCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Mu","charmCR");
        //        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","charmCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Ele","charmCR");

        /*
        if(do1lAnalysis) BookHisto(l, "","lowMTCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","lowMTCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Mu","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","lowMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Ele","lowMTCR");

        if(do1lAnalysis) BookHisto(l, "","highMTCR"); // this is when there is nothing
        if(do1lAnalysis) BookHisto(l, "_1Mu","highMTCR");
        if(do1lAnalysis) BookHisto(l, "_1Ele","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Mu","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_1Mu1Ele","highMTCR");
        //        if(do2lAnalysis) BookHisto(l, "_2Ele","highMTCR");
        */

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

        /*
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

        Book("ChargedHiggsTopBottom/GENERATOR/etabAss_"+l,"bAss"+l+"; eta (associated b)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromTopH_"+l,"bFromTopH"+l+"; eta (from top from the Higgs)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromTopAss_"+l,"bFromTopAss"+l+"; eta (from associated top)",50,0,10);
        Book("ChargedHiggsTopBottom/GENERATOR/etabFromH_"+l,"bFromH"+l+"; eta (from Higgs Decay)",50,0,10);
        */

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
    bAss=NULL;
    bFromH=NULL;

    bFromTopAss=NULL;
    WFromTopAss=NULL;

    leptonTopAssH=NULL;

    leptonFromTopH=NULL;
    topFromH = NULL;
    bFromTopH=NULL;
    WFromTopH=NULL;
    genLepSig=0;

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
            if(abs(genpar->GetParentPdgId())==37) {
                if(verbose) cout << "   =$$$$= TOP from Higgs" << endl;
                topFromH = genpar;
            } else {
                if(verbose) cout << "   ==> prompt top" << endl;
            }
        } else if(abs(genpar->GetPdgId()) == 5){
        /// b
            if(abs(genpar->GetParentPdgId())==37)  {
                bFromH = genpar;
            } else if(abs(genpar->GetParentPdgId())==6) {
                if(abs(genpar->GetGrandParentPdgId())==37) {
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

        //        if(abs(genpar->GetPdgId()) == 1 or abs(genpar->GetPdgId()) == 2 or abs(genpar->GetPdgId()) == 3 or abs(genpar->GetPdgId()) == 4 or abs(genpar->GetPdgId()) == 11 or abs(genpar->GetPdgId()) == 13) {
        if(abs(genpar->GetPdgId()) == 24 and abs(genpar->GetParentPdgId()) == 6) {
            /// W
            if(topFromH!=NULL) {
                if ( topFromH->GetPdgId()*genpar->GetPdgId()>0 ) WFromTopH=genpar;
                if ( topFromH->GetPdgId()*genpar->GetPdgId()<0 )  WFromTopAss=genpar;
            }
        }
        //        }

        if(abs(genpar->GetPdgId()) == 11 or abs(genpar->GetPdgId()) == 13) {

            if ( abs(genpar->GetParentPdgId()) == 24 and abs(genpar->GetGrandParentPdgId()) == 6) genLepSig++;

            // lepton in acceptance
            if(genpar->Pt()<20) continue;
            if(abs(genpar->Eta())>2.4) continue;

            if(topFromH!=NULL) {
                // electron -11; W+ is 24; top is 6
                if ( abs(genpar->GetParentPdgId()) == 24 and abs(genpar->GetGrandParentPdgId()) == 6 and topFromH->GetPdgId()*genpar->GetPdgId()<0) {
                    if(verbose) cout << "H->top->W->l candidate (there is a second one in the acceptance)" << endl;
                    rightComb=true;
                    leptonFromTopH=genpar;
                }
            }
        }
    }

    //    if(verbose) cout << " found B: pt= " << genpar->GetP4().Pt() << " charge=" << genpar->GetPdgId() << " moPdgId=" << genpar->GetParentPdgId() << endl;
    if(verbose and bFromH!=NULL) cout << "   ==> B from Higgs " << bFromH->GetP4().Pt() << endl;
    if(verbose and bFromTopH!=NULL) cout << "   ==> B from TOP from Higgs " << bFromTopH->GetP4().Pt() << endl;
    if(verbose and bFromTopAss!=NULL) cout << "   ==> B from associated TOP " << bFromTopAss->GetP4().Pt() << endl;
    if(verbose and bAss!=NULL) cout << "    ==> prompt b " << bAss->GetP4().Pt() << endl;

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

void ChargedHiggsTopBottom::eventShapePlot(Event*e, string label, string category, string systname, string phasespace) {


    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Centrality_"+label,systname,evt_C,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HemiOut_"+label,systname,evt_HemiMetOut,e->weight());



}

void ChargedHiggsTopBottom::computeVar(Event*e) {


    ////$$$$$$
    ////$$$$$$
    ////$$$$$$  HT, ST
    ////$$$$$$

    evt_HT=0;

    // check the impact of the HT(w/ central jet only)
    for(int i=0;i!=e->NcentralJets();++i) {
        evt_HT += e->GetCentralJet(i)->Pt();
    }

    evt_ST=evt_HT+e->GetMet().Pt();
    if(leadLep!=NULL) evt_ST=evt_HT+leadLep->Pt();
    if(trailLep!=NULL) evt_ST=evt_HT+trailLep->Pt();


    ////$$$$$$
    ////$$$$$$
    ////$$$$$$ Event shapes (centrality, hemi)
    ////$$$$$$

    double hemiMetIn = 0.; // all objects inside MET hemisphere
    double hemiMetOut = 0.; // all objects outside MET hemisphere

    // check the impact of the HT(w/ central jet only)
    for(int i=0;i!=e->NcentralJets();++i) {
        if(e->GetCentralJet(i)->GetP4().Vect().Dot(e->GetMet().GetP4().Vect()) > 0 ) hemiMetIn += e->GetCentralJet(i)->GetP4().Pt();
        else hemiMetOut += e->GetCentralJet(i)->GetP4().Pt();
    }

    if(leadLep->GetP4().Vect().Dot(e->GetMet().GetP4().Vect()) > 0 ) hemiMetIn += leadLep->GetP4().Pt();
    else hemiMetOut += leadLep->GetP4().Pt();
    if(trailLep!=NULL) {
        if(trailLep->GetP4().Vect().Dot(e->GetMet().GetP4().Vect()) > 0 ) hemiMetIn += trailLep->GetP4().Pt();
        else hemiMetOut += trailLep->GetP4().Pt();
    }

    evt_HemiMetOut=hemiMetOut;


    float sumE=0;
    float sumPt=0;
    for(int i=0;i!=e->NcentralJets();++i) {
        sumE += e->GetCentralJet(i)->GetP4().E();
        sumPt += e->GetCentralJet(i)->GetP4().Pt();
    }

    if(sumE!=0) evt_C=sumPt/sumE;


    ////$$$$$$
    ////$$$$$$
    ////$$$$$$ Loops over B-jets
    ////$$$$$$

    if(e->Bjets()>1) {
        // MT2bb to remove later
        evt_MT2bb=ChargedHiggs::mt2(e->GetBjet(0)->GetP4(),e->GetBjet(1)->GetP4(),e->GetMet().GetP4());
        if(leadLep!=NULL) evt_MT2bb1l=ChargedHiggs::mt2(e->GetBjet(0)->GetP4(),e->GetBjet(1)->GetP4(),e->GetMet().GetP4()+leadLep->GetP4());

        double minDRbb=99999;
        double maxDRbb=0;
        double minDRbb_invMass=-1;
        double maxDRbb_invMass=-1;
        double indexI=-1;
        double indexJ=-1;
        double indexMaxJ=-1;


        double DEtaMaxBB=0.;
        double sumDRBB=0.;

        for(int i=0;i!=e->Bjets();++i) {
            Jet* bjet_i = e->GetBjet(i);
            for(int j=0;j!=e->Bjets();++j) {
                if (j==i) continue;
                Jet* bjet_j = e->GetBjet(j);

                // block to check the DRbb
                double dr = bjet_i->DeltaR(bjet_j);
                double mass = (bjet_i->GetP4() + bjet_j->GetP4()).M();
                if(dr<minDRbb) { minDRbb=dr; minDRbb_invMass=mass; indexI=i; indexJ=j;}
                if(dr>maxDRbb and i==0) { maxDRbb=dr; maxDRbb_invMass=mass; indexMaxJ=j;}

                // block to check the DEtaMaxBB
                if(bjet_i->DeltaEta(*bjet_j)>DEtaMaxBB) DEtaMaxBB=bjet_i->DeltaEta(bjet_j);

                // block to check the AverageDRBB
                sumDRBB +=dr;

            }
        }

        evt_minDRbb=minDRbb;
        evt_minDRbb_invMass=minDRbb_invMass;
        evt_DEtaMaxBB=DEtaMaxBB;
        evt_avDRBB=sumDRBB/e->Bjets();

    }

    double DEtaMaxJJ=0.;
    double AvCSVPt=0;
    double SumPt=0;

    for(int i=0;i!=e->NcentralJets();++i) {
        Jet* lj = e->GetCentralJet(i);
        for(int j=0;j!=e->NcentralJets();++j) {
            if (j==i) continue;
            Jet* jet = e->GetCentralJet(j);
            if(lj->DeltaEta(*jet)>DEtaMaxJJ) DEtaMaxJJ=lj->DeltaEta(*jet);

            // block to check the ptweighted average CSV of failing jets
            if(jet->bdiscr<0.8484 && jet->bdiscr>0) AvCSVPt += (jet->bdiscr * jet->GetP4().Pt());
            if(jet->bdiscr<0.8484 && jet->bdiscr>0) SumPt += jet->GetP4().Pt();

        }
    }

    if(SumPt!=0) evt_AvCSVPt=AvCSVPt/SumPt;
    evt_DEtaMaxJJ=DEtaMaxJJ;

    ////$$$$$$
    ////$$$$$$
    ////$$$$$$ Lepton and B-jets
    ////$$$$$$

    double minDRlb=99999;
    double maxDRlb=0;
    double minDRlb_invMass=-1;
    double maxDRlb_invMass=-1;

    double Ptlbmax=0;
    double DRlbmaxPt=-1;

    for(int i=0;i!=e->Bjets();++i) {
        Jet* bjet = e->GetBjet(i);
        double dr = bjet->DeltaR(leadLep);
        double mass = (bjet->GetP4() + leadLep->GetP4()).M();
        double pt = (bjet->GetP4() + leadLep->GetP4()).Pt();
        if(dr<minDRlb) { minDRlb=dr; minDRlb_invMass=mass; }
        if(pt>Ptlbmax) { DRlbmaxPt=dr; Ptlbmax=pt; }
    }

    if(trailLep) {
        for(int i=0;i!=e->Bjets();++i) {
            Jet* bjet = e->GetBjet(i);
            double dr = bjet->DeltaR(trailLep);
            double mass = (bjet->GetP4() + trailLep->GetP4()).M();
            double pt = (bjet->GetP4() + trailLep->GetP4()).Pt();
            if(dr<minDRlb) { minDRlb=dr; minDRlb_invMass=mass; }
            if(pt>Ptlbmax) { DRlbmaxPt=dr; Ptlbmax=pt; }
        }
    }

    evt_minDRlb_invMass=minDRlb_invMass;
    evt_DRlbmaxPt=DRlbmaxPt;

    if(e->Bjets()>0) {
        Jet * bj1 = e->GetBjet(0);
        if (bj1 != NULL) {
            float deltaEtalb= bj1->DeltaEta(*leadLep);
            float deltaPhilb= bj1->DeltaPhi(*leadLep);
            float deltaRlb = bj1->DeltaR(*leadLep);

            evt_DRl1b1=deltaRlb;
            if(trailLep) {
                float deltaEtal2b= bj1->DeltaEta(*trailLep);
                float deltaPhil2b= bj1->DeltaPhi(*trailLep);
                float deltaRl2b = bj1->DeltaR(*trailLep);
                evt_DRl2b1=deltaRl2b;
            }
        }
    }


    ////$$$$$$
    ////$$$$$$
    ////$$$$$$ Loop over central jets
    ////$$$$$$

    if(do1lAnalysis and e->NcentralJets() >= 3 ) {

        double PtJJJmax=0;
        double MJJJmaxPt=-1;
        double AvDRJJJmaxPT=-1;

        //        int indexi=-1;
        //        int indexj=-1;
        //        int indexk=-1;

        for(int i=0;i!=e->NcentralJets();++i) {
            Jet* jet_i = e->GetCentralJet(i);
            for(int j=0;j!=e->NcentralJets();++j) {
                if(i==j) continue;
                Jet* jet_j = e->GetCentralJet(j);
                double dr_ij = jet_i->DeltaR(jet_j);
                for(int k=0;k!=e->NcentralJets();++k) {

                    if(i==k) continue;
                    if(j==k) continue;
                    Jet* jet_k = e->GetCentralJet(k);
                    double dr_kj = jet_k->DeltaR(jet_j);
                    double dr_ki = jet_k->DeltaR(jet_i);
                    double mass = (jet_i->GetP4() + jet_j->GetP4() + jet_k->GetP4()).M();
                    double pt = (jet_i->GetP4() + jet_j->GetP4() + jet_k->GetP4()).Pt();
                    if(pt>Ptlbmax) { MJJJmaxPt=mass; PtJJJmax=pt; AvDRJJJmaxPT = (dr_kj+dr_ki + dr_ij)/3;}

                }
            }
        }

        evt_MJJJmaxPt = MJJJmaxPt;
        evt_AvDRJJJmaxPt = AvDRJJJmaxPT;

    }

    ////$$$$$$
    ////$$$$$$
    ////$$$$$$ Below for dileptons only
    ////$$$$$$

    if(leadLep==NULL) return;
    if(trailLep==NULL) return;

    evt_MT2ll=ChargedHiggs::mt2(leadLep->GetP4(),trailLep->GetP4(),e->GetMet().GetP4());

    if(e->Bjets()>0) {

        TLorentzVector p4W = e->GetMet().GetP4()+leadLep->GetP4();
        TLorentzVector p4W2 = e->GetMet().GetP4()+trailLep->GetP4();

        evt_MTmin=min(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
        evt_MTmax=max(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4()),ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4()));
        evt_MTtot=sqrt(ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4())*ChargedHiggs::mtMassive(p4W,e->GetBjet(0)->GetP4())  +
                       ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4())*ChargedHiggs::mtMassive(p4W2,e->GetBjet(0)->GetP4())
                       );
    }

    ////$$$$$$
    ////$$$$$$
    ////$$$$$$
    ////$$$$$$

}



void ChargedHiggsTopBottom::leptonPlot(Event*e, string label, string category, string systname, string phasespace) {

    bool filldo1l=(do1lAnalysis && ( (category.find("_1Mu")  !=string::npos ) || ( category.find("_1Ele") !=string::npos ) ));
    bool filldo2l=(do2lAnalysis && ( (category.find("_2Mu")  !=string::npos ) || ( category.find("_2Ele") !=string::npos ) || ( category.find("_1Mu1Ele")  !=string::npos ) ));

    if(not (filldo1l || filldo2l)) return;

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonPt_"+label,systname,leadLep->Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonEta_"+label,systname,leadLep->Eta(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonIso_"+label,systname,leadLep->Isolation(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonMiniIso_"+label,systname,leadLep->MiniIsolation(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonMva_"+label,systname,leadLep->Mva(),e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ptW_"+label,systname, (e->GetMet().GetP4()+leadLep->GetP4()).Pt(), e->weight() );
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Mt_"+label,systname, evt_MT, e->weight() );
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+label,systname, e->GetMet().Pt(), e->weight() );

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+label,systname, evt_ST ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+label,systname, evt_ST ,e->weight());

    if(bdt.size()>0) {
        if(do1lAnalysis && bdt.size()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+label,systname,bdt[0],e->weight());
        if(do1lAnalysis && bdt.size()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+label,systname,bdt[1],e->weight());
        if(do1lAnalysis && bdt.size()>2) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+label,systname,bdt[2],e->weight());
        if(do2lAnalysis && bdt.size()>3) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+label,systname,bdt[3],e->weight());
        if(do2lAnalysis && bdt.size()>4) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+label,systname,bdt[4],e->weight());
        if(do2lAnalysis && bdt.size()>5) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+label,systname,bdt[5],e->weight());
    } else {
        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+label,systname,-1,e->weight());
        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+label,systname,-1,e->weight());
        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+label,systname,-1,e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+label,systname,-1,e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+label,systname,-1,e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+label,systname,-1,e->weight());
    }

    if(do1lAnalysis) return;
    if(trailLep==NULL) return;

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailPt_"+label,systname,trailLep->Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailEta_"+label,systname,trailLep->Eta(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailIso_"+label,systname,trailLep->Isolation(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailMiniIso_"+label,systname,trailLep->MiniIsolation(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeptonTrailMva_"+label,systname,trailLep->Mva(),e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MT2ll_"+label, systname, evt_MT2ll , e->weight() );

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonM_"+label,systname,(trailLep->GetP4() + leadLep->GetP4()).M(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonPT_"+label,systname,(trailLep->GetP4() + leadLep->GetP4()).Pt(),e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DiLeptonDeltaR_"+label,systname,leadLep->DeltaR(*trailLep),e->weight());

    if(e->Bjets()==0) return;

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

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+label,systname, evt_HT ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+label,systname, e->NcentralJets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+label,systname, e->NforwardJets() ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+label,systname, e->Bjets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+label,systname, e->Njets() ,e->weight());

    ///////////
    //// STUDY third b jet ()ordered by csv
    ////

    vector<pair<float,int> > valid; // csv, idx

    for(int i = 0 ; i<e->NcentralJets() ;++i)
        {
            valid.push_back(pair<float,int>(e->GetCentralJet(i)->bdiscr,i));
        }

    std::sort(valid.begin(), valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;
    /*
    if (valid.size() != 0 ) {
        for(int i=0;i!=e->NcentralJets();++i) {
            cout << " e->GetCentralJet discr=" << valid[i].first << " sec=" <<  valid[i].second  << endl;
        }
    }
    */

    if(e->NcentralJets()>=3) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+label,systname,  valid[2].first,e->weight());

    ///////////
    //// STUDY various jets properties
    //// light jet to make the top hadronic top

    if(do1lAnalysis) {

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

    }

    ///////////
    //// STUDY various jets properties
    //// B jets angular separation

    if(e->Bjets()>1) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+label, systname, evt_minDRbb , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+label, systname, evt_minDRbb_invMass , e->weight() );
        //        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb_"+label, systname, evt_MT2bb , e->weight() );
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MT2bb1l_"+label, systname, evt_MT2bb1l , e->weight() );

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

    //    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NextOverLeadingBPt_"+label,systname, e->GetBjet(1)->Pt()/e->GetBjet(0)->Pt() ,e->weight());
    //    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/LeadingBPt_OverHT_"+label,systname, e->GetBjet(0)->Pt()/evt_HT ,e->weight());

    if(e->Bjets()>1)         Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMaxBB_"+label, systname, evt_DEtaMaxBB , e->weight() );

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMax_"+label, systname, evt_DEtaMaxJJ , e->weight() );

    ///////$$$$$$
    ///////$$$$$$
    ///////$$$$$$

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dRlb_"+label, systname, evt_DRl1b1 , e->weight() );
    //        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtalb_"+label, systname, deltaEtalb , e->weight() );
    //        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhilb_"+label, systname, abs(deltaPhilb) , e->weight() );

    //            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtal2b_"+label, systname, deltaEtal2b , e->weight() );
    //            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhil2b_"+label, systname, abs(deltaPhil2b) , e->weight() );
    if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dRl2b_"+label, systname, evt_DRl2b1 , e->weight() );


    ///////$$$$$$
    ///////$$$$$$
    ///////$$$$$$


    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_mass_"+label, systname, evt_minDRlb_invMass , e->weight() );
    //    if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRlb_"+label, systname, evt_minDRlb , e->weight() );

    if(e->Bjets()>0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DRlbmaxPT_"+label, systname, evt_DRlbmaxPt , e->weight() );

    if(e->NcentralJets()>2 && do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/MJJJmaxPT_"+label, systname, evt_MJJJmaxPt , e->weight() );
    if(e->NcentralJets()>2 && do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/AvDRJJJmaxPT_"+label, systname, evt_AvDRJJJmaxPt , e->weight() );

    if(e->NcentralJets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/AvCSVPt_"+label, systname, evt_AvCSVPt , e->weight() );

}


void ChargedHiggsTopBottom::classifyHF(Event*e, string label, string category, string systname, string phasespace, string Sregion) {


    if((label.find("TTTo2L2Nu_TuneCUETP8M2")!=string::npos) || (label.find("TTToSemilepton_TuneCUETP8M2")!=string::npos)
       || (label.find("TTTo2L2Nu_ttbbFilter")!=string::npos) || (label.find("TTToSemilepton_ttbbFilter")!=string::npos)
       || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=string::npos)
       || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrdown")!=string::npos)
       || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrup")!=string::npos)
       || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrdown")!=string::npos)
       || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrup")!=string::npos)
       || (label.find("TTJets")!=string::npos)
       ) {

        //https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc#L35
        string LabelHF="other_";
        if((e->GetGenTtbarId()%100)==55 || (e->GetGenTtbarId()%100)==54) LabelHF="tt2bMerged_";
        if((e->GetGenTtbarId()%100)==53) LabelHF="tt2b_";
        if((e->GetGenTtbarId()%100)==52 || (e->GetGenTtbarId()%100)==51) LabelHF="tt1b_";
        if((e->GetGenTtbarId()%100)==45 || (e->GetGenTtbarId()%100)==44 || (e->GetGenTtbarId()%100)==43 || (e->GetGenTtbarId()%100)==42 || (e->GetGenTtbarId()%100)==41) LabelHF="ttc_";
        if((e->GetGenTtbarId()%100)==00) LabelHF="ttlight_";

        if(not(
               (e->GetGenTtbarId()%100)==55 || (e->GetGenTtbarId()%100)==54 || (e->GetGenTtbarId()%100)==53 || (e->GetGenTtbarId()%100)==52 || (e->GetGenTtbarId()%100)==51 ||
               (e->GetGenTtbarId()%100)==45 || (e->GetGenTtbarId()%100)==44 || (e->GetGenTtbarId()%100)==43 || (e->GetGenTtbarId()%100)==42 || (e->GetGenTtbarId()%100)==41 ||
               (e->GetGenTtbarId()%100)==0
               )
           ) cout << "$$$$$$$$$$$$$$$    ===> other is " << e->GetGenTtbarId() << endl;

        vector<pair<float,int> > valid; // csv, idx
        for(int i = 0 ; i<e->NcentralJets() ;++i)
            {
                valid.push_back(pair<float,int>(e->GetCentralJet(i)->bdiscr,i));
            }

        std::sort(valid.begin(), valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

        /*
        if (valid.size() != 0 ) {
            for(int i=0;i!=e->NcentralJets();++i) {
                //                cout << " e->GetCentralJet discr=" << valid[i].first << " sec=" <<  valid[i].second  << endl;
            }
        }
        */

        if(!(Sregion.find("SR")!=string::npos)) {
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+Sregion+LabelHF+label,systname, e->Bjets() ,e->weight());
            if(e->NcentralJets()>=3 && valid.size() != 0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+Sregion+LabelHF+label,systname, valid[2].first ,e->weight());

            if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_"+Sregion+LabelHF+label, systname, evt_minDRbb ,e->weight());
            if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/minDRbb_mass_"+Sregion+LabelHF+label, systname, evt_minDRbb_invMass ,e->weight());
            if(e->Bjets()>1) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/DEtaMaxBB_"+Sregion+LabelHF+label,systname, evt_DEtaMaxBB ,e->weight());
        }

        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+Sregion+LabelHF+label,systname, evt_ST ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());

        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+Sregion+LabelHF+label,systname, evt_ST ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());

        /////
        /////
        /////

        if(bdt.size()>0) {
            //
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+Sregion+LabelHF+label,systname,bdt[0],e->weight());
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+Sregion+LabelHF+label,systname,bdt[1],e->weight());
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+Sregion+LabelHF+label,systname,bdt[2],e->weight());
            //
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+Sregion+LabelHF+label,systname,bdt[3],e->weight());
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+Sregion+LabelHF+label,systname,bdt[4],e->weight());
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+Sregion+LabelHF+label,systname,bdt[5],e->weight());
        } else {
            //
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt1_"+Sregion+LabelHF+label,systname,-1,e->weight());
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2_"+Sregion+LabelHF+label,systname,-1,e->weight());
            if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt3_"+Sregion+LabelHF+label,systname,-1,e->weight());
            //
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt4_"+Sregion+LabelHF+label,systname,-1,e->weight());
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt5_"+Sregion+LabelHF+label,systname,-1,e->weight());
            if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt6_"+Sregion+LabelHF+label,systname,-1,e->weight());
        }

        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lh_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[6],bdt[7]),e->weight());
        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1lm_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[8],bdt[9]),e->weight());
        if(do1lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_1ll_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[10],bdt[11]),e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lh_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[12],bdt[13]),e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2lm_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[14],bdt[15]),e->weight());
        if(do2lAnalysis) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/bdt2D_2ll_"+Sregion+LabelHF+label,systname,bin_->GetCluster(bdt[16],bdt[17]),e->weight());


    }

}


void ChargedHiggsTopBottom::printSynch(Event*e) {

    // note the METfilters
    // NB to change miniISO,ptCutEle + tausID + Bjets medium
    std::cout << "=======================================" << std::endl;

    //    std::cout << "event=" << e->eventNum() << " ntaus=" << e->Ntaus() << std::endl;

    //    if( e->eventNum()==1741933 || e->eventNum()==1741934 || e->eventNum()==1741940 || e->eventNum()==1741946 || e->eventNum()==1741948) {
    //    if( e->eventNum()==1741948) {
    //        if(e->Ntaus()>0) std::cout << " tauPt=" << e->GetTau(0)->Pt() << " tauEta=" << e->GetTau(0)->Eta() << " tauPhi=" << e->GetTau(0)->Phi();
    //        if(e->Ntaus()>1) std::cout << " tauPt=" << e->GetTau(1)->Pt() << " tauEta=" << e->GetTau(1)->Eta() << " tauPhi=" << e->GetTau(1)->Phi();


    std::cout << "run=" << e->runNum() << " lumi=" << e->lumiNum() << " evt=" << e->eventNum()<< std::endl;

    if(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or e->IsTriggered("HLT_Photon165_HE10") or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v")) std::cout << "passEleTrigger(Ele27_eta2p1_WPTight_Gsf or Ele105_CaloIdVT_GsfTrkIdT or Photon165_HE10 or Ele35_WPLoose_Gsf)" << std::endl;
    if((e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") or e->IsTriggered("HLT_Mu50_v"))) std::cout << "passTrigger(IsoMu24 and IsoTkMu24 and Mu50)" << std::endl;
    //        if(e->GetName().find("SingleElectron")!=string::npos and e->IsTriggered("HLT_Ele32_eta2p1_WPTight_Gsf_v")) std::cout << "passEleTrigger" << std::endl;
    //        if(e->GetName().find("SingleMuon")!=string::npos and (e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"))) std::cout << "passMuonTrigger" << std::endl;
    std::cout << " Nleps(pt>10, eta<2.4)=" << e->Nleps() << std::endl;
    if(leadLep != NULL) {
        std::cout << " leadLep->Pt()=" << leadLep->Pt();
        std::cout << " leadLep->Eta()=" << leadLep->Eta();
        std::cout << " leadLep->Phi()=" << leadLep->Phi();
        std::cout << " leadLep->Isolation()=" << leadLep->Isolation();
        std::cout << " leadLep->Isolation()/leadLep->Pt()=" << leadLep->Isolation()/leadLep->Pt();
        std::cout << " leadLep->MiniIsolation()=" << leadLep->MiniIsolation();
        std::cout << " leadLep->Mva()=" << leadLep->Mva();
        std::cout << " leadLep->IsMuon()=" << leadLep->IsMuon() << " leadLep->IsElectron()=" << leadLep->IsElectron();
        std::cout << " " << std::endl;
    }
    if(trailLep != NULL) {
        std::cout << " trailLep->Pt()=" << trailLep->Pt();
        std::cout << " trailLep->Eta()=" << trailLep->Eta();
        std::cout << " trailLep->Phi()=" << trailLep->Phi();
        std::cout << " trailLep->Isolation()=" << trailLep->Isolation();
        std::cout << " trailLep->MiniIsolation()=" << trailLep->MiniIsolation();
        std::cout << " trailLep->IsMuon()=" << trailLep->IsMuon() << " trailLep->IsElectron()=" << trailLep->IsElectron();
        std::cout << " " << std::endl;
    }
    std::cout << " nCentralJets(pt>40,absEta<2.4,looseId)=" << e->NcentralJets();
    std::cout << " nTaus=" << e->Ntaus();
    std::cout << " nBs=" << e->Bjets();
    std::cout << " met=" << e->GetMet().Pt();
    std::cout << " " << std::endl;

    std::cout << " genTTbar flag " << e->GetGenTtbarId() << std::endl;

    if((e->GetGenTtbarId()%100)==55 || (e->GetGenTtbarId()%100)==54 || (e->GetGenTtbarId()%100)==53) cout << "  this is ttbar + "<< endl;
    if((e->GetGenTtbarId()%100)==52 || (e->GetGenTtbarId()%100)==51) cout << "  this is ttbar + b"<< endl;
    if((e->GetGenTtbarId()%100)==44 || (e->GetGenTtbarId()%100)==43 || (e->GetGenTtbarId()%100)==42 || (e->GetGenTtbarId()%100)==41) cout << "  this is ttbar + c(c)"<< endl;
    if((e->GetGenTtbarId()%100)==00) cout << "  this is ttbar + light"<< endl;

    //    if(e->Ntaus()>0) std::cout << " tauPt=" << e->GetTau(0)->Pt() << " tauEta=" << e->GetTau(0)->Eta() << " tauPhi=" << e->GetTau(0)->Phi();
    //    if(e->Ntaus()>1) std::cout << " tauPt=" << e->GetTau(1)->Pt() << " tauEta=" << e->GetTau(1)->Eta() << " tauPhi=" << e->GetTau(1)->Phi();
    //        std::cout << " totalweight=" << e->weight() ;
    //        std::cout << " systname=" << systname << std::endl;

    for(int i=0;i!=min(e->NcentralJets(),10);++i) {
        std::cout << "    pt[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Pt() << " eta[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Eta() << " phi[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Phi() << std::endl;
    }

    //    std::cout << "=======================================" << std::endl;

    //    }

}


int ChargedHiggsTopBottom::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsTopBottom]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif

    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsTopBottom]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    /*
    std::cout << e->GetName() << std::endl;
    if(e->GetName().find("SingleMuon")!=string::npos) std::cout << "this should be SingleMuon" << std::endl;
    if(e->GetName().find("SingleElectron")!=string::npos) std::cout << "this should be SingleElectron" << std::endl;

    std::cout << "---------------------------------------------------------------------------" << std::endl;

    std::cout << "run=" << e->runNum() << " lumi=" << e->lumiNum() << " evt=" << e->eventNum() << " systname" << systname << " label=" << label << std::endl;

    for(int i=0;i!=min(e->Nleps(),10);++i) {
        std::cout << " lep fromP4   pt[" <<i<<"]="<< e->GetLepton(i)->GetP4().Pt() << " eta[" <<i<<"]="<< e->GetLepton(i)->GetP4().Eta() << std::endl;
        std::cout << " lep fromPt   pt[" <<i<<"]="<< e->GetLepton(i)->Pt() << " eta[" <<i<<"]="<< e->GetLepton(i)->Eta() << std::endl;
    }


    for(int i=0;i!=min(e->NcentralJets(),10);++i) {
        std::cout << " jet fromP4   pt[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Pt() << " eta[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Eta() << std::endl;
        std::cout << " jet fromPt   pt[" <<i<<"]="<< e->GetCentralJet(i)->Pt() << " eta[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Eta() << std::endl;
    }
    */

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
    //    doSynch
    double LeadingLeptonElePt_= 35; // singleLepton
    if(doICHEP) LeadingLeptonElePt_= 30; // singleLepton
    double NextLeadingLeptonPt_= 10; //

    for(int i=0;i!=e->Nleps();++i) {
        Lepton *it = e->GetLepton(i);

        //        bool muon=(it->IsMuon() and it->Pt()>LeadingLeptonPt_ and it->IsMedium() and (it->Isolation() < 0.15*it->GetP4().Pt()) and leadLep==NULL );
        //        bool ele=(it->IsElectron() and it->Pt()>LeadingLeptonElePt_ and it->IsTight() and leadLep==NULL );

        bool muon=(it->IsMuon() and it->Pt()>LeadingLeptonPt_ and it->IsMedium() and it->MiniIsolation() < 0.1 and leadLep==NULL );
        bool ele=(it->IsElectron() and it->Pt()>LeadingLeptonElePt_ and it->IsEleMvaTight() and it->MiniIsolation() < 0.1 and leadLep==NULL );

        if(muon or ele) {

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


    if (!e->IsRealData()) {
        // MC
        /*
        passTriggerMu=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"));
        //        passTriggerEle=(e->IsTriggered("HLT_Ele32_eta2p1_WPTight_Gsf_v")); // added later, Julie
        //        passTriggerEle=(e->IsTriggered("HLT_Ele27_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele30_WPTight_Gsf_v")); // Ele27_WPTight in runC/runH 2.0 ntuples
        passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v")); // asked Dominick , used in stop
        */

        // to add in next ntuples HLT_TkMu50_v and HLT_Mu55_v Mu33
        passTriggerMu=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") or e->IsTriggered("HLT_Mu50_v"));
        //        passTriggerEle=(e->IsTriggered("HLT_Ele32_eta2p1_WPTight_Gsf_v"));
        //        passTriggerEle=(e->IsTriggered("HLT_Ele27_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele30_WPTight_Gsf_v"));
        passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or e->IsTriggered("HLT_Photon165_HE10") or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v"));

        passTriggerMuEle=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") or e->IsTriggered("HLT_Mu50_v") or
                          e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v") or e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v") or
                          e->IsTriggered("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or e->IsTriggered("HLT_Photon165_HE10") or
                          e->IsTriggered("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v") or e->IsTriggered("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v") or
                          e->IsTriggered("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v") or e->IsTriggered("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v")
                          );

        passTriggerEleEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v") or e->IsTriggered("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or
                           e->IsTriggered("HLT_Photon165_HE10") or
                           e->IsTriggered("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v")
                           );

        // to add in next ntuples HLT TkMu17 TrkIsoVVL TkMu8 TrkIsoVVL DZ
        passTriggerMuMu=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") or e->IsTriggered("HLT_Mu50_v") or
                         e->IsTriggered("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v") or e->IsTriggered("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v") or
                         e->IsTriggered("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v") or e->IsTriggered("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")
                         );


    } else {
        // DATA
        /*
        if(e->GetName().find("SingleMuon")!=string::npos) passTriggerMu=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"));
        //        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele32_eta2p1_WPTight_Gsf_v")); // added later, Julie
        //        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele30_WPTight_Gsf_v")); //Ele27_WPTight exist in runC
        //        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v")); // asked Dominick , used in stop
        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v")); // asked Dominick , used in stop
        //        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_WPTight_Gsf_v")); // for the moment in the v2.0 tag
        */

        if(e->GetName().find("SingleMuon")!=string::npos) passTriggerMu=(e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") or e->IsTriggered("HLT_Mu50_v"));
        if(e->GetName().find("SingleElectron")!=string::npos) passTriggerEle=(e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") or e->IsTriggered("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or e->IsTriggered("HLT_Photon165_HE10") or e->IsTriggered("HLT_Ele35_WPLoose_Gsf_v"));

    }

    if(doICHEP) {
        // do not apply in MC
        passTriggerMu=true;
        passTriggerEle=true;

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
    }

    /*
    cout << " trigger passTriggerMu=" << passTriggerMu << endl;
    cout << " HLT_IsoTkMu24_v" << e->IsTriggered("HLT_IsoTkMu24_v");
    cout << " HLT_IsoMu24_v" << e->IsTriggered("HLT_IsoMu24_v") << endl;

    cout << " passTriggerEle=" << passTriggerEle;
    cout << "Ele27_eta2p1_WPTight_Gsf_v=" << e->IsTriggered("HLT_Ele27_eta2p1_WPTight_Gsf_v") << endl;
    */

    CutSelector cut;
    cut.SetMask(MaxCut-1);
    cut.SetCutBit(Total);
    if(do1lAnalysis) {
        Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_1Mu/CutFlow_"+label,systname,0,e->weight());
        Fill("ChargedHiggsTopBottom/CutFlow_1Ele/CutFlow_"+label,systname,0,e->weight());
        ///

        if(!e->IsRealData() && (systname.find("NONE")    !=string::npos)) {
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
        if(!e->IsRealData() && (systname.find("NONE")    !=string::npos)) {
            Fill("ChargedHiggsTopBottom/CutFlowNoWei/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_2Mu/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_1Mu1Ele/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
            Fill("ChargedHiggsTopBottom/CutFlowNoWei_2Ele/CutFlowNoWei_"+label,systname,0,e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents());
        }
    }

    string category="";
    evt_MT=e->Mt(Event::MtLepton);

    ////// --> 1l analysis
    if((do1lAnalysis or doTaulAnalysis) && leadLep!=NULL) {

        // MARIA temporary commentig triggers
        //        bool onemu=(leadLep->IsMuon());
        //        bool oneele=(leadLep->IsElectron() and fabs(leadLep->Eta())<2.1);

        bool onemu=(leadLep->IsMuon() and passTriggerMu);
        bool oneele=(leadLep->IsElectron() and fabs(leadLep->Eta())<2.1 and passTriggerEle);

        if ( leadLep->Pt()>LeadingLeptonPt_ && (onemu or oneele)) {
            cut.SetCutBit(OneLep); // one lep
        }
        if( cut.passAllUpTo(OneLep) ) {
            if(onemu) {
                category="_1Mu";
                // RECO
                if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                // ID
                if (not e->IsRealData()) { e->SetPtEtaSF("muID",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muID"); }
                // ISO
                if (not e->IsRealData()) { e->SetPtEtaSF("muISO",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muISO"); }
                // TRG
                if (not e->IsRealData()) { e->SetPtEtaSF("muTRG",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muTRG"); }
            }
            if(oneele) {
                category="_1Ele";
                // ID-MVA
                if (not e->IsRealData()) { e->SetPtEtaSF("eleTight",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTight"); }
                // miniIso
                if (not e->IsRealData()) { e->SetPtEtaSF("eleTightIso",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTightIso"); }
                // TRG
                if (not e->IsRealData()) { e->SetPtEtaSF("eleTightTRG",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTightTRG"); }
            }
        }

        if ( e->Nleps() == 1 ) cut.SetCutBit(NoSecondLep); // only one lep <--- kill ttbar dilepton ttbar
        if ( nTau == 0 ) cut.SetCutBit(NoTau); // tau Veto <--- dilepton killer

    }

    ////// --> 2l analysis
    if((do2lAnalysis or doTaulAnalysis) and leadLep!=NULL and trailLep!=NULL) {

        // MARIA temporary commentig triggers
        //        bool twomu=(leadLep->IsMuon() and trailLep->IsMuon());
        //        bool twoele=(leadLep->IsElectron() and trailLep->IsElectron() and fabs(leadLep->Eta())<2.1);
        //        bool onemuoneele=(((leadLep->IsElectron() and trailLep->IsMuon()) ||
        //                           (trailLep->IsElectron() and leadLep->IsMuon()) ));


        bool twomu=(leadLep->IsMuon() and trailLep->IsMuon() and passTriggerMu);
        bool twoele=(leadLep->IsElectron() and trailLep->IsElectron() and fabs(leadLep->Eta())<2.1 and passTriggerEle);
        bool onemuoneele=(((leadLep->IsElectron() and trailLep->IsMuon() and trailLep->Pt()>LeadingLeptonPt_) ||
                           (trailLep->IsElectron() and leadLep->IsMuon())) and passTriggerMu);


        if ( nOSLepPair == 1 and leadLep->Pt()>LeadingLeptonPt_ and (twomu or twoele or onemuoneele) ) {
            cut.SetCutBit(OneLep); // one OS lepton Pair + leading above trigger threshould
        }
        if( cut.passAllUpTo(OneLep) ) {
            if(twomu) {
                category="_2Mu";
                // leading RECO-medium ISO-tight; subleading RECO-loose ISO-tight
                // TRG
                if (not e->IsRealData()) { e->SetPtEtaSF("muTRG",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muTRG"); }
                // RECO
                if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                // ID-leading
                if (not e->IsRealData()) { e->SetPtEtaSF("muID",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muID"); }
                // ISO-leading
                if (not e->IsRealData()) { e->SetPtEtaSF("muISO",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muISO"); }
                // RECO trailing
                if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                // ID-trailing
                if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("muIDloose",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("muIDloose"); }
                // ISO-trailing
                if (not e->IsRealData()  && trailLep->Pt()>20) { e->SetPtEtaSF("muISOloose",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("muISOloose"); }
            }
            if(twoele) {
                category="_2Ele";
                // RECO/ISO applied and ele TRG missing
                if (not e->IsRealData()) { e->SetPtEtaSF("eleTight",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTight"); }
                if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("eleLoose",trailLep->Pt(),trailLep->Eta()); e->ApplySF("eleLoose"); }

                if (not e->IsRealData()) { e->SetPtEtaSF("eleTightIso",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTightIso"); }
                if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("eleLooseIso",trailLep->Pt(),trailLep->Eta()); e->ApplySF("eleLooseIso"); }

                if (not e->IsRealData()) { e->SetPtEtaSF("eleTightTRG",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTightTRG"); }
            }
            // this 1Mu1Ele muon above the muon trigger threshould otherwise bias in the turnon
            if(onemuoneele) {
                category="_1Mu1Ele";
                if(leadLep->IsElectron() and trailLep->IsMuon()) {
                    // ele
                    if (not e->IsRealData()) { e->SetPtEtaSF("eleTight",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTight"); }
                    if (not e->IsRealData()) { e->SetPtEtaSF("eleTightIso",leadLep->Pt(),leadLep->Eta()); e->ApplySF("eleTightIso"); }
                    // mu
                    if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("muIDloose",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("muIDloose"); }
                    if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("muISOloose",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("muISOloose"); }
                    if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                    if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("muLooseTRG",trailLep->Pt(),fabs(trailLep->Eta())); e->ApplySF("muLooseTRG"); }
                }
                if(leadLep->IsMuon() and trailLep->IsElectron()) {
                    // ele
                    if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("eleLoose",trailLep->Pt(),trailLep->Eta()); e->ApplySF("eleLoose"); }
                    if (not e->IsRealData() && trailLep->Pt()>20) { e->SetPtEtaSF("eleLooseIso",trailLep->Pt(),trailLep->Eta()); e->ApplySF("eleLooseIso"); }
                    // mu
                    if (not e->IsRealData()) { e->SetPtEtaSF("muID",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muID"); }
                    if (not e->IsRealData()) { e->SetPtEtaSF("muISO",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muISO"); }
                    if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                    if (not e->IsRealData()) { e->SetPtEtaSF("muTRG",leadLep->Pt(),fabs(leadLep->Eta())); e->ApplySF("muTRG"); }
                }
            }
        }

        if ( e->Nleps() == 2 ) cut.SetCutBit(NoSecondLep); // only two lep <--- kill multiboson/ttV
        if ( true ) cut.SetCutBit(NoTau); // tau Veto -- not needed

    }

    ////// --> other cuts
    if ( e->GetMet().Pt() >= 30 ) cut.SetCutBit(Met);

    if((do1lAnalysis or doTaulAnalysis) && leadLep!=NULL) {
        cut.SetCutBit(Mt); // Mt -- not applied put in the various lepton categories
        //        if ( e->Mt(evt_MT<120) cut.SetCutBit(Mt); // dilepton Killer
    } else {
        cut.SetCutBit(Mt); // Mt -- not needed
    }
    if(do1lAnalysis or doTaulAnalysis) {
        if ( e->NcentralJets() >= 5 ) cut.SetCutBit(NJets);
    } else {
        if ( e->NcentralJets() >= 3 ) cut.SetCutBit(NJets);
    }
    if ( e->Bjets() >= 2 )  cut.SetCutBit(NB);

    ////$$$$$$$
    ////$$$$$$$ Apply SF
    ////$$$$$$$

    if(doICHEP) {
        if (not e->IsRealData() and
            ((label.find("TTJets_DiLept")!=string::npos) || (label.find("TTJets_SingleLeptFrom")!=string::npos))) {
            e->ApplyTopReweight();
        }
    } else {
        if (not e->IsRealData() and
            ((label.find("TTTo2L2Nu_TuneCUETP8M2")!=string::npos) || (label.find("TTToSemilepton_TuneCUETP8M2")!=string::npos)
             || (label.find("TTTo2L2Nu_ttbbFilter")!=string::npos) || (label.find("TTToSemilepton_ttbbFilter")!=string::npos)
             || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=string::npos)
             || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrdown")!=string::npos)
             || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-fsrup")!=string::npos)
             || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrdown")!=string::npos)
             || (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-isrup")!=string::npos)
             || (label.find("TTJets")!=string::npos)
             )) {

            e->ApplyTopReweight();
        }
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

    /*
    // used later as CSV reshaping
    if (not e->IsRealData()) {
        e->ApplyBTagSF(1);// 0=loos wp  1=medium wp
        //        e->ApplyBTagSF(0);// 0=loos wp  1=medium wp
    }
    */

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
    /*
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
    */
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


    ////$$$$$$$
    ////$$$$$$$
    //// VARIOUS SYNCH printout
    ////$$$$$$$
    ////$$$$$$$

    if(doSynch) { printSynch(e); return EVENT_NOT_USED;}

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
    //// UP TO NOW: LEPTONS selection only
    ////


    /////
    /////
    /////


    if(doSynch) { printSynch(e); return EVENT_NOT_USED;}
    if (e->Bjets() == 0) return EVENT_NOT_USED;

    bool rightCombination =true; // for all the bkg

    bool Baseline=(e->Bjets() > 0 && ( ( do1lAnalysis && e->NcentralJets() >3 ) || ( do2lAnalysis && e->NcentralJets() >1 )));
    bool BaselineTau=(e->Bjets() > 0 && ( ( trailLep==NULL && e->NcentralJets()==3 ) || ( trailLep!=NULL && e->NcentralJets()==1 )));

    auto sf=dynamic_cast<SF_CSVReweight*>(e->GetWeight()->GetSF("btag-reweight"));
    if (sf == NULL)  Log(__FUNCTION__,"ERROR","Unable to find btag reweight sf");

    for(int i=0;i!=e->NcentralJets();++i) {
        Jet* lj = e->GetCentralJet(i);
        //        sf->add_to_sf( pt_j, eta_j, csv_j, flavor_j);
        sf->add_to_sf(lj->GetP4().Pt(),lj->GetP4().Eta(),lj->bdiscr,lj->Flavor());
    }

    sf->set();
    if ( not e->IsRealData() ) e->ApplySF("btag-reweight");

    if( Baseline or (doTaulAnalysis && BaselineTau )) {

        ////////
        ////
        //// Fill gen Info
        ////
        
        if ( not e->IsRealData() ) {
            nGenB = genInfoForBKG(e);
        }

        if ( not e->IsRealData() and (label.find("HplusToTB")  !=string::npos ) ){
            
            rightCombination=false; // reset for Higgs
            rightCombination=genInfoForSignal(e); // compute the right combination in the higgs case
            
            //            if(bFromTopH!=NULL) cout << " found bFromTopH->GetP4().Pt() " << bFromTopH->GetP4().Pt() << endl;
            /*
            ///// GENERATION
            if(bAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbAss_"+label,systname,bAss->GetP4().Pt(),e->weight());
            if(bFromTopH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromTopH_"+label,systname,bFromTopH->GetP4().Pt(),e->weight());
            if(bFromTopAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromTopAss_"+label,systname,bFromTopAss->GetP4().Pt(),e->weight());
            if(bFromH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/ptbFromH_"+label,systname,bFromH->GetP4().Pt(),e->weight());
            
            if(bAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabAss_"+label,systname,bAss->GetP4().Eta(),e->weight());
            if(bFromTopH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromTopH_"+label,systname,bFromTopH->GetP4().Eta(),e->weight());
            if(bFromTopAss!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromTopAss_"+label,systname,bFromTopAss->GetP4().Eta(),e->weight());
            if(bFromH!=NULL) Fill("ChargedHiggsTopBottom/GENERATOR/etabFromH_"+label,systname,bFromH->GetP4().Eta(),e->weight());
            */

        }
        
        ////////
        ////
        //// compute main variables
        ////

        computeVar(e);

        ////////
        ////
        //// Read MVA
        ////
        
        bdt.clear();

        if(do2lAnalysis) SetVariable("mtMin",evt_MTmin);
        if(do1lAnalysis) SetVariable("MJJJmaxPt",evt_MJJJmaxPt);

        SetVariable("ht",evt_HT);
        SetVariable("met_pt",e->GetMet().GetP4().Pt());
        SetVariable("lep1_pt",leadLep->GetP4().Pt());
        SetVariable("bjet_pt[0]",e->GetBjet(0)->GetP4().Pt());
        SetVariable("DRl1b1",evt_DRl1b1);
        SetVariable("MassDRlbmin",evt_minDRlb_invMass);
        SetVariable("mt",evt_MT);
        SetVariable("Cen",evt_C);
        SetVariable("HemiOut",evt_HemiMetOut);
        SetVariable("DRlbmaxPT",evt_DRlbmaxPt);
        SetVariable("st",evt_ST);
        SetVariable("AvCSVPt",evt_AvCSVPt);


        if( e->Bjets()>1 ) {
            SetVariable("DEtaMaxBB",evt_DEtaMaxBB);
            SetVariable("DRbbmin",evt_minDRbb);
            SetVariable("MassDRbbmin",evt_minDRbb_invMass);
            SetVariable("mt2bb1l",evt_MT2bb1l);  // remove ??
        } else {
            SetVariable("DRbbmin",3.);
            SetVariable("DEtaMaxBB",1.5);
            SetVariable("MassDRbbmin",150.);
            SetVariable("mt2bb1l",50); // remove ??
        }


        //    vector<float> bdt;
        for(unsigned i =0 ;i< readers_.size() ; ++i)
            {
                    bdt.push_back(readers_[i]->EvaluateMVA("BDTG") );
            }
        

        //        cout << "bla6,7 = " << bin_->GetCluster(bdt[6],bdt[7]) << endl;

    }

    ////////
    ////
    //// Fill tree
    ////

    if (systname.find("NONE")    !=string::npos) {
    // Andrea test
    //    if (systname.find("JESUp")    !=string::npos) {
        if( e->Bjets() > 0 && ( ( do1lAnalysis && e->NcentralJets() >3 ) || ( do2lAnalysis && e->NcentralJets() >1 ))) {
            if(writeTree) setTree(e,label,category);
            if(writeTree) FillTree("tree_tb");
        }
    }

    if (systname.find("NONE")    !=string::npos) {
        //        std::cout << "doTaulAnalysis" << doTaulAnalysis << std::endl;
        // this target the 1l final state
        // >=3 jets for 1l and >=1 jets for the 2l
        bool taul1L=(trailLep==NULL);
        bool taul2L=(trailLep!=NULL);
        if( doTaulAnalysis &&  BaselineTau > 0 &&  (taul1L or taul2L) && evt_MT>=120) {
            //            cout << "filling tree "<< endl;
            if(writeTree) setTree(e,label,category);
            if(writeTree) FillTree("tree_tb");
        }
    }

    ////////
    ////
    //// Control regions
    ////

    bool charmCR=false;
    bool extraRadCR=false;
    bool topCR=false;
    bool topCRR4=false;
    bool topCRR5=false;
    bool extraRadCRR7=false;
    bool extraRadCRR10=false;
    //    bool highMTCR=false;
    //    bool lowMTCR=false;

    // 4jets, ==3b
    if( do1lAnalysis and e->NcentralJets() == 4 and e->Bjets() == 3 ) charmCR=true;
    if( do1lAnalysis and e->NcentralJets() >= 5 and e->Bjets() == 1 ) extraRadCR=true;
    if( do2lAnalysis and e->NcentralJets() >= 3 and e->Bjets() == 1 ) extraRadCR=true;

    if( do1lAnalysis and e->NcentralJets() == 5 and e->Bjets() == 1 ) extraRadCRR7=true;
    if( do2lAnalysis and e->NcentralJets() == 3 and e->Bjets() == 1 ) extraRadCRR7=true;
    if( do1lAnalysis and e->NcentralJets() >5 and e->Bjets() == 1 ) extraRadCRR10=true;
    if( do2lAnalysis and e->NcentralJets() >3 and e->Bjets() == 1 ) extraRadCRR10=true;

    if( do1lAnalysis and e->NcentralJets() == 4 and e->Bjets() > 0 and e->Bjets() < 3 ) topCR=true;
    if( do2lAnalysis and e->NcentralJets() == 2 and e->Bjets() > 0 ) topCR=true;

    if( do1lAnalysis and e->NcentralJets() == 4 and e->Bjets() ==1 and e->Bjets() < 3 ) topCRR4=true;
    if( do2lAnalysis and e->NcentralJets() == 2 and e->Bjets() ==1 ) topCRR4=true;

    if( do1lAnalysis and e->NcentralJets() == 4 and e->Bjets() > 1 and e->Bjets() < 3 ) topCRR5=true;
    if( do2lAnalysis and e->NcentralJets() == 2 and e->Bjets() > 1 ) topCRR5=true;

    //    if( do1lAnalysis && evt_MT>=120 ) highMTCR=true;
    //    if( do1lAnalysis && evt_MT<120 ) lowMTCR=true;

    if(topCR) jetPlot(e, label, category, systname,"topCR");
    if(topCR) leptonPlot(e,label, category, systname,"topCR");
    if(topCR) eventShapePlot(e, label, category, systname,"topCR");
    if(topCR) classifyHF(e, label, category, systname,"topCR","");

    if(topCRR4) jetPlot(e, label, category, systname,"topCRR4");
    if(topCRR4) leptonPlot(e,label, category, systname,"topCRR4");
    if(topCRR4) eventShapePlot(e, label, category, systname,"topCRR4");
    if(topCRR4) classifyHF(e, label, category, systname,"topCRR4","");

    if(topCRR5) jetPlot(e, label, category, systname,"topCRR5");
    if(topCRR5) leptonPlot(e,label, category, systname,"topCRR5");
    if(topCRR5) eventShapePlot(e, label, category, systname,"topCRR5");
    if(topCRR5) classifyHF(e, label, category, systname,"topCRR5","");

    /////

    if(extraRadCR) jetPlot(e, label, category, systname,"extraRadCR");
    if(extraRadCR) leptonPlot(e, label, category, systname,"extraRadCR");
    if(extraRadCR) eventShapePlot(e, label, category, systname,"extraRadCR");
    if(extraRadCR) classifyHF(e, label, category, systname,"extraRadCR","");

    if(extraRadCRR10) jetPlot(e, label, category, systname,"extraRadCRR10");
    if(extraRadCRR10) leptonPlot(e, label, category, systname,"extraRadCRR10");
    if(extraRadCRR10) eventShapePlot(e, label, category, systname,"extraRadCRR10");
    if(extraRadCRR10) classifyHF(e, label, category, systname,"extraRadCRR10","");

    if(extraRadCRR7) jetPlot(e, label, category, systname,"extraRadCRR7");
    if(extraRadCRR7) leptonPlot(e, label, category, systname,"extraRadCRR7");
    if(extraRadCRR7) eventShapePlot(e, label, category, systname,"extraRadCRR7");
    if(extraRadCRR7) classifyHF(e, label, category, systname,"extraRadCRR7","");

    /////

    if(do1lAnalysis && charmCR) jetPlot(e, label, category, systname,"charmCR");
    if(do1lAnalysis && charmCR) leptonPlot(e, label, category, systname,"charmCR");
    if(do1lAnalysis && charmCR) eventShapePlot(e, label, category, systname,"charmCR");
    if(do1lAnalysis && charmCR) classifyHF(e, label, category, systname,"charmCR","");

    ////////
    ////
    //// UP TO NOW: Baseline selection
    ////

    if(!cut.passAll() )   return EVENT_NOT_USED;

    jetPlot(e, label, category, systname,"Baseline");
    leptonPlot(e, label, category, systname,"Baseline");
    eventShapePlot(e, label, category, systname,"Baseline");
    classifyHF(e, label, category, systname,"Baseline","");

    /*
    if( do1lAnalysis and evt_MT<120  and e->Bjets()==2 and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT>=120 and e->Bjets()==2 and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT<120  and e->Bjets()>2  and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR3_"+label,systname, evt_HT ,e->weight());
    if( do1lAnalysis and evt_MT>=120 and e->Bjets()>2  and e->NcentralJets()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR4_"+label,systname, evt_HT ,e->weight());

    if( do2lAnalysis and evt_MTmin<200 and e->Bjets()==2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
    if( do2lAnalysis and evt_MTmin>=200 and e->Bjets()==2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());
    if( do2lAnalysis and evt_MTmin<200 and e->Bjets()>2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR3_"+label,systname, evt_HT ,e->weight());
    if( do2lAnalysis and evt_MTmin>=200 and e->Bjets()>2 and e->NcentralJets()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR4_"+label,systname, evt_HT ,e->weight());
    */

    /// bdt1, bdt2 is the 1l
    if( do1lAnalysis and e->Bjets()==2 and e->NcentralJets()==5) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR1_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>0) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR1_"+label,systname,bdt[0],e->weight());
            if(bdt.size()>1) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR1_"+label,systname,bdt[1],e->weight());
            if(bdt.size()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR1_"+label,systname,bdt[2],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR1_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR1_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR1_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR1_");
    }

    /// bdt1, bdt2 is the 1l
    if( do1lAnalysis and e->Bjets()==2 and e->NcentralJets()>5) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR3_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR3_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>0) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR3_"+label,systname,bdt[0],e->weight());
            if(bdt.size()>1) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR3_"+label,systname,bdt[1],e->weight());
            if(bdt.size()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR3_"+label,systname,bdt[2],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR3_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR3_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR3_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR3_");
    }

    /// bdt1, bdt2 is the 1l
    if( do1lAnalysis and e->Bjets()>2 and e->NcentralJets()==5) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR2_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>0) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR2_"+label,systname,bdt[0],e->weight());
            if(bdt.size()>1) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR2_"+label,systname,bdt[1],e->weight());
            if(bdt.size()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR2_"+label,systname,bdt[2],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR2_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR2_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR2_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR2_");
    }

    /// bdt1, bdt2 is the 1l
    if( do1lAnalysis and e->Bjets()>2 and e->NcentralJets()>5) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR4_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR4_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>0) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR4_"+label,systname,bdt[0],e->weight());
            if(bdt.size()>1) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR4_"+label,systname,bdt[1],e->weight());
            if(bdt.size()>2) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR4_"+label,systname,bdt[2],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt1_SR4_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt2_SR4_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt3_SR4_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR4_");
    }

    /////////
    /////////
    /////////
    /////////

    /// bdt1, bdt2 is the 1l
    if( do2lAnalysis and e->Bjets()==2 and e->NcentralJets()==3) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR1_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR1_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>3) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR1_"+label,systname,bdt[3],e->weight());
            if(bdt.size()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR1_"+label,systname,bdt[4],e->weight());
            if(bdt.size()>5) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR1_"+label,systname,bdt[5],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR1_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR1_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR1_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR1_");
    }

    /// bdt1, bdt2 is the 1l
    if( do2lAnalysis and e->Bjets()==2 and e->NcentralJets()>3) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR3_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR3_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>3) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR3_"+label,systname,bdt[3],e->weight());
            if(bdt.size()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR3_"+label,systname,bdt[4],e->weight());
            if(bdt.size()>5) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR3_"+label,systname,bdt[5],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR3_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR3_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR3_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR3_");
    }

    /// bdt1, bdt2 is the 1l
    if( do2lAnalysis and e->Bjets()>2 and e->NcentralJets()==3) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR2_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR2_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>3) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR2_"+label,systname,bdt[3],e->weight());
            if(bdt.size()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR2_"+label,systname,bdt[4],e->weight());
            if(bdt.size()>5) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR2_"+label,systname,bdt[5],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR2_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR2_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR2_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR2_");
    }

    /// bdt1, bdt2 is the 1l
    if( do2lAnalysis and e->Bjets()>2 and e->NcentralJets()>3) {
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/HT_SR4_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/Baseline"+category+"/ST_SR4_"+label,systname, evt_ST ,e->weight());
        if(bdt.size()>0) {
            if(bdt.size()>3) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR4_"+label,systname,bdt[3],e->weight());
            if(bdt.size()>4) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR4_"+label,systname,bdt[4],e->weight());
            if(bdt.size()>5) Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR4_"+label,systname,bdt[5],e->weight());
        } else {
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt4_SR4_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt5_SR4_"+label,systname,-1,e->weight());
            Fill("ChargedHiggsTopBottom/Baseline"+category+"/bdt6_SR4_"+label,systname,-1,e->weight());
        }
        classifyHF(e, label, category, systname,"Baseline","SR4_");
    }

    //////////////
    //////////////
    //////////////
    //////////////

    /*
    if(e->runNum()==274244 and e->lumiNum()==417 and e->eventNum()==596731964) {
        std::cout << "run=" << e->runNum() << " lumi=" << e->lumiNum() << " evt=" << e->eventNum();
        std::cout << " leadLep->Pt()=" << leadLep->Pt();
        std::cout << " leadLep->Eta()=" << leadLep->Eta();
        std::cout << " leadLep->Phi()=" << leadLep->Phi();
        std::cout << " leadLep->Isolation()=" << leadLep->Isolation();
        std::cout << " leadLep->IsMuon()=" << leadLep->IsMuon() << " leadLep->IsElectron()=" << leadLep->IsElectron();

        if((category.find("_1Mu")  !=string::npos )) cout << " ==> 1Mu=" << endl;
        if((category.find("_1Ele")  !=string::npos )) cout << " ==> 1Ele=" << endl;
        std::cout << " nbjets=" << e->Bjets() << " NcentralJets=" <<  e->NcentralJets() << endl;
        std::cout << " nTau=" << nTau << " met=" << e->GetMet().Pt() << " e->Nleps()=" << e->Nleps() << " e->Ntaus()=" << e->Ntaus() << endl;

        for(int i=0;i!=min(e->NcentralJets(),10);++i) {
            std::cout << "    pt[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Pt() << " eta[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Eta() << " phi[" <<i<<"]="<< e->GetCentralJet(i)->GetP4().Phi();
        }

        std::cout << e->GetBareJet(5)->GetP4().Pt() << " Eta=" << e->GetBareJet(5)->GetP4().Eta() << " Phi=" << e->GetBareJet(5)->GetP4().Phi()  << " IsJetExceptValidity()=" << e->GetBareJet(5)->IsJetExceptValidity() << std::endl;
    }
    */


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

    //    if (!(systname.find("NONE")!=string::npos))      return EVENT_NOT_USED;
    return EVENT_NOT_USED;

    ////////////
    /////
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
