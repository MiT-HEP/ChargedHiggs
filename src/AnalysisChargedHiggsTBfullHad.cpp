#include "interface/AnalysisChargedHiggsTBfullHad.hpp"

void ChargedHiggsTopBottomFullHad::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetPtCut(10);
    l->SetIsoCut(-1.); // absolute isolation
    //https://indico.cern.ch/event/594396/contributions/2402538/attachments/1389409/2116003/20161215_MuonPOG_GeneralPPD.pdf
    l->SetIsoRelCut(0.25); // relative isolation // for muon 0.25 is loose, 0.15 is tight
    l->SetMiniIsoRelCut(-1); // relative mini-isolation
    l->SetEtaCut(2.4);
    l->SetVetoCut(); // loosest selection of them all
    l->SetTightCut(false); // use the loose selection for now
}

void ChargedHiggsTopBottomFullHad::SetJetCuts(Jet *j){
    // https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80XReReco
    // doSynch
    j->SetBCut(0.8484); //0.8484 medium // 0.5426 loose
    j->SetEtaCut(4.7);
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(40);
    //    j->SetPtCut(30); // for MIAO
    j->SetPuIdCut(-100);
}

void ChargedHiggsTopBottomFullHad::SetTauCuts(Tau *t){
    // these are used for the Veto
    t->SetPtCut(20);
    t->SetEtaCut(2.3);
    t->SetMuRej(true);
    t->SetEleRej(true);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(2.5);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(1);  // 0=TauDecayModeFindingNewDMs 1=TauDecayModeFinding
}

void ChargedHiggsTopBottomFullHad::setTree(Event*e, string label, string category )
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

    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NcentralJets",e->NcentralJets());
    SetTreeVar("NBJets",e->Bjets());
    SetTreeVar("NFatJets",e->NFatJets());

    for(int i=0; i!=min(e->NFatJets(),5);i++){
        SetTreeVar("fatjet_pt",i,e->GetFatJet(i)->Pt());
        SetTreeVar("fatjet_eta",i,e->GetFatJet(i)->Eta());
        SetTreeVar("fatjet_phi",i,e->GetFatJet(i)->Phi());
        SetTreeVar("fatjet_e",i,e->GetFatJet(i)->E());
        SetTreeVar("fatjet_tau1",i,e->GetFatJet(i)->Tau1());
        SetTreeVar("fatjet_tau2",i,e->GetFatJet(i)->Tau2());
        SetTreeVar("fatjet_tau3",i,e->GetFatJet(i)->Tau3());
        SetTreeVar("fatjet_SDMass",i,e->GetFatJet(i)->SDMass());
        SetTreeVar("fatjet_bSubJet",i,e->GetFatJet(i)->IsSubjetBTag());
        SetTreeVar("fatjet_bSubJetLoose",i,e->GetFatJet(i)->IsSubjetBTagLoose());
        SetTreeVar("fatjet_CorrPrunedMass",i,e->GetFatJet(i)->CorrPrunedMass());
    }

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
    SetTreeVar("DRbbmin",evt_minDRbb);
    SetTreeVar("MassDRbbmin",evt_minDRbb_invMass);

    SetTreeVar("DEtaMaxBB",evt_DEtaMaxBB);

    SetTreeVar("Cen",evt_C);

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
            if(label.find("DYJetsToLL_M-50_HT") !=string::npos) mc =221 ;
            if(label.find("WJetsToLNu_HT")!=string::npos) mc =222;

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

            // QCD
            if(label.find("QCD_HT") !=string::npos) mc =500 ;

        }

    SetTreeVar("mc",mc);

    SetTreeVar("genTTid",e->GetGenTtbarId());

    if ( not e->IsRealData() and (label.find("HplusToTB")  !=string::npos ) ){
        if(WFromTopH!= NULL) {
            SetTreeVar("WFromTopH_pt",WFromTopH->Pt());
            SetTreeVar("WFromTopH_eta",WFromTopH->Eta());
            SetTreeVar("WFromTopH_phi",WFromTopH->Phi());
            SetTreeVar("WFromTopH_lep",WFromTopH_lep);
        }

        if(WFromTopAss!= NULL) {
            SetTreeVar("WFromTopAss_pt",WFromTopAss->Pt());
            SetTreeVar("WFromTopAss_eta",WFromTopAss->Eta());
            SetTreeVar("WFromTopAss_phi",WFromTopAss->Phi());
            SetTreeVar("WFromTopAss_lep",WFromTopAss_lep);
        }
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
            SetTreeVar("topFromH_lep",topFromH_lep);
        }
        if(topAss!= NULL) {
            SetTreeVar("topAss_pt",topAss->Pt());
            SetTreeVar("topAss_eta",topAss->Eta());
            SetTreeVar("topAss_phi",topAss->Phi());
            SetTreeVar("topAss_lep",topAss_lep);
        }
    }

    if( not e->IsRealData() and (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) ){
        if(WBKGplus!=NULL) {
            SetTreeVar("WBKGplus_pt",WBKGplus->Pt());
            SetTreeVar("WBKGplus_eta",WBKGplus->Eta());
            SetTreeVar("WBKGplus_phi",WBKGplus->Phi());
            SetTreeVar("WBKGplus_lep",WBKGplus_lep);
        }
        if(WBKGminus!=NULL) {
            SetTreeVar("WBKGminus_pt",WBKGminus->Pt());
            SetTreeVar("WBKGminus_eta",WBKGminus->Eta());
            SetTreeVar("WBKGminus_phi",WBKGminus->Phi());
            SetTreeVar("WBKGminus_lep", WBKGminus_lep);
        }
        if(bBKGplus!=NULL) {
            SetTreeVar("bBKGplus_pt",bBKGplus->Pt());
            SetTreeVar("bBKGplus_eta",bBKGplus->Eta());
            SetTreeVar("bBKGplus_phi",bBKGplus->Phi());
        }
        if(bBKGminus!=NULL) {
            SetTreeVar("bBKGminus_pt",bBKGminus->Pt());
            SetTreeVar("bBKGminus_eta",bBKGminus->Eta());
            SetTreeVar("bBKGminus_phi",bBKGminus->Phi());
        }
        if(topBKGplus!=NULL) {
            SetTreeVar("topBKGplus_pt",topBKGplus->Pt());
            SetTreeVar("topBKGplus_eta",topBKGplus->Eta());
            SetTreeVar("topBKGplus_phi",topBKGplus->Phi());
            SetTreeVar("topBKGplus_lep",topBKGplus_lep);
        }
        if(topBKGminus!=NULL) {
            SetTreeVar("topBKGminus_pt",topBKGminus->Pt());
            SetTreeVar("topBKGminus_eta",topBKGminus->Eta());
            SetTreeVar("topBKGminus_phi",topBKGminus->Phi());
            SetTreeVar("topBKGminus_lep",topBKGminus_lep);
        }
    }

}


