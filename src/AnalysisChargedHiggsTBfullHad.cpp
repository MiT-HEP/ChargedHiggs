#include "interface/AnalysisChargedHiggsTBfullHad.hpp"

void ChargedHiggsTopBottomFullHad::SetLeptonCuts(Lepton *l){
    // these are used for the Veto
    l->SetPtCut(10);
    l->SetIsoCut(-1.); // absolute isolation
    //https://indico.cern.ch/event/594396/contributions/2402538/attachments/1389409/2116003/20161215_MuonPOG_GeneralPPD.pdf
    l->SetMiniIsoRelCut(0.40); // relative mini-isolation  // 0.1 is tight and 0.4 is for loose
    l->SetEtaCut(2.4);
    l->SetVetoCut(); // loosest selection of them all
    if(abs(l->GetType())==13) l->SetLooseCut(true); // loosest selection of them all
    l->SetTightCut(false); // use the loose selection for now
    l->SetMvaLooseCut(true);

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

void ChargedHiggsTopBottomFullHad::SetFatJetCuts(FatJet *f){
    f->SetEtaCut(2.4);
    f->SetPtCut(450);
    f->SetSDMassCut(65);
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

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+match+"tcut_"+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+match+"outtcut_"+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+match+"wcut_"+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+match+"outwcut_"+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_tcut_"+match+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outtcut_"+match+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_wcut_"+match+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outwcut_"+match+l,"SDMass "+l + ";SDMass [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau32_"+match+l, "tau32 "+l + ";tau32 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau21_"+match+l, "tau21 "+l + ";tau21 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/sdtau32_"+match+l, "sdtau32 "+l + ";sdtau32 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/sdtau21_"+match+l, "sdtau21 "+l + ";sdtau21 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/top_sdtau32_"+match+l, "top_sdtau32 "+l + ";sdtau32 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/W_sdtau21_"+match+l, "W_sdtau21 "+l + ";sdtau21 ",120,0.,1.2);
    
}

void ChargedHiggsTopBottomFullHad::BookGenTTBar(string l, string category, string phasespace, string labelHF){

    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_tt2bMerged_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_tt2b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_tt1b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_ttc_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_ttlight_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/invariantExt_other_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_tt2bMerged_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_tt2b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_tt1b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_ttc_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_ttlight_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/invariantExt_other_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_tt2bMerged_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_tt2b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_tt1b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_ttc_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_ttlight_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/invariantExt_other_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_tt2bMerged_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_tt2b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_tt1b_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_ttc_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_ttlight_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/invariantExt_other_"+l,"invariantExt "+labelHF+l + ";Mass (tb) [GeV] ",360,100,3700);


}

void ChargedHiggsTopBottomFullHad::BookFatjetPro(string l, string category, string phasespace, string pile){


    Book("ChargedHiggsTopBottom/"+phasespace+category+"/top_pt_"+pile+l,"top_pt_ "+pile+l + "; top[GeV] ",120,400,1600); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/W_pt_"+pile+l,"W_pt_ "+pile+l + "; W [GeV]",140,200,1600); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/fat_pt_"+pile+l,"fat_pt_ "+pile+l + "; FatJet [GeV]",160,0,1600); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NSubJetB_"+pile+l,"NSubJetB_ "+pile+l + "; NSubJetB",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NSubJetBLoose_"+pile+l,"NSubJetBLoose_ "+pile+l + "; NSubJetBLoose",10,0,10);


    if(doTrigger){
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_OR_"+pile+l,"fatpt_OR "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_AK8PFJet450_"+pile+l,"fatpt_AK8PFJet450 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_PFHT900_"+pile+l,"fatpt_PFHT900 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_AK8PFHT700_"+pile+l,"fatpt_AK8PFHT700 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_AK8DiPFJet300_200_"+pile+l,"fatpt_AK8DiPFJet300_200 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_AK8PFJet400_"+pile+l,"fatpt_AK8PFJet400 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_AK8PFHT800_"+pile+l,"fatpt_AK8PFHT800 "+pile+l+"; FatJet [GeV]",160,0,1600);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/fatpt_IsoMu24_"+pile+l,"fatpt_IsoMu24 "+pile+l+"; FatJet [GeV]",160,0,1600);
    }

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau32_"+pile+l,"tau32_ "+pile+l + "; tau32 ",120,0.,1.2); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/tau21_"+pile+l,"tau21_ "+pile+l + "; tau21 ",120,0.,1.2); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_"+pile+l,"SDMass_"+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_tcut_"+pile+l,"SDMass_tcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outtcut_"+pile+l,"SDMass_outtcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_wcut_"+pile+l,"SDMass_wcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/SDMass_outwcut_"+pile+l,"SDMass_outwcut_ "+pile+l + "; SDMass[GeV] ",55,0,220); ///ranges
}

void ChargedHiggsTopBottomFullHad::BookEnCorr(string category, string phasespace){


    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_100","jet_100GeV ;Pt ratio of jet,100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_300","jet_300GeV ;Pt ratio of jet,300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_500","jet_500GeV ;Pt ratio of jet,500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_700","jet_700GeV ;Pt ratio of jet,700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_900","jet_900GeV ;Pt ratio of jet,900GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_1100","jet_1100GeV ;Pt ratio of jet,1100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_1300","jet_1300GeV ;Pt ratio of jet,1300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_1500","jet_1500GeV ;Pt ratio of jet,1500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_1700","jet_1700GeV ;Pt ratio of jet,1700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrJet_1800plus","jet_over1800GeV ;Pt ratio of jet,over1800GeV",200,0,2);

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_100","jet_100GeV ;RawPt ratio of jet,100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_300","jet_300GeV ;RawPt ratio of jet,300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_500","jet_500GeV ;RawPt ratio of jet,500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_700","jet_700GeV ;RawPt ratio of jet,700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_900","jet_900GeV ;RawPt ratio of jet,900GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_1100","jet_1100GeV ;RawPt ratio of jet,1100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_1300","jet_1300GeV ;RawPt ratio of jet,1300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_1500","jet_1500GeV ;RawPt ratio of jet,1500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_1700","jet_1700GeV ;RawPt ratio of jet,1700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrRAWJet_1800plus","jet_over1800GeV ;RawPt ratio of jet,over1800GeV",200,0,2);

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_100","b_100GeV ;Pt ratio of b,100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_300","b_300GeV ;Pt ratio of b,300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_500","b_500GeV ;Pt ratio of b,500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_700","b_700GeV ;Pt ratio of b,700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_900","b_900GeV ;Pt ratio of b,900GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_1100","b_1100GeV ;Pt ratio of b,1100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_1300","b_1300GeV ;Pt ratio of b,1300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_1500","b_1500GeV ;Pt ratio of b,1500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_1700","b_1700GeV ;Pt ratio of b,1700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrBottom_1800plus","b_over1800GeV ;Pt ratio of b,over1800GeV",200,0,2);


    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_100","top_100GeV ;energy ratio of top,100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_300","top_300GeV ;energy ratio of top,300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_500","top_500GeV ;energy ratio of top,500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_700","top_700GeV ;energy ratio of top,700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_900","top_900GeV ;energy ratio of top,900GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_1100","top_1100GeV ;energy ratio of top,1100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_1300","top_1300GeV ;energy ratio of top,1300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_1500","top_1500GeV ;energy ratio of top,1500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_1700","top_1700GeV ;energy ratio of top,1700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrTop_1800plus","top_over1800GeV ;energy ratio of top,over1800GeV",200,0,2);

    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_100","W_100GeV ;energy ratio of W,100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_300","W_300GeV ;energy ratio of W,300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_500","W_500GeV ;energy ratio of W,500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_700","W_700GeV ;energy ratio of W,700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_900","W_900GeV ;energy ratio of W,900GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_1100","W_1100GeV ;energy ratio of W,1100GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_1300","W_1300GeV ;energy ratio of W,1300GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_1500","W_1500GeV ;energy ratio of W,1500GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_1700","W_1700GeV ;energy ratio of W,1700GeV",200,0,2);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/CorrW_1800plus","W_over1800GeV ;energy ratio of W,over1800GeV",200,0,2);
}


