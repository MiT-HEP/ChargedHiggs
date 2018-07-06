#include "interface/AnalysisQG.hpp"
#include <iostream>

int QGAnalysis::BdtEtaBin(float eta)
{
    float aeta = fabs(eta);
    if (aeta<1.3) return 1;
    else if (aeta<1.5) return 2;
    else if (aeta<1.8) return 3;
    else if (aeta<2.1) return 4;
    else if (aeta<2.4) return 5;
    else if (aeta<2.7) return 6;
    else if (aeta<3.0) return 7;
    else if (aeta<4.7) return 8;
    return -1;
}

int QGAnalysis::BdtPtBin(float pt)
{
    for (int i=1;i<=20 ;++i)
    {
        float min = 20.*std::pow(100, (i-1)/20.);
        float max = 20.*std::pow(100, (i)/20.); 
        if (pt>=min and pt <max) return i;
    }
    return -1;
}

void QGAnalysis::AddVariable( string name, char type){ 
    Log ( __FUNCTION__,"INFO","Adding Variable '"+name+"'");
    varValues_.Add(name,type); 
    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    else { 
        Log(__FUNCTION__,"ERROR","TMVA type not supported");
    }
}//end add variable

int QGAnalysis::Rematch(Event *e, Jet *j, float dR){
    GenParticle * gp =NULL;
    float deltaR=0, geneta=0, genpt=0;
    int ig=0;
    int jet_pdg=0;
    float hardestPt=-1.;
    //bool isHardestQ = false;
    for (gp = e->GetGenParticle(ig) ; gp != NULL ; gp=e->GetGenParticle(++ig))
        {
        
            //cout << "GEN num: " << ig << " pdg id: " << gp->GetPdgId() << " genpt: " << gp->Pt() << " geneta: " << gp->Eta() << " deltaR: " << gp->DeltaR(j) << endl;
        if  (gp->DeltaR(j) >dR) continue;
        if  ( abs(gp->GetPdgId()) > 6 and abs(gp->GetPdgId()) !=21 ) continue; // only quark and gluons
        if  ( abs(gp->GetPdgId()) ==0  ) continue; 

        Log(__FUNCTION__,"DEBUG",Form("%ld: Considering GP pdg=%d pt=%f eta=%f phi=%f",e->eventNum(), gp->GetPdgId(),gp->Pt(), gp->Eta(),gp->Phi() ) ) ;

        if  (gp->DeltaR(j) >dR) continue;
        Log(__FUNCTION__,"DEBUG",Form("       -> Passing dR=%f",dR) ) ;


        //if ( not (gp->GetFlag() & ( BareMonteCarlo::HardProcess | BareMonteCarlo::HardProcessBeforeFSR | BareMonteCarlo::HardProcessDecayed) ) ) continue;
        
        if ( hardestPt <0 or gp->Pt() > hardestPt)
        {
            Log(__FUNCTION__,"DEBUG",Form("       -> is also hardest than previous %f",hardestPt) ) ;
            jet_pdg = gp->GetPdgId();
            hardestPt = gp->Pt();
            deltaR = gp->DeltaR(j);
            genpt = gp->Pt();
            geneta = gp->Eta();
        }

        }
    //cout << "SELECTED GEN: " << " pdg id: " << jet_pdg << " genpt: " << genpt << " geneta: " << geneta <<  " deltaR: " << deltaR << endl;
    return jet_pdg;

}


void QGAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.15);
    l->SetEtaCut(2.4);
    l->SetVetoCut();
    l->SetMediumCut(true);
}

void QGAnalysis::SetJetCuts(Jet *j){
    j->SetBCut(0.8484);
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.4);
    j->SetPtCut(30);
    j->SetPuIdCut(-999);
    //j->SetPuIdCut(0.5);
}

void QGAnalysis::SetTauCuts(Tau *t){ 
    t->SetPtCut(8000);  // remove taus!!! 
    t->SetIsoCut(2.5); 
    t->SetEtaCut(2.1); 
    t->SetMuRej(true); 
    t->SetEleRej(true);
}