void ChargedHiggsTopBottomFullHad::AddSpectator( string name, char type, int r){

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


void ChargedHiggsTopBottomFullHad::AddVariable( string name, char type, int r){
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



void ChargedHiggsTopBottomFullHad::Init()
{

    Preselection();

    // fill variables for miniTREE
    InitTree("tree_tb");

    Branch("tree_tb","sig",'I');
    Branch("tree_tb","mc",'I'); // to distinguish between the different mc

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

    // fill counter and scalar
    Branch("tree_tb","NcentralJets",'I');
    Branch("tree_tb","NJets",'I');
    Branch("tree_tb","NBJets",'I');
    Branch("tree_tb","NFatJets",'I');

    // fill event variables
    Branch("tree_tb","met_pt",'F');
    Branch("tree_tb","met_phi",'F');
    Branch("tree_tb","ht",'F');
    Branch("tree_tb","DRbbmin",'F');
    Branch("tree_tb","MassDRbbmin",'F');
    Branch("tree_tb","DEtaMaxBB",'F');
    Branch("tree_tb","Cen",'F');

    Branch("tree_tb","fatjet_pt",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_eta",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_phi",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_e",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_tau1",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_tau2",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_tau3",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_SDMass",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_CorrPrunedMass",'f',5,"NFatJets");
    Branch("tree_tb","fatjet_bSubJet",'i',5,"NFatJets");
    Branch("tree_tb","fatjet_bSubJetLoose",'i',5,"NFatJets");

    // fill all the object vector
    Branch("tree_tb","jet_pt",'f',10,"NJets");
    Branch("tree_tb","jet_eta",'f',10,"NJets");
    Branch("tree_tb","jet_phi",'f',10,"NJets");
    Branch("tree_tb","jet_e",'f',10,"NJets");
    Branch("tree_tb","jet_discr",'f',10,"NJets");

    // fill all the object vector
    Branch("tree_tb","bjet_pt",'f',10,"NBJets");
    Branch("tree_tb","bjet_eta",'f',10,"NBJets");
    Branch("tree_tb","bjet_e",'f',10,"NBJets");
    Branch("tree_tb","bjet_phi",'f',10,"NBJets");
    Branch("tree_tb","bjet_discr",'f',10,"NBJets");

    //// VARIOUS gen  INFO

    Branch("tree_tb","genTTid",'I');

    Branch("tree_tb","WFromTopH_pt",'F');
    Branch("tree_tb","WFromTopH_phi",'F');
    Branch("tree_tb","WFromTopH_eta",'F');
    Branch("tree_tb","WFromTopH_lep",'I');

    Branch("tree_tb","WFromTopAss_pt",'F');
    Branch("tree_tb","WFromTopAss_phi",'F');
    Branch("tree_tb","WFromTopAss_eta",'F');
    Branch("tree_tb","WFromTopAss_lep",'I');

    Branch("tree_tb","bFromH_pt",'F');
    Branch("tree_tb","bFromH_phi",'F');
    Branch("tree_tb","bFromH_eta",'F');

    Branch("tree_tb","bFromTopH_pt",'F');
    Branch("tree_tb","bFromTopH_phi",'F');
    Branch("tree_tb","bFromTopH_eta",'F');

    Branch("tree_tb","bFromTopAss_pt",'F');
    Branch("tree_tb","bFromTopAss_phi",'F');
    Branch("tree_tb","bFromTopAss_eta",'F');

    Branch("tree_tb","topFromH_pt",'F');
    Branch("tree_tb","topFromH_phi",'F');
    Branch("tree_tb","topFromH_eta",'F');
    Branch("tree_tb","topFromH_lep",'I');

    Branch("tree_tb","topAss_pt",'F');
    Branch("tree_tb","topAss_phi",'F');
    Branch("tree_tb","topAss_eta",'F');
    Branch("tree_tb","topAss_lep",'I');

    Branch("tree_tb","WBKGplus_pt",'F');
    Branch("tree_tb","WBKGplus_phi",'F');
    Branch("tree_tb","WBKGplus_eta",'F');
    Branch("tree_tb","WBKGplus_lep",'I');

    Branch("tree_tb","WBKGminus_pt",'F');
    Branch("tree_tb","WBKGminus_phi",'F');
    Branch("tree_tb","WBKGminus_eta",'F');
    Branch("tree_tb","WBKGminus_lep",'I');

    Branch("tree_tb","bBKGplus_pt",'F');
    Branch("tree_tb","bBKGplus_phi",'F');
    Branch("tree_tb","bBKGplus_eta",'F');
    
    Branch("tree_tb","bBKGminus_pt",'F');
    Branch("tree_tb","bBKGminus_phi",'F');
    Branch("tree_tb","bBKGminus_eta",'F');

    Branch("tree_tb","topBKGplus_pt",'F');
    Branch("tree_tb","topBKGplus_phi",'F');
    Branch("tree_tb","topBKGplus_eta",'F');
    Branch("tree_tb","topBKGplus_lep",'I');

    Branch("tree_tb","topBKGminus_pt",'F');
    Branch("tree_tb","topBKGminus_phi",'F');
    Branch("tree_tb","topBKGminus_eta",'F');
    Branch("tree_tb","topBKGminus_lep",'I');

    return;

    // MVA stuff below

    TMVA::Tools::Instance();
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader"<<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
        {
            cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
            readers_[i]->BookMVA("BDTG",weights[i].c_str());
        }
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;


}

void ChargedHiggsTopBottomFullHad::BookCutFlow(string l, string category)
{
    
    cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Histo CutFlow/Baseline" <<l<<endl;
    
    Book(    "ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),10,-.5,10-.5);
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total");
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoLep+1,"N_{lep} == 0");
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoTau+1,"N_{taus} = 0");
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(HTcut+1,"HTcut>800");
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneBOneFat+1,"OneBOneFat");
    GetHisto("ChargedHiggsTopBottom/CutFlow"+category+"/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneBOneFatMirror+1,"OneBOneFatMirror"); 
}

void ChargedHiggsTopBottomFullHad::BookFlavor(string l, string category, string phasespace, string flavor, string SR)
{
    
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+SR+flavor+l);
    AddFinalHisto("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+SR+flavor+l);

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+SR+flavor+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+SR+flavor+l,"ST "+l+"; ST ( HT+met+lepsPt )",800,0,8000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+SR+flavor+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",40,0,2000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+SR+flavor+l,"ST "+l+"; ST ( HT+met+lepsPt )",50,0,2500);

}

void ChargedHiggsTopBottomFullHad::BookGenMatch(string l, string category, string phasespace, string cut, string match)
{

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+match+cut+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
}

void ChargedHiggsTopBottomFullHad::BookGenTTBar(string l, string category, string phasespace, string labelHF){

    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_"+labelHF+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_"+labelHF+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_"+labelHF+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_"+labelHF+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);

}

void ChargedHiggsTopBottomFullHad::BookFatjetPro(string l, string category, string phasespace, string pile){


    Book("ChargedHiggsTopBottom/"+phasespace+category+"/top_pt_"+pile+l,"top_pt_ "+pile+l + "; top[GeV] ",120,400,1600); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/W_pt_"+pile+l,"W_pt_ "+pile+l + "; W [GeV]",140,200,1600); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau32_"+pile+l,"tau32_ "+pile+l + "; tau32 ",120,0.,1.2); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau21_"+pile+l,"tau21_ "+pile+l + "; tau21 ",120,0.,1.2); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_tcut_"+pile+l,"SDMass_tcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outtcut_"+pile+l,"SDMass_outtcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_wcut_"+pile+l,"SDMass_wcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outwcut_"+pile+l,"SDMass_outwcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+pile+l,"HT_ "+pile+l + "; HT[GeV] ",260,800,6000); ///ranges
}


