#include "interface/AnalysisChargedHiggsQCDPurity.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/CutSelector.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include <string>
#include <algorithm>


//#define VERBOSE 1
#define DIRECT_SEL true
#define INVERSE_SEL false

#define FULL_SEL true
#define THREEJETS_SEL false

void ChargedHiggsQCDPurity::Init()
{
    // -- simplified selection
    for ( string& l : AllLabel()  ) 
        for (size_t eBin = -1 ; eBin + 1 < EtaBins.size() ; ++eBin )
        for (size_t iBin = -1 ; iBin + 1 < PtBins.size() ; ++iBin )
        {
            float eta = -1;
            float pt = -1;
            if (iBin>=0 ) pt= PtBins[iBin];
            if (eBin>=0 ) eta= EtaBins[eBin];
            //                       direct, fullSel
            Book( dir + HistName(pt,eta, DIRECT_SEL , THREEJETS_SEL)+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            // ---- 
            Book( dir + HistName(pt,eta, DIRECT_SEL , THREEJETS_SEL, "Upar")+"_"+ l  , ("EtMissParallel "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL, "Upar")+"_"+ l  , ("EtMissParallelIsoInv "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt,eta, DIRECT_SEL , THREEJETS_SEL, "Uperp")+"_"+ l  , ("EtMissPerp "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL, "Uperp")+"_"+ l  , ("EtMissPerpIsoInv "+ l).c_str(),250,0.,500.);
            //  Used in case of spline reweighting iterations
            Book( dir + HistName(pt,eta, DIRECT_SEL,  THREEJETS_SEL, "TauPt")+"_"+ l  , ("PtTau "+ l).c_str(),1000,0.,1000.);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL, "TauPt")+"_"+ l  , ("PtTauIsoInv "+ l).c_str(),1000,0.,1000.);

            // Prong
            Book( dir + HistName(pt,eta, DIRECT_SEL , THREEJETS_SEL)+"_1p_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL)+"_1p_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            Book( dir + HistName(pt,eta, DIRECT_SEL , THREEJETS_SEL)+"_3p_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL)+"_3p_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
        }

    // --- for event NOT BINNED! CONTROL PLOTS
    for (string &l : AllLabel() )
    {
            //  Used for control plots
            string hist= "TauPt_IsoInv_Control";
            Book( dir + hist+"_"+ l  , ("PtTauIsoInv "+ l).c_str(),1000,0.,1000.); // this guys has the R factor applied in the loos selection
            hist= "TauPt_Control";
            Book( dir + hist+"_"+ l  , ("PtTau "+ l).c_str(),1000,0.,1000.); // this guys has the R factor applied in the loos selection
            hist= "TauEta_IsoInv_Control";
            Book( dir + hist+"_"+ l  , ("EtaTauIsoInv "+ l).c_str(),100,-2.5,2.5); 
            //
            hist= "TauEta_Control";
            Book( dir + hist+"_"+ l  , ("EtaTau "+ l).c_str(),100,-2.5,2.5); 
            //
            hist= "EtMiss_IsoInv_Control";
            Book( dir + hist+"_"+ l  , ("EtMissTauIsoInv "+ l).c_str(),250,0,500);
            hist= "EtMiss_Control";
            Book( dir + hist+"_"+ l  , ("EtMiss "+ l).c_str(),250,0,500);

            hist= "DrTauJet_Control";
            Book( dir + hist+"_"+ l  , ("EtMiss "+ l).c_str(),1000,0,5.0);
    }

    // -- full selection
    for ( string& l : AllLabel()  ) 
    {
        for (size_t eBin = -1 ; eBin + 1 < EtaBins.size() ; ++eBin )
        for (size_t iBin = -1 ; iBin + 1 < PtBins.size() ; ++iBin )
        {
            float eta = -1;
            float pt = -1;
            if (iBin>=0 ) pt= PtBins[iBin];
            if (eBin>=0 ) eta= EtaBins[eBin];

            //                       direct full
            Book( dir + HistName(pt,eta, DIRECT_SEL , FULL_SEL)+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, FULL_SEL)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt,eta, DIRECT_SEL , FULL_SEL)+"_1p_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, FULL_SEL)+"_1p_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            Book( dir + HistName(pt,eta, DIRECT_SEL , FULL_SEL)+"_3p_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, FULL_SEL)+"_3p_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            // ---  NoR
            Book( dir + HistName(pt,eta, INVERSE_SEL, FULL_SEL)+"_NoR_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            //
            Book( dir + HistName(pt,eta, DIRECT_SEL , FULL_SEL, "Upar")+"_"+ l  , ("EtMissParallel "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, DIRECT_SEL, FULL_SEL, "Uperp")+"_"+ l  , ("EtMissPerp "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt,eta, INVERSE_SEL , FULL_SEL, "Upar")+"_"+ l  , ("EtMissParallelIsoInv "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt,eta, INVERSE_SEL, FULL_SEL, "Uperp")+"_"+ l  , ("EtMissPerpIsoInv "+ l).c_str(),250,0.,500.);

        }

        // I don't need to split it by pt
        Book( dir + "Mt"+"_"+ l  , ("Mt "+ l).c_str(),8000,0.,8000); // same binning in TauNu
        Book( dir + "MtIsoInv"+"_"+ l  , ("MtIsoInv "+ l).c_str(),8000,0.,8000.);
        Book( none + "EtMissIsoInv"+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),1000,0.,1000.);
        Book( none + "EtMiss"+"_"+ l  , ("EtMiss "+ l).c_str(),1000,0.,1000.); // copy of the Tau Nu ? 
        Book( dir +"DrTauJet_"+ l  , ("#DeltaR(#tau,j) "+ l).c_str(),1000,0,5.0);
        Book( dir +"DrTauJetIsoInv_"+ l  , ("#DeltaR(#tau,j) "+ l).c_str(),1000,0,5.0);

        Book(    none+"RbbMin_"+l,"RbbMin "+l+";R_{bb}^{min}",100,0,2*TMath::Pi());
        Book(    none+"RCollMin_"+l,"RCollMin "+l+";R_{coll}^{min}",100,0,2*TMath::Pi());

        Book(    none+"RbbMinIsoInv_"+l,"RbbMin IsoInv "+l+";R_{bb}^{min}",100,0,2*TMath::Pi());
        Book(    none+"RCollMinIsoInv_"+l,"RCollMin IsoInv "+l+";R_{coll}^{min}",100,0,2*TMath::Pi());
        // study categorization
        for(int i=0;i<=1;++i)
        {
            Book( dir + "Mt_"+Form("cat%d",i)+"_"+ l  , ("Mt "+ l).c_str(),8000,0.,8000); // same binning in TauNu
            Book( dir + "MtIsoInv_"+Form("cat%d",i)+"_"+ l  , ("MtIsoInv "+ l).c_str(),8000,0.,8000.);
        }
    }

}

int ChargedHiggsQCDPurity::analyze(Event*e,string systname)
{
    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] start analyze event"<<endl;
    #endif
    string label = GetLabel(e);
    // do not distinguish between data and mc, 
    // so we can run closures
    // Fill Real Iso 
    Tau *t = e->GetTau(0);
    // Fill Inverted Iso
    Tau *tInv = e->GetTauInvIso(0);

    e->ApplyTopReweight();

    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is Tau? "<< (t==NULL) <<endl;
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is TauInv? "<< (tInv == NULL)<<endl;
    #endif

    // --- take the selection from the tau nu analysis
    CutSelector direct; 
        direct.SetMask(ChargedHiggsTauNu::MaxCut-1);
        direct.SetCut(Selection(e,true,false));
    CutSelector inverse;
        inverse.SetMask(ChargedHiggsTauNu::MaxCut-1);
        inverse.SetCut(Selection(e,false,false));

    // TODO:
    // * what do I do with event with a Tau and an Inv tau? -> DY ? 
    // * put a limit on the TauInv sideband ? 3.0 -20 GeV

   bool passDirectLoose=direct.passAllUpTo(ChargedHiggsTauNu::ThreeJets);
   bool passInverseLoose=inverse.passAllUpTo(ChargedHiggsTauNu::ThreeJets);

    
    //bool passDirectLoose=direct.passAllUpTo(ChargedHiggsTauNu::OneBjet);
    //bool passInverseLoose=inverse.passAllUpTo(ChargedHiggsTauNu::OneBjet);
    //#warning QCD BJets
    //LogN(__FUNCTION__,"WARNING","ONE B REQUIRED FOR QCD LOOSE",10);
    
    
    // check minimal selection: Three bjets
    if ( not passDirectLoose
         and not  passInverseLoose
       ) return EVENT_NOT_USED;


    //  USE PRESCALE PATH ONLY FOR THE "inclusive/Loose" selection
    bool passPrescale=false;
    if (not e->IsRealData()) passPrescale=true;
    //if (  e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v") ) passPrescale=true;

    //#warning MET80 TRigger in QCD
    //if (  e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80") ) passPrescale=true;
    //LogN(__FUNCTION__,"WARNING","MET 80 in QCD Loose",10);
    //
    //bool passMatchDirect=true;
    //bool passMatchInverse=true;
    
    #warning Tau Match
    bool passMatchDirect=false;
    bool passMatchInverse=false;
    if (e->IsRealData()) { passMatchDirect=true; passMatchInverse=true;}
    else {  
        if (e->GetTau(0) !=NULL and e->GetTau(0)->Rematch(e)==15) passMatchDirect=true;
        if (e->GetTauInvIso(0) !=NULL and e->GetTauInvIso(0)->Rematch(e)==15) passMatchInverse=true;
    }

    //--------------

    if (t != NULL and passMatchDirect and passDirectLoose and passPrescale) // direct
    {
        float pt = t->Pt();
        float eta = t->Eta();
        int flavor= t->Rematch(e);

        if (pt  > 8000 or pt <0 )  Log(__FUNCTION__,"INFO",Form("strange event : tau Pt=%.0f",pt));

        string hist = HistName(pt,eta,DIRECT_SEL, THREEJETS_SEL);
        Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,eta,DIRECT_SEL, THREEJETS_SEL,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
        hist = HistName(pt,eta,DIRECT_SEL, THREEJETS_SEL,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );
        hist = HistName(pt,eta, DIRECT_SEL, THREEJETS_SEL, "TauPt") ;
        Fill( dir+hist +"_"+label,systname, t->Pt(), e->weight() );

        hist = HistName(pt,eta, DIRECT_SEL, THREEJETS_SEL) ;
        if (t->GetNProng()==1) hist +="_1p";
        else hist+="_3p";
        Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );

        { // CONTROL PLOTS -- LOOSE SELECTION
            string hist= "TauPt_Control";
            Fill( dir+hist +"_"+label,systname, t->Pt(), e->weight() );
            hist= "EtMiss_Control";
            Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );

            float dr=100.; for(int i=0;i<e->Njets(); ++i) { dr = std::min(dr, e->GetJet(i)->DeltaR(t)) ;}
            hist= "DrTauJet_Control";
            Fill( dir+hist +"_"+label,systname, dr, e->weight() );
        }

    }

    if (tInv != NULL and passMatchInverse and passInverseLoose and passPrescale) // inv iso
    {
        float pt = tInv->Pt();
        float eta = tInv->Eta();
        int flavor= tInv->Rematch(e);
        if (pt  > 8000 or pt <0 )  Log(__FUNCTION__,"INFO",Form("strange event : tau Pt=%.0f",pt));
        string hist = HistName(pt,eta,INVERSE_SEL,THREEJETS_SEL);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,eta,INVERSE_SEL, THREEJETS_SEL,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight() );
        hist = HistName(pt,eta,INVERSE_SEL, THREEJETS_SEL,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight() );
        hist = HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL, "TauPt") ;
        Fill( dir+hist +"_"+label,systname, tInv->Pt(), e->weight() );

        hist = HistName(pt,eta, INVERSE_SEL, THREEJETS_SEL) ;
        if (tInv->GetNProng()==1) hist+="_1p";
        else hist+="_3p";
        Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
    }

    // not in the LOOSE --- BTAG SF, only MC
    //#warning no-btag-sf
    if (not e->IsRealData()) e->ApplyBTagSF(1);// 0=loos wp
    // -------------------------- FULL SELECTION -----------------------------------------------
    //
    if (t!=NULL and passMatchDirect and not e->IsRealData()) 
    {
        e->ApplyTauSF(t,false,""); 
    }
    
    // N minus one direct
    if (t!=NULL and passMatchDirect and direct.passAllExcept(ChargedHiggsTauNu::Met) ) 
        {
            Fill( none + "EtMiss" +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        }

    if (t!=NULL and passMatchDirect and direct.passAllExcept(ChargedHiggsTauNu::AngRbb) ) 
        {
            Fill( none + "RbbMin" +"_"+label,systname, e->RbbMin(), e->weight() );
        }

    if (t!=NULL and passMatchDirect and direct.passAllExcept(ChargedHiggsTauNu::AngColl) ) 
        {
            Fill( none + "RCollMin" +"_"+label,systname, e->RCollMin(), e->weight() );
        }

    // DIRECT 
    if (t!=NULL and passMatchDirect and direct.passAll() ) 
    {
            //if ( not e->IsRealData()) e->ApplySF("btag");
            #ifdef VERBOSE
            if (VERBOSE >0 ) Log(__FUNCTION__,"DEBUG", "is tau pass full selection");
            #endif
            //if(e->IsRealData() and (systname=="NONE" or systname=="") ) Log(__FUNCTION__,"SYNC",Form("%d,%d,%ld",e->runNum(),e->lumiNum(),e->eventNum()) );
            float pt = t->Pt();
            float eta = t->Eta();
            int flavor= t->Rematch(e);
            string hist = HistName(pt,eta,DIRECT_SEL,FULL_SEL);  
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight());

            hist= "Mt"; //UNBLIND --------
            if ( Unblind(e) ) Fill(dir+hist+"_"+label,systname, e->Mt() ,e->weight());

            hist = HistName(pt,eta,DIRECT_SEL, FULL_SEL,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
            hist = HistName(pt,eta, DIRECT_SEL, FULL_SEL,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );

            hist = HistName(pt,eta, DIRECT_SEL,FULL_SEL) ;
            if (t->GetNProng()==1) hist+="_1p";
            else hist+="_3p";
            Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );

            if( e->Bjets()> 1)hist = "Mt_cat0";
            else hist = "Mt_cat1";
            Fill(dir+hist+"_"+label,systname, e->Mt() ,e->weight(false));

            float dr=100.; for(int i=0;i<e->Njets(); ++i) { dr = std::min(dr, e->GetJet(i)->DeltaR(t)) ;}
            hist= "DrTauJet";
            Fill( dir+hist +"_"+label,systname, dr, e->weight(false) );
    }

    // ---------------------- INF TAU SF 
    //
    if (tInv != NULL and passMatchInverse and passInverseLoose ){ // USE weight(false) to apply TF on data!
        float pt = tInv->Pt();                                                   
        float eta = tInv->Eta();                                                   
        //const string sf="tauinviso";
        string sfname="tauinvisospline";
        if (tInv->GetNProng() ==1 ) sfname+="_1p";
        else sfname+="_3p";
        // if the SF don't exist go on, but don't fill inconsistent events
        if( not e->ExistSF(sfname) ){
            LogN(__FUNCTION__,"WARNING","Tau inviso SF does not exist",10);
            return EVENT_NOT_USED;
        }
        e->SetPtEtaSF(sfname,tInv->Pt(),tInv->Eta());
        e->ApplySF(sfname); // only in weight(false) sf are applied in data

        //Log(__FUNCTION__,"DEBUG",string("syst name is ") + systname + Form("weight is %f",e->weight(false)) );
        if (tInv != NULL and passInverseLoose and passPrescale){ // CONTROL PLOTS -- LOOSE SELECTION
            string hist= "TauPt_IsoInv_Control";
            Fill( dir+hist +"_"+label,systname, tInv->Pt(), e->weight(false) );
            hist= "EtMiss_IsoInv_Control";
            Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight(false) );
        }

        if (inverse.passAllExcept(ChargedHiggsTauNu::Met))
        {
            //if (not e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120") ) Log(__FUNCTION__,"ERROR","Event Is NOT Triggered!!! Why am I here?");
            //Log(__FUNCTION__,"DEBUG",Form("N-1 MET Selection InvIso w=%.4f",e->weight(false)));
            Fill( none + "EtMissIsoInv" +"_"+label,systname, e->GetMet().Pt(), e->weight(false) );

        }

        if (inverse.passAllExcept(ChargedHiggsTauNu::AngRbb) ) 
            {
                Fill( none + "RbbMinIsoInv" +"_"+label,systname, e->RbbMin(3,tInv), e->weight(false) );
            }

        if (inverse.passAllExcept(ChargedHiggsTauNu::AngColl) ) 
            {
                Fill( none + "RCollMinIsoInv" +"_"+label,systname, e->RCollMin(3,tInv), e->weight(false) );
            }

        // remove trigger & met
        unsigned mymask =  0 ;
        for(unsigned i=0;i<ChargedHiggsTauNu::MaxCut;++i){
            if (i==ChargedHiggsTauNu::Trigger) continue;
            if (i==ChargedHiggsTauNu::Met) continue;
            mymask |= (1<<i); // set all 1
        }

        if (passPrescale and passInverseLoose and inverse.passMask(mymask) )
        {
            // On data avoid R fact -- n minus one
            string hist=HistName(pt,eta, INVERSE_SEL, FULL_SEL)+"_NoR";
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight(true));
        }

        if (inverse.passAll()) { // FULL SELECTION
            //if ( not e->IsRealData()) e->ApplySF("btag");
            #ifdef VERBOSE
            if (VERBOSE >0 ) Log(__FUNCTION__,"DEBUG","is tauInv full selection");
            #endif


            if (e->weight(false) == 0 )Log(__FUNCTION__,"WARNING","event weight after SF is 0 ");

            int flavor= tInv->Rematch(e);
            string hist = HistName(pt,eta,INVERSE_SEL,FULL_SEL);        
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight(false));


            //hist = HistName(pt,false,true,"Mt");  
            hist = "MtIsoInv";
            Fill(dir+hist+"_"+label,systname, e->Mt(Event::MtTauInv) ,e->weight(false));
            hist = HistName(pt,eta,INVERSE_SEL, FULL_SEL,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight(false) );
            hist = HistName(pt,eta,INVERSE_SEL, FULL_SEL,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight(false) );

            if( e->Bjets()> 1)hist = "MtIsoInv_cat0";
            else hist = "MtIsoInv_cat1";
            Fill(dir+hist+"_"+label,systname, e->Mt(Event::MtTauInv) ,e->weight(false));

            hist = HistName(pt, eta, INVERSE_SEL,FULL_SEL) ;
            if (tInv->GetNProng() ==1 ) hist+="_1p";
            else hist+="_3p";
            Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight(false) );

            float dr=100.; for(int i=0;i<e->NjetsInvIso(); ++i) { dr = std::min(dr, e->GetJetInvIso(i)->DeltaR(tInv)) ;}
            hist= "DrTauJetIsoInv";
            Fill( dir+hist +"_"+label,systname, dr, e->weight(false) );
        } // inverse.passAll
    } // tInv


    return EVENT_NOT_USED;
}

