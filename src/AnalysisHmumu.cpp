#include "interface/AnalysisHmumu.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "Python.h"

#include "interface/HiggsTemplateCrossSections.hpp"

#define VERBOSE 0

//#define SCIKIT_TIMING
#ifdef SCIKIT_TIMING
    #warning SCIKIT_TIMING dont use for submission
#endif

//#warning Hmumu ANALYSIS NON ISO

void HmumuAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(10); 
    l->SetIsoRelCut(0.25);
    l->SetEtaCut(2.4);
    //l->SetTightCut(true);
    //l->SetMediumCut(false);
    l->SetVetoCut();
    l->SetTightCut(false);
    l->SetLooseCut(false);
    l->SetMediumCut(true);
    l->SetTrackerMuonCut(true);
    l->SetGlobalMuonCut(true);
}

void HmumuAnalysis::SetJetCuts(Jet *j) { 
    j->SetBCut(0.8484); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
    j->SetPuIdCut(-999);
}

void HmumuAnalysis::SetTauCuts(Tau *t){ 
    t->SetIsoCut(2.5); 
    t->SetEtaCut(2.1); 
    t->SetPtCut(8000);  // remove taus!!! 
    t->SetMuRej(true); 
    t->SetEleRej(true);
    t->SetTrackPtCut(30.);
    t->SetProngsCut(1); 
    t->SetDecayMode(1);
}

void HmumuAnalysis::SetPhotonCuts(Photon *p){
    p->SetIsoCut(-1); 
    p->SetPtCut(8000);
}

double HmumuAnalysis::dimu_dPhiStar(Lepton* mu0, Lepton*mu1) {  // Phi separation in the parent's rest frame
      double phi_star = 0;
      //double mu_dPhi = std::abs(mu0->Phi() - mu1->Phi());
      //if(mu_dPhi > TMath::Pi()) mu_dPhi = 2*TMath::Pi() - mu_dPhi;
      double mu_dPhi = fabs(mu0->DeltaPhi(*mu1)); 
      double phiACOP = TMath::Pi() - mu_dPhi;
      double thetaStarEta = std::acos(std::tanh((mu0->Eta() - mu1->Eta())/float(2.)));
      phi_star = std::tan(phiACOP/float(2.))*std::sin(thetaStarEta);
      return phi_star;
}

string HmumuAnalysis::Category(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start Category");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   
   // construct a Higgs-like object  
    Object Hmm;
    Hmm += *mu0;
    Hmm += *mu1;

    if (doSync){
        int cat=-100;
        // set, in case exit before
        SetTreeVar("cat",cat);
        SetTreeVar("pt1",mu0->Pt());
        SetTreeVar("pt2",mu1->Pt());
        SetTreeVar("eta1",mu0->Eta());
        SetTreeVar("eta2",mu1->Eta());
        SetTreeVar("phi1",mu0->Phi());
        SetTreeVar("phi2",mu1->Phi());
        SetTreeVar("njets",jets.size());
        SetTreeVar("mass",Hmm.M());
        SetTreeVar("Hpt" ,Hmm.Pt());
        SetTreeVar("Heta" ,Hmm.Eta());
        SetTreeVar("Hphi" ,Hmm.Phi());
        for(int ijet=0;ijet<jets.size();++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,jets[ijet]->Pt());
            SetTreeVar("jet_eta",ijet,jets[ijet]->Eta());
            SetTreeVar("jet_phi",ijet,jets[ijet]->Phi());
        }
    }

    string muStr ="";
    if ( fabs(eta1) < 0.8 and fabs(eta0) <0.8) muStr = "BB";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <0.8) muStr =  "BO";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <1.6) muStr =  "BO";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <0.8) muStr =  "BE";
    else if (fabs(eta1) < 0.8 and fabs(eta0) <2.4) muStr =  "BE";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <1.6) muStr =  "OO";
    else if (fabs(eta1) < 1.6 and fabs(eta0) <2.4) muStr =  "OE";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <1.6) muStr =  "OE";
    else if (fabs(eta1) < 2.4 and fabs(eta0) <2.4) muStr =  "EE";
    else return "";

    // vbf -- fully combinatorics
    bool isTightVbf=false;
    bool isTightGF=false;
    bool isVbf=false;

    for(unsigned i=0;i<jets.size() ;++i)
    for (unsigned j=i+1;j<jets.size();++j)
    {
        // vbf preselection. jets are pt-sorted
        if (jets[i]->Pt() >40 and jets[j]->Pt() >30) isVbf=true;
        else continue;

        double mjj= jets[i]->InvMass( jets[j] ) ;
        double detajj= fabs( jets[i]->Eta() - jets[j]->Eta() );
        if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Jet Eta (i=%d,j=%d) etai=%f etaj=%f | Pt_i=%f Pt_j=%f Pz_i=%f Pz_j=%f",i,j,jets[i]->Eta(),jets[j]->Eta(),jets[i]->Pt(),jets[j]->Pt(),jets[i]->GetP4().Pz(),jets[j]->GetP4().Pz()));

        // orig
        if (mjj > 650 and detajj > 3.5)  isTightVbf = true;
        if (mjj > 250 and Hmm.Pt() >50 )  isTightGF = true;
        //#warning LOOSE_VBF
        //if (mjj > 400 and detajj > 3.5)  isTightVbf = true;
        //if (mjj > 100 and Hmm.Pt() >50 )  isTightGF = true;
    }

    int nbjets=0;
    for(unsigned i=0;i<jets.size() ;++i)
    {
        if (jets[i]->IsBJet() and jets[i]->Pt() >30 and abs(jets[i]->Eta())<2.4)  nbjets +=1;
    }

    string vbfStr="";
    /* OneB
    if (nbjets >0 ) vbfStr="OneB";
    else if (isTightVbf ) vbfStr = "VBF0";
    else if (isTightGF) vbfStr = "GF";
    else if (isVbf) vbfStr = "VBF1";
    else if (Hmm.Pt() >25) vbfStr = "Untag0";
    else vbfStr = "Untag1";
    */
    if (isTightVbf and nbjets==0) vbfStr = "VBF0";
    else if (isTightGF and nbjets==0) vbfStr = "GF";
    else if (isVbf and nbjets==0) vbfStr = "VBF1";
    else if (Hmm.Pt() >25 and nbjets==0) vbfStr = "Untag0";
    else if (nbjets == 0 ) vbfStr = "Untag1";

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + vbfStr + "_" + muStr);

    if (doSync){
        int cat=-100;
        // derive cat number -- hardcoded. should use HmmConfig :(
        if (vbfStr =="VBF0")        cat=0;
        else if (vbfStr =="OneB")   cat=1;
        else if (vbfStr =="GF")     cat=2;
        else if (vbfStr =="VBF1")   cat=3;
        else if (vbfStr =="Untag0") cat=4;
        else if (vbfStr =="Untag1") cat=5;

        if (muStr=="BB")      cat += 0*6;
        else if (muStr=="BO") cat += 1*6;
        else if (muStr=="BE") cat += 2*6;
        else if (muStr=="OO") cat += 3*6;
        else if (muStr=="OE") cat += 4*6;
        else if (muStr=="EE") cat += 5*6;

        SetTreeVar("cat",cat);
        SetTreeVar("nbjets",nbjets);
    }

    if (vbfStr == "" ) return "";
    return vbfStr +"_" + muStr;
}