void ChargedHiggsTopBottomFullHad::BookHistojet(string l, string category, string phasespace)
{

    cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Final Histo" <<l << " category=" << category << " phasespace="  << phasespace <<endl;

    /**********************************************
     *        Book histos                         *
     **********************************************/
    
    /////
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",40,0,2000);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+l,"HT "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);

    /////
    if(doTrigger){
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_OR_"+l,"HT_OR "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFJet450_"+l,"HT_AK8PFJet450 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_PFHT900_"+l,"HT_PFHT900 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFHT700_"+l,"HT_AK8PFHT700 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8DiPFJet300_200_"+l,"HT_AK8DiPFJet300_200 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFJet400_"+l,"HT_AK8PFJet400 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFHT800_"+l,"HT_AK8PFHT800 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/HT_IsoMu24_"+l,"HT_IsoMu24 "+l+"; HT (P_{T}^{jet}>40 [GeV])",800,0,8000);   
    }    

    /////
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_zoom_"+l,"ST "+l+"; ST ( HT+met+lepsPt )",50,0,2500);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+l,"ST "+l+"; ST ( HT+MET+lepsPt )",800,0,8000);

    if(doTrigger){
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_OR_"+l,"ST_OR "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFJet450_"+l,"ST_AK8PFJet450 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_PFHT900_"+l,"ST_PFHT900 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFHT700_"+l,"ST_AK8PFHT700 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8DiPFJet300_200_"+l,"ST_AK8DiPFJet300_200 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFJet400_"+l,"ST_AK8PFJet400 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFHT800_"+l,"ST_AK8PFHT800 "+l+"; ST (HT+MET)",800,0,8000);
        Book("ChargedHiggsTopBottom/"+phasespace+category+"/ST_IsoMu24_"+l,"ST_IsoMu24 "+l+"; ST (HT+MET)",800,0,8000);
    }

    ///// Vertices
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+l,"Met "+l+";MET [GeV]",50,0.,1000.);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+l,"Vertices "+l + ";Number of vertices",50,0.,50.);
    
    ///// AK4 jets
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 |#eta|<=2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);

    ///// AK8 jets
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NFatjets_"+l,"NFatjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);

    ////lepton
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/muon_pt_"+l,"muon_pt "+l+";muon_pt [GeV]",100,0.,1000.);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/ele_pt_"+l,"ele_pt "+l+";ele_pt [GeV]",100,0.,1000.);

}

void ChargedHiggsTopBottomFullHad::BookHisto(string l, string category, string phasespace)
{

    //hist of higgs plot

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



    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_100_"+l,"jet_100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_300_"+l,"jet_300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_500_"+l,"jet_500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_700_"+l,"jet_700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_900_"+l,"jet_900GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_1100_"+l,"jet_1100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_1300_"+l,"jet_1300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_1500_"+l,"jet_1500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_1700_"+l,"jet_1700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Discr_1800plus_"+l,"jet_over1800GeV "+l + ";discr",120,0,1.2);

    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_100_"+l,"jet_100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_300_"+l,"jet_300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_500_"+l,"jet_500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_700_"+l,"jet_700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_900_"+l,"jet_900GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_1100_"+l,"jet_1100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_1300_"+l,"jet_1300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_1500_"+l,"jet_1500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_1700_"+l,"jet_1700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/Discr_1800plus_"+l,"jet_over1800GeV "+l + ";discr",120,0,1.2);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_100_"+l,"jet_100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_300_"+l,"jet_300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_500_"+l,"jet_500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_700_"+l,"jet_700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_900_"+l,"jet_900GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_1100_"+l,"jet_1100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_1300_"+l,"jet_1300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_1500_"+l,"jet_1500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_1700_"+l,"jet_1700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/Discr_1800plus_"+l,"jet_over1800GeV "+l + ";discr",120,0,1.2);

    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_100_"+l,"jet_100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_300_"+l,"jet_300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_500_"+l,"jet_500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_700_"+l,"jet_700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_900_"+l,"jet_900GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_1100_"+l,"jet_1100GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_1300_"+l,"jet_1300GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_1500_"+l,"jet_1500GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_1700_"+l,"jet_1700GeV "+l + ";discr",120,0,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/Discr_1800plus_"+l,"jet_over1800GeV "+l + ";discr",120,0,1.2);


    ///// Properties of the Higgs candidates
    //t0b
    Book2D("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/Pt_Mass_tb_"+l,"Pt_Mass_tb "+l + ";Mass (tb) [GeV] ",360,100,3700,320,400,3600);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dPhitb_"+l,"dPhitb "+l + ";dPhi (t,b) [GeV] ",100,0,TMath::Pi()); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dEtatb_"+l,"dEtatb "+l + ";dEta (t,b) [GeV] ",100,0,10); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtAsytb_"+l,"PtAsytb "+l + ";(top - b)/(top +b) ",50,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtB1_"+l,"PtB1 "+l + "; B leading ",200,0,1000); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtTopH_"+l,"PtTopH "+l + "; Top Makes Higgs ",100,400,1200); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/nbsubjet_"+l,"nbsubjet "+l + "; nbsubjet ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/nbsubjetloose_"+l,"nbsubjetloose "+l + "; nbsubjetloose ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/softdropM_"+l,"softdropM "+l + "; SDMass of top [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/TAU32_"+l,"TAU32 "+l + "; tau32 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dRHj_"+l,"dRHj "+l + ";DR (H,fatjet)", 100,0,5);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dPhiHj_"+l,"dPhiHj "+l + ";dPhi (H,fatjet)", 100,0,TMath::Pi());
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/dEtaHj_"+l,"dEtaHj "+l + ";dEta (H,fatjet)", 100,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/PtAsyHj_"+l,"PtAsyHj "+l + ";(H - fj)/(H + fj) ",100,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/num_lo_fj_"+l,"num_lo_fj "+l + "; #leftover fatjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/num_lo_bj_"+l,"num_lo_bj "+l + "; #leftover bjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t0b"+"/num_lo_j_"+l,"num_lo_j "+l + "; #leftover jets", 10,0,10);
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
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/nbsubjet_"+l,"nbsubjet "+l + "; nbsubjet ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/nbsubjetloose_"+l,"nbsubjetloose "+l + "; nbsubjetloose ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/softdropM_"+l,"softdropM "+l + "; SDMass of top [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/TAU32_"+l,"TAU32 "+l + "; tau32 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/dRHj_"+l,"dRHj "+l + ";DR (H,fatjet)", 100,0,5);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/dPhiHj_"+l,"dPhiHj "+l + ";dPhi (H,fatjet)", 100,0,TMath::Pi());
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/dEtaHj_"+l,"dEtaHj "+l + ";dEta (H,fatjet)", 100,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/PtAsyHj_"+l,"PtAsyHj "+l + ";(H - fj)/(H + fj) ",100,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/num_lo_fj_"+l,"num_lo_fj "+l + "; #leftover fatjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/num_lo_bj_"+l,"num_lo_bj "+l + "; #leftover bjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_t1b"+"/num_lo_j_"+l,"num_lo_j "+l + "; #leftover jets", 10,0,10);
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
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/PtWH_"+l,"PtWH "+l + "; W ",150,0,1500);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/nbsubjet_"+l,"nbsubjet "+l + "; nbsubjet ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/nbsubjetloose_"+l,"nbsubjetloose "+l + "; nbsubjetloose ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/softdropM_"+l,"softdropM "+l + "; SDMass of top [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/TAU21_"+l,"TAU21 "+l + "; tau21 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/dRHj_"+l,"dRHj "+l + ";DR (H,fatjet)", 100,0,5);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/dPhiHj_"+l,"dPhiHj "+l + ";dPhi (H,fatjet)", 100,0,TMath::Pi());
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/dEtaHj_"+l,"dEtaHj "+l + ";dEta (H,fatjet)", 100,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/PtAsyHj_"+l,"PtAsyHj "+l + ";(H - fj)/(H + fj) ",100,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/num_lo_fj_"+l,"num_lo_fj "+l + "; #leftover fatjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/num_lo_bj_"+l,"num_lo_bj "+l + "; #leftover bjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbb"+"/num_lo_j_"+l,"num_lo_j "+l + "; #leftover jets", 10,0,10);
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
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/PtWH_"+l,"PtWH "+l + "; W ",150,0,1500);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/nbsubjet_"+l,"nbsubjet "+l + "; nbsubjet ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/nbsubjetloose_"+l,"nbsubjetloose "+l + "; nbsubjetloose ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/softdropM_"+l,"softdropM "+l + "; SDMass of top [GeV] ",55,0,220);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/TAU21_"+l,"TAU21 "+l + "; tau21 ",120,0.,1.2);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/dRHj_"+l,"dRHj "+l + ";DR (H,fatjet)", 100,0,5);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/dPhiHj_"+l,"dPhiHj "+l + ";dPhi (H,fatjet)", 100,0,TMath::Pi());
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/dEtaHj_"+l,"dEtaHj "+l + ";dEta (H,fatjet)", 100,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/PtAsyHj_"+l,"PtAsyHj "+l + ";(H - fj)/(H + fj) ",100,0.,1); ///ranges
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/num_lo_fj_"+l,"num_lo_fj "+l + "; #leftover fatjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/num_lo_bj_"+l,"num_lo_bj "+l + "; #leftover bjets", 10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+"_wbj"+"/num_lo_j_"+l,"num_lo_j "+l + "; #leftover jets", 10,0,10);    

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