int ChargedHiggsQCDPurity::FindBin(float pt,const vector<float> &v)
{
    for(size_t iBin=0;iBin + 1<v.size() ;++iBin)
        if ( pt>= v[iBin] and pt< v[iBin+1] ) return iBin;
    return -1;
}

string ChargedHiggsQCDPurity::HistName(float pt,float eta, bool Direct, bool FullSelection, string var)
{
    #ifdef VERBOSE
    if (VERBOSE>0){cout<<"[ChargedHiggsQCDPurity]::[HistName]::[DEBUG] called HistName with:"<<endl;
                   cout<<"\t * pt="<<pt <<endl;
                   cout<<"\t * Direct= "<<Direct<<endl;
                   cout<<"\t * FullSection= "<<FullSelection<<endl;
                   cout<<"\t * var = "<<var<<endl;
    }
    #endif
    int iBin=FindBin(pt,PtBins);
    int eBin=FindBin(fabs(eta),EtaBins);
    string name;

    if (iBin<0 ) name = var +"_BinNotFound"; 
    else name= Form("%s_pt%.0f_%.0f",var.c_str(),PtBins[iBin],PtBins[iBin+1]);
    
    if (eBin>=0)
    {
        name += Form("_eta%.1f_%.1f", EtaBins[eBin],EtaBins[eBin+1]); 
    }

    if (not Direct) name += "_IsoInv";

    if ( FullSelection) name += "_FullSelection" ;

    #ifdef VERBOSE
    if(VERBOSE>0) cout<<"[ChargedHiggsQCDPurity]::[HistName]::[DEBUG] returning name="<<name<<endl;
    #endif

    return name;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