string HmumuAnalysis::CategoryAutoCat(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets,float met){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start CategoryAutoCat");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   
   // construct a Higgs-like object  
    Object Hmm;
    Hmm += *mu0;
    Hmm += *mu1;

    if (doSync){
        int cat=-100;
        // set, in case exit before
        SetTreeVar("cat",cat);
        SetTreeVar("pt1",mu0->Pt());
        SetTreeVar("pt2",mu1->Pt());
        SetTreeVar("eta1",mu0->Eta());
        SetTreeVar("eta2",mu1->Eta());
        SetTreeVar("phi1",mu0->Phi());
        SetTreeVar("phi2",mu1->Phi());
        SetTreeVar("njets",jets.size());
        SetTreeVar("mass",Hmm.M());
        SetTreeVar("Hpt" ,Hmm.Pt());
        SetTreeVar("met" ,met);
        for(int ijet=0;ijet<jets.size();++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,jets[ijet]->Pt());
            SetTreeVar("jet_eta",ijet,jets[ijet]->Eta());
            SetTreeVar("jet_phi",ijet,jets[ijet]->Phi());
        }
    }


    // fully combinatorics
    vector<bool> pass; // name of category and order. if they pass.
    for (int i=0;i<categories_.size();++i)
    {
        pass.push_back(false);
    }

    // FIXME: is this the deifinition?
    float mu_res_eta = std::max( fabs(mu0->Eta()),fabs(mu1->Eta())) ;

    int nbjets=0;
    for(unsigned i=0;i<jets.size() ;++i)
    {
        if (jets[i]->IsBJet() and jets[i]->Pt() >30 and abs(jets[i]->Eta())<2.4)  nbjets +=1;
    }

    if (Hmm.Pt() <7.) pass[5]=true; // no jets
    if (Hmm.Pt() >=7. and Hmm.Pt() <18.) pass[10]=true; // no jets
    if (Hmm.Pt() >=18. and nbjets >=1 ) pass[4] = true; // no jets mjj/detajj

    for(unsigned i=0;i<jets.size() ;++i)
    for (unsigned j=i+1;j<jets.size();++j)
    {
        // vbf preselection. jets are pt-sorted
        if (not (jets[i]->Pt() >40 and jets[j]->Pt() >30)) continue;

        double mjj= jets[i]->InvMass( jets[j] ) ;
        double detajj= fabs( jets[i]->Eta() - jets[j]->Eta() );

        // orig
        // https://indico.cern.ch/event/605422/contributions/2461226/attachments/1436814/2209863/dt_plus_bdt_limits.pdf slide8
        if (Hmm.Pt() >=18.0 and nbjets==0)
        {
            if ( detajj >=4.0 )
            {
                if (mjj>=1000.){
                    if ( met < 60 and Hmm.Pt() < 30) pass[0]=true;
                    if ( met < 60 and Hmm.Pt() > 30.)  pass[15] = true;
                    if ( met >=60) pass[2] = true;
                }
                else{
                    if ( Hmm.Pt() <  100 and met >=65.) pass[1] = true;
                    if ( Hmm.Pt() <  100 and met < 65.) pass[7] = true;
                    if ( Hmm.Pt() >= 100) pass[8] = true;
                }
            }
            else{
                if ( mu_res_eta <0.797 and Hmm.Pt() >= 60. and met >= 70.) pass[3]=true; 
                if ( mu_res_eta <0.797 and Hmm.Pt() >= 60. and met < 70.) pass[13]=true; 
                if ( mu_res_eta <0.797 and Hmm.Pt() <  60. ) pass[14]=true; 
                if ( mu_res_eta >= 0.797 and mu_res_eta <1.4 and Hmm.Pt() >=40. and met >= 50.) pass[6]=true;
                if ( mu_res_eta >= 0.797 and mu_res_eta <1.4 and Hmm.Pt() >=40. and met <  50.) pass[12]=true;
                if ( mu_res_eta >= 0.797 and mu_res_eta <1.4 and Hmm.Pt() < 40.) pass[11]=true;
                if ( mu_res_eta >= 1.4) pass[9]=true;  //EE
            }
        }

    }


    string catStr="";
    int icat=-100;
    for(size_t i=0;i<pass.size();++i)
    {
        if (pass[i] ) { icat= i;catStr = categories_[i];} // cat0 is the less significant
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + catStr);

    if (doSync){
        SetTreeVar("cat",icat);
        SetTreeVar("nbjets",nbjets);
    }

    return catStr;
}

