
#include "interface/AnalysisQG.hpp"

int QGAnalysis::Rematch(Event *e, Jet *j, float dR){
    GenParticle * gp =NULL;
    int ig=0;
    int jet_pdg=0;
    float hardestPt=-1.;
    //bool isHardestQ = false;
    for (gp = e->GetGenParticle(ig) ; gp != NULL ; gp=e->GetGenParticle(++ig))
    {
        if  (gp->DeltaR(j) >dR) continue;
        if  ( abs(gp->GetPdgId()) > 6 and abs(gp->GetPdgId()) !=21 ) continue; // only quark and gluons
        if  ( abs(gp->GetPdgId()) ==0  ) continue; 

        //if ( not (gp->GetFlag() & ( BareMonteCarlo::HardProcess | BareMonteCarlo::HardProcessBeforeFSR | BareMonteCarlo::HardProcessDecayed) ) ) continue;

        if ( hardestPt <0 or gp->Pt() > hardestPt)
        {
            jet_pdg = gp->GetPdgId();
            hardestPt = gp->Pt();
        }

    }
    return jet_pdg;

}


void QGAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.15);
    l->SetEtaCut(2.4);
}

void QGAnalysis::SetJetCuts(Jet *j){
    j->SetBCut(0.8484);
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(30);
    //j->SetPuIdCut(0.5);
}

void QGAnalysis::Init(){
    if (doMM) InitMM();
    if (doJJ) InitJJ();
    return;
}