void ChargedHiggsTopBottomFullHad::BookHisto(string l, string category, string phasespace)
{

    cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Final Histo" <<l << " category=" << category << " phasespace="  << phasespace <<endl;

    /**********************************************
     *        Book histos                         *
     **********************************************/
    
    /////
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",40,0,2000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
    
    /////
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+l,"ST "+l+"; ST ( HT+met+lepsPt )",50,0,2500);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);
    
    ///// Vertices
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+l,"Met "+l+";MET [GeV]",50,0.,1000.);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+l,"Vertices "+l + ";Number of vertices",50,0.,50.);
    
    ///// AK4 jets
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 |#eta|<=2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);

    ///// AK8 jets
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NWjets_"+l,"NWjets "+l + ";Number of W jets ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/PTWjets_"+l,"PTWjets "+l + ";pt of W jets ",100,0,500);


    ///// Higgs candidates with gen info
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b_thbh"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b_thb"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b_tb"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b_tbh"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b_other"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b_thbh"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b_thb"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b_tb"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b_tbh"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b_other"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_th_nb"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_th_bh"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_wh_nb"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_wh_bh"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_ntnw_nb"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_ntnw_bh"+"/invariant_"+l,"invariant "+l + ";Mass (wbb)  [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb_other"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj_good"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj_bad"+"/invariant_"+l,"invariant "+l + ";Mass (wbb)  [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj_other"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);


    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b_thbh"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b_tb"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b_thb"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b_other"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);

    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b_thbh"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b_tb"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b_thb"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b_other"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);


    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b_tbh"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b_tbh"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_wbb_th_bh"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_wbj_good"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);


    //higgs mass short range
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",80,400,1200);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariant_"+l,"invariant "+l + ";Mass (tb) [GeV] ",80,400,1200);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",80,400,1200);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariant_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",80,400,1200);
    


    /// this is the histogram for the limits
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_"+l,"invariant "+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_"+l,"invariant "+l + ";Mass (wbb) [GeV] ",360,100,3700);


    ///// Properties of the Higgs candidates
    //t0b
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dPhitb_"+l,"dPhitb "+l + ";dPhi (t,b) [GeV] ",100,0,TMath::Pi()); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dEtatb_"+l,"dEtatb "+l + ";dEta (t,b) [GeV] ",100,0,10); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtAsytb_"+l,"PtAsytb "+l + ";(top - b)/(top +b) ",50,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtB1_"+l,"PtB1 "+l + "; B leading ",200,0,1000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtTopH_"+l,"PtTopH "+l + "; Top Makes Higgs ",100,400,1200); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_"+l,"HT "+l + "; HT[GeV] ",260,800,6000); ///ranges
    //HT divided
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_400_below_"+l,"HT_400_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_500_below_"+l,"HT_500_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_800_below_"+l,"HT_800_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_1000_below_"+l,"HT_1000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_2000_below_"+l,"HT_2000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_3000_below_"+l,"HT_3000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_400_in_"+l,"HT_400_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_500_in_"+l,"HT_500_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_800_in_"+l,"HT_800_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_1000_in_"+l,"HT_1000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_2000_in_"+l,"HT_2000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_3000_in_"+l,"HT_3000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_400_above_"+l,"HT_400_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_500_above_"+l,"HT_500_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_800_above_"+l,"HT_800_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_1000_above_"+l,"HT_1000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_2000_above_"+l,"HT_2000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/HT_3000_above_"+l,"HT_3000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges


    //t1b
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/dPhitb_"+l,"dPhitb "+l + ";dPhi (t,b) [GeV] ",100,0,TMath::Pi()); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/dEtatb_"+l,"dEtatb "+l + ";dEta (t,b) [GeV] ",100,0,10); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/PtAsytb_"+l,"PtAsytb "+l + ";(top - b)/(top +b) ",50,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/PtB1_"+l,"PtB1 "+l + "; B leading ",200,0,1000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/PtTopH_"+l,"PtTopH "+l + "; Top Makes Higgs ",100,400,1200); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_"+l,"HT "+l + "; HT[GeV] ",260,800,6000); ///ranges
    //HT divided
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_400_below_"+l,"HT_400_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_500_below_"+l,"HT_500_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_800_below_"+l,"HT_800_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_1000_below_"+l,"HT_1000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_2000_below_"+l,"HT_2000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_3000_below_"+l,"HT_3000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_400_in_"+l,"HT_400_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_500_in_"+l,"HT_500_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_800_in_"+l,"HT_800_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_1000_in_"+l,"HT_1000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_2000_in_"+l,"HT_2000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_3000_in_"+l,"HT_3000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_400_above_"+l,"HT_400_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_500_above_"+l,"HT_500_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_800_above_"+l,"HT_800_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_1000_above_"+l,"HT_1000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_2000_above_"+l,"HT_2000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/HT_3000_above_"+l,"HT_3000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges


    //Wbb
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/dPhitb_"+l,"dPhitb "+l + ";dPhi ((wb),b) [GeV] ",100,0,TMath::Pi()); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/dEtatb_"+l,"dEtatb "+l + ";dEta ((wb) ,b) [GeV] ",100,0,10); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/PtAsytb_"+l,"PtAsytb "+l + ";((wb) - b)/((wb) +b) ",50,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/PtB1_"+l,"PtB1 "+l + "; B leading ",200,0,1000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/PtTopH_"+l,"PtTopH "+l + "; Top(W+b) ",100,400,1200); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_"+l,"HT "+l + "; HT[GeV] ",260,800,6000); ///ranges
    //HT divided
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_400_below_"+l,"HT_400_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_500_below_"+l,"HT_500_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_800_below_"+l,"HT_800_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_1000_below_"+l,"HT_1000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_2000_below_"+l,"HT_2000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_3000_below_"+l,"HT_3000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_400_in_"+l,"HT_400_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_500_in_"+l,"HT_500_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_800_in_"+l,"HT_800_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_1000_in_"+l,"HT_1000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_2000_in_"+l,"HT_2000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_3000_in_"+l,"HT_3000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_400_above_"+l,"HT_400_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_500_above_"+l,"HT_500_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_800_above_"+l,"HT_800_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_1000_above_"+l,"HT_1000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_2000_above_"+l,"HT_2000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/HT_3000_above_"+l,"HT_3000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Masst_"+l,"Masst "+l + "; top Mass ",30,100,300); ///ranges


    //wbj
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/dPhitb_"+l,"dPhitb "+l + ";dPhi ((wb),b) [GeV] ",100,0,TMath::Pi()); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/dEtatb_"+l,"dEtatb "+l + ";dEta ((wb) ,b) [GeV] ",100,0,10); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/PtAsytb_"+l,"PtAsytb "+l + ";((wb) - b)/((wb) +b) ",50,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/PtB1_"+l,"PtB1 "+l + "; B leading ",200,0,1000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/PtTopH_"+l,"PtTopH "+l + "; Top(W+b) ",100,400,1200); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_"+l,"HT "+l + "; HT[GeV] ",260,800,6000); ///ranges
    //HT divided
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_400_below_"+l,"HT_400_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_500_below_"+l,"HT_500_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_800_below_"+l,"HT_800_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_1000_below_"+l,"HT_1000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_2000_below_"+l,"HT_2000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_3000_below_"+l,"HT_3000_below_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_400_in_"+l,"HT_400_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_500_in_"+l,"HT_500_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_800_in_"+l,"HT_800_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_1000_in_"+l,"HT_1000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_2000_in_"+l,"HT_2000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_3000_in_"+l,"HT_3000_in_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_400_above_"+l,"HT_400_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_500_above_"+l,"HT_500_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_800_above_"+l,"HT_800_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_1000_above_"+l,"HT_1000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_2000_above_"+l,"HT_2000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/HT_3000_above_"+l,"HT_3000_above_ "+l + "; HT[GeV] ",260,800,6000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Masst_"+l,"Masst "+l + "; top Mass ",30,100,300); ///ranges



//    cout <<"Book : ChargedHiggsTopBottom/"+phasespace+"_tb"+"/invariant_"+l <<endl;

}



void ChargedHiggsTopBottomFullHad::Preselection()
{

    for ( string l : AllLabel()  ) {

        BookCutFlow(l, "");
        BookHisto(l, "","Baseline"); // this is when there is nothing
        BookHisto(l, "","OneBOneFat");
        BookHisto(l, "","OneBOneFat_one");
        BookHisto(l, "","OneBOneFat_two");
        BookHisto(l, "","OneBOneFat_three");

        BookHisto(l, "","OneBOneMirrorFat");
        BookHisto(l, "","OneBOneMirrorFat_one");
        BookHisto(l, "","OneBOneMirrorFat_two");
        BookHisto(l, "","OneBOneMirrorFat_three");

        BookFatjetPro(l, "_t0b", "OneBOneFat_one", "pu_");
        BookFatjetPro(l, "_t0b", "OneBOneFat_one", "nopu_");
        BookFatjetPro(l, "_t0b", "OneBOneFat_two", "pu_");
        BookFatjetPro(l, "_t0b", "OneBOneFat_two", "nopu_");
        BookFatjetPro(l, "_t0b", "OneBOneFat_three", "pu_");
        BookFatjetPro(l, "_t0b", "OneBOneFat_three", "nopu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_one", "pu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_one", "nopu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_two", "pu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_two", "nopu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_three", "pu_");
        BookFatjetPro(l, "_t1b", "OneBOneFat_three", "nopu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_one", "pu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_one", "nopu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_two", "pu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_two", "nopu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_three", "pu_");
        BookFatjetPro(l, "_wbb", "OneBOneFat_three", "nopu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_one", "pu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_one", "nopu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_two", "pu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_two", "nopu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_three", "pu_");
        BookFatjetPro(l, "_wbj", "OneBOneFat_three", "nopu_");
        BookFatjetPro(l, "_no", "OneBOneFat_one", "pu_");
        BookFatjetPro(l, "_no", "OneBOneFat_one", "nopu_");
        BookFatjetPro(l, "_no", "OneBOneFat_two", "pu_");
        BookFatjetPro(l, "_no", "OneBOneFat_two", "nopu_");
        BookFatjetPro(l, "_no", "OneBOneFat_three", "pu_");
        BookFatjetPro(l, "_no", "OneBOneFat_three", "nopu_");
        BookFatjetPro(l, "_no", "NoBOneTop", "pu_");
        BookFatjetPro(l, "_no", "NoBOneTop", "nopu_");
        BookFatjetPro(l, "_no", "NoBOneW", "pu_");
        BookFatjetPro(l, "_no", "NoBOneW", "nopu_");

        BookGenTTBar(l, "", "OneBOneFat_one", "tt2bMerged_");
        BookGenTTBar(l, "", "OneBOneFat_one", "tt2b_");
        BookGenTTBar(l, "", "OneBOneFat_one", "tt1b_");
        BookGenTTBar(l, "", "OneBOneFat_one", "ttc_");
        BookGenTTBar(l, "", "OneBOneFat_one", "ttlight_");
        BookGenTTBar(l, "", "OneBOneFat_one", "other_");

        BookGenTTBar(l, "", "OneBOneFat_two", "tt2bMerged_");
        BookGenTTBar(l, "", "OneBOneFat_two", "tt2b_");
        BookGenTTBar(l, "", "OneBOneFat_two", "tt1b_");
        BookGenTTBar(l, "", "OneBOneFat_two", "ttc_");
        BookGenTTBar(l, "", "OneBOneFat_two", "ttlight_");
        BookGenTTBar(l, "", "OneBOneFat_two", "other_");

        BookGenTTBar(l, "", "OneBOneFat_three", "tt2bMerged_");
        BookGenTTBar(l, "", "OneBOneFat_three", "tt2b_");
        BookGenTTBar(l, "", "OneBOneFat_three", "tt1b_");
        BookGenTTBar(l, "", "OneBOneFat_three", "ttc_");
        BookGenTTBar(l, "", "OneBOneFat_three", "ttlight_");
        BookGenTTBar(l, "", "OneBOneFat_three", "other_");





        BookGenMatch(l, "", "OneBOneFat", "tcut_", "lept_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "lept_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "lept_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "lept_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "hadt_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "hadt_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "hadt_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "hadt_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "lepw_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "lepw_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "lepw_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "lepw_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "hadw_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "hadw_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "hadw_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "hadw_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "b_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "b_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "b_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "b_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "no_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "no_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "no_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "no_");

        BookGenMatch(l, "", "OneBOneFat", "tcut_", "QCD_");
        BookGenMatch(l, "", "OneBOneFat", "outtcut_", "QCD_");
        BookGenMatch(l, "", "OneBOneFat", "wcut_", "QCD_");
        BookGenMatch(l, "", "OneBOneFat", "outwcut_", "QCD_");

    }

}

bool ChargedHiggsTopBottomFullHad::genInfoForSignal(Event*e) {


    GenParticle *genCH = NULL;

    WFromTopAss=NULL;
    WFromTopH=NULL;
    WBKGplus=NULL;
    WBKGminus=NULL;

    topFromH = NULL;
    topAss = NULL;
    topBKGplus = NULL;
    topBKGminus = NULL;

    bFromH=NULL;
    bFromTopH=NULL;
    bFromTopAss=NULL;
    bBKGplus=NULL;
    bBKGminus=NULL;

    topFromH_lep = 0;
    topAss_lep = 0;
    topBKGplus_lep = 0;
    topBKGminus_lep = 0;

    WFromTopH_lep = 0;
    WFromTopAss_lep = 0;
    WBKGplus_lep = 0;
    WBKGminus_lep = 0;

    bool rightComb=false;

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        //ch-higgs
        if(abs(genpar->GetPdgId()) == 37) if(genCH==NULL) genCH = genpar;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        if(genCH!=NULL){
            //top
            if( abs(genpar->GetPdgId()) == 6 && abs(genpar->GetParentPdgId()) == 37 ) topFromH = genpar;
            //bottom
            else if( abs(genpar->GetPdgId()) == 5 && abs(genpar->GetParentPdgId()) == 37) bFromH = genpar;
        }
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);

        //W
        if(abs(genpar->GetPdgId()) == 24 && abs(genpar->GetParentPdgId()) == 6){
            if(topFromH!=NULL){
                if ( topFromH->GetPdgId()*genpar->GetPdgId()>0 ) WFromTopH=genpar;
                else if ( topFromH->GetPdgId()*genpar->GetPdgId()<0 )  WFromTopAss=genpar;
                rightComb=true;
            }else if(topFromH==NULL){
                if ( genpar->GetPdgId() == 24 && genpar->GetParentPdgId() == 6 )  WBKGplus = genpar;
                else if( genpar->GetPdgId() == -24 && genpar->GetParentPdgId() == -6 ) WBKGminus = genpar;
            }
        //bottom
        }else if(abs(genpar->GetPdgId()) == 5 && abs(genpar->GetParentPdgId()) == 6){
            if(topFromH!=NULL){
                if ( topFromH->GetPdgId()*genpar->GetPdgId()>0 ) bFromTopH = genpar;
                else if ( topFromH->GetPdgId()*genpar->GetPdgId()<0 ) bFromTopAss = genpar;
            }else if(topFromH==NULL) {
                if ( genpar->GetPdgId() == 5 && genpar->GetParentPdgId() == 6 )bBKGplus = genpar;
                else if( genpar->GetPdgId() == -5 && genpar->GetParentPdgId() == -6 ) bBKGminus = genpar;
            }
        //top
        }else if(abs(genpar->GetPdgId()) == 6){
            if(genCH != NULL && abs(genpar->GetParentPdgId()) != 37) topAss = genpar;
            if(genCH == NULL){
                if(genpar->GetPdgId() == 6)topBKGplus = genpar;
                if(genpar->GetPdgId() == -6)topBKGminus = genpar;
            }
        //lepton
        }else if( ((abs(genpar->GetPdgId()) == 11 or abs(genpar->GetPdgId()) == 13) and (genpar->IsPromptFinalState() or genpar->IsDirectPromptTauDecayProductFinalState()))
                  or (abs(genpar->GetPdgId()) == 15 and genpar->IsPromptDecayed()) ) {
            if(topFromH!=NULL){
                // covers the signal
                if ( topFromH->GetPdgId()*genpar->GetPdgId()<0 ) {topFromH_lep = 1; WFromTopH_lep = 1;}
                else if ( topFromH->GetPdgId()*genpar->GetPdgId()>0 ) {topAss_lep = 1; WFromTopAss_lep = 1;}                    
            }else if(topFromH==NULL){
                // top BKG
                if ( ((genpar->GetPdgId() == -11 or genpar->GetPdgId() == -13) and (genpar->IsPromptFinalState() or genpar->IsDirectPromptTauDecayProductFinalState())) or
                     (genpar->GetPdgId() == -15 and genpar->IsPromptDecayed() ) ) {topBKGplus_lep = 1;WBKGplus_lep = 1;}
                else if ( ((genpar->GetPdgId() == 11 or genpar->GetPdgId() == 13) and genpar->IsPromptFinalState()) or
                          (genpar->GetPdgId() == 15 and genpar->IsPromptDecayed() ) ) {topBKGminus_lep = 1;WBKGminus_lep = 1;}
            }
        }
    }

    return rightComb;
}