string HmumuAnalysis::CategoryBdt(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets,float met){
    bdt.clear();
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start CategoryBdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   
   // construct a Higgs-like object  
    Object Hmm;
    Hmm += *mu0;
    Hmm += *mu1;


    //SetVariable("mu1_abs_eta",fabs(mu0->Eta()));    
    //SetVariable("mu2_abs_eta",fabs(mu1->Eta()));    
    SetVariable("dimu_pt",Hmm.Pt());    
    SetVariable("dimu_eta",Hmm.Eta());    
    SetVariable("dimu_abs_dEta",fabs(mu0->Eta()-mu1->Eta()));    
    SetVariable("dimu_abs_dPhi",fabs(mu0->DeltaPhi(*mu1)));
    //SetVariable("dimu_dPhiStar",dimu_dPhiStar(mu0,mu1));
    if (jets.size() >=1)
    {
        //SetVariable("jet1_pt",jets[0]->Pt());    
        SetVariable("jet1_eta",jets[0]->Eta());    
    }
    else{
        //SetVariable("jet1_pt",0.);    
        SetVariable("jet1_eta",-5.);    
    }
    if (jets.size() >=2)
    {
        //SetVariable("jet2_pt",jets[1]->Pt());    
        SetVariable("jet2_eta",jets[1]->Eta());    
    }   
    else
    {
        //SetVariable("jet2_pt",0.);    
        SetVariable("jet2_eta",-5.);    
    }   

    vector<pair<float,float> > mjj_detajj;

    for(unsigned i=0;i<jets.size() ;++i)
    for (unsigned j=i+1;j<jets.size();++j)
    {
        mjj_detajj.push_back( pair<float,float>(jets[i]->InvMass( jets[j] ), fabs( jets[i]->Eta() - jets[j]->Eta() )) );
    }
    for(size_t i=mjj_detajj.size(); i<4;++i)
    {
        mjj_detajj.push_back(pair<float,float>(0.,-1));
    }

    sort ( mjj_detajj.begin(),mjj_detajj.end(), [](const pair<float,float>&x,const pair<float,float>&y){if (x.first > y.first) return true;if (x.first<y.first) return false; return x.second > y.second; } );
    SetVariable("dijet1_mass",mjj_detajj[0].first);    
    SetVariable("dijet2_mass",mjj_detajj[1].first);    
    //SetVariable("dijet3_mass",mjj_detajj[2].first);    
    //SetVariable("dijet4_mass",mjj_detajj[3].first);    
    SetVariable("dijet1_abs_dEta",mjj_detajj[0].second);    
    SetVariable("dijet2_abs_dEta",mjj_detajj[1].second);    
    //SetVariable("dijet3_abs_dEta",mjj_detajj[2].second);    
    //SetVariable("dijet4_abs_dEta",mjj_detajj[3].second);    
    //
    if (doSync){
        SetTreeVar("pt1",mu0->Pt());
        SetTreeVar("pt2",mu1->Pt());
        SetTreeVar("eta1",mu0->Eta());
        SetTreeVar("eta2",mu1->Eta());
        SetTreeVar("phi1",mu0->Phi());
        SetTreeVar("phi2",mu1->Phi());
        SetTreeVar("njets",jets.size());
        SetTreeVar("mass",Hmm.M());
        SetTreeVar("Hpt" ,Hmm.Pt());
        SetTreeVar("Heta" ,Hmm.Eta());
        SetTreeVar("Hphi" ,Hmm.Phi());
        SetTreeVar("deltaeta",fabs(mu0->Eta()-mu1->Eta()));    
        SetTreeVar("deltaphi",fabs(mu0->DeltaPhi(*mu1)));
        SetTreeVar("met" ,met);
        for(int ijet=0;ijet<jets.size();++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,jets[ijet]->Pt());
            SetTreeVar("jet_eta",ijet,jets[ijet]->Eta());
            SetTreeVar("jet_phi",ijet,jets[ijet]->Phi());
        }
        SetTreeVar("mjj_1",mjj_detajj[0].first);
        SetTreeVar("mjj_2",mjj_detajj[1].first);
        SetTreeVar("detajj_1",mjj_detajj[0].second);
        SetTreeVar("detajj_2",mjj_detajj[1].second);
    }

    int nbjets=0;
    int ncentjets=0;
    int nfwdjets=0;
    for(unsigned i=0;i<jets.size() ;++i)
    {
        if (jets[i]->IsBJet() and jets[i]->Pt() >30 and abs(jets[i]->Eta())<2.4)  nbjets +=1;
        if (abs(jets[i]->Eta())<2.4)  ncentjets +=1;
        else nfwdjets +=1;
    }
    SetVariable("nJetsCent",ncentjets);    
    SetVariable("nJetsFwd",nfwdjets);    
    SetVariable("nBMed",nbjets);    
    SetVariable("MET",met);    
    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
         bdt.push_back(readers_[i]->EvaluateMVA("BDTG_default") );
    }
    if (doSync)
    {
        SetTreeVar("bdt",bdt[0]);
        SetTreeVar("nbjets",nbjets);
    }


    int icat=-100; string catStr="";

    float mu_max_eta = std::max(fabs(mu0->Eta()),fabs(mu1->Eta()));
    float mu_ave_eta = (fabs(mu0->Eta())+fabs(mu1->Eta()))/2.;

    //*
    if ( bdt[0] < -0.400 ) icat = 0 ;
    if ( bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta >= 1.900 ) icat = 1 ;
    if ( bdt[0] >= -0.400 and bdt[0] < 0.050 and mu_max_eta >= 1.900 ) icat = 2 ;
    if ( bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta >= 1.900 ) icat = 3 ;
    if ( bdt[0] >= 0.050 and bdt[0] < 0.250 and mu_max_eta < 0.900 ) icat = 4 ;
    if ( bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta < 0.900 ) icat = 5 ;
    if ( mu_max_eta < 1.900 and bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta >= 0.900 ) icat = 6 ;
    if ( bdt[0] >= 0.050 and bdt[0] < 0.250 and mu_max_eta >= 0.900 ) icat = 7 ;
    if ( bdt[0] >= -0.400 and bdt[0] < 0.050 and mu_max_eta < 1.900 ) icat = 8 ;
    if ( bdt[0] < 0.730 and bdt[0] >= 0.650 ) icat = 9 ;
    if ( mu_max_eta < 1.900 and bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta >= 0.900 ) icat = 10 ;
    if ( bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta < 0.900 ) icat = 11 ;
    if ( bdt[0] >= 0.730 ) icat = 12 ;
    //

    if (icat>=0)catStr=Form("cat%d",icat);
    if (doSync){
        SetTreeVar("cat",icat);
        SetTreeVar("nbjets",nbjets);
    }

    // SciKit
    if (doScikit)
    {
       
        #ifdef SCIKIT_TIMING
        //static double time_bdtg=0;
        //static double time_sgd=0;
        //static double time_svr=0;
        static double time_mlp=0;
        //static double time_keras=0;
        TStopwatch sw;
        #endif
        

        scikit.clear();
        x.clear();
        //x.push_back( *(float*)varValues_.GetPointer("Hpt") );
        x.push_back( Hmm.Pt());
        x.push_back( Hmm.Eta());
        x.push_back( Hmm.Phi());
        x.push_back( mjj_detajj[0].first );
        x.push_back( mjj_detajj[1].first );
        x.push_back( mjj_detajj[0].second );
        x.push_back( mjj_detajj[1].second );
        x.push_back( fabs(mu0->DeltaPhi(*mu1)) );
        x.push_back( fabs(mu0->Eta()-mu1->Eta()));
        x.push_back( nbjets );


        #ifdef SCIKIT_TIMING
            sw.Reset();sw.Start();
        #endif

        //float bdtg= py->Eval("bdtg.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]"); // THIS IS VERY SLOW
        //float bdtg=-999;
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_bdtg += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        //float sgd= py->Eval("sgd.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]");
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_sgd += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        //float svr= py->Eval("svr.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]");
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_svr += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        float mlp= py->Eval("mlp.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]");
        #ifdef SCIKIT_TIMING
            sw.Stop(); time_mlp += sw.CpuTime(); sw.Reset(); sw.Start();
        #endif
        //float keras= py->Eval("keras.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]");
        ////float keras=-999;
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_keras += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        //scikit.push_back(bdtg);
        //scikit.push_back(sgd);
        //scikit.push_back(svr);
        scikit.push_back(mlp);
        //scikit.push_back(keras);

        #ifdef SCIKIT_TIMING
           //Log(__FUNCTION__,"SCIKIT",Form("Scikit values are: bdtg=%f sgd=%f svr=%f mlp=%f  ",bdtg,svr,sgd,mlp) );
           //Log(__FUNCTION__,"TIME",Form("BDTG Time: %.3lf",time_bdtg) );
           //Log(__FUNCTION__,"TIME",Form("SGD Time: %.3lf",time_sgd) );
           //Log(__FUNCTION__,"TIME",Form("SVR Time: %.3lf",time_svr) );
           Log(__FUNCTION__,"TIME",Form("MLP Time: %.3lf",time_mlp) );
           //Log(__FUNCTION__,"TIME",Form("KERAS Time: %.3lf",time_keras) );
        #endif
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + catStr);
    return catStr;
}

void HmumuAnalysis::AddVariable( string name, char type){ 
    Log(__FUNCTION__,"INFO","Adding variable '"+name+"'");
    varValues_.Add(name,type); 
    if ( type == 'I') for(auto& r : readers_ ) r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    else { 
        Log(__FUNCTION__,"ERROR",string("type '") + type + "' not supported");
    }
}//end add variable