void QGAnalysis::SetPhotonCuts(Photon *p){
    p->SetIsoCut(-1); 
    p->SetIsoRelCut(.1);  // quite tight 10%
    p->SetPtCut(20);
}

void QGAnalysis::Init(){
    if (doMM) InitMM();
    if (doJJ) InitJJ();
    InitTmva();
    qgLikelihood_ . reset ( new QGLikelihoodCalculator("/afs/cern.ch/user/s/strolog/public/forGiorgia/pdfQG_AK4chs_13TeV_v1_94X_ghosts.root") ) ;
    return;
}

void QGAnalysis::FillMapAndWeights(string dir){
    for(int ipt=1;ipt<=20;ipt++) 
    for(int ieta=1;ieta<=8;ieta++)
        {
        binmap[ pair<int,int>(ipt,ieta) ] = weights.size();
        weights.push_back(dir + Form("/TMVAClassification_pt%d_eta%d_BDTG.weights.xml",ipt,ieta));
        }
}

void QGAnalysis::InitTmva(){
    FillMapAndWeights("/afs/cern.ch/user/y/ytakahas/public/forQGtagger/");

    TMVA::Tools::Instance();
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );
    AddVariable("axis2",'F');    // 
    AddVariable("axis1",'F');    // 
    AddVariable("ptD",'F');    // 
    AddVariable("pt_dr_log/pt",'F');    // pt_dr_log/pt
    AddVariable("charged_multiplicity",'F');    // Int* -> this function is deprecated, please provide all variables to the reader as floats

    for( size_t i=0;i<weights.size() ;++i)
    {
        Log(__FUNCTION__,"INFO","Loading weights from '" + weights[i]+"'" );
        ifstream f(weights[i].c_str()); 
        if (f.good())
            readers_[i]->BookMVA("BDTG",weights[i].c_str());
        else{
            Log(__FUNCTION__,"INFO"," -> Weight file '" + weights[i]+"' doesn't exist" );
            delete readers_[i];
            readers_[i] = NULL;
        }
    }
}