//    BookEnCorr("","Baseline");

    for ( string l : AllLabel()  ) {

        BookCutFlow(l, "");
        BookHistojet(l, "","Baseline"); // this is when there is nothing
        BookHistojet(l, "","OneBOneFat");


        BookFatjetPro(l, "","Baseline","");
        BookFatjetPro(l, "","OneBOneFat", "");

        BookHisto(l, "","OneBOneFat");
    
        if(doZeroB){
            BookHistojet(l, "","NoB");
            BookHistojet(l, "","NoBOneTop");
            BookHistojet(l, "","NoBOneW");
            BookFatjetPro(l, "","NoB","");
            BookFatjetPro(l, "","NoBOneTop","");
            BookFatjetPro(l, "","NoBOneW","");
        }

        if(doLep){
            BookHistojet(l, "","OneBOneFat_Ele");
            BookHistojet(l, "","OneBOneFat_Mu");
            BookFatjetPro(l, "","OneBOneFat_Ele", "");
            BookFatjetPro(l, "","OneBOneFat_Mu", "");
            BookHisto(l, "","OneBOneFat_Ele");
            BookHisto(l, "","OneBOneFat_Mu");
        }

        if(doSig){
            BookHistojet(l, "_t0b", "OneBOneFat");
            BookHistojet(l, "_t1b", "OneBOneFat");
            BookHistojet(l, "_wbb", "OneBOneFat");
            BookHistojet(l, "_wbj", "OneBOneFat");
            BookHisto(l, "","OneBOneFat_one_lowj");
            BookHisto(l, "","OneBOneFat_two_lowj");
            BookHisto(l, "","OneBOneFat_three_lowj");
            BookHisto(l, "","OneBOneFat_one_highj");
            BookHisto(l, "","OneBOneFat_two_highj");
            BookHisto(l, "","OneBOneFat_three_highj");        

/*            BookFatjetPro(l, "_t0b", "OneBOneFat", "");
            BookFatjetPro(l, "_t1b", "OneBOneFat", "");
            BookFatjetPro(l, "_wbb", "OneBOneFat", "");
            BookFatjetPro(l, "_wbj", "OneBOneFat", "");
            BookFatjetPro(l, "_no", "OneBOneFat", "");   */
        }

        if(doMirror){
            BookHistojet(l, "","OneBOneMirrorFat");
            BookHistojet(l, "_t0b", "OneBOneMirrorFat");
            BookHistojet(l, "_t1b", "OneBOneMirrorFat");
            BookHistojet(l, "_wbb", "OneBOneMirrorFat");
            BookHistojet(l, "_wbj", "OneBOneMirrorFat");


            BookFatjetPro(l, "", "OneBOneMirrorFat", "");
/*            BookFatjetPro(l, "_t0b", "OneBOneMirrorFat", "");
            BookFatjetPro(l, "_t1b", "OneBOneMirrorFat", "");
            BookFatjetPro(l, "_wbb", "OneBOneMirrorFat", "");
            BookFatjetPro(l, "_wbj", "OneBOneMirrorFat", "");
            BookFatjetPro(l, "_no", "OneBOneMirrorFat", ""); */

            BookHisto(l, "","OneBOneMirrorFat");
/*            BookHisto(l, "","OneBOneMirrorFat_one_lowj");
            BookHisto(l, "","OneBOneMirrorFat_two_lowj");
            BookHisto(l, "","OneBOneMirrorFat_three_lowj");
            BookHisto(l, "","OneBOneMirrorFat_one_highj");
            BookHisto(l, "","OneBOneMirrorFat_two_highj");
            BookHisto(l, "","OneBOneMirrorFat_three_highj");
*/
        }
        
        if(doPileUp){
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
        }
        
        if(doGentt){
            BookGenTTBar(l, "", "OneBOneFat", "");
            BookGenTTBar(l, "", "OneBOneFat_one", "");
            BookGenTTBar(l, "", "OneBOneFat_two", "");
            BookGenTTBar(l, "", "OneBOneFat_three", "");
        }
        
        if(doGenSig){
            BookGenMatch(l, "", "OneBOneFat", "", "lept_");
            BookGenMatch(l, "", "OneBOneFat", "", "hadt_");
            BookGenMatch(l, "", "OneBOneFat", "", "lepw_");
            BookGenMatch(l, "", "OneBOneFat", "", "hadw_");
            BookGenMatch(l, "", "OneBOneFat", "", "b_");
            BookGenMatch(l, "", "OneBOneFat", "", "no_");

            BookGenMatch(l, "", "OneBOneFat", "", "top_");
            BookGenMatch(l, "", "OneBOneFat", "", "W_");
            BookGenMatch(l, "", "OneBOneFat", "", "bottom_");
            BookGenMatch(l, "", "OneBOneFat", "", "weirda_");
            BookGenMatch(l, "", "OneBOneFat", "", "weirdb_");
            BookGenMatch(l, "", "OneBOneFat", "", "QCD_");
            BookGenMatch(l, "", "OneBOneFat", "", "none_");

            BookGenMatch(l, "", "Baseline", "", "lept_");
            BookGenMatch(l, "", "Baseline", "", "hadt_");
            BookGenMatch(l, "", "Baseline", "", "lepw_");
            BookGenMatch(l, "", "Baseline", "", "hadw_");
            BookGenMatch(l, "", "Baseline", "", "b_");
            BookGenMatch(l, "", "Baseline", "", "no_");

            BookGenMatch(l, "", "Baseline", "", "top_");
            BookGenMatch(l, "", "Baseline", "", "W_");
            BookGenMatch(l, "", "Baseline", "", "bottom_");
            BookGenMatch(l, "", "Baseline", "", "weirda_");
            BookGenMatch(l, "", "Baseline", "", "weirdb_");
            BookGenMatch(l, "", "Baseline", "", "QCD_");
            BookGenMatch(l, "", "Baseline", "", "none_");
        }





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

    siglep = 0;

    bool rightComb=false;

    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        //if(abs(genpar->GetPdgId()) == 6 || abs(genpar->GetPdgId()) == 37) cout << "gen id is: " << genpar->GetPdgId() << endl;
        //ch-higgs
        if(abs(genpar->GetPdgId()) == 34 || abs(genpar->GetPdgId()) == 37) if(genCH==NULL) genCH = genpar;
    }

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);
        if(genCH!=NULL){
            //top
            if( abs(genpar->GetPdgId()) == 6 && (abs(genpar->GetParentPdgId()) == 34 || abs(genpar->GetParentPdgId()) == 37) ) topFromH = genpar;
            //bottom
            else if( abs(genpar->GetPdgId()) == 5 && (abs(genpar->GetParentPdgId()) == 34 || abs(genpar->GetParentPdgId()) == 37)) bFromH = genpar;
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
            if(genCH != NULL && abs(genpar->GetParentPdgId()) != 34 && abs(genpar->GetParentPdgId()) != 37) topAss = genpar;
            if(genCH == NULL){
                if(genpar->GetPdgId() == 6)topBKGplus = genpar;
                if(genpar->GetPdgId() == -6)topBKGminus = genpar;
            }
        //lepton
        }else if( ((abs(genpar->GetPdgId()) == 11 or abs(genpar->GetPdgId()) == 13) and (genpar->IsPromptFinalState() or genpar->IsDirectPromptTauDecayProductFinalState()))
                  or (abs(genpar->GetPdgId()) == 15 and genpar->IsPromptDecayed()) ) {
            siglep = 1;
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


void ChargedHiggsTopBottomFullHad::Encorrection(Event*e, string systname, string phasespace) {

    string range_j = "";
    string range_b = "";
    string range_t = "";
    string range_W = "";


    //Test rawPt
    for(Int_t k = 0; k!=e->Njets(); k++){
        Jet *rjet = e->GetJet(k);
        bool thisistop = 0;
        bool thisisb   = 0;
        for(int kfj=0;kfj!=e->NFatJets();kfj++) {
            FatJet* AK8j = e->GetFatJet(kfj);
            if( AK8j->IsTopJet() ) { if(rjet->DeltaR(*AK8j) < 1.2) thisistop = 1; }
        }
        if(thisistop) continue;
        switch(int(rjet->Pt()/200.)){
            case 0: range_j = "100"; break;
            case 1: range_j = "300"; break;
            case 2: range_j = "500"; break;
            case 3: range_j = "700"; break;
            case 4: range_j = "900"; break;
            case 5: range_j = "1100"; break;
            case 6: range_j = "1300"; break;
            case 7: range_j = "1500"; break;
            case 8: range_j = "1700"; break;
            default: range_j = "1800plus";
        }
        for(Int_t iya = 0; iya < e->NGenPar(); iya++){
            GenParticle *gbjet = e->GetGenParticle(iya);
            if(abs(gbjet->GetPdgId()) != 5) continue;
            if(rjet->DeltaR(*gbjet) < 0.4) {thisisb = 1;Fill("ChargedHiggsTopBottom/"+phasespace+"/CorrBottom_"+range_j ,systname, rjet->Pt()/gbjet->Pt() ,e->weight());}
        }   
        if(!thisisb) continue;
        for(Int_t i = 0; ; i++){
            GenJet *gjbjet = e->GetGenJet(i);
            if(gjbjet==NULL) break;
            if(rjet->DeltaR(*gjbjet) < 0.4) Fill("ChargedHiggsTopBottom/"+phasespace+"/CorrJet_"+range_j ,systname, rjet->Pt()/gjbjet->Pt() ,e->weight());
        }
        switch(int(rjet->rawPt/200.)){
            case 0: range_j = "100"; break;
            case 1: range_j = "300"; break;
            case 2: range_j = "500"; break;
            case 3: range_j = "700"; break;
            case 4: range_j = "900"; break;
            case 5: range_j = "1100"; break;
            case 6: range_j = "1300"; break;
            case 7: range_j = "1500"; break;
            case 8: range_j = "1700"; break;
            default: range_j = "1800plus";
        }
        for(Int_t i = 0; ; i++){
            GenJet *gjbjet = e->GetGenJet(i);
            if(gjbjet==NULL) break;
            if(rjet->DeltaR(*gjbjet) < 0.4) Fill("ChargedHiggsTopBottom/"+phasespace+"/CorrRAWJet_"+range_j ,systname, rjet->rawPt/gjbjet->Pt() ,e->weight());
        }
    }


    //Gentop
    for(Int_t k = 0; k!=e->NFatJets(); k++){
        FatJet* rtjet = e->GetFatJet(k);
        if(!rtjet->IsTopJet()) continue;    
        switch(int(rtjet->Pt()/200.)){
            case 0: range_t = "100"; break;
            case 1: range_t = "300"; break;
            case 2: range_t = "500"; break;
            case 3: range_t = "700"; break;
            case 4: range_t = "900"; break;
            case 5: range_t = "1100"; break;
            case 6: range_t = "1300"; break;
            case 7: range_t = "1500"; break;
            case 8: range_t = "1700"; break;
            default: range_t = "1800plus";
        }
        for(Int_t i = 0; i < e->NGenPar(); i++){
            GenParticle *gtjet = e->GetGenParticle(i);
            if(abs(gtjet->GetPdgId()) != 6) continue;
            if(rtjet->DeltaR(*gtjet) < 0.8) Fill("ChargedHiggsTopBottom/"+phasespace+"/CorrTop_"+range_t ,systname, rtjet->Pt()/gtjet->Pt() ,e->weight());
        }
    }


    //GenW
    for(Int_t k = 0; k!=e->NFatJets(); k++){
        FatJet* rwjet = e->GetFatJet(k);
        if(!rwjet->IsWJet()) continue;
        switch(int(rwjet->Pt()/200.)){
            case 0: range_W = "100"; break;
            case 1: range_W = "300"; break;
            case 2: range_W = "500"; break;
            case 3: range_W = "700"; break;
            case 4: range_W = "900"; break;
            case 5: range_W = "1100"; break;
            case 6: range_W = "1300"; break;
            case 7: range_W = "1500"; break;
            case 8: range_W = "1700"; break;
            default: range_W = "1800plus";
        }
        for(Int_t i = 0; i < e->NGenPar(); i++){
            GenParticle *gwjet = e->GetGenParticle(i);
            if(abs(gwjet->GetPdgId()) != 24) continue;
            if(rwjet->DeltaR(*gwjet) < 0.8) Fill("ChargedHiggsTopBottom/"+phasespace+"/CorrW_"+range_W ,systname, rwjet->Pt()/gwjet->Pt() ,e->weight());
        }
    }
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

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos) {
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

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos) if( (mintoph < 0.3 && j == topsnum1) || (mintopa < 0.2 && j == topsnum2) ) continue;
        if(label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (mintopbkg1 < 0.2 && j == topbnum1) || (mintopbkg2 < 0.2 && j == topbnum2) ) continue;

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos){
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

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos) if( (mintoph < 0.3 && j == topsnum1) || (mintopa < 0.2 && j == topsnum2) ) continue;
        if (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (mintopbkg1 < 0.2 && j == topbnum1) || (mintopbkg2 < 0.2 && j == topbnum2) ) continue;

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos) if( (minWth < 0.3 && j == Wsnum1) || (minWta < 0.2 && j == Wsnum2) ) continue;
        if (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) if( (minWbkg1 < 0.2 && j == Wbnum1) || (minWbkg2 < 0.2 && j == Wbnum2) ) continue;


        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos){
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
    string fjtype="other_";


    // maybetop
    for(int j=0;j!=e->NFatJets();j++) {

        bool flag_top = 0;
        bool flag_W = 0;
        bool flag_b = 0;

        FatJet* fatjet = e->GetFatJet(j);

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos) {
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

        if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos || label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos) {
            for(Int_t m = 0; m < e->NGenPar(); m++){
                GenParticle *gfjet = e->GetGenParticle(m);
                if(abs(gfjet->GetPdgId()) == 6 && fatjet->DeltaR(*gfjet) < 0.8) flag_top = 1;
                if(abs(gfjet->GetPdgId()) == 24 && fatjet->DeltaR(*gfjet) < 0.8) flag_W = 1;
                if(abs(gfjet->GetPdgId()) == 5 && fatjet->DeltaR(*gfjet) < 0.8) flag_b = 1;
            }

//            if(fatjet->Pt() > 400. && flag_top && flag_W && flag_b) fjtype="top_";
//            else if(fatjet->Pt() > 200. && flag_W && !flag_b) fjtype="W_";
            if(flag_top && flag_W && flag_b) fjtype="top_";
            else if(flag_W && !flag_b) fjtype="W_";
            else if(!flag_W && flag_b) fjtype="bottom_";
            else if(!flag_top && flag_W && flag_b) fjtype="weirda_";
            else if(flag_top && !flag_W && !flag_b) fjtype="weirdb_";
            else fjtype="none_";
        }else if(label.find("QCD_HT") !=string::npos){
            fjtype="none_";
        }
   
        if(fatjet->Pt() > 400.){
            if(fatjet->Tau3()< 0.81*fatjet->Tau2()) Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_tcut_"+fjtype+label,systname,fatjet->SDMass(),e->weight());
            else Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_outtcut_"+fjtype+label,systname,fatjet->SDMass(),e->weight());
        }

        if(fatjet->Pt() > 200.){
            if(fatjet->Tau2()< 0.6*fatjet->Tau1()) Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_wcut_"+fjtype+label,systname,fatjet->SDMass(),e->weight());
            else Fill("ChargedHiggsTopBottom/"+phasespace+"/SDMass_outwcut_"+fjtype+label,systname,fatjet->SDMass(),e->weight());
        }

        if(fatjet->Pt() > 400. && fatjet->Tau2()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+"/tau32_"+fjtype+label ,systname, fatjet->Tau3()/fatjet->Tau2(), e->weight());
        if(fatjet->Pt() > 200. && fatjet->Tau1()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+"/tau21_"+fjtype+label ,systname, fatjet->Tau2()/fatjet->Tau1(), e->weight());

        if(fatjet->Pt() > 400. && fatjet->SDTau2()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+"/sdtau32_"+fjtype+label ,systname, fatjet->SDTau3()/fatjet->SDTau2(), e->weight());
        if(fatjet->Pt() > 200. && fatjet->SDTau1()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+"/sdtau21_"+fjtype+label ,systname, fatjet->SDTau2()/fatjet->SDTau1(), e->weight());

        if(fatjet->Pt() > 400. && fatjet->SDTau2()!=0 && fatjet->SDMass() > 105 && fatjet->SDMass() < 220) Fill("ChargedHiggsTopBottom/"+phasespace+"/top_sdtau32_"+fjtype+label ,systname, fatjet->SDTau3()/fatjet->SDTau2(), e->weight());
        if(fatjet->Pt() > 200. && fatjet->SDTau1()!=0 && fatjet->SDMass() > 65 && fatjet->SDMass() < 105) Fill("ChargedHiggsTopBottom/"+phasespace+"/W_sdtau21_"+fjtype+label ,systname, fatjet->SDTau2()/fatjet->SDTau1(), e->weight());

    }

    return;

    //////
    ////// Define the label for the tb and wbb categories
    //////

    string LabelGenT0B="other_";
    string LabelGenT1B="other_";
    string LabelGenWBB="other_";
    string LabelGenWBJ="other_";

    if(label.find("HplusToTB") !=string::npos || label.find("WprimeToTB") !=string::npos){

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
    secondbwbb=NULL;
    secondb = NULL;

    numtop = 0;
    topJet=NULL;
    wJet=NULL;
    wJetwbj=NULL;

    topFromHOpenCand.SetXYZM(0.,0.,0.,0.);
    topFromHwbj.SetXYZM(0.,0.,0.,0.);


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

    Double_t min_topm = 999;
    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* temp = e->GetFatJet(j);

        if((mirror and temp->IsTopJetMirror()) or (!mirror and temp->IsTopJet())) {
            numtop++;

            Double_t temp_topm = fabs(temp->SDMass() - ChargedHiggs::Mtop);
            if(temp_topm < min_topm){
                min_topm = temp_topm;
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
                    secondbwbb=e->GetBjet(ib);
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
                wJetwbj = temp;
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
                wJetwbj = temp;
                evt_MH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).M();
                evt_PtH_wbj=(topFromHwbj+leadingbWBJ->GetP4()).Pt();
            }
        }
    }

    higgsPlot(e, label, "_wbj",systname,phasespace);