void ChargedHiggsTopBottomFullHad::eventShapePlot(Event*e, string label, string category, string systname, string phasespace) {


    //    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Centrality_"+label,systname,evt_C,e->weight());
    //    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HemiOut_"+label,systname,evt_HemiMetOut,e->weight());

}

// this compute the index of the matched jets
void ChargedHiggsTopBottomFullHad::classifyLabelGenEv(Event*e, string label, string systname, string phasespace) {

    Double_t minWth = 99., minWta = 99.; FatJet * wth, wtass;
    Double_t minWbkg1 = 99., minWbkg2 = 99.; FatJet * wbkg1, wbkg2;

    Double_t minbh = 99., minbth = 99., minbta = 99.; FatJet * bh, bth, bta;
    Double_t minbbkg1 = 99., minbbkg2 = 99.; FatJet * bbkg1, bbkg2;

    Double_t mintoph = 99., mintopa = 99.;  FatJet * toph, tass;
    Double_t mintopbkg1 = 99., mintopbkg2 = 99.; FatJet * topbkg1, topbkg2;

    Int_t topsnum1 = -1; // sig higss
    Int_t topsnum2 = -1; // sig ass
    Int_t topbnum1 = -1; // bkg +
    Int_t topbnum2 = -1; // bkg -
    Int_t Wsnum1 = -1; // sig H -> top -> W
    Int_t Wsnum2 = -1; // sig ass top -> W
    Int_t Wbnum1 = -1; // bkg +
    Int_t Wbnum2 = -1; // bkg -
    Int_t bsnum1 = -1; // sig b from Higs
    Int_t bsnum2 = -1; // Higgs top -> b
    Int_t bsnum3 = -1; // sig ass top -> b
    Int_t bbnum1 = -1; // bkg +
    Int_t bbnum2 = -1; // bkg -

    // maybetop
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* maybetop = e->GetFatJet(j);
        if(maybetop->Pt() < 400.) continue;

        if(label.find("HplusToTB") !=string::npos ) {
            if(topFromH!=NULL){
                Double_t deltoph = maybetop->DeltaR(*topFromH);
                if (deltoph < mintoph){ mintoph = deltoph; /*toph = maybetop;*/ topsnum1 = j;}
            }
            //
            if(topAss!=NULL){
                Double_t deltopa = maybetop->DeltaR(*topAss);
                if (deltopa < mintopa){ mintopa = deltopa; /*tass = maybetop;*/ topsnum2 = j;}
            }
        }else if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ){
            if(topBKGplus!=NULL) {
                Double_t deltopbkg1 = maybetop->DeltaR(*topBKGplus);
                if (deltopbkg1 < mintopbkg1){ mintopbkg1 = deltopbkg1; /*topbkg1 = maybetop;*/ topbnum1 = j;}
            }
            if(topBKGminus!=NULL) {
                Double_t deltopbkg2 = maybetop->DeltaR(*topBKGminus);
                if (deltopbkg2 < mintopbkg2){ mintopbkg2 = deltopbkg2; /*topbkg2 = maybetop;*/ topbnum2 = j;}
            }
        }
    }


    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* maybeW = e->GetFatJet(j);
        if(maybeW->Pt() < 200.) continue;

        if(label.find("HplusToTB") !=string::npos ) if( (mintoph < 0.3 && j == topsnum1) || (mintopa < 0.2 && j == topsnum2) ) continue;
        if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (mintopbkg1 < 0.2 && j == topbnum1) || (mintopbkg2 < 0.2 && j == topbnum2) ) continue;

        if(label.find("HplusToTB") !=string::npos ){
            if(WFromTopH!=NULL) {
                Double_t delWth = maybeW->DeltaR(*WFromTopH);
                if (delWth < minWth){ minWth = delWth; /*wth = maybeW;*/ Wsnum1 = j;}
            }
            //
            if(WFromTopAss!=NULL) {
                Double_t delWta = maybeW->DeltaR(*WFromTopAss);
                if (delWta < minWta){ minWta = delWta; /*wtass = maybeW;*/ Wsnum2 = j;}
            }
        }else if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ){
            if(WBKGplus!=NULL) {
                Double_t delWbkg1 = maybeW->DeltaR(*WBKGplus);
                if (delWbkg1 < minWbkg1){ minWbkg1 = delWbkg1; /*wbkg1 = maybeW;*/ Wbnum1 = j; }
            }
            //
            if(WBKGminus!=NULL) {
                Double_t delWbkg2 = maybeW->DeltaR(*WBKGminus);
                if (delWbkg2 < minWbkg2){ minWbkg2 = delWbkg2; /*wbkg2 = maybeW;*/ Wbnum2 = j; }
            }
        }
    }


    for(int j=0;j!=e->Bjets();j++) {

        Jet* maybeB = e->GetBjet(j);
        //        if(maybeB->Pt() < 200.) continue;

        if(label.find("HplusToTB") !=string::npos ) if( (mintoph < 0.3 && j == topsnum1) || (mintopa < 0.2 && j == topsnum2) ) continue;
        if (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (mintopbkg1 < 0.2 && j == topbnum1) || (mintopbkg2 < 0.2 && j == topbnum2) ) continue;

        if(label.find("HplusToTB") !=string::npos ) if( (minWth < 0.3 && j == Wsnum1) || (minWta < 0.2 && j == Wsnum2) ) continue;
        if (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (minWbkg1 < 0.2 && j == Wbnum1) || (minWbkg2 < 0.2 && j == Wbnum2) ) continue;


        if(label.find("HplusToTB") !=string::npos ){
            if(bFromH!=NULL) {
                Double_t delbh = maybeB->DeltaR(*bFromH);
                if (delbh < minbh){ minbh = delbh; /*bh = maybeW*/;  bsnum1 = j;}
            }
            //
            if(bFromTopH!=NULL) {
                Double_t delbth = maybeB->DeltaR(*bFromTopH);
                if (delbth < minbth){ minbth = delbth; /*bth = maybeW*/; bsnum2 = j;}
            }
            //
            if(bFromTopAss!=NULL) {
                Double_t delbta = maybeB->DeltaR(*bFromTopAss);
                if (delbta < minbta){ minbta = delbta; /*bta = maybeW*/; bsnum3 = j;}
            }
        }else if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ){
            if(bBKGplus!=NULL) {
                Double_t delbbkg1 = maybeB->DeltaR(*bBKGplus);
                if (delbbkg1 < minbbkg1){ minbbkg1 = delbbkg1; /*bbkg1 = maybeW;*/ bbnum1 = j; }
            }
            //
            if(bBKGminus!=NULL) {
                Double_t delbbkg2 = maybeB->DeltaR(*bBKGminus);
                if (delbbkg2 < minbbkg2){ minbbkg2 = delbbkg2; /*bbkg2 = maybeW;*/ bbnum2 = j; }
            }
        }

    }


    //////
    //////
    ////// Define the label for the jet level plots
    //////
    /////

    string match="other_";

    // maybetop
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* fatjet = e->GetFatJet(j);

        if(label.find("HplusToTB") !=string::npos ) {
            if( (mintoph < 0.3 && j == topsnum1 && topFromH_lep == 1) || (mintopa < 0.2 && j == topsnum2 && topAss_lep == 1) ) match="lept_";
            else if( (mintoph < 0.3 && j == topsnum1 && topFromH_lep == 0) || (mintopa < 0.2 && j == topsnum2 && topAss_lep == 0) ) match="hadt_";
            else if( (j == Wsnum1 && minWth < 0.3 && WFromTopH_lep == 1) || (j == Wsnum2 && minWta < 0.2 && WFromTopAss_lep == 1) ) match="lepw_";
            else if( (j == Wsnum1 && minWth < 0.3 && WFromTopH_lep == 0) || (j == Wsnum2 && minWta < 0.2 && WFromTopAss_lep == 0) ) match="hadw_";
            else if( j == bsnum1 && minbh < 0.2 ) match="b_";
            else match="no_";
        }else if (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) {
            if( (mintopbkg1 < 0.2 && j == topbnum1 && topBKGplus_lep == 1) || (mintopbkg2 < 0.2 && j == topbnum2 && topBKGminus_lep == 1) ) match="lept_";
            else if( (mintopbkg1 < 0.2 && j == topbnum1 && topBKGplus_lep == 0) || (mintopbkg2 < 0.2 && j == topbnum2 && topBKGminus_lep == 0) ) match="hadt_";
            else if( (j == Wbnum1 && minWbkg1 < 0.2 && WBKGplus_lep == 1) || (j == Wbnum2 && minWbkg2 < 0.2 && WBKGminus_lep == 1) ) match="lepw_";
            else if( (j == Wbnum1 && minWbkg1 < 0.2 && WBKGplus_lep == 0) || (j == Wbnum2 && minWbkg2 < 0.2 && WBKGminus_lep == 0) ) match="hadw_";
            else if( (j == bbnum1 && minbbkg1 < 0.2) || (j == bbnum2 && minbbkg2 < 0.2) )  match="b_";
            else match="no_";
        }else if(label.find("QCD_HT") !=string::npos){
            match="QCD_";
        }else {
            match="";
        }


        // add btagging condition
