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

    for(int i=0; i!=min(e->NFatJets(),10);i++){
        SetTreeVar("fatjet_pt",i,e->GetFatJet(i)->Pt());
        SetTreeVar("fatjet_eta",i,e->GetFatJet(i)->Eta());
        SetTreeVar("fatjet_phi",i,e->GetFatJet(i)->Phi());
        SetTreeVar("fatjet_e",i,e->GetFatJet(i)->E());
        SetTreeVar("fatjet_tau1",i,e->GetFatJet(i)->Tau1());
        SetTreeVar("fatjet_tau2",i,e->GetFatJet(i)->Tau2());
        SetTreeVar("fatjet_tau3",i,e->GetFatJet(i)->Tau3());
        SetTreeVar("fatjet_SDMass",i,e->GetFatJet(i)->SDMass());
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

    Branch("tree_tb","fatjet_pt",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_eta",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_phi",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_e",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_tau1",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_tau2",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_tau3",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_SDMass",'d',10,"NFatJets");
    Branch("tree_tb","fatjet_CorrPrunedMass",'d',10,"NFatJets");

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


void ChargedHiggsTopBottomFullHad::BookHisto(string l, string category, string phasespace)
{

    cout <<"[ChargedHiggsTopBottom]::[Init]::[INFO] Boking Final Histo" <<l<<endl;

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

}

void ChargedHiggsTopBottomFullHad::Preselection()
{

    for ( string l : AllLabel()  ) {

        BookCutFlow(l, "");
        BookHisto(l, "","Baseline"); // this is when there is nothing 

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

        vector<pair<float,int> > valid; // csv, idx
        for(int i = 0 ; i<e->NcentralJets() ;++i)
            {
                valid.push_back(pair<float,int>(e->GetCentralJet(i)->bdiscr,i));
            }

        std::sort(valid.begin(), valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

        //////////

        if(!(Sregion.find("SR")!=string::npos)) {
            Fill("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+Sregion+LabelHF+label,systname, e->Bjets() ,e->weight());
            if(e->NcentralJets()>=3 && valid.size() != 0) Fill("ChargedHiggsTopBottom/"+phasespace+category+"/thirdBDiscr_"+Sregion+LabelHF+label,systname, valid[2].first ,e->weight());

        }

        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());
        Fill("ChargedHiggsTopBottom/"+phasespace+category+"/HT_zoom_"+Sregion+LabelHF+label,systname, evt_HT ,e->weight());

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
    if(evt_HT<800) return EVENT_NOT_USED;

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


    if(e->Wjets()>0 || e->Topjets()>0) {

        cout << "=======================================" << endl;
        cout << "N(W) jets = " << e->Wjets() << endl;
        cout << "N(TOP) jets = " << e->Topjets() << endl;

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