//    higgsPlot(e, label, "_tb",systname,phasespace);
//    higgsPlot(e, label, "_wbb",systname,phasespace);
}

void ChargedHiggsTopBottomFullHad::PlotAss(Event*e, string label, string systname, string phasespace) {

    num_otherfj = 0;
    num_otherbj = 0;
    num_otherj = 0;

    string range_j = "";

    string sigcategory = "_other";

    TLorentzVector potentialH;
    if(evt_MH_t0b>0) {potentialH = topJet->GetP4() + leadingb->GetP4(); sigcategory = "_t0b";}
    else if(evt_MH_t1b>0) {potentialH = topJet->GetP4() + leadingb->GetP4(); sigcategory = "_t1b";}
    else if(evt_MH_Wbb>0) {potentialH = topFromHOpenCand + leadingb->GetP4(); sigcategory = "_wbb";}
    else if(evt_MH_wbj>0) {potentialH = topFromHwbj + leadingbWBJ->GetP4(); sigcategory = "_wbj";}
    else return;


    for(int j=0;j!=e->NFatJets();j++) {
        
        if(topJet!=NULL && e->GetFatJet(j) == topJet) continue;
        if(wJet!=NULL && e->GetFatJet(j) == wJet) continue;
        if(wJetwbj!=NULL && e->GetFatJet(j) == wJetwbj) continue;
        if(e->GetFatJet(j)->IsSubjetBTag()!=0) continue;       
 
        num_otherfj++;
//        FatJet* temp = e->GetFatJet(j);

//        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/dPhiHj_"+label,systname, ChargedHiggs::deltaPhi(potentialH.Phi(), temp->Phi()) ,e->weight());
//        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/dEtaHj_"+label,systname, fabs(potentialH.Eta() - temp->Eta()), e->weight());
//        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/PtAsyHj_"+label,systname,  (potentialH.Pt() - temp->Pt())/(potentialH.Pt() + temp->Pt()),e->weight());
//        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/dRHj_"+label,systname,   potentialH.DeltaR(temp->GetP4()), e->weight());

    }

    for(int j=0;j!=e->Njets();j++) {

        Jet *rjet = e->GetJet(j);
        switch(int(rjet->Pt()/200.)){
            case 0: range_j = "100_"; break;
            case 1: range_j = "300_"; break;
            case 2: range_j = "500_"; break;
            case 3: range_j = "700_"; break;
            case 4: range_j = "900_"; break;
            case 5: range_j = "1100_"; break;
            case 6: range_j = "1300_"; break;
            case 7: range_j = "1500_"; break;
            case 8: range_j = "1700_"; break;
            default: range_j = "1800plus_";
        }
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/Discr_"+range_j+label ,systname, rjet->bdiscr ,e->weight());

    
        if(leadingb!=NULL && e->GetJet(j) == leadingb) continue;
        if(leadingbWBJ!=NULL && e->GetJet(j) == leadingbWBJ) continue;
        if(secondb!=NULL && e->GetJet(j) == secondb) continue;
        if(secondbwbb!=NULL && e->GetJet(j) == secondbwbb) continue;      
        if(topJet!=NULL && e->GetJet(j)->DeltaR(*topJet) < 1.2) continue;
        if(wJet!=NULL && e->GetJet(j)->DeltaR(*wJet) < 1.2) continue;
        if(wJetwbj!=NULL && e->GetJet(j)->DeltaR(*wJetwbj) < 1.2) continue;
        if(evt_MH_Wbb>0 && topFromHOpenCand.DeltaR(e->GetJet(j)->GetP4()) < 1.2) continue;
        if(evt_MH_wbj>0 && topFromHwbj.DeltaR(e->GetJet(j)->GetP4()) < 1.2) continue;

        num_otherj++;

    }

    
    for(int j=0; j!= e->Bjets() ; j++){

        if(leadingb!=NULL && e->GetBjet(j) == leadingb) continue;
        if(leadingbWBJ!=NULL && e->GetBjet(j) == leadingbWBJ) continue;
        if(secondb!=NULL && e->GetBjet(j) == secondb) continue;
        if(secondbwbb!=NULL && e->GetBjet(j) == secondbwbb) continue;
        if(topJet!=NULL && e->GetBjet(j)->DeltaR(*topJet) < 1.2) continue;
        if(wJet!=NULL && e->GetBjet(j)->DeltaR(*wJet) < 1.2) continue;
        if(wJetwbj!=NULL && e->GetBjet(j)->DeltaR(*wJetwbj) < 1.2) continue;
        if(evt_MH_Wbb>0 && topFromHOpenCand.DeltaR(e->GetBjet(j)->GetP4()) < 1.2) continue;
        if(evt_MH_wbj>0 && topFromHwbj.DeltaR(e->GetBjet(j)->GetP4()) < 1.2) continue;

        num_otherbj++;
    }


    Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/num_lo_fj_"+label,systname, num_otherfj, e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/num_lo_bj_"+label,systname, num_otherbj, e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/num_lo_j_"+label,systname, num_otherj, e->weight());
       
 
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
   evt_ST=0;

    // check the impact of the HT(w/ central jet only)
    for(int i=0;i!=e->NcentralJets();++i) {
        evt_HT += e->GetCentralJet(i)->Pt();
    }

    evt_ST=evt_HT+e->GetMet().Pt();
    for(int k=0;k!=e->Nleps();k++) {
        Lepton *lep1 = e->GetLepton(k);
        if( lep1->IsMuon() || lep1->IsElectron() ) evt_ST+=lep1->Pt();
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

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_"+label,systname, evt_ST ,e->weight());

    
    //HT trigger
  if(doTrigger){
    if( (e->IsTriggered("HLT_PFHT900_v") || e->IsTriggered("HLT_AK8PFJet450_v") || e->IsTriggered("HLT_AK8PFJet400_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT800_TrimMass50_v") || e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") || e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v")) && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v")) )
        {Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_OR_"+label,systname, evt_HT ,e->weight());       
         Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_OR_"+label,systname, evt_ST ,e->weight());}
    if(e->IsTriggered("HLT_PFHT900_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))){
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_PFHT900_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_PFHT900_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_AK8PFJet450_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFJet450_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFJet450_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFHT700_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFHT700_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8DiPFJet300_200_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8DiPFJet300_200_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_AK8PFJet400_TrimMass30_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFJet400_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFJet400_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_AK8PFHT800_TrimMass50_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_AK8PFHT800_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_AK8PFHT800_"+label,systname, evt_ST ,e->weight());
    }
    if(e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v")) {
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_IsoMu24_"+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/ST_IsoMu24_"+label,systname, evt_ST ,e->weight());
    }
  } //end if dotrigger

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+label,systname, e->NcentralJets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+label,systname, e->NforwardJets() ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+label,systname, e->Bjets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+label,systname, e->Njets() ,e->weight());
    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NFatjets_"+label,systname, e->NFatJets() ,e->weight());

    Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+label,systname, e->GetMet().Pt() ,e->weight());



}

