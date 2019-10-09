#include "interface/AnalysisDY.hpp"
#define DEEP_B_LOOSE ((year==2016)?0.2217:(year==2017)?0.1522:0.1241)
#define DEEP_B_MEDIUM ((year==2016)?0.6321:(year==2017)?0.4941:0.4148)
#define DEEP_B_TIGHT ((year==2016)?0.8953:(year==2017)?.8001:.7527)

void DYAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10);
    l->SetIsoRelCut(0.15);
    l->SetEtaCut(2.4); 
    l->SetTightCut(false);
    l->SetMediumCut(true);
}
void DYAnalysis::SetJetCuts(Jet *j) { 
    j->SetBCut(-100);
    j->SetEtaCut(2.0); 
    j->SetEtaCutCentral(2.0);
    j->SetPtCut(30);
    j->SetPuIdCut(100);
    if (year==2017) j->SetEENoiseCut(true);
    else  j->SetEENoiseCut(false);
    j->SetDeepBCut(DEEP_B_MEDIUM); 
} 

void DYAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
	    Book ("DYAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
	    Book ("DYAnalysis/Vars/Ptmm_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
	    Book ("DYAnalysis/Vars/NJmm_"+ l ,"NJmm;N_{jets}^{#mu#mu};Events", 10,0,10);
	    Book ("DYAnalysis/Vars/Npvmm_"+ l ,"Npvmm", 50,0,50);
        vector<int> jbin{500,1000,1500,2000,10000};
        vector<string> flavours={"G","Q","O","U"};
        for (int i=0;i<jbin.size()-1;++i)
        {
	        Book ( Form("DYAnalysis/Vars/axis2_jet%d_%d_",jbin[i],jbin[i+1])+ l ,"Npvmm", 100,0,10);
	        Book ( Form("DYAnalysis/Vars/mult_jet%d_%d_",jbin[i],jbin[i+1])+ l ,"Npvmm", 500,0,500);
	        Book ( Form("DYAnalysis/Vars/qgl_jet%d_%d_",jbin[i],jbin[i+1])+ l ,"Npvmm", 500,0,1);
            for(auto fl : flavours)
            {
	            Book ( Form("DYAnalysis/Vars/qgl_%s_jet%d_%d_",fl.c_str(),jbin[i],jbin[i+1])+ l ,"Npvmm", 500,0,1);
	            Book ( Form("DYAnalysis/Vars/axis2_%s_jet%d_%d_",fl.c_str(),jbin[i],jbin[i+1])+ l ,"Npvmm", 100,0,10);
	            Book ( Form("DYAnalysis/Vars/mult_%s_jet%d_%d_",fl.c_str(),jbin[i],jbin[i+1])+ l ,"Npvmm", 500,0,500);
            }

        }
    }

}

