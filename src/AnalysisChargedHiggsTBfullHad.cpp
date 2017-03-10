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
    /// MARIA FIXME 2.4 --> 2.3
    t->SetEtaCut(2.4);
    t->SetMuRej(false);
    t->SetEleRej(false);
    t->SetTrackPtCut(-1.); // no requirement on the leading Track
    t->SetIsoRelCut(-1); // LooseCombinedIsolationDeltaBetaCorr3Hits
    t->SetIsoCut(2.5);
    t->SetProngsCut(-1); // all Prong
    t->SetDecayMode(1);  // 0=TauDecayModeFindingNewDMs 1=TauDecayModeFinding
}


void ChargedHiggsTopBottomFullHad::Init()
{

    Preselection();

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
    
     /// Vertices
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Met_"+l,"Met "+l+";MET [GeV]",50,0.,1000.);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Vertices_"+l,"Vertices "+l + ";Number of vertices",50,0.,50.);
    
    /////
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/NBjets_"+l,"NBjets "+l + ";Number of b jets P_{T}>40 ",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Njets_"+l,"Njets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Ncentraljets_"+l,"Ncentraljets "+l + ";Number of jets P_{T}>40 |#eta|<=2.4",10,0,10);
    Book("ChargedHiggsTopBottom/"+phasespace+category+"/Nforwardjets_"+l,"Nforwardjets "+l + ";Number of jets P_{T}>40 |#eta|>2.4",10,0,10);
    
}

void ChargedHiggsTopBottomFullHad::Preselection()
{

    for ( string l : AllLabel()  ) {

        BookCutFlow(l, "");
        BookHisto(l, "","Baseline"); // this is when there is nothing 

    }

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
    if((label.find("TTTo2L2Nu")!=string::npos) || (label.find("TTToSemilepton")!=string::npos) ||  (label.find("TT_TuneCUETP8M2T4")!=string::npos)  ) {

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

    if ( e->Nleps() == 0 ) cut.SetCutBit(NoLep); // only two lep <--- kill multiboson/ttV
    if ( cut.passAllUpTo(NoLep) ) Fill("ChargedHiggsTopBottom/CutFlow/CutFlow_"+label,systname,NoLep,e->weight());

    if(!cut.passAllUpTo(NoLep) )   return EVENT_NOT_USED;

    std::cout << " e->NFatJets() = " << e->NFatJets()  << std::endl;

    ////////
    //// UP TO NOW: LEPTONS selection only
    ////

    bool Baseline=(e->Bjets() > 0);

    computeVar(e);
    
    string category="";
    jetPlot(e, label, category, systname,"Baseline");

    return EVENT_NOT_USED;

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
