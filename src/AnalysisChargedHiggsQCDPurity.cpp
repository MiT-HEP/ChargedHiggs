#include "interface/AnalysisChargedHiggsQCDPurity.hpp"
#include "interface/GeneralFunctions.hpp"

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
        }
    // --- for event not in the PtBins 
    // -- full selection
    for ( string& l : AllLabel()  ) 
        for (size_t iBin = -1 ; iBin + 1 < PtBins.size() ; ++iBin )
        {
            float pt = -1;
            if (iBin>=0 ) pt= PtBins[iBin];
            Book( dir + HistName(pt, true , true)+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, true)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
            //
            Book( dir + HistName(pt, true , true,"Mt")+"_"+ l  , ("Mt "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, true,"Mt")+"_"+ l  , ("MtIsoInv "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt, true , true, "Upar")+"_"+ l  , ("EtMissParallel "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, true, true, "Uperp")+"_"+ l  , ("EtMissPerp "+ l).c_str(),250,0.,500.);

            Book( dir + HistName(pt, false , true, "Upar")+"_"+ l  , ("EtMissParallelIsoInv "+ l).c_str(),250,0.,500);
            Book( dir + HistName(pt, false, true, "Uperp")+"_"+ l  , ("EtMissPerpIsoInv "+ l).c_str(),250,0.,500.);

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

    // TODO:
    // * what do I do with event with a Tau and an Inv tau? -> DY ? 
    // * put a limit on the TauInv sideband ? 3.0 -20 GeV

    // TODO -> use TAU + MET TRIGGER 
    if ( e->IsRealData() and not e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v") )  {
        return EVENT_NOT_USED;
    }
    //cout <<" EVENT TRIGGERED"<<endl;
    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is real data? "<<e->IsRealData() <<" and pass trigger" <<endl;
    #endif
    //
    // ---
    if ( t==NULL and tInv==NULL ) return EVENT_NOT_USED;
    if ( e->Nleps() >0 ) return EVENT_NOT_USED;
    if ( e->Njets() <3 ) return EVENT_NOT_USED;

    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] pass basic selection (Njets, nLepts)"<<endl;
    #endif

    if (t != NULL and t->Pt()>=51 and fabs(t->Eta())<2.1)
    {
        float pt = t->Pt();

        if (pt  > 8000 or pt <0 ) 
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[INFO] strange event:  tau Pt="<<pt<<endl;

        string hist = HistName(pt,true, false);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,true, false,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
        hist = HistName(pt,true, false,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );
    }

    if (tInv != NULL and tInv->Pt()>=51 and fabs(tInv->Eta())<2.1)
    {
        cout<<" TauInv Guard A"<<endl; //DEBUG
        float pt = tInv->Pt();
        if (pt  > 8000 or pt <0 ) 
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[INFO] strange event:  tau (inv iso) Pt="<<pt<<endl;
        string hist = HistName(pt,false,false);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        hist = HistName(pt,false, false,"Uperp");
        Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight() );
        hist = HistName(pt,false, false,"Upar");
        Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight() );
    }

    // -------------------------- FULL SELECTION -----------------------------------------------
    if ( e->GetMet().Pt() <130 ) return EVENT_NOT_USED;
    if ( e->Bjets() <1 ) return EVENT_NOT_USED;

    #ifdef VERBOSE
    if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] pass Met, and Bjets"<<endl;
    #endif

    double DPhiEtMissJet1=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(0))->Phi()));
    double DPhiEtMissJet2=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(1))->Phi()));
    double DPhiEtMissJet3=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(2))->Phi()));

    if (t!=NULL) 
    {
        double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),t->Phi()));

        double RbbMin=min(min(sqrt(pow(DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
        double RCollMin=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(DPhiEtMissTau,2)));
        double RsrMax=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
        if ( RCollMin*TMath::RadToDeg() >=40 and RbbMin*TMath::RadToDeg() >=40 ){
            #ifdef VERBOSE
            if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is tau, pass angular cuts"<<endl;
            #endif
            float pt = t->Pt();
            string hist = HistName(pt,true,true);  
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight());

            hist = HistName(pt,true,true,"Mt");  
            Fill(dir+hist+"_"+label,systname, e->Mt() ,e->weight());
            hist = HistName(pt,true, true,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,t), e->weight() );
            hist = HistName(pt,true, true,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,t), e->weight() );
        }
    }

    if (tInv != NULL ) {
        cout<<" TauInv Guard B"<<endl; //DEBUG
        #ifdef VERBOSE
        if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is tauInv"<<endl;
        #endif
        // if the SF don't exist go on, but don't fill inconsistent events
        if( not e->ExistSF("tauinviso") ){
            static int count = 0 ;
            if (count++ <100) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[INFO] tauinviso SF does not exists"<<endl;
            return EVENT_NOT_USED;
        }

        //cout <<"[DEBUG] Setting SF Pt Eta for tauinviso to "<<tInv->Pt()<<" "<<tInv->Eta()<<endl;
        e->SetPtEtaSF("tauinviso",tInv->Pt(),tInv->Eta());
        e->ApplySF("tauinviso");

        if (e->weight() == 0 )
        {
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[WARNING] event weight after SF is 0."<<endl; 
        }
        //e->weight();
        double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),tInv->Phi()));
        double RbbMin=min(min(sqrt(pow(DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
        double RCollMin=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(DPhiEtMissTau,2)));
        double RsrMax=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
        if ( RCollMin*TMath::RadToDeg() >=40 and RbbMin*TMath::RadToDeg() >=40 ){    
            #ifdef VERBOSE
            if (VERBOSE >0 ) cout<<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG1] is TauInv, pass angular cuts. fill with SF."<<endl;
            #endif
            float pt = tInv->Pt();                                                   
            string hist = HistName(pt,false,true);                                   
            Fill(dir+hist+"_"+label,systname, e->GetMet().Pt() ,e->weight());

            hist = HistName(pt,false,true,"Mt");  
            Fill(dir+hist+"_"+label,systname, e->Mt(Event::MtTauInv) ,e->weight());
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[DEBUG] Filling histo: '"<<dir+hist+"_"+label<<"' with Mt="<< e->Mt(Event::MtTauInv) <<" and weight="<<e->weight()<<endl;
            hist = HistName(pt,false, true,"Uperp");
            Fill( dir+hist +"_"+label,systname, Upar(e,tInv), e->weight() );
            hist = HistName(pt,false, true,"Upar");
            Fill( dir+hist +"_"+label,systname, Uperp(e,tInv), e->weight() );
        }

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
