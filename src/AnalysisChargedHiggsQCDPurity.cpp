#include "interface/AnalysisChargedHiggsQCDPurity.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/CutSelector.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include <string>


//#define VERBOSE 1

void ChargedHiggsQCDPurity::Init()
{
    for ( string& l : AllLabel()  ) 
        for (size_t iBin = -1 ; iBin + 1 < PtBins.size() ; ++iBin )
        {
            float pt = -1;
            if (iBin>=0 ) pt= PtBins[iBin];
            //                       direct, fullSel
            Book( dir + HistName(pt, true , false)+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, false)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            // ---- 
            Book( dir + HistName(pt, true , false, "Upar")+"_"+ l  , ("EtMissParallel "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, false, "Upar")+"_"+ l  , ("EtMissParallelIsoInv "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt, true , false, "Uperp")+"_"+ l  , ("EtMissPerp "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, false, "Uperp")+"_"+ l  , ("EtMissPerpIsoInv "+ l).c_str(),250,0.,500.);
            //  Used in case of spline reweighting iterations
            Book( dir + HistName(pt, true,  false, "TauPt")+"_"+ l  , ("PtTau "+ l).c_str(),1000,0.,1000.);
            Book( dir + HistName(pt, false, false, "TauPt")+"_"+ l  , ("PtTauIsoInv "+ l).c_str(),1000,0.,1000.);
        }
    // --- for event not in the PtBins 
    // -- full selection
    for ( string& l : AllLabel()  ) 
    {
        for (size_t iBin = -1 ; iBin + 1 < PtBins.size() ; ++iBin )
        {
            float pt = -1;
            if (iBin>=0 ) pt= PtBins[iBin];
            Book( dir + HistName(pt, true , true)+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, true)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            //
            Book( dir + HistName(pt, true , true, "Upar")+"_"+ l  , ("EtMissParallel "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, true, true, "Uperp")+"_"+ l  , ("EtMissPerp "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt, false , true, "Upar")+"_"+ l  , ("EtMissParallelIsoInv "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, true, "Uperp")+"_"+ l  , ("EtMissPerpIsoInv "+ l).c_str(),250,0.,500.);

        }
        // I don't need to split it by pt
            Book( dir + "Mt"+"_"+ l  , ("Mt "+ l).c_str(),250,0.,500);
            Book( dir + "MtIsoInv"+"_"+ l  , ("MtIsoInv "+ l).c_str(),250,0.,500.);
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

    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is Tau? "<< (t==NULL) <<endl;
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is TauInv? "<< (tInv == NULL)<<endl;
    #endif

    // --- take the selection from the tau nu analysis
    CutSelector direct; 
        direct.SetMask(ChargedHiggsTauNu::MaxCut-1);
        direct.SetCut(ChargedHiggsTauNu::Selection(e,true));
    CutSelector inverse;
        inverse.SetMask(ChargedHiggsTauNu::MaxCut-1);
        inverse.SetCut(ChargedHiggsTauNu::Selection(e,false));

    // TODO:
    // * what do I do with event with a Tau and an Inv tau? -> DY ? 
    // * put a limit on the TauInv sideband ? 3.0 -20 GeV

    if ( not direct.passAllUpTo(ChargedHiggsTauNu::ThreeJets)
         and not inverse.passAllUpTo(ChargedHiggsTauNu::ThreeJets)
       ) return EVENT_NOT_USED;


    //  USE PRESCALE PATH ONLY FOR THE "inclusive/Loose" selection
    bool passPrescale=false;
    if (not e->IsRealData()) passPrescale=true;
    if (  e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v") ) passPrescale=true;

    if (t != NULL and passPrescale) // direct
    {
        float pt = t->Pt();

        if (pt  > 8000 or pt <0 )  Log(__FUNCTION__,"INFO",Form("strange event : tau Pt=%.0f",pt));

        string hist = HistName(pt,true, false);
        Fill( dir + hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,true, false,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
        hist = HistName(pt,true, false,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );
        hist = HistName(pt, true, false, "TauPt") ;
        Fill( dir+hist +"_"+label,systname, t->Pt(), e->weight() );
    }

    if (tInv != NULL and passPrescale) // inv iso
    {
        float pt = tInv->Pt();
        if (pt  > 8000 or pt <0 )  Log(__FUNCTION__,"INFO",Form("strange event : tau Pt=%.0f",pt));
        string hist = HistName(pt,false,false);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,false, false,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight() );
        hist = HistName(pt,false, false,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight() );
        hist = HistName(pt, false, false, "TauPt") ;
        Fill( dir+hist +"_"+label,systname, tInv->Pt(), e->weight() );
    }

    // -------------------------- FULL SELECTION -----------------------------------------------

    if (t!=NULL and direct.passAll() ) 
    {
            #ifdef VERBOSE
            if (VERBOSE >0 ) Log(__FUNCTION__,"DEBUG", "is tau pass full selection");
            #endif
            float pt = t->Pt();
            string hist = HistName(pt,true,true);  
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight());

            hist= "Mt"; //UNBLIND --------
            if ( Unblind(e) ) Fill(dir+hist+"_"+label,systname, e->Mt() ,e->weight());

            hist = HistName(pt,true, true,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
            hist = HistName(pt,true, true,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );
    }

    if (tInv != NULL and inverse.passAll()) {
        #ifdef VERBOSE
        if (VERBOSE >0 ) Log(__FUNCTION__,"DEBUG","is tauInv full selection");
        #endif
        // if the SF don't exist go on, but don't fill inconsistent events
        if( not e->ExistSF("tauinviso") ){
            static int count = 0 ;
            if (count++ < 20 )Log(__FUNCTION__,"WARNING","tau inviso SF does not exist" );
            return EVENT_NOT_USED;
        }

        e->SetPtEtaSF("tauinviso",tInv->Pt(),tInv->Eta());
        e->ApplySF("tauinviso");

        if (e->weight() == 0 )Log(__FUNCTION__,"WARNING","event weight after SF is 0 ");

            float pt = tInv->Pt();                                                   
            string hist = HistName(pt,false,true);                                   
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight());

            //hist = HistName(pt,false,true,"Mt");  
            hist = "MtIsoInv";
            Fill(dir+hist+"_"+label,systname, e->Mt(Event::MtTauInv) ,e->weight());
            hist = HistName(pt,false, true,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight() );
            hist = HistName(pt,false, true,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight() );
    }


    return EVENT_NOT_USED;
}

int ChargedHiggsQCDPurity::FindBin(float pt)
{
    for(size_t iBin=0;iBin + 1<PtBins.size() ;++iBin)
        if ( pt>= PtBins[iBin] and pt< PtBins[iBin+1] ) return iBin;
    return -1;
}

string ChargedHiggsQCDPurity::HistName(float pt, bool Direct, bool FullSelection, string var)
{
    #ifdef VERBOSE
    if (VERBOSE>0){cout<<"[ChargedHiggsQCDPurity]::[HistName]::[DEBUG] called HistName with:"<<endl;
                   cout<<"\t * pt="<<pt <<endl;
                   cout<<"\t * Direct= "<<Direct<<endl;
                   cout<<"\t * FullSection= "<<FullSelection<<endl;
                   cout<<"\t * var = "<<var<<endl;
    }
    #endif
    int iBin=FindBin(pt);
    string name;

    if (iBin<0 ) name = var +"_BinNotFound"; 
    else name= Form("%s_pt%.0f_%.0f",var.c_str(),PtBins[iBin],PtBins[iBin+1]);

    if (not Direct) name += "_IsoInv";

    if ( FullSelection) name += "_FullSelection" ;

    #ifdef VERBOSE
    if(VERBOSE>0) cout<<"[ChargedHiggsQCDPurity]::[HistName]::[DEBUG] returning name="<<name<<endl;
    #endif

    return name;
}

// ---- All plots have a wPlus/wMinus 
void ChargedHiggsQCDPurity::Book(string name, string title,int nBins, double xmin, double xmax){
    AnalysisBase::Book(name,title,nBins,xmin,xmax);
    if (name.find("_Data") != string::npos) return;
    string wPlus = name + "_wPlus";
    string wMinus = name + "_wMinus";

    AnalysisBase::Book(wPlus,title,nBins,xmin,xmax);
    AnalysisBase::Book(wMinus,title,nBins,xmin,xmax);

}

void ChargedHiggsQCDPurity::Fill(string name, string syst , double value, double weight)
{
    AnalysisBase::Fill(name,syst,value,weight);
    if (name.find("_Data") != string::npos) return;
    string wPlus = name + "_wPlus";
    string wMinus = name + "_wMinus";
    if (weight>0 ) AnalysisBase::Fill(wPlus,syst,value,weight);
    if (weight<0 ) AnalysisBase::Fill(wMinus,syst,value,weight);
    return ;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