//        if(fatjet->Tau3()< 0.81*fatjet->Tau2() && fatjet->IsSubjetBTag() > 0 ){
        if(fatjet->Tau3()< 0.81*fatjet->Tau2()) Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_"+match+"tcut_"+label,systname,fatjet->SDMass(),e->weight());
        else Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_"+match+"outtcut_"+label,systname,fatjet->SDMass(),e->weight());

        if(fatjet->Tau2()< 0.6*fatjet->Tau1()) Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_"+match+"wcut_"+label,systname,fatjet->SDMass(),e->weight());
        else Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_"+match+"outwcut_"+label,systname,fatjet->SDMass(),e->weight());

    }

    //////
    ////// Define the label for the tb and wbb categories
    //////

    string LabelGenT0B="other_";
    string LabelGenT1B="other_";
    string LabelGenWBB="other_";
    string LabelGenWBJ="other_";

    if(label.find("HplusToTB") !=string::npos){

        /// TOP+B category
        if(evt_MH_t0b>0 && leadingb!=NULL) {
            if(bFromH!=NULL && topFromH!=NULL){
                if (leadingb->DeltaR(*bFromH) < 0.2) {
                    if(topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3)) LabelGenT0B="_t0b_thbh";
                    else LabelGenT0B="_t0b_tbh";
                } else {
                    if(topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3)) LabelGenT0B="_t0b_thb";
                    else LabelGenT0B="_t0b_tb";
                }
            }else if(bFromH!=NULL){
                if (leadingb->DeltaR(*bFromH) < 0.2) LabelGenT0B="_t0b_tbh";
                else LabelGenT0B="_t0b_tb";
            }else if(topFromH!=NULL){
                if (topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3) ) LabelGenT0B="_t0b_thb";
                else LabelGenT0B="_t0b_tb";
            }else LabelGenT0B="_t0b_tb";
        }


        if(evt_MH_t1b>0 && leadingb!=NULL) {
            if(bFromH!=NULL && topFromH!=NULL){
                if (leadingb->DeltaR(*bFromH) < 0.2) {
                    if(topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3)) LabelGenT1B="_t1b_thbh";
                    else LabelGenT1B="_t1b_tbh";
                } else {
                    if(topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3)) LabelGenT1B="_t1b_thb";
                    else LabelGenT1B="_t1b_tb";
                }
            }else if(bFromH!=NULL){
                if (leadingb->DeltaR(*bFromH) < 0.2) LabelGenT1B="_t1b_tbh";
                else LabelGenT1B="_t1b_tb";
            }else if(topFromH!=NULL){
                if (topsnum1>0 && topJet==e->GetFatJet(topsnum1) && (e->GetFatJet(topsnum1)->DeltaR(*topFromH) < 0.3) ) LabelGenT1B="_t1b_thb";
                else LabelGenT1B="_t1b_tb";
            }else LabelGenT1B="_t1b_tb";
        }

        /// W+B+B category
        if(evt_MH_Wbb>0 && numtop == 0 && leadingb!=NULL) {
            if(bFromH!=NULL && topFromH!=NULL && WFromTopH!=NULL){
                if(leadingb->DeltaR(*bFromH) < 0.2){
                    if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_bh";
                    else if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_bh";
                    else LabelGenWBB="_wbb_ntnw_bh";
                }else{
                    if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_nb";
                    else if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_nb";
                    else LabelGenWBB="_wbb_ntnw_nb";
                }
            }else if(bFromH!=NULL && topFromH!=NULL){
                if(leadingb->DeltaR(*bFromH) < 0.2){
                    if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_bh";
                    else LabelGenWBB="_wbb_ntnw_bh";
                }else{
                    if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_nb";
                    else LabelGenWBB="_wbb_ntnw_nb";
                }
            }else if(bFromH!=NULL && WFromTopH!=NULL){
                if(leadingb->DeltaR(*bFromH) < 0.2){
                    if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_bh";
                    else LabelGenWBB="_wbb_ntnw_bh";
                }else{
                    if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_nb";
                    else LabelGenWBB="_wbb_ntnw_nb";
                }
            }else if(topFromH!=NULL && WFromTopH!=NULL){
                if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_nb";
                else if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_nb";
                else LabelGenWBB="_wbb_ntnw_nb";
            }else if(bFromH!=NULL){
                if(leadingb->DeltaR(*bFromH) < 0.2) LabelGenWBB="_wbb_ntnw_bh";
                else LabelGenWBB="_wbb_ntnw_nb";
            }else if(topFromH!=NULL){
                if(topFromHOpenCand.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBB="_wbb_th_nb";
                else LabelGenWBB="_wbb_ntnw_nb";
            }else if(WFromTopH!=NULL){
                if(Wsnum1>0 && wJet==e->GetFatJet(Wsnum1) && e->GetFatJet(Wsnum1)->DeltaR(*WFromTopH) < 0.3) LabelGenWBB="_wbb_wh_nb";
                else LabelGenWBB="_wbb_ntnw_nb";        
            }else LabelGenWBB="_wbb_ntnw_nb";


        }


        ///W+b+j
        if(evt_MH_wbj>0 && numtop == 0 && leadingbWBJ!=NULL) {
            if(bFromH!=NULL && topFromH!=NULL){
                if(leadingbWBJ->DeltaR(*bFromH) < 0.2){
                    if(topFromHwbj.DeltaR(topFromH->GetP4()) < 0.3) LabelGenWBJ="_wbj_good";
                    else LabelGenWBJ="_wbj_bad";
                }else LabelGenWBJ="_wbj_bad";
            }else LabelGenWBJ="_wbj_bad";
        }       
        
    } //end singal

    higgsPlot(e, label, LabelGenT0B,systname,phasespace);
    higgsPlot(e, label, LabelGenT1B,systname,phasespace);
    higgsPlot(e, label, LabelGenWBB,systname,phasespace);
    higgsPlot(e, label, LabelGenWBJ,systname,phasespace);

}


