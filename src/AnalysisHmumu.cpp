#include "interface/AnalysisHmumu.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TPython.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"

#include "interface/HiggsTemplateCrossSections.hpp"
#include "interface/HiggsTemplateCrossSections_Stage1p1.hpp"

#include "TVectorD.h"
#include "TMatrixD.h"

#define VERBOSE 0

//#define SCIKIT_TIMING
#ifdef SCIKIT_TIMING
    #warning SCIKIT_TIMING dont use for submission
#endif

//#warning Hmumu ANALYSIS NON ISO
//

//#define DEEP_B_LOOSE 0.1522
//#define DEEP_B_MEDIUM 0.4941
//#define DEEP_B_TIGHT 0.8001

#define DEEP_B_LOOSE ((year==2016)?0.2217:(year==2017)?0.1522:0.1241)
#define DEEP_B_MEDIUM ((year==2016)?0.6321:(year==2017)?0.4941:0.4148)
#define DEEP_B_TIGHT ((year==2016)?0.8953:(year==2017)?.8001:.7527)


#define SYNC_VERBOSE 1

template<>
float HmumuAnalysis::GetVariable( string name){ return *(float*)varValues_.GetPointer(name); } ;

template<class T>
T HmumuAnalysis::GetVariable( string name){ return *(T*)varValues_.GetPointer(name); } ;

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
    //l->SetTrackerMuonCut(true);
    //l->SetGlobalMuonCut(true);
}

void HmumuAnalysis::SetJetCuts(Jet *j) { 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
#warning 25GEV_JETS
    j->SetPtCut(25); 
    //#warning NOPUID
    //j->SetPuIdCut(-999);
    //#warning PUID
    j->SetPuIdCut(100);
    // Noise hard cut on jets
    if (year==2017) j->SetEENoiseCut(true);
    else j->SetEENoiseCut(false);
    // medium CSV
    j->SetBCut(-100); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetDeepBCut(DEEP_B_MEDIUM); 
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

void HmumuAnalysis::SetPhotonCuts(Photon *p){ // Already selecting Medium ided photons
    // I'm removing jets that look really likes photons.
    p->SetIsoCut(-1); 
    p->SetIsoRelCut(0.15); 
    p->SetPtCut(30);
    p->SetEtaCut(1.44);
}

string HmumuAnalysis::CategoryExclusive(Event *e)
{
    TLorentzVector zero(0,0,0,0);
    /* The exclusive categories may use a different muon selection:
     * they can reset the H definition
     */
    string category = "";
    isMiniIsoLeptons=false;

    miniIsoLeptons.clear();
    selectedJetsMiniIso.clear();
    selectedFatjets.clear();
    selectedJetsVHHadr.clear();
    alphaVHHadr.clear();
    valueVHHadr=-1;

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
    //vector<Jet*> selectedJetsMiniIso;
    //selectedJetsMiniIso.clear();
    int nbjets=0;
    int nbloose=0;
    int nblooseDeep=0; //L = 0.1522, M=0.4941, T=??
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
        selectedJetsMiniIso.push_back(j);
        
        //count bjets
        if (abs(j->Eta())>2.4 ) continue;
        //if (j->Btag() > 0.5426) nbloose+=1;
        //if (j->Btag() > 0.8484) nbjets+=1;
        if (j->GetDeepB() > DEEP_B_LOOSE) nbloose+=1;
        if (j->GetDeepB() > DEEP_B_MEDIUM) nbjets+=1;
    }
    std::sort(selectedJetsMiniIso.begin(),selectedJetsMiniIso.end(),[](Jet const *a, Jet const *b ){return a->Pt() > b->Pt();});

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
    if (category == "" and (nbloose >1 or nbjets >0) and miniIsoLeptons.size() >2) category = "ttHLep";
    if (category == "" and (nbloose >1 or nbjets >0) and selectedJetsMiniIso.size() > 4) category = "ttHHadr";

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
    /*
    if (category == "") // mu0_local and mu1_local arenot null!
    {
        bool vhhadr=false;
        // find fatjets
        for(int ifat=0; ;++ifat)
        {
            FatJet* f =  e->GetBareFatJet(ifat);
            if (f==NULL) break;
            // loose
            if (f->Pt() < 100) continue;
            if (fabs(f->Eta())>2.4) continue;
            //if ( f->SDTau2()/f->SDTau1() > 0.8) continue;
            if ( f->Tau2()/f->Tau1() > 0.6) continue;
            if ( f->SDMass() < 65 or f->SDMass() >150) continue; 
            if (mu0_local->DeltaR(f) <0.8) continue;
            if (mu1_local->DeltaR(f) <0.8) continue;
           
            // nominal 
            // if (f->Pt() < 200) continue; 
            // if (fabs(f->Eta())>2.4) continue;
            // if ( f->SDTau2()/f->SDTau1() >0.6) continue;
            // if ( f->SDMass() < 65 or f->SDMass() >105) continue; 
            // if (mu0_local->DeltaR(f) <0.8) continue;
            // if (mu1_local->DeltaR(f) <0.8) continue;
            //
            // top t32 and mass 105:220
            vhhadr=true;
            selectedFatjets.push_back(f);
        }
        // sort
        if (vhhadr)
        {
            std::sort(selectedFatjets.begin(),selectedFatjets.end(),[](FatJet const *a, FatJet const *b ){return a->Pt() > b->Pt();});
            category="VHHadr";
        }

    }
    */

    //if (category == "" and mu0_local != NULL and mu1_local!=NULL) { // VH Hadr resolved
    // use normal muons. Non miniIso
    //if (category == "" and mu0_local != NULL and mu1_local!=NULL) { // VH Hadr resolved
    if (category == "" and mu0 != NULL and mu1!=NULL) { // VH Hadr resolved
        kf->clear();
        //if (selectedJetsMiniIso.size() >=2){
        if (selectedJets.size() >=2){
            //for (auto j : selectedJetsMiniIso) 
            //{
            //    JME::JetParameters bins,vars;
            //    vars.setJetPt( j->GetP4().Pt() );
            //    bins.setJetEta( j->GetP4().Eta() );
            //    bins.setRho( std::min(e->Rho(),float(40.)) ); // corrections up to rho 40.

            //    float sigma=1.0 ;

            //    if ( jet_resolution->getRecord(bins)==NULL){ sigma=1.0;}
            //    else{ sigma =  jet_resolution->evaluateFormula( *jet_resolution->getRecord(bins),vars); }

            //    kf->p4.push_back( j->GetP4());
            //    kf->sigma.push_back(sigma);
            //}
            //kf->H=Hmm.GetP4();
            //kf->doGhost=false; 
            //kf->runGeneric();
            bool vhhadrres=false;
            bool highmjj=false; // if there is a pair with high mjj, veto it
            float met = e->GetMet().Pt();
            if (nbjets==0 and met <70){
                // loop over the selected jets local
                for(unsigned ijet=0;ijet < std::min(unsigned(selectedJets.size()),3U);++ijet) 
                    for(unsigned jjet=ijet+1;jjet < std::min(unsigned(selectedJets.size()),3U);++jjet) 
                    {
                        //compute the invariant mass between ijet and jjet 
                        //Log(__FUNCTION__,"DEBUG",Form("Considering VH couple %d,%d alphaS=%u",ijet,jjet,kf->alpha.size()));
                        TLorentzVector Z;
                        if ( Z.M()> 400) highmjj=true;

                        //if (selectedJets[ijet]->QGL()<0.5 or selectedJets[jjet]->QGL()<0.5) continue; 
                        if (selectedJets[ijet]->QGL() * selectedJets[jjet]->QGL()<0.2) continue; 
                        Z += selectedJets[ijet]->GetP4();
                        Z += selectedJets[jjet]->GetP4();

                        auto dphijj = selectedJets[ijet]->DeltaPhi(selectedJets[jjet]);
                        auto drmmjj = Hmm.GetP4().DeltaR(Z);
                        auto drjj = selectedJets[ijet]->DeltaR(selectedJets[jjet]);
                        
                        //if ( Z.M() > 60 and Z.M()< 110 and dphijj < 2.0 and drjj<2.0 and fabs(drmmjj-TMath::Pi() )<0.4){
                        if ( Z.M() > 60 and Z.M()< 110  and drjj<2.0 and fabs(drmmjj-TMath::Pi() )<0.4){
                            if (not vhhadrres) { // save jets that selected VH tag
                                selectedJetsVHHadr.push_back(selectedJets[ijet]);
                                selectedJetsVHHadr.push_back(selectedJets[jjet]);
                                //alphaVHHadr.push_back(kf->alpha[ijet]);
                                //alphaVHHadr.push_back(kf->alpha[jjet]);
                                //valueVHHadr = kf->value;
                            }
                            vhhadrres=true;
                        }
                    }

            }
            if (vhhadrres and not highmjj)category="VHHadr";
        }

        // TODO kin fit assuming met 0
    }

    // ggHX: I have a ggH with miniIso but not with std selection
    //if (category=="")
    //{
    //    if ( (mu0==NULL or mu1==NULL ) and mu0_local != NULL and mu1_local != NULL)
    //    {
    //        category = "ggHX";
    //    }
    //}

    // RESET Hmm WARNING!
    if (category != "" and category != "VHHadr")
    {
        Hmm.SetP4(zero); // make sure it is 0.
        mu0 = mu0_local;
        mu1 = mu1_local;
        Hmm += *mu0;
        Hmm += *mu1;
        mass_=Hmm.M();
        pt_ = Hmm.Pt();
        selectedJets.clear();
        for(auto j : selectedJetsMiniIso) selectedJets.push_back(j);
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

string HmumuAnalysis::CategoryBdtMIT(Event *e){
    bdt.clear();
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start CategoryBdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon0 eta0=%f | Pt=%f Pz=%f",mu0->Eta(),mu0->Pt(),mu0->GetP4().Pz()));
    if (VERBOSE>2)Log(__FUNCTION__,"DEBUG",Form("Muon1 eta1=%f | Pt=%f Pz=%f",mu1->Eta(),mu1->Pt(),mu1->GetP4().Pz()));
   

    SetVariable("Hpt",Hmm.Pt());    
    SetVariable("Heta",Hmm.Eta());    
    SetVariable("Hphi",Hmm.Phi());    
    SetVariable("deltaeta",fabs(mu0->Eta()-mu1->Eta()));    
    SetVariable("deltaphi",fabs(mu0->DeltaPhi(*mu1)));

    SetVariable("eta1",mu0->Eta());    
    SetVariable("eta2",mu1->Eta());    
    SetVariable("njets",selectedJets.size());    
    SetVariable("mt1",ChargedHiggs::mt(mu0->Pt(),e->GetMet().Pt(),mu0->Phi(),e->GetMet().Phi()));    
    SetVariable("mt2",ChargedHiggs::mt(mu1->Pt(),e->GetMet().Pt(),mu1->Phi(),e->GetMet().Phi()));    
    SetVariable("met",e->GetMet().Pt());    
    SetVariable("pass_leptonveto",passLeptonVeto);    

    // compute secondary jets variables: TODO unify computation of variables

    SetVariable("ncentjets",jetVar_["ncentjets"]);    
    SetVariable("htCent",jetVar_["htCent"]);    
    SetVariable("nbjets",jetVar_["nbjets"]);    
    SetVariable("maxDeepB",jetVar_["maxDeepB"]);    
    SetVariable("leadDeepB",jetVar_["leadDeepB"]);    
    SetVariable("maxCSV",jetVar_["maxDeepB"]); //Change to Deep
    //mjj vars
    SetVariable("mjj_1",jetVar_["mjj_1"]);
    SetVariable("mjj_2",jetVar_["mjj_2"]);
    SetVariable("detajj_1",jetVar_["detajj_1"]);
    SetVariable("detajj_2",jetVar_["detajj_2"]);

    //soft variables
    //SetVariable("softNjets1",jetVar_["softNjets1"]);    
    //SetVariable("softHt1",jetVar_["softHt1"]);    
    //SetVariable("softHt5",jetVar_["softHt5"]);    
    //SetVariable("softHt10",jetVar_["softHt10"]);    
    // qgl
    SetVariable("firstQGL",  jetVar_["firstQGL"]);
    SetVariable("secondQGL", jetVar_["secondQGL"]);
    SetVariable("thirdQGL",  jetVar_["thirdQGL"]);

    //Log(__FUNCTION__,"DEBUG","---------------------------");
    //unsigned int i=0;
    //for (const string& name : {"Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","firstQGL","secondQGL","thirdQGL","nbjets","maxDeepB","leadDeepB","maxCSV","mt1","mt2","met"})
    //{
    //    Log(__FUNCTION__,"DEBUG",Form("Var '%s' idx %d is %f",name.c_str(),i,GetVariable<float>(name)));
    //    ++i;
    //}
    //Log(__FUNCTION__,"DEBUG","---------------------------");

    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
         bdt.push_back(readers_[i]->EvaluateMVA("BDTG_default") );
    }

    int icat=-100; string catStr="";

    float mu_max_eta = std::max(fabs(mu0->Eta()),fabs(mu1->Eta()));
    float mu_ave_eta = (fabs(mu0->Eta())+fabs(mu1->Eta()))/2.;

    if (mu_max_eta <0.9) // BB
    {
        if (bdt[0] >0.92) icat=0;
        else if (bdt[0] > 0.79)icat=1;
        else if (bdt[0] > 0.5) icat=2;
        else                   icat=3;
    }
    else if (mu_max_eta <1.9){ // XO
        if (bdt[0]> 0.68) icat=4;
        else if(bdt[0] >0.25) icat=5;
        else icat=6;
    }
    else{
        if (bdt[0] >0.6) icat=7;
        else if (bdt[0] >0.1) icat=8;
        else icat=9;
    }

    if (icat>=0) catStr=Form("cat%d",icat);

    //if (doSync and not processingSyst_)
    //{
    //    //SetTreeVar("bdt",bdt[0]);
    //    //SetTreeVar("cat",icat);
    //}

    // SciKit

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + catStr);
    return catStr;
}

string HmumuAnalysis::CategoryBdtUCSD(Event *e){
    string category="";
    {
        //RAFFAELE
        //-1, 0, 0.4, 0.65, 1 --> raffaele
        float v=bdt[1];
        if (year==2016){
            if (v < -0.125) category="cat0"; 
            else if (v < 0.185 ) category="cat1";
            else if (v < 0.309 ) category="cat2";
            else if (v < 0.471 ) category="cat3";
            else category="cat4";
        }
        else if (year==2017){
            if (v < -0.113) category="cat0"; 
            else if (v < 0.191 ) category="cat1";
            else if (v < 0.315 ) category="cat2";
            else if (v < 0.498 ) category="cat3";
            else category="cat4";
        }
        else if (year==2018){
            if (v < -0.115) category="cat0"; 
            else if (v < 0.191 ) category="cat1";
            else if (v < 0.312 ) category="cat2";
            else if (v < 0.496 ) category="cat3";
            else category="cat4";
        }

    }
    return category;
}


