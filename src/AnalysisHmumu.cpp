#include "interface/AnalysisHmumu.hpp"

#define VERBOSE 0

//#warning Hmumu ANALYSIS NON ISO
void HmumuAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10); 
    l->SetIsoRelCut(0.25);
    l->SetEtaCut(2.4);
    //l->SetTightCut(true);
    //l->SetMediumCut(false);
    l->SetTightCut(false);
    l->SetLooseCut(false);
    l->SetMediumCut(true);
}
void HmumuAnalysis::SetJetCuts(Jet *j) { 
    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
    j->SetPuIdCut(-999);
}

void HmumuAnalysis::SetTauCuts(Tau *t){ 
    t->SetIsoCut(2.5); 
    t->SetEtaCut(2.1); 
    t->SetPtCut(8000);  // remove taus!!! 
    t->SetMuRej(true); 
    t->SetEleRej(true);
    t->SetTrackPtCut(30.);
    t->SetProngsCut(1); 
    t->SetDecayMode(1);
}

void HmumuAnalysis::SetPhotonCuts(Photon *p){
    p->SetIsoCut(-1); 
    p->SetPtCut(8000);
}

string HmumuAnalysis::Category(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start Category");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   
   // construct a Higgs-like object  
    Object Hmm;
    Hmm += *mu0;
    Hmm += *mu1;

    if (doSync){
        int cat=-100;
        // set, in case exit before
        SetTreeVar("cat",cat);
        SetTreeVar("pt1",mu0->Pt());
        SetTreeVar("pt2",mu1->Pt());
        SetTreeVar("eta1",mu0->Eta());
        SetTreeVar("eta2",mu1->Eta());
        SetTreeVar("phi1",mu0->Phi());
        SetTreeVar("phi2",mu1->Phi());
        SetTreeVar("njets",jets.size());
        SetTreeVar("mass",Hmm.M());
        SetTreeVar("Hpt" ,Hmm.Pt());
        for(int ijet=0;ijet<jets.size();++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,jets[ijet]->Pt());
            SetTreeVar("jet_eta",ijet,jets[ijet]->Eta());
            SetTreeVar("jet_phi",ijet,jets[ijet]->Phi());
        }
    }

    string muStr ="";
    if ( fabs(eta1) < 0.8 and fabs(eta0) <0.8) muStr = "BB";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <0.8) muStr =  "BO";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <1.6) muStr =  "BO";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <0.8) muStr =  "BE";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <2.4) muStr =  "BE";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <1.6) muStr =  "OO";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <2.4) muStr =  "OE";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <1.6) muStr =  "OE";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <2.4) muStr =  "EE";
    else return "";

    // vbf -- fully combinatorics
    bool isTightVbf=false;
    bool isTightGF=false;
    bool isVbf=false;

    for(unsigned i=0;i<jets.size() ;++i)
    for (unsigned j=i+1;j<jets.size();++j)
    {
        // vbf preselection. jets are pt-sorted
        if (jets[i]->Pt() >40 and jets[j]->Pt() >30) isVbf=true;
        else continue;

        double mjj= jets[i]->InvMass( jets[j] ) ;
        double detajj= fabs( jets[i]->Eta() - jets[j]->Eta() );
        if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Jet Eta (i=%d,j=%d) etai=%f etaj=%f | Pt_i=%f Pt_j=%f Pz_i=%f Pz_j=%f",i,j,jets[i]->Eta(),jets[j]->Eta(),jets[i]->Pt(),jets[j]->Pt(),jets[i]->GetP4().Pz(),jets[j]->GetP4().Pz()));

        if (mjj > 650 and detajj > 3.5)  isTightVbf = true;
        if (mjj > 250 and Hmm.Pt() >50 )  isTightGF = true;
    }

    int nbjets=0;
    for(unsigned i=0;i<jets.size() ;++i)
    {
        if (jets[i]->IsBJet() and jets[i]->Pt() >30 and abs(jets[i]->Eta())<2.4)  nbjets +=1;
    }

    string vbfStr="";
    if (nbjets >0 ) vbfStr="OneB";
    else if (isTightVbf ) vbfStr = "VBF0";
    else if (isTightGF) vbfStr = "GF";
    else if (isVbf) vbfStr = "VBF1";
    else if (Hmm.Pt() >25) vbfStr = "Untag0";
    else vbfStr = "Untag1";

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + vbfStr + "_" + muStr);

    if (doSync){
        int cat=-100;
        // derive cat number -- hardcoded. should use HmmConfig :(
        if (vbfStr =="VBF0")        cat=0;
        else if (vbfStr =="OneB")   cat=1;
        else if (vbfStr =="GF")     cat=2;
        else if (vbfStr =="VBF1")   cat=3;
        else if (vbfStr =="Untag0") cat=4;
        else if (vbfStr =="Untag1") cat=5;

        if (muStr=="BB")      cat += 0*6;
        else if (muStr=="BO") cat += 1*6;
        else if (muStr=="BE") cat += 2*6;
        else if (muStr=="OO") cat += 3*6;
        else if (muStr=="OE") cat += 4*6;
        else if (muStr=="EE") cat += 5*6;

        SetTreeVar("cat",cat);
        SetTreeVar("nbjets",nbjets);
    }

    return vbfStr +"_" + muStr;
}

void HmumuAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");
    // define categories -- for booking histos
    vector< string> mu_cats{"BB","BO","BE","OO","OE","EE"};
    vector<string> vbf_cats{"VBF0","GF","VBF1","OneB","Untag0","Untag1"};

    for( const auto & m : mu_cats)
    for( const auto & v : vbf_cats)
    {
        categories_.push_back(v + "_" + m);
    }
    //

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    // 
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
        for(const auto & c : categories_)
        {
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 960,60,300); // every 4 (old16) per GeV
            // for systematics, only counts the total
	        Book ("HmumuAnalysis/Vars/Mmm_Count_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 1,105,150); // every 4 (old16) per GeV
            AddFinalHisto("HmumuAnalysis/Vars/Mmm_Count_"+c+"_"+l);
        }
    }

    if (doSync){
        InitTree("hmm");
        Branch("hmm","cat",'I');
        Branch("hmm","eventNum",'I');
        Branch("hmm","runNum",'I');
        Branch("hmm","lumiNum",'I');
        Branch("hmm","pt1",'F');
        Branch("hmm","pt2",'F');
        Branch("hmm","eta1",'F');
        Branch("hmm","eta2",'F');
        Branch("hmm","phi1",'F');
        Branch("hmm","phi2",'F');
        Branch("hmm","mass",'F');
        Branch("hmm","Hpt",'F');
        Branch("hmm","njets",'I');
        Branch("hmm","nbjets",'I');
        Branch("hmm","jet_pt",'d',20,"njets");
        Branch("hmm","jet_eta",'d',20,"njets");
        Branch("hmm","jet_phi",'d',20,"njets");
        Branch("hmm","pass_recomuons",'I');
        Branch("hmm","pass_asymmcuts",'I');
        Branch("hmm","pass_trigger",'I');
        Branch("hmm","pass_leptonveto",'I');
        Branch("hmm","pass_all",'I');
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

int HmumuAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    /*
     */

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon0: ");
    Lepton*mu0 = e->GetMuon(0);
    if (VERBOSE and mu0)Log(__FUNCTION__,"DEBUG",Form("GetMuon0: pt=%f",mu0->Pt()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon1: ");
    Lepton*mu1 = e->GetMuon(1);
    int il2=1;
    while (mu0 != NULL and mu1!= NULL and mu1->Charge() * mu0->Charge() != -1) { mu1= e->GetMuon(++il2);} // fully combinatorics on the second muon
    if (VERBOSE and mu1)Log(__FUNCTION__,"DEBUG",Form("GetMuon1: pt=%f",mu1->Pt()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetJets: ");

    vector<Jet*> selectedJets;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        selectedJets.push_back(e->GetJet(i));
    }

    string category = Category(mu0, mu1, selectedJets);
    e->ApplyBTagSF(1); //0 loose, 1 medium, 2 tight

    //// Truth
    //GenParticle *genmu0=NULL; GenParticle *genmu1=NULL;

    //for( int iGen=0 ; /*empty*/ ; ++iGen)
    //{
    //    GenParticle *g = e->GetGenParticle(iGen); 
    //    if (g==NULL ) break;  // end loop statement
    //    if (not g->IsDressed()) continue;
    //    if (not abs(g->GetPdgId())==13) continue;
    //    
    //    if (genmu0== NULL) genmu0=g;
    //    else if (genmu1==NULL) {genmu1=g; break;}
    //}

    //bool genMuons = genmu0!=NULL and genmu1 !=NULL;

    //bool isGen=false;
    //bool isReco=false;

    //if (genMuons) // no requirement on pT
    //{
    //    Object Ztruth(*genmu0); 
    //    Ztruth += *genmu1;
    //    if (Ztruth.M() > 60 and Ztruth.M()<120) isGen=true;
    //}
    //
    // preselection
    bool recoMuons= mu0 != NULL and mu1 !=NULL; 
    if (recoMuons and mu0->Charge() * mu1->Charge() != -1 ) recoMuons=false; // 


    bool passAsymmPtCuts = (recoMuons and  mu0->Pt() >26 );

    // Trigger
    bool passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); //

    bool passLeptonVeto= true;
    if (e->GetMuon(2) != NULL) passLeptonVeto=false;
    for(int i=0; ; ++i) { 
            Lepton *el= e->GetElectron(i);
            if (el == NULL) break;
            //if (el->Pt() >15) passLeptonVeto=false; // FIXME 10 ?!?
            #warning ABSURD_ELE_VETO
            //|eta| < 1.4442 || 1.566 <|eta| <2.5 
            if (el->Pt() >10 and (fabs(el->Eta()) <1.4442 or fabs(el->Eta())>1.566) and fabs(el->Eta())<2.5 ) passLeptonVeto=false; // FIXME 10 ?!?
    }


    if (doSync) {
        SetTreeVar("eventNum",e->eventNum());
        SetTreeVar("runNum",e->runNum());
        SetTreeVar("lumiNum",e->lumiNum());
        SetTreeVar("pass_recomuons",recoMuons);
        SetTreeVar("pass_asymmcuts",passAsymmPtCuts);
        SetTreeVar("pass_trigger",passTrigger);
        SetTreeVar("pass_leptonveto",passLeptonVeto);
        if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto and category != "" ) SetTreeVar("pass_all",1);
        else SetTreeVar("pass_all",0); 
        FillTree("hmm");
    }


    // ---------------------- 25 -------------------
    if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto)
    {
        cut.SetCutBit(Leptons);
        if (passTrigger) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *mu1;
        mass_=Z.M();

       	//if (cut.passAllUpTo(Trigger) ){
        //}
        if (VERBOSE)Log(__FUNCTION__,"DEBUG","event pass selection");
        if(Unblind(e))Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, mass_,e->weight()) ;
        if(Unblind(e) and category != "")Fill("HmumuAnalysis/Vars/Mmm_"+ category+"_"+ label,systname, mass_,e->weight()) ;
    }


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