void ChargedHiggsTopBottomFullHad::getCandidate(Event*e, string label, string systname, string phasespace, bool mirror) {

    evt_MH_tb=-1;
    evt_MH_t1b=-1;
    evt_MH_t0b=-1;
    evt_MH_Wbb=-1;
    evt_MH_wbj=-1;

    evt_PtH_tb =-1;
    evt_PtH_t1b=-1;
    evt_PtH_t0b=-1;
    evt_PtH_Wbb=-1;
    evt_PtH_wbj=-1;


    leadingb=NULL;
    leadingbWBJ = NULL;
    secondb = NULL;

    numtop = 0;
    topJet=NULL;
    wJet=NULL;

    //////// search for leading B jet
    Int_t flag_lb[10] = {0};
    Int_t leadingokb = -1;

    for(int mm=0;mm!=e->Bjets();mm++) {

        Jet* ab = e->GetBjet(mm);
        for(int kfjet=0;kfjet!=e->NFatJets();kfjet++) {
            FatJet* AK8jet = e->GetFatJet(kfjet);
            if( (!mirror and AK8jet->IsTopJet()) or (mirror and AK8jet->IsTopJetMirror()) ) { if(ab->DeltaR(*AK8jet) < 1.2) flag_lb[mm] = 1; }
        }
    }

    for(int mm=0;mm!=e->Bjets();mm++) {
        if(flag_lb[mm] == 1) continue;
        leadingokb = mm; break;
    }

    if(leadingokb==(-1)) return;
    leadingb = e->GetBjet(leadingokb);

    /////
    ///// Top + leading b , higgs candidate
    /////

    Double_t mintoppt = 9999;
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* temp = e->GetFatJet(j);
        if(temp->Tau2()==0  || temp->Tau3()==0 ) continue;

        if((mirror and temp->IsTopJetMirror()) or (!mirror and temp->IsTopJet())) {
            numtop++;

            Double_t temptoppt = (temp->GetP4() + leadingb->GetP4()).Pt();
            if(temptoppt < mintoppt){
                mintoppt = temptoppt;
                topJet = temp;
                evt_MH_tb=(topJet->GetP4() + leadingb->GetP4()).M();
                evt_PtH_tb=(topJet->GetP4() + leadingb->GetP4()).Pt();
            }

        }
    }
    if(topJet != NULL){
        if(topJet->IsSubjetBTag() > 0) {evt_MH_t1b = evt_MH_tb; evt_PtH_t1b = evt_PtH_tb;}
        else if(topJet->IsSubjetBTag() == 0) {evt_MH_t0b = evt_MH_tb; evt_PtH_t0b = evt_PtH_tb;}
    }


    higgsPlot(e, label, "_t0b",systname,phasespace);
    higgsPlot(e, label, "_t1b",systname,phasespace);

    if(evt_MH_tb > 0) return;

    /////
    ///// W + b + b , higgs candidate
    /////


    Double_t mintopmass = 999.;
    topFromHOpenCand.SetXYZM(0.,0.,0.,0.);
    Int_t orderW = -1, orderb = -1;
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* temp = e->GetFatJet(j);

        /// for each W pick the b, such that is the W+b closest to the topMass
        if( ( !mirror || !temp->IsWJetMirror()) && ( mirror || !temp->IsWJet()) ) continue;

        for(int ib=0;ib!=e->Bjets();ib++) {

            if(flag_lb[ib] == 1 || ib == leadingokb) continue;

            TLorentzVector mayt = temp->GetP4() + e->GetBjet(ib)->GetP4();

            if(mayt.DeltaR(leadingb->GetP4())<1.2) continue;

            if( fabs(mayt.Eta()-leadingb->Eta()) < 1.5){
                Double_t deltopmass = fabs(mayt.M() - ChargedHiggs::Mtop);
                if (deltopmass < mintopmass) {
                    topFromHOpenCand = mayt;
                //                w1 = temp; orderW = j;
                //                b1 = e->GetBjet(ib); orderb = ib;
                    wJet=temp;
                    mintopmass = deltopmass;
                    evt_MH_Wbb=(topFromHOpenCand+leadingb->GetP4()).M();
                    evt_PtH_Wbb=(topFromHOpenCand+leadingb->GetP4()).Pt();
                }
            }
        }
    }

    higgsPlot(e, label, "_wbb",systname,phasespace);

    if(evt_MH_Wbb > 0) return;

    /////
    ///// to implement W + b + j , higgs candidate
    ///// j can be the leading jet from the higgs or the b from the top

    double maxcsv = 0.;
    int orderj = -1;    

    topFromHwbj.SetXYZM(0.,0.,0.,0.);
    double mintopmass1 = 999.;


    for(int ijet = 0; ijet != e->Njets() ; ijet++){
        Jet* mayb = e->GetJet(ijet);
        if(mayb->bdiscr < 0.8484 && mayb->bdiscr > maxcsv){
            maxcsv = mayb->bdiscr;
            orderj = ijet;
        }
    }

    if(orderj==(-1)) return;

    if(e->GetJet(orderj)->Pt() > leadingb->Pt()) {leadingbWBJ = e->GetJet(orderj); secondb = leadingb;}
    else {leadingbWBJ = leadingb; secondb = e->GetJet(orderj);}
        
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* temp = e->GetFatJet(j);  
        if( ( !mirror || !temp->IsWJetMirror()) && ( mirror || !temp->IsWJet()) ) continue;

        for(int ib=0;ib!=e->Bjets();ib++) {
        
            if(flag_lb[ib] == 1) continue;
            if(e->GetBjet(ib) == leadingbWBJ) continue;

            TLorentzVector wjt = temp->GetP4() + e->GetBjet(ib)->GetP4();

            if(wjt.DeltaR(leadingbWBJ->GetP4())<1.2) continue;
            if(fabs(wjt.Eta()-leadingbWBJ->Eta()) > 1.5) continue;

            Double_t deltopmass1 = fabs(wjt.M() - ChargedHiggs::Mtop);
            if (deltopmass1 < mintopmass1) {
                mintopmass1 = deltopmass1;
                topFromHwbj = wjt;
                evt_MH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).M();
                evt_PtH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).Pt();
            }
            
        }
    }

    if(secondb == e->GetJet(orderj)){
        for(int j=0;j!=e->NFatJets();j++) {

            FatJet* temp = e->GetFatJet(j);
            if( ( !mirror || !temp->IsWJetMirror()) && ( mirror || !temp->IsWJet()) ) continue;

            TLorentzVector wjt = temp->GetP4() + secondb->GetP4();

            if(wjt.DeltaR(leadingbWBJ->GetP4())<1.2) continue;
            if(fabs(wjt.Eta()-leadingbWBJ->Eta()) < 1.5) continue;

            Double_t deltopmass2 = fabs(wjt.M() - ChargedHiggs::Mtop);
            if (deltopmass2 < mintopmass1) {
                mintopmass1 = deltopmass2;
                topFromHwbj = wjt;
                evt_MH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).M();
                evt_PtH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).Pt();
            }
        }
    }

    higgsPlot(e, label, "_wbj",systname,phasespace);


//    higgsPlot(e, label, "_tb",systname,phasespace);
//    higgsPlot(e, label, "_wbb",systname,phasespace);
}


