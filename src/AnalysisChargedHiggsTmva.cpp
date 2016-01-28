#include "interface/AnalysisChargedHiggsTmva.hpp"

void TmvaAnalysis::AddVariable( string name, char type){ 
    cout<<"[TmvaAnalysis]::[AddVariable]::[INFO] Adding variable: '"<<name<<"'"<<endl;
    varValues_.Add(name,type); 
    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    //else if ( type == 'D') for(auto &r : readers_) r -> AddVariable(name.c_str(),  (double*)varValues_.GetPointer(name));
    else { 
        cout <<"[TmvaAnalysis]::[AddVariable]::[ERROR] type '"<<type<<"' not supported"<<endl;
    }
}//end add variable


void TmvaAnalysis::Init(){
    cout<<"[TmvaAnalysis]::[Init]::[INFO] Book histos"<<endl;
    for ( string& l : AllLabel()  ) {
        Book( "ChargedHiggsTmva/Vars/Mt_"+l, "Mt_"+l, 2000,0,2000.);
        for( int bdtIdx=0; bdtIdx < int(weights.size()) ; ++bdtIdx)
        {
            Book(Form("ChargedHiggsTmva/Vars/Bdt%d_",bdtIdx)+l, "BDT_"+l, 200,-1.,1.);
            Book2D(Form("ChargedHiggsTmva/Vars/MtVsBdt%d_",bdtIdx)+l, "BDT_"+l, 2000,0,2000,200,-1.,1.);
        }

        // BDT Correlations
        Book2D("ChargedHiggsTmva/Vars/Bdt0Bdt1"+l, "BDT0_BDT1"+l, 200,-1.,1.,200,-1.,1.);
        Book2D("ChargedHiggsTmva/Vars/Bdt0Bdt2"+l, "BDT0_BDT2"+l, 200,-1.,1.,200,-1.,1.);
        Book2D("ChargedHiggsTmva/Vars/Bdt1Bdt3"+l, "BDT1_BDT3"+l, 200,-1.,1.,200,-1.,1.);
        Book2D("ChargedHiggsTmva/Vars/Bdt2Bdt3"+l, "BDT2_BDT3"+l, 200,-1.,1.,200,-1.,1.);
    }

    TMVA::Tools::Instance();

    cout <<"[TmvaAnalysis]::[Init]::[INFO] Init Reader"<<endl;
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    // --- variables, local store and to the reader
    AddVariable("NJets",'F');    // Int* -> this function is deprecated, please provide all variables to the reader as floats
    AddVariable("NCJets",'F');    
    AddVariable("BJets",'F');    
    AddVariable("pTj1",'F');
    AddVariable("pTb1",'F');
    AddVariable("etat1",'F');
    AddVariable("phimet",'F');
    AddVariable("phit1",'F');
    AddVariable("ht",'F');
    // -- Angular Variables
    AddVariable("rbb",'F');
    AddVariable("rcoll",'F');
    AddVariable("rsr",'F');
    AddVariable("pTt1oMet",'F');
    //AddVariable("DPhiEtMissJet1",'F' );
    //AddVariable("DPhiEtMissTau",'F');

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
    {
        cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
        readers_[i]->BookMVA("BDT",weights[i].c_str());
    }
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;
    
    // ------------------------ CATEGORIES ----------------- ; 
    // configurable ? TODO
    categories.AddVariable("bdt0");
    categories.AddVariable("bdt1");

    // -0.25 and bdt[1] > -.4
    categories.SetBoundary("bdt0",0, -.25);
    categories.SetBoundary("bdt1",0, -.4);
    categories.SetBoundary("bdt0",1, -0.8);
    categories.SetBoundary("bdt0",1, -0.9);

    for(int icat=0;icat< categories.GetNcat() ;++icat)
    for ( string& l : AllLabel()  ) {
        Book( string("ChargedHiggsTmva/Vars/Mt_") + Form("cat%d_",icat) +l, "Mt_"+l, 2000,0,2000.);
        Book2D( string("ChargedHiggsTmva/Vars/Bdt0Bdt1") + Form("cat%d_",icat) + l, "BDT0_BDT1"+l, 200,-1.,1.,200,-1.,1.);
    }

} // end init