string HmumuAnalysis::CategoryBdt(Event *e){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start CategoryBdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return "";
    

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

    //if (doSync and not processingSyst_)
    //{
    //    SetTreeVar("bdt",bdt[0]);
    //    SetTreeVar("cat",icat);
    //}

    // SciKit
    if (doScikit)
    {
    /*
       
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
        x.push_back( jetVar_["ncentjets"]);// ncentr jets ... todo
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
            sort(csv_score.begin(),csv_score.end(),std::greater<float>());
            sort(qgl_score.begin(),qgl_score.end(),[](const pair<float,float >&x,const pair<float, float >&y){ if (x.first> y.first) return true; if(x.first<y.first)return false; return x.second> y.second; } );

        x.push_back( htCent);
        x.push_back( passLeptonVeto);
        x.push_back( (mjj.size() >0 ) ? mjj[0].first : 0.0);
        x.push_back( (mjj.size() >1 ) ? mjj[1].first : 0.0);
        x.push_back( (mjj.size()>0) ? fabs(selectedJets[mjj[0].second.first]->Eta() - selectedJets[mjj[0].second.second]->Eta()): -1.0 ); 
        x.push_back( (mjj.size()>1) ? fabs(selectedJets[mjj[1].second.first]->Eta() - selectedJets[mjj[1].second.second]->Eta()): -1.0 );

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
    */
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Category: returning '" + catStr);
    return catStr;
}

void HmumuAnalysis::AddVariable( string name, char type){ 
    Log(__FUNCTION__,"INFO","Adding variable '"+name+"'");
    varValues_.Add(name,type); 

    if ( type == 'I') {
        int ir=-1;
        for(auto& r : readers_ ){ 
        ++ir;
        if (mvavars_.find(pair<string,int>(name,ir))!=mvavars_.end()) continue;
        mvavars_[pair<string,int>(name,ir)] =1;
        r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
    }}

    else if ( type == 'F') {
        int ir=-1;
        for(auto&r : readers_) { 
        ++ir;
        if (mvavars_.find(pair<string,int>(name,ir))!=mvavars_.end()) continue;
        mvavars_[pair<string,int>(name,ir)] =1;
        r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
    }}

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
    Log(__FUNCTION__,"ERROR","Scikit unimplemented in CC7");
    throw abortException(); // unimplemented in CC7
    //discr.push_back("BTDG");
    //discr.push_back("SGD");
    //discr.push_back("SVR");
    //discr.push_back("MLP");
    //discr.push_back("KERAS");

    //py . reset(new TPython);
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
    //py ->Exec("kmodel=keras.models.load_model('aux/hmm/keras.hd')");
    //py ->Exec("ridge=joblib.load('aux/hmm/Ridge.pkl')");
    //py ->Exec("en=joblib.load('aux/hmm/EN.pkl')");

    // Make sure we can use x[...] inside
    //PyObject* pyx = py->ObjectProxy_FromVoidPtr(&x, "std::vector<float>");
    //PyObject* pymain = PyImport_ImportModule("__main__");

    //PyModule_AddObject(pymain, "x", pyx);

    //Py_DECREF(pymain);
}

void HmumuAnalysis::InitTmva(int pos){
    Log(__FUNCTION__,"INFO","Init Reader");
    TMVA::Tools::Instance();

    if (readers_.size() != pos) {
            Log(__FUNCTION__,"ERROR","readers doesn't match pos") ;
            throw abortException();
    } 
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
    Log(__FUNCTION__,"INFO",Form("Loading weights idx=%d: ",pos)+weights[pos]);
    readers_[pos]->BookMVA("BDTG_default",weights[pos].c_str());

}

void HmumuAnalysis::InitTmvaMIT(int pos){
    Log(__FUNCTION__,"INFO","Init Reader");
    TMVA::Tools::Instance();
    if (readers_.size() != pos) {
            Log(__FUNCTION__,"ERROR","readers doesn't match pos") ;
            throw abortException();
    } 
    readers_ . push_back( new TMVA::Reader() );

    //varscheme = 3 "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","firstQGL","secondQGL","thirdQGL","nbjets","maxDeepB","leadDeepB","maxCSV","mt1","mt2","met",]
    AddVariable("Hpt",'F');    
    AddVariable("Heta",'F');    
    AddVariable("Hphi",'F');    
    AddVariable("deltaphi",'F');    
    AddVariable("deltaeta",'F');
    AddVariable("eta1",'F');    
    AddVariable("eta2",'F');    
    AddVariable("ncentjets",'F');    
    AddVariable("njets",'F');    
    AddVariable("htCent",'F');    
    AddVariable("pass_leptonveto",'F');    
    AddVariable("mjj_1",'F');    
    AddVariable("mjj_2",'F');    
    AddVariable("detajj_1",'F');    
    AddVariable("detajj_2",'F');    
    //AddVariable("softNjets1",'F');    
    //AddVariable("softHt1",'F');    
    //AddVariable("softHt5",'F');    
    //AddVariable("softHt10",'F');    
    AddVariable("firstQGL",'F');    
    AddVariable("secondQGL",'F');    
    AddVariable("thirdQGL",'F');    
    AddVariable("nbjets",'F');    
    AddVariable("maxDeepB",'F');    
    AddVariable("leadDeepB",'F');    
    AddVariable("maxCSV",'F');    
    AddVariable("mt1",'F');    
    AddVariable("mt2",'F');    
    AddVariable("met",'F');    

    // 
    AddSpectator("bdt",'F');SetVariable("bdt",0.);
    AddSpectator("mass",'F');SetVariable("mass",0.);
    AddSpectator("weight*((mc<-10&&mc>=-19)*48.58*0002176+(mc<-20&&mc>=-29)*3.782*0.0002176+(mc<-30&&mc>=-39)*0.8839*0.0002176+(mc<-40&&mc>=-49)*0.5328*0.0002176+(mc<-50&&mc>-59)*0.84*0.0002176+(mc<-60&&mc>=-69)*0.5071*0.0002176)",'F');SetVariable("weight*((mc<-10&&mc>=-19)*48.58*0002176+(mc<-20&&mc>=-29)*3.782*0.0002176+(mc<-30&&mc>=-39)*0.8839*0.0002176+(mc<-40&&mc>=-49)*0.5328*0.0002176+(mc<-50&&mc>-59)*0.84*0.0002176+(mc<-60&&mc>=-69)*0.5071*0.0002176)",0.);

    // load weights
    Log(__FUNCTION__,"INFO",Form("Loading weights idx=%d: ",pos)+weights[pos]);
    readers_[pos]->BookMVA("BDTG_default",weights[pos].c_str());

}

void HmumuAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");
    rnd_ . reset( new TRandom3() ) ;

    if (not jet_resolution)
    { // init jet resolution
        Log(__FUNCTION__,"INFO","Init JER");
        jet_resolution.reset( new JME::JetResolutionObject("aux/jer/Summer16_25nsV1/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt") ) ;
    }

    if (not kf)
    {
        Log(__FUNCTION__,"INFO","Init KF");
        kf.reset (new KinematicFit);
        kf->genericType=1;
        kf->lambda=1.; // lambda->0, MET is neglected, lambda->inf MET->0
    }

    // define categories -- for booking histos
    Log(__FUNCTION__,"INFO",Form("Loading Configuration with catType=%d",catType) );
    if (catType>=1)
    {
        categories_.clear();
        for (int i=0;i<30;++i) categories_.push_back(Form("cat%d",i));
        //if (catType<4){}
        InitTmva(0); InitTmvaUCSD(1,0);//InitTmvaUCSD(2,2);
        if (catType>=10)InitTmvaMIT(0); //outdated
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
    if(doUnbinned)
    {
        InitTree("final");
        Branch("final","cat",'I');
        Branch("final","mmm",'D');
    }

	Log(__FUNCTION__,"INFO","Booking Histograms");
    for ( string l : AllLabel()  ) {
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 360,60,150);
	    Book ("HmumuAnalysis/Vars/NpvOnZ_"+ l ,"Npv;Npv;Events", 100,0,100);
	    Book ("HmumuAnalysis/Vars/RhoOnZ_"+ l ,"Rho;Rho;Events", 400,0,100);
	    Book ("HmumuAnalysis/Vars/NpvOnH_"+ l ,"Npv;Npv;Events", 100,0,100);
	    Book ("HmumuAnalysis/Vars/RhoOnH_"+ l ,"Rho;Rho;Events", 400,0,100);

	    //Book ("HmumuAnalysis/Vars/NpvOnZ_norwgt_"+ l ,"Npv;Npv;Events", 100,0,100);
	    //Book ("HmumuAnalysis/Vars/NpvOnH_norwgt_"+ l ,"Npv;Npv;Events", 100,0,100);
	    //Book ("HmumuAnalysis/Vars/NpvAll_norwgt_"+ l ,"Npv;Npv;Events", 100,0,100);
	    Book ("HmumuAnalysis/Vars/NpvAll_"+ l ,"Npv;Npv;Events", 100,0,100);

	    // Control variables
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
	    Book ("HmumuAnalysis/Vars/MuonPt_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MuonEta_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,-5,5);

	    Book ("HmumuAnalysis/Vars/MuonMiniIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
	    Book ("HmumuAnalysis/Vars/MuonMiniIso_Pt_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MuonMiniIso_Eta_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,-5,5);

	    //Book2D ("HmumuAnalysis/Vars/PtVsMassOnZ_"+ l ,"Mmm;m^{#mu#mu} [GeV];p_{T}^{#mu#mu};Events", 800,60,200,200,0,200);
	    Book ("HmumuAnalysis/Vars/MetOnZ_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/RawMetOnZ_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/TrackMetOnZ_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_nojec_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/RawMetOnH_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/TrackMetOnH_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnZ_rw_"+ l ,"Met On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/MetOnH_rw_"+ l ,"Met On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ0j_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ1j_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnZ2j_"+ l ,"Pt On Z (70-110);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnH_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnH_no_nnlops_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/PtOnH_no_btagsf_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
        
        // bdt variables
	    Book ("HmumuAnalysis/Vars/RapOnH_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,-5,5);
	    Book ("HmumuAnalysis/Vars/CosThetaCSOnH_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,-1,1);
	    Book ("HmumuAnalysis/Vars/PhiCSOnH_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,-3.1416,3.1416);
	    Book ("HmumuAnalysis/Vars/MinDrMuJOnH_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,0,10.);
    
	    Book ("HmumuAnalysis/Vars/PtOnH_2jet_"+ l ,"Pt On Hmm (110-150);Met [GeV];Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/CosThetaCSOnH_2jet_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,-1,1);
	    Book ("HmumuAnalysis/Vars/PhiCSOnH_2jet_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,-3.1416,3.1416);
	    Book ("HmumuAnalysis/Vars/MinDrMuJOnH_2jet_"+ l ,"Rapifity On Hmm (110-150);Met [GeV];Events", 200,0,10.);

        // bdt
	    Book ("HmumuAnalysis/Vars/BdtOnZ_"+ l ,"Bdt On Z (70-110);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/BdtOnH_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/BdtOnH_BB_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);

	    Book ("HmumuAnalysis/Vars/BdtOnH_Prefire_"+ l ,"Bdt On Hmm for events that prefire (110-150);Bdt;Events", 1000,-1,1); // useful only if run unprefirebale and data
	    Book ("HmumuAnalysis/Vars/BdtOnH_NoPrefire_"+ l ,"Bdt On Hmm for events that prefire (110-150);Bdt;Events", 1000,-1,1); // w/o prefire weights
        //USCD
	    Book ("HmumuAnalysis/Vars/Bdt01jUCSDOnH_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/Bdt2jUCSDOnH_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
        //
	    Book ("HmumuAnalysis/Vars/Bdt01jUCSDOnZ_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/Bdt2jUCSDOnZ_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
        //no puidsf
	    Book ("HmumuAnalysis/Vars/Bdt01jUCSDOnH_nopuidsf_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/Bdt2jUCSDOnH_nopuidsf_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
        //
	    Book ("HmumuAnalysis/Vars/Bdt01jUCSDOnZ_nopuidsf_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);
	    Book ("HmumuAnalysis/Vars/Bdt2jUCSDOnZ_nopuidsf_"+ l ,"Bdt On Hmm (110-150);Bdt;Events", 1000,-1,1);

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
	    Book ("HmumuAnalysis/Vars/DeltaEtaJJ1OnH_"+ l ,"EtaJet1 On Hmm (110-150);Bdt;Events", 200,0,10);

        // study of jet quality in 2.65-3.139 for 2017
        {
            std::vector<int> ptbins{0,30,50,80,120,8000};
            for (const string&v : {"axis1","axis2","ptD","mult","cemf","nemf","ptrawnemf"})
            for (int ipt=0;ipt<ptbins.size()-1;++ipt)
            {
                int nbins=200; float xmin=0,xmax=1;
                if (v=="axis1" or v=="axis2") {xmin=0; xmax=10;}
                if (v=="mult") {xmin=0; xmax=100;nbins=100;}
                Book ("HmumuAnalysis/Vars/"+v+"_Jet1OnH_Eta2p65_2p139_"+Form("pt%d_%d_",ptbins[ipt],ptbins[ipt+1])+ l ,"EtaJet1 On Hmm (110-150);Bdt;Events", nbins,xmin,xmax);
            }
        }


        // btag, soft track activity
	    //Book ("HmumuAnalysis/Vars/SoftNJetsOnH_"+ l ,"NJets On Hmm (110-150);Bdt;Events", 10,0,10);
	    //Book ("HmumuAnalysis/Vars/SoftHtOnH_"+ l ,"SoftHT On Hmm (110-150);Bdt;Events", 1000,0,1000);
	    Book ("HmumuAnalysis/Vars/DeepBOnH_"+ l ,"DeepB On Hmm (110-150);Bdt;Events", 100,0,1);
	    Book ("HmumuAnalysis/Vars/DeepBOnH_no_btagsf_"+ l ,"DeepB On Hmm (110-150);Bdt;Events", 100,0,1);
	    Book ("HmumuAnalysis/Vars/MaxCSVOnH_"+ l ,"DeepB On Hmm (110-150);Bdt;Events", 100,0,1);

        for (string jetType :{"B","C","L"})
        {
	        Book ("HmumuAnalysis/Vars/DeepBOnZ_"+jetType+"_"+ l ,"DeepB On Hmm (70-110);Deep;Events", 100,0,1);
	        Book ("HmumuAnalysis/Vars/DeepBOnZ_emu_"+jetType+"_"+ l ,"DeepB On Hmm (70-110);Deep;Events", 100,0,1);
        }
        // qg
	    Book ("HmumuAnalysis/Vars/QGLJet1OnH_"+ l ,"QGL On Hmm (110-150);QGL;Events", 100,0,1);
        // mt
	    Book ("HmumuAnalysis/Vars/Mt1OnH_"+ l ,"QGL On Hmm (110-150);QGL;Events", 200,0,1000);
	    Book ("HmumuAnalysis/Vars/Mt2OnH_"+ l ,"QGL On Hmm (110-150);QGL;Events", 200,0,1000);
	    Book ("HmumuAnalysis/Vars/CosThetaCSOnH_"+ l ,"CosThetaStar;#cos(#theta^{*}) ;Events", 200,-1,1); 

        // exclusive categories related variables
	    Book ("HmumuAnalysis/Vars/MtOnH_WLep_"+ l ,"Mt;mt [GeV];Events", 1000,0,1000); 
	    Book ("HmumuAnalysis/Vars/DeltaR_ggHX_"+ l ,"dR ggHX;dR;Events", 1000,0,10); 
	    Book ("HmumuAnalysis/Vars/NJets_ggHX_"+ l ,"nJets ggHX;nJets;Events", 10,0,10); 
        
        // VHHadr Opt
	    Book ("HmumuAnalysis/Vars/DPhijj_VHHadr_"+ l ,"DPhijj;#Delta#phi(j,j) ;Events", 100,0,3.1416); 
	    Book ("HmumuAnalysis/Vars/DEtajj_VHHadr_"+ l ,"DEtajj;#Delta#eta(j,j) ;Events", 100,0,10); 
	    Book ("HmumuAnalysis/Vars/CosThetaCSmm_VHHadr_"+ l ,"CosThetaStar;#cos(#theta^{*}) ;Events", 200,-1,1); 
	    Book ("HmumuAnalysis/Vars/CosThetaCS_VHHadr_"+ l ,"CosThetaStar;#cos(#theta^{*}) ;Events", 200,-1,1); 
	    Book ("HmumuAnalysis/Vars/NJets_VHHadr_"+ l ,"NJets;N_{jets} ;Events", 10,0,10); 
	    Book ("HmumuAnalysis/Vars/Ptjj_VHHadr_"+ l ,"Ptjj;ptJJ ;Events", 200,0,1000); 
	    Book ("HmumuAnalysis/Vars/DPhimmjj_VHHadr_"+ l ,"DPhijj;#Delta#phi(j,j) ;Events", 100,0,3.1416); 
	    Book ("HmumuAnalysis/Vars/DEtammjj_VHHadr_"+ l ,"DEtajj;#Delta#eta(j,j) ;Events", 100,0,10); 
        //
	    Book ("HmumuAnalysis/Vars/Mjjkf_VHHadr_"+ l ,"Ptjj;ptJJ ;Events", 200,0,1000); 
	    Book ("HmumuAnalysis/Vars/Malljkf_VHHadr_"+ l ,"Ptjj;ptJJ ;Events", 200,0,1000); 
	    Book ("HmumuAnalysis/Vars/Metkf_VHHadr_"+ l ,"Ptjj;ptJJ ;Events", 200,0,1000); 
	    //Book ("HmumuAnalysis/Vars/Valuekf_VHHadr_"+ l ,"Ptjj;ptJJ ;Events", 1000,0,100); 

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

	    //Book2D ("HmumuAnalysis/Vars/BdtMass_UF_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);
	    Book2D ("HmumuAnalysis/Vars/BdtMass_UCSD1_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);
	    Book2D ("HmumuAnalysis/Vars/BdtMass_UCSD2_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);

	    Book2D ("HmumuAnalysis/Vars/BdtMassNonVbf_UCSD1_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);
	    Book2D ("HmumuAnalysis/Vars/BdtMassNonVbf_UCSD2_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);

	    Book2D ("HmumuAnalysis/Vars/BdtMassFull_UCSD1_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);
	    Book2D ("HmumuAnalysis/Vars/BdtMassFull_UCSD2_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,110,150,200,-1,1);

	    Book ("HmumuAnalysis/Vars/Mbb_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mbbcorr_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mbbkf2_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mbbkf3_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Bres_HbbHmm_"+ l ,"B Res;res;Events", 2000,-5,5.);
	    Book ("HmumuAnalysis/Vars/Jres_HbbHmm_"+ l ,"B Res;res;Events", 2000,-5,5.);
	    //Book ("HmumuAnalysis/Vars/Mmm_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160); // 2000 60 160
	    Book ("HmumuAnalysis/Vars/Mmm_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 20000,0,1000); // 2000 60 160
	    Book2D ("HmumuAnalysis/Vars/Mmm_HbbHmm_2D_"+ l ,"Mass Hmm and Hbb;Hbb+mm;Events", 2000,0,1000,1000,1000,1000); // 2000 60 160
	    Book ("HmumuAnalysis/Vars/Mmm_KF2_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mmm_KF3_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mem_KF2_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);
	    Book ("HmumuAnalysis/Vars/Mem_KF3_HbbHmm_"+ l ,"Mass (110-150);Hbbw;Events", 2000,60,160);

	    //Book2D ("HmumuAnalysis/Vars/M4m_HmmHmm_"+ l ,"Mass 4 muons XX compatibles;m_{4#mu};Events", 8000,0,2000,1000,0,500);

        // --- histograms for limits extraction
        for(const auto & c : categories_)
        {
	        Book ("HmumuAnalysis/Vars/Mmm_"+ c + "_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 2000,60,160); // every 4 (old16) per GeV
	        //Book2D ("HmumuAnalysis/Vars/JetEtaPhi_"+ c + "_"+ l ,"Jet EtaPhi; eta; phi;Events", 100,-5,5,100,-3.1415,3.1415); // CHECK2D
            if (doSTXS){
                // trigger stxs filling
                STXS::ENumToString(STXS::Stage1p1::GG2HLL_PTV_0_75);
                if (l == "GluGlu_HToMuMu_M125" or 
                        l=="VBF_HToMuMu_M125" or 
                        l=="WPlusH_HToMuMu_M125" or
                        l=="WMinusH_HToMuMu_M125" or 
                        l=="ZH_HToMuMu_M125" or
                        l=="ttH_HToMuMu_M125"){
                    for (const auto &x : STXS::stxs_to_string){
                        if (x.second != "" )Book("HmumuAnalysis/Vars/Mmm_"+ c + "_" + l+ "_STXS_125_"+x.second , "Mmm;m^{#mu#mu} [GeV];Events;"+x.second, 2000,60,160);
                    }
                }// samples
            }//STXS
        }

    } //end label loop

    if (doSync){
        InitTree("hmm");
        Branch("hmm","cat",'I');
        Branch("hmm","weight",'D');
        Branch("hmm","mcWeight",'D');
        Branch("hmm","puWeight",'D');
        Branch("hmm","baremcWeight",'D');
        Branch("hmm","muonSF",'D');
        Branch("hmm","btagSF",'D');
        Branch("hmm","nnlopsSF",'D');
        Branch("hmm","l1SF",'D');
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
        Branch("hmm","HY",'F');
        Branch("hmm","njets",'I');
        Branch("hmm","nbjets",'I'); // default btagging
        Branch("hmm","ncentjets",'I');
        Branch("hmm","nfwdjets",'I');
        Branch("hmm","jet_pt",'d',20,"njets");
        Branch("hmm","jet_eta",'d',20,"njets");
        Branch("hmm","jet_phi",'d',20,"njets");

        Branch("hmm","drmj",'F');
        Branch("hmm","detajj_lead",'F');
        Branch("hmm","dphijj_lead",'F');
        Branch("hmm","zepen",'F');

        // Deep B-tagging. HardestJet. Higher btagged jet.
        Branch("hmm","leadDeepB",'F'); 
        Branch("hmm","maxDeepB",'F'); 
        // Soft ACtivity
        //Branch("hmm","softNjets1",'I'); 
        //Branch("hmm","softHt1",'F'); 
        //Branch("hmm","softNjets2",'I'); 
        //Branch("hmm","softHt2",'F'); 
        //Branch("hmm","softNjets5",'I'); 
        //Branch("hmm","softHt5",'F'); 
        //Branch("hmm","softNjets10",'I'); 
        //Branch("hmm","softHt10",'F'); 
        
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
        Branch("hmm","mjj_lead",'F');
        Branch("hmm","dphijj_lead",'F');
        Branch("hmm","mjj_1",'F');
        Branch("hmm","mjj_2",'F');
        Branch("hmm","detajj_1",'F');
        Branch("hmm","detajj_2",'F');
        // muo
        Branch("hmm","deltaphi",'F');
        Branch("hmm","deltaeta",'F');
        Branch("hmm","costhetastar",'F');
        Branch("hmm","phics",'F');
        //Branch("hmm","",'F');

        //photons
        //Branch("hmm","npho",'I'); // with eta less than 1.44
        //Branch("hmm","pho_pt",'d',20,"npho");
        //Branch("hmm","pho_eta",'d',20,"npho");
        //Branch("hmm","pho_dr1",'d',20,"npho");
        //Branch("hmm","pho_dr2",'d',20,"npho");

        Branch("hmm","bdt",'F');
        Branch("hmm","bdtUCSD01j",'F');
        Branch("hmm","bdtUCSD2j",'F');
        if (doScikit) Branch("hmm","keras",'F');

        Branch("hmm","mjj_1_JESUp",'F');
        Branch("hmm","mjj_2_JESUp",'F');

        Branch("hmm","mjj_1_JESDown",'F');
        Branch("hmm","mjj_2_JESDown",'F');
    }

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Init");

}

int HmumuAnalysis::analyze(Event *event, string systname)
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
    jetVar_.clear();
    bdt.clear();
    mu0=nullptr;
    mu1=nullptr;

    // cache to understand if the event as been processed when I arrived at the End() function
    // ----------------------------------------
    //
    //if (SYNC_VERBOSE and not processingSyst_ and e->runNum()==297722 and e->lumiNum()==47 and e->eventNum()==12105464)
    //    Log(__FUNCTION__,"SYNC",Form("Analyze event %ld:%ld:%ld",e->runNum(),e->lumiNum(),e->eventNum()));

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);
    string label = GetLabel(e);
    string labelSTXS="";

    // translate to a consistent set
    if ( label == "GluGluHToMuMu_M120") label = "GluGlu_HToMuMu_M120";
    if ( label == "GluGluHToMuMu_M125") label = "GluGlu_HToMuMu_M125";
    if ( label == "GluGluHToMuMu_M130") label = "GluGlu_HToMuMu_M130";

    if ( label == "VBFHToMuMu_M120") label = "VBF_HToMuMu_M120";
    if ( label == "VBFHToMuMu_M125") label = "VBF_HToMuMu_M125";
    if ( label == "VBFHToMuMu_M130") label = "VBF_HToMuMu_M130";

    if ( label == "WplusH_HToMuMu_WToAll_M120") label = "WPlusH_HToMuMu_M120";
    if ( label == "WplusH_HToMuMu_WToAll_M125") label = "WPlusH_HToMuMu_M125";
    if ( label == "WplusH_HToMuMu_WToAll_M130") label = "WPlusH_HToMuMu_M130";

    if ( label == "WminusH_HToMuMu_WToAll_M120") label = "WMinusH_HToMuMu_M120";
    if ( label == "WminusH_HToMuMu_WToAll_M125") label = "WMinusH_HToMuMu_M125";
    if ( label == "WminusH_HToMuMu_WToAll_M130") label = "WMinusH_HToMuMu_M130";

    if ( label == "ZH_HToMuMu_ZToAll_M120") label = "ZH_HToMuMu_M120";
    if ( label == "ZH_HToMuMu_ZToAll_M125") label = "ZH_HToMuMu_M125";
    if ( label == "ZH_HToMuMu_ZToAll_M130") label = "ZH_HToMuMu_M130";

    if ( label == "ttHToMuMu_M120") label = "ttH_HToMuMu_M120";
    if ( label == "ttHToMuMu_M125") label = "ttH_HToMuMu_M125";
    if ( label == "ttHToMuMu_M130") label = "ttH_HToMuMu_M130";

    if ( label == "bbHToMuMu_M-125") label = "bbH_HToMuMu_M125";

    //multiPD. No need to keep separate histograms, but save the flags
    if (label.find("SingleMuon") !=string::npos) {isSingleMuon=true; label="Data";} else { isSingleMuon=false;}
    if (label.find("DoubleMuon") !=string::npos) {isDoubleMuon=true; label="Data";} else {isDoubleMuon=false;}

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
     * NVTX: Removed. Done with MetPhiCorrector
     */
    
    float nvtxrwgt=1.0;
    //if (year==2016 and not e->IsRealData()) { e->SetPtEtaSF("nvtx2016",std::min(e->Npv(),60),0); e->ApplySF("nvtx2016");nvtxrwgt= e->GetWeight()->GetSF("nvtx2016")->get();}
    //if (year==2017 and not e->IsRealData()) { e->SetPtEtaSF("nvtx2017",std::min(e->Npv(),60),0); e->ApplySF("nvtx2017");nvtxrwgt=e->GetWeight()->GetSF("nvtx2017")->get(); }
    //if (year==2018 and not e->IsRealData()) { e->SetPtEtaSF("nvtx2018",std::min(e->Npv(),60),0); e->ApplySF("nvtx2018");nvtxrwgt=e->GetWeight()->GetSF("nvtx2018")->get();}

    /*
     * L1 PreFiring Map
     */

    //#warning NO_PREFIRE
    double l1prefire=1.0;
    if(year==2017)l1prefire=e->ApplyL1PreFire(2017); // apply to the event weight, but keep it for comp
    else if(year==2016)l1prefire=e->ApplyL1PreFire(2016); // apply to the event weight, but keep it for comp
    if (doSync and not processingSyst_) { SetTreeVar("l1SF",l1prefire);}
    //Log(__FUNCTION__,"DEBUG",Form("L1Prefire is %lf",l1prefire)); 

    /*
     * HIGGS REWEIGHT -- UNCERTAINTIES
     */

    double nnlops = 1.0;
    if (not e->IsRealData()){

        if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start HTXS Classification");

        HTXS::HiggsClassification  hc;
        STXS::HiggsClassification  hc2;
        if (label.find( "GluGlu_HToMuMu") != string::npos) {
            hc.prodMode = HTXS::GGF;
            hc2.prodMode = STXS::GGF;
        }
        else if (label.find( "VBF_HToMuMu") != string::npos) {
            hc.prodMode = HTXS::VBF;
            hc2.prodMode = STXS::VBF;
        }
        else if (label.find( "ZH_HToMuMu") != string::npos) {
            if (e->GetPdfId(1) <=6 and e->GetPdfId(2) <=6)
            {
                hc.prodMode = HTXS::QQ2ZH; // I should look into the PDF productions GG2ZH vs QQ2ZH
                hc2.prodMode = STXS::QQ2ZH; 
            }
            else
            {
                hc.prodMode = HTXS::GG2ZH; // I should look into the PDF productions GG2ZH vs QQ2ZH
                hc2.prodMode = STXS::GG2ZH; 
            }
        }
        else if (label.find( "WMinusH_HToMuMu") != string::npos){ 
            hc.prodMode = HTXS::WH;
            hc2.prodMode = STXS::WH;
        }
        else if (label.find( "WPlusH_HToMuMu") != string::npos) {
            hc.prodMode = HTXS::WH;
            hc2.prodMode = STXS::WH;
        }
        else if (label.find( "ttH_HToMuMu") != string::npos){
            hc.prodMode = HTXS::TTH;
            hc2.prodMode = STXS::TTH;
        }
        else {
            hc.prodMode = HTXS::UNKNOWN;
            hc2.prodMode = STXS::UNKNOWN;
        }

        
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
            
            if (Higgs != NULL) {
                hc.higgs =  Higgs->GetP4();
                hc2.higgs =  Higgs->GetP4();
            }
            if (mu0T != NULL) {
                hc. p4decay_higgs = mu0T -> GetP4();
                hc2. p4decay_higgs = mu0T -> GetP4();
            }
            if (V != NULL){
                hc. V = V -> GetP4();
                hc2. V = V -> GetP4();
            }

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
            // Stage 1p1
            hc2.stage1p1_cat= STXS::getStage1p1Category(hc2.prodMode, hc2.higgs, jets, hc2.V, quarkDecayed);


            SF_WG1* sf_wg1 = dynamic_cast<SF_WG1*> (e->GetWeight()->GetSF("wg1") ); 
            sf_wg1->SetNjets30(jets.size());
            sf_wg1->SetPTH(hc.higgs.Pt());
            sf_wg1->SetSTXS(hc.stage1_cat_pTjet30GeV);
            sf_wg1->SetSTXS1p1(hc2.stage1p1_cat);
            
            e->ApplySF("wg1");
            //else if (hc.prodMode == HTXS::VBF )  // QQ2HQQ hc.stage1_cat_pTjet30GeV ~200
            //{
            //    //tot   |  200  | Mjj60 | Mjj120 | Mjj350 | Mjj700 | Mjj1000 | Mjj1500 |   25   |  2jet 
            //}

            if (hc.prodMode == HTXS::GGF)
            {
                /*****************/
                /*     NNLO PS   */
                /*****************/
                SF_NNLOPS_Reweight* sf_nnlops = dynamic_cast<SF_NNLOPS_Reweight*> (e->GetWeight()->GetSF("nnlops") ); 
                sf_nnlops->set(hc.higgs.Pt(),jets.size());
                //Log(__FUNCTION__,"DEBUG",Form("Applying SF NNLOPS %lf",sf_nnlops->get()));
                e->ApplySF("nnlops");
                nnlops=sf_nnlops->get();

                //Log(__FUNCTION__,"DEBUG",Form("NNLOPS is %lf",nnlops)); 
            }
            if (doSTXS)
            {
                labelSTXS=STXS::ENumToString(hc2.stage1p1_cat);
            }
        }

    }
    if (doSync and not processingSyst_) { SetTreeVar("nnlopsSF",nnlops); }


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

    //if (SYNC_VERBOSE and not processingSyst_ and e->runNum()==297722 and e->lumiNum()==47 and e->eventNum()==12105464)
    //    Log(__FUNCTION__,"SYNC",Form("Selected Muons are: mu0=%ld mu1=%ld pt=%f,%f",long(mu0),long(mu1),(mu0)?mu0->Pt():0,(mu1)?mu1->Pt():0));

    Hmm.SetP4(zero); // make sure it is 0.

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

        TLorentzVector tmp0,tmp1,tmp;
        tmp0.SetPtEtaPhiM(mu0->Pt(),mu0->Eta(),mu0->Phi(),0.105);
        tmp1.SetPtEtaPhiM(mu1->Pt(),mu1->Eta(),mu1->Phi(),0.105);
        tmp=tmp0+tmp1;
        if (mass_<0 or fabs((tmp.M()-Hmm.M())/tmp.M() ) >0.01  ) // bug in TLorentzVector? numerical instability?
        {
            Hmm.SetP4(tmp);
            mu0->SetP4(tmp0);
            mu1->SetP4(tmp1);
            Log(__FUNCTION__,"WARNING",Form("Resetting muon mass to 0.105, and hmm. (numerical instability of TLV?) event=%d/%d/%d",e->runNum(),e->lumiNum(),e->eventNum()));
            Log(__FUNCTION__,"WARNING",Form("mu0 pt=%f, eta=%f, phi=%f, mass=%f, rap=%f",mu0->Pt(),mu0->Eta(),mu0->Phi(),mu0->M(),mu0->Rapidity()));
            Log(__FUNCTION__,"WARNING",Form("mu1 pt=%f, eta=%f, phi=%f, mass=%f, rap=%f",mu1->Pt(),mu1->Eta(),mu1->Phi(),mu1->M(),mu1->Rapidity()));
            Log(__FUNCTION__,"WARNING",Form("Hmm pt=%f, eta=%f, phi=%f, mass=%f, mass_=%f, mass3=%f, rap=%f",Hmm.Pt(),Hmm.Eta(),Hmm.Phi(),Hmm.M(),mass_,tmp.M(),Hmm.Rapidity()));
        }
    }

    // ------------------------
    // BTAG SF: before using jets. (if reshaping)
    // -----------------------
  
    //#warning NO_BTAGSF 
    
    double btagsf=1; 
    if (true) // CSV-SF for passing loose,medium or tigth cuts
    {
        btagsf=e->ApplyBTagSF(3,year); //0 loose, 1 medium, 2 tight, 3 reshaping
        //Log(__FUNCTION__,"DEBUG",Form("BTag SF is %lf",btagsf));
    }

    double puidsf=1.;
    if (true) 
    {
        puidsf=e->ApplyPuIdSF(year); //working point set in the sf configuration. Applying both pass and fails.
    }
    
    //Log(__FUNCTION__,"DEBUG",Form("BtagSF is %lf",btagsf)); 
    if (doSync and not processingSyst_) { SetTreeVar("btagSF",btagsf);}
    // select jets
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetJets: ");

    selectedJets.clear();
    for(unsigned i=0;i<e->Njets() ; ++i)
    {
        Jet *j=e->GetJet(i);
        if (selectedJets.size() == 0 and j->Pt() <30) continue;
        selectedJets.push_back(j);
    }
    updateMjj();

    ///

    // ------------------- DIRTY CATEGORIES for studies

    // ------------------------------------
    // OFFICIAL CATEGORIZATION
    string category;

    BdtUF(0);
    BdtUCSD(1,0); // --> nonvbf
    //BdtUCSD(2,2); --> vbf

    if (catType==2) category = CategoryBdt(e);
    else if (catType>=3 and catType <10) { 
        category = CategoryBdtUCSD(e);  // 0-5

        string categoryExc = CategoryExclusive(e); 
        recoMuons = (mu0 != NULL and mu1 !=NULL);  // muons may have been recomputed here
        isExclusiveCat = ( categoryExc!="" and categoryExc!="ggHX");
        if (categoryExc == "ggHX" and category == "") isExclusiveCat=true;

        if (categoryExc == "ttHHadr") category = "cat5";// 15
        if (categoryExc == "ttHLep" ) category="cat6";  // 
        if (categoryExc == "ZHLep" ) category="cat7";   // 
        if (categoryExc == "WHLep" ) category="cat8";   // 
        if (categoryExc == "VHHadr" ) category="cat9";   // 
        //if (categoryExc == "ggHX" and category == "" ) category="cat14";   

        // enforce it for VBF Pisa
        ////if (jetVar_["mjj_1"] > 400) category="cat14";
        //#warning NOMJJ
        if (catType == 4 and jetVar_["mjj_lead"] > 400 and abs(jetVar_["detajj_lead"])>2.5 and selectedJets.size() >0 and selectedJets[0]->Pt()>35 and categoryExc != "ttHHadr" and categoryExc != "ttHLep") category="cat10";
    } 
    else if (catType ==10)
    {
        category = CategoryBdtMIT(e); 
    }
    else if (catType ==11) // MIT + exclusive
    {
        category = CategoryBdtMIT(e); 
        string categoryExc = CategoryExclusive(e); 
        recoMuons = mu0 != NULL and mu1 !=NULL;  // muons may have been recomputed here
        isExclusiveCat = ( categoryExc!="" and categoryExc!="ggHX");
        if (categoryExc == "ggHX" and category == "") isExclusiveCat=true;

        if (categoryExc == "ttHHadr") category = "cat10";// 
        if (categoryExc == "ttHLep" ) category="cat11";  // 
        if (categoryExc == "ZHLep" ) category="cat12";   // 
        if (categoryExc == "WHLep" ) category="cat13";   // 
        if (categoryExc == "VHHadr" ) category="cat14";   // 
        if (categoryExc == "ggHX" and category == "" ) category="cat15";   // 

        // enforce it for VBF Pisa
        //if (jetVar_["mjj_1"] > 400) category="cat16";
        //if (jetVar_["mjj_lead"] > 400) category="cat16";
    }
    else category = Category(mu0, mu1, selectedJets);

    // --------------- DIRTY CATEGORIES -----------
    //string categoryDirty=""; // this may double count

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

        if (doSync and not processingSyst_) {SetTreeVar("muonSF",sf->sf);}
    }
    
    // ***************************
    // **       preselection    **
    // ***************************
    

    // Trigger
    passAsymmPtCuts = (recoMuons and  mu0->Pt() >29 and mu1->Pt() >20 );
    if(year==2016 or year==2018) passAsymmPtCuts = (recoMuons and  mu0->Pt() >26 and mu1->Pt() >20 );
    if (multipd_) passAsymmPtCuts= (recoMuons and mu0->Pt() >26 and mu1->Pt() >20);

    if (year==2016) {passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); }
    else if (year==2017) {passTrigger=e->IsTriggered("HLT_IsoMu27_v") or e->IsTriggered("HLT_IsoTkMu27_v"); }
    else if (year==2018) {passTrigger=e->IsTriggered("HLT_IsoMu24_v") or e->IsTriggered("HLT_IsoTkMu24_v"); }

    passTrigger1 = false; 
    passTrigger2 = false;

    if (recoMuons) 
    {
        //cout <<" DOING TRIGGER MATCHING "<<endl;
        bool passTriggerEvent = passTrigger;
        if (year==2016) passTrigger1 = (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;
        else if (year==2017) passTrigger1 = (e->IsTriggered("HLT_IsoMu27_v",mu0) or e->IsTriggered("HLT_IsoTkMu27_v",mu0)) ;
        else if (year==2018) passTrigger1 = (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;

        //passTrigger1 = passTrigger1 and mu0->GetTightId();

        if ( (mu1->Pt() > 29 and year==2017) or (mu1->Pt()>26 and (year== 2016 or year==2018) )) 
        {
            if (year==2016) passTrigger2 = (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
            else if (year==2017) passTrigger2 = (e->IsTriggered("HLT_IsoMu27_v",mu1) or e->IsTriggered("HLT_IsoTkMu27_v",mu1)) ;
            else if (year==2018) passTrigger2 = (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
            //passTrigger2 = passTrigger2 and mu1->GetTightId();
        }
        passTrigger=passTrigger1 or passTrigger2;
        #warning DISABLING_TRIGGER_MATCHING_2016
        if (year==2016) passTrigger=passTrigger or passTriggerEvent;

        if (multipd_) // switch to HLT_IsoMu24. 
        {
            // The or, is there for the prescale case. May change with sf and syst
            passTrigger1 = passTrigger1 || (e->IsTriggered("HLT_IsoMu24_v",mu0) or e->IsTriggered("HLT_IsoTkMu24_v",mu0)) ;
            passTrigger2 = passTrigger2 || (e->IsTriggered("HLT_IsoMu24_v",mu1) or e->IsTriggered("HLT_IsoTkMu24_v",mu1)) ;
            passTrigger=passTrigger1 or passTrigger2;
        }
        
        if (multipd_ and isDoubleMuon and passTrigger) return 0; // Single Muon has precedence over  DoubleMuon. Assume this event is already been analyzed
        if (multipd_ and isDoubleMuon and fabs(mu0->Eta())<2.1 and fabs(mu1->Eta())<2.1){
                // no trigger matching here.Check dimuon trigge
                passTrigger = e->IsTriggered("HLT_DoubleIsoMu20_eta2p1_v");
        }

        if (passTriggerEvent and not passTrigger) Log(__FUNCTION__,"INFO","Fail to trigger event due to trigger matching");
        if (not passTriggerEvent and passTrigger) Log(__FUNCTION__,"ERROR","Event triggered by object but not globally");

        //if (SYNC_VERBOSE and not processingSyst_ and e->runNum()==297722 and e->lumiNum()==47 and e->eventNum()==12105464)
        //    Log(__FUNCTION__,"SYNC",Form("Trigger: passTrigger=%d passTrigger1=%d passTRigger2=%d",passTriggerEvent,passTrigger1,passTrigger2));
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
            if (el->Pt() >15 and 
                    (fabs(el->Eta()) <1.4442 or fabs(el->Eta())>1.566) and 
                    fabs(el->Eta())<2.5 and
                    mu0->DeltaR(el) >0.4 and mu1->DeltaR(el) >0.4
               ) passLeptonVeto=false; // FIXME 10 ?!?
        }
    }


    Lepton *el= e->GetElectron(0);
    if (mu0 != NULL and passTrigger and mu0->Pt() >29 and el != NULL and el->Pt() > 20)  // singleMuon trigger
    {
        Object Z; 
        Z.SetP4(zero); 
        Z += *mu0;
        Z += *el;
        double mass=Z.M();
        if (jetVar_["maxDeepB"]>-0.5 and mass>70 and mass <110)
        {
            int fl = fabs(selectedJets[ jetVar_["maxDeepBIdx"]]->hadFlavor());
            string flStr =  (fl==5) ? "B" : (fl==4) ? "C": "L";
            Fill("HmumuAnalysis/Vars/DeepBOnZ_emu_"+flStr+"_"+ label,systname, jetVar_["maxDeepB"],e->weight()/btagsf);
        }
    }

    if (mu0 != NULL and passTrigger and mu0->Pt() >29 and el != NULL and el->Pt() > 20)  // singleMuon trigger
    { // emu selection 
        Object Hcache;Hcache.SetP4(Hmm.GetP4());//saveit
        Object Z; 
        Z.SetP4(zero); 
        Z += *mu0;
        Z += *el;
        Hmm.SetP4(Z.GetP4());
        updateMjj(); // --> KF is defined with  e m
        if (jetVar_["mbbkf2"] > 110 and jetVar_["mbbkf2"]< 140) 
            Fill("HmumuAnalysis/Vars/Mem_KF2_HbbHmm_"+ label,systname, Z.M(),e->weight());
        if (jetVar_["mbbkf3"] > 110 and jetVar_["mbbkf3"]< 140) 
            Fill("HmumuAnalysis/Vars/Mem_KF3_HbbHmm_"+ label,systname, Z.M(),e->weight());
        Hmm.SetP4(Hcache.GetP4());
        updateMjj(); // --> KF is defined again with em
    }


    // -- FINAL SELECTION --
    FillSyncTree(label, systname ,category);
    //if ( recoMuons and passTrigger and passAsymmPtCuts and passLeptonVeto)
    if ( recoMuons and passTrigger and passAsymmPtCuts)
    {
        if (VERBOSE)Log(__FUNCTION__,"DEBUG","event pass selection");

        if (mass_ >= 70 and mass_<110){ // no zptrwgt
            if (selectedJets.size()==0)Fill("HmumuAnalysis/Vars/PtOnZ0j_"+ label,systname, pt_ ,e->weight());
            if (selectedJets.size()==1)Fill("HmumuAnalysis/Vars/PtOnZ1j_"+ label,systname, pt_ ,e->weight());
            if (selectedJets.size()>=2)Fill("HmumuAnalysis/Vars/PtOnZ2j_"+ label,systname, pt_ ,e->weight());
        }

        float zptrw = 1.;
        if (label == "DY" or label == "DYJetsToLL_M-105To160") 
        {
            zptrw = rzpt_.getZPtReweight(pt_ ,year,std::min(int(selectedJets.size()),int(2)));
            //if (year == 2016 and label != "DYJetsToLL_M-105To160" )zptrw = getZPtReweight(pt_ );
            //else zptrw = getZPtReweight2018(pt_ );
        }
        //cout<<"ZPtRWgh is "<<zptrw<<endl;
        // generally apply z-pt reweight
        
        {
            SF* sf= e->GetWeight()->GetSF("dummy"); // base, I modify this one
            sf->syst=0;
            sf->sf= zptrw; 
            sf->err = 0.0;
            e->ApplySF("dummy");
        }

        Fill("HmumuAnalysis/Vars/NpvAll_"+ label,systname, e->Npv() ,e->weight());
        //Fill("HmumuAnalysis/Vars/NpvAll_norwgt_"+ label,systname, e->Npv() ,e->weight()/nvtxrwgt);
        
	    //Book2D ("HmumuAnalysis/Vars/PtVsMassOnZ_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 800,60,200,200,0,200);
        //Fill2D("HmumuAnalysis/Vars/PtVsMassOnZ_"+label,systname,mass_,pt_,e->weight());

        if (mass_ >= 70 and mass_<110){
            Fill("HmumuAnalysis/Vars/MetOnZ_"+ label,systname, e->GetMet().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/RawMetOnZ_"+ label,systname, e->GetMet().GetRawMetP4().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/TrackMetOnZ_"+ label,systname, e->GetMet().GetTrackMetP4().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/MetOnZ_rw_"+ label,systname, e->GetMet().Pt(),e->weight()*zptrw);
            Fill("HmumuAnalysis/Vars/PtOnZ_"+ label,systname, pt_ ,e->weight());
            Fill("HmumuAnalysis/Vars/NpvOnZ_"+ label,systname, e->Npv() ,e->weight());
            //Fill("HmumuAnalysis/Vars/NpvOnZ_norwgt_"+ label,systname, e->Npv() ,e->weight()/nvtxrwgt);
            Fill("HmumuAnalysis/Vars/RhoOnZ_"+ label,systname, e->Rho() ,e->weight());

            if (jetVar_["maxDeepB"]>-0.5 )
            {
                int fl = fabs(selectedJets[ jetVar_["maxDeepBIdx"]]->hadFlavor());
                string flStr =  (fl==5) ? "B" : (fl==4) ? "C": "L";
                Fill("HmumuAnalysis/Vars/DeepBOnZ_"+flStr+"_"+ label,systname, jetVar_["maxDeepB"],e->weight()/btagsf);
            }

            if(catType>=2)Fill("HmumuAnalysis/Vars/BdtOnZ_"+ label,systname, bdt[0] ,e->weight());

            if (catType>=2 and bdt.size() >1)
            {
                if (selectedJets.size()<2) {
                    Fill("HmumuAnalysis/Vars/Bdt01jUCSDOnZ_"+label,systname,bdt[1],e->weight());
                    Fill("HmumuAnalysis/Vars/Bdt01jUCSDOnZ_nopuidsf_"+label,systname,bdt[1],e->weight()/puidsf);
                }
                //else if (jetVar_["mjj_lead"]<400 and jetVar_["nbjets"]==0 ) {
                //    Fill("HmumuAnalysis/Vars/Bdt2jUCSDOnZ_"+label,systname,bdt[2],e->weight());
                //    Fill("HmumuAnalysis/Vars/Bdt2jUCSDOnZ_nopuidsf_"+label,systname,bdt[2],e->weight()/puidsf);
                //}
            }

            if (doScikit and catType==2){
                for(size_t i=0;i<discr.size();++i)
                {
                    const auto& s = discr[i];
                    Fill("HmumuAnalysis/Vars/"+s+"OnZ_"+ label,systname, scikit[i] ,e->weight());
                }
            }
        }

        if (mass_ >= 110 and mass_<150){

            if (not isMiniIsoLeptons) {
                Fill("HmumuAnalysis/Vars/MuonIso_"+ label,systname, mu0->Isolation(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonPt_"+ label,systname, mu0->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonEta_"+ label,systname, mu0->Eta(),e->weight());

                Fill("HmumuAnalysis/Vars/MuonIso_"+ label,systname, mu1->Isolation(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonPt_"+ label,systname, mu1->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonEta_"+ label,systname, mu1->Eta(),e->weight());
            }
            else{
                Fill("HmumuAnalysis/Vars/MuonMiniIso_"+ label,systname, mu0->MiniIsolation(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonMiniIso_Pt_"+ label,systname, mu0->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonMiniIso_Eta_"+ label,systname, mu0->Eta(),e->weight());

                Fill("HmumuAnalysis/Vars/MuonMiniIso_"+ label,systname, mu1->MiniIsolation(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonMiniIso_Pt_"+ label,systname, mu1->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/MuonMiniIso_Eta_"+ label,systname, mu1->Eta(),e->weight());
            }

            Fill("HmumuAnalysis/Vars/MetOnH_"+ label,systname, e->GetMet().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/RawMetOnH_"+ label,systname, e->GetMet().GetRawMetP4().Pt(),e->weight());
            Fill("HmumuAnalysis/Vars/TrackMetOnH_"+ label,systname, e->GetMet().GetTrackMetP4().Pt(),e->weight());

            Fill("HmumuAnalysis/Vars/Mt1OnH_"+ label,systname,ChargedHiggs::mt(mu0->Pt(),e->GetMet().Pt(),mu0->Phi(),e->GetMet().Phi()) ,e->weight());
            Fill("HmumuAnalysis/Vars/Mt2OnH_"+ label,systname,ChargedHiggs::mt(mu1->Pt(),e->GetMet().Pt(),mu1->Phi(),e->GetMet().Phi()) ,e->weight());
            Fill("HmumuAnalysis/Vars/MetOnH_rw_"+ label,systname, e->GetMet().Pt(),e->weight()*zptrw);
            Fill("HmumuAnalysis/Vars/PtOnH_"+ label,systname, pt_,e->weight());
            Fill("HmumuAnalysis/Vars/PtOnH_zptrwg_"+ label,systname, pt_,e->weight() *zptrw);
            Fill("HmumuAnalysis/Vars/PtOnH_no_btagsf_"+ label,systname, pt_,e->weight()/btagsf);
            if (nnlops >0)Fill("HmumuAnalysis/Vars/PtOnH_no_nnlops_"+ label,systname, pt_,e->weight()/nnlops);
            Fill("HmumuAnalysis/Vars/Mjj1OnH_" + label,systname, jetVar_["mjj_1"],e->weight() ) ;
            Fill("HmumuAnalysis/Vars/DeltaEtaJJ1OnH_" + label,systname, jetVar_["detajj_1"],e->weight() ) ;
            Fill("HmumuAnalysis/Vars/NpvOnH_"+ label,systname, e->Npv() ,e->weight());
            //Fill("HmumuAnalysis/Vars/NpvOnH_norwgt_"+ label,systname, e->Npv() ,e->weight()/nvtxrwgt);
            Fill("HmumuAnalysis/Vars/RhoOnH_"+ label,systname, e->Rho() ,e->weight());

            Fill("HmumuAnalysis/Vars/RapOnH_"+ label , systname, Hmm.Rapidity(),e->weight());
            //Fill("HmumuAnalysis/Vars/CosThetaCSOnH_"+ label , systname, ChargedHiggs::CosThetaCS(&mu0->GetP4(),&mu1->GetP4()),e->weight());
            Fill("HmumuAnalysis/Vars/CosThetaCSOnH_"+ label , systname, ChargedHiggs::CosThetaCSPos(mu0,mu1),e->weight());
            Fill("HmumuAnalysis/Vars/PhiCSOnH_"+ label , systname, ChargedHiggs::PhiCSPos(mu0,mu1),e->weight());

            float drmj=10;
            for( auto j : selectedJets)
            {
                drmj = min(drmj, float(j->DeltaR(mu0)));
                drmj = min(drmj, float(j->DeltaR(mu1)));
            }
            Fill("HmumuAnalysis/Vars/MinDrMuJOnH_"+ label ,systname,drmj ,e->weight());
            if (selectedJets.size() >=2){ 
                Fill("HmumuAnalysis/Vars/PtOnH_2jet_"+ label , systname, Hmm.Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/CosThetaCSOnH_2jet_"+ label , systname, ChargedHiggs::CosThetaCSPos(mu0,mu1), e->weight());
                Fill("HmumuAnalysis/Vars/PhiCSOnH_2jet_"+ label , systname, ChargedHiggs::PhiCSPos(mu0,mu1), e->weight() );
                Fill("HmumuAnalysis/Vars/MinDrMuJOnH_2jet_"+ label ,systname, drmj, e->weight() );
            }
            //#warning MetNoJec Applied only to plots
            //e->ApplyMetNoJEC();
            
            Fill("HmumuAnalysis/Vars/MetOnH_nojec_"+ label,systname, e->GetMet().Pt(),e->weight());

            //if (mjj.size()> 0)
            //{
            //    Fill("HmumuAnalysis/Vars/SoftNJetsOnH_" + label,systname, jetVar_["softNjets1"],e->weight() ) ;
            //    Fill("HmumuAnalysis/Vars/SoftHtOnH_" + label,systname, jetVar_["softHt1"],e->weight() ) ;
            //}

            if (selectedJets.size() >0)
            {
                Fill("HmumuAnalysis/Vars/DeepBOnH_"+ label,systname, jetVar_["maxDeepB"],e->weight());
                Fill("HmumuAnalysis/Vars/DeepBOnH_no_btagsf_"+ label,systname, jetVar_["maxDeepB"],e->weight()/btagsf);
                Fill("HmumuAnalysis/Vars/MaxCSVOnH_"+ label,systname, jetVar_["maxCSV"],e->weight());
                Fill("HmumuAnalysis/Vars/PtJet1OnH_"+ label,systname, selectedJets[0]->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/EtaJet1OnH_"+ label,systname, selectedJets[0]->Eta(),e->weight());
                Fill("HmumuAnalysis/Vars/QGLJet1OnH_"+ label,systname, selectedJets[0]->QGL(),e->weight());

                if ( 2.65<fabs(selectedJets[0]->Eta()) and fabs(selectedJets[0]->Eta())<3.139)
                {// study of jet quality in 2.65-3.139 for 2017
                    float ptraw=selectedJets[0]->rawPt;
                    std::vector<int> ptbins{0,30,50,80,120,8000};
                    string ptbin="";
                    for (int ipt=0;ipt<ptbins.size()-1;++ipt)
                    {
                        if (ptraw >= ptbins[ipt] and ptraw <ptbins[ipt+1])
                        {
                            ptbin=Form("pt%d_%d",ptbins[ipt],ptbins[ipt+1]);
                        }
                    }
                    
                    if (ptbin != "")
                    {
                    //cout<<"DEBUG: -------------------------------" <<endl; 
                    //cout<<"DEBUG: A1: "<<-TMath::Log(std::max(selectedJets[0]->QGLVar("axis1"),.00007F))<<endl; // ~9.5
                    //cout<<"DEBUG: A2: "<<-TMath::Log(std::max(selectedJets[0]->QGLVar("axis2"),.00007F))<<endl;
                    //cout<<"DEBUG: ptD: "<< selectedJets[0]->QGLVar("ptD")<<endl;
                    //cout<<"DEBUG: mult: "<<selectedJets[0]->QGLVar("mult")<<endl;
                    //cout<<"DEBUG: ptraw: "<<selectedJets[0]->rawPt<<endl;
                    //cout<<"DEBUG: pt: "<<selectedJets[0]->Pt()<<endl;
                    for (const string&v : {"axis1","axis2","ptD","mult","cemf","nemf","ptrawnemf"})
                        {
                            float value=-999;
                            if ( v=="axis1" or v=="axis2"){ value=-TMath::Log(std::max(selectedJets[0]->QGLVar(v),.00007F)) ;}
                            else if (v=="cemf"){value=selectedJets[0]->GetCEMF();}
                            else if (v=="nemf"){value=selectedJets[0]->GetNEMF();}
                            else if (v=="ptrawnemf"){ value=selectedJets[0]->GetNEMF()*ptraw;}
                            else value=selectedJets[0]->QGLVar(v);
                            Fill ("HmumuAnalysis/Vars/"+v+"_Jet1OnH_Eta2p65_2p139_"+ptbin+"_"+ label,systname , value,e->weight());
                        }
                    }
                }//end study on jet quality in eta ...
            }//end jet1

            if (selectedJets.size() >1)
            {
                Fill("HmumuAnalysis/Vars/PtJet2OnH_"+ label,systname, selectedJets[1]->Pt(),e->weight());
                Fill("HmumuAnalysis/Vars/EtaJet2OnH_"+ label,systname, selectedJets[1]->Eta(),e->weight());
            }

            Fill("HmumuAnalysis/Vars/NJetsOnH_"+ label,systname, selectedJets.size(),e->weight());
            Fill("HmumuAnalysis/Vars/NBJetsOnH_"+ label,systname, e->Bjets(),e->weight());
            Fill("HmumuAnalysis/Vars/CosThetaCSOnH_"+ label,systname,  ChargedHiggs::CosThetaCSPos(mu0,mu1),e->weight());
            
            if (catType>=2 and bdt.size() >1)
            {
                if (selectedJets.size()<2){
                    Fill("HmumuAnalysis/Vars/Bdt01jUCSDOnH_"+label,systname,bdt[1],e->weight());
                    Fill("HmumuAnalysis/Vars/Bdt01jUCSDOnH_nopuidsf_"+label,systname,bdt[1],e->weight()/puidsf);
                }
                //else if (jetVar_["mjj_lead"]<400 and jetVar_["nbjets"]==0 ) {
                //    Fill("HmumuAnalysis/Vars/Bdt2jUCSDOnH_"+label,systname,bdt[2],e->weight());
                //    Fill("HmumuAnalysis/Vars/Bdt2jUCSDOnH_nopuidsf_"+label,systname,bdt[2],e->weight()/puidsf);
                //}
            }

            if(catType>=2 and bdt.size() >0 )Fill("HmumuAnalysis/Vars/BdtOnH_"+ label,systname, bdt[0] ,e->weight());
            if(catType>=2 and bdt.size() >0 )Fill("HmumuAnalysis/Vars/BdtOnH_NoPrefire_"+ label,systname, bdt[0] ,e->weight()/l1prefire);
            if(catType>=2 and bdt.size() >0 and e->GetL1FinalOr(-1))Fill("HmumuAnalysis/Vars/BdtOnH_Prefire_"+ label,systname, bdt[0] ,e->weight());
            if(catType>=2 and bdt.size() >0 )Fill("HmumuAnalysis/Vars/BdtOnH_zptrwg_"+ label,systname, bdt[0] ,e->weight()*zptrw);
            if(catType>=2 and bdt.size() >0 and fabs(mu0->Eta())<0.8 and fabs(mu1->Eta())<0.8)
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

            //if (catType == 3 and category == "cat18") // WHLep
            if (catType == 5 and category == "cat13") // WHLep
            {
                Fill("HmumuAnalysis/Vars/MtOnH_WLep_"+ label,systname, mt,e->weight()) ;
            }

            if (catType==5 and category == "cat14" ) // VHHadr
            {
                Jet *j1=selectedJetsVHHadr[0];
                Jet *j2=selectedJetsVHHadr[1];
                TLorentzVector jj=j1->GetP4() + j2->GetP4();
                Fill("HmumuAnalysis/Vars/DPhijj_VHHadr_"+ label,systname, j1->DeltaPhi(j2),e->weight()) ;
                Fill("HmumuAnalysis/Vars/DEtajj_VHHadr_"+ label,systname, j1->DeltaEta(j2),e->weight()) ;
                Fill("HmumuAnalysis/Vars/CosThetaCSmm_VHHadr_"+ label,systname, ChargedHiggs::CosThetaCSPos(mu0,mu1),e->weight()) ;
                Fill("HmumuAnalysis/Vars/CosThetaCS_VHHadr_"+ label,systname, ChargedHiggs::CosThetaCS(&j1->GetP4(),&j2->GetP4(),13),e->weight()) ;
                Fill("HmumuAnalysis/Vars/NJets_VHHadr_"+ label,systname, selectedJets.size(),e->weight()) ;
                Fill("HmumuAnalysis/Vars/Ptjj_VHHadr_"+ label,systname, jj.Pt(),e->weight()) ;

                Fill("HmumuAnalysis/Vars/DPhimmjj_VHHadr_"+ label,systname, jj.DeltaPhi(Hmm.GetP4()),e->weight()) ;
                Fill("HmumuAnalysis/Vars/DEtammjj_VHHadr_"+ label,systname, fabs(jj.Eta()-Hmm.Eta()),e->weight()) ;

                // KF
                Fill("HmumuAnalysis/Vars/Mjjkf_VHHadr_"+ label,systname, jetVar_["mjjkf"],e->weight()) ;
                Fill("HmumuAnalysis/Vars/Malljkf_VHHadr_"+ label,systname, jetVar_["malljkf"],e->weight()) ;
                Fill("HmumuAnalysis/Vars/Metkf_VHHadr_"+ label,systname, jetVar_["metkf"],e->weight()) ;
                //Fill("HmumuAnalysis/Vars/Valuekf_VHHadr_"+ label,systname, jetVar_["valuekf"],e->weight()) ;

            }

            //if (catType == 3 and category == "cat19") // ggHX
            if (catType == 5 and category == "cat15") //
            {
                Fill("HmumuAnalysis/Vars/NJets_ggHX_"+ label,systname, selectedJets.size(),e->weight()) ;
                Fill("HmumuAnalysis/Vars/DeltaR_ggHX_"+ label,systname,mu0->DeltaR(mu1),e->weight()) ;
            }
        }

        float met = e->GetMet().Pt();
        if (met < 60)
        {
            if (mass_>60 and category != "")
            {

                Fill("HmumuAnalysis/Vars/Mbb_HbbHmm_"+ label,systname, jetVar_["mbb"],e->weight());
                Fill("HmumuAnalysis/Vars/Mbbcorr_HbbHmm_"+ label,systname, jetVar_["mbbcorr"],e->weight());
                Fill("HmumuAnalysis/Vars/Mbbkf2_HbbHmm_"+ label,systname, jetVar_["mbbkf2"],e->weight());
                Fill("HmumuAnalysis/Vars/Mbbkf3_HbbHmm_"+ label,systname, jetVar_["mbbkf3"],e->weight());

                if (jetVar_["mbbcorr"] >10.)
                {
                    Fill("HmumuAnalysis/Vars/Bres_HbbHmm_"+ label,systname, jetVar_["bb_b1_res"],e->weight());
                    Fill("HmumuAnalysis/Vars/Bres_HbbHmm_"+ label,systname, jetVar_["bb_b2_res"],e->weight());
                    Fill("HmumuAnalysis/Vars/Jres_HbbHmm_"+ label,systname, jetVar_["bb_j1_res"],e->weight());
                    Fill("HmumuAnalysis/Vars/Jres_HbbHmm_"+ label,systname, jetVar_["bb_j2_res"],e->weight());
                }

                if (jetVar_["mbbcorr"] > 110 and jetVar_["mbbcorr"]< 140) 
                {
                    Fill("HmumuAnalysis/Vars/Mmm_HbbHmm_"+ label,systname, mass_,e->weight());
                    if (not processingSyst_  and jetVar_["dphibbmm"]> 3.1415-0.5)
                        Fill2D("HmumuAnalysis/Vars/Mmm_HbbHmm_2D_"+label,systname,mass_,jetVar_["mbbmmcorr"],e->weight());
                }
                if (jetVar_["mbbkf2"] > 110 and jetVar_["mbbkf2"]< 140) 
                    Fill("HmumuAnalysis/Vars/Mmm_KF2_HbbHmm_"+ label,systname, mass_,e->weight());
                if (jetVar_["mbbkf3"] > 110 and jetVar_["mbbkf3"]< 140) 
                    Fill("HmumuAnalysis/Vars/Mmm_KF3_HbbHmm_"+ label,systname, mass_,e->weight());
            }

        }
        

        Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, mass_,e->weight()) ;
        if( category != "")
        {
            Fill("HmumuAnalysis/Vars/Mmm_"+ category+"_"+ label,systname, mass_,e->weight()) ;
            if (doSTXS and labelSTXS!="" and (
                        label == "GluGlu_HToMuMu_M125" or 
                        label =="VBF_HToMuMu_M125" or 
                        label =="WPlusH_HToMuMu_M125" or
                        label =="WMinusH_HToMuMu_M125" or 
                        label =="ZH_HToMuMu_M125" or
                        label =="ttH_HToMuMu_M125")){
                Fill("HmumuAnalysis/Vars/Mmm_"+ category + "_" + label+ "_STXS_125_"+labelSTXS,systname,mass_,e->weight());
            }


            if (not processingSyst_ and catType>=2){ // don't need syst for this
                if (selectedJets.size() <2)Fill2D("HmumuAnalysis/Vars/BdtMassFull_UCSD1_"+ label,systname,mass_,bdt[1],e->weight());
                //else Fill2D("HmumuAnalysis/Vars/BdtMassFull_UCSD2_"+ label,systname,mass_,bdt[2],e->weight());
            }

            if (not isExclusiveCat and not processingSyst_ and catType>=2){ // don't need syst for this
                //bool isBB= fabs(mu0->Eta()) < 0.9 and fabs(mu1->Eta())<0.9;
                //bool isOO= fabs(mu0->Eta()) < 1.9 and fabs(mu1->Eta())<1.9;
                
                //Fill2D("HmumuAnalysis/Vars/BdtMass_UF_"+ label,systname,mass_,bdt[0],e->weight());
                if (selectedJets.size() <2)Fill2D("HmumuAnalysis/Vars/BdtMass_UCSD1_"+ label,systname,mass_,bdt[1],e->weight());
                //else Fill2D("HmumuAnalysis/Vars/BdtMass_UCSD2_"+ label,systname,mass_,bdt[2],e->weight());

                if (jetVar_["mjj_lead"]<400)
                {
                    if (selectedJets.size() <2)Fill2D("HmumuAnalysis/Vars/BdtMassNonVbf_UCSD1_"+ label,systname,mass_,bdt[1],e->weight());
                    //else Fill2D("HmumuAnalysis/Vars/BdtMassNonVbf_UCSD2_"+ label,systname,mass_,bdt[2],e->weight());
                }
            }
            
            // for runI category the variable is set in Category 
            if (catType >1 and (not processingSyst_))
            {
                int cat=0;
                if (category=="cat0") cat=0;
                else if (category=="cat1") cat=1;
                else if (category=="cat2") cat=2;
                else if (category=="cat3") cat=3;
                else if (category=="cat4") cat=4;
                else if (category=="cat5") cat=5;
                else if (category=="cat6") cat=6;
                else if (category=="cat7") cat=7;
                else if (category=="cat8") cat=8;
                else if (category=="cat9") cat=9;
                else if (category=="cat10") cat=10;
                else if (category=="cat11") cat=11;
                else if (category=="cat12") cat=12;
                else if (category=="cat13") cat=13;
                else if (category=="cat14") cat=14;
                else if (category=="cat15") cat=15;
                else if (category=="cat16") cat=16;
                else if (category=="cat17") cat=17;
                else if (category=="cat18") cat=18;
                else if (category=="cat19") cat=19;
                else cat=20;
                SetTreeVar("cat",cat);
            }



            if (doUnbinned and mass_>110 and mass_<150 and (not processingSyst_) and e->IsRealData())
            {
                SetTreeVar("mmm",mass_);
                FillTree("final");
            }


        } //Unblind and category != ""

    } // reco pass trigger and pass asymm cuts


    if (VERBOSE)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}

void HmumuAnalysis::EndEvent(){
    if (doSync and e!=NULL and e->eventNum() == eventNum and e->runNum() == runNum and e->lumiNum() == lumiNum and fillTree)
    {
        FillTree("hmm");
    }
}

void HmumuAnalysis::updateMjj(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","updateMjj: jet var ");
    mjj.clear();
    for(unsigned i=0;i<selectedJets.size() ;++i)
        for (unsigned j=i+1;j<selectedJets.size();++j)
        {
            mjj.push_back( pair<float,pair<int,int> >(selectedJets[i]->InvMass( selectedJets[j] ), pair<int,int>(i,j) ) );
        }
    sort ( mjj.begin(),mjj.end(), [](  const pair<float,pair<int,int> >&x,const pair<float, pair<int,int> >&y){if (x.first > y.first) return true;if (x.first<y.first) return false; if  (x.second.first < y.second.first) return true; if (x.second.first > y.second.first) return false; return x.second.second < y.second.second; } );

    // update jet variables 
    jetVar_.clear();
    jetVar_["maxCSV"]=-1.;
    jetVar_["aveCSV"]=0.;
    jetVar_["ncentjets"]=0;
    jetVar_["leadCSV"] = -1;
    jetVar_["aveQGLcent"]=0.;
    jetVar_["htCent"]=0.;
    jetVar_["nfwdjets"]=0.;
    jetVar_["nbjets"]=0.;

    vector<float> csv_score;
    vector<pair<float,int> > deepb_score;
    vector<pair<float,float>> qgl_score;

    jetVar_["mjj_lead"]= (selectedJets.size()>=2)? ( selectedJets[0]->InvMass( selectedJets[1] )): 0;
    jetVar_["detajj_lead"]= (selectedJets.size()>=2)? fabs(selectedJets[0]->Eta()-selectedJets[1]->Eta()): 0;
    jetVar_["dphijj_lead"]= (selectedJets.size()>=2)? fabs(selectedJets[0]->DeltaPhi(selectedJets[1])): 0;

    //jetVar_["y*"] = (selectedJets.size()>=2) ? (Hmm.Eta() - (selectedJets[0]->Eta() + selectedJets[1]->Eta())/2.0 ) :-10;
    //jetVar_["z*"] = (selectedJets.size()>=2) ? (jetVar_["y*"] /fabs(selectedJets[0]->Eta() - selectedJets[1]->Eta()) ):-10;

    jetVar_["y*"] = (selectedJets.size()>=2) ? ( - Hmm.Rapidity() + (selectedJets[0]->Rapidity() + selectedJets[1]->Rapidity())/2.0 ) :-10;
    jetVar_["z*"] = (selectedJets.size()>=2) ? (jetVar_["y*"] /fabs(selectedJets[0]->Rapidity() - selectedJets[1]->Rapidity()) ):-10;


    for(unsigned i=0;i<selectedJets.size() ;++i)
    {
        if ( abs(selectedJets[i]->Eta())<=2.4)
        {
            jetVar_["ncentjets"]+=1;
            csv_score.push_back(selectedJets[i]->Btag());
            deepb_score.push_back(pair<float,int>(selectedJets[i]->GetDeepB(),i));
            qgl_score.push_back(pair<float,float>(selectedJets[i]->Pt(),selectedJets[i]->QGL()));
            //jetVar_["aveCSV"] += selectedJets[i]->Btag()*selectedJets[i]->Pt(); // pt-weighte
            jetVar_["aveQGLcent"] += selectedJets[i]->QGL(); // not pt weighted
            jetVar_["htCent"] += selectedJets[i]->Pt();
            if (selectedJets[i]->GetDeepB() > DEEP_B_MEDIUM) jetVar_["nbjets"]+=1;
        }
        else
        {
            jetVar_["nfwdjets"] +=1;
        }
    }
    if (jetVar_["ncentjets"]>0) jetVar_["aveQGLcent"] /= jetVar_["ncentjets"];
    //if (jetVar_["htCent"]>0) jetVar_["aveCSV"] /= jetVar_["htCent"];
    jetVar_["leadCSV"] =  (selectedJets.size()>0) ?selectedJets[0]->Btag():-1;
    jetVar_["leadDeepB"] =  (selectedJets.size()>0) ?selectedJets[0]->GetDeepB():-1;

    sort(csv_score.begin(),csv_score.end(),std::greater<float>());
    jetVar_["maxCSV"]=(csv_score.size()>0)?csv_score[0]:-1;
    //jetVar_["secondCSV"] = (csv_score.size()>1)?csv_score[1]:-1;
    //jetVar_["thirdCSV"] = (csv_score.size()>2)?csv_score[2]:-1;

    sort(deepb_score.begin(),deepb_score.end(),std::greater<std::pair<float,int> >());
    jetVar_["maxDeepB"]=(deepb_score.size()>0)?deepb_score[0].first:-1;
    jetVar_["maxDeepBIdx"]=(deepb_score.size()>0)?deepb_score[0].second:-1;

    sort(qgl_score.begin(),qgl_score.end(),[](const pair<float,float >&x,const pair<float, float >&y){ if (x.first> y.first) return true; if(x.first<y.first)return false; return x.second> y.second; } );
    jetVar_["firstQGL"]  =  (qgl_score.size() >0) ? qgl_score[0].second : -1 ;
    jetVar_["secondQGL"] = (qgl_score.size() >1) ? qgl_score[1].second : -1 ;
    jetVar_["thirdQGL"]  =  (qgl_score.size() >2) ? qgl_score[2].second : -1 ;
    
    jetVar_["mjj_1"] = (mjj.size() >0 ) ? mjj[0].first: 0.0;
    jetVar_["mjj_2"] = (mjj.size() > 1) ? mjj[1].first: 0.0;
    jetVar_["detajj_1"] = (mjj.size()>0) ? fabs(selectedJets[mjj[0].second.first]->Eta() - selectedJets[mjj[0].second.second]->Eta()): -1.0;
    jetVar_["detajj_2"] = (mjj.size()>1) ? fabs(selectedJets[mjj[1].second.first]->Eta() - selectedJets[mjj[1].second.second]->Eta()): -1.0;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","updateMjj: jet var (b-jets) ");
    // double b
    int nbjets=0;
    Jet *b1=NULL,*b2=NULL;
    int idx_b1=-1,idx_b2=-1;
    for(unsigned i=0;i<selectedJets.size() ;++i)
    {
        Jet *j=selectedJets[i];
        if (abs(j->Eta())>2.4 ) continue;
        if (j->GetDeepB() > DEEP_B_MEDIUM) nbjets+=1;
        if (b1 ==NULL) {b1=j;idx_b1=i;}
        else if (b2==NULL) {b2=j;idx_b2=i;}
    }


    kf -> clear();

    jetVar_["mbb"] = 0.;
    jetVar_["mbbcorr"] = 0.;
    jetVar_["mbbkf2"] = 0.;
    jetVar_["mbbkf3"] = 0.;

    if (nbjets >=2 and mu0 and mu1)
    {
        TLorentzVector BB(0,0,0,0);
        BB += b1->GetP4() ;
        BB += b2->GetP4() ;
        jetVar_["mbb"]=BB.M();

        TLorentzVector BBCorr(0,0,0,0);
        BBCorr += (b1->GetP4()*b1->GetBCorrection());
        BBCorr += (b2->GetP4()*b2->GetBCorrection());

        jetVar_["mbbcorr"]=BBCorr.M();
        TLorentzVector BBMM=BBCorr + Hmm.GetP4();
        jetVar_["mbbmmcorr"]=BBMM.M();
        jetVar_["dphibbmm"]=BB.DeltaPhi(Hmm.GetP4());

        class MyJet{
            public:
               TLorentzVector p4; 
               float sigma; //resolution estimator
               int isB{0}; // not used

               float var() {return sigma*sigma;}
        }; 


        idx_b1=-1; //this refers to myJets
        idx_b2=-1;
        vector<MyJet> myJets;
        for (auto j : selectedJets) 
        {
            MyJet aJet;
            aJet.p4 = j->GetP4();
            //aJet.sigma = j->GetJESUnc();
            JME::JetParameters bins,vars;
            vars.setJetPt( j->GetP4().Pt() );
            bins.setJetEta( j->GetP4().Eta() );
            bins.setRho( std::min(e->Rho(),float(40.)) ); // corrections up to rho 40.
            
            //cout <<"DEBUG: Getting record for bins:"<<j->GetP4().Eta()<<":"<<e->Rho()<<endl;
            //cout<< "IS NULL? "<<jet_resolution->getRecord(bins) <<endl;
            float sigma=1.0 ;

            if ( jet_resolution->getRecord(bins)==NULL)
                { 
                    sigma=1.0;
                }
            else
                { 
                sigma =  jet_resolution->evaluateFormula( *jet_resolution->getRecord(bins),vars); 
                }
            
            if (j->IsBJet() and fabs(j->Eta())<2.4) {
                aJet.p4 *= j->GetBCorrection();
                aJet.isB=1;
                sigma = j->GetBCorrectionUncertainty() / 1.1775; // I assume they give the HWHM
            }

            aJet.sigma=sigma;
            myJets.push_back(aJet);
            if (j==b1) idx_b1=myJets.size()-1;
            if (j==b2) idx_b2=myJets.size()-1;

            // mv through kf
            kf->p4.push_back( j->GetP4());
            kf->sigma.push_back(aJet.sigma);
        }

        kf->H=Hmm.GetP4();
        kf->doGhost=false; // if I turn them on, they are added to the p4 list while running (not twice) but cannot switch them off until clear
        // kf->run() sholud be the same of kf2

        // Add Ghost? kf can switch them on easily
        if(false){
            MyJet a,b,c;
            a.p4.SetPtEtaPhiM(30,0,0,0);
            b.p4.SetPtEtaPhiM(30,0,+2./3*TMath::Pi(),0);
            c.p4.SetPtEtaPhiM(30,0,-2./3*TMath::Pi(),0);
            a.sigma = .3333; // c.a. 10 GeV
            b.sigma = .3333;
            c.sigma = .3333;
            myJets.push_back(a);
            myJets.push_back(b);
            myJets.push_back(c);
        }

        int N=myJets.size();
        TVectorD x(N+2),c(N+2);
        TMatrixD A(N+2,N+2);
        
        float pHx = Hmm.GetP4().Px();
        float pHy = Hmm.GetP4().Py();
        c(0) = -pHx;
        c(1) = -pHy;
        for(int i=0;i<N;++i) c(i+2)   = 2./(myJets[i].var());
        for(int i=0;i<N;++i) A(0,i+2) = myJets[i].p4.Px();
        for(int i=0;i<N;++i) A(1,i+2) = myJets[i].p4.Py();
        for(int i=0;i<N;++i) A(i+2,0) = -myJets[i].p4.Px();
        for(int i=0;i<N;++i) A(i+2,1) = -myJets[i].p4.Py();
        for(int i=0;i<N;++i) A(i+2,i+2)= 2./(myJets[i].var());
        
        A.Invert();
        x=A*c;

        TLorentzVector b1_kf2=(b1->GetP4()*b1->GetBCorrection());
        TLorentzVector b2_kf2=(b2->GetP4()*b2->GetBCorrection());
        b1_kf2 *= x(idx_b1+2);
        b2_kf2 *= x(idx_b2+2);
        TLorentzVector BB_KF2(0,0,0,0);
        BB_KF2+=b1_kf2;
        BB_KF2+=b2_kf2;
        jetVar_["mbbkf2"] = BB_KF2.M();

        TLorentzVector Closure(0,0,0,0);
        Closure+=Hmm.GetP4();
        for(int i=0;i<myJets.size();++i)
        {
            TLorentzVector j = myJets[i].p4;
            j*=x(i+2);
            Closure += j;
        }
        kf->runGeneric();

        TLorentzVector b1_kf3=(b1->GetP4()*b1->GetBCorrection());
        TLorentzVector b2_kf3=(b2->GetP4()*b2->GetBCorrection());
        b1_kf3 *= kf->alpha[idx_b1];
        b2_kf3 *= kf->alpha[idx_b2];
        TLorentzVector BB_KF3(0,0,0,0);
        BB_KF3+=b1_kf3;
        BB_KF3+=b2_kf3;
        jetVar_["mbbkf3"] = BB_KF3.M();
        
        // Check resolution
        GenJet *b1_gen=NULL,*b2_gen=NULL;
        for (int gidx=0;;++gidx)
        {
            auto j=e->GetGenJet(gidx) ;
            if (j==NULL) break;
            if (j->DeltaR(b1) <0.4) b1_gen=j;
            if (j->DeltaR(b2) <0.4) b2_gen=j;
        }
        GenParticle *b1_genp=NULL,*b2_genp=NULL;
        for(int gidx=0;;++gidx)
        {
            auto g = e->GetGenParticle(gidx);
            if (g==NULL) break;
            if ( abs(g->GetPdgId()) != 5) continue;
            if (g->DeltaR(b1) <0.4 ) b1_genp=g;
            if (g->DeltaR(b2) <0.4 ) b2_genp=g;
        }

        jetVar_["bb_b1_res"] = (b1_gen)? (b1->Pt()*b1->GetBCorrection()/b1_gen->Pt()-1.)/myJets[idx_b1].sigma : -10;
        jetVar_["bb_b2_res"] = (b2_gen)? (b2->Pt()*b2->GetBCorrection()/b2_gen->Pt()-1.)/myJets[idx_b2].sigma : -10;

        // Log(__FUNCTION__,"DEBUG","-----------------------------------------------");
        // Log(__FUNCTION__,"DEBUG",Form("bb_b1_res is %5.3f Reco = %.1f * %.3f GenJet=%.1f GenB=%.1f",jetVar_["bb_b1_res"],b1->Pt(),b1->GetBCorrection(),(b1_gen)?b1_gen->Pt():-10,(b1_genp)?b1_genp->Pt():-10 ));
        // Log(__FUNCTION__,"DEBUG",Form("bb_b2_res is %5.3f Reco = %.1f * %.3f GenJet=%.1f GenB=%.1f",jetVar_["bb_b2_res"],b2->Pt(),b2->GetBCorrection(),(b2_gen)?b2_gen->Pt():-10,(b2_genp)?b2_genp->Pt():-10 ));
        // Log(__FUNCTION__,"DEBUG","-----------------------------------------------");
        
        // first leading non-bjets
        jetVar_["bb_j1_res"] = -10;
        jetVar_["bb_j2_res"] = -10;
        int kjet=1;
        for(int i=0;i<selectedJets.size();++i) // not the ghosts
        {
            if (i==idx_b1) continue;
            if (i==idx_b2) continue;
            if (kjet>2) continue;
            for (int gidx=0;;++gidx)
            {
                auto j=e->GetGenJet(gidx) ;
                if (j==NULL) break;
                if (j->GetP4().DeltaR(myJets[i].p4) <0.4)
                {
                    jetVar_[Form("bb_j%d_res",kjet)] = (myJets[i].p4.Pt()/j->Pt()-1.) / myJets[i].sigma;
                    ++kjet;
                    break;
                }
            }
        }

        /*
        kf->doGhost=true;
        kf->run();
        
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        Log(__FUNCTION__,"DEBUG",Form("x(0) %f ==%f",x( 2),kf->alpha[0]));
        Log(__FUNCTION__,"DEBUG",Form("x(b1) %f ==%f",x(idx_b1+2),kf->alpha[idx_b1]));
        Log(__FUNCTION__,"DEBUG",Form("x(b2) %f ==%f",x(idx_b2+2),kf->alpha[idx_b2]));
        Log(__FUNCTION__,"DEBUG",Form("mbb=%f mbbcorr=%f mbbkf2=%f mbbkf3=%f",jetVar_["mbb"],jetVar_["mbbcorr"],jetVar_["mbbkf2"],jetVar_["mbbkf3"]));
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        */

        /* 
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        if (mu0 == NULL or mu1==NULL) Log(__FUNCTION__,"DEBUG","-> no Muons");
        Log(__FUNCTION__,"DEBUG",Form("mbbkf: mbb=%f njets=%d ptZ=%f phiZ=%f",jetVar_["mbb"],selectedJets.size(),recoil.Pt(),recoil.Phi()));
        Log(__FUNCTION__,"DEBUG",Form("mbbkf2: mbb=%f mbbkf2=%f alpha=%f beta=%f njets=%d ptZ=%f phiZ=%f ptBB=%f phiBB=%f",jetVar_["mbb"],jetVar_["mbbkf2"],x(idx_b1+2),x(idx_b2+2),selectedJets.size(),Hmm.Pt(),Hmm.Phi(),BB_KF2.Pt(),BB_KF2.Phi()));
        Log(__FUNCTION__,"DEBUG",Form("MET=%f",e->GetMet().Pt()));
        Log(__FUNCTION__,"DEBUG",Form("Closure2 =%f ==0",Closure.Pt()));
        for (int i=0;i<myJets.size();++i){
            auto& j= myJets[i];
            Log(__FUNCTION__,"DEBUG",Form("* Jet pt=%f eta=%f phi=%f unc=%f isB=%d pt_corr=%f alpha=%f",j.p4.Pt(),j.p4.Eta(),j.p4.Phi(),j.sigma,j.isB,j.p4.Pt()*x(i+2),x(i+2)));
        }
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        for (int idx=0;;++idx)
        {
            auto j=e->GetGenJet(idx) ;
            if (j==NULL) break;
            Log(__FUNCTION__,"DEBUG",Form("* GenJet pt=%f eta=%f phi=%f",j->Pt(),j->Eta(),j->Phi()));

        }
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        if (mu0 ) Log(__FUNCTION__,"DEBUG",Form("* mu0 pt=%f eta=%f phi=%f",mu0->Pt(),mu0->Eta(),mu0->Phi()));
        if (mu1 ) Log(__FUNCTION__,"DEBUG",Form("* mu1 pt=%f eta=%f phi=%f",mu1->Pt(),mu1->Eta(),mu1->Phi()));
        Log(__FUNCTION__,"DEBUG","------------------------------------------------");
        */
        
    }// at least 2 b

    jetVar_["malljkf"] = 0.;
    jetVar_["mjjkf"] = 0.;
    jetVar_["metkf"] = 0.;
    jetVar_["valuekf"] = 0.;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","updateMjj: jet var (2-jets) ");
    if (selectedJets.size()>=2 and mu0 and mu1)
    { // kinematic fit (version 2) for VH
        kf->clear();
        for (auto j : selectedJets) 
        {
            JME::JetParameters bins,vars;
            vars.setJetPt( j->GetP4().Pt() );
            bins.setJetEta( j->GetP4().Eta() );
            bins.setRho( std::min(e->Rho(),float(40.)) ); // corrections up to rho 40.
            
            float sigma=1.0 ;

            if ( jet_resolution->getRecord(bins)==NULL){ sigma=1.0;}
            else{ sigma =  jet_resolution->evaluateFormula( *jet_resolution->getRecord(bins),vars); }

            kf->p4.push_back( j->GetP4());
            kf->sigma.push_back(sigma);
        }
        if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("updateMjj: jet var (2-jets) p2 jet size=%d",selectedJets.size()));

        kf->H=Hmm.GetP4();
        kf->runGeneric();
        
        TLorentzVector jall; // mjjkf metkf malljkf
        TLorentzVector jj; // mjjkf metkf malljkf
        if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("updateMjj: (2-j) p3"));
        for(unsigned i =0 ; i<selectedJets.size();++i) 
        {
            if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("updateMjj: (2-j) -- considering jets (%d) kfsize=%u",i,kf->alpha.size()));
            TLorentzVector p4= selectedJets[i]->GetP4();
            p4*=kf->alpha[i];
            jall+=p4;
            for(unsigned j =i+1 ; j<selectedJets.size();++j) 
            {
                if (VERBOSE)Log(__FUNCTION__,"DEBUG",Form("updateMjj: (2-j) -- considering jets (%d,%d) kfsize=%u",i,j,kf->alpha.size()));
                TLorentzVector p4_2= selectedJets[j]->GetP4();
                p4_2*=kf->alpha[j];
                TLorentzVector jj_tmp=p4+p4_2;
                if (jj.M() < jj_tmp.M()) jj=jj_tmp;
            }
        }
        jetVar_["malljkf"] = jall.M();
        jetVar_["mjjkf"] = jj.M();
        jall+=Hmm.GetP4();
        // met
        jetVar_["metkf"] = jall.Pt();
        jetVar_["valuekf"] = kf->value;
    } // end KF for VH

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","updateMjj: jet var (end) ");
}


void HmumuAnalysis::FillSyncTree(const string& label, const string& systname, const string& category){
    if (not doSync) return;
    if (doSync and not processingSyst_) {
        eventNum = e->eventNum();
        runNum   = e->runNum();
        lumiNum  = e->lumiNum();
        fillTree=true;
        // global info
        SetTreeVar("eventNum",e->eventNum());
        SetTreeVar("runNum",e->runNum());
        SetTreeVar("lumiNum",e->lumiNum());

        SetTreeVar("baremcWeight",1.);
        //SetTreeVar("muonSF",1.);
        //SetTreeVar("btagSF",1.);
        //SetTreeVar("nnlopsSF",1.); // reset both for data and MC

        if (e->IsRealData()){
            SetTreeVar("mc",0);
            SetTreeVar("mcWeight",1.);
            SetTreeVar("puWeight",1.);
            SetTreeVar("weight",1.);

        }

        else {
            SetTreeVar("weight",e->weight());
            SetTreeVar("mcWeight",e->GetWeight()->GetBareMCWeight() * e->GetWeight()->GetBareMCXsec() / e->GetWeight()->GetBareNevents());
            SetTreeVar("baremcWeight",e->GetWeight()->GetBareMCWeight());
            SetTreeVar("puWeight",e->GetWeight()->GetBarePUWeight());
            int mc=0;
            if (label.find( "GluGlu_HToMuMu") != string::npos) mc -=10;
            if (label.find( "GluGluH_HToMuMu") != string::npos and label.find("powheg") != string::npos) mc -=15;
            if (label.find( "VBF_HToMuMu") != string::npos) mc -=20;
            if (label.find( "VBFH_HToMuMu") != string::npos and label.find("powheg") != string::npos) mc -=25;
            if (label.find( "ZH_HToMuMu") != string::npos) mc -=30;
            if (label.find( "WMinusH_HToMuMu") != string::npos) mc -=40;
            if (label.find( "WPlusH_HToMuMu") != string::npos) mc -=50;
            if (label.find( "ttH_HToMuMu") != string::npos) mc -=60;
            if (label.find( "M120") != string::npos) mc -=1;
            if (label.find( "M125") != string::npos) mc -=2;
            if (label.find( "M130") != string::npos) mc -=3;
            if (label == "DY") mc =10;
            if (label == "DYJetsToLL_M-105To160") mc =11;
            if (label == "TT") mc =20;
            if (label == "ST") mc =21;
            if (label == "WW") mc =30;
            if (label == "WZ") mc =31;
            if (label == "ZZ") mc =32;
            if (label == "EWK_LLJJ") mc =40;
            if (mc == 0) mc =1;
            SetTreeVar("mc",mc);
            //Log(__FUNCTION__,"DEBUG",Form("mc=%d label=%s",mc,label.c_str()));
        }
        
        int recoMuons= (mu0!=NULL and mu1!=NULL) ? 1:0;
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
            SetTreeVar("HY" ,Hmm.Rapidity());
            SetTreeVar("Heta" ,Hmm.Eta());
            SetTreeVar("Hphi" ,Hmm.Phi());
            SetTreeVar("deltaeta",fabs(mu0->Eta()-mu1->Eta()));    
            SetTreeVar("deltaphi",fabs(mu0->DeltaPhi(*mu1)));
            SetTreeVar("mt1" ,ChargedHiggs::mt(mu0->Pt(),e->GetMet().Pt(),mu0->Phi(),e->GetMet().Phi()));
            SetTreeVar("mt2" ,ChargedHiggs::mt(mu1->Pt(),e->GetMet().Pt(),mu1->Phi(),e->GetMet().Phi()));
            SetTreeVar("costhetastar",ChargedHiggs::CosThetaCSPos(mu0,mu1));
            SetTreeVar("phics",ChargedHiggs::PhiCSPos(mu0,mu1));
            fillTree=true; 
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
            SetTreeVar("HY" ,-10);
            SetTreeVar("Heta",-10);
            SetTreeVar("Hphi",-10);
            SetTreeVar("deltaeta",-10);    
            SetTreeVar("deltaphi",-10);
            SetTreeVar("mt1", -10);
            SetTreeVar("mt2", -10);
            SetTreeVar("costhetastar",-10);
            SetTreeVar("phics",-10);
        } 
        SetTreeVar("njets",selectedJets.size());
        SetTreeVar("met" ,e->GetMet().Pt());
        SetTreeVar("metphi" ,e->GetMet().Phi());

        SetTreeVar("bdt", (bdt.size()>0)?bdt[0]:-10);
        SetTreeVar("bdtUCSD01j", (bdt.size()>1)?bdt[1]:-10);
        //SetTreeVar("bdtUCSD2j", (bdt.size()>2)?bdt[2]:-10);
    

        // basic jet info
        for(int ijet=0;ijet<std::min(selectedJets.size(),size_t(20));++ijet)
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
        
        if (recoMuons){
            float drmj=100.;
            for( auto j : selectedJets)
            {
                drmj = min(drmj, float(j->DeltaR(mu0)));
                drmj = min(drmj, float(j->DeltaR(mu1)));
            }
            if (drmj>99.) drmj=0.; 
            SetTreeVar("drmj",drmj);    
        } else SetTreeVar("drmj",-10);

        SetTreeVar("detajj_lead" ,jetVar_["detajj_lead"]);
        SetTreeVar("dphijj_lead" ,jetVar_["dphijj_lead"]);

        SetTreeVar("zepen", jetVar_["z*"]);    

        SetTreeVar("aveQGLCent" ,jetVar_["aveQGLcent"]);
        //SetTreeVar("aveCSV"     ,jetVar_["aveCSV"]);
        SetTreeVar("maxCSV"     ,jetVar_["maxCSV"]);

        //SetTreeVar("secondCSV"  ,jetVar_["secondCSV"]);
        //SetTreeVar("thirdCSV"   ,jetVar_["thirdCSV"]);

        SetTreeVar("firstQGL"   ,jetVar_["firstQGL"]);
        SetTreeVar("secondQGL"  ,jetVar_["secondQGL"]);
        SetTreeVar("thirdQGL"   ,jetVar_["thirdQGL"]);
        
        SetTreeVar("htCent"     ,jetVar_["htCent"]);
        SetTreeVar("nbjets"     ,jetVar_["nbjets"]);
        SetTreeVar("ncentjets"  ,jetVar_["ncentjets"]);
        SetTreeVar("nfwdjets"   ,jetVar_["nfwdjets"]);

        SetTreeVar("leadDeepB"  ,jetVar_["leadDeepB"]); 
        SetTreeVar("maxDeepB"   ,jetVar_["maxDeepB"] ); 

        //SetTreeVar("softNjets1" ,jetVar_["softNjets1"]);
        //SetTreeVar("softHt1"    ,jetVar_["softHt1"]);
        //SetTreeVar("softNjets5" ,jetVar_["softNjets5"]);
        //SetTreeVar("softHt5"    ,jetVar_["softHt5"]);
        //SetTreeVar("softNjets10",jetVar_["softNjets10"]);
        //SetTreeVar("softHt10"   ,jetVar_["softHt10"]);

        SetTreeVar("mjj_lead"   ,jetVar_["mjj_lead"]);
        SetTreeVar("dphijj_lead"   ,jetVar_["dphijj_lead"]);
        SetTreeVar("detajj_lead"   ,jetVar_["detajj_lead"]);
        SetTreeVar("mjj_1"      ,jetVar_["mjj_1"]);
        SetTreeVar("mjj_2"      ,jetVar_["mjj_2"]);
        SetTreeVar("detajj_1"   ,jetVar_["detajj_1"]);
        SetTreeVar("detajj_2"   ,jetVar_["detajj_2"]);

        // fill category
        int cat=-1;
        for( int i=0;i<50;++i)
        {
            if(category ==Form("cat%d",i))
            {
                cat=i;
                break;
            }
        }
        SetTreeVar("mmm",mass_);
        SetTreeVar("cat",cat);

    }

    if (doSync and (systname == "JESUp" or systname=="JESDown") ) 
    {
        SetTreeVar("mjj_1_"+systname,(mjj.size() > 0 ) ? mjj[0].first: 0.0);
        SetTreeVar("mjj_2_"+systname,(mjj.size() > 1 ) ? mjj[1].first: 0.0);
    }

    return ;
}

float HmumuAnalysis::BdtUF(int pos)
{
    if (bdt.size() != readers_.size()) {bdt.clear(); bdt.resize(readers_.size());}
    bdt[pos]=-1.;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start Bdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return bdt[pos];

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

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

    if (pos>=0) bdt[pos]=(readers_[pos]->EvaluateMVA("BDTG_default") );

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Bdt");
    return bdt[pos];
}

float HmumuAnalysis::BdtUCSD(int pos,int nj)
{
    if (bdt.size() != readers_.size()) {bdt.clear(); bdt.resize(readers_.size());}
    bdt[pos]=-1.;

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start Bdt");
    // return the right category
    if (mu0== NULL or mu1==NULL) return bdt[pos];
    if (nj==2 and selectedJets.size() <2) return bdt[pos];

    float eta0 = mu0->Eta();
    float eta1 = mu1->Eta();

    SetVariable("dimu_pt",Hmm.Pt());    
    SetVariable("dimu_eta",Hmm.Eta());    

    SetVariable("hmmpt",Hmm.Pt());    
    SetVariable("hmmrap",Hmm.Rapidity());    
    SetVariable("hmmthetacs",ChargedHiggs::CosThetaCSPos(mu0,mu1));    
    SetVariable("hmmphics",ChargedHiggs::PhiCSPos(mu0,mu1));

    if (selectedJets.size() >=1)
    {
        SetVariable("j1pt",selectedJets[0]->Pt());    
        SetVariable("j1eta",selectedJets[0]->Eta());    
    }
    else{
        SetVariable("j1pt",0.);    
        SetVariable("j1eta",0.);    
    }
    float drmj=100.;
    for( auto j : selectedJets)
    {
        drmj = min(drmj, float(j->DeltaR(mu0)));
        drmj = min(drmj, float(j->DeltaR(mu1)));
    }
    if (drmj>99.) drmj=0.; 
    SetVariable("drmj",drmj);    

    float dphimmj = (selectedJets.size()>0)?float(selectedJets[0]->DeltaPhi(Hmm)):0.;
    if(selectedJets.size()>1)dphimmj=min(dphimmj,float(selectedJets[1]->DeltaPhi(Hmm)));
    float detammj = (selectedJets.size()>0)?float(selectedJets[0]->DeltaEta(Hmm)):0.;
    if(selectedJets.size()>1)detammj=min(detammj,float(selectedJets[1]->DeltaEta(Hmm)));
    SetVariable("dphimmj",dphimmj);    
    SetVariable("detammj",detammj);    

    if (selectedJets.size() >=2)
    {
        SetVariable("j2pt",selectedJets[1]->Pt());    
        SetVariable("detajj",jetVar_["detajj_lead"]);    
        SetVariable("dphijj",jetVar_["dphijj_lead"]);    
        SetVariable("mjj",jetVar_["mjj_lead"]);    

        SetVariable("zepen", jetVar_["z*"]);    
    }
    else
    {
        SetVariable("j2pt",0.);    
        SetVariable("detajj",0.);    
        SetVariable("dphijj",0.);    
        SetVariable("mjj",0.);    
        SetVariable("zepen", 0.);    
    }
    SetVariable("met",e->GetMet().Pt());    
    SetVariable("njets",selectedJets.size());    
    SetVariable("nbjets",jetVar_["nbjets"]);     // medium
    SetVariable("m1ptOverMass",mu0->Pt()/mass_);    
    SetVariable("m2ptOverMass",mu1->Pt()/mass_);    
    SetVariable("m1eta",mu0->Eta());    
    SetVariable("m2eta",mu1->Eta());    

    if (pos>=0) bdt[pos]=(readers_[pos]->EvaluateMVA("BDTG_default") );
    
    
    //if (e->eventNum() == 821623)
    if (bdt[pos]<-.98)
    {
        Log(__FUNCTION__,"DEBUG-SYNC",Form("mu0 pt=%f, eta=%f, phi=%f, mass=%f, rap=%f",mu0->Pt(),mu0->Eta(),mu0->Phi(),mu0->M(),mu0->Rapidity()));
        Log(__FUNCTION__,"DEBUG-SYNC",Form("mu1 pt=%f, eta=%f, phi=%f, mass=%f, rap=%f",mu1->Pt(),mu1->Eta(),mu1->Phi(),mu1->M(),mu1->Rapidity()));
        Log(__FUNCTION__,"DEBUG-SYNC",Form("Hmm pt=%f, eta=%f, phi=%f, mass=%f, mass_=%f, rap=%f",Hmm.Pt(),Hmm.Eta(),Hmm.Phi(),Hmm.M(),mass_,Hmm.Rapidity()));
        Log(__FUNCTION__,"DEBUG-SYNC",Form("y*=%f, z*=%f, rap=%f, rap2=%f",jetVar_["y*"],jetVar_["z*"],Hmm.Rapidity(),(selectedJets[0]->Rapidity() + selectedJets[1]->Rapidity())/2.0));
        Log(__FUNCTION__,"DEBUG-SYNC",Form("Jet0/1: Jet 1 pt=%f, eta=%f, phi=%f, y=%f Jet2 pt=%f, eta=%f, phi=%f, y=%f",selectedJets[0]->Pt(),selectedJets[0]->Eta(),selectedJets[0]->Phi(),selectedJets[0]->Rapidity(),selectedJets[1]->Pt(),selectedJets[1]->Eta(),selectedJets[1]->Phi(),selectedJets[1]->Rapidity()));

        Log(__FUNCTION__,"DEBUG-SYNC",Form("evaluating bdt pos=%d nj=%d",pos,nj));
        for (auto &v : vector<string>({"hmmpt","hmmrap","hmmthetacs","hmmphics","j1pt","j1eta","j2pt","mjj","detajj","dphijj","zepen","njets","detammj","dphimmj","m1ptOverMass","m2ptOverMass","m1eta","m2eta"}) )
        {
            Log(__FUNCTION__,"DEBUG-SYNC",Form(" %s=%f",v.c_str(),GetVariable<float>(v)));
        }
        Log(__FUNCTION__,"DEBUG-SYNC",Form("  mva value=%f",bdt[pos]));
        Log(__FUNCTION__,"DEBUG-SYNC","------------------------");
    }
    

    if (VERBOSE)Log(__FUNCTION__,"DEBUG","End Bdt");
    return bdt[pos];
}

void HmumuAnalysis::InitTmvaUCSD(int pos,int nj){
    //nj==0 -> nonvbf
    //nj==2 -> vbf
    Log(__FUNCTION__,"INFO","Init Reader");
    TMVA::Tools::Instance();
    if (readers_.size() != pos) {
            Log(__FUNCTION__,"ERROR","readers doesn't match pos") ;
            throw abortException();
    } 
    readers_ . push_back( new TMVA::Reader() );

    AddVariable("hmmpt",'F');    
    AddVariable("hmmrap",'F');    
    AddVariable("hmmthetacs",'F');    
    AddVariable("hmmphics",'F');
    AddVariable("j1pt",'F');    
    AddVariable("j1eta",'F');     // temp

    AddVariable("j2pt",'F');    
    AddVariable("detajj",'F');    
    AddVariable("dphijj",'F');    
    AddVariable("mjj",'F');    
    
    AddVariable("zepen",'F');    
    if (nj==0) AddVariable("njets",'F');    
    if(nj==2) AddVariable("detammj",'F');
    AddVariable("dphimmj",'F');    
    if(nj==0) AddVariable("detammj",'F');

    AddVariable("m1ptOverMass",'F');    
    AddVariable("m2ptOverMass",'F');    
    AddVariable("m1eta",'F');    
    AddVariable("m2eta",'F');    
    if (nj==2) AddVariable("njets",'F');    
    if (nj==2) AddVariable("nsajets2",'F');    
    if (nj==2) AddVariable("sajetsht2",'F');    
    if (nj==2) AddVariable("ptbalance",'F');    
    if (nj==2) AddVariable("ptcentrality",'F');    
  
    if (nj==0){ 
        AddSpectator("event",'F');SetVariable("event",1.);
        AddSpectator("hmass",'F');SetVariable("hmass",1.);
        AddSpectator("hmerr",'F');SetVariable("hmerr",1.);
        AddSpectator("weight",'F');SetVariable("weight",1.);
        AddSpectator("bdtucsd_2jet_nonvbf",'F');SetVariable("bdtucsd_2jet_nonvbf",1.);
    }
    else {
        AddSpectator("xxx",'F');SetVariable("xxx",1.);
    }


    // load weights
    Log(__FUNCTION__,"INFO",Form("Loading weights idx=%d: ",pos)+weights[pos]);
    readers_[pos]->BookMVA("BDTG_default",weights[pos].c_str());

}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