void ChargedHiggsTopBottomFullHad::fatjetPlot(Event*e, string label, string systname, string phasespace,bool Mirror) {

    string sigcategory = "";
    string puornot = "";

/*
    if(evt_MH_t0b>0) sigcategory = "_t0b";
    else if(evt_MH_t1b>0) sigcategory = "_t1b";
    else if(evt_MH_Wbb>0) sigcategory = "_wbb";
    else if(evt_MH_wbj>0) sigcategory = "_wbj";
    else sigcategory = "_no"; 
*/

    if(doPileUp){
        if(e->Npv() < 20) puornot = "nopu_";
        else puornot = "pu_";
    }

    for(int j=0;j!=e->NFatJets();j++) {

        FatJet* fatjet = e->GetFatJet(j);
        
        if( (!Mirror and fatjet->IsTopJet()) or (Mirror and fatjet->IsTopJetMirror()) ) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/top_pt_"+puornot+label,systname,fatjet->Pt(),e->weight());
        if( (!Mirror and fatjet->IsWJet()) or (Mirror and fatjet->IsWJetMirror()) ) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/W_pt_"+puornot+label,systname,fatjet->Pt(),e->weight());

        // PT
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fat_pt_"+puornot+label,systname,fatjet->Pt(),e->weight());
        //NBSubJet
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/NSubJetB_"+puornot+label,systname,fatjet->IsSubjetBTag(),e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/NSubJetBLoose_"+puornot+label,systname,fatjet->IsSubjetBTagLoose(),e->weight());


      if(doTrigger){
        if( (e->IsTriggered("HLT_PFHT900_v") || e->IsTriggered("HLT_AK8PFJet450_v") || e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") || e->IsTriggered("HLT_AK8PFJet400_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT800_TrimMass50_v")  || e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v")) && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v")) )
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_OR_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_PFHT900_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_PFHT900_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_AK8PFJet450_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_AK8PFJet450_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_AK8PFHT700_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_AK8DiPFJet300_200_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_AK8PFJet400_TrimMass30_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_AK8PFJet400_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_AK8PFHT800_TrimMass50_v") && (e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v"))) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_AK8PFHT800_"+puornot+label,systname, fatjet->Pt() ,e->weight());
        if(e->IsTriggered("HLT_IsoMu24_v") || e->IsTriggered("HLT_Mu50_v")) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/fatpt_IsoMu24_"+puornot+label,systname, fatjet->Pt() ,e->weight());
      } // end if dotrigger


        // taus
        if(fatjet->Tau2()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/tau32_"+puornot+label,systname,fatjet->Tau3()/fatjet->Tau2(),e->weight());
        if(fatjet->Tau1()!=0) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/tau21_"+puornot+label,systname,fatjet->Tau2()/fatjet->Tau1(),e->weight());

        // SD
        Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_"+puornot+label,systname,fatjet->SDMass(),e->weight());

    
        if(doPileUp){
            if(fatjet->Tau3()< 0.81*fatjet->Tau2()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_tcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
            else Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_outtcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());

            if(fatjet->Tau2()< 0.6*fatjet->Tau1()) Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_wcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
            else Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/SDMass_outwcut_"+puornot+label,systname,fatjet->SDMass(),e->weight());
        }
    }