int DYAnalysis::analyzeMM(Event *e, string systname)
{
    string label = GetLabel(e);

    mu0 = e->GetMuon(0);
    mu1 = e->GetMuon(1);

    j0 = e->GetJet(0);
    j1 = e->GetJet(1); // UnUsed

    
    // Truth
    GenParticle *genmu0=NULL; GenParticle *genmu1=NULL;

    for( int iGen=0 ; /*empty*/ ; ++iGen)
    {
        GenParticle *g = e->GetGenParticle(iGen); 
        if (g==NULL ) break;  // end loop statement
        //if (not g->IsDressed()) continue;
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

    /*
     * L1 PreFiring Map
     */

    //#warning NO_PREFIRE
    double l1prefire=1.0;
    if(year==2017)l1prefire=e->ApplyL1PreFire(2017); // apply to the event weight, but keep it for comp
    else if(year==2016)l1prefire=e->ApplyL1PreFire(2016); // apply to the event weight, but keep it for comp

    /*
     * BTag
     */
    double btagsf=1; 
    btagsf=e->ApplyBTagSF(3,year); //0 loose, 1 medium, 2 tight, 3 reshaping
    /*
     * Muon SF
     */
    ApplyMuonSF(e,systname);
    
    float ptcut0=29, ptcut1=20;
    bool passAsymmPtCuts = (recoMuons and  mu0->Pt() >ptcut0 and mu1->Pt() >ptcut1 );

    bool passTrigger=false;
    if (year==2016) {passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); }
    else if (year==2017) {passTrigger=e->IsTriggered("HLT_IsoMu27_v") or e->IsTriggered("HLT_IsoTkMu27_v"); }
    else if (year==2018) {passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); }

    bool passTrigger1 = false; 
    bool passTrigger2 = false;

    if (recoMuons) 
    {
        bool passTriggerEvent = passTrigger;
        if (year==2016) passTrigger1 = (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;
        else if (year==2017) passTrigger1 = (e->IsTriggered("HLT_IsoMu27_v",mu0) or e->IsTriggered("HLT_IsoTkMu27_v",mu0)) ;
        else if (year==2018) passTrigger1 = (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;

        passTrigger1 = passTrigger1 and mu0->GetTightId();

        if ( (mu1->Pt() > ptcut0 and year==2017) or (mu1->Pt()>26 and (year== 2016 or year==2018) )) 
        {
            if (year==2016) passTrigger2 = (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
            else if (year==2017) passTrigger2 = (e->IsTriggered("HLT_IsoMu27_v",mu1) or e->IsTriggered("HLT_IsoTkMu27_v",mu1)) ;
            else if (year==2018) passTrigger2 = (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
            passTrigger2 = passTrigger2 and mu1->GetTightId();
        }
        passTrigger=passTrigger1 or passTrigger2;

        if (passTriggerEvent and not passTrigger) Log(__FUNCTION__,"INFO","Fail to trigger event due to trigger matching");

    }

    Object Z;
    if (recoMuons)
    {
        TLorentzVector zero(0,0,0,0);
        Z.SetP4(zero); // make sure it is 0.
        Z += *mu0;
        Z += *mu1;
    }

    // Full selection
    if ( recoMuons and passAsymmPtCuts and passTrigger and Z.M() >60 and Z.M()<120)
    {

        Fill("DYAnalysis/Vars/Mmm_"+ label,systname, Z.M(),e->weight()) ;
        Fill("DYAnalysis/Vars/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
       	Fill("DYAnalysis/Vars/Npvmm_"+ label,systname, e->Npv(),e->weight()) ;
        Fill("DYAnalysis/Vars/NJmm_"+ label,systname, e->NcentralJets(),e->weight()) ;

        if (j0 != NULL and j0->Pt() > 500) {
            vector<float> jbin{500,1000,1500,2000,10000};
            //std::cout<<"[DEBUG] Considering jet with pt"<<j0->Pt()<<std::endl;
            //first that is 'not less'
            auto ib=std::lower_bound(jbin.begin(),jbin.end(),std::min(j0->Pt(),8000.F));
            //std::cout<<"[DEBUG] -> found="<< (ib!=jbin.end()) <<" i="<<ib-jbin.begin()<<std::endl;
            if (ib != jbin.end())
            {
                int i= ib-jbin.begin()-1;
                Fill(Form("DYAnalysis/Vars/axis2_jet%.0f_%.0f_",jbin[i],jbin[i+1])+label,systname,j0->QGLVar("log:axis2"),e->weight());
                Fill(Form("DYAnalysis/Vars/mult_jet%.0f_%.0f_",jbin[i],jbin[i+1])+label,systname,j0->QGLVar("mult"),e->weight());
                Fill(Form("DYAnalysis/Vars/qgl_jet%.0f_%.0f_",jbin[i],jbin[i+1])+label,systname,j0->QGL(),e->weight());

                string flavor=""; int fl=j0->Flavor();
                if ( label != "Data" and fl ==21 ) flavor="G";
                else if ( label != "Data" and abs(fl)<5 ) flavor="Q";
                else if ( label != "Data" and abs(fl)!=0 ) flavor="O";
                else if ( label != "Data" and abs(fl)==0 ) flavor="U";

                if (label!="Data")
                {
                    Fill(Form("DYAnalysis/Vars/axis2_%s_jet%.0f_%.0f_",flavor.c_str(),jbin[i],jbin[i+1])+label,systname,j0->QGLVar("log:axis2"),e->weight());
                    Fill(Form("DYAnalysis/Vars/mult_%s_jet%.0f_%.0f_",flavor.c_str(),jbin[i],jbin[i+1])+label,systname,j0->QGLVar("mult"),e->weight());
                    Fill(Form("DYAnalysis/Vars/qgl_%s_jet%.0f_%.0f_",flavor.c_str(),jbin[i],jbin[i+1])+label,systname,j0->QGL(),e->weight());
                }
            }
        } // j0
    } // full selection on muons


    return 0;
}



int DYAnalysis::analyze(Event *e, string systname)
{

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");
    analyzeMM(e,systname);
    //analyzeEM(e,systname);
    //analyzeEE(e,systname);
    //analyzeLLL(e,systname);
    return 0;

}

void DYAnalysis::ApplyMuonSF(Event*e,string systname)
{
    // ---------------------------------------------------------------
    //  ------------------- APPLY Muon SF --------------
    // ---------------------------------------------------------------
    //

    // weights
    double w_2016_BF=0.5498;
    double w_2016_GH=1.-w_2016_BF;

    double w_2017_BC= 4.802+9.629;
    double w_2017_DE= 2.435+9.628;
    double w_2017_F = 13.433;

    double w_2018_A=14./59.97; // total runA, I should use the splitting inside the run
    double w_2018_BD=1.-w_2018_A;

    double sfTrigger=1.0;
    
    // ---------------------------------------------------------------
    //                          Trigger SF
    // ---------------------------------------------------------------

    if (year ==2017){
        SF* sf0_runBCDEF =e->GetWeight()->GetSF("mu_trigger_runBCDEF");
        if (sf0_runBCDEF== NULL) cout<<"Unable to get SF TRG BCDEF"<<endl;

        SF_PtEta_And_Eff* sf_runBCDEF = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runBCDEF);
        if (sf_runBCDEF== NULL) {cout<<"Unable to convert SF TRG BCDEF:" <<endl; sf0_runBCDEF->print(); }

        //double effdata_runGH=1.0,effmc_runGH =1.0;
        double effdata_runBCDEF=1.0,effmc_runBCDEF =1.0;

        bool changed=false;
        if (mu0)
        {
            changed=true;
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu0->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu0->Pt(),30.1F);

            sf_runBCDEF->set( pt_for_sf, eta_for_sf);
            effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runBCDEF->syst*0.005 ) ; // 0.5% correlated with runGH
            effmc_runBCDEF *= 1.- (sf_runBCDEF->getMCEff()) ;
        }

        if (mu1 and mu1->Pt() > 30)
        {
            changed=true;

            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu1->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu1->Pt(),30.1F);

            sf_runBCDEF->set( pt_for_sf ,eta_for_sf );
            effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runBCDEF->syst*0.005) ; 
            effmc_runBCDEF *= 1.- (sf_runBCDEF->getMCEff()) ;
        }

        if(changed)
        {
            effdata_runBCDEF = 1.-effdata_runBCDEF;
            effmc_runBCDEF = 1.-effmc_runBCDEF;
        }

        sfTrigger = effdata_runBCDEF/effmc_runBCDEF;
    }
    else if (year==2016){
        SF* sf0_runBF =e->GetWeight()->GetSF("mu_trigger_2016_runBF");
        if (sf0_runBF== NULL) cout<<"Unable to get SF TRG 2016BF"<<endl;

        SF* sf0_runGH =e->GetWeight()->GetSF("mu_trigger_2016_runGH");
        if (sf0_runGH== NULL) cout<<"Unable to get SF TRG 2016GH"<<endl;

        SF_PtEta_And_Eff* sf_runBF = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runBF);
        SF_PtEta_And_Eff* sf_runGH = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runGH);
        if (sf_runBF== NULL) {cout<<"Unable to convert SF TRG 2016 BF:" <<endl; sf0_runBF->print(); }
        if (sf_runGH== NULL) {cout<<"Unable to convert SF TRG 2016 GH:" <<endl; sf0_runGH->print(); }

        double effdata_runBF=1.0,effmc_runBF =1.0;
        double effdata_runGH=1.0,effmc_runGH =1.0;

        bool changed=false;
        if (mu0)
        {
            changed=true;
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu0->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu0->Pt(),30.1F);

            sf_runBF->set( std::max(mu0->Pt(),float(30.1)), eta_for_sf);
            sf_runGH->set( std::max(mu0->Pt(),float(30.1)), eta_for_sf);
            effdata_runBF *= 1.-(sf_runBF->getDataEff()+sf_runGH->syst*0.005 ) ; // 0.5% correlated with runGH
            effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*0.005 ) ; // 0.5% correlated with runGH
            effmc_runBF *= 1.- (sf_runBF->getMCEff()) ;
            effmc_runGH *= 1.- (sf_runGH->getMCEff()) ;
        }

        if (mu1 and mu1->Pt() > 26)
        {
            changed=true;

            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu1->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu1->Pt(),30.1F);

            sf_runBF->set( pt_for_sf ,eta_for_sf );
            sf_runGH->set( pt_for_sf ,eta_for_sf );
            effdata_runBF *= 1.-(sf_runBF->getDataEff()+sf_runGH->syst*0.005) ; // 0.5% correlated runGH runBCDEF
            effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*0.005) ; // 0.5% correlated runGH runBCDEF
            effmc_runBF *= 1.- (sf_runBF->getMCEff()) ;
        }

        if(changed)
        {
            effdata_runGH = 1.-effdata_runGH;
            effmc_runGH = 1.-effmc_runGH;

            effdata_runBF = 1.-effdata_runBF;
            effmc_runBF = 1.-effmc_runBF;
        }

        sfTrigger = w_2016_GH* effdata_runGH / effmc_runGH + w_2016_BF*effdata_runBF/effmc_runBF;
    
    }
    else if (year==2018){
        SF* sf0_runA =e->GetWeight()->GetSF("mu_trigger_beforeHLT_2018");
        if (sf0_runA== NULL) cout<<"Unable to get SF TRG 2018 Beforet HLT"<<endl;

        SF* sf0_runBD =e->GetWeight()->GetSF("mu_trigger_afterHLT_2018");
        if (sf0_runBD== NULL) cout<<"Unable to get SF TRG 2018 After HLT"<<endl;

        SF_PtEta_And_Eff* sf_runA = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runA);
        SF_PtEta_And_Eff* sf_runBD = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runBD);
        if (sf_runA== NULL) {cout<<"Unable to convert SF TRG 2016 A:" <<endl; sf0_runA->print(); }
        if (sf_runBD== NULL) {cout<<"Unable to convert SF TRG 2016 BD:" <<endl; sf0_runBD->print(); }

        double effdata_runA=1.0,effmc_runA =1.0;
        double effdata_runBD=1.0,effmc_runBD =1.0;

        bool changed=false;
        if (mu0)
        {
            changed=true;
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu0->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu0->Pt(),30.1F);

            sf_runA->set( pt_for_sf, eta_for_sf);
            sf_runBD->set( pt_for_sf, eta_for_sf);
            effdata_runA *= 1.-(sf_runA->getDataEff()+sf_runBD->syst*0.005 ) ; // 0.5% correlated with runBD
            effdata_runBD *= 1.-(sf_runBD->getDataEff()+sf_runBD->syst*0.005 ) ; // 0.5% correlated with runBD
            effmc_runA *= 1.- (sf_runA->getMCEff()) ;
            effmc_runBD *= 1.- (sf_runBD->getMCEff()) ;
        }

        if (mu1 and mu1->Pt() > 26)
        {
            changed=true;

            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            //float eta_for_sf= std::max(std::min(mu1->Eta(),2.399F),-2.399F);
            float pt_for_sf = std::max(mu1->Pt(),30.1F);

            sf_runA->set( pt_for_sf ,eta_for_sf );
            sf_runBD->set( pt_for_sf ,eta_for_sf );
            effdata_runA *= 1.-(sf_runA->getDataEff()+sf_runBD->syst*0.005) ; // 0.5% correlated runBD runBCDEF
            effdata_runBD *= 1.-(sf_runBD->getDataEff()+sf_runBD->syst*0.005) ; // 0.5% correlated runBD runBCDEF
            effmc_runA *= 1.- (sf_runA->getMCEff()) ;
        }

        if(changed)
        {
            effdata_runBD = 1.-effdata_runBD;
            effmc_runBD = 1.-effmc_runBD;

            effdata_runA = 1.-effdata_runA;
            effmc_runA = 1.-effmc_runA;
        }

        sfTrigger = w_2018_BD* effdata_runBD / effmc_runBD + w_2018_A*effdata_runA/effmc_runA;
    }
    
    // ---------------------------------------------------------------
    //              ID
    // ---------------------------------------------------------------
    double sfId=1.0;
    if (year==2017){
        double sfBC=1.0, sfDE=1.0, sfF=1.0;
        if (mu0 and mu0->Pt() >20.)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));

            e->SetPtEtaSF("mu_id_runBC", pt_for_sf,eta_for_sf );
            sfBC = e->GetWeight()->GetSF("mu_id_runBC")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlated 

            e->SetPtEtaSF("mu_id_runDE", pt_for_sf,eta_for_sf );
            sfDE = e->GetWeight()->GetSF("mu_id_runDE")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlated with BC

            e->SetPtEtaSF("mu_id_runF", pt_for_sf,eta_for_sf );
            sfF = e->GetWeight()->GetSF("mu_id_runF")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlagted with BC
        }
        else if (mu0){ //low pt
            float pt_for_sf=mu0->Pt();
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            e->SetPtEtaSF("mu_id_BF_lowpt",pt_for_sf,eta_for_sf);
            double sf_tmp=e->GetWeight()->GetSF("mu_id_BF_lowpt")->sf;
            sfBC=sf_tmp;
            sfDE=sf_tmp;
            sfF =sf_tmp;
        }

        if (mu1 and mu1->Pt()>20)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            e->SetPtEtaSF("mu_id_runBC", pt_for_sf,eta_for_sf);
            sfBC *= e->GetWeight()->GetSF("mu_id_runBC")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlated 

            e->SetPtEtaSF("mu_id_runDE", pt_for_sf,eta_for_sf );
            sfDE *= e->GetWeight()->GetSF("mu_id_runDE")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlated with BC

            e->SetPtEtaSF("mu_id_runF", pt_for_sf,eta_for_sf);
            sfF *= e->GetWeight()->GetSF("mu_id_runF")->sf + e->GetWeight()->GetSF("mu_id_runBC")->syst*0.01; // 1% correlagted with BC
        }
        else if (mu1){ //low pt
            float pt_for_sf=mu1->Pt();
            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            e->SetPtEtaSF("mu_id_BF_lowpt",pt_for_sf,eta_for_sf);
            double sf_tmp=e->GetWeight()->GetSF("mu_id_BF_lowpt")->sf;
            sfBC *=sf_tmp;
            sfDE *=sf_tmp;
            sfF  *=sf_tmp;
        }

        sfId = (w_2017_BC*sfBC + w_2017_DE*sfDE + w_2017_F*sfF)/(w_2017_BC+w_2017_DE+w_2017_F);
    }
    else if (year==2018)
    {
        double sf=1.0;
        if (mu0)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));

            e->SetPtEtaSF("mu_id_2018", pt_for_sf,eta_for_sf );
            sf = e->GetWeight()->GetSF("mu_id_2018")->sf ; 

        }

        if (mu1)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            e->SetPtEtaSF("mu_id_2018", pt_for_sf,eta_for_sf);
            sf *= e->GetWeight()->GetSF("mu_id_2018")->sf ; 

        }
        sfId = sf;
    }
    else if (year==2016)
    {
        double sfBF=1.0,sfGH=1.0;
        if (mu0 and mu0->Pt()>20)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            //float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            float eta_for_sf= std::max(std::min(mu0->Eta(),2.399F),-2.399F); // id 2016 is in eta and not in aeta

            e->SetPtEtaSF("mu_id_2016_runBF", pt_for_sf,eta_for_sf );
            sfBF = e->GetWeight()->GetSF("mu_id_2016_runBF")->sf ; 

            e->SetPtEtaSF("mu_id_2016_runGH", pt_for_sf,eta_for_sf );
            sfBF = e->GetWeight()->GetSF("mu_id_2016_runGH")->sf ; 
        }