void ChargedHiggsTopBottomFullHad::computeVar(Event*e) {


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


    ////$$$$$$
    ////$$$$$$
    ////$$$$$$
    ////$$$$$$

 
   evt_HT=0;

    // check the impact of the HT(w/ central jet only)
    for(int i=0;i!=e->NcentralJets();++i) {
        evt_HT += e->GetCentralJet(i)->Pt();
    }

    ////$$$$$$
    ////$$$$$$
    ////$$$$$$
    ////$$$$$$

    if(e->Bjets()>1) {

        double DEtaMaxBB=0.;

        for(int i=0;i!=e->Bjets();++i) {
            Jet* bj = e->GetBjet(i);
            for(int j=0;j!=e->Bjets();++j) {
                if (j==i) continue;
                Jet* bjet = e->GetBjet(j);
                if(bj->DeltaEta(*bjet)>DEtaMaxBB) DEtaMaxBB=bj->DeltaEta(*bjet);

                //                double dr = bjet->DeltaR(e->GetBjet(j));
                //                double mass = (bjet->GetP4() + e->GetBjet(j)->GetP4()).M();
                //                if(dr<minDRbb) { minDRbb=dr; minDRbb_invMass=mass; indexI=i; indexJ=j;}
                //                if(dr>maxDRbb and i==0) { maxDRbb=dr; maxDRbb_invMass=mass; indexMaxJ=j;}
            }
        }

        evt_DEtaMaxBB=DEtaMaxBB;

    }


    double DEtaMaxJJ=0.;

    for(int i=0;i!=e->Njets();++i) {
        Jet* lj = e->GetJet(i);
        for(int j=0;j!=e->Njets();++j) {
            if (j==i) continue;
            Jet* jet = e->GetJet(j);
            if(lj->DeltaEta(*jet)>DEtaMaxJJ) DEtaMaxJJ=lj->DeltaEta(*jet);

            //                double dr = bjet->DeltaR(e->GetBjet(j));
            //                double mass = (bjet->GetP4() + e->GetBjet(j)->GetP4()).M();
            //                if(dr<minDRbb) { minDRbb=dr; minDRbb_invMass=mass; indexI=i; indexJ=j;}
            //                if(dr>maxDRbb and i==0) { maxDRbb=dr; maxDRbb_invMass=mass; indexMaxJ=j;}
        }
    }



}

void ChargedHiggsTopBottomFullHad::jetPlot(Event*e, string label, string category, string systname, string phasespace) {

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

}

void ChargedHiggsTopBottomFullHad::fatjetPlot(Event*e, string label, string systname, string phasespace) {

    string sigcategory = "_other";
    string puornot = "_other";

    if(evt_MH_t0b>0) sigcategory = "_t0b";
    else if(evt_MH_t1b>0) sigcategory = "_t1b";
    else if(evt_MH_Wbb>0) sigcategory = "_wbb";
    else if(evt_MH_wbj>0) sigcategory = "_wbj";
    else sigcategory = "_no"; 

    if(e->Npv() < 20) puornot = "nopu_";
    else puornot = "pu_";


    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* fatjet = e->GetFatJet(j);

        if(fatjet->IsTopJet()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/top_pt_"+puornot+label,systname,fatjet->Pt(),e->weight());
        if(fatjet->IsWJet()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/W_pt_"+puornot+label,systname,fatjet->Pt(),e->weight());
        if(fatjet->Tau2()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/tau32_"+puornot+label,systname,fatjet->Tau3()/fatjet->Tau2(),e->weight());
        if(fatjet->Tau1()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/tau21_"+puornot+label,systname,fatjet->Tau2()/fatjet->Tau1(),e->weight());

        if(fatjet->Tau3()< 0.81*fatjet->Tau2()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_tcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
        else Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_outtcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());

        if(fatjet->Tau2()< 0.6*fatjet->Tau1()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_wcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
        else Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_outwcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
    }

    Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/HT_"+puornot+label,systname, evt_HT ,e->weight());       


}


void ChargedHiggsTopBottomFullHad::higgsPlot(Event*e, string label, string category,string systname, string phasespace) {

    // this is for all label
    if(evt_MH_t1b>0 and (category.find("_t1b_")!=string::npos))  Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_t1b,e->weight());
    if(evt_MH_t0b>0 and (category.find("_t0b_")!=string::npos))  Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_t0b,e->weight());
    if(evt_MH_Wbb>0 and (category.find("_wbb_")!=string::npos))  Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_Wbb,e->weight());
    if(evt_MH_wbj>0 and (category.find("_wbj_")!=string::npos))  Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_wbj,e->weight());


    if(evt_MH_t1b>0 and (category.find("_t1b_")!=string::npos)) Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_t1b,evt_PtH_t1b,e->weight());
    if(evt_MH_t0b>0 and (category.find("_t0b_")!=string::npos)) Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_t0b,evt_PtH_t0b,e->weight());
    if(evt_MH_Wbb>0 and (category.find("_wbb_th_bh")!=string::npos)) Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_Wbb,evt_PtH_Wbb,e->weight());
    if(evt_MH_wbj>0 and (category.find("_wbj_good")!=string::npos)) Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_wbj,evt_PtH_wbj,e->weight());


    if(evt_MH_t1b>0 and (category.find("_t1b")!=string::npos) and not (category.find("_t1b_")!=string::npos)) {
        //Pt-Mass-2D
        Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_t1b,evt_PtH_t1b,e->weight());
        //invariant mass short range
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_t1b,e->weight());
        //invariant mass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariantExt_"+label,systname,evt_MH_t1b,e->weight());
        // two tb jets are in the opposite hemisphere
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topJet->Phi()) ,e->weight());
        // difference in rapidity
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  topJet->DeltaEta(*leadingb),e->weight());
        // pt asymmetry
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topJet->Pt() - leadingb->Pt())/(topJet->Pt() + leadingb->Pt()),e->weight());
        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topJet->Pt() ,e->weight());
        //HT
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());
        //HT divided
        if( (label.find("M-400") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 360. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 360. and evt_MH_t1b < 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-500") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 450. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 450. and evt_MH_t1b < 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-800") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 700. and evt_MH_t1b < 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-1000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 860. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 860. and evt_MH_t1b < 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-2000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 1700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 1700. and evt_MH_t1b < 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-3000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t1b < 2580. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 2580. and evt_MH_t1b < 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t1b >= 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_above_"+label,systname, evt_HT ,e->weight());
        }

    } //end t1b

    if(evt_MH_t0b>0 and (category.find("_t0b")!=string::npos) and not (category.find("_t0b_")!=string::npos)) {
        //Pt-Mass-2D
        Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_t0b,evt_PtH_t0b,e->weight());
        //invariant mass short range
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_t0b,e->weight());
        //invariant mass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariantExt_"+label,systname,evt_MH_t0b,e->weight());
        // two tb jets are in the opposite hemisphere
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topJet->Phi()) ,e->weight());
        // difference in rapidity
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  topJet->DeltaEta(*leadingb),e->weight());
        // pt asymmetry
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topJet->Pt() - leadingb->Pt())/(topJet->Pt() + leadingb->Pt()),e->weight());
        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topJet->Pt() ,e->weight());
        //HT
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());
        //HT divided
        if( (label.find("M-400") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 360. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 360. and evt_MH_t0b < 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-500") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 450. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 450. and evt_MH_t0b < 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_above_"+label,systname, evt_HT ,e->weight());
        }
        
        if( (label.find("M-800") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 700. and evt_MH_t0b < 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-1000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 860. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 860. and evt_MH_t0b < 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_above_"+label,systname, evt_HT ,e->weight());
        }
        
        if( (label.find("M-2000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 1700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 1700. and evt_MH_t0b < 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-3000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_t0b < 2580. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 2580. and evt_MH_t0b < 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_t0b >= 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_above_"+label,systname, evt_HT ,e->weight());
        }
                    
    } //end t0b



    if(evt_MH_Wbb>0 and (category.find("_wbb")!=string::npos) and not (category.find("_wbb_")!=string::npos)) {
        //Pt-Mass-2D
        Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_Wbb,evt_PtH_Wbb,e->weight());
        //invariant mass short range
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_Wbb,e->weight());
        //invariant mass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariantExt_"+label,systname,evt_MH_Wbb,e->weight());
        // two tb jets are in the opposite hemisphere
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topFromHOpenCand.Phi()) ,e->weight());
        // difference in rapidity
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  fabs(topFromHOpenCand.Eta()-leadingb->GetP4().Eta())  ,e->weight());
        // pt asymmetry
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topFromHOpenCand.Pt() - leadingb->Pt())/(topFromHOpenCand.Pt() + leadingb->Pt()),e->weight());

        // mass of the top candidate
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Masst_"+label,systname,  topFromHOpenCand.M() ,e->weight());

        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topFromHOpenCand.Pt() ,e->weight());
        //HT
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());
        //HT divided
        if( (label.find("M-400") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 360. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 360. and evt_MH_Wbb < 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-500") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 450. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 450. and evt_MH_Wbb < 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-800") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 700. and evt_MH_Wbb < 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-1000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 860. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 860. and evt_MH_Wbb < 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-2000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 1700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 1700. and evt_MH_Wbb < 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-3000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_Wbb < 2580. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 2580. and evt_MH_Wbb < 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_Wbb >= 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_above_"+label,systname, evt_HT ,e->weight());
        }
            
    } //end Wbb

    
    if(evt_MH_wbj>0 and (category.find("_wbj")!=string::npos) and not (category.find("_wbj_")!=string::npos)) {
        //Pt-Mass-2D
        Fill2D("ChargedHiggsTopBottom/"+phasespace+category+"/Pt_Mass_tb_"+label,systname,evt_MH_wbj,evt_PtH_wbj,e->weight());
        //invariant mass short range
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariant_"+label,systname,evt_MH_wbj,e->weight());
        //invariant mass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/invariantExt_"+label,systname,evt_MH_wbj,e->weight());
        // two tb jets are in the opposite hemisphere
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingbWBJ->Phi(), topFromHwbj.Phi()) ,e->weight());
        // difference in rapidity
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  fabs(topFromHwbj.Eta()-leadingbWBJ->GetP4().Eta())  ,e->weight());
        // pt asymmetry
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topFromHwbj.Pt() - leadingbWBJ->Pt())/(topFromHwbj.Pt() + leadingbWBJ->Pt()),e->weight());

        // mass of the top candidate
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Masst_"+label,systname,  topFromHwbj.M() ,e->weight());

        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingbWBJ->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topFromHwbj.Pt() ,e->weight());
        //HT        
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+label,systname, evt_HT ,e->weight());
        //HT divided
        if( (label.find("M-400") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 360. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 360. and evt_MH_wbj < 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 520. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_400_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-500") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 450. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 450. and evt_MH_wbj < 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 650. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_500_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-800") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 700. and evt_MH_wbj < 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 880. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_800_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-1000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 860. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 860. and evt_MH_wbj < 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 1080. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_1000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-2000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 1700. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 1700. and evt_MH_wbj < 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 2100. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_2000_above_"+label,systname, evt_HT ,e->weight());
        }

        if( (label.find("M-3000") !=string::npos) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) or (label.find("QCD_HT") !=string::npos) ){
            if( evt_MH_wbj < 2580. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_below_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 2580. and evt_MH_wbj < 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_in_"+label,systname, evt_HT ,e->weight());
            else if( evt_MH_wbj >= 3140. ) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_3000_above_"+label,systname, evt_HT ,e->weight());
        }

    }//end wbj


}

    