//    Fill("ChargedHiggsTopBottom/"+phasespace+sigcategory+"/HT_"+puornot+label,systname, evt_HT ,e->weight());       


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
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topJet->Phi()) ,e->weight());
        // difference in rapidity
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  topJet->DeltaEta(*leadingb),e->weight());
        // pt asymmetry
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topJet->Pt() - leadingb->Pt())/(topJet->Pt() + leadingb->Pt()),e->weight());
        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topJet->Pt() ,e->weight());
        //NSubJetB
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjet_"+label,systname, topJet->IsSubjetBTag(), e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjetloose_"+label,systname, topJet->IsSubjetBTagLoose(), e->weight());
        //SDMass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/softdropM_"+label,systname, topJet->SDMass() ,e->weight());
        //tau ID
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/TAU32_"+label,systname, topJet->Tau3()/topJet->Tau2() ,e->weight());
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
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topJet->Phi()) ,e->weight());
        // difference in rapidity
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  topJet->DeltaEta(*leadingb),e->weight());
        // pt asymmetry
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topJet->Pt() - leadingb->Pt())/(topJet->Pt() + leadingb->Pt()),e->weight());
        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topJet->Pt() ,e->weight());
        //NSubJet
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjet_"+label,systname, topJet->IsSubjetBTag(), e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjetloose_"+label,systname, topJet->IsSubjetBTagLoose(), e->weight());
        //SDMass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/softdropM_"+label,systname, topJet->SDMass() ,e->weight());
        //tau ID
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/TAU32_"+label,systname, topJet->Tau3()/topJet->Tau2() ,e->weight());
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
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingb->Phi(), topFromHOpenCand.Phi()) ,e->weight());
        // difference in rapidity
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  fabs(topFromHOpenCand.Eta()-leadingb->GetP4().Eta())  ,e->weight());
        // pt asymmetry
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topFromHOpenCand.Pt() - leadingb->Pt())/(topFromHOpenCand.Pt() + leadingb->Pt()),e->weight());

        // mass of the top candidate
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Masst_"+label,systname,  topFromHOpenCand.M() ,e->weight());

        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingb->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topFromHOpenCand.Pt() ,e->weight());
        //Pt of W selected
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtWH_"+label,systname, wJet->Pt() ,e->weight());
        //NBSubJet
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjet_"+label,systname, wJet->IsSubjetBTag(), e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjetloose_"+label,systname, wJet->IsSubjetBTagLoose(), e->weight());
        //SDMass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/softdropM_"+label,systname, wJet->SDMass() ,e->weight());
        //tau ID
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/TAU21_"+label,systname, wJet->Tau2()/wJet->Tau1() ,e->weight());
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
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dPhitb_"+label,systname, ChargedHiggs::deltaPhi(leadingbWBJ->Phi(), topFromHwbj.Phi()) ,e->weight());
        // difference in rapidity
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/dEtatb_"+label,systname,  fabs(topFromHwbj.Eta()-leadingbWBJ->GetP4().Eta())  ,e->weight());
        // pt asymmetry
