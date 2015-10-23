#include "interface/AnalysisChargedHiggsTmva.hpp"

void TmvaAnalysis::AddVariable( string name, char type){ 
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
        for( int bdtIdx=0; bdtIdx < int(weights.size()) ; ++bdtIdx)
        {
            Book(Form("ChargedHiggsTmva/Vars/Bdt%d_",bdtIdx)+l, "BDT_"+l, 1000,-10.,10.);
            Book2D(Form("ChargedHiggsTmva/Vars/Bdt%dVsMt_",bdtIdx)+l, "BDT_"+l, 1000,-10.,10.,2000,0,2000);
        }
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
    //AddVariable("DPhiEtMissJet1",'F' );
    //AddVariable("DPhiEtMissTau",'F');

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
    {
        cout <<"[TmvaAnalysis]::[Init]::[INFO] Loading weights idx="<<i<<": '"<< weights[i]<<"'"<<endl;
        readers_[i]->BookMVA("BDT",weights[i].c_str());
    }
    cout <<"[TmvaAnalysis]::[Init]::[INFO] Done"<<endl;

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
    //SetVariable("DPhiEtMissJet1",DPhiEtMissJet1);
    //SetVariable("DPhiEtMissTau",DPhiEtMissTau);

    vector<float> bdt;
    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
        bdt.push_back(readers_[i]->EvaluateMVA("BDT") );

        Fill( Form("ChargedHiggsTmva/Vars/Bdt%d_",i)+label,systname,bdt[i] ,e->weight() ) ;
        Fill2D(Form("ChargedHiggsTmva/Vars/Bdt%dVsMt_",i)+label,systname,bdt[i], e->Mt(),e->weight() ) ;
    }

    return 0; // for the moment FIXME

} // end analyze

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