void ChargedHiggsTopBottomFullHad::classifyHF(Event*e, string label, string category, string systname, string phasespace, string Sregion) {


    //    if((label.find("TTTo2L2Nu")!=string::npos) || (label.find("TTToSemilepton")!=string::npos))  {
    if((label.find("TTTo2L2Nu_TuneCUETP8M2")!=string::npos) || (label.find("TTToSemilepton_TuneCUETP8M2")!=string::npos) ||  (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=string::npos)  ) {

        //https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc#L35
        string LabelHF="other_";
        if((e->GetGenTtbarId()%100)==55 || (e->GetGenTtbarId()%100)==54) LabelHF="tt2bMerged_";
        if((e->GetGenTtbarId()%100)==53) LabelHF="tt2b_";
        if((e->GetGenTtbarId()%100)==52 || (e->GetGenTtbarId()%100)==51) LabelHF="tt1b_";
        if((e->GetGenTtbarId()%100)==45 || (e->GetGenTtbarId()%100)==44 || (e->GetGenTtbarId()%100)==43 || (e->GetGenTtbarId()%100)==42 || (e->GetGenTtbarId()%100)==41) LabelHF="ttc_";
        if((e->GetGenTtbarId()%100)==00) LabelHF="ttlight_";
/*
        vector<pair<float,int> > valid; // csv, idx
        for(int i = 0 ; i<e->NcentralJets() ;++i)
            {
                valid.push_back(pair<float,int>(e->GetCentralJet(i)->bdiscr,i));
            }

        std::sort(valid.begin(), valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;
*/
        //////////

        if(evt_MH_t1b>0)  Fill("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_"+LabelHF+label,systname,evt_MH_t1b,e->weight());
        else if(evt_MH_t0b>0)  Fill("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_"+LabelHF+label,systname,evt_MH_t0b,e->weight());
        else if(evt_MH_Wbb>0)  Fill("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_"+LabelHF+label,systname,evt_MH_Wbb,e->weight());
        else if(evt_MH_wbj>0)  Fill("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_"+LabelHF+label,systname,evt_MH_wbj,e->weight());




/*
        if(!(Sregion.find("SR")!=string::npos)) {
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+Sregion+LabelHF+label,systname, e->Bjets() ,e->weight());
            if(e->NcentralJets()>=3 && valid.size() != 0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+Sregion+LabelHF+label,systname, valid[2].first ,e->weight());

        }

        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());
*/
    }

}


int ChargedHiggsTopBottomFullHad::analyze(Event*e,string systname)
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

    if ( e->Nleps() == 0 ) cut.SetCutBit(NoLep); // kill Top/W/Z
    if ( cut.passAllUpTo(NoLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoLep,e->weight());
    if ( e->Ntaus() == 0 ) cut.SetCutBit(NoTau); // tau Veto <--- dilepton killer
    if ( cut.passAllUpTo(NoTau) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoTau,e->weight());

    if(!cut.passAllUpTo(NoTau) )   return EVENT_NOT_USED;

    //    std::cout << " e->NFatJets() = " << e->NFatJets()  << std::endl;

    // HT cut to simulate the trigger
    computeVar(e);
    if ( evt_HT<800 ) cut.SetCutBit(HTcut); //HT cut
    if ( cut.passAllUpTo(HTcut) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,HTcut,e->weight());
    if(evt_HT<800) return EVENT_NOT_USED;
    if(e->Bjets() == 0) return EVENT_NOT_USED;    

    ////////
    //// UP TO NOW: LEPTONS selection only
    ////

    bool Baseline=(e->Bjets() > 0);
    bool rightCombination =true;

    if ( not e->IsRealData() and ((label.find("HplusToTB") !=string::npos ) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ))){

        rightCombination=genInfoForSignal(e); // compute the right combination in the higgs case

    }
    
    string category="";
    jetPlot(e, label, category, systname,"Baseline");


    if(e->Bjets()>0 && (e->Wjets()>0 || e->Topjets()>0)) {
        cut.SetCutBit(OneBOneFat); //OneBOneFat
        if ( cut.passAllUpTo(OneBOneFat) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,OneBOneFat,e->weight());

        // make candidate top+b, W+b+b,w+b+j
        getCandidate(e,label,systname,"OneBOneFat",false);
        if(e->Bjets() == 1) {getCandidate(e,label,systname,"OneBOneFat_one",false); fatjetPlot(e,label,systname,"OneBOneFat_one");classifyHF(e,label,"",systname,"OneBOneFat_one","");}
        else if(e->Bjets() == 2) {getCandidate(e,label,systname,"OneBOneFat_two",false); fatjetPlot(e,label,systname,"OneBOneFat_two");classifyHF(e,label,"",systname,"OneBOneFat_two","");}
        else if(e->Bjets() >= 3) {getCandidate(e,label,systname,"OneBOneFat_three",false); fatjetPlot(e,label,systname,"OneBOneFat_three");classifyHF(e,label,"",systname,"OneBOneFat_three","");}

        if ( not e->IsRealData() and ((label.find("HplusToTB") !=string::npos ) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) or (label.find("QCD_HT") !=string::npos))){
            classifyLabelGenEv(e,label,systname,"OneBOneFat");
        }
    }

    if(e->Bjets()>0 && (e->WjetsMirror()>0 || e->TopjetsMirror()>0)) {
        cut.SetCutBit(OneBOneFatMirror);
        if ( cut.passAllUpTo(OneBOneFatMirror) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,OneBOneFatMirror,e->weight());

        getCandidate(e,label,systname,"OneBOneMirrorFat",true);
        if(e->Bjets() == 1) getCandidate(e,label,systname,"OneBOneMirrorFat_one",true);
        else if(e->Bjets() == 2) getCandidate(e,label,systname,"OneBOneMirrorFat_two",true);
        else if(e->Bjets() >= 3) getCandidate(e,label,systname,"OneBOneMirrorFat_three",true);

    }


    if(e->Bjets() == 0){ //control region
        if ( not e->IsRealData() and (label.find("QCD_HT") !=string::npos) ){
            if(e->Topjets() > 0) fatjetPlot(e,label,systname,"NoBOneTop");
            else if(e->Wjets()>0) fatjetPlot(e,label,systname,"NoBOneW");
        }
    }


    //  set the genMatching for good or bad combination and fill the Mass of the Higgs candidate
    //        classifyLabelGenEv(e,label,systname,"NoBOneFat");
    //        jetPlot(e, label, category, systname,"NoBOneFat");
    //    }

    /*
    if(e->Wjets()>0 || e->Topjets()>0) {

        cout << "=======================================" << endl;
        cout << "N(W) jets = " << e->Wjets() << endl;
        cout << "N(TOP) jets = " << e->Topjets() << endl;

        cout << " evt_MH_tb=" << evt_MH_tb << " evt_MH_Wbb=" << evt_MH_Wbb << endl;

        for(int i=0;i!=e->NFatJets();++i) {
            FatJet* j = e->GetFatJet(i);
            std::cout << " Pt()= " << j->Pt();
            std::cout << " Tau1()= " << j->Tau1();
            std::cout << " Tau2()= " << j->Tau2();
            std::cout << " Tau3()= " << j->Tau3();
            std::cout << " SDMass()= " << j->SDMass();
            std::cout << " CorrPrunedMass()= " << j->CorrPrunedMass() << endl;
            std::cout << " subjet_btag= " << j->IsSubjetBTag()   << endl;
            std::cout << " is TOP jet = " << j->IsTopJet()   << endl;
            std::cout << " is W jet = " << j->IsWJet()   << endl;
        }
    }
    */

    // ////////
    // ////
    // //// Fill tree
    // ////

    if (systname.find("NONE")    !=string::npos) {
        if(writeTree) setTree(e,label,category);
        if(writeTree) FillTree("tree_tb");
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