//        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtAsytb_"+label,systname,  (topFromHwbj.Pt() - leadingbWBJ->Pt())/(topFromHwbj.Pt() + leadingbWBJ->Pt()),e->weight());

        // mass of the top candidate
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/Masst_"+label,systname,  topFromHwbj.M() ,e->weight());

        // pt of the leading b
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtB1_"+label,systname,  leadingbWBJ->Pt() ,e->weight());
        //pt of the selected top
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtTopH_"+label,systname, topFromHwbj.Pt() ,e->weight());
        //Pt of W selected
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/PtWH_"+label,systname, wJetwbj->Pt() ,e->weight());
        //NBSubJet
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjet_"+label,systname, wJetwbj->IsSubjetBTag(), e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/nbsubjetloose_"+label,systname, wJetwbj->IsSubjetBTagLoose(), e->weight());
        //SDMass
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/softdropM_"+label,systname, wJetwbj->SDMass() ,e->weight());
        //tau ID
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/TAU21_"+label,systname, wJetwbj->Tau2()/wJetwbj->Tau1() ,e->weight());
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

    /////
    ///// APPLY ALL THE SCALE FACTORS
    /////

//b-tag scale
    auto sf=dynamic_cast<SF_CSVReweight*>(e->GetWeight()->GetSF("btag-reweight"));
    if (sf == NULL)  Log(__FUNCTION__,"ERROR","Unable to find btag reweight sf");

    for(int i=0;i!=e->NcentralJets();++i) {
        Jet* lj = e->GetCentralJet(i);
        sf->add_to_sf(lj->GetP4().Pt(),lj->GetP4().Eta(),lj->bdiscr,lj->hadFlavor());
    }

    sf->set();
    if ( not e->IsRealData() ) e->ApplySF("btag-reweight");

//top_pt scale

    if (not e->IsRealData() and (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=string::npos)) e->ApplyTopReweight();



// TRIGGER STORY

    bool passtrigger = (e->IsTriggered("HLT_AK8DiPFJet300_200_TrimMass30_v") || e->IsTriggered("HLT_AK8PFJet450_v") || e->IsTriggered("HLT_PFHT900_v") || e->IsTriggered("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v") || e->IsTriggered("HLT_AK8PFJet400_TrimMass30_v") || e->IsTriggered("HLT_AK8PFHT800_TrimMass50_v"));

    if(!passtrigger) return EVENT_NOT_USED;


// OFFLINE
    computeVar(e);
    
    //HT scale
    if( not e->IsRealData() and (label.find("QCD_HT")!=string::npos)){
        string sfname="QCDHTReweighting";

        if( not e->ExistSF(sfname) ){
            LogN(__FUNCTION__,"WARNING","QCD HT SF does not exist",10);
            return EVENT_NOT_USED;
        }

        e->SetPtEtaSF(sfname,evt_HT,0);
        e->ApplySF(sfname);
    }

    //HT cut
    if(evt_HT<900) return EVENT_NOT_USED;

//    if(doSig && evt_HT<800) return EVENT_NOT_USED;
//    if(doLep && !doSig && evt_ST<800) return EVENT_NOT_USED;


    /////
    ///// MAKE PLOTS HERE
    /////


    if(e->Bjets() == 0) return EVENT_NOT_USED;


    if ( e->Ntaus() == 0 ) cut.SetCutBit(NoTau); // tau Veto <--- dilepton killer
    if ( cut.passAllUpTo(NoTau) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoTau,e->weight());

    if(!cut.passAllUpTo(NoTau) )   return EVENT_NOT_USED;



    if(doTrigger && e->Bjets()>0 && e->NFatJets()>0){
        int trimu = 0;
        for(int k=0;k!=e->Nleps();k++) {
            Lepton *it = e->GetLepton(k);

            if(it->IsMuon() && it->Pt() > 27 && fabs(it->Eta()) <= 2.4) trimu++;
        }
        if(e->GetMet().Pt() > 100 && trimu>0){
            fatjetPlot(e,label,systname,"OneBOneFat",false);
            jetPlot(e, label, "", systname,"OneBOneFat");
        }
    }

    //if(doLep && e->Bjets()>0 && (e->Wjets()>0 || e->Topjets()>0)){
    if(doLep && e->Bjets()>0){

        int nummu = 0;
        int numel = 0;
        Lepton *oneele = NULL;
        Lepton *onemu = NULL;
        for(int k=0;k!=e->Nleps();k++) {
            Lepton *it = e->GetLepton(k);

            if(e->GetMet().Pt() > 100 and it->IsMuon() and fabs(it->Eta()) <= 2.4) Fill("ChargedHiggsTopBottom/OneBOneFat/muon_pt_"+label,systname, it->Pt() ,e->weight());
            if(e->GetMet().Pt() > 100 and it->IsElectron() and fabs(it->Eta()) <= 2.4) Fill("ChargedHiggsTopBottom/OneBOneFat/ele_pt_"+label,systname, it->Pt() ,e->weight());

            if(it->IsMuon() && it->Pt() > 10 && it->Pt() < 35 && fabs(it->Eta()) <= 2.4) {nummu++; onemu = it;}
            if(it->IsElectron() && it->Pt() > 10 && it->Pt() < 35 && fabs(it->Eta()) <= 2.4) {numel++; oneele = it;}
        }

        if(nummu + numel == 1) {
            if(numel==1 && oneele != NULL && oneele->Pt() > 20){
                //RECO
                if (not e->IsRealData() && oneele->Pt() > 25) { e->SetPtEtaSF("eleRECO",oneele->Pt(),oneele->Eta()); e->ApplySF("eleRECO"); }
                // ID-MVA
                if (not e->IsRealData()) { e->SetPtEtaSF("eleLoose",oneele->Pt(),oneele->Eta()); e->ApplySF("eleLoose"); }
                // miniIso
                if (not e->IsRealData()) { e->SetPtEtaSF("eleLooseIso",oneele->Pt(),oneele->Eta()); e->ApplySF("eleLooseIso"); }
            }
            if(nummu==1 && onemu != NULL){
                // RECO
                if (not e->IsRealData()) { e->SetPtEtaSF("muRECO",e->Npv(),0); e->ApplySF("muRECO"); }
                // ID
                if (not e->IsRealData() && onemu->Pt()>20) { e->SetPtEtaSF("muID",onemu->Pt(),fabs(onemu->Eta())); e->ApplySF("muID"); }
                // ISO
                if (not e->IsRealData() && onemu->Pt()>20) { e->SetPtEtaSF("muISO",onemu->Pt(),fabs(onemu->Eta())); e->ApplySF("muISO"); }
            }
        }


        if(e->GetMet().Pt() > 100 && nummu + numel == 1) {
            getCandidate(e,label,systname,"OneBOneFat",false);
            PlotAss(e,label,systname,"OneBOneFat");
            fatjetPlot(e,label,systname,"OneBOneFat",false);
            jetPlot(e, label, "", systname,"OneBOneFat");
            if(numel==1 && oneele != NULL){
                fatjetPlot(e,label,systname,"OneBOneFat_Ele",false);
                jetPlot(e, label, "", systname,"OneBOneFat_Ele");
                getCandidate(e,label,systname,"OneBOneFat_Ele",false);
                PlotAss(e,label,systname,"OneBOneFat_Ele");
            }
            if(nummu==1 && onemu != NULL){
                fatjetPlot(e,label,systname,"OneBOneFat_Mu",false);
                jetPlot(e, label, "", systname,"OneBOneFat_Mu");
                getCandidate(e,label,systname,"OneBOneFat_Mu",false);
                PlotAss(e,label,systname,"OneBOneFat_Mu");
            }
            
            if(doGentt) classifyHF(e,label,"",systname,"OneBOneFat","");
            /*
            if(e->Bjets() == 1 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneFat_one_lowj",false);
            else if(e->Bjets() == 1 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_one_highj",false);
            else if(e->Bjets() == 2 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneFat_two_lowj",false);
            else if(e->Bjets() == 2 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_two_highj",false);
            else if(e->Bjets() >= 3 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneFat_three_lowj",false);
            else if(e->Bjets() >= 3 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_three_highj",false);
            */
        }
    }

