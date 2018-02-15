#include "interface/AnalysisHmumu.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "Python.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"

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
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    j->SetPtCut(30); 
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100);
    // medium CSV
    //j->SetBCut(0.8484); //L=0.5803 , M=  0.8838, T=9693
    //j->SetDeepBCut(-100); // L = 0.1522, M=0.4941, T=0.8001
    j->SetBCut(-100); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetDeepBCut(0.4941); 
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

string HmumuAnalysis::CategoryExclusive(Event *e)
{
    TLorentzVector zero(0,0,0,0);
    /* The exclusive categories may use a different muon selection:
     * they can reset the H definition
     */
    string category = "";

    vector<Lepton*> miniIsoLeptons;
    int nMuons=0;
    for(unsigned il=0 ; ;++il)
    {
        Lepton *l = e->GetBareLepton(il);
        if (l == NULL) break;  // exit strategy
        
        // selection
        if (l->Pt() <10 ) continue;
        //l->SetIsoRelCut(0.25);
        if (abs(l->Eta()) >2.4) continue;
        //medium id
        if( not l->GetMediumId() ) continue;

        //MINI-ISO
        if( l->MiniIsolation() >0.4 ) continue;//loose

        // for muons require tracker and global
        if (l->IsMuonDirty() and not l->GetTrackerMuon())  continue;
        if (l->IsMuonDirty() and not l->GetGlobalMuon())  continue;

        // selected leptons
        miniIsoLeptons.push_back(l);
        if(l->IsMuonDirty()) nMuons+=1;
    }
    // sort miniIsoLeptons by pt
    std::sort(miniIsoLeptons.begin(),miniIsoLeptons.end(),[](Lepton const *a, Lepton const *b ){return a->Pt() > b->Pt();});

    // chek jets 
    vector<Jet*> selectedJets_local;
    int nbjets=0;
    int nbloose=0;
    int nblooseDeep=0; //L = 0.2219, M=0.6324, T=0.8958
    int nbjetsDeep=0;
    for(unsigned ij=0 ; ;++ij)
    {
        Jet *j = e->GetBareJet(ij);
        if (j == NULL) break;  // exit strategy
        //apply jet selection except overlap removal
        if (not j->IsJetExceptValidity()) continue;

        bool isLep=false;
        for(auto l : miniIsoLeptons)
        {
            if (j->DeltaR(l) < 0.4) isLep=true;
        }
        if (isLep) continue;

        //OK
        selectedJets_local.push_back(j);
        
        //count bjets
        if (abs(j->Eta())>2.4 ) continue;
        //if (j->Btag() > 0.5426) nbloose+=1;
        //if (j->Btag() > 0.8484) nbjets+=1;
        if (j->GetDeepB() > 0.2219) nbloose+=1;
        if (j->GetDeepB() > 0.6324) nbjets+=1;
    }
    std::sort(selectedJets_local.begin(),selectedJets_local.end(),[](Jet const *a, Jet const *b ){return a->Pt() > b->Pt();});

    // find reco Muons: Opposite Charge 
    Lepton * mu0_local=NULL;
    Lepton * mu1_local=NULL;

    // Standard selection (two leading muons)
    for( auto& l : miniIsoLeptons)
    {
        if (mu0_local == NULL and l->IsMuonDirty() ) { mu0_local = l;}
        if (mu1_local == NULL and (mu0_local != l ) and l->IsMuonDirty() and mu0_local->Charge()*l->Charge() == -1)  mu1_local = l;
    }
    
    //useless to continue if no OS muons
    if(mu0_local == NULL or mu1_local==NULL)  return "";

    // CHECK first ttH categories, priority is in the order ttHLep, ttHHadr
    if (category == "" and nbloose >0 and miniIsoLeptons.size() >2) category = "ttHLep";
    if (category == "" and nbloose >0 and selectedJets_local.size() > 4) category = "ttHHadr";

    //if ttH fails, check ZH Lept
    if (category == "" and miniIsoLeptons.size() >=4) //ZH Z->ee
    {
        Lepton * e0=NULL;
        Lepton * e1=NULL;

        for( auto& l : miniIsoLeptons)
        {
            if (e0 == NULL and l->IsElectronDirty() ) { e0 = l;}
            if (e1 == NULL and (e0 != l ) and l->IsElectronDirty() and e0->Charge()*l->Charge() == -1)  e1 = l;
        }

        if (e0 != NULL and e1 !=NULL){
            Object Z ;
            Z.SetP4(zero); // make sure it is 0.
            Z+= *e0;
            Z+= *e1;
            if (Z.M() > 91-20 and Z.M() <91+20) category = "ZHLep";// e ch
        }
    }
        
    if (category == "" and miniIsoLeptons.size() >=4) //ZH Z->mm
    {
        // search the less boosted OSSF muon pair in the Z window
        Lepton *muZ0{NULL},*muZ1{NULL};
        float Zpt=-1; //found a Z candidate
        for( unsigned il0=0;il0<miniIsoLeptons.size();++il0)
        {
            Lepton *l0=miniIsoLeptons[il0];
            if(not l0->IsMuonDirty() ) continue;
            for( unsigned il1=il0+1;il1<miniIsoLeptons.size();++il1)
            {
                Lepton *l1=miniIsoLeptons[il1];
                if(not l1->IsMuonDirty() ) continue;
                if(l0->Charge() *l1->Charge() != -1 ) continue;
                // construct Z candidate
                Object Z ;
                Z.SetP4(zero); // make sure it is 0.
                Z+= *l0;
                Z+= *l1;
                if (Z.M() > 91-20 and Z.M() <91+20 and (Zpt <0 or Z.Pt() < Zpt)) 
                {
                    muZ0=l0;
                    muZ1=l1;
                    Zpt = Z.Pt();
                }
            }
        }

        if (Zpt>0) // Zmm Lept
        {
            Lepton * mu0_tmp{NULL};
            Lepton * mu1_tmp{NULL};
            //find H -> m leptons
            for( auto& l : miniIsoLeptons)
            {
                if (l== muZ0 ) continue;
                if (l== muZ1 ) continue;
                if (mu0_tmp == NULL and l->IsMuonDirty() ) { mu0_tmp = l;}
                if (mu1_tmp == NULL and (mu0_tmp != l ) and l->IsMuonDirty() and mu0_tmp->Charge()*l->Charge() == -1)  mu1_tmp = l;
            }

            // if I have both a Z and a H candidate
            if (mu0_tmp and mu1_tmp)
            {
                mu0_local=mu0_tmp;
                mu1_local=mu1_tmp;
                category = "ZHLep";
            }
    
        }

    }// end ZH -> mm logic

    // WH W->ln
    if (category == "" and miniIsoLeptons.size() >=3) //WH Hmm
    {
        Lepton *wLepton{NULL};
        // the two leading leptons  
        for( auto& l : miniIsoLeptons)
        {
            if (l != mu0_local and l!=mu1_local and wLepton == NULL) wLepton=l;
        }
        // compute mt
        if (wLepton != NULL){ // this guard should be always useless
            float met = e->GetMet().Pt();
            float phi = e->GetMet().Phi();
            mt = ChargedHiggs::mt(wLepton->Pt(),met,wLepton->Phi(),phi);
            category = "WHLep";
        }
    }

    // VH Hadronic boosted 
    // if (category == "")
    // {
    // }

    // ggHX: I have a ggH with miniIso but not with std selection
    if (category=="")
    {
        if ( (mu0==NULL or mu1==NULL ) and mu0_local != NULL and mu1_local != NULL)
        {
            category = "ggHX";
        }
    }

    // RESET Hmm WARNING!
    if (category != "")
    {
        Hmm.SetP4(zero); // make sure it is 0.
        mu0 = mu0_local;
        mu1 = mu1_local;
        Hmm += *mu0;
        Hmm += *mu1;
        mass_=Hmm.M();
        pt_ = Hmm.Pt();
        selectedJets.clear();
        for(auto j : selectedJets_local) selectedJets.push_back(j);
        updateMjj();
        isMiniIsoLeptons=true;
    }
    else 
    {   // leptons have been selected using miniIsolation
        isMiniIsoLeptons=false;
    }


    return category;

}

