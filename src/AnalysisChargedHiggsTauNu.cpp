#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/Logger.hpp" // for static functions
#include <memory>

//#define SYNC 1

void ChargedHiggsTauNu::Init()
{

    for ( string& l : AllLabel()  ) {

        Log(__FUNCTION__,"INFO", "Booking Histo CutFlow_" + l);
  
        Book(    "ChargedHiggsTauNu/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);
        {
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Total+1,"Total"); // bin shift
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneTau+1,"N_{taus} #geq 1");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(NoLep+1,"N_{leps} = 0");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(ThreeJets,"N_{jets} #geq 3");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(OneBjet,"N_{Bjets} #geq 1");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(Met,"MET > 60 GeV");
            //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"R_{sr}^{Max} < 1.96");
            //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"R_{sr}^{Max} < 140");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(AngColl,"R_{coll}^{Min}>40");
            GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(AngRbb,"R_{bb}^{Min}>40");
            //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"#Delta #phi (Tau,Jet1)<2.09");
        }

        // Cut flow with QCD
        Book(    "ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+l , ("CutFlowQCD "+ l).c_str(), 100, -.5, 100-.5);
        {
            GetHisto("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+l,"")->GetXaxis()->SetBinLabel(1, "N_{taus} #geq 1 \\ N_{leps} = 0 \\ N_{jets} #geq 3 \\ Trigger");
            GetHisto("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+l,"")->GetXaxis()->SetBinLabel(2,"N_{Bjets} #geq 1");
            GetHisto("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+l,"")->GetXaxis()->SetBinLabel(3,"MET > 60 GeV");
            GetHisto("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+l,"")->GetXaxis()->SetBinLabel(4,"R_{bb}^{Min}>40");
        }

        // -- cut flow 2 is the one used to synchronized with HIP
        Book(    "ChargedHiggsTauNu/CutFlow/CutFlow2_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);

        Book(    "ChargedHiggsTauNu/NOne/NTaus_"+l,"NTaus "+l +";Number of selected hadronic tau" ,1000,0,1000);

        Book(    "ChargedHiggsTauNu/NOne/Tau1Pt_"+l,"Tau1Pt "+l +";p_{T}^{#tau}(leading #tau) [GeV]" ,1000,0,1000);

        //Book(    "ChargedHiggsTauNu/NOne/Tau1Eta_"+l,"Tau1Eta "+l +";#eta^{#tau}(leading #tau) [GeV]",100,-5,5);

        Book(    "ChargedHiggsTauNu/NOne/NJets_"+l,"NJets "+l + "; Number of selected jets",1000,0,1000);

        Book(    "ChargedHiggsTauNu/NOne/Jet1Pt_"+l,"Jet1Pt "+l+";p_{T}^{jet}(leading jet) [GeV]",1000,0,1000);

        Book(    "ChargedHiggsTauNu/NOne/Jet1Eta_"+l,"Jet1Eta "+l+";#eta^{jet}(leading jet) [GeV]",100,-5,5);

        //Book(    "ChargedHiggsTauNu/NOne/Jet2Pt_"+l,"Jet2Pt "+l+";p_{T}^{jet}(sub-leading jet) [GeV]",1000,0,1000);

        //Book(    "ChargedHiggsTauNu/NOne/Jet2Eta_"+l,"Jet2Eta "+l+";#eta^{jet}(sub-leading jet) [GeV]",100,-5,5);

        //Book(    "ChargedHiggsTauNu/NOne/Jet3Pt_"+l,"Jet3Pt "+l+";p_{T}^{jet}(subsub-leading jet) [GeV]",1000,0,1000);

        //Book(    "ChargedHiggsTauNu/NOne/Jet3Eta_"+l,"Jet3Eta "+l + ";#eta^{jet}(subsub-leading jet) [GeV]",100,-5,5);

        Book(    "ChargedHiggsTauNu/NOne/NBjets_"+l,"NBjets "+l + ";Number of selected b jets",1000,0,1000);

        Book(    "ChargedHiggsTauNu/NOne/Bdiscr_"+l,"BDiscr "+l + ";Btag Discr",1000,-2,2);

        Book(    "ChargedHiggsTauNu/NOne/Bjet1Pt_"+l,"Bjet1Pt "+l+"p_{T}^{b-tagged jet} [GeV]",1000,0,1000);

        //Book(    "ChargedHiggsTauNu/NOne/Bjet1Eta_"+l,"Bjet1Eta "+l+";#eta^{b-tagged jet} [GeV]",100,-5,5);

        //Book(    "ChargedHiggsTauNu/NOne/MaxDEtaBjetJets_"+l,"MaxDEtaBjetJets "+l+";Max #Delta#eta (b-tagged jet,jets)",100,0,10);

        //Book(    "ChargedHiggsTauNu/NOne/MaxInvMassBjetJets_"+l,"MaxInvMassBjetJets "+l+";Max invariant mass (b-tagged jet,jets) [GeV]",2000,0,2000);

        Book(    "ChargedHiggsTauNu/NOne/EtMiss_"+l,"EtMiss "+l+";E_{T}^{miss} [GeV]",1000,0,1000);

        //Book2D(  "ChargedHiggsTauNu/NOne/EtMissVsMt_"+l,"EtMissVsMt "+l+";m_{T} [GeV];E_{T}^{miss} [GeV]",1000,0,1000,1000,0,1000);

        Book2D(  "ChargedHiggsTauNu/NOne/DPhiJet1MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l + ";#Delta #phi(#tau,MET);#Delta #phi(Jet1,MET)",50,0,TMath::Pi(),50,0,TMath::Pi());

        Book2D(  "ChargedHiggsTauNu/NOne/DPhiJet2MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l+";Delta #phi(#tau,MET);#Delta #phi(Jet2,MET)",50,0,TMath::Pi(),50,0,TMath::Pi());

        Book2D(  "ChargedHiggsTauNu/NOne/DPhiJet3MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l+";#Delta #phi(#tau,MET);#Delta #phi(Jet3,MET)",50,0,TMath::Pi(),50,0,TMath::Pi());

        Book(    "ChargedHiggsTauNu/NOne/RCollMin_"+l,"RCollMin "+l+";R_{coll}^{min}",100,0,2*TMath::Pi());

        Book(    "ChargedHiggsTauNu/NOne/RbbMin_"+l,"RbbMin "+l+";R_{bb}^{min}",100,0,2*TMath::Pi());

        //Book(    "ChargedHiggsTauNu/NOne/RsrMax_"+l,"RsrMax "+l+";R_{sr}^{max}",100,0,2*TMath::Pi());

        //Book(    "ChargedHiggsTauNu/NOne/DPhiTauJet1_"+l,"DPhiTauJet1 "+l+";#Delta #phi (#tau, leading jet)",50,0,TMath::Pi());

        //Book2D(  "ChargedHiggsTauNu/NOne/RCollMinVsMt_"+l,"RCollMinVsMt "+l + ";m_{T} [GeV];R_{Coll}^{min}",1000,0,1000,100,0,2*TMath::Pi());

        //Book2D(  "ChargedHiggsTauNu/NOne/RbbMinVsMt_"+l,"RbbMinVsMt "+l+";m_{T} [GeV];R_{bb}^{min}",1000,0,1000,100,0,2*TMath::Pi());

        //Book2D(  "ChargedHiggsTauNu/NOne/RsrMaxVsMt_"+l,"RsrMaxVsMt "+l+";m_{T} [GeV];R_{sr}^{Max}",1000,0,1000,100,0,2*TMath::Pi());

        //Book2D(  "ChargedHiggsTauNu/NOne/DPhiTauJet1VsMt_"+l,"DPhiTauJet1VsMt "+l + ";m_{T} [GeV];#Delta #phi (#tau, leading jet)",1000,0,1000,50,0,TMath::Pi());

        /*********************************************
         *              FULL SELECTION               *
         *********************************************/

        // for the TTJets background
        Book(    "ChargedHiggsTauNu/Vars/JetInvMass_" + l ,"Jet Invariant mass;m_{jj} [GeV]",1000,0,1000) ;
        //Book(    "ChargedHiggsTauNu/Vars/Jet13InvMass_" + l ,"Jet Invariant mass;m_{jj} [GeV]",1000,0,1000) ;
        //Book(    "ChargedHiggsTauNu/Vars/Jet23InvMass_" + l ,"Jet Invariant mass;m_{jj} [GeV]",1000,0,1000) ;

        //Book(    "ChargedHiggsTauNu/Vars/Jet1QGL_" + l ,"QGL of the leading jet;QGL",300,-1.5,1.5) ;
        //Book(    "ChargedHiggsTauNu/Vars/Jet2QGL_" + l ,"QGL of the subleading jet;QGL",300,-1.5,1.5) ;

        //Book(    "ChargedHiggsTauNu/Vars/MtDecoQ_" + l ,"Mt deco;MtQ",1000,0,1000) ;
        //Book(    "ChargedHiggsTauNu/Vars/MtDecoCosPhi_" + l ,"Mt deco;MtCosPhi",1000,-1,1) ;

        Book(    "ChargedHiggsTauNu/Vars/Mt_matchTau_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        Book(    "ChargedHiggsTauNu/Vars/Mt_matchEle_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        Book(    "ChargedHiggsTauNu/Vars/Mt_matchMu_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        Book(    "ChargedHiggsTauNu/Vars/Mt_matchJet_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        Book(    "ChargedHiggsTauNu/Vars/Mt_matchOther_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        /**********************************************
         *                   MT                       *
         **********************************************/
        Book(    "ChargedHiggsTauNu/Vars/Mt_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        AddFinalHisto("ChargedHiggsTauNu/Vars/Mt_"+l);
        // study categorization
        for(int i=0;i<=1;++i)
        {
        Book(    string("ChargedHiggsTauNu/Vars/Mt_")+Form("cat%d",i)+"_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        }


    }

}


unsigned ChargedHiggsTauNu::Selection(Event *e, bool direct, bool muon) {
    
    CutSelector cut; 
    cut.SetMask(MaxCut-1);
    cut.SetCutBit(Total);

    std::unique_ptr<Tau> garbage; //if created will be deleted

    Tau *t =NULL;
    if (direct and not muon) t = e->GetTau(0); 
    else if(not muon) t = e->GetTauInvIso(0);
    else {
        Lepton*m=e->GetMuon(0);
        //Construct a fake tau
        if (m!=NULL){
            t=new Tau();
            t->SetP4( m->GetP4() );
            t-> iso =0;
            t-> type =15;
            t-> iso2=0;
            t-> id=1;
            t-> id_ele=1;
            t-> id_mu=1;
            t-> id_iso=1;
            garbage.reset(t); // make sure it will be deleted
        }
    }

    Object *sub = NULL;
    if (direct and not muon) sub = e->GetTau(1);
    if (muon) sub=e->GetMuon(1);

    //------------- TRIGGER -----------
    {
        if ( not muon and e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110"))  cut.SetCutBit(Trigger);
        else if (muon and e->IsTriggered("HLT_IsoMu20")) cut.SetCutBit(Trigger);
    }
    
    if (muon) { // special treatment for muons..
        
        if(e->IsTriggered("HLT_IsoMu20_v")) {
            cut.SetCutBit(Trigger);
            cout << "triggered" << endl;
        }
        else cout << "not triggered" << endl;
        
    }
    
    // --------------END TRIGGER

    if (t== NULL) return cut.raw();
    //if (sub != NULL) return cut.raw(); //multiple taus

    //----------------- ONE TAU -------------
    if (isLightMass){
        if (  // pt 20, Iso 1.5
             t->Pt()>= 50 and 
             fabs(t->Eta() ) <2.1
                ) cut.SetCutBit(OneTau) ;
    }
    else{//high mass
        if (  // pt 20, Iso 1.5
             t->Pt()>= 60 and 
             fabs(t->Eta() ) <2.1
                ) cut.SetCutBit(OneTau) ;
    }


    bool lepVeto=false;
    if (e->Nleps() ==0 ) lepVeto=true;
    else if( e->GetMuon(0) == NULL){ // no 10 GeV muon
        
        if (e->GetElectron(0) !=NULL and e->GetElectron(0)->Pt() <15) lepVeto=true; // pt ordered
    
    }

    if ( lepVeto and not muon) cut.SetCutBit(NoLep);
    if ( muon  and e->Nleps() ==1) cut.SetCutBit(NoLep);;

    // ---- At least 3 jets
    if ( direct and e->Njets() >=3 ) cut.SetCutBit(ThreeJets);
    if ( not direct and e->NjetsInvIso() >=3 ) cut.SetCutBit(ThreeJets);

    // --- At least 1 b-jet
    if ( direct and e->Bjets() >=1 ) cut.SetCutBit(OneBjet) ;
    if ( not direct and e->BjetsInvIso() >=1 ) cut.SetCutBit(OneBjet) ;

    
    // MET 
    if (isLightMass){
        if ( e->GetMet().Pt() >= 90 ) cut.SetCutBit(Met);
    }
    else{
        if ( e->GetMet().Pt() >= 100 ) cut.SetCutBit(Met); // or PtUncorr
    }

    double RbbMin= e->RbbMin(3,t);
    double RCollMin= e-> RCollMin(3,t);
    double RsrMax= e->RsrMax(3,t);

    //if ( RCollMin*TMath::RadToDeg() >40 ) cut.SetCutBit( AngColl) ;
    if ( RCollMin*TMath::RadToDeg() >= 0 ) cut.SetCutBit( AngColl) ;

    //if ( RbbMin*TMath::RadToDeg() >40 ) cut.SetCutBit(AngRbb) ;
    if ( RbbMin > 0.8 ) cut.SetCutBit(AngRbb) ;

    return cut.raw();
}

int ChargedHiggsTauNu::analyze(Event*e,string systname)
{
    #ifdef VERBOSE
        if(VERBOSE>0)Log(__FUNCTION__,"DEBUG","analyze event with syst "+systname);
    #endif
    string label = GetLabel(e);

    if(e->weight() == 0.) cout <<"[ChargedHiggsTauNu]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    #ifdef SYNC
        if (not e->IsRealData() and e->GetWeight() ->GetBareSF() != 1.0)  Log(__FUNCTION__,"SYNC-ERROR",Form("SF at the beginning is not 1: %lf",e->GetWeight() ->GetBareSF()));
    #endif
    
    e->ApplyTopReweight();

    #ifdef SYNC
        double toprw = e->GetWeight()->GetBareSF();
    #endif

    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,Total,e->weight());
    Fill("ChargedHiggsTauNu/NOne/NTaus_"+label,systname, e->Ntaus() ,e->weight());
    
    Tau *t= e->GetTau(0);


    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCut( Selection(e,true, false) );

    #ifdef SYNC
    if (SYNC>0){
        if(not e->IsRealData()) Log(__FUNCTION__,"SYNC",Form("(%d,%d,%u) MCWeight=%lf Xsec=%lf PU=%lf TopReweight=%lf",
                    e->runNum(),e->lumiNum(),e->eventNum(),
                    e->GetWeight()->GetBareMCWeight(),
                    e->GetWeight()->GetBareMCXsec(),
                    e->GetWeight()->GetBarePUWeight(),
                    toprw)
                );
        CutSelector mymask(MaxCut);
        mymask.reset();
        mymask.SetCutBit(Trigger);

        if (cut.passMask(mymask) ) Log(__FUNCTION__,"SYNC",Form("(%d,%d,%u) Trigger",e->runNum(),e->lumiNum(),e->eventNum()) ) ; 
        mymask.SetCutBit(OneTau);
        if (cut.passMask(mymask) ) Log(__FUNCTION__,"SYNC",Form("(%d,%d,%u) Selected tau: pt=%e eta=%e",e->runNum(),e->lumiNum(),e->eventNum(),e->GetTau(0)->Pt(),e->GetTau(0)->Eta()) ) ; 
        if (cut.passMask(mymask) ) Log(__FUNCTION__,"SYNC",Form("(%d,%d,%u) Njets %d Bjets %d",e->runNum(),e->lumiNum(),e->eventNum(),e->Njets(), e->Bjets()) ) ; 
        if (cut.passMask(mymask) ) Log(__FUNCTION__,"SYNC",Form("(%d,%d,%u) TauSel metPt=%e metPhi=%e mT=%e",e->runNum(),e->lumiNum(),e->eventNum(),e->GetMet().Pt(),e->GetMet().Phi(),e->Mt()) ) ; 
    }
    #endif

    // here I have the PV and the MET Filters
    if (e->IsRealData() ) 
        { 
            int pos=0;
            Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            CutSelector mymask(MaxCut);
            mymask.reset();

            mymask.SetCutBit(Trigger); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(OneTau);++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(NoLep); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(ThreeJets); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(OneBjet); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(Met); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(AngRbb); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

        }

    // here I have the PV and the MET Filters
    if (e->IsRealData() ) 
        { 
            int pos=0;
            Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            CutSelector mymask(MaxCut);
            mymask.reset();

            mymask.SetCutBit(Trigger);; ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(OneTau);++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(NoLep); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(ThreeJets); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(OneBjet); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(Met); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

            mymask.SetCutBit(AngRbb); ++pos;
            if( cut.passMask( mymask ) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow2_"+label,systname,pos,e->weight());

        }

    //Log(__FUNCTION__,"DEBUG","Analyze event with syst "+ systname + Form(" Njets=%d NB=%d PassAll=%d cuts=%s", e->Njets(),e->Bjets() ,cut.passAll(), ChargedHiggs::printBinary(cut.raw()).c_str() ));

    if ( cut.pass(NoLep) and not e->IsRealData() ){
        // SF for Veto
        {
            GenParticle * gp  = e->GetGenElectron(0,2.4);
            if (not e->ExistSF("eleveto")) Log(__FUNCTION__,"WARNING","No eleveto SF"); //FIXME Remove this line, may be slow
            if (gp != NULL and gp->Pt() > 15) {e->SetPtEtaSF("eleveto",gp->Pt(),fabs(gp->Eta())); e->ApplySF("eleveto");}  // this should be SC-eta, some how propagated
        }
        //Muon
        {
            GenParticle * gp  = e->GetGenMuon(0,2.4);
            if (not e->ExistSF("muveto")) Log(__FUNCTION__,"WARNING","No muveto SF"); //FIXME Remove this line, may be slow
            if (gp != NULL and gp->Pt() > 10) {e->SetPtEtaSF("muveto",gp->Pt(),fabs(gp->Eta())); e->ApplySF("muveto");}  // this should be SC-eta, some how propagated
        }
    }
 
    // Tau and Trigger efficiency (SAMI)
    // For non re-HLT samples --> apply directly the efficiencies in data
    // For all the oters, apply data/mc efficiencies
    string tauLegSF, metLegSF;
    tauLegSF = "tauLeg";
    metLegSF = "metLeg";  


    //#warning no sf trigger
    if (cut.pass(Trigger) and not e->IsRealData()) {
        
        if (t!=NULL){e->ApplyTauSF(t,false,"");}

        if( not e->ExistSF(metLegSF) ) Log(__FUNCTION__,"WARING" ,"No Tau"+metLegSF+" SF");  
        
        // Met SF only for pT > 20
        if(e->GetMet().Pt()>20) {
            e->SetPtEtaSF(metLegSF, e->GetMet().Pt(), 0);
            e->ApplySF(metLegSF);
        }
    } 

    //#warning nobtag-sf
    //Log(__FUNCTION__,"DEBUG",Form("syst is=%s weight before=%e",systname.c_str(),e->weight()) ) ;
    
    if (not e->IsRealData()) e->ApplyBTagSF(0);// 0=loos wp

    //Log(__FUNCTION__,"DEBUG",Form("syst is=%s weight after=%e",systname.c_str(),e->weight()) ) ;

    #warning TauMatch
    if (not e->IsRealData() ) 
        {
        if (e->GetTau(0) == NULL )  return 0;
        //if (e->GetTau(0)->Rematch(e) !=15) return 0;
        }
    

    if (e->IsRealData() or e->GetTau(0)->Rematch(e) == 15) {
        if( cut.passAllUpTo( OneTau)   ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,OneTau,e->weight());
        if( cut.passAllUpTo(NoLep)     ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,NoLep,e->weight());
        if( cut.passAllUpTo(ThreeJets) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,ThreeJets,e->weight());
        if( cut.passAllUpTo(OneBjet)   ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,OneBjet,e->weight());
        if( cut.passAllUpTo(Met)       ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,Met,e->weight());
        if( cut.passAllUpTo(Trigger)   ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,Trigger,e->weight());

        
        if( cut.passAllUpTo(ThreeJets) && cut.pass(Trigger)) Fill("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+label,systname,0,e->weight());
        if( cut.passAllUpTo(OneBjet) && cut.pass(ChargedHiggsTauNu::Trigger)) Fill("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+label,systname,1,e->weight());
        if( cut.passAllUpTo(Met)) Fill("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+label,systname,2,e->weight());
    }

    // VARS, N-1 ,
    // 1 hadronic tau only. with Pt> 50 and eta <2.1
    // before angular variables
    // ...
   
    //cout <<"cut = "<<cut<<endl;
    //CutSelector s; s.SetCutBit(Met);
    //cout <<"met = "<<s<<endl;
    // -------------------- ANGULAR VARIABLES -----------
    double DEtaMax=0.;
    double InvMassMax=0.;
  
    
    Jet * bj1 = e->LeadBjet();
    for(int i=0;i!=e->Njets();++i)
        {
            if (bj1 == NULL ) break;
            Jet* jet = e->GetJet(i);
            if(bj1->DeltaEta(*jet)>DEtaMax) DEtaMax=bj1->DeltaEta(*jet);
            if(bj1->InvMass(*jet)>InvMassMax) InvMassMax=bj1->InvMass(*jet);
        }


    double DPhiEtMissJet1=e->DPhiEtMissJet(0); 
    double DPhiEtMissJet2=e->DPhiEtMissJet(1); 
    double DPhiEtMissJet3=e->DPhiEtMissJet(2); 
    double DPhiEtMissTau= ( t== NULL) ? -1 : fabs(e->GetMet().DeltaPhi(t));

    // up To angular variables
    double RbbMin= e->RbbMin();
    double RCollMin= e-> RCollMin();
    double RsrMax= e->RsrMax();


    double DPhiTauJet1=e->DPhiTauJet(t,0);

    // --- studies for angular variables
    if ( cut.passAllUpTo(Met) and (e->IsRealData() or e->GetTau(0)->Rematch(e) == 15 )){
        //Fill("ChargedHiggsTauNu/NOne/MaxDEtaBjetJets_"+label,systname, DEtaMax ,e->weight());
        //Fill("ChargedHiggsTauNu/NOne/MaxInvMassBjetJets_"+label,systname, InvMassMax ,e->weight());
        Fill2D("ChargedHiggsTauNu/NOne/DPhiJet1MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet1,e->weight());
        Fill2D("ChargedHiggsTauNu/NOne/DPhiJet2MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet2,e->weight());
        Fill2D("ChargedHiggsTauNu/NOne/DPhiJet3MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet3,e->weight());
        // ---
        //Fill("ChargedHiggsTauNu/NOne/RsrMax_"+label,systname,RsrMax,e->weight());
        //Fill2D("ChargedHiggsTauNu/NOne/RsrMaxVsMt_"+label,systname,e->Mt(),RsrMax,e->weight());
        //Fill("ChargedHiggsTauNu/NOne/DPhiTauJet1_"+label,systname,DPhiTauJet1,e->weight());
        //Fill2D("ChargedHiggsTauNu/NOne/DPhiTauJet1VsMt_"+label,systname,e->Mt(),DPhiTauJet1,e->weight());
    }


    //Angular Cuts

    // ANG VALUES FOR Regression
    if (e->IsRealData() or e->GetTau(0)->Rematch(e) == 15){
        if (cut.passAllUpTo(AngColl) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,AngColl,e->weight());

        if(cut.passAllUpTo(AngRbb) ) Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,AngRbb,e->weight());

        if(cut.passAllUpTo(AngRbb) ) Fill("ChargedHiggsTauNu/CutFlowQCD/CutFlowQCD_"+label,systname,3,e->weight());
        
        // ------------------- N-1 SELECTIONS
        if( cut.passAllExcept(Met) )
        {   
            //Fill2D("ChargedHiggsTauNu/NOne/EtMissVsMt_"+label,systname,e->Mt(),e->GetMet().Pt(),e->weight());
            Fill("ChargedHiggsTauNu/NOne/EtMiss_"+label,systname, e->GetMet().Pt() ,e->weight());
        }

        if (cut.passAllExcept( AngRbb) ){
            Fill("ChargedHiggsTauNu/NOne/RbbMin_"+label,systname,RbbMin,e->weight());
            //Fill2D("ChargedHiggsTauNu/NOne/RbbMinVsMt_"+label,systname,e->Mt(),RbbMin,e->weight());
        }

        if (cut.passAllExcept(AngColl) ) {
            Fill("ChargedHiggsTauNu/NOne/RCollMin_"+label,systname,RCollMin,e->weight());
            //Fill2D("ChargedHiggsTauNu/NOne/RCollMinVsMt_"+label,systname,e->Mt(),RCollMin,e->weight());
        }

        if (cut.passAllExcept(ThreeJets) ) 
        {
            Fill("ChargedHiggsTauNu/NOne/NJets_"+label,systname, e->Njets() ,e->weight());
            Jet* j1 = e->LeadJet();
            if (j1 !=NULL ) 
                {        
                    Fill("ChargedHiggsTauNu/NOne/Jet1Pt_"+label,systname, j1->Pt() ,e->weight());
                    Fill("ChargedHiggsTauNu/NOne/Jet1Eta_"+label,systname,j1->Eta() ,e->weight());
                }

            Jet* j2 = e->GetJet(1);
            if (j2 !=NULL ) 
                {        
                    //Fill("ChargedHiggsTauNu/NOne/Jet2Pt_"+label,systname, j2->Pt() ,e->weight());
                    //Fill("ChargedHiggsTauNu/NOne/Jet2Eta_"+label,systname,j2->Eta() ,e->weight());
                }

            Jet* j3 = e->GetJet(3);
            if (j3 !=NULL ) 
                {        
                    //Fill("ChargedHiggsTauNu/NOne/Jet3Pt_"+label,systname, j3->Pt() ,e->weight());
                    //Fill("ChargedHiggsTauNu/NOne/Jet3Eta_"+label,systname,j3->Eta() ,e->weight());
                }
        } // Three jets , n-1

        if( cut.passAllExcept( OneTau) )
        { 
            Fill("ChargedHiggsTauNu/NOne/Tau1Pt_"+label,systname, t->Pt() ,e->weight());
            //Fill("ChargedHiggsTauNu/NOne/Tau1Eta_"+label,systname,t->Eta(),e->weight());
        }
   

        if (cut.passAllExcept(OneBjet) )
        {
            Fill("ChargedHiggsTauNu/NOne/NBjets_"+label,systname, e->Bjets() ,e->weight());
            if( e->GetCentralJet(0)) Fill("ChargedHiggsTauNu/NOne/Bdiscr_"+label,systname, e->GetCentralJet(0)->bdiscr ,e->weight()); // of the leading jet or of the b?
            if (bj1 != NULL) 
                {        
                    Fill("ChargedHiggsTauNu/NOne/Bjet1Pt_"+label,systname, bj1->Pt() ,e->weight());
                    //Fill("ChargedHiggsTauNu/NOne/Bjet1Eta_"+label,systname,bj1->Eta(),e->weight());
                }
        }
    }

    // ------------------------ FULL SELECTION ---------------
    if (cut.passAll() ) 
    {
        e->ApplySF("tauid"); // only in MC

        //Log(__FUNCTION__,"DEBUG",Form("Syst=%s, Weight Before=%le",systname.c_str(),e->weight()));
        e->SetPtEtaSF("tauid2",e->GetTau(0)->Pt(),0.);
        e->ApplySF("tauid2"); // only in MC
        //Log(__FUNCTION__,"DEBUG",Form("Syst=%s, Weight After=%le",systname.c_str(),e->weight()));

        //if(e->IsRealData() and (systname=="NONE" or systname=="")) Log(__FUNCTION__,"SYNC",Form("%d,%d,%ld",e->runNum(),e->lumiNum(),e->eventNum()) );
        //

        if ( Unblind(e) and (e->IsRealData() or e->GetTau(0)->Rematch(e) == 15 )) 
        { 
            Fill("ChargedHiggsTauNu/Vars/Mt_"+label,systname, e->Mt() ,e->weight());
            if (e->Bjets() >1) Fill("ChargedHiggsTauNu/Vars/Mt_cat0_"+label,systname, e->Mt() ,e->weight());
            else  Fill("ChargedHiggsTauNu/Vars/Mt_cat1_"+label,systname, e->Mt() ,e->weight());
        }
        //if ( Unblind(e) ) Fill("ChargedHiggsTauNu/Vars/MtDecoQ_"+label,systname, e->MtDecoQ() ,e->weight());
        //if ( Unblind(e) ) Fill("ChargedHiggsTauNu/Vars/MtDecoCosPhi_"+label,systname, e->MtDecoCosPhi() ,e->weight());

        // -- Book(    "ChargedHiggsTauNu/Vars/Mt_matchTau_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        // -- Book(    "ChargedHiggsTauNu/Vars/Mt_matchEle_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        // -- Book(    "ChargedHiggsTauNu/Vars/Mt_matchMu_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        // -- Book(    "ChargedHiggsTauNu/Vars/Mt_matchJet_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection
        // -- Book(    "ChargedHiggsTauNu/Vars/Mt_matchOther_"+l,"Mt "+l + ";m_{T} [GeV]",8000,0,8000); // the Vars directory contains the full selection

        if (not e->IsRealData() )
        {
            int pdgid=e->GetTau(0)->Rematch(e);
            if (pdgid==15)Fill("ChargedHiggsTauNu/Vars/Mt_matchTau_"+label,systname, e->Mt() ,e->weight());
            else if (pdgid==11)Fill("ChargedHiggsTauNu/Vars/Mt_matchEle_"+label,systname, e->Mt() ,e->weight());
            else if (pdgid==13)Fill("ChargedHiggsTauNu/Vars/Mt_matchMu_"+label,systname, e->Mt() ,e->weight());
            else if (pdgid==21 or pdgid==1 )Fill("ChargedHiggsTauNu/Vars/Mt_matchJet_"+label,systname, e->Mt() ,e->weight());
            else Fill("ChargedHiggsTauNu/Vars/Mt_matchOther_"+label,systname, e->Mt() ,e->weight());
        }

        //Fill("ChargedHiggsTauNu/Vars/Jet1QGL_"+label,systname,e->GetJet(0)->QGL() , e->weight() );
        //Fill("ChargedHiggsTauNu/Vars/Jet2QGL_"+label,systname,e->GetJet(1)->QGL() , e->weight() );
        if (e->IsRealData() or e->GetTau(0)->Rematch(e) == 15) Fill("ChargedHiggsTauNu/Vars/JetInvMass_"+label,systname,e->GetJet(0)->InvMass(e->GetJet(1)) , e->weight() );
        //Fill("ChargedHiggsTauNu/Vars/Jet13InvMass_"+label,systname,e->GetJet(0)->InvMass(e->GetJet(2)) , e->weight() );
        //Fill("ChargedHiggsTauNu/Vars/Jet23InvMass_"+label,systname,e->GetJet(1)->InvMass(e->GetJet(2)) , e->weight() );

        return EVENT_USED;
    }
    else { return EVENT_NOT_USED; }
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