#warning 2016_No_Lowpt_Id_SF
        //else if (mu0)// lowpt
        //{
        //    float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
        //    float pt_for_sf = mu0->Pt();
        //    e->SetPtEtaSF("mu_id_2016_lowpt_runBF", pt_for_sf,eta_for_sf );
        //    sfBF = e->GetWeight()->GetSF("mu_id_2016_lowpt_runBF")->sf ; 
        //    e->SetPtEtaSF("mu_id_2016_lowpt_runGH", pt_for_sf,eta_for_sf );
        //    sfBF = e->GetWeight()->GetSF("mu_id_2016_lowpt_runGH")->sf ; 
        //}

        if (mu1 and mu1->Pt()>20)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            //float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            float eta_for_sf= std::max(std::min(mu1->Eta(),2.399F),-2.399F); // id 2016 is in eta and not in aeta

            e->SetPtEtaSF("mu_id_2016_runBF", pt_for_sf,eta_for_sf );
            sfBF *= e->GetWeight()->GetSF("mu_id_2016_runBF")->sf ; 

            e->SetPtEtaSF("mu_id_2016_runGH", pt_for_sf,eta_for_sf );
            sfBF *= e->GetWeight()->GetSF("mu_id_2016_runGH")->sf ; 
        }
        //else if (mu1) //low pt
        //{
        //    float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
        //    float pt_for_sf = mu1->Pt();
        //    e->SetPtEtaSF("mu_id_2016_lowpt_runBF", pt_for_sf,eta_for_sf );
        //    sfBF *= e->GetWeight()->GetSF("mu_id_2016_lowpt_runBF")->sf ; 
        //    e->SetPtEtaSF("mu_id_2016_lowpt_runGH", pt_for_sf,eta_for_sf );
        //    sfBF *= e->GetWeight()->GetSF("mu_id_2016_lowpt_runGH")->sf ; 
        //}
        sfId = w_2016_BF*sfBF + w_2016_GH*sfGH;
    }

    // ---------------------------------------------------------------
    //              ISO
    // ---------------------------------------------------------------

    double sfIso=1.0; // TODO, apply a different sf for miniIsolation, use flag isMiniIsoLeptons
    if (year==2017){
        double sfBC=1.0, sfDE=1.0, sfF=1.0;
        // ISO Loose
        if (mu0)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float eta_for_sf= std::min(fabs(mu0->Eta()),float(2.399));
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            e->SetPtEtaSF("mu_iso_runBC", pt_for_sf,eta_for_sf);
            sfBC = e->GetWeight()->GetSF("mu_iso_runBC")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlated 

            e->SetPtEtaSF("mu_iso_runDE", pt_for_sf,eta_for_sf);
            sfDE = e->GetWeight()->GetSF("mu_iso_runDE")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlated with BC

            e->SetPtEtaSF("mu_iso_runF", pt_for_sf,eta_for_sf);
            sfF = e->GetWeight()->GetSF("mu_iso_runF")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlagted with BC

            //sfIso *= (w_BC*sf1 + w_DE*sf2 + w_F*sf3)/(w_BC+w_DE+w_F);
        }
        if (mu1)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            e->SetPtEtaSF("mu_iso_runBC", pt_for_sf,eta_for_sf );
            sfBC *= e->GetWeight()->GetSF("mu_iso_runBC")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlated 

            e->SetPtEtaSF("mu_iso_runDE", pt_for_sf,eta_for_sf);
            sfDE *= e->GetWeight()->GetSF("mu_iso_runDE")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlated with BC

            e->SetPtEtaSF("mu_iso_runF", pt_for_sf,eta_for_sf);
            sfF*= e->GetWeight()->GetSF("mu_iso_runF")->sf + e->GetWeight()->GetSF("mu_iso_runBC")->syst*0.01; // 1% correlagted with BC

        }
        sfIso = (w_2017_BC*sfBC + w_2017_DE*sfDE + w_2017_F*sfF)/(w_2017_BC+w_2017_DE+w_2017_F);
    }
    else if (year==2018){
        double sf=1.0;
        if (mu0)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu0->Eta()),2.399F);

            e->SetPtEtaSF("mu_iso_2018", pt_for_sf,eta_for_sf );
            sf = e->GetWeight()->GetSF("mu_iso_2018")->sf ; 

        }

        if (mu1)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            float eta_for_sf= std::min(fabs(mu1->Eta()),float(2.399));
            e->SetPtEtaSF("mu_iso_2018", pt_for_sf,eta_for_sf);
            sf *= e->GetWeight()->GetSF("mu_iso_2018")->sf ; 

        }
        sfIso = sf;
    }
    else if (year==2016){
        double sfBF=1.0,sfGH=1.0;
        if (mu0)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu0->Pt(),min_pt_sf),max_pt_sf);
            //float eta_for_sf= std::min(fabs(mu0->Eta()),2.399F);
            float eta_for_sf= std::max(std::min(mu0->Eta(),2.399F),-2.399F); // id 2016 is in eta and not in aeta
            e->SetPtEtaSF("mu_iso_2016_runBF", pt_for_sf,eta_for_sf );
            sfBF = e->GetWeight()->GetSF("mu_iso_2016_runBF")->sf ; 
            e->SetPtEtaSF("mu_iso_2016_runGH", pt_for_sf,eta_for_sf );
            sfGH = e->GetWeight()->GetSF("mu_iso_2016_runGH")->sf ; 

        }

        if (mu1)
        {
            float min_pt_sf=20.+0.001,max_pt_sf=120.-0.001;
            float pt_for_sf = std::min(std::max(mu1->Pt(),min_pt_sf),max_pt_sf);
            //float eta_for_sf= std::min(fabs(mu1->Eta()),2.399F);
            float eta_for_sf= std::max(std::min(mu1->Eta(),2.399F),-2.399F); // id 2016 is in eta and not in aeta
            e->SetPtEtaSF("mu_iso_2016_runBF", pt_for_sf,eta_for_sf);
            sfBF *= e->GetWeight()->GetSF("mu_iso_2016_runBF")->sf ; 
            e->SetPtEtaSF("mu_iso_2016_runGH", pt_for_sf,eta_for_sf);
            sfGH *= e->GetWeight()->GetSF("mu_iso_2016_runGH")->sf ; 

        }
        sfIso = w_2016_BF*sfBF + w_2016_GH*sfGH;
    }
    
    // apply trigger, id and iso sf
    {
        SF* sf= e->GetWeight()->GetSF("dummy"); // base, I modify this one
        sf->syst=0;
        sf->sf= sfId*sfIso*sfTrigger; //syst already included
        sf->err = 0.0;
        e->ApplySF("dummy");

        //if (doSync and not processingSyst_) {SetTreeVar("muonSF",sf->sf);}
    }

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