void HmumuAnalysis::AddSpectator( string name, char type){ 
    Log(__FUNCTION__,"INFO","Adding spectator '"+name+"'");
    varValues_.Add(name,type); 
    if ( type == 'I') for(auto& r : readers_ ) r -> AddSpectator(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddSpectator(name.c_str(),  (float*)varValues_.GetPointer(name));
    else { 
        Log(__FUNCTION__,"ERROR",string("type '") + type + "' not supported");
    }
}//end add variable

void HmumuAnalysis::InitScikit(){

    //discr.push_back("BTDG");
    //discr.push_back("SGD");
    //discr.push_back("SVR");
    discr.push_back("MLP");
    //discr.push_back("KERAS");

    py . reset(new TPython);
    py -> Exec("from sklearn.ensemble import RandomForestClassifier");
    py -> Exec("from sklearn.svm import SVC");
    py -> Exec("from sklearn.svm import SVR");
    py -> Exec("from sklearn.neural_network import MLPClassifier");
    py -> Exec("from sklearn.neural_network import MLPRegressor");
    py -> Exec("from sklearn.linear_model import SGDClassifier");
    py -> Exec("from sklearn.linear_model import SGDRegressor");
    py -> Exec("from sklearn.linear_model import PassiveAggressiveClassifier");
    py -> Exec("from sklearn.linear_model import PassiveAggressiveRegressor");
    py -> Exec("from sklearn.linear_model import Perceptron");
    py -> Exec("from sklearn.linear_model import Ridge");
    py -> Exec("from sklearn.linear_model import ElasticNet");

    // KERAS
    //py -> Exec("from keras.models import Sequential");
    //py -> Exec("from keras.layers import Dense, Activation");
    //py -> Exec("from keras.wrappers.scikit_learn import KerasRegressor");
    //py -> Exec("from sklearn.preprocessing import StandardScaler");
    //py -> Exec("from sklearn.pipeline import Pipeline");

    py -> Exec("from sklearn.externals import joblib ");

    //py ->Exec("rfc=joblib.load('aux/hmm/RFC.pkl')");
    py ->Exec("mlp=joblib.load('aux/hmm/MLP.pkl')");
    //py ->Exec("svr=joblib.load('aux/hmm/SVR.pkl')");
    //py ->Exec("par=joblib.load('aux/hmm/PAR.pkl')");
    //py ->Exec("sgd=joblib.load('aux/hmm/SGD.pkl')");
    //py ->Exec("bdtg=joblib.load('aux/hmm/BDTG.pkl')");
    //py ->Exec("keras=joblib.load('aux/hmm/KERAS.pkl')");
    //py ->Exec("ridge=joblib.load('aux/hmm/Ridge.pkl')");
    //py ->Exec("en=joblib.load('aux/hmm/EN.pkl')");

    // Make sure we can use x[...] inside
    PyObject* pyx = py->ObjectProxy_FromVoidPtr(&x, "std::vector<float>");
    PyObject* pymain = PyImport_ImportModule("__main__");

    PyModule_AddObject(pymain, "x", pyx);

    Py_DECREF(pymain);
}

void HmumuAnalysis::InitTmva(){
    Log(__FUNCTION__,"INFO","Init Reader");
    TMVA::Tools::Instance();
    for( size_t i=0;i<weights.size() ;++i)
        readers_ . push_back( new TMVA::Reader() );

    //AddVariable("mu1_abs_eta",'F');    
    //AddVariable("mu2_abs_eta",'F');    
    AddVariable("dimu_pt",'F');    
    AddVariable("dimu_eta",'F');    
    AddVariable("dimu_abs_dEta",'F');    
    AddVariable("dimu_abs_dPhi",'F');
    //AddVariable("dimu_dPhiStar",'F');
    //AddVariable("jet1_pt",'F');    
    //AddVariable("jet2_pt",'F');    
    AddVariable("jet1_eta",'F');    
    AddVariable("jet2_eta",'F');    
    AddVariable("dijet1_mass",'F');    
    AddVariable("dijet2_mass",'F');    
    //AddVariable("dijet3_mass",'F');    
    //AddVariable("dijet4_mass",'F');    
    AddVariable("dijet1_abs_dEta",'F');    
    AddVariable("dijet2_abs_dEta",'F');    
    //AddVariable("dijet3_abs_dEta",'F');    
    //AddVariable("dijet4_abs_dEta",'F');    
    AddVariable("nJetsCent",'F');    
    AddVariable("nJetsFwd",'F');    
    AddVariable("nBMed",'F');    
    AddVariable("MET",'F');    

    AddSpectator("samp_ID",'F');SetVariable("samp_ID",0);
    AddSpectator("samp_wgt",'F');SetVariable("samp_wgt",1.);
    AddSpectator("res_wgt",'F');SetVariable("res_wgt",1.);
    AddSpectator("LHE_HT",'F');SetVariable("LHE_HT",1.);
    AddSpectator("dimu_mass_Roch",'F');SetVariable("dimu_mass_Roch",125.);
    AddSpectator("BASE_cat",'F');SetVariable("BASE_cat",1);

    // load weights
    for( size_t i=0;i<weights.size() ;++i)
        {
            Log(__FUNCTION__,"INFO",Form("Loading weights idx=%d:",i)+weights[i]);
            readers_[i]->BookMVA("BDTG_default",weights[i].c_str());
        }

}

void HmumuAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");
    rnd_ . reset( new TRandom3() ) ;

    // define categories -- for booking histos
    Log(__FUNCTION__,"INFO",Form("Loading Configuration with catType=%d",catType) );
    if (catType>=1)
    {
        categories_.clear();
        for (int i=0;i<16;++i)
            categories_.push_back(Form("cat%d",i));
        InitTmva();
        if (doScikit)InitScikit();
    }
    else
    {
        vector< string> mu_cats{"BB","BO","BE","OO","OE","EE"};
        vector<string> vbf_cats{"VBF0","GF","VBF1","OneB","Untag0","Untag1"};

        categories_.clear();
        for( const auto & m : mu_cats)
        for( const auto & v : vbf_cats)
        {
            categories_.push_back(v + "_" + m);
        }
    }
    //

	Log(__FUNCTION__,"INFO","Booking Histo Mass");
    for ( string l : AllLabel()  ) {
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    Book ("HmumuAnalysis/Vars/Mmm_NoTrigger_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    //Book ("HmumuAnalysis/Vars/Mmm_DoubleMuonTrigger_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    // 
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
	    Book ("HmumuAnalysis/Vars/MetOnZ_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnZ_rw_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_rw_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnH_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/BdtOnZ_"+ l ,"Bdt On Z (70-110);Bdt;Events", 1000,-1,1);

	    Book ("HmumuAnalysis/Vars/BdtOnH_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/BdtOnH_BB_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);

        //Scikit
        if(doScikit){
            for(const auto& s : discr)
            {
                Book ("HmumuAnalysis/Vars/"+s+"OnZ_"+ l ,s+" On Z (70-110);"+s+";Events", 1000,-1,1.0001);
                Book ("HmumuAnalysis/Vars/"+s+"OnH_"+ l ,s+" On Hmm (110-150);"+s+";Events", 1000,-1,1.0001);
                Book ("HmumuAnalysis/Vars/"+s+"OnH_BB_"+ l ,s+" On Hmm (110-150);"+s+";Events", 1000,-1,1.0001);
            }
        }
        // --- histograms for limits extraction
        for(const auto & c : categories_)
        {
	        //Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 960,60,300); // every 4 (old16) per GeV
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 2000,60,160); // every 4 (old16) per GeV
	        //Book2D ("HmumuAnalysis/Vars/JetEtaPhi_"+ c + "_"+ l ,"Jet EtaPhi; eta; phi;Events", 100,-5,5,100,-3.1415,3.1415); // CHECK2D
            // for systematics, only counts the total
        }

        for(const auto & c : {string("ttHHadr"),string("ttHLep"),string("superPure")})
        {
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 2000,60,160); // every 4 (old16) per GeV
        }
    }

    if (doSync){
        InitTree("hmm");
        Branch("hmm","cat",'I');
        Branch("hmm","mcWeight",'I');
        Branch("hmm","puWeight",'I');
        Branch("hmm","eventNum",'I');
        Branch("hmm","runNum",'I');
        Branch("hmm","lumiNum",'I');
        Branch("hmm","pt1",'F');
        Branch("hmm","pt2",'F');
        Branch("hmm","eta1",'F');
        Branch("hmm","eta2",'F');
        Branch("hmm","phi1",'F');
        Branch("hmm","phi2",'F');
        Branch("hmm","mass",'F');
        Branch("hmm","met",'F');
        Branch("hmm","Hpt",'F');
        Branch("hmm","njets",'I');
        Branch("hmm","nbjets",'I');
        Branch("hmm","jet_pt",'d',20,"njets");
        Branch("hmm","jet_eta",'d',20,"njets");
        Branch("hmm","jet_phi",'d',20,"njets");
        Branch("hmm","pass_recomuons",'I');
        Branch("hmm","pass_asymmcuts",'I');
        Branch("hmm","pass_trigger",'I');
        Branch("hmm","pass_trigger1",'I');
        Branch("hmm","pass_trigger2",'I');
        Branch("hmm","pass_leptonveto",'I');
        Branch("hmm","pass_all",'I');
        // for training
        Branch("hmm","mc",'I');
        Branch("hmm","Heta",'F');
        Branch("hmm","Hphi",'F');
        Branch("hmm","mjj_1",'F');
        Branch("hmm","mjj_2",'F');
        Branch("hmm","detajj_1",'F');
        Branch("hmm","detajj_2",'F');
        Branch("hmm","deltaphi",'F');
        Branch("hmm","deltaeta",'F');
        Branch("hmm","bdt",'F');
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

int HmumuAnalysis::analyze(Event *e, string systname)
{

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );


    if (doEvenOnly){
        if ( label.find("HToMuMu") != string::npos )
        {
            if (  (e->eventNum()&1 ) == 0 ) e->ApplySF("double");
            else {
                return 0;
            }
        }
    }

    if (doOddOnly and not doEvenOnly){
        if (  (e->eventNum()&1 ) == 1 ) e->ApplySF("double");
        else {
            return 0;
        }
    }

    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    /*
     * TOP PT REWEIGHT
     */

    e->ApplyTopReweight();

    /*
     * HIGGS REWEIGHT -- UNCERTAINTIES
     */
    if (not e->IsRealData()){

        HTXS::HiggsClassification  hc;
        if (label.find( "GluGlu_HToMuMu") != string::npos) hc.prodMode = HTXS::GGF;
        else if (label.find( "VBF_HToMuMu") != string::npos) hc.prodMode = HTXS::VBF;
        else if (label.find( "ZH_HToMuMu") != string::npos) hc.prodMode = HTXS::GG2ZH; // I should look into the PDF productions GG2ZH vs QQ2ZH
        else if (label.find( "WMinusH_HToMuMu") != string::npos) hc.prodMode = HTXS::WH;
        else if (label.find( "WPlusH_HToMuMu") != string::npos) hc.prodMode = HTXS::WH;
        else if (label.find( "ttH_HToMuMu") != string::npos) hc.prodMode = HTXS::TTH;
        else hc.prodMode = HTXS::UNKNOWN;

        
        if (hc.prodMode != HTXS::UNKNOWN)
        {
            GenParticle* Higgs = NULL;
            GenParticle* mu0T=NULL;
            GenParticle* mu1T=NULL;
            GenParticle* V = NULL;
            int idxV=-1;
            bool quarkDecayed=false;
            int Njets30=0;
            for( int ig=0;;++ig){
                GenParticle *g =  e->GetGenParticle(ig);
                if (g==NULL) break;
                if (abs(g->GetPdgId())==25 and abs(g->GetParentPdgId() ) != 25) Higgs=g;
                if (g->GetPdgId() == 13 and g->IsPromptFinalState() and e->GenParticleDecayedFrom(ig,25)) mu0T = g;
                if (g->GetPdgId() == -13 and g->IsPromptFinalState() and e->GenParticleDecayedFrom(ig,25)) mu1T = g;
                if (abs(g->GetPdgId())==23 and abs(g->GetParentPdgId() ) != 23 and not e->GenParticleDecayedFrom(ig,6)) {V=g;idxV=ig;}
                if (abs(g->GetPdgId())==24 and abs(g->GetParentPdgId() ) != 24 and not e->GenParticleDecayedFrom(ig,6)) { V=g;idxV=ig;}

            }
            for( int ig=0;;++ig){
                GenParticle *g =  e->GetGenParticle(ig);
                if (g==NULL) break;
                int idx;
                if (g->GetPdgId() <=5 and  e->GenParticleDecayedFrom(ig,23,idx) and idx==idxV) quarkDecayed=true;
                if (g->GetPdgId() <=5 and  e->GenParticleDecayedFrom(ig,24,idx) and idx==idxV) quarkDecayed=true;
            }
            
            if (Higgs != NULL) hc.higgs =  Higgs->GetP4();
            if (mu0T != NULL) hc. p4decay_higgs = mu0T -> GetP4();
            if (V != NULL) hc. V = V -> GetP4();

            // Get Njets 30
            vector<TLorentzVector> jets;
            for(int ij =0; ;++ij)
            {
                GenJet *j = e->GetGenJet(ij); 
                if (j==NULL) break;
                if ( mu0T and j->DeltaR(mu0T) < 0.3) continue;
                if ( mu1T and j->DeltaR(mu1T) < 0.3) continue;
                if (j->Pt() < 30 ) continue;
                if (abs(j->Eta()) > 5 ) continue;
                jets.push_back( j->GetP4() ) ;
            }

            // classify, only stage1  30 GeV
            hc.stage1_cat_pTjet30GeV = HTXS::Stage1::UNKNOWN;
            hc.stage1_cat_pTjet30GeV = HTXS::getStage1Category( hc.prodMode,hc.higgs, jets,hc.V,quarkDecayed);

            SF_WG1* sf_wg1 = dynamic_cast<SF_WG1*> (e->GetWeight()->GetSF("wg1") ); 
            sf_wg1->SetNjets30(jets.size());
            sf_wg1->SetPTH(hc.higgs.Pt());
            sf_wg1->SetSTXS(hc.stage1_cat_pTjet30GeV);

            //Log(__FUNCTION__,"DEBUG","------------ HTXS -------------"  );
            //Log(__FUNCTION__,"DEBUG","label="+label );
            //Log(__FUNCTION__,"DEBUG","systematics="+systname );
            //Log(__FUNCTION__,"DEBUG",Form("HiggsProd Mode=%d",int(hc.prodMode)) );
            //Log(__FUNCTION__,"DEBUG",Form("NGenJets=%u",jets.size())    );
            //Log(__FUNCTION__,"DEBUG",Form("mu0 = %f",(mu0T==NULL)?-999.:mu0T->Pt())    );
            //Log(__FUNCTION__,"DEBUG",Form("mu1 = %f",(mu1T==NULL)?-999.:mu1T->Pt())    );
            //Log(__FUNCTION__,"DEBUG",Form("V = %f",(V==NULL)?-999.:V->Pt())    );
            //Log(__FUNCTION__,"DEBUG",Form("V->Q = %d",int(quarkDecayed))    );
            //Log(__FUNCTION__,"DEBUG",Form("HiggsPt=%f",hc.higgs.Pt())   );
            //Log(__FUNCTION__,"DEBUG",Form("STXS Cat=%d",int(hc.stage1_cat_pTjet30GeV)) ) ;
            //Log(__FUNCTION__,"DEBUG",Form("sf wg1=%lf",sf_wg1->get()));
            //sf_wg1->print();
            //Log(__FUNCTION__,"DEBUG","-------------------------------"  );
            
            e->ApplySF("wg1");
        }
    }
    /*****************/


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon0: ");
    Lepton*mu0 = e->GetMuon(0);
    if (VERBOSE and mu0)Log(__FUNCTION__,"DEBUG",Form("GetMuon0: pt=%f",mu0->Pt()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon1: ");
    Lepton*mu1 = e->GetMuon(1);
    int il2=1;
    while (mu0 != NULL and mu1!= NULL and mu1->Charge() * mu0->Charge() != -1) { mu1= e->GetMuon(++il2);} // fully combinatorics on the second muon
    if (VERBOSE and mu1)Log(__FUNCTION__,"DEBUG",Form("GetMuon1: pt=%f",mu1->Pt()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetJets: ");

    vector<Jet*> selectedJets;
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        selectedJets.push_back(e->GetJet(i));
    }

    // ------------------- DIRTY CATEGORIES for studies
    int nbloose=0;
    for(int i=0;i < e->Njets() ;++i){
        Jet *j= e->GetJet(i);
        if ( not j->IsJet() ) continue;
        if (fabs(j->Eta() ) >2.4) continue;
        if (j->Btag() > 0.5426 ) continue;
        nbloose += 1;
    }

    string categoryDirty=""; // this may double count
    if (e->Bjets()>0 and e->Njets() >4  )
    {
        categoryDirty="ttHHadr"; 
    }
    
    //if ( nbloose >0  and e->Njets() >1  and e->Nleps()>2  )
    if ( e->Bjets() >0  and e->Njets() >1  and e->Nleps()>2  )
    {
        categoryDirty ="ttHLep"; 
    }
    else if (catType==2 and bdt.size() >1 and bdt[0]>.84)
    {
        categoryDirty="superPure";
    }

    // ------------------------------------

    string category;
    if (catType==1) category = CategoryAutoCat(mu0,mu1,selectedJets,e->GetMet().Pt() ) ;
    else if (catType==2) category = CategoryBdt(mu0,mu1,selectedJets,e->GetMet().Pt());
    else if (catType==3) { 
        category = CategoryBdt(mu0,mu1,selectedJets,e->GetMet().Pt()); 
        if (e->Bjets()>0 and e->Njets() >4) category = "cat13";  // ttHHadr
        if (e->Bjets()>0 and e->Njets() >1  and e->Nleps()>2 ) category="cat14";  // veto Njets> 4
    } 
    else category = Category(mu0, mu1, selectedJets);


    if (true) // CSV-SF for passing loose,medium or tigth cuts
    {
        e->ApplyBTagSF(1); //0 loose, 1 medium, 2 tight
    }
    else // CSV-Reweight
    {
        auto sf=dynamic_cast<SF_CSVReweight*>(e->GetWeight()->GetSF("btag-reweight"));
        if (sf == NULL)  Log(__FUNCTION__,"ERROR","Unable to find btag reweight sf");

        for (auto& j: selectedJets)
        {
            sf->add_to_sf(j->Pt() , j->Eta(), j->Btag() , j->Flavor());
        }

        sf->set();
        e->ApplySF("btag-reweight"); 
    }

    double w_BCDEF = 0.5498;
    double sfTrigger=1.0;
     
    // Trigger SF
    if (true){
        SF* sf0_runGH =e->GetWeight()->GetSF("muTRG_runGH");
        SF* sf0_runBCDEF =e->GetWeight()->GetSF("muTRG_runBCDEF");

        if (sf0_runGH== NULL) cout<<"Unable to get SF TRG GH"<<endl;
        if (sf0_runBCDEF== NULL) cout<<"Unable to get SF TRG BCDEF"<<endl;

        SF_PtEta_And_Eff* sf_runGH = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runGH);
        SF_PtEta_And_Eff* sf_runBCDEF = dynamic_cast<SF_PtEta_And_Eff*>(sf0_runBCDEF);
        if (sf_runGH== NULL) {cout<<"Unable to convert SF TRG GH:" <<endl; sf0_runGH->print(); }
        if (sf_runBCDEF== NULL) {cout<<"Unable to convert SF TRG BCDEF:" <<endl; sf0_runBCDEF->print(); }

        double effdata_runGH=1.0,effmc_runGH =1.0;
        double effdata_runBCDEF=1.0,effmc_runBCDEF =1.0;

        bool changed=false;
        if (mu0)
        {
            changed=true;
            sf_runGH->set( std::max(mu0->Pt(),float(26.1)), fabs(mu0->Eta()) );
            //effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*sf_runGH->getDataErr() ) ;
            effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*0.005 ) ; // 0.5%
            effmc_runGH *= 1.- (sf_runGH->getMCEff()) ;

            sf_runBCDEF->set( std::max(mu0->Pt(),float(26.1)), fabs(mu0->Eta()) );
            //effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runBCDEF->syst*sf_runBCDEF->getDataErr() ) ;
            effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runGH->syst*0.005 ) ; // 0.5% correlated with runGH
            effmc_runBCDEF *= 1.- (sf_runBCDEF->getMCEff()) ;
        }

        if (mu1 and mu1->Pt() > 26)
        {
            changed=true;

            sf_runGH->set( std::max(mu1->Pt(),float(26.1)) ,fabs(mu1->Eta()) );
            //effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*sf_runGH->getDataErr() ) ;
            effdata_runGH *= 1.-(sf_runGH->getDataEff()+sf_runGH->syst*0.005 ) ; // 0.05%
            effmc_runGH *= 1.- (sf_runGH->getMCEff()) ;

            sf_runBCDEF->set( std::max(mu1->Pt(),float(26.1)) ,fabs(mu1->Eta()) );
            //effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runBCDEF->syst*sf_runBCDEF->getDataErr() ) ;
            effdata_runBCDEF *= 1.-(sf_runBCDEF->getDataEff()+sf_runGH->syst*0.005) ; // 0.5% correlated runGH runBCDEF
            effmc_runBCDEF *= 1.- (sf_runBCDEF->getMCEff()) ;
        }

        if(changed)
        {
            effdata_runGH = 1.-effdata_runGH;
            effmc_runGH = 1.-effmc_runGH;

            effdata_runBCDEF = 1.-effdata_runBCDEF;
            effmc_runBCDEF = 1.-effmc_runBCDEF;
        }

        //SF* sftrig= e->GetWeight()->GetSF("dummy"); // base, I modify this one
        //sftrig->syst=0;
        //sftrig->sf=effdata/effmc;
        //sftrig->err = 0.0;
        //e->ApplySF("dummy");
        //
        sfTrigger = (1.-w_BCDEF)* effdata_runGH / effmc_runGH + w_BCDEF*effdata_runBCDEF/effmc_runBCDEF;
    }
    
    // ID
    double sfId=1.0;

    if (mu0)
    {
        e->SetPtEtaSF("muID_runBCDEF", std::max(mu0->Pt(),float(20.1)),fabs(mu0->Eta()) );
        //float sf1 = e->GetWeight()->GetSF("muID_runBCDEF")->get(); // include syst
        float sf1 = e->GetWeight()->GetSF("muID_runBCDEF")->sf + e->GetWeight()->GetSF("muID_runGH")->syst*0.01; // 1% correlated runGH
        e->SetPtEtaSF("muID_runGH", std::max(mu0->Pt(),float(20.1)),fabs(mu0->Eta()) );
        //float sf2 = e->GetWeight()->GetSF("muID_runGH")->get();
        float sf2 = e->GetWeight()->GetSF("muID_runGH")->sf + e->GetWeight()->GetSF("muID_runGH")->syst*0.01; // 1%
        sfId *= w_BCDEF*sf1 + (1.-w_BCDEF) *sf2;
    }
    if (mu1)
    {
        e->SetPtEtaSF("muID_runBCDEF", std::max(mu1->Pt(),float(20.1)),fabs(mu1->Eta()) );
        //float sf1 = e->GetWeight()->GetSF("muID_runBCDEF")->get();
        float sf1 = e->GetWeight()->GetSF("muID_runBCDEF")->sf + e->GetWeight()->GetSF("muID_runGH")->syst *0.01; // 1% correlated runGH
        e->SetPtEtaSF("muID_runGH", std::max(mu1->Pt(),float(20.1)),fabs(mu1->Eta()) );
        //float sf2 = e->GetWeight()->GetSF("muID_runGH")->get();
        float sf2 = e->GetWeight()->GetSF("muID_runGH")->sf + e->GetWeight()->GetSF("muID_runGH")->syst *0.01;
        sfId *= w_BCDEF*sf1 + (1.-w_BCDEF) *sf2;
    }

    double sfIso=1.0;
    // ISO Loose
    if (mu0)
    {
        e->SetPtEtaSF("muISOloose_runBCDEF",std::max( mu0->Pt(),float(20.1)),fabs(mu0->Eta()) );
        //float sf1 = e->GetWeight()->GetSF("muISOloose_runBCDEF")->get();
        float sf1 = e->GetWeight()->GetSF("muISOloose_runBCDEF")->sf + e->GetWeight()->GetSF("muISOloose_runGH")->syst*0.005; // .5% correlated runGH
        e->SetPtEtaSF("muISOloose_runGH", std::max(mu0->Pt(),float(20.1)),fabs(mu0->Eta()) );
        //float sf2 = e->GetWeight()->GetSF("muISOloose_runGH")->get();
        float sf2 = e->GetWeight()->GetSF("muISOloose_runGH")->sf + e->GetWeight()->GetSF("muISOloose_runGH")->syst*0.005; // .5%
        sfIso *= w_BCDEF*sf1 + (1.-w_BCDEF) *sf2;
    }
    if (mu1)
    {
        e->SetPtEtaSF("muISOloose_runBCDEF", std::max(mu1->Pt(),float(20.1)),fabs(mu1->Eta()) );
        //float sf1 = e->GetWeight()->GetSF("muISOloose_runBCDEF")->get();
        float sf1 = e->GetWeight()->GetSF("muISOloose_runBCDEF")->sf + e->GetWeight()->GetSF("muISOloose_runGH")->syst*0.005; // .5% correlated runGH
        e->SetPtEtaSF("muISOloose_runGH", std::max(mu1->Pt(),float(20.1)),fabs(mu1->Eta()) );
        //float sf2 = e->GetWeight()->GetSF("muISOloose_runGH")->get();
        float sf2 = e->GetWeight()->GetSF("muISOloose_runGH")->sf + e->GetWeight()->GetSF("muISOloose_runGH")->syst*0.005 ; // .5%
        sfIso *= w_BCDEF*sf1 + (1.-w_BCDEF) *sf2;
    }
    
    // apply trigger, id and iso sf
    {
        SF* sf= e->GetWeight()->GetSF("dummy"); // base, I modify this one
        sf->syst=0;
        sf->sf= sfId*sfIso*sfTrigger; //syst already included
        sf->err = 0.0;
        e->ApplySF("dummy");
    }
    
    

    //
    // preselection
    bool recoMuons= mu0 != NULL and mu1 !=NULL; 
    if (recoMuons and mu0->Charge() * mu1->Charge() != -1 ) recoMuons=false; // 

    // Trigger
    bool passAsymmPtCuts = (recoMuons and  mu0->Pt() >26 );
    bool passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); 

    bool passTrigger1{false}, passTrigger2{false};

    if (  recoMuons and ( (label.find("HToMuMu") != string::npos or e->IsRealData()) ) ) // 
    {
        //cout <<" DOING TRIGGER MATCHING "<<endl;
        bool passTriggerEvent = passTrigger;
        passTrigger1 = (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;
        //if (mu1->Pt() > 26 ) 
        passTrigger2 = (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
        passTrigger=passTrigger1 or passTrigger2;

        if (passTriggerEvent and not passTrigger) Log(__FUNCTION__,"INFO","Fail to trigger event due to trigger matching");
        if (not passTriggerEvent and passTrigger) Log(__FUNCTION__,"ERROR","Event triggered by object but not globally");
    }

    bool passLeptonVeto= true;
    if (e->GetMuon(2) != NULL) passLeptonVeto=false;

    if (recoMuons)
    {
        for(int i=0; ; ++i) { 
            Lepton *el= e->GetElectron(i);
            if (el == NULL) break;
            //if (el->Pt() >15) passLeptonVeto=false; // FIXME 10 ?!?
            //#warning ABSURD_ELE_VETO
            //|eta| < 1.4442 || 1.566 <|eta| <2.5 
            // DR with muon 
            if (el->Pt() >10 and 
                    (fabs(el->Eta()) <1.4442 or fabs(el->Eta())>1.566) and 
                    fabs(el->Eta())<2.5 and
                    mu0->DeltaR(el) >0.4 and mu1->DeltaR(el) >0.4
               ) passLeptonVeto=false; // FIXME 10 ?!?
        }
    }


    if (doSync) {
        SetTreeVar("eventNum",e->eventNum());
        SetTreeVar("runNum",e->runNum());
        SetTreeVar("lumiNum",e->lumiNum());
        if (e->IsRealData()){
            SetTreeVar("mc",0);
            SetTreeVar("mcWeight",1.);
            SetTreeVar("puWeight",1.);
        }
        else {
            SetTreeVar("mcWeight",e->GetWeight()->GetBareMCWeight() * e->GetWeight()->GetBareMCXsec() / e->GetWeight()->GetBareNevents());
            SetTreeVar("puWeight",e->GetWeight()->GetBarePUWeight());
            int mc=0;
            if (label.find( "GluGlu_HToMuMu") != string::npos) mc -=10;
            if (label.find( "VBF_HToMuMu") != string::npos) mc -=20;
            if (label.find( "ZH_HToMuMu") != string::npos) mc -=30;
            if (label.find( "WMinusH_HToMuMu") != string::npos) mc -=40;
            if (label.find( "WPlusH_HToMuMu") != string::npos) mc -=50;
            if (label.find( "ttH_HToMuMu") != string::npos) mc -=60;
            if (label.find( "M120") != string::npos) mc -=1;
            if (label.find( "M125") != string::npos) mc -=2;
            if (label.find( "M130") != string::npos) mc -=3;
            if (label == "DY") mc =10;
            if (label == "TT") mc =20;
            if (label == "ST") mc =21;
            if (label == "WW") mc =30;
            if (label == "WZ") mc =31;
            if (label == "ZZ") mc =32;
            if (label == "EWK_LLJJ") mc =40;
            if (mc == 0) mc =1;
            SetTreeVar("mc",mc);
        }
        SetTreeVar("pass_recomuons",recoMuons);
        SetTreeVar("pass_asymmcuts",passAsymmPtCuts);
        SetTreeVar("pass_trigger",passTrigger);
        SetTreeVar("pass_trigger1",passTrigger1);
        SetTreeVar("pass_trigger2",passTrigger2);
        SetTreeVar("pass_leptonveto",passLeptonVeto);
        if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto and category != "" ) SetTreeVar("pass_all",1);
        else SetTreeVar("pass_all",0); 
        FillTree("hmm");
    }

    if ( recoMuons)
    {
        Object Z(*mu0);
        Z += *mu1;
        mass_=Z.M();
        pt_ = Z.Pt();
    }

    if ( recoMuons and passAsymmPtCuts)
    {
        if(Unblind(e))Fill("HmumuAnalysis/Vars/Mmm_NoTrigger_"+ label,systname, mass_,e->weight()) ;
        //if(Unblind(e) and (passTrigger or e->IsTriggered("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v") or e->IsTriggered("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v") or e->IsTriggered("HLT_Mu45_eta2p1_v") or e->IsTriggered("HLT_Mu50_v") ))Fill("HmumuAnalysis/Vars/Mmm_DoubleMuonTrigger_"+ label,systname, mass_,e->weight()) ;
    }

    // -- FINAL SELECTION --
    //if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto)
    if ( recoMuons and passTrigger and passAsymmPtCuts)
    {
        cut.SetCutBit(Leptons);
        if (passTrigger) cut.SetCutBit(Trigger);

        //Object Z(*mu0);
        //Z += *mu1;
        //mass_=Z.M();

       	//if (cut.passAllUpTo(Trigger) ){
        //}
        if (VERBOSE)Log(__FUNCTION__,"DEBUG","event pass selection");

        float zptrw = 1.;
        if (label == "DY") zptrw = getZPtReweight(pt_ );
        //cout<<"ZPtRWgh is "<<zptrw<<endl;
        
        if (mass_ >= 70 and mass_<110){
            Fill("HmumuAnalysis/Vars/MetOnZ_"+ label,systname, e->GetMet().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/MetOnZ_rw_"+ label,systname, e->GetMet().Pt(),e->weight()*zptrw);
            Fill("HmumuAnalysis/Vars/PtOnZ_"+ label,systname, pt_ ,e->weight());

            if(catType==2)Fill("HmumuAnalysis/Vars/BdtOnZ_"+ label,systname, bdt[0] ,e->weight());
            if (doScikit and catType==2){
                for(size_t i=0;i<discr.size();++i)
                {
                    const auto& s = discr[i];
                    Fill("HmumuAnalysis/Vars/"+s+"OnZ_"+ label,systname, scikit[i] ,e->weight());
                }
            }
        }
        if (mass_ >= 110 and mass_<150){
            Fill("HmumuAnalysis/Vars/MetOnH_"+ label,systname, e->GetMet().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/MetOnH_rw_"+ label,systname, e->GetMet().Pt(),e->weight()*zptrw);
            Fill("HmumuAnalysis/Vars/PtOnH_"+ label,systname, pt_,e->weight());
            if(catType==2)Fill("HmumuAnalysis/Vars/BdtOnH_"+ label,systname, bdt[0] ,e->weight());
            if(catType==2 and fabs(mu0->Eta())<0.8 and fabs(mu1->Eta())<0.8)
                Fill("HmumuAnalysis/Vars/BdtOnH_BB_"+ label,systname, bdt[0] ,e->weight());
            if (doScikit and catType==2){
                for(size_t i=0;i<discr.size();++i)
                {
                    const auto& s = discr[i];
                    Fill("HmumuAnalysis/Vars/"+s+"OnH_"+ label,systname, scikit[i] ,e->weight());
                    if (fabs(mu0->Eta())<0.8 and fabs(mu1->Eta())<0.8)
                        Fill("HmumuAnalysis/Vars/"+s+"OnH_BB_"+ label,systname, scikit[i] ,e->weight());
                }
            }
        }

        //if (category != "" and mass_ > 110 and mass_<150 and (systname=="" or systname=="NONE")){ //CHECK2D
        //        // first key is mjj-> sorting
        //        map<float, pair<float,float>, std::greater<float> > eta_j;
        //        map<float, pair<float,float>, std::greater<float> > phi_j;
        //        map<float, float, std::greater<float> > detajj;

        //        for(unsigned i=0;i<selectedJets.size() ;++i)
        //        for (unsigned j=i+1;j<selectedJets.size();++j)
        //        {
        //            eta_j[ selectedJets[i]->InvMass( selectedJets[j] ) ]  =  pair<float,float>( selectedJets[j]->Eta(), selectedJets[i]->Eta());
        //            phi_j[ selectedJets[i]->InvMass( selectedJets[j] ) ]  =  pair<float,float>( selectedJets[j]->Phi(), selectedJets[i]->Phi());
        //            detajj[ selectedJets[i]->InvMass( selectedJets[j] ) ]  =  fabs(selectedJets[j]->Eta() - selectedJets[i]->Eta()) ;
        //        }
        //        if (eta_j.size() >0)
        //        {
        //            Fill2D("HmumuAnalysis/Vars/JetEtaPhi_"+ category + "_"+ label,systname,(eta_j.begin())->second.first,(phi_j.begin())->second.first,e->weight());
        //            Fill2D("HmumuAnalysis/Vars/JetEtaPhi_"+ category + "_"+ label,systname,(eta_j.begin())->second.second,(phi_j.begin())->second.second,e->weight());
        //        }
        //        if (eta_j.size() >1)
        //        {
        //            Fill2D("HmumuAnalysis/Vars/JetEtaPhi_"+ category + "_"+ label,systname,(++eta_j.begin())->second.first ,(++phi_j.begin())->second.first,e->weight());
        //            Fill2D("HmumuAnalysis/Vars/JetEtaPhi_"+ category + "_"+ label,systname,(++eta_j.begin())->second.second,(++phi_j.begin())->second.second,e->weight());
        //        }
        //}

        if(Unblind(e))Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, mass_,e->weight()) ;
        if(Unblind(e) and category != "")Fill("HmumuAnalysis/Vars/Mmm_"+ category+"_"+ label,systname, mass_,e->weight()) ;
        if(Unblind(e) and (categoryDirty=="ttHHadr" or categoryDirty == "superPure") )Fill("HmumuAnalysis/Vars/Mmm_"+ categoryDirty+"_"+ label,systname, mass_,e->weight()) ;
        if(Unblind(e) and categoryDirty == "ttHLep")Fill("HmumuAnalysis/Vars/Mmm_"+categoryDirty+"_"+ label,systname, mass_,e->weight()) ;
    }


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}

float HmumuAnalysis::getZPtReweight(float Zpt)
{
    if (rzpt_.get() == NULL) {
        rzpt_.reset( new TH1D("r__1","Pt On Z (70-110)",100,0,1000) ) ;
        rzpt_->SetBinContent(1,1.030536);
        rzpt_->SetBinContent(2,0.9691712);
        rzpt_->SetBinContent(3,0.9776648);
        rzpt_->SetBinContent(4,0.9961141);
        rzpt_->SetBinContent(5,1.019372);
        rzpt_->SetBinContent(6,1.034224);
        rzpt_->SetBinContent(7,1.045814);
        rzpt_->SetBinContent(8,1.050968);
        rzpt_->SetBinContent(9,1.054974);
        rzpt_->SetBinContent(10,1.068111);
        rzpt_->SetBinContent(11,1.08331);
        rzpt_->SetBinContent(12,1.096292);
        rzpt_->SetBinContent(13,1.116588);
        rzpt_->SetBinContent(14,1.123308);
        rzpt_->SetBinContent(15,1.121119);
        rzpt_->SetBinContent(16,1.12358);
        rzpt_->SetBinContent(17,1.125133);
        rzpt_->SetBinContent(18,1.119461);
        rzpt_->SetBinContent(19,1.099604);
        rzpt_->SetBinContent(20,1.090148);
        rzpt_->SetBinContent(21,1.10452);
        rzpt_->SetBinContent(22,1.106105);
        rzpt_->SetBinContent(23,1.030325);
        rzpt_->SetBinContent(24,1.061008);
        rzpt_->SetBinContent(25,1.03754);
        rzpt_->SetBinContent(26,1.051334);
        rzpt_->SetBinContent(27,1.025027);
        rzpt_->SetBinContent(28,1.006342);
        rzpt_->SetBinContent(29,1.028017);
        rzpt_->SetBinContent(30,1.032076);
        rzpt_->SetBinContent(31,0.9873907);
        rzpt_->SetBinContent(32,0.9989432);
        rzpt_->SetBinContent(33,1.013082);
        rzpt_->SetBinContent(34,0.9704208);
        rzpt_->SetBinContent(35,0.9751695);
        rzpt_->SetBinContent(36,0.9554832);
        rzpt_->SetBinContent(37,1.03948);
        rzpt_->SetBinContent(38,1.032827);
        rzpt2_.reset( new TF1("myf","1.36959e+00-1.39690e-03*x +x*x*7.11835e-07") );
    }
    if (Zpt < 350) return rzpt_->GetBinContent(rzpt_->FindBin(Zpt));
    else return rzpt2_->Eval (std::min(Zpt,float(1000.))) ;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
