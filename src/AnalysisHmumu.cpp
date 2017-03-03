#include "interface/AnalysisHmumu.hpp"

//#warning Hmumu ANALYSIS NON ISO
void HmumuAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.20);
    l->SetEtaCut(2.5);
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

string HmumuAnalysis::Category(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets){
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();
   
   // construct a Higgs-like object  
    Object Hmm;
    Hmm += *mu0;
    Hmm += *mu1;

    string muStr ="";
    if ( fabs(eta1) < 0.8 and fabs(eta0) <0.8) muStr = "BB";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <0.8) muStr =  "BO";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <1.6) muStr =  "BO";
    else if (fabs(eta1) < 2.1 and fabs(eta0) <0.8) muStr =  "BE";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <2.1) muStr =  "BE";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <1.6) muStr =  "OO";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <2.1) muStr =  "OE";
    else if (fabs(eta1) < 2.1 and fabs(eta0) <1.6) muStr =  "OE";
    else if (fabs(eta1) < 2.1 and fabs(eta0) <2.1) muStr =  "EE";
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

        if (mjj > 650 and detajj > 3.5)  isTightVbf = true;
        if (mjj > 250 and Hmm.Pt() >50 )  isTightGF = true;
    }

    int nbjets=0;
    for(unsigned i=0;i<jets.size() ;++i)
    {
        if (jets[i]->IsBJet() and jets[i]->Pt() >30 and abs(jets[i]->Eta())<2.5)  nbjets +=1;
    }

    string vbfStr="";
    if (isTightVbf ) vbfStr = "VBF0";
    else if (nbjets >0 ) vbfStr="OneB";
    else if (isTightGF) vbfStr = "GF";
    else if (isVbf) vbfStr = "VBF1";
    else vbfStr = "Untag";

    return vbfStr +"_" + muStr;
}

void HmumuAnalysis::Init(){
    // define categories -- for booking histos
    vector< string> mu_cats{"BB","BO","BE","OO","OE","EE"};
    vector<string> vbf_cats{"VBF0","GF","VBF1","OneB","Untag"};

    for( const auto & m : mu_cats)
    for( const auto & v : vbf_cats)
    {
        categories_.push_back(v + "_" + m);
    }
    //

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    // 
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
        for(const auto & c : categories_)
        {
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 1440,60,150);
        }
    }


}

int HmumuAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    vector<Jet*> selectedJets;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        selectedJets.push_back(e->GetJet(i));
    }

    string category = Category(mu0, mu1, selectedJets);
    e->ApplyBTagSF(1); //0 loose, 1 medium, 2 tight

    // Truth
    GenParticle *genmu0=NULL; GenParticle *genmu1=NULL;

    for( int iGen=0 ; /*empty*/ ; ++iGen)
    {
        GenParticle *g = e->GetGenParticle(iGen); 
        if (g==NULL ) break;  // end loop statement
        if (not g->IsDressed()) continue;
        if (not abs(g->GetPdgId())==13) continue;
        
        if (genmu0== NULL) genmu0=g;
        else if (genmu1==NULL) {genmu1=g; break;}
    }

    bool genMuons = genmu0!=NULL and genmu1 !=NULL;

    bool isGen=false;
    bool isReco=false;

    if (genMuons) // no requirement on pT
    {
        Object Ztruth(*genmu0); 
        Ztruth += *genmu1;
        if (Ztruth.M() > 60 and Ztruth.M()<120) isGen=true;
    }

    bool recoMuons= mu0 != NULL and mu1 !=NULL; 

    // ---------------------- 25 -------------------
    if ( recoMuons)
    {
        cut.SetCutBit(Leptons);
        if (e->IsTriggered("HLT_IsoMu24_v") ) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *mu1;
        mass_=Z.M();

       	if (cut.passAllUpTo(Trigger) ){
            if(Unblind(e))Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, mass_,e->weight()) ;
            if(Unblind(e) and category != "")Fill("HmumuAnalysis/Vars/Mmm_"+ category+"_"+ label,systname, mass_,e->weight()) ;
        }
    }


    return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