void TmvaAnalysis::End(){
    //delete reader_;
}

int TmvaAnalysis::analyze(Event*e,string systname){

    string label = GetLabel(e);

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;
   
    // MET 
    //if ( e->IsRealData() and not e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v") )  {

    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();

    // ALIGN with TRIGGER
    if ( e->GetMet().Pt() < 130 ) return 0;
    if ( t1->Pt() <51 or fabs(t1->Eta() )  >=2.1 ) return 0;

    SetVariable("NJets",e->Njets());
    SetVariable("NCJets",e->NcentralJets());
    SetVariable("BJets",e->Bjets());
    SetVariable("etat1",t1->Eta());
    SetVariable("phit1",t1->Phi());
    SetVariable("phimet",e->GetMet().Phi());
    SetVariable("ht",e->Ht());
    if (j1 != NULL ) SetVariable("pTj1",j1->Pt());
    else  SetVariable("pTj1",0);
    if (bj1 != NULL ) SetVariable("pTb1",bj1->Pt());
    else  SetVariable("pTb1",0);
    double DPhiEtMissJet1=-1 ; if (e->GetJet(0) != NULL ) DPhiEtMissJet1 = fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(0))->Phi()));
    double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),t1->Phi()));

    SetVariable("rbb",e->RbbMin());
    SetVariable("rcoll",e->RCollMin());
    SetVariable("rsr",e->RsrMax());
    SetVariable("pTt1oMet", t1->Pt() / e->GetMet().Pt() ) ;
    //SetVariable("DPhiEtMissJet1",DPhiEtMissJet1);
    //SetVariable("DPhiEtMissTau",DPhiEtMissTau);

    vector<float> bdt;
    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
        bdt.push_back(readers_[i]->EvaluateMVA("BDT") );

        Fill( Form("ChargedHiggsTmva/Vars/Bdt%d_",i)+label,systname,bdt[i] ,e->weight() ) ;
        Fill2D(Form("ChargedHiggsTmva/Vars/MtVsBdt%d_",i)+label,systname,e->Mt(),bdt[i],e->weight() ) ;
    }

    // FILL2D
    {
        if(bdt.size() > 1) Fill2D("ChargedHiggsTmva/Vars/Bdt0Bdt1"+label,systname,bdt[0],bdt[1],e->weight());
        if(bdt.size() > 2) Fill2D("ChargedHiggsTmva/Vars/Bdt0Bdt2"+label,systname,bdt[0],bdt[2],e->weight());
        if(bdt.size() > 3) Fill2D("ChargedHiggsTmva/Vars/Bdt1Bdt3"+label,systname,bdt[1],bdt[3],e->weight());
        if(bdt.size() > 3) Fill2D("ChargedHiggsTmva/Vars/Bdt2Bdt3"+label,systname,bdt[2],bdt[3],e->weight());
    
    }

    if( bdt.size() >2){
        if (bdt[0]> -0.25 and bdt[1] > -.4) { // check -- BDT 0 = QCD 200 GeV, BDT 1 = TTJets 200 GeV
            if( Unblind(e) )Fill("ChargedHiggsTmva/Vars/Mt_"+label,systname,e->Mt(), e->weight() ) ;
        }
    }

    // ----------- MULTI CATEGORY ANALYSIS ------
    categories.SetVariable("bdt0",bdt[0]);
    categories.SetVariable("bdt1",bdt[1]);
    int mycat=categories.computeCat();
    if(mycat >=0 )
    {
        if(Unblind(e))Fill(string("ChargedHiggsTmva/Vars/Mt_") + Form("cat%d_",mycat) + label , systname,e->Mt(), e->weight() ) ; 
        Fill2D( string("ChargedHiggsTmva/Vars/Bdt0Bdt1") + Form("cat%d_",mycat)+label,systname,bdt[0],bdt[1],e->weight());
    }
    // -----------------------------------------
    return 0; // 

} // end analyze

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