void QGAnalysis::InitMM(){

    Log(__FUNCTION__,"INFO","Booking Histos for Zmm");
    
    for ( string l : AllLabel()  ) {

        Book ("QGAnalysis/CutFlow/CutFlowZmm_"+ l ,"CutFlow;Events",10,-.5,10-.5);
        Book ("QGAnalysis/Zmm/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
        // 
        Book ("QGAnalysis/Zmm/Ptmm_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
        Book ("QGAnalysis/Zmm/Npv_"+ l ,"Npvmm", 50,0,50);
        Book("QGAnalysis/Zmm/nEvent_"+ l,"Event Number", 1000000, 0, 1000000) ;
        for (const string& t : jetTypes ) {
            for (const string& v : jetVars ){

                if ( l == "Data" and t != "U") continue; // Data have no MC info

                float xmin=0., xmax=1.0;
                int nbins=200;

                if ( v == "axis2" || v == "axis1") { xmin=0; xmax=10;}
                if ( v == "mult") { xmin=0; xmax=50;nbins=50;}
                if ( v == "cmult" || v == "nmult") { xmin=0; xmax=50;nbins=50;}
                if ( v == "PtDrLog" ) {xmin=0; xmax=1.5;}

                Book ("QGAnalysis/Zmm/"+v +"_"+ t+"_"+ l ,v+"_" + t, 200, xmin, xmax);
                //Book ("QGAnalysis/Zmm/BDT_"+t+"_"+ l ,"BDT_" + t, 200,-1,1.0);
                Book ("QGAnalysis/Zmm/QGL_"+t+"_"+ l ,"QGL_" + t, 200,0.,1.0);
                Book ("QGAnalysis/Zmm/QGL_n_"+t+"_"+ l ,"QGL_n_" + t, 200,0.,1.0);
                for(int ptb=0;ptb < ptBins.size()-1; ++ptb)         
                for(int aetab=0;aetab < aetaBins.size()-1; ++aetab)
                {
                    //cout << "QGAnalysis/Zmm/QGL_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l << endl;
                    //if(t=="U" && l=="DY") { cout  << "====>UNDEFINED WITH PT" << ptBins[ptb] << "AND ETA" << aetaBins[aetab] << endl;}

                    Book ("QGAnalysis/Zmm/"+v+"_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,v +"_" + t, nbins,xmin,xmax);
                    //Book ("QGAnalysis/Zmm/BDT_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"BDT_" + t, nbins,-1.,1.);
                    Book ("QGAnalysis/Zmm/QGL_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"QGL_" + t, nbins,0.,1.);
                    Book ("QGAnalysis/Zmm/QGL_n_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"QGL_n_" + t, nbins,0.,1.);

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

                float xmin=0., xmax=1.0;
                int nbins=200;

                if ( v == "axis2" || v == "axis1") { xmin=0; xmax=10;}
                if ( v == "mult" ) { xmin=0; xmax=100;nbins=100;}
                if ( v == "cmult" || v == "nmult") { xmin=0; xmax=50;nbins=50;}
                if ( v == "PtDrLog" ) {xmin=0; xmax=1.5;}

                Book ("QGAnalysis/DiJet/"+v +"_"+ t+"_"+ l ,v+"_" + t, 200,xmin,xmax);
                //Book ("QGAnalysis/DiJet/BDT_"+t+"_"+ l ,"BDT_" + t, 200,-1,1.0);
                Book ("QGAnalysis/DiJet/QGL_n_"+t+"_"+ l ,"QGL_n_" + t, 200,0.,1.0);
                Book ("QGAnalysis/DiJet/QGL_"+t+"_"+ l ,"QGL_" + t, 200,0.,1.0);

                for(int ptb=0;ptb < ptBins.size()-1; ++ptb)         
                for(int aetab=0;aetab < aetaBins.size()-1; ++aetab)
                {
                    Book ("QGAnalysis/DiJet/"+v+"_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,v +"_" + t, nbins,xmin,xmax);
                    //Book ("QGAnalysis/DiJet/BDT_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"BDT_" + t, nbins,-1.,1.);
                    Book ("QGAnalysis/DiJet/QGL_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"QGL_" + t, nbins,0.,1.);
                    Book ("QGAnalysis/DiJet/QGL_n_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,"QGL_n_" + t, nbins,0.,1.);
                } // pt-eta
            } // vars
        } // types
    } // labels

}

int QGAnalysis::analyzeMM(Event *e, string systname)
{
    if ( e->IsRealData() and e->GetName().find("SingleMuon") == string::npos )  return 0; // avoid data double counting
    //if ( e->IsRealData() and e->runNum() <= 278801) return 0; //split B2F and FGH runs
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

    Fill("QGAnalysis/Zmm/nEvent_"+ label,systname, e->eventNum(),e->weight()) ;
    
    if (cut.passAll() ){

        Fill("QGAnalysis/CutFlow/CutFlowZmm_"+label,systname,MaxCut,e->weight());
        Fill("QGAnalysis/Zmm/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
        Fill("QGAnalysis/Zmm/Npv_"+ label,systname, e->Npv(),e->weight()) ;
        string etaStr=Binning::findBinStr( aetaBins, fabs(j0->Eta()), "eta%.1f_%.1f");
        //string ptStr=Binning::findBinStr( ptBins, j0->Pt(), "pt%.0f_%.0f"); // binned with Jet PT
        string ptStr=Binning::findBinStr( ptBins, Z.Pt(), "pt%.0f_%.0f"); // binned with Z PT
        int flavor = abs(j0->Flavor()); // parton flavor

        //int flavor = Rematch(e,j0,0.4); // rematching with gen particles. See above.
        string type="U";
        if (flavor == 21) type="G";
        else if (flavor <= 6 and flavor != 0) type="Q"; 
        
        //cout << "id: " << type << " pt: " << j0->Pt() << " eta: " << j0->Eta() << " phi: " << j0->Phi() << " Zpt: " << Z.Pt() << endl;
        //cout << "------------------------------------" << endl;
        SetVariable("axis1",-TMath::Log(j0->QGLVar("axis1")));
        SetVariable("axis2",-TMath::Log(j0->QGLVar("axis2")));
        SetVariable("ptD",j0->QGLVar("ptD"));
        SetVariable("pt_dr_log/pt",j0->QGLVar("PtDrLog")/j0->Pt());
        SetVariable("charged_multiplicity",j0->QGLVar("cmult"));
        //SetVariable("mult",j0->QGLVar("mult"));
        
        /*SetVariable("axis1", 3.4148640);
        SetVariable("axis2", 4.6128964);                                                                                                          
        SetVariable("ptD", 0.7207920);                                                                                                                
        SetVariable("pt_dr_log/pt",  0.570370);                                                                                             
        SetVariable("charged_multiplicity", 2); */

        // Find right idx for bdt
        float bdt=-999;
        int idx=-1;
        if ( binmap.find(pair<int,int>(BdtPtBin(j0->Pt()),BdtEtaBin(j0->Eta()))) != binmap.end() )
            idx = binmap[ pair<int,int>(BdtPtBin(j0->Pt()),BdtEtaBin(j0->Eta()))];
        if(idx >=0 and readers_[idx] != NULL ) bdt = readers_[idx] ->EvaluateMVA("BDTG");
        
        float mult = j0->QGLVar("mult");
        float ptD = j0->QGLVar("ptD");
        float axis2=-TMath::Log(j0->QGLVar("axis2"));
        float newQGL = -1;
        if (mult >2 ) newQGL = qgLikelihood_ -> computeQGLikelihood( j0->Pt(), abs(j0->Eta() ),  e->Rho() , {mult, ptD, axis2} );

        //cout << bdt << endl;
        // global 
        
        //Fill("QGAnalysis/Zmm/BDT_" +type +"_"+ label, systname, bdt,e->weight()) ;
        Fill("QGAnalysis/Zmm/QGL_" +type +"_"+ label, systname, j0->QGL(),e->weight()) ;        
        Fill("QGAnalysis/Zmm/QGL_n_" +type +"_"+ label, systname, newQGL,e->weight()) ;        
        Fill("QGAnalysis/Zmm/mult_" +type +"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
        //Fill("QGAnalysis/Zmm/cmult_" +type +"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
        //Fill("QGAnalysis/Zmm/nmult_" +type +"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
        Fill("QGAnalysis/Zmm/ptD_" +type +"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
        //Fill("QGAnalysis/Zmm/PtDrLog_" +type +"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
        Fill("QGAnalysis/Zmm/axis2_" +type +"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
        //Fill("QGAnalysis/Zmm/axis1_" +type +"_"+ label, systname, -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;

        // binned
        if (etaStr != "NotFound" and ptStr != "NotFound")
        {
            //cout <<" Filling QGAnalysis/Zmm/QGL_" +type +"_" + ptStr + "_" + etaStr+"_"+ label<< endl;
            //if(type=="U" && label=="DY") { cout  << "====>FILLING UNDEFINED WITH PT: " << ptStr << endl;}
            /*COMMENT ME*/
            //Log(__FUNCTION__,"DEBUG",Form("Filling eta=%s, pt=%s, cmult=%.1f",etaStr.c_str(),ptStr.c_str(), j0->QGLVar("cmult")) );
            /**/
            //Fill("QGAnalysis/Zmm/BDT_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, bdt ,e->weight()) ;
            Fill("QGAnalysis/Zmm/QGL_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGL(),e->weight()) ;
            Fill("QGAnalysis/Zmm/QGL_n_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, newQGL,e->weight()) ;
            Fill("QGAnalysis/Zmm/mult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
            //Fill("QGAnalysis/Zmm/cmult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
            //Fill("QGAnalysis/Zmm/nmult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
            Fill("QGAnalysis/Zmm/ptD_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
            //Fill("QGAnalysis/Zmm/PtDrLog_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
            Fill("QGAnalysis/Zmm/axis2_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
            //Fill("QGAnalysis/Zmm/axis1_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;
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
        //int flavorJ0 = Rematch(e,j0,0.4);
        string typeJ0="U";
        if (flavorJ0 == 21) typeJ0="G";
        else if (flavorJ0 < 6 and flavorJ0 != 0) typeJ0="Q"; 

        int flavorJ1 = abs(j1->Flavor());
        //int flavorJ1 = Rematch(e,j1,0.4);
        string typeJ1="U";
        if (flavorJ1 == 21) typeJ1="G";
        else if (flavorJ1 < 6 and flavorJ1 != 0) typeJ1="Q";            

        float bdt_0=-999., bdt_1=-999.;                                                                                                                                                                 
        int idx_0=-1, idx_1=-1;                                                                                                                                                                       
        if ( binmap.find(pair<int,int>(BdtPtBin(j0->Pt()),BdtEtaBin(j0->Eta()))) != binmap.end() )                   
            idx_0 = binmap[ pair<int,int>(BdtPtBin(j0->Pt()),BdtEtaBin(j0->Eta()))];                                                                                                                  
        if ( binmap.find(pair<int,int>(BdtPtBin(j1->Pt()),BdtEtaBin(j1->Eta()))) != binmap.end() )  
            idx_1 = binmap[ pair<int,int>(BdtPtBin(j1->Pt()),BdtEtaBin(j1->Eta()))];                                                                                                                  

        SetVariable("axis1",-TMath::Log(j0->QGLVar("axis1")));
        SetVariable("axis2",-TMath::Log(j0->QGLVar("axis2")));
        SetVariable("ptD",j0->QGLVar("ptD"));
        SetVariable("pt_dr_log/pt",j0->QGLVar("PtDrLog")/j0->Pt());
        SetVariable("charged_multiplicity",j0->QGLVar("cmult"));
        //SetVariable("mult",j0->QGLVar("mult"));

        if(idx_0 >=0 and readers_[idx_0] != NULL ) bdt_0 = readers_[idx_0] ->EvaluateMVA("BDTG");

        SetVariable("axis1",-TMath::Log(j1->QGLVar("axis1")));
        SetVariable("axis2",-TMath::Log(j1->QGLVar("axis2")));
        SetVariable("ptD",j1->QGLVar("ptD"));
        SetVariable("pt_dr_log/pt",j1->QGLVar("PtDrLog")/j1->Pt());
        SetVariable("charged_multiplicity",j1->QGLVar("cmult"));
        //SetVariable("mult",j1->QGLVar("mult"));

        if(idx_1 >=0 and readers_[idx_1] != NULL ) bdt_1 = readers_[idx_1] ->EvaluateMVA("BDTG");

        float mult = j0->QGLVar("mult");
        float ptD = j0->QGLVar("ptD");
        float axis2=-TMath::Log(j0->QGLVar("axis2"));

        float newQGL_0 = -1;
        if (mult >2 ) newQGL_0 = qgLikelihood_ -> computeQGLikelihood( j0->Pt(), abs(j0->Eta() ),  e->Rho() , {mult, ptD, axis2} );

        mult = j1->QGLVar("mult");
        ptD = j1->QGLVar("ptD");
        axis2=-TMath::Log(j1->QGLVar("axis2"));
        float newQGL_1 = -1;
        if (mult >2 ) newQGL_1 = qgLikelihood_ -> computeQGLikelihood( j1->Pt(), abs(j1->Eta() ),  e->Rho() , {mult, ptD, axis2} );
        //cout << bdt_1 << endl;

        //cout << " Jet Pt: " << j1->Pt() << " Jet Eta: " << j1->Eta() << " PtDrLog: " << j1->QGLVar("PtDrLog")/j1->Pt()  << " axis2: " <<  -TMath::Log(j1->QGLVar("axis2")) << " axis1: " <<  -TMath::Log(j1->QGLVar("axis1"))  << " ptD: " << j1->QGLVar("ptD") << endl;

        // global 

        //Fill("QGAnalysis/DiJet/BDT_" +typeJ0 +"_"+ label, systname, bdt_0, e->weight()) ;
        //Fill("QGAnalysis/DiJet/BDT_" +typeJ1 +"_"+ label, systname, bdt_1, e->weight()) ;

        Fill("QGAnalysis/DiJet/QGL_" +typeJ0 +"_"+ label, systname, j0->QGL(),e->weight()) ;
        Fill("QGAnalysis/DiJet/QGL_" +typeJ1 +"_"+ label, systname, j1->QGL(),e->weight()) ;

        Fill("QGAnalysis/DiJet/QGL_n_" +typeJ0 +"_"+ label, systname, newQGL_0,e->weight()) ;
        Fill("QGAnalysis/DiJet/QGL_n_" +typeJ1 +"_"+ label, systname, newQGL_1,e->weight()) ;

        Fill("QGAnalysis/DiJet/mult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/cmult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/nmult_" +typeJ0 +"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/ptD_" +typeJ0 +"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/PtDrLog_" +typeJ0 +"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis2_" +typeJ0 +"_"+ label, systname,  -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
        //Fill("QGAnalysis/DiJet/axis1_" +typeJ0 +"_"+ label, systname,  -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;

        Fill("QGAnalysis/DiJet/mult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("mult"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/nmult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("nmult"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/cmult_" +typeJ1 +"_"+ label, systname, j1->QGLVar("cmult"),e->weight()) ;
        Fill("QGAnalysis/DiJet/ptD_" +typeJ1 +"_"+ label, systname, j1->QGLVar("ptD"),e->weight()) ;
        //Fill("QGAnalysis/DiJet/PtDrLog_" +typeJ1 +"_"+ label, systname, j1->QGLVar("PtDrLog")/j1->Pt(),e->weight()) ;
        Fill("QGAnalysis/DiJet/axis2_" +typeJ1 +"_"+ label, systname,  -TMath::Log(j1->QGLVar("axis2")),e->weight()) ;
        //Fill("QGAnalysis/DiJet/axis1_" +typeJ1 +"_"+ label, systname,  -TMath::Log(j1->QGLVar("axis1")),e->weight()) ;
        // binned
        
        // pt1 <-> pt0: TP
        if (etaStrJ0 != "NotFound" and ptStrJ1 != "NotFound")
        {

            //Fill("QGAnalysis/DiJet/BDT_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, bdt_0,e->weight()) ;
            Fill("QGAnalysis/DiJet/QGL_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGL(),e->weight()) ;
            Fill("QGAnalysis/DiJet/QGL_n_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, newQGL_0,e->weight()) ;
            //Fill("QGAnalysis/DiJet/nmult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("nmult"),e->weight()) ;
            //Fill("QGAnalysis/DiJet/cmult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("cmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/mult_" +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/ptD_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
            //Fill("QGAnalysis/DiJet/PtDrLog_"  +typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, j0->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis2_"+typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
            //Fill("QGAnalysis/DiJet/axis1_"+typeJ0 +"_" + ptStrJ1 + "_" + etaStrJ0+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis1")),e->weight()) ;

        }

        if (etaStrJ1 != "NotFound" and ptStrJ0 != "NotFound")
        {
            //Fill("QGAnalysis/DiJet/BDT_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, bdt_1,e->weight()) ;
            Fill("QGAnalysis/DiJet/QGL_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGL(),e->weight()) ;
            Fill("QGAnalysis/DiJet/QGL_n_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, newQGL_1,e->weight()) ;
            Fill("QGAnalysis/DiJet/mult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("mult"),e->weight()) ;
            //Fill("QGAnalysis/DiJet/nmult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("nmult"),e->weight()) ;
            //Fill("QGAnalysis/DiJet/cmult_" +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("cmult"),e->weight()) ;
            Fill("QGAnalysis/DiJet/ptD_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("ptD"),e->weight()) ;
            //Fill("QGAnalysis/DiJet/PtDrLog_"  +typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, j1->QGLVar("PtDrLog")/j0->Pt(),e->weight()) ;
            Fill("QGAnalysis/DiJet/axis2_"+typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, -TMath::Log(j1->QGLVar("axis2")),e->weight()) ;
            //Fill("QGAnalysis/DiJet/axis1_"+typeJ1 +"_" + ptStrJ0 + "_" + etaStrJ1+"_"+ label, systname, -TMath::Log(j1->QGLVar("axis1")),e->weight()) ;
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
