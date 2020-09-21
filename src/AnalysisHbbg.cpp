#include "interface/AnalysisHbbg.hpp"
#include "TRandom3.h"
#include "TStopwatch.h"
#include <algorithm>
#include "interface/GeneralFunctions.hpp"
#include "interface/HiggsTemplateCrossSections.hpp"
#include "interface/HiggsTemplateCrossSections_Stage1p1.hpp"

#include "TVectorD.h"
#include "TMatrixD.h"

#define VERBOSE 0

#define DEEP_B_LOOSE ((year==2016)?0.2217:(year==2017)?0.1522:0.1241)
#define DEEP_B_MEDIUM ((year==2016)?0.6321:(year==2017)?0.4941:0.4148)
#define DEEP_B_TIGHT ((year==2016)?0.8953:(year==2017)?.8001:.7527)


#define SYNC_VERBOSE 1


void HbbgAnalysis::SetLeptonCuts(Lepton *l){ 
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

void HbbgAnalysis::SetJetCuts(Jet *j) { 
    j->SetEtaCut(4.7); 
    j->SetEtaCutCentral(2.5); 
    #warning 25GEV_JETS
    j->SetPtCut(25); 
    j->SetPuIdCut(100);
    // Noise hard cut on jets
    if (year==2017) j->SetEENoiseCut(true);
    else j->SetEENoiseCut(false);
    // DeepCSV
    j->SetBCut(-100); //L=0.5426 , M=  0.8484, T0.9535 
    j->SetDeepBCut(DEEP_B_MEDIUM); 
}

void HbbgAnalysis::SetTauCuts(Tau *t){ 
    t->SetIsoCut(2.5); 
    t->SetEtaCut(2.1); 
    t->SetPtCut(8000);  // remove taus!!! 
    t->SetMuRej(true); 
    t->SetEleRej(true);
    t->SetTrackPtCut(30.);
    t->SetProngsCut(1); 
    t->SetDecayMode(1);
}

void HbbgAnalysis::SetPhotonCuts(Photon *p){ // Already selecting Medium ided photons
    // I'm removing jets that look really likes photons.
    p->SetIsoCut(-1); 
    p->SetIsoRelCut(0.15); 
    p->SetPtCut(15);
    p->SetEtaCut(2.5);
}


void HbbgAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

    if (not jet_resolution)
    { // init jet resolution
        Log(__FUNCTION__,"INFO","Init JER");
        jet_resolution.reset( new JME::JetResolutionObject("aux/jer/Summer16_25nsV1/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt") ) ;
    }

    kf.reset (new KinematicFit2);

    // define categories -- for booking histos

	Log(__FUNCTION__,"INFO","Booking Histograms");
    for ( string l : AllLabel()  ) {
        for (string s : {"PreTrigger","Preselection","Final"} )
        {
        for (string suff : {"","_RIGHT","_WRONG"})
        {
	        Book ("HbbgAnalysis/"+s+"/mass_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 2000,60,160);
	        Book ("HbbgAnalysis/"+s+"/pho_pt_"+ l+suff,"pTg;p_{T}^{#gamma} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/pho_eta_"+ l+suff,"etag;#eta^{#gamma} [GeV];Events", 200,-5,5);
	        Book ("HbbgAnalysis/"+s+"/mjj_"+ l+suff,"Mjj;m^{qq} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/mbb_"+ l+suff,"Mbb;m^{bb} [GeV];Events", 2000,60,160);
	        Book ("HbbgAnalysis/"+s+"/jet1_pt_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/jet1_eta_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,-5,5);
	        Book ("HbbgAnalysis/"+s+"/jet2_pt_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/jet2_eta_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,-5,5);
	        Book ("HbbgAnalysis/"+s+"/bjet1_pt_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/bjet1_eta_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,-5,5);
	        Book ("HbbgAnalysis/"+s+"/bjet2_pt_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,0,2000);
	        Book ("HbbgAnalysis/"+s+"/bjet2_eta_"+ l+suff,"Mbbg;m^{bb#gamma} [GeV];Events", 200,-5,5);
	        Book ("HbbgAnalysis/"+s+"/bdt_"+ l+suff,"Mjj;m^{qq} [GeV];Events", 200,0,2000);
        }// suffix
        } // selection tag

        for (int icat=0; icat <10 ;++icat)
	    Book (Form("HbbgAnalysis/Categories/mass_cat%d_",icat)+ l,"Mbbg;m^{bb#gamma} [GeV];Events", 2000,60,160);


    } //end label loop
    

    if (doTree){
        if (debug)Log(__FUNCTION__,"DEBUG","Init Tree");
        InitTree("hbbg");
        Branch("hbbg","eventNum",'I');
        Branch("hbbg","runNum",'I');
        Branch("hbbg","lumiNum",'I');
        Branch("hbbg","weight",'D');
        Branch("hbbg","mc",'I');
        // Higgs System
        Branch("hbbg","pt",'D');
        Branch("hbbg","mass",'D');
        Branch("hbbg","dphibbg",'F');
        Branch("hbbg","detabbg",'F');
        Branch("hbbg","drbb",'F');
        Branch("hbbg","dphibb",'F');

        Branch("hbbg","ptb1",'F');
        Branch("hbbg","ptb2",'F');
        Branch("hbbg","ptpho",'F');
        Branch("hbbg","etab1",'F');
        Branch("hbbg","etab2",'F');
        Branch("hbbg","etapho",'F');
        Branch("hbbg","phib1",'F');
        Branch("hbbg","phib2",'F');
        Branch("hbbg","phipho",'F');

        Branch("hbbg","chi2",'F'); // b-kinematic fit

        //dijet system
        Branch("hbbg","detajj",'F');
        Branch("hbbg","dphijj",'F');
        // associated jets
        Branch("hbbg","njets",'I');
        Branch("hbbg","jet_pt",'d',20,"njets");
        Branch("hbbg","jet_eta",'d',20,"njets");
        Branch("hbbg","jet_phi",'d',20,"njets");

        Branch("hbbg","nbjets",'I');
        Branch("hbbg","nbjets_loose",'I');
        Branch("hbbg","ht",'F');
        Branch("hbbg","ht_cent",'F');
        Branch("hbbg","ht_jj",'F');
        // Higgs and  dijet system
        Branch("hbbg","z_",'F');
        Branch("hbbg","y_",'F');
    }

    if (doTMVA){InitTmva();}

    if (debug)Log(__FUNCTION__,"DEBUG","End Init");
}

void HbbgAnalysis::updateTreeVar(){
        SetTreeVar("eventNum",e->eventNum());
        SetTreeVar("runNum",e->runNum());
        SetTreeVar("lumiNum",e->lumiNum());
        //SetTreeVar("cat",cat);
        for (const string & s : {"pt","mass","njets","detajj","dphijj","z_","y_","njets","dphibbg","detabbg","nbjets","nbjets_loose","ht","ht_cent","ht_jj","ptb1","ptb2","etab1","etab2","phib1","phib2","chi2","drbb","dphibb"}){
            SetTreeVar(s,eventVar_[s]);
        }
        SetTreeVar("weight",e->weight());

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
        // Higgs System        
        SetTreeVar("ptpho",pho->Pt());
        SetTreeVar("etapho",pho->Eta());
        SetTreeVar("phipho",pho->Phi());


        // truth
        // VBF_HiggsZG_Zbb_M125

        int mc=100;
        if (label.find( "GluGlu_HiggsZG_Zbb_M125") != string::npos) mc =-10;
        if (label.find( "VBF_HiggsZG_Zbb_M125") != string::npos) mc =-20;
        if (label == "DY") mc =10;
        if (label == "TT") mc =20;
        if (label == "ST") mc =21;
        if (label == "WW") mc =30;
        if (label == "WZ") mc =31;
        if (label == "ZZ") mc =32;
        if (label == "QCD") mc = 50;
        if (label == "Other") mc = 100;
        if (label == "Data") mc =0;
        SetTreeVar("mc",mc);

}

void HbbgAnalysis::updateTmva(){
    // Set Variables
    for (const string & s : {"pt","dphijj","detajj","y_","ht","drbb","dphibb","dphibbg","detabbg","ptb1","ptb2","etab1","etab2"}){
            SetTmvaVariable(s,eventVar_[s]);
    }
    SetTmvaVariable("jet_pt[0]",selectedJets[0]->Pt());
    SetTmvaVariable("jet_pt[1]",selectedJets[1]->Pt());
    SetTmvaVariable("jet_eta[0]",selectedJets[0]->Eta());
    SetTmvaVariable("jet_eta[1]",selectedJets[1]->Eta());
    SetTmvaVariable("Alt$(jet_eta[2],-10)",(selectedJets.size()>1)?selectedJets[2]->Eta():-10.);
    SetTmvaVariable("ptpho",pho->Pt());
    SetTmvaVariable("etapho",pho->Eta());
    
    // EvaluateBDT
    bdt.clear();
    for(unsigned i =0 ;i< readers_.size() ; ++i)
    {
         bdt.push_back(readers_[i]->EvaluateMVA("BDTG") );
    }
}

void HbbgAnalysis::updateEventVar( )
{
    eventVar_ . clear();
    
    //b1->GetBCorrection()
    // -----------------------------------
    TLorentzVector Zbb,Hbbg;

    Zbb += (selectedBJets[0]->GetP4() * selectedBJets[0]->GetBCorrection());
    Zbb += (selectedBJets[1]->GetP4() * selectedBJets[1]->GetBCorrection());

    Hbbg +=  Zbb;
    Hbbg += (pho->GetP4());

    eventVar_["mass"] = Hbbg.M(); //override with kf later if available
    eventVar_["pt"] = Hbbg.Pt();
    mass_= Hbbg.M(); // Use this one!
    pt_= Hbbg.Pt();

    // -----------------------------------
    TLorentzVector Zbb_u,Hbbg_u; // uncorrected b
    Zbb_u += (selectedBJets[0]->GetP4() );
    Zbb_u += (selectedBJets[1]->GetP4() );

    Hbbg_u +=  Zbb_u;
    Hbbg_u += (pho->GetP4());


    TLorentzVector Zbb_kf,Hbbg_kf; 
    bool haveKF=false;

    // -------------- KF ---------------------
    if (kf->p4.size() >0 and kf->alpha_out.size()>1 and kf->value_out < 1.e9){ // filled and ok
        TLorentzVector b1_kf = (selectedBJets[0]->GetP4()*selectedBJets[0]->GetBCorrection() *kf->alpha_out[0]);
        TLorentzVector b2_kf = (selectedBJets[1]->GetP4()*selectedBJets[1]->GetBCorrection() * kf->alpha_out[1]);

        Zbb_kf += b1_kf ;
        Zbb_kf += b2_kf ;

        Hbbg_kf +=  Zbb_kf;
        Hbbg_kf += (pho->GetP4());

        eventVar_["mass_kf"] = Hbbg_kf.M();
        eventVar_["pt_kf"] = Hbbg_kf.Pt();
        
        mass_= Hbbg_kf.M(); // Use this one!
        pt_= Hbbg_kf.Pt();

        eventVar_["mass"] = Hbbg_kf.M();
        eventVar_["pt"] = Hbbg_kf.Pt();

        eventVar_["ptb1"] = b1_kf.Pt();
        eventVar_["ptb2"] = b2_kf.Pt();
        eventVar_["etab1"] = b1_kf.Eta();
        eventVar_["etab2"] = b2_kf.Eta();
        eventVar_["phib1"] = b1_kf.Phi();
        eventVar_["phib2"] = b2_kf.Phi();

        eventVar_["chi2"] = kf->value_out;

        haveKF=true;
    }
    // -----------------------------------


    eventVar_["mass_breg"] = Hbbg.M();
    eventVar_["pt_breg"] = Hbbg.Pt();

    eventVar_["mass_uncorr"] = Hbbg_u.M();
    eventVar_["pt_uncorr"] = Hbbg_u.Pt();

    eventVar_["mjj"] = selectedJets[0]->InvMass(selectedJets[1]);
    eventVar_["mbb"] = Zbb.M(); // breg, no kf
    eventVar_["ptbb"] = Zbb.Pt();
    eventVar_["etabb"] = Zbb.Eta();
    eventVar_["detabb"] = fabs(selectedBJets[0]->Eta() - selectedBJets[1]->Eta());
    eventVar_["dphibb"] = fabs(selectedBJets[0]->DeltaPhi(selectedBJets[1]) );
    eventVar_["drbb"] = fabs(selectedBJets[0]->DeltaR(selectedBJets[1]) );

    eventVar_["mbb_u"] = Zbb_u.M();
    eventVar_["ptbb_u"] = Zbb_u.Pt();

    eventVar_["detajj"] = fabs(selectedJets[0]->Eta() - selectedJets[1]->Eta());
    eventVar_["dphijj"] = fabs(selectedJets[0]->DeltaPhi(selectedJets[1]) );
    eventVar_["njets"] = selectedJets.size();

    if (haveKF){ // use kf for these ones
        eventVar_["y_"] = (selectedJets.size()>=2) ? ( - Hbbg_kf.Rapidity() + (selectedJets[0]->Rapidity() + selectedJets[1]->Rapidity())/2.0 ) :-10;
        eventVar_["z_"] = (selectedJets.size()>=2) ? (eventVar_["y_"] /fabs(selectedJets[0]->Rapidity() - selectedJets[1]->Rapidity()) ):-10;

        eventVar_["dphibbg"] = fabs(Zbb_kf.DeltaPhi(pho->GetP4()));
        eventVar_["detabbg"] = fabs(Zbb_kf.Eta() - pho->Eta());

    } // end haveKF
    
  
    // additional bjets 
    eventVar_["nbjets"]=0; 
    eventVar_["nbjets_loose"]=0; 
    eventVar_["ht"]=0.; 
    eventVar_["ht_cent"]=0.; 
    eventVar_["ht_jj"]=0.;  // between the two mjj jets
    for(unsigned i=0;i<selectedJets.size() ;++i)
    {
        eventVar_["ht"] += selectedJets[i]->Pt(); 
        if ( abs(selectedJets[i]->Eta())<=2.4) // TO CHECK
        {
            if (selectedJets[i]->GetDeepB() > DEEP_B_MEDIUM) eventVar_["nbjets"]+=1;
            if (selectedJets[i]->GetDeepB() > DEEP_B_LOOSE) eventVar_["nbjets_loose"]+=1;
            eventVar_["ht_cent"] += selectedJets[i]->Pt(); 
        }

        if (( i>=2 and selectedJets[0]->Eta()<selectedJets[i]->Eta() and selectedJets[i]->Eta() <selectedJets[1]->Eta()) or
            ( i>=2 and selectedJets[1]->Eta()<selectedJets[i]->Eta() and selectedJets[i]->Eta() <selectedJets[0]->Eta())
            ) eventVar_["ht_jj"] += selectedJets[i]->Pt();
    }

    // central HT
}

void HbbgAnalysis::matchToGen()
{
    // gen Matching contains: b b g - q q - H Z
    //                        0 1 2   3 4   5 6
    genMatching.clear();
    genMatching.resize(8, nullptr);

    for( int ig=0;;++ig){
        GenParticle *g =  e->GetGenParticle(ig);
        if (g==NULL) break;
        if ( abs(g->GetPdgId()) == 5 and g->GetParentPdgId() == 23 and e->GenParticleDecayedFrom(ig,25) ) 
        {
            if (selectedBJets.size() >0 and selectedBJets[0]->DeltaR(g) <0.4) 
                    {
                    if (genMatching[0] != nullptr) Log(__FUNCTION__,"WARNING","Double association for B0");
                    genMatching[0] = g;
                    }
            if (selectedBJets.size() >1 and selectedBJets[1]->DeltaR(g) <0.4) 
                    {
                    if (genMatching[1] != nullptr) Log(__FUNCTION__,"WARNING","Double association for B0");
                    genMatching[1] = g;
                    }
        }
        if (abs(g->GetPdgId()) == 25 and g->IsLHE()) genMatching[5]=g;
        if (abs(g->GetPdgId()) == 23 and g->GetParentPdgId() == 25) genMatching[6]=g;
        if (abs(g->GetPdgId()) == 22 and g->GetParentPdgId() == 25) genMatching[2]=g;
        if (abs(g->GetPdgId()) < 5 and g->IsLHE() and g->Pt() >0.1 and selectedJets.size()>0 and selectedJets[0]->DeltaR(g)<0.4) genMatching[3] = g;  
        if (abs(g->GetPdgId()) < 5 and g->IsLHE() and g->Pt() >0.1 and selectedJets.size()>0 and selectedJets[1]->DeltaR(g)<0.4) genMatching[4] = g;  


    }
    /* VBF NLO -> 
     * *       35 *        0 *         2 *         0 *     1e+11 *
     * *       35 *        1 *        -4 *         0 *    -1e+11 *
     * *       35 *        2 *        25 * 36.910307 * -1.929126 *
     * *       35 *        3 *         1 * 66.467781 * 1.5728097 *
     * *       35 *        4 *        -1 * 37.307477 * -1.408825 *
     * *       35 *        5 *        21 * 18.729079 * 2.0525645 *
     * *       36 *        0 *         1 *         0 *     1e+11 * qg->Hqq
     * *       36 *        1 *        21 *         0 *    -1e+11 *
     * *       36 *        2 *        25 * 76.890944 * -1.234957 *
     * *       36 *        3 *         1 * 67.260719 * 1.9591242 *  (!)
     * *       36 *        4 *         2 * 39.900033 * -4.206107 *
     * *       36 *        5 *        -2 * 49.021449 * -3.687607 *  (!)
     */
    return ;
}

void HbbgAnalysis::printObject(const string &s, Object*o) const
{
    if (o==nullptr)
    {
        Log(__FUNCTION__,"DEBUG",s+": NULL");
        return;
    }
	Log(__FUNCTION__,"DEBUG",s+Form(": pt %.1f eta %.2f phi %.1f m %.1f",o->Pt(),o->Eta(),o->Phi(),o->M()));
    return;
}
void HbbgAnalysis::printDebugInfo(){
	Log(__FUNCTION__,"DEBUG","Matching:");
    for (unsigned int ig=0;ig<genMatching.size();++ig)
    {
    // gen Matching contains: b b g - q q - H Z
    //                        0 1 2   3 4   5 6
        switch (ig)
        {
            case 0:  Log(__FUNCTION__,"DEBUG", "b1"); break;
            case 1:  Log(__FUNCTION__,"DEBUG", "b2"); break;
            case 2:  Log(__FUNCTION__,"DEBUG", "g"); break;
            case 3:  Log(__FUNCTION__,"DEBUG", "q1"); break;
            case 4:  Log(__FUNCTION__,"DEBUG", "q2"); break;
            case 5:  Log(__FUNCTION__,"DEBUG", "H"); break;
            case 6:  Log(__FUNCTION__,"DEBUG", "Z"); break;
        }
        GenParticle*g=genMatching[ig];
        printObject("genM",g);
    }
	Log(__FUNCTION__,"DEBUG","Truth Information:");
    for( int ig=0;;++ig){
        GenParticle *g =  e->GetGenParticle(ig);
        if (g==NULL) break;
        if ( abs(g->GetPdgId()) == 5 and g->GetParentPdgId() == 23 and e->GenParticleDecayedFrom(ig,25) ) 
        {
            printObject("b-quark",g);
        }
        if (abs(g->GetPdgId()) == 25 and g->IsLHE()) printObject("H",g);
        if (abs(g->GetPdgId()) == 23 and g->GetParentPdgId() == 25) printObject("Z",g);
        if (abs(g->GetPdgId()) == 22 and g->GetParentPdgId() == 25) printObject("g",g);
        if (abs(g->GetPdgId()) < 5 and g->IsLHE() and g->Pt() >0.1) printObject("q",g);  
    }
	Log(__FUNCTION__,"DEBUG","Selected Info:");
    if (selectedBJets.size()>0 ) printObject("bjet",selectedBJets[0]);
    if (selectedBJets.size()>1 ) printObject("bjet",selectedBJets[1]);
    printObject("pho",pho);

    for (Jet *j : selectedJets){
        printObject("jet",j);
    }

    // loop over photons
	Log(__FUNCTION__,"DEBUG","All photons");
    for( int ip=0;;++ip){
        Photon *p =  e->GetPhoton(ip);
        if (p==nullptr) break;
        printObject("pho-all",p);
    }
    // 
	Log(__FUNCTION__,"DEBUG","Relevant Var");
    Log(__FUNCTION__,"DEBUG",Form("mass %f",eventVar_["mass"]));
    Log(__FUNCTION__,"DEBUG",Form("mbb %f",eventVar_["mbb"]));
    Log(__FUNCTION__,"DEBUG",Form("mass (u) %f",eventVar_["mass_uncorr"]));
    Log(__FUNCTION__,"DEBUG",Form("mbb (u) %f",eventVar_["mbb_uncorr"]));
    Log(__FUNCTION__,"DEBUG",Form("mjj %f",eventVar_["mjj"]));
    Log(__FUNCTION__,"DEBUG",Form("detajj %f",eventVar_["detajj"]));

    Log(__FUNCTION__,"DEBUG",Form("mass (kf) %f",eventVar_["mass_kf"]));
    Log(__FUNCTION__,"DEBUG",Form("mbb (kf) %f",eventVar_["mbb_kf"]));
}

void HbbgAnalysis::fillHists(const string & s, const string &l, const string & syst){
    Fill ("HbbgAnalysis/"+s+"/mass_"+ l,syst ,mass_,e->weight());
    Fill ("HbbgAnalysis/"+s+"/pho_pt_"+ l,syst ,pho->Pt(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/pho_eta_"+ l,syst ,pho->Eta(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/mjj_"+ l,syst , eventVar_["mjj"],e->weight());
    Fill ("HbbgAnalysis/"+s+"/mbb_"+ l ,syst,eventVar_["mbb"],e->weight());
    Fill ("HbbgAnalysis/"+s+"/jet1_pt_"+ l,syst ,selectedJets[0]->Pt(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/jet1_eta_"+ l,syst ,selectedJets[0]->Eta(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/jet2_pt_"+ l,syst ,selectedJets[1]->Pt(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/jet2_eta_"+ l ,syst,selectedJets[1]->Eta(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/bjet1_pt_"+ l ,syst,selectedBJets[0]->Pt(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/bjet1_eta_"+ l ,syst,selectedBJets[0]->Eta(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/bjet2_pt_"+ l ,syst,selectedBJets[1]->Pt(),e->weight());
    Fill ("HbbgAnalysis/"+s+"/bjet2_eta_"+ l ,syst,selectedBJets[1]->Eta(),e->weight());

    if (doTMVA and bdt.size()>0) Fill ("HbbgAnalysis/"+s+"/bdt_"+ l ,syst,bdt[0],e->weight());

    if (isSignal and genMatching.size() >0 )
    {
    // gen Matching contains: b b g - q q - H Z
    //                        0 1 2   3 4   5 6
        bool Hbbg = genMatching[0] and genMatching[1] and genMatching[2];
        Fill ("HbbgAnalysis/"+s+"/mass_"+ l + (Hbbg ? "_RIGHT" :"_WRONG"),syst ,mass_,e->weight());
        bool isGamma = genMatching[2]; 
        Fill ("HbbgAnalysis/"+s+"/pho_pt_"+ l + (isGamma? "_RIGHT":"_WRONG"),syst ,pho->Pt(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/pho_eta_"+ l + (isGamma ? "_RIGHT":"_WRONG"),syst ,pho->Eta(),e->weight());
        bool isQQ = genMatching[3] and genMatching[4]; 
        Fill ("HbbgAnalysis/"+s+"/mjj_"+ l + (isQQ ? "_RIGHT":"_WRONG"),syst , eventVar_["mjj"],e->weight());
        bool isBB = genMatching[0] and genMatching[1]; 
        Fill ("HbbgAnalysis/"+s+"/mbb_"+ l  + (isBB ? "_RIGHT":"_WRONG"),syst,eventVar_["mbb"],e->weight());
        bool isQ1 = genMatching[3] ; 
        bool isQ2 = genMatching[4]; 
        Fill ("HbbgAnalysis/"+s+"/jet1_pt_"+ l + (isQ1 ? "_RIGHT":"_WRONG"),syst ,selectedJets[0]->Pt(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/jet1_eta_"+ l + (isQ1 ? "_RIGHT":"_WRONG"),syst ,selectedJets[0]->Eta(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/jet2_pt_"+ l + (isQ2 ? "_RIGHT":"_WRONG"),syst ,selectedJets[1]->Pt(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/jet2_eta_"+ l + (isQ2 ? "_RIGHT":"_WRONG"),syst,selectedJets[1]->Eta(),e->weight());
        bool isB1 = genMatching[0] ; 
        bool isB2 = genMatching[1]; 
        Fill ("HbbgAnalysis/"+s+"/bjet1_pt_"+ l + (isB1 ? "_RIGHT":"_WRONG"),syst,selectedBJets[0]->Pt(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/bjet1_eta_"+ l + (isB1 ? "_RIGHT":"_WRONG"),syst,selectedBJets[0]->Eta(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/bjet2_pt_"+ l + (isB2 ? "_RIGHT":"_WRONG"),syst,selectedBJets[1]->Pt(),e->weight());
        Fill ("HbbgAnalysis/"+s+"/bjet2_eta_"+ l + (isB2 ? "_RIGHT":"_WRONG"),syst,selectedBJets[1]->Eta(),e->weight());

    }
}

void HbbgAnalysis::reset() // reset private memebers
{
    eventNum=0;
    runNum=0;
    lumiNum=0;
    mass_=0.;
    pt_=0.;
    pho=nullptr;
    selectedJets . clear();
    selectedBJets . clear();
    passTrigger=false;
    categories_ . clear();
    eventVar_ . clear();
    genMatching .clear();
    isSignal=false;
    kf->clear();
    bdt.clear();
}

int HbbgAnalysis::analyze(Event *event, string systname)
{
    if (debug)Log(__FUNCTION__,"DEBUG",Form("Analyze event %ld:%ld:%ld",e->runNum(),e->lumiNum(),e->eventNum()));
    
    //** reset 
    reset();

    if (debug)Log(__FUNCTION__,"DEBUG","Start analyze: " +systname);

    label = GetLabel(e);
    string selection="None"; // level of selection applied

    if (label == "Other") Log(__FUNCTION__,"WARNING","Unable to associate label to file: "+e->GetName() );

    if (label.find("HiggsZG_Zbb") != string::npos) isSignal=true;

    /*
     * only odd or event events for signal
     */
    if (doEvenOnly){
        if ( isSignal )
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

    //e->ApplyTopReweight();


    /*
     * L1 PreFiring Map
     */

    //#warning NO_PREFIRE
    double l1prefire=1.0;
    if(year==2017)l1prefire=e->ApplyL1PreFire(2017); // apply to the event weight, but keep it for comp
    else if(year==2016)l1prefire=e->ApplyL1PreFire(2016); // apply to the event weight, but keep it for comp

    /*
     * HIGGS REWEIGHT -- UNCERTAINTIES
     */

    double nnlops = 1.0;
    if (not e->IsRealData()){

        if (VERBOSE)Log(__FUNCTION__,"DEBUG","Start HTXS Classification");

        HTXS::HiggsClassification  hc;
        STXS::HiggsClassification  hc2;
        if (label.find( "GluGlu_HiggsZG") != string::npos) {
            hc.prodMode = HTXS::GGF;
            hc2.prodMode = STXS::GGF;
        }
        else if (label.find( "VBF_HiggsZG") != string::npos) {
            hc.prodMode = HTXS::VBF;
            hc2.prodMode = STXS::VBF;
        }
        else if (label.find( "ZH_HiggsZG") != string::npos) {
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
        else if (label.find( "WMinusH_HiggsZG") != string::npos){ 
            hc.prodMode = HTXS::WH;
            hc2.prodMode = STXS::WH;
        }
        else if (label.find( "WPlusH_HiggsZG") != string::npos) {
            hc.prodMode = HTXS::WH;
            hc2.prodMode = STXS::WH;
        }
        else if (label.find( "ttH_HiggsZG") != string::npos){
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
            GenParticle* mu0T=NULL; //pho
            GenParticle* mu1T=NULL; //Z
            GenParticle* V = NULL;
            int idxV=-1;
            bool quarkDecayed=false;
            int Njets30=0;
            for( int ig=0;;++ig){
                GenParticle *g =  e->GetGenParticle(ig);
                if (g==NULL) break;
                if (abs(g->GetPdgId())==25 and abs(g->GetParentPdgId() ) != 25) Higgs=g;
                if (g->GetPdgId() == 22 and e->GenParticleDecayedFrom(ig,25)) mu0T = g;
                if (g->GetPdgId() == 23 and e->GenParticleDecayedFrom(ig,25)) mu1T = g;
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
#warning FIXME_GENJET_CLEANING
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
                e->ApplySF("nnlops");
                nnlops=sf_nnlops->get();

            }
            //if (doSTXS)
            //{
            //    labelSTXS=STXS::ENumToString(hc2.stage1p1_cat);
            //}
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
    
    // select jets
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","GetJets: ");

    //for(unsigned i=0;i<e->Njets() ; ++i)
    //{
    //    Jet *j=e->GetJet(i);
    //    if (selectedJets.size() == 0 and j->Pt() <30) continue;
    //    selectedJets.push_back(j);
    //}


    // TRIGGER SF. TODO 


    if (year==2016) {passTrigger=e->IsTriggered("HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200_v"); }

    // Objects : 4 jets, 1 photon
   
    // ***************************
    // **       preselection    **
    // ***************************
    
    pho = e->GetPhoton(0);
    if (pho == nullptr) return 1;

    // Selection: bJets
    selectedBJets.push_back( e->GetBjet(0) ) ;
    selectedBJets.push_back( e->GetBjet(1) ) ;
    if (selectedBJets[0] == nullptr or selectedBJets[1] == nullptr) return 1;

    // Selection: jets
    selectedJets.clear();
    for(int i=0;; ++i){
        Jet *j = e->GetJet(i); 
        if (j==nullptr) break;
        if (j==selectedBJets[0] or j==selectedBJets[1]) continue;
        if (j->DeltaR(pho) < 0.4) continue; // make sure. probably cleaning already done
        selectedJets.push_back(j);
    }
    
    if (selectedJets.size() <2) return 1;
    // ------------
    //
    updateEventVar();

    // -------------------------------------------------------
    //
    //
    if (fabs(eventVar_["mbb"]-91 )<30){
        kf->lambda_MET=1.; 
        kf->lambda_V=10.; 
        kf->cutWidth=31.;  // the Z value is more precise
        kf->doV=1; // Z

        kf->p4.push_back(selectedBJets[0]->GetP4()*selectedBJets[0]->GetBCorrection());
        kf->p4.push_back(selectedBJets[1]->GetP4()*selectedBJets[1]->GetBCorrection());
        kf->notV.push_back(0);
        kf->notV.push_back(0);
        kf->sigma.push_back(1.);
        kf->sigma.push_back(1.);

        for (Jet *j : selectedJets)
        {
            kf->p4.push_back(j->GetP4());
            kf->notV.push_back(1);
            kf->sigma.push_back(1.);
        }
        // photon
        kf->p4.push_back(pho->GetP4());
        kf->notV.push_back(1);
        kf->sigma.push_back(0.001);

        kf->run();
        //kf->print();
        //if (kf->alpha_out > 1.e9) return 1;// outside of 30GeV window for mbb
    }
    updateEventVar(); // update Variables with KF info. mbb and mass is with kf now
    if (doTree) updateTreeVar();

    if (isSignal)matchToGen();
    if (isSignal and debug) printDebugInfo();

    // 1 photon, 2b mbb in Z, 2j, mjj >250 
    if (pho->Pt() > 15 and fabs(pho->Eta())<=2.5 and fabs(eventVar_["mbb"]-91 )<30 and eventVar_["mjj"]>  250)
    {
        selection="PreTrigger"; // to study trigger turn on
        fillHists(selection,label,systname);

        if ( doTree and (systname == "" or systname =="NONE")) FillTree("hbbg");
        if (doTMVA) updateTmva();

        if (passTrigger) {
            selection="Preselection";
            fillHists(selection,label,systname);
            if ( fabs(eventVar_["mbb"]-91 )<30 and  //Z
                    eventVar_["mjj"]> 250  // VBF
                    // and eventVar_["detajj"]> 2. // VBF
               )
            {
                selection="Final";
                fillHists(selection,label,systname);

                if (bdt[0]> 0.85) Fill("HbbgAnalysis/Categories/mass_cat0_"+label,systname,mass_, e->weight());
                else if (bdt[0]> 0.5) Fill("HbbgAnalysis/Categories/mass_cat1_"+label,systname,mass_, e->weight());
                else Fill("HbbgAnalysis/Categories/mass_cat2_"+label,systname,mass_, e->weight());
            }
        } //Preselection
    }

    
    if (debug)Log(__FUNCTION__,"DEBUG","end Analyze");
    return 0;
}

void HbbgAnalysis::EndEvent(){}

void HbbgAnalysis::InitTmva(){
    Log(__FUNCTION__,"INFO","Init Reader");
    TMVA::Tools::Instance();
    for(size_t pos = 0; pos <weights.size() ;++pos)
    {
        readers_ . push_back( new TMVA::Reader() );
        // Variables
        AddTmvaVariable("pt",'F');
        AddTmvaVariable("dphijj",'F');
        AddTmvaVariable("detajj",'F');
        AddTmvaVariable("jet_pt[0]",'F');
        AddTmvaVariable("jet_pt[1]",'F');
        AddTmvaVariable("jet_eta[0]",'F');
        AddTmvaVariable("jet_eta[1]",'F');
        AddTmvaVariable("Alt$(jet_eta[2],-10)",'F');
        AddTmvaVariable("y_",'F');
        AddTmvaVariable("ht",'F');
        AddTmvaVariable("drbb",'F');
        AddTmvaVariable("dphibb",'F');
        AddTmvaVariable("dphibbg",'F');
        AddTmvaVariable("detabbg",'F');
        AddTmvaVariable("ptb1",'F');
        AddTmvaVariable("ptb2",'F');
        AddTmvaVariable("etab1",'F');
        AddTmvaVariable("etab2",'F');
        AddTmvaVariable("ptpho",'F');
        AddTmvaVariable("etapho",'F');

        AddTmvaSpectator("mass",'F');SetTmvaVariable("mass",125.);
        Log(__FUNCTION__,"INFO",Form("Loading weights idx=%d: ",pos)+weights[pos]);
        readers_[pos]->BookMVA("BDTG",weights[pos].c_str());
    }
}

void HbbgAnalysis::AddTmvaVariable( string name, char type){ 
    Log(__FUNCTION__,"INFO","Adding variable '"+name+"'");
    varValues_.Add(name,type); 
    if ( type == 'I') {
        int ir=-1;
        for(auto& r : readers_ ){ 
            ++ir;
            r -> AddVariable(name.c_str(),  (int*)varValues_.GetPointer(name));
        }
    }
    else if ( type == 'F') {
        int ir=-1;
        for(auto&r : readers_) { 
            ++ir;
            r -> AddVariable(name.c_str(),  (float*)varValues_.GetPointer(name));
        }}
    else { 
        Log(__FUNCTION__,"ERROR",string("type '") + type + "' not supported");
    }
    return;
} //end add variable

void HbbgAnalysis::AddTmvaSpectator( string name, char type){ 
    Log(__FUNCTION__,"INFO","Adding spectator '"+name+"'");
    varValues_.Add(name,type); 
    if ( type == 'I') for(auto& r : readers_ ) r -> AddSpectator(name.c_str(),  (int*)varValues_.GetPointer(name));
    else if ( type == 'F') for(auto&r : readers_) r -> AddSpectator(name.c_str(),  (float*)varValues_.GetPointer(name));
    else { 
        Log(__FUNCTION__,"ERROR",string("type '") + type + "' not supported");
    }
}//end add spectator





// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