//    return EVENT_NOT_USED;

    if ( e->Nleps() == 0 ) cut.SetCutBit(NoLep); // kill Top/W/Z
    if ( cut.passAllUpTo(NoLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoLep,e->weight());

    if(!cut.passAllUpTo(NoLep) )   return EVENT_NOT_USED;


    ////////
    //// UP TO NOW: LEPTONS selection only
    ////

//Below if we want to test QCD 0b
//    if(!passtrigger) return EVENT_NOT_USED;
//    if(evt_HT<900) return EVENT_NOT_USED;    


    if(doZeroB && e->Bjets() == 0){ //control region
        jetPlot(e, label, "", systname,"NoB");
        fatjetPlot(e,label,systname,"NoB",false);
        if(e->Topjets() > 0) {jetPlot(e, label, "", systname,"NoBOneTop");fatjetPlot(e,label,systname,"NoBOneTop",false);}
        else if(e->Wjets()>0) {jetPlot(e, label, "", systname,"NoBOneW");fatjetPlot(e,label,systname,"NoBOneW",false);}
    }

//    if(e->Bjets() == 0) return EVENT_NOT_USED;

    string category="";



    bool Baseline=(e->Bjets() > 0);
    bool rightCombination =true;
    if ( not e->IsRealData() and ((label.find("HplusToTB") !=string::npos ) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) or (label.find("WprimeToTB") !=string::npos))){

        rightCombination=genInfoForSignal(e); // compute the right combination in the higgs case
//        Encorrection(e, systname, "Baseline");

    }


    jetPlot(e, label, category, systname,"Baseline");
//    fatjetPlot(e,label,systname,"Baseline",false);  
    
    if(doGenSig){
        if ( not e->IsRealData() and ((label.find("HplusToTB") !=string::npos ) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) or (label.find("QCD_HT") !=string::npos) or (label.find("WprimeToTB") !=string::npos)) and siglep == 0){
            classifyLabelGenEv(e,label,systname,"Baseline");
        }
    }


    if(doSig && e->GetMet().Pt() <= 100 && e->Bjets()>0 && (e->Wjets()>0 || e->Topjets()>0)) {

        // make candidate top+b, W+b+b,w+b+j
        getCandidate(e,label,systname,"OneBOneFat",false);
        PlotAss(e,label,systname,"OneBOneFat");
        fatjetPlot(e,label,systname,"OneBOneFat",false);
        jetPlot(e, label, "", systname,"OneBOneFat");


        if(evt_MH_t0b>0) jetPlot(e, label, "_t0b", systname, "OneBOneFat");
        else if(evt_MH_t1b>0) jetPlot(e, label, "_t1b", systname, "OneBOneFat");
        else if(evt_MH_Wbb>0) jetPlot(e, label, "_wbb", systname, "OneBOneFat");
        else if(evt_MH_wbj>0) jetPlot(e, label, "_wbj", systname, "OneBOneFat");

        if(doGentt) classifyHF(e,label,"",systname,"OneBOneFat","");

        if(e->Bjets() == 1 && num_otherj <= 2) {getCandidate(e,label,systname,"OneBOneFat_one_lowj",false); /*fatjetPlot(e,label,systname,"OneBOneFat_one",false);classifyHF(e,label,"",systname,"OneBOneFat_one","");*/}
        else if(e->Bjets() == 1 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_one_highj",false);
        else if(e->Bjets() == 2 && num_otherj <= 2) {getCandidate(e,label,systname,"OneBOneFat_two_lowj",false); /*fatjetPlot(e,label,systname,"OneBOneFat_two",false);classifyHF(e,label,"",systname,"OneBOneFat_two","");*/}
        else if(e->Bjets() == 2 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_two_highj",false);
        else if(e->Bjets() >= 3 && num_otherj <= 2) {getCandidate(e,label,systname,"OneBOneFat_three_lowj",false); /*fatjetPlot(e,label,systname,"OneBOneFat_three",false);classifyHF(e,label,"",systname,"OneBOneFat_three","");*/}
        else if(e->Bjets() >= 3 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneFat_three_highj",false);

        if(doGenSig){
            if ( not e->IsRealData() and ((label.find("HplusToTB") !=string::npos ) or (label.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") !=string::npos ) or (label.find("QCD_HT") !=string::npos) or (label.find("WprimeToTB") !=string::npos))){
                classifyLabelGenEv(e,label,systname,"OneBOneFat");
            }
        }
    }

    if(doMirror && e->GetMet().Pt() <= 100 && e->Bjets()>0 && (e->WjetsMirror()>0 || e->TopjetsMirror()>0)) {

        getCandidate(e,label,systname,"OneBOneMirrorFat",true);
        PlotAss(e,label,systname,"OneBOneMirrorFat");
        fatjetPlot(e,label,systname,"OneBOneMirrorFat",true);
        jetPlot(e, label, "", systname,"OneBOneMirrorFat");

        if(evt_MH_t0b>0) jetPlot(e, label, "_t0b", systname, "OneBOneMirrorFat");
        else if(evt_MH_t1b>0) jetPlot(e, label, "_t1b", systname, "OneBOneMirrorFat");
        else if(evt_MH_Wbb>0) jetPlot(e, label, "_wbb", systname, "OneBOneMirrorFat");
        else if(evt_MH_wbj>0) jetPlot(e, label, "_wbj", systname, "OneBOneMirrorFat");




/*        if(e->Bjets() == 1 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneMirrorFat_one_lowj",true);
        else if(e->Bjets() == 1 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneMirrorFat_one_highj",true);
        else if(e->Bjets() == 2 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneMirrorFat_two_lowj",true);
        else if(e->Bjets() == 2 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneMirrorFat_two_highj",true);
        else if(e->Bjets() >= 3 && num_otherj <= 2) getCandidate(e,label,systname,"OneBOneMirrorFat_three_lowj",true);
        else if(e->Bjets() >= 3 && num_otherj > 2) getCandidate(e,label,systname,"OneBOneMirrorFat_three_highj",true);  */
    }




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