void QGAnalysis::InitMM(){

    Log(__FUNCTION__,"INFO","Booking Histos for Zmm");

    for ( string l : AllLabel()  ) {
        Book ("QGAnalysis/CutFlow/CutFlowZmm_"+ l ,"CutFlow;Events",10,-.5,10-.5);
        Book ("QGAnalysis/Zmm/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
        // 
        Book ("QGAnalysis/Zmm/Ptmm_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
        //Book ("QGAnalysis/Zmm/Met_"+ l ,"Met;MET [GeV];Events", 1000,0,1000);
        Book ("QGAnalysis/Zmm/Npv_"+ l ,"Npvmm", 50,0,50);
        //
        for (const string& t : jetTypes ) {
            for (const string& v : jetVars ){

                if ( l == "Data" and t != "U") continue; // Data have no MC info

                float xmin=0., xmax=1.0;
                int nbins=200;

                if ( v == "axis2" || v == "axis1") { xmin=0; xmax=10;}
                if ( v == "mult") { xmin=0; xmax=100;nbins=100;}
                if ( v == "cmult" || v == "nmult") { xmin=0; xmax=50;nbins=50;}
                if ( v == "PtDrLog" ) {xmin=0; xmax=1.5;}

                Book ("QGAnalysis/Zmm/"+v +"_"+ t+"_"+ l ,v+"_" + t, 200,-1,1.0);
                for(int ptb=0;ptb < ptBins.size()-1; ++ptb)         
                for(int aetab=0;aetab < aetaBins.size()-1; ++aetab)
                {
                    Book ("QGAnalysis/Zmm/"+v+"_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,v +"_" + t, nbins,xmin,xmax);
                } // pt-eta
            } // vars
        } // types
    } // labels

}

void QGAnalysis::InitJJ(){

    Log(__FUNCTION__,"INFO","Booking Histos for DiJet");

    for ( string l : AllLabel()  ) {
        Book ("QGAnalysis/CutFlow/CutFlowDiJet_"+ l ,"CutFlow;Events",10,-.5,10-.5);
        // 
        Book ("QGAnalysis/DiJet/Npv_"+ l ,"Npv jj", 50,0,50);
        //
        for (const string& t : jetTypes ) {
            for (const string& v : jetVars ){

                if ( l == "Data" and t != "U") continue; // Data have no MC info

                float xmin=-1, xmax=1.0;
                int nbins=200;

                if ( v == "axis2" || v == "axis1") { xmin=0; xmax=10;}
                if ( v == "mult" ) { xmin=0; xmax=100;nbins=100;}
                if ( v == "cmult" || v == "nmult") { xmin=0; xmax=50;nbins=50;}
                if ( v == "PtDrLog" ) {xmin=0; xmax=1.5;}

                Book ("QGAnalysis/DiJet/"+v +"_"+ t+"_"+ l ,v+"_" + t, 200,-1,1.0);
                for(int ptb=0;ptb < ptBins.size()-1; ++ptb)         
                for(int aetab=0;aetab < aetaBins.size()-1; ++aetab)
                {
                    Book ("QGAnalysis/DiJet/"+v+"_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,v +"_" + t, nbins,xmin,xmax);
                } // pt-eta
            } // vars
        } // types
    } // labels

}

int QGAnalysis::analyzeMM(Event *e, string systname)
{

    if ( e->IsRealData() and e->GetName().find("DoubleMuon") == string::npos )  return 0; // avoid data double counting
    if ( e->IsRealData() and e->runNum() <= 278801) return 0; //split B2F and FGH runs
    //Log(__FUNCTION__,"DEBUG","Analyzing");

    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;
    Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,Total,e->weight());

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    if (mu0 == NULL or mu1 == NULL) return 0;

    cut.SetCutBit(Leptons);
    if (e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v") ) cut.SetCutBit(Trigger);
    Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,Trigger,e->weight());

    Object Z(*mu0);
    Z += *mu1;
    if (cut.passAllUpTo(Trigger) ){
        Fill("QGAnalysis/Zmm/Mmm_"+ label,systname, Z.M(),e->weight()) ;
    }

    if (Z.M()> 70 && Z.M()<110) cut.SetCutBit(Mass);

    Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,Mass,e->weight());

    if ( j0 == NULL ) return 0; // 1 Jet is needed

    if ( j1 == NULL ) cut.SetCutBit(OneJet);
    if ( j1 != NULL and j1->Pt() < Z.Pt() * 0.3 ) cut.SetCutBit(OneJet);

    Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,OneJet,e->weight());
    if ( j0->DeltaPhi(Z) > 2.5 ) cut.SetCutBit(DeltaPhi);

    Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,DeltaPhi,e->weight());

    if (cut.passAll() ){
        Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,MaxCut,e->weight());
        Fill("QGAnalysis/Zmm/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
        //Fill("QGAnalysis/Zmm/Metmm_"+ label,systname, Z.Pt(),e->weight()) ;
        Fill("QGAnalysis/Zmm/Npv_"+ label,systname, e->Npv(),e->weight()) ;
        string etaStr=Binning::findBinStr( aetaBins, fabs(j0->Eta()), "eta%.1f_%.1f");
        //string ptStr=Binning::findBinStr( ptBins, j0->Pt(), "pt%.0f_%.0f"); // binned with Jet PT
        string ptStr=Binning::findBinStr( ptBins, Z.Pt(), "pt%.0f_%.0f"); // binned with Z PT
        //int flavor = abs(j0->Flavor()); // parton flavor
        int flavor = Rematch(e,j0,0.4); // rematching with gen particles. See above.
        string type="U";
        if (flavor == 21) type="G";
        else if (flavor <= 6 and flavor != 0) type="Q"; 
           
        // global 
        Fill("QGAnalysis/Zmm/QGL_" +type +"_"+ label, systname, j0->QGL(),e->weight()) ;        
        Fill("QGAnalysis/Zmm/mult_" +type +"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
        Fill("QGAnalysis/Zmm/cmult_" +type +"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
        Fill("QGAnalysis/Zmm/nmult_" +type +"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
        Fill("QGAnalysis/Zmm/ptD_" +type +"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
        Fill("QGAnalysis/Zmm/PtDrLog_" +type +"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
        Fill("QGAnalysis/Zmm/axis2_" +type +"_"+ label, systname, j0->QGLVar("axis2"),e->weight()) ;
        Fill("QGAnalysis/Zmm/axis1_" +type +"_"+ label, systname, j0->QGLVar("axis1"),e->weight()) ;
        // binned
        if (etaStr != "NotFound" and ptStr != "NotFound")
        {
            /*COMMENT ME*/
            //Log(__FUNCTION__,"DEBUG",Form("Filling eta=%s, pt=%s, cmult=%.1f",etaStr.c_str(),ptStr.c_str(), j0->QGLVar("cmult")) );
            /**/
            
            Fill("QGAnalysis/Zmm/QGL_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGL(),e->weight()) ;
            Fill("QGAnalysis/Zmm/mult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
            Fill("QGAnalysis/Zmm/cmult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
            Fill("QGAnalysis/Zmm/nmult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
            Fill("QGAnalysis/Zmm/ptD_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
            Fill("QGAnalysis/Zmm/PtDrLog_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
            Fill("QGAnalysis/Zmm/axis2_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
            Fill("QGAnalysis/Zmm/axis1_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;
        }

    }

    return 0;
}

int QGAnalysis::analyzeJJ(Event *e, string systname)
{
    if ( e->IsRealData() and e->GetName().find("ZeroBias") == string::npos )  return 0; // avoid data double counting


    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(DiJetMaxCut-1) ;
    cut.SetCutBit( DiJetTotal ) ;
    Fill("QGAnalysis/CutFlow/CutFlowDiJet_"+label,systname,DiJetTotal,e->weight());

    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);
    Jet *j2 = e->GetJet(2);


    if ( j0 == NULL or j1 == NULL) return 0;
    if ( j2 == NULL  or 2.*j2->Pt()/( j1->Pt() + j0->Pt() ) ) cut.SetCutBit(DiJetTwoJets) ;

    if (cut.passAllUpTo(DiJetTwoJets) ){
        Fill("QGAnalysis/CutFlow/CutFlowDiJet_" + label,systname,DiJetTwoJets, e->weight() ) ;
    }

    if ( j0->DeltaPhi(*j1) > 2.5 ) cut.SetCutBit(DiJetDeltaPhi);

    if( cut.passAllUpTo(DiJetTwoJets) ) Fill("QGAnalysis/CutFlow/CutFlowDiJet_"+label,systname,DiJetDeltaPhi,e->weight());

    if (cut.passAll() ){
        Fill("QGAnalysis/CutFlow/CutFlowDiJet_"+label,systname,DiJetMaxCut,e->weight());
        Fill("QGAnalysis/DiJet/Npv_"+ label,systname, e->Npv(),e->weight()) ;
        string etaStrJ0=Binning::findBinStr( aetaBins, fabs(j0->Eta()), "eta%.1f_%.1f");
        string etaStrJ1=Binning::findBinStr( aetaBins, fabs(j1->Eta()), "eta%.1f_%.1f");
        LogN(__FUNCTION__,"WARNING","Not T&P diJet",10);
        string ptStrJ0=Binning::findBinStr( ptBins, j0->Pt(), "pt%.0f_%.0f"); 
        string ptStrJ1=Binning::findBinStr( ptBins, j1->Pt(), "pt%.0f_%.0f"); 
        int flavorJ0 = abs(j0->Flavor());

        string typeJ0="U";
        if (flavorJ0 == 21) typeJ0="G";
        else if (flavorJ0 < 6 and flavorJ0 != 0) typeJ0="Q"; 

        int flavorJ1 = abs(j1->Flavor());
        string typeJ1="U";
        if (flavorJ1 == 21) typeJ1="G";
        else if (flavorJ1 < 6 and flavorJ1 != 0) typeJ1="Q"; 
           
        // global 
        Fill("QGAnalysis/DiJet/QGL_" +typeJ0 +"_"+ label, systname, j0->QGL(),e->weight()) ;
        Fill("QGAnalysis/DiJet/QGL_" +typeJ1 +"_"+ label, systname, j1->QGL(),e->weight()) ;
         
        Fill("QGAnalysis/DiJet/mult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/cmult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/nmult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/ptD_" +typeJ0 +"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
        Fill("QGAnalysis/DiJet/PtDrLog_" +typeJ0 +"_"+ label, systname, j0->QGLVar("PtDrLog"),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis2_" +typeJ0 +"_"+ label, systname, j0->QGLVar("axis2"),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis1_" +typeJ0 +"_"+ label, systname, j0->QGLVar("axis1"),e->weight()) ;

        Fill("QGAnalysis/DiJet/mult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("mult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/nmult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("nmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/cmult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("cmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/ptD_" +typeJ1 +"_"+ label, systname, j1->QGLVar("ptD"),e->weight()) ;
        Fill("QGAnalysis/DiJet/PtDrLog_" +typeJ1 +"_"+ label, systname, j1->QGLVar("PtDrLog"),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis2_" +typeJ1 +"_"+ label, systname, j1->QGLVar("axis2"),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis1_" +typeJ1 +"_"+ label, systname, j1->QGLVar("axis1"),e->weight()) ;
        // binned
        
        // pt1 <-> pt0: TP
        if (etaStrJ0 != "NotFound" and ptStrJ1 != "NotFound")
        {
            Fill("QGAnalysis/DiJet/QGL_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGL(),e->weight()) ;
            Fill("QGAnalysis/DiJet/nmult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/cmult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/mult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/ptD_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
            Fill("QGAnalysis/DiJet/PtDrLog_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("PtDrLog"),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis2_"+typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis1_"+typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;

        }

        if (etaStrJ1 != "NotFound" and ptStrJ0 != "NotFound")
        {
            Fill("QGAnalysis/DiJet/QGL_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGL(),e->weight()) ;
            Fill("QGAnalysis/DiJet/mult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("mult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/nmult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("nmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/cmult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("cmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/ptD_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("ptD"),e->weight()) ;
            Fill("QGAnalysis/DiJet/PtDrLog_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("PtDrLog"),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis2_"+typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, -TMath::Log(j1->QGLVar("axis2")),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis1_"+typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, -TMath::Log(j1->QGLVar("axis1")),e->weight()) ;
        }

    }

    return 0;
}


int QGAnalysis::analyze(Event *e, string systname)
{

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");
    if (doMM) analyzeMM(e,systname);
    if (doJJ) analyzeJJ(e,systname);
    return 0;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