string HmumuAnalysis::Category(Lepton*mu0, Lepton*mu1, const vector<Jet*>& jets){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start Category");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   

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
    }

    if (vbfStr == "" ) return "";
    return vbfStr +"_" + muStr;
}

string HmumuAnalysis::CategoryBdt(Event *e){
    bdt.clear();
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start CategoryBdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   

    SetVariable("dimu_pt",Hmm.Pt());    
    SetVariable("dimu_eta",Hmm.Eta());    
    SetVariable("dimu_abs_dEta",fabs(mu0->Eta()-mu1->Eta()));    
    SetVariable("dimu_abs_dPhi",fabs(mu0->DeltaPhi(*mu1)));
    if (selectedJets.size() >=1)
    {
        //SetVariable("jet1_pt",jets[0]->Pt());    
        SetVariable("jet1_eta",selectedJets[0]->Eta());    
    }
    else{
        //SetVariable("jet1_pt",0.);    
        SetVariable("jet1_eta",-5.);    
    }
    if (selectedJets.size() >=2)
    {
        //SetVariable("jet2_pt",jets[1]->Pt());    
        SetVariable("jet2_eta",selectedJets[1]->Eta());    
    }   
    else
    {
        //SetVariable("jet2_pt",0.);    
        SetVariable("jet2_eta",-5.);    
    }   

    SetVariable("dijet1_mass", (mjj.size()>0) ? mjj[0].first:-1);    
    SetVariable("dijet2_mass", (mjj.size()>1) ? mjj[1].first:-1);    

    float deta1=-1;
    if (mjj.size()> 0) deta1 = fabs( selectedJets[mjj[0].second.first]->Eta() - selectedJets[mjj[0].second.second]->Eta());
    float deta2=-1;
    if (mjj.size()> 1) deta2 = fabs( selectedJets[mjj[1].second.first]->Eta() - selectedJets[mjj[1].second.second]->Eta());
    SetVariable("dijet1_abs_dEta",deta1);    
    SetVariable("dijet2_abs_dEta",deta2);    


    int nbjets=0;
    int nbjetsCSV=0;
    int ncentjets=0;
    int nfwdjets=0;

    for(unsigned i=0;i<selectedJets.size() ;++i)
    {
        if (selectedJets[i]->IsBJet() and selectedJets[i]->Pt() >30 and abs(selectedJets[i]->Eta())<2.4)  nbjets +=1;
        if (selectedJets[i]->Btag()>0.8484 and selectedJets[i]->Pt() >30 and abs(selectedJets[i]->Eta())<2.4)  nbjetsCSV +=1;
        if (abs(selectedJets[i]->Eta())<2.4)
            { 
                ncentjets +=1; 
            }
        else nfwdjets +=1;
    }

    SetVariable("nJetsCent",ncentjets);    
    SetVariable("nJetsFwd",nfwdjets);    
    SetVariable("nBMed",nbjetsCSV);    
    SetVariable("MET",e->GetMet().Pt());    

    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
         bdt.push_back(readers_[i]->EvaluateMVA("BDTG_default") );
    }

    int icat=-100; string catStr="";

    float mu_max_eta = std::max(fabs(mu0->Eta()),fabs(mu1->Eta()));
    float mu_ave_eta = (fabs(mu0->Eta())+fabs(mu1->Eta()))/2.;

    //*
    if ( bdt[0] < -0.400 ) icat = 0 ;
    if ( bdt[0] >= 0.050 and bdt[0] < 0.250 and mu_max_eta >= 1.900 ) icat = 1 ;
    if ( bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta >= 1.900 ) icat = 2 ;
    if ( bdt[0] >= -0.400 and bdt[0] < 0.050 and mu_max_eta >= 1.900 ) icat = 3 ;
    if ( bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta >= 1.900 ) icat = 4 ;
    if ( bdt[0] >= 0.050 and bdt[0] < 0.250 and mu_max_eta < 0.900 ) icat = 5 ;
    if ( bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta < 0.900 ) icat = 6 ;
    if ( mu_max_eta < 1.900 and bdt[0] >= 0.250 and bdt[0] < 0.400 and mu_max_eta >= 0.900 ) icat = 7 ;
    if ( bdt[0] >= 0.050 and bdt[0] < 0.250 and mu_max_eta >= 0.900 and mu_max_eta < 1.9) icat = 8 ;
    if ( bdt[0] >= -0.400 and bdt[0] < 0.050 and mu_max_eta < 0.9 ) icat = 9 ;
    if ( bdt[0] >= -0.400 and bdt[0] < 0.050 and mu_max_eta < 1.900  and mu_max_eta >=0.9) icat = 10 ;
    if ( bdt[0] < 0.730 and bdt[0] >= 0.650 ) icat = 11 ;
    if ( mu_max_eta < 1.900 and bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta >= 0.900 ) icat = 12;
    if ( bdt[0] < 0.650 and bdt[0] >= 0.400 and mu_max_eta < 0.900 ) icat = 13 ;
    if ( bdt[0] >= 0.730 ) icat = 14 ;
    //

    if (icat>=0) catStr=Form("cat%d",icat);

    if (doSync and not processingSyst_)
    {
        SetTreeVar("bdt",bdt[0]);
        SetTreeVar("cat",icat);
    }

    // SciKit
    if (doScikit)
    {
       
        #ifdef SCIKIT_TIMING
        //static double time_bdtg=0;
        //static double time_sgd=0;
        //static double time_svr=0;
        //static double time_mlp=0;
        static double time_keras=0;
        TStopwatch sw;
        #endif
        

        scikit.clear();
        x.clear();

        x.push_back (Hmm.Pt());
        x.push_back (Hmm.Eta());
        x.push_back (Hmm.Phi());
        x.push_back( mu0->DeltaPhi(mu1));
        x.push_back( fabs(mu0->Eta()-mu1->Eta()));
        x.push_back( mu0->Eta());
        x.push_back( mu1->Eta());
        x.push_back( ncentjets);
        x.push_back( selectedJets.size());

        float aveQGLcent=0.0;
        float maxCSV=0.0;
        float leadDeepB = -100.;
        float maxDeepB = -100.;
        float aveCSV=0.0;
        float htCent=0.0;

            vector<float> csv_score;
            vector<pair<float,float>> qgl_score;

            if (selectedJets.size() >0)
            {
                leadDeepB = selectedJets[0]->GetDeepB();
            }

            for(unsigned i=0;i<selectedJets.size() ;++i)
            {
                if (abs(selectedJets[i]->Eta())<2.4)
                { 
                    aveQGLcent += selectedJets[i]->QGL(); 
                    csv_score.push_back(selectedJets[i]->Btag());
                    qgl_score.push_back(pair<float,float>(selectedJets[i]->Pt(),selectedJets[i]->QGL()));
                    maxCSV = std::max(selectedJets[i]->Btag(),maxCSV)  ;
                    aveCSV += selectedJets[i]->Btag() * selectedJets[i]->Pt() ;
                    htCent += selectedJets[i]->Pt();
                    maxDeepB = std::max(selectedJets[i]->GetDeepB(),maxDeepB)  ;
                }
                else nfwdjets +=1;
            }
            if (ncentjets >0 ) aveQGLcent /= ncentjets;
            if (htCent>0) aveCSV /=htCent;
            sort(csv_score.begin(),csv_score.end());
            sort(qgl_score.begin(),qgl_score.end(),[](const pair<float,float >&x,const pair<float, float >&y){ if (x.first> y.first) return true; if(x.first<y.first)return false; return x.second> y.second; } );

        x.push_back( htCent);
        x.push_back( passLeptonVeto);
        x.push_back( (mjj.size() >0 ) ? mjj[0].first : 0.0);
        x.push_back( (mjj.size() >1 ) ? mjj[1].first : 0.0);
        x.push_back( (mjj.size()>0) ? fabs(selectedJets[mjj[0].second.first]->Eta() - selectedJets[mjj[0].second.second]->Eta()): -1.0 ); 
        x.push_back( (mjj.size()>1) ? fabs(selectedJets[mjj[1].second.first]->Eta() - selectedJets[mjj[1].second.second]->Eta()): -1.0 );
        int soft1Njets=-1,soft5Njets=-1,soft10Njets=-1;
        float soft1Ht=-1,soft5Ht=-1,soft10Ht=-1;
        if (mjj.size() >0 )
        {
            Jet * j1= selectedJets[mjj[0].second.first];
            Jet * j2= selectedJets[mjj[0].second.second];
            pair<int,float> soft1 = e->softVariables(j1,j2,1.);
            pair<int,float> soft5 = e->softVariables(j1,j2,5.);
            pair<int,float> soft10 = e->softVariables(j1,j2,10.);
            soft1Njets= soft1.first;
            soft1Ht= soft1.second;
            soft5Njets= soft5.first;
            soft5Ht= soft5.second;
            soft10Njets= soft10.first;
            soft10Ht= soft10.second;
        }
        x.push_back( soft1Njets);
        x.push_back( soft1Ht);
        x.push_back( soft5Njets);
        x.push_back( soft5Ht);
        x.push_back( soft10Njets);
        x.push_back( soft10Ht);

        x.push_back( (qgl_score.size() >0) ? qgl_score[0].second : -1 );
        x.push_back( (qgl_score.size() >1) ? qgl_score[1].second : -1 );
        x.push_back( (qgl_score.size() >2) ? qgl_score[2].second : -1 );
        x.push_back( nbjetsCSV );
        x.push_back( maxDeepB );
        x.push_back( leadDeepB );
        x.push_back( maxCSV );

        x.push_back( ChargedHiggs::mt(mu0->Pt(),e->GetMet().Pt(),mu0->Phi(),e->GetMet().Phi()));
        x.push_back(ChargedHiggs::mt(mu1->Pt(),e->GetMet().Pt(),mu1->Phi(),e->GetMet().Phi()));
        x.push_back( e->GetMet().Pt() );



        #ifdef SCIKIT_TIMING
            sw.Reset();sw.Start();
        #endif

        //float bdtg= py->Eval("bdtg.predict([[x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9]]])[0]"); // THIS IS VERY SLOW
        //float bdtg=-999;
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_bdtg += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        #ifdef SCIKIT_TIMING
            sw.Stop(); time_mlp += sw.CpuTime(); sw.Reset(); sw.Start();
        #endif
    
        //py->Exec( "print 'X=',x,x[0],x[1]"); DEBUG
        //py->Exec( "print 'eval=',kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16] ] ] ))[0][0]");
        float keras= py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16],x[17], x[18], x[19],x[20],x[21],x[22],x[23],x[24],x[25],x[26],x[27], x[28] ] ] ))[0][0]");
        ////float keras=-999;
        //#ifdef SCIKIT_TIMING
        //    sw.Stop(); time_keras += sw.CpuTime(); sw.Reset(); sw.Start();
        //#endif

        //scikit.push_back(bdtg);
        //scikit.push_back(sgd);
        //scikit.push_back(svr);
        //scikit.push_back(mlp);
        scikit.push_back(keras);
        if (doSync) SetTreeVar("keras",keras);

        #ifdef SCIKIT_TIMING
           //Log(__FUNCTION__,"SCIKIT",Form("Scikit values are: bdtg=%f sgd=%f svr=%f mlp=%f  ",bdtg,svr,sgd,mlp) );
           //Log(__FUNCTION__,"TIME",Form("BDTG Time: %.3lf",time_bdtg) );
           //Log(__FUNCTION__,"TIME",Form("SGD Time: %.3lf",time_sgd) );
           //Log(__FUNCTION__,"TIME",Form("SVR Time: %.3lf",time_svr) );
           //Log(__FUNCTION__,"TIME",Form("MLP Time: %.3lf",time_mlp) );
           Log(__FUNCTION__,"TIME",Form("KERAS Time: %.3lf",time_keras) );
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
    //discr.push_back("MLP");
    discr.push_back("KERAS");

    py . reset(new TPython);
    //py -> Exec("from sklearn.ensemble import RandomForestClassifier");
    //py -> Exec("from sklearn.svm import SVC");
    //py -> Exec("from sklearn.svm import SVR");
    //py -> Exec("from sklearn.neural_network import MLPClassifier");
    //py -> Exec("from sklearn.neural_network import MLPRegressor");
    //py -> Exec("from sklearn.linear_model import SGDClassifier");
    //py -> Exec("from sklearn.linear_model import SGDRegressor");
    //py -> Exec("from sklearn.linear_model import PassiveAggressiveClassifier");
    //py -> Exec("from sklearn.linear_model import PassiveAggressiveRegressor");
    //py -> Exec("from sklearn.linear_model import Perceptron");
    //py -> Exec("from sklearn.linear_model import Ridge");
    //py -> Exec("from sklearn.linear_model import ElasticNet");

    // KERAS
    py -> Exec("import keras");
    py -> Exec("import numpy as np");
    //py -> Exec("from keras.models import Sequential");
    //py -> Exec("from keras.layers import Dense, Activation");
    //py -> Exec("from keras.wrappers.scikit_learn import KerasRegressor");
    //py -> Exec("from sklearn.preprocessing import StandardScaler");
    //py -> Exec("from sklearn.pipeline import Pipeline");

    py -> Exec("from sklearn.externals import joblib ");


    //py ->Exec("rfc=joblib.load('aux/hmm/RFC.pkl')");
    //py ->Exec("mlp=joblib.load('aux/hmm/MLP.pkl')");
    //py ->Exec("svr=joblib.load('aux/hmm/SVR.pkl')");
    //py ->Exec("par=joblib.load('aux/hmm/PAR.pkl')");
    //py ->Exec("sgd=joblib.load('aux/hmm/SGD.pkl')");
    //py ->Exec("bdtg=joblib.load('aux/hmm/BDTG.pkl')");
    py ->Exec("kmodel=keras.models.load_model('aux/hmm/keras.hd')");
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
        for (int i=0;i<30;++i)
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

	Log(__FUNCTION__,"INFO","Booking Histograms");
    for ( string l : AllLabel()  ) {
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);

	    // Control variables
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
	    Book ("HmumuAnalysis/Vars/MetOnZ_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnZ_rw_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_rw_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnH_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
    
        // bdt
	    Book ("HmumuAnalysis/Vars/BdtOnZ_"+ l ,"Bdt On Z (70-110);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/BdtOnH_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/BdtOnH_BB_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);

        // zpt reweight
	    Book ("HmumuAnalysis/Vars/BdtOnH_zptrwg_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/PtOnH_zptrwg_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
        
        // bdt vars
	    Book ("HmumuAnalysis/Vars/Mjj1OnH_"+ l ,"Mjj1 On Hmm (110-150);Bdt;Events", 200,0,2000);
	    Book ("HmumuAnalysis/Vars/PtJet1OnH_"+ l ,"PtJet1 On Hmm (110-150);Bdt;Events", 200,0,200);
	    Book ("HmumuAnalysis/Vars/PtJet2OnH_"+ l ,"PtJet2 On Hmm (110-150);Bdt;Events", 200,0,200);
	    Book ("HmumuAnalysis/Vars/EtaJet1OnH_"+ l ,"EtaJet1 On Hmm (110-150);Bdt;Events", 200,-5,5);
	    Book ("HmumuAnalysis/Vars/EtaJet2OnH_"+ l ,"EtaJet2 On Hmm (110-150);Bdt;Events", 200,-5,5);
	    Book ("HmumuAnalysis/Vars/NJetsOnH_"+ l ,"NJets On Hmm (110-150);Bdt;Events", 10,0,10);
	    Book ("HmumuAnalysis/Vars/NBJetsOnH_"+ l ,"NBJets On Hmm (110-150);Bdt;Events", 10,0,10);

        // btag, soft track activity
	    Book ("HmumuAnalysis/Vars/SoftNJetsOnH_"+ l ,"NJets On Hmm (110-150);Bdt;Events", 10,0,10);
	    Book ("HmumuAnalysis/Vars/SoftHtOnH_"+ l ,"NJets On Hmm (110-150);Bdt;Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/DeepBOnH_"+ l ,"NJets On Hmm (110-150);Bdt;Events", 100,0,1);

        // exclusive categories related variables
	    Book ("HmumuAnalysis/Vars/MtOnH_WLep_"+ l ,"Mt;mt [GeV];Events", 1000,0,1000); 
	    Book ("HmumuAnalysis/Vars/DeltaR_ggHX_"+ l ,"dR ggHX;dR;Events", 1000,0,10); 
	    Book ("HmumuAnalysis/Vars/NJets_ggHX_"+ l ,"nJets ggHX;nJets;Events", 10,0,10); 

        // Study cut for tag categories ttH, WH,ZH, 

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
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 2000,60,160); // every 4 (old16) per GeV
	        //Book2D ("HmumuAnalysis/Vars/JetEtaPhi_"+ c + "_"+ l ,"Jet EtaPhi; eta; phi;Events", 100,-5,5,100,-3.1415,3.1415); // CHECK2D
        }

    } //end label loop

    if (doSync){
        InitTree("hmm");
        Branch("hmm","cat",'I');
        Branch("hmm","weight",'D');
        Branch("hmm","mcWeight",'D');
        Branch("hmm","puWeight",'D');
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
        Branch("hmm","metphi",'F');
        Branch("hmm","mt1",'F');
        Branch("hmm","mt2",'F');
        Branch("hmm","Hpt",'F');
        Branch("hmm","njets",'I');
        Branch("hmm","nbjets",'I'); // default btagging
        Branch("hmm","ncentjets",'I');
        Branch("hmm","nfwdjets",'I');
        Branch("hmm","jet_pt",'d',20,"njets");
        Branch("hmm","jet_eta",'d',20,"njets");
        Branch("hmm","jet_phi",'d',20,"njets");

        // Deep B-tagging. HardestJet. Higher btagged jet.
        Branch("hmm","leadDeepB",'F'); 
        Branch("hmm","maxDeepB",'F'); 
        // Soft ACtivity
        Branch("hmm","softNjets1",'I'); 
        Branch("hmm","softHt1",'F'); 
        Branch("hmm","softNjets2",'I'); 
        Branch("hmm","softHt2",'F'); 
        Branch("hmm","softNjets5",'I'); 
        Branch("hmm","softHt5",'F'); 
        Branch("hmm","softNjets10",'I'); 
        Branch("hmm","softHt10",'F'); 
        
        // QGL
        Branch("hmm","aveQGLCent",'F');    
        Branch("hmm","firstQGL",'F');     // pt sorted
        Branch("hmm","secondQGL",'F');    
        Branch("hmm","thirdQGL",'F');    
        Branch("hmm","maxCSV",'F');     // csv sorted
        Branch("hmm","aveCSV",'F');    
        Branch("hmm","htCent",'F');    
        Branch("hmm","secondCSV",'F');    
        Branch("hmm","thirdCSV",'F');    

        Branch("hmm","pass_recomuons",'I');
        Branch("hmm","miniIsoLeptons",'I');
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
        Branch("hmm","htmjj_1",'F');
        Branch("hmm","htmjj_2",'F');
        Branch("hmm","deltaphi",'F');
        Branch("hmm","deltaeta",'F');

        Branch("hmm","bdt",'F');
        if (doScikit) Branch("hmm","keras",'F');

        Branch("hmm","mjj_1_JESUp",'F');
        Branch("hmm","mjj_2_JESUp",'F');

        Branch("hmm","mjj_1_JESDown",'F');
        Branch("hmm","mjj_2_JESDown",'F');
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

int HmumuAnalysis::analyze(Event *e, string systname)
{
    if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("Analyze event %ld:%ld:%ld",e->runNum(),e->lumiNum(),e->eventNum()));

    if (systname == "" or systname =="NONE") processingSyst_=false;
    else processingSyst_=true;

    //** reset 
    mt=0.0;// WHLep only for plotting
    TLorentzVector zero(0,0,0,0);
    Hmm.SetP4(zero); // make sure it is 0.
    selectedJets.clear();
    mjj.clear();
    //

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

    /*
     * TOP PT REWEIGHT
     */

    e->ApplyTopReweight();

    /*
     * HIGGS REWEIGHT -- UNCERTAINTIES
     */

    if (not e->IsRealData()){

        if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start HTXS Classification");

        HTXS::HiggsClassification  hc;
        if (label.find( "GluGlu_HToMuMu") != string::npos) hc.prodMode = HTXS::GGF;
        else if (label.find( "VBF_HToMuMu") != string::npos) hc.prodMode = HTXS::VBF;
        else if (label.find( "ZH_HToMuMu") != string::npos) {
            if (e->GetPdfId(1) <=6 and e->GetPdfId(2) <=6)
            {
                hc.prodMode = HTXS::QQ2ZH; // I should look into the PDF productions GG2ZH vs QQ2ZH
            }
            else
            {
                hc.prodMode = HTXS::GG2ZH; // I should look into the PDF productions GG2ZH vs QQ2ZH
            }
        }
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
            
            e->ApplySF("wg1");
        }
    }
    /*****************/


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon0: ");
    mu0 = e->GetMuon(0);
    if (VERBOSE and mu0)Log(__FUNCTION__,"DEBUG",Form("GetMuon0: pt=%f",mu0->Pt()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetMuon1: ");
    mu1 = e->GetMuon(1);
    int il2=1;
    while (mu0 != NULL and mu1!= NULL and mu1->Charge() * mu0->Charge() != -1) { mu1= e->GetMuon(++il2);} // fully combinatorics on the second muon
    if (VERBOSE and mu1)Log(__FUNCTION__,"DEBUG",Form("GetMuon1: pt=%f",mu1->Pt()));

    bool recoMuons= mu0 != NULL and mu1 !=NULL; 
    if (recoMuons and mu0->Charge() * mu1->Charge() != -1 ) recoMuons=false; // 


    if (recoMuons)
    {
        /* default definition. Hmm can be redefined by exclusive categories
         */
        TLorentzVector zero(0,0,0,0);
        Hmm.SetP4(zero); // make sure it is 0.
        Hmm += *mu0;
        Hmm += *mu1;
        mass_=Hmm.M();
        pt_ = Hmm.Pt();
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetJets: ");

    selectedJets.clear();
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        selectedJets.push_back(e->GetJet(i));
    }
    updateMjj();

    // SYNCH DEBUG run: 299149 LS:76 event: 67754842
    if ( (e->eventNum() == 67754842 and e->runNum() == 299149 and e->lumiNum() == 76)  or
         (e->eventNum() == 479703485 and e->runNum() == 297722 and e->lumiNum() == 314)
        ){
        Log(__FUNCTION__,"SYNC-DEBUG",Form("Selected Jets: %ld:%ld:%lu",e->runNum(),e->lumiNum(),e->eventNum()));
        for (auto & j : selectedJets)
         Log(__FUNCTION__,"SYNC-DEBUG",Form("jet (pt,eta,phi): %f %f %f",j->Pt(),j->Eta(),j->Phi()));
        Log(__FUNCTION__,"SYNC-DEBUG",Form("All jets"));
        for(int i=0;;++i)
        {
            Jet *j = e->GetBareJet(i);
            if (j==NULL) break;
            Log(__FUNCTION__,"SYNC-DEBUG",Form("Considering jet (%f,%f,%f)",j->Pt(),j->Eta(),j->Phi()));
            Log(__FUNCTION__,"SYNC-DEBUG",Form("    * IsJet %d",j->IsJet()));
            Log(__FUNCTION__,"SYNC-DEBUG",Form("    * IsJetEV %d",j->IsJetExceptValidity()));
            Log(__FUNCTION__,"SYNC-DEBUG",Form("    * PuId= %f",j->GetPuId()));
        }



    }
    ///

    // ------------------- DIRTY CATEGORIES for studies
    int nbloose=0;
    for(int i=0;i < e->Njets() ;++i){
        Jet *j= e->GetJet(i);
        if ( not j->IsJet() ) continue;
        if (fabs(j->Eta() ) >2.4) continue;
        if (j->Btag() > 0.5426 ) continue;
        nbloose += 1;
    }


    // ------------------------------------
    // OFFICIAL CATEGORIZATION
    string category;

    if (catType==2) category = CategoryBdt(e);
    else if (catType==3) { 
        category = CategoryBdt(e); 
        string categoryExc = CategoryExclusive(e); 
        recoMuons = mu0 != NULL and mu1 !=NULL;  // muons may have been recomputed here

        if (categoryExc == "ttHHadr") category = "cat15";// 
        if (categoryExc == "ttHLep" ) category="cat16";  // 
        if (categoryExc == "ZHLep" ) category="cat17";   // 
        if (categoryExc == "WHLep" ) category="cat18";   // 
        if (categoryExc == "ggHX" and category == "" ) category="cat19";   // 
    } 
    else category = Category(mu0, mu1, selectedJets);

    // --------------- DIRTY CATEGORIES -----------
    //string categoryDirty=""; // this may double count

    // ------------------------
    // BTAG SF
    // -----------------------
  
#warning NO_BTAGSF 
    /* 
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
    */

    //  ------------------- APPLY SF --------------
    //
#warning NO_MU_SF

    /*
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

    double sfIso=1.0; // TODO, apply a different sf for miniIsolation, use flag isMiniIsoLeptons
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
    */
    
    // ***************************
    // **       preselection    **
    // ***************************
    

    // Trigger
    bool passAsymmPtCuts = (recoMuons and  mu0->Pt() >30 and mu1->Pt() >20 );
    bool passTrigger=e->IsTriggered("HLT_IsoMu27_v") or e->IsTriggered("HLT_IsoTkMu27_v"); 

    bool passTrigger1{false}, passTrigger2{false};

    if (  recoMuons and ( (label.find("HToMuMu") != string::npos or e->IsRealData()) ) ) // 
    {
        //cout <<" DOING TRIGGER MATCHING "<<endl;
        bool passTriggerEvent = passTrigger;
        passTrigger1 = (e->IsTriggered("HLT_IsoMu27_v",mu0) or e->IsTriggered("HLT_IsoTkMu27_v",mu0)) ;
        //if (mu1->Pt() > 30 ) 
        passTrigger2 = (e->IsTriggered("HLT_IsoMu27_v",mu1) or e->IsTriggered("HLT_IsoTkMu27_v",mu1)) ;
        passTrigger=passTrigger1 or passTrigger2;

        if (passTriggerEvent and not passTrigger) Log(__FUNCTION__,"INFO","Fail to trigger event due to trigger matching");
        if (not passTriggerEvent and passTrigger) Log(__FUNCTION__,"ERROR","Event triggered by object but not globally");
    }

    passLeptonVeto= true;
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

    if (doSync and not processingSyst_) {
        // global info
        SetTreeVar("eventNum",e->eventNum());
        SetTreeVar("runNum",e->runNum());
        SetTreeVar("lumiNum",e->lumiNum());

        if (e->IsRealData()){
            SetTreeVar("mc",0);
            SetTreeVar("mcWeight",1.);
            SetTreeVar("puWeight",1.);
            SetTreeVar("weight",1.);
        }

        else {
            SetTreeVar("weight",e->weight());
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
        SetTreeVar("miniIsoLeptons",isMiniIsoLeptons);
        SetTreeVar("pass_asymmcuts",passAsymmPtCuts);
        SetTreeVar("pass_trigger",passTrigger);
        SetTreeVar("pass_trigger1",passTrigger1);
        SetTreeVar("pass_trigger2",passTrigger2);
        SetTreeVar("pass_leptonveto",passLeptonVeto);
        if ( recoMuons and passTrigger and passAsymmPtCuts and category != "" ) SetTreeVar("pass_all",1);
        else SetTreeVar("pass_all",0); 
        //
        if (recoMuons)
        {
            SetTreeVar("pt1",mu0->Pt());
            SetTreeVar("pt2",mu1->Pt());
            SetTreeVar("eta1",mu0->Eta());
            SetTreeVar("eta2",mu1->Eta());
            SetTreeVar("phi1",mu0->Phi());
            SetTreeVar("phi2",mu1->Phi());
            SetTreeVar("mass",Hmm.M());
            SetTreeVar("Hpt" ,Hmm.Pt());
            SetTreeVar("Heta" ,Hmm.Eta());
            SetTreeVar("Hphi" ,Hmm.Phi());
            SetTreeVar("deltaeta",fabs(mu0->Eta()-mu1->Eta()));    
            SetTreeVar("deltaphi",fabs(mu0->DeltaPhi(*mu1)));
            SetTreeVar("mt1" ,ChargedHiggs::mt(mu0->Pt(),e->GetMet().Pt(),mu0->Phi(),e->GetMet().Phi()));
            SetTreeVar("mt2" ,ChargedHiggs::mt(mu1->Pt(),e->GetMet().Pt(),mu1->Phi(),e->GetMet().Phi()));
        }
        else
        {
            SetTreeVar("pt1",-10);
            SetTreeVar("pt2",-10);
            SetTreeVar("eta1",-10);
            SetTreeVar("eta2",-10);
            SetTreeVar("phi1",-10);
            SetTreeVar("phi2",-10);
            SetTreeVar("mass",-10);
            SetTreeVar("Hpt" ,-10);
            SetTreeVar("Heta",-10);
            SetTreeVar("Hphi",-10);
            SetTreeVar("deltaeta",-10);    
            SetTreeVar("deltaphi",-10);
            SetTreeVar("mt1", -10);
            SetTreeVar("mt2", -10);
        } 
        SetTreeVar("njets",selectedJets.size());
        SetTreeVar("met" ,e->GetMet().Pt());
        SetTreeVar("metphi" ,e->GetMet().Phi());

        // basic jet info
        for(int ijet=0;ijet<selectedJets.size();++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,selectedJets[ijet]->Pt());
            SetTreeVar("jet_eta",ijet,selectedJets[ijet]->Eta());
            SetTreeVar("jet_phi",ijet,selectedJets[ijet]->Phi());
        }
        for(int ijet=selectedJets.size();ijet<20 ;++ijet)
        {
            SetTreeVar("jet_pt" ,ijet,0.0);
            SetTreeVar("jet_eta",ijet,0.0);
            SetTreeVar("jet_phi",ijet,0.0);
        }

        // compute secondary jets variables
        int nbjets=0;
        int ncentjets=0;
        int nfwdjets=0;
        float aveQGLcent=0.0;
        float maxCSV=0.0;
        float leadDeepB = -100.;
        float maxDeepB = -100.;
        float aveCSV=0.0;
        float htCent=0.0;

        vector<float> csv_score;
        vector<pair<float,float>> qgl_score;

        if (selectedJets.size() >0)
        {
            leadDeepB = selectedJets[0]->GetDeepB();
        }

        for(unsigned i=0;i<selectedJets.size() ;++i)
        {
            if (selectedJets[i]->IsBJet() and selectedJets[i]->Pt() >30 and abs(selectedJets[i]->Eta())<2.4)  nbjets +=1;
            if (abs(selectedJets[i]->Eta())<2.4)
            { 
                ncentjets +=1; 
                aveQGLcent += selectedJets[i]->QGL(); 
                csv_score.push_back(selectedJets[i]->Btag());
                qgl_score.push_back(pair<float,float>(selectedJets[i]->Pt(),selectedJets[i]->QGL()));
                maxCSV = std::max(selectedJets[i]->Btag(),maxCSV)  ;
                aveCSV += selectedJets[i]->Btag() * selectedJets[i]->Pt() ;
                htCent += selectedJets[i]->Pt();
                maxDeepB = std::max(selectedJets[i]->GetDeepB(),maxDeepB)  ;
            }
            else nfwdjets +=1;
        }
        if (ncentjets >0 ) aveQGLcent /= ncentjets;
        if (htCent>0) aveCSV /=htCent;
        sort(csv_score.begin(),csv_score.end());
        sort(qgl_score.begin(),qgl_score.end(),[](const pair<float,float >&x,const pair<float, float >&y){ if (x.first> y.first) return true; if(x.first<y.first)return false; return x.second> y.second; } );


        SetTreeVar("aveQGLCent",aveQGLcent);
        SetTreeVar("aveCSV",aveCSV);
        SetTreeVar("maxCSV",maxCSV);
        SetTreeVar("secondCSV", ( csv_score.size() >1) ? csv_score[1] : -1.);
        SetTreeVar("thirdCSV" , ( csv_score.size() >2) ? csv_score[2] : -1.);

        SetTreeVar("firstQGL",  (qgl_score.size() >0) ? qgl_score[0].second : -1 );
        SetTreeVar("secondQGL", (qgl_score.size() >1) ? qgl_score[1].second : -1 );
        SetTreeVar("thirdQGL",  (qgl_score.size() >2) ? qgl_score[2].second : -1 );
        
        SetTreeVar("htCent",htCent);
        SetTreeVar("nbjets",nbjets);
        SetTreeVar("ncentjets",ncentjets);
        SetTreeVar("nfwdjets",nfwdjets);

        SetTreeVar("leadDeepB",leadDeepB ); 
        SetTreeVar("maxDeepB",maxDeepB ); 

        if (mjj.size() >0)
        {
            Jet * j1= selectedJets[mjj[0].second.first];
            Jet * j2= selectedJets[mjj[0].second.second];
            pair<int,float> soft1 = e->softVariables(j1,j2,1.);
            pair<int,float> soft2 = e->softVariables(j1,j2,2.);
            pair<int,float> soft5 = e->softVariables(j1,j2,5.);
            pair<int,float> soft10 = e->softVariables(j1,j2,10.);
            SetTreeVar("softNjets1",soft1.first);
            SetTreeVar("softHt1",soft1.second);
            SetTreeVar("softNjets2",soft2.first);
            SetTreeVar("softHt2",soft2.second);
            SetTreeVar("softNjets5",soft5.first);
            SetTreeVar("softHt5",soft5.second);
            SetTreeVar("softNjets10",soft10.first);
            SetTreeVar("softHt10",soft10.second);
        }
        else{
            SetTreeVar("softNjets1",-1);
            SetTreeVar("softHt1",-1);
            SetTreeVar("softNjets2",-1);
            SetTreeVar("softHt2",-1);
            SetTreeVar("softNjets5",-1);
            SetTreeVar("softHt5",-1);
            SetTreeVar("softNjets10",-1);
            SetTreeVar("softHt10",-1);
        }

        SetTreeVar("mjj_1",(mjj.size() >0 ) ? mjj[0].first: 0.0);
        SetTreeVar("mjj_2",(mjj.size() > 1) ? mjj[1].first: 0.0);
        SetTreeVar("detajj_1",(mjj.size()>0) ? fabs(selectedJets[mjj[0].second.first]->Eta() - selectedJets[mjj[0].second.second]->Eta()): -1.0);
        SetTreeVar("detajj_2",(mjj.size()>1) ? fabs(selectedJets[mjj[1].second.first]->Eta() - selectedJets[mjj[1].second.second]->Eta()): -1.0);

        vector<float> ht_mjj;
        for(unsigned imjj =0; imjj<mjj.size() ;++imjj)
        {
            float epsilon=1e-4;
            float ht=0.0;
            float eta0 = std::min( selectedJets[ mjj[imjj].second.first] -> Eta (), selectedJets[ mjj[imjj].second.second] -> Eta () ) + epsilon;
            float eta1 = std::max( selectedJets[ mjj[imjj].second.first] -> Eta (), selectedJets[ mjj[imjj].second.second] -> Eta () ) - epsilon;
            for(unsigned i=0;i<selectedJets.size() ;++i)
            {
                if (eta0 < selectedJets[i] -> Eta() and selectedJets[i] -> Eta() < eta1)
                {
                    ht += selectedJets[i] -> Pt() ;
                }
            }
            ht_mjj . push_back(ht);
        }
        SetTreeVar("htmjj_1", (mjj.size()>0) ? ht_mjj[0]: 0.0 ) ;
        SetTreeVar("htmjj_2", (mjj.size()>1) ? ht_mjj[1]: 0.0 ) ;

    } // end doSync

    if (doSync and (systname == "JESUp" or systname=="JESDown") ) 
    {
        SetTreeVar("mjj_1_"+systname,(mjj.size() > 0 ) ? mjj[0].first: 0.0);
        SetTreeVar("mjj_2_"+systname,(mjj.size() > 1 ) ? mjj[1].first: 0.0);
    }

    // -- FINAL SELECTION --
    //if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto)
    if ( recoMuons and passTrigger and passAsymmPtCuts)
    {
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
            Fill("HmumuAnalysis/Vars/PtOnH_zptrwg_"+ label,systname, pt_,e->weight() *zptrw);
            float mjj1=(mjj.size() >0 ) ? mjj[0].first:-1 ; 
            Fill("HmumuAnalysis/Vars/Mjj1OnH_" + label,systname, mjj1,e->weight() ) ;

            if (mjj.size()> 0)
            {
                Jet * j1= selectedJets[mjj[0].second.first];
                Jet * j2= selectedJets[mjj[0].second.second];
                pair<int,float> soft1 = e->softVariables(j1,j2,1.);
                int softNj=soft1.first;
                float softHt=soft1.second;
                Fill("HmumuAnalysis/Vars/SoftNJetsOnH_" + label,systname, softNj,e->weight() ) ;
                Fill("HmumuAnalysis/Vars/SoftHtOnH_" + label,systname, softHt,e->weight() ) ;
            }

            if (selectedJets.size() >0)
            {
                Fill("HmumuAnalysis/Vars/DeepBOnH_"+ label,systname, selectedJets[0]->GetDeepB(),e->weight());
                Fill("HmumuAnalysis/Vars/PtJet1OnH_"+ label,systname, selectedJets[0]->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/EtaJet1OnH_"+ label,systname, selectedJets[0]->Eta(),e->weight());
            }

            if (selectedJets.size() >1)
            {
                Fill("HmumuAnalysis/Vars/PtJet2OnH_"+ label,systname, selectedJets[1]->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/EtaJet2OnH_"+ label,systname, selectedJets[1]->Eta(),e->weight());
            }

            Fill("HmumuAnalysis/Vars/NJetsOnH_"+ label,systname, selectedJets.size(),e->weight());
            Fill("HmumuAnalysis/Vars/NBJetsOnH_"+ label,systname, e->Bjets(),e->weight());
            if(catType>=2 )Fill("HmumuAnalysis/Vars/BdtOnH_"+ label,systname, bdt[0] ,e->weight());
            if(catType>=2 )Fill("HmumuAnalysis/Vars/BdtOnH_zptrwg_"+ label,systname, bdt[0] ,e->weight()*zptrw);
            if(catType>=2 and fabs(mu0->Eta())<0.8 and fabs(mu1->Eta())<0.8)
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

            if (catType == 3 and category == "cat18") // WHLep
            {
                Fill("HmumuAnalysis/Vars/MtOnH_WLep_"+ label,systname, mt,e->weight()) ;
            }

            if (catType == 3 and category == "cat19") // WHLep
            {
                Fill("HmumuAnalysis/Vars/NJets_ggHX_"+ label,systname, selectedJets.size(),e->weight()) ;
                Fill("HmumuAnalysis/Vars/DeltaR_ggHX_"+ label,systname,mu0->DeltaR(mu1),e->weight()) ;
            }
        }


        if(Unblind(e))Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, mass_,e->weight()) ;
        if(Unblind(e) and category != "")Fill("HmumuAnalysis/Vars/Mmm_"+ category+"_"+ label,systname, mass_,e->weight()) ;

        //if(Unblind(e) and (categoryDirty=="ttHHadr" or categoryDirty == "superPure") )Fill("HmumuAnalysis/Vars/Mmm_"+ categoryDirty+"_"+ label,systname, mass_,e->weight()) ;
        //if(Unblind(e) and categoryDirty == "ttHLep")Fill("HmumuAnalysis/Vars/Mmm_"+categoryDirty+"_"+ label,systname, mass_,e->weight()) ;
    }


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}

void HmumuAnalysis::EndEvent(){
    if (doSync)
    {
        FillTree("hmm");
    }
}

void HmumuAnalysis::updateMjj(){
    mjj.clear();
    for(unsigned i=0;i<selectedJets.size() ;++i)
        for (unsigned j=i+1;j<selectedJets.size();++j)
        {
            mjj.push_back( pair<float,pair<int,int> >(selectedJets[i]->InvMass( selectedJets[j] ), pair<int,int>(i,j) ) );
        }
    sort ( mjj.begin(),mjj.end(), [](  const pair<float,pair<int,int> >&x,const pair<float, pair<int,int> >&y){if (x.first > y.first) return true;if (x.first<y.first) return false; if  (x.second.first < y.second.first) return true; if (x.second.first > y.second.first) return false; return x.second.second < y.second.second; } );

}

float HmumuAnalysis::getZPtReweight(float Zpt)
{
    if (rzpt_.get() == NULL) {
        Double_t xAxis1[97] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 160, 170, 180, 190, 200, 250, 300, 400, 500, 600, 8000}; 
       rzpt_.reset( new TH1D("ratio__1","Pt On Hmm (110-150)",96, xAxis1));
       rzpt_->SetBinContent(1,1.25442);
       rzpt_->SetBinContent(2,1.12704);
       rzpt_->SetBinContent(3,1.12533);
       rzpt_->SetBinContent(4,1.05604);
       rzpt_->SetBinContent(5,1.10264);
       rzpt_->SetBinContent(6,1.08767);
       rzpt_->SetBinContent(7,1.10362);
       rzpt_->SetBinContent(8,1.08591);
       rzpt_->SetBinContent(9,1.06601);
       rzpt_->SetBinContent(10,1.0916);
       rzpt_->SetBinContent(11,1.06448);
       rzpt_->SetBinContent(12,1.01392);
       rzpt_->SetBinContent(13,1.04377);
       rzpt_->SetBinContent(14,1.02068);
       rzpt_->SetBinContent(15,0.994305);
       rzpt_->SetBinContent(16,1.00707);
       rzpt_->SetBinContent(17,0.976421);
       rzpt_->SetBinContent(18,0.975673);
       rzpt_->SetBinContent(19,0.979493);
       rzpt_->SetBinContent(20,0.986701);
       rzpt_->SetBinContent(21,0.921996);
       rzpt_->SetBinContent(22,0.958257);
       rzpt_->SetBinContent(23,0.947919);
       rzpt_->SetBinContent(24,0.915694);
       rzpt_->SetBinContent(25,0.963098);
       rzpt_->SetBinContent(26,0.902018);
       rzpt_->SetBinContent(27,0.921341);
       rzpt_->SetBinContent(28,0.947538);
       rzpt_->SetBinContent(29,0.919088);
       rzpt_->SetBinContent(30,0.89809);
       rzpt_->SetBinContent(31,0.93795);
       rzpt_->SetBinContent(32,0.951556);
       rzpt_->SetBinContent(33,0.920607);
       rzpt_->SetBinContent(34,0.931718);
       rzpt_->SetBinContent(35,0.972628);
       rzpt_->SetBinContent(36,0.991912);
       rzpt_->SetBinContent(37,0.982581);
       rzpt_->SetBinContent(38,0.986466);
       rzpt_->SetBinContent(39,0.948026);
       rzpt_->SetBinContent(40,0.881102);
       rzpt_->SetBinContent(41,0.975588);
       rzpt_->SetBinContent(42,0.967278);
       rzpt_->SetBinContent(43,0.96418);
       rzpt_->SetBinContent(44,0.8935);
       rzpt_->SetBinContent(45,0.984849);
       rzpt_->SetBinContent(46,0.934862);
       rzpt_->SetBinContent(47,1.08122);
       rzpt_->SetBinContent(48,0.90832);
       rzpt_->SetBinContent(49,0.996306);
       rzpt_->SetBinContent(50,1.04286);
       rzpt_->SetBinContent(51,1.01652);
       rzpt_->SetBinContent(52,0.99927);
       rzpt_->SetBinContent(53,0.987264);
       rzpt_->SetBinContent(54,1.00254);
       rzpt_->SetBinContent(55,0.98932);
       rzpt_->SetBinContent(56,1.04193);
       rzpt_->SetBinContent(57,1.02381);
       rzpt_->SetBinContent(58,1.01032);
       rzpt_->SetBinContent(59,1.03059);
       rzpt_->SetBinContent(60,1.01214);
       rzpt_->SetBinContent(61,1.07218);
       rzpt_->SetBinContent(62,0.999929);
       rzpt_->SetBinContent(63,1.06193);
       rzpt_->SetBinContent(64,0.949346);
       rzpt_->SetBinContent(65,0.920078);
       rzpt_->SetBinContent(66,0.973534);
       rzpt_->SetBinContent(67,1.00212);
       rzpt_->SetBinContent(68,0.953533);
       rzpt_->SetBinContent(69,1.01944);
       rzpt_->SetBinContent(70,1.03041);
       rzpt_->SetBinContent(71,0.999917);
       rzpt_->SetBinContent(72,1.07828);
       rzpt_->SetBinContent(73,1.06935);
       rzpt_->SetBinContent(74,0.919939);
       rzpt_->SetBinContent(75,1.06806);
       rzpt_->SetBinContent(76,1.05322);
       rzpt_->SetBinContent(77,0.98814);
       rzpt_->SetBinContent(78,0.982285);
       rzpt_->SetBinContent(79,1.03646);
       rzpt_->SetBinContent(80,1.25187);
       rzpt_->SetBinContent(81,1.02401);
       rzpt_->SetBinContent(82,1.0413);
       rzpt_->SetBinContent(83,1.06257);
       rzpt_->SetBinContent(84,0.982519);
       rzpt_->SetBinContent(85,1.09106);
       rzpt_->SetBinContent(86,1.01377);
       rzpt_->SetBinContent(87,1.03474);
       rzpt_->SetBinContent(88,0.990248);
       rzpt_->SetBinContent(89,1.00668);
       rzpt_->SetBinContent(90,0.975461);
       rzpt_->SetBinContent(91,1.03657);
       rzpt_->SetBinContent(92,0.992107);
       rzpt_->SetBinContent(93,0.861815);
       rzpt_->SetBinContent(94,0.930741);
       rzpt_->SetBinContent(95,0.857949);
       rzpt_->SetBinContent(96,1.0433);
       rzpt_->SetBinContent(97,2.25209);
        //rzpt2_.reset( new TF1("myf","1.36959e+00-1.39690e-03*x +x*x*7.11835e-07") );
    }
    if (Zpt < 1000) return rzpt_->GetBinContent(rzpt_->FindBin(Zpt));
    else return 1.0;
    //if (Zpt < 350) return rzpt_->GetBinContent(rzpt_->FindBin(Zpt));
    //else return rzpt2_->Eval (std::min(Zpt,float(1000.))) ;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
