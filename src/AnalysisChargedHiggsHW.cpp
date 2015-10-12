#include "interface/AnalysisChargedHiggsHW.hpp"

using namespace TMath;

void ChargedHiggsHW::Init(){

    for (string l : AllLabel()){

        cout << "[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram CutFlow for VetoFirst. Sample: " << l << endl;
        Book(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l ).c_str(), ("CutFlow "+ l).c_str(), 28, -.5, 28-.5);
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(1, "Total");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(2, "1 light jets");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(3, "2 light jets");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(4, "1 lepton");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(5, "met > 20");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(6, "1 bjets");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(7, "2 bjets");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(8, "3 bjets");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(9, "W mass");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(10, "Top veto");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(11, "H mass");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(12, "Top mass");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(13, "Charged H mass");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(13, "Charged H mass 250");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(14, "Charged H mass 300");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(15, "Charged H mass 350");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(16, "Charged H mass 400");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(17, "Charged H mass 450");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(18, "Charged H mass 500");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(19, "Charged H mass 550");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(20, "Charged H mass 600");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(21, "Charged H mass 650");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(22, "Charged H mass 700");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(23, "Charged H mass 750");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(24, "Charged H mass 800");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(25, "Charged H mass 850");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(26, "Charged H mass 900");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(27, "Charged H mass 950");
        GetHisto(("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(28, "Charged H mass 1000");

        cout << "[ChargedHiggsHW]::[Init]::[INFO] Booking Histograms for variables for VetoFirst. Sample: " << l << endl;
        Book(("ChargedHiggsHW/VetoFirst/Vars/NJets_" + l).c_str(), ("Number of jets " + l).c_str(), 13, -.5, 12.5);
        Book(("ChargedHiggsHW/VetoFirst/Vars/NBJets_" + l).c_str(), ("Number of bjets " + l).c_str(), 6, -.5, 5.5);
        Book(("ChargedHiggsHW/VetoFirst/Vars/NLJets_" + l).c_str(), ("Number of light jets " + l).c_str(), 9, -.5, 8.5);
        Book(("ChargedHiggsHW/VetoFirst/Vars/NLeptons_" + l).c_str(), ("Number of leptons " + l).c_str(), 6, -.5, 5.5);
        Book(("ChargedHiggsHW/VetoFirst/Vars/Met_" + l).c_str(), ("MET "+ l).c_str(), 50, 20, 350);
        Book(("ChargedHiggsHW/VetoFirst/Vars/BJet1_Pt_" + l).c_str(), ("pT of leading bjet "+ l).c_str(), 50, 20, 500);
        Book(("ChargedHiggsHW/VetoFirst/Vars/BJet2_Pt_" + l).c_str(), ("pT of second leading bjet "+ l).c_str(), 50, 20, 300);
        Book(("ChargedHiggsHW/VetoFirst/Vars/BJet3_Pt_" + l).c_str(), ("pT of third leading bjet "+ l).c_str(), 50, 20, 200);
        Book(("ChargedHiggsHW/VetoFirst/Vars/LJet1_Pt_" + l).c_str(), ("pT of leading light jet "+ l).c_str(), 50, 20, 400);
        Book(("ChargedHiggsHW/VetoFirst/Vars/LJet2_Pt_" + l).c_str(), ("pT of second leading light jet "+ l).c_str(), 50, 20, 200);
        Book(("ChargedHiggsHW/VetoFirst/Vars/Lepton_Pt_" + l).c_str(), ("p_{T} of lepton "+ l).c_str(), 50, 20, 250);
        Book(("ChargedHiggsHW/VetoFirst/Vars/HadronicW_Mass_" + l).c_str(), ("M_{W} reconstructed from jets "+ l).c_str(), 50, 30, 130);
        Book(("ChargedHiggsHW/VetoFirst/Vars/HadronicW_Pt_" + l).c_str(), ("W p_{T} reconstructed from jets "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/WJets_dR_" + l).c_str(), ("dR(j1,j2) "+ l).c_str(), 50, 0, 6);
        Book(("ChargedHiggsHW/VetoFirst/Vars/GenHadronicW_Pt_" + l).c_str(), ("generated W p_{T} reconstructed from jets "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Mass_" + l).c_str(), ("M_{W} reconstructed from lepton+MET "+ l).c_str(), 50, 30, 130);
        Book(("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Pt_" + l).c_str(), ("W p_{T} reconstructed from lepton+MET "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/GenLeptonicW_Pt_" + l).c_str(), ("generated W p_{T} reconstructed from lepton+MET "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/H_Mass_" + l).c_str(), ("Higgs Mass "+ l).c_str(), 50, 75, 175);
        Book(("ChargedHiggsHW/VetoFirst/Vars/H_Pt_" + l).c_str(), ("Higgs p_{T} "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/HBJets_dR_" + l).c_str(), ("dR(b1,b2) "+ l).c_str(), 50, 0, 6);
        Book(("ChargedHiggsHW/VetoFirst/Vars/GenH_Pt_" + l).c_str(), ("generated Higgs p_{T} "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoFirst/Vars/TopVeto_" + l).c_str(), ("Max number of top candidates "+ l).c_str(), 3, -.5, 2.5);
        Book(("ChargedHiggsHW/VetoFirst/Vars/T_Mass_" + l).c_str(), ("Top mass "+ l).c_str(), 50, 75, 500);
        Book(("ChargedHiggsHW/VetoFirst/Vars/T_Pt_" + l).c_str(), ("Top p_{T} "+ l).c_str(), 50, 0, 600);
        Book(("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Mass_" + l).c_str(), ("Charged Higgs Mass "+ l).c_str(), 50, 100, 1300);
        Book(("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Pt_" + l).c_str(), ("Charged Higgs p_{T} "+ l).c_str(), 50, 0, 600);
        Book(("ChargedHiggsHW/VetoFirst/Vars/GenChargedH_Mass_" + l).c_str(), ("generated Charged Higgs Mass "+ l).c_str(), 50, 50, 1300);
        Book(("ChargedHiggsHW/VetoFirst/Vars/GenChargedH_Pt_" + l).c_str(), ("generated Charged Higgs p_{T} "+ l).c_str(), 50, 0, 600);

        cout << "[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram CutFlow for VetoSecond. Sample: " << l << endl;
        Book(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l ).c_str(), ("CutFlow "+ l).c_str(), 28, -.5, 28-.5);
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(1, "Total");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(2, "1 light jets");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(3, "2 light jets");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(4, "1 lepton");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(5, "met > 20");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(6, "1 bjets");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(7, "2 bjets");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(8, "3 bjets");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(9, "W mass");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(10, "H mass");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(11, "Top veto");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(12, "Top mass");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(13, "Charged H mass 250");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(14, "Charged H mass 300");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(15, "Charged H mass 350");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(16, "Charged H mass 400");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(17, "Charged H mass 450");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(18, "Charged H mass 500");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(19, "Charged H mass 550");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(20, "Charged H mass 600");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(21, "Charged H mass 650");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(22, "Charged H mass 700");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(23, "Charged H mass 750");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(24, "Charged H mass 800");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(25, "Charged H mass 850");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(26, "Charged H mass 900");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(27, "Charged H mass 950");
        GetHisto(("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + l).c_str(), "")->GetXaxis()->SetBinLabel(28, "Charged H mass 1000");

        cout << "[ChargedHiggsHW]::[Init]::[INFO] Booking Histograms for variables for VetoSecondSecondple: " << l << endl;
        Book(("ChargedHiggsHW/VetoSecond/Vars/NJets_" + l).c_str(), ("Number of jets " + l).c_str(), 13, -.5, 12.5);
        Book(("ChargedHiggsHW/VetoSecond/Vars/NBJets_" + l).c_str(), ("Number of bjets " + l).c_str(), 6, -.5, 5.5);
        Book(("ChargedHiggsHW/VetoSecond/Vars/NLJets_" + l).c_str(), ("Number of light jets " + l).c_str(), 9, -.5, 8.5);
        Book(("ChargedHiggsHW/VetoSecond/Vars/NLeptons_" + l).c_str(), ("Number of leptons " + l).c_str(), 6, -.5, 5.5);
        Book(("ChargedHiggsHW/VetoSecond/Vars/Met_" + l).c_str(), ("MET "+ l).c_str(), 50, 20, 350);
        Book(("ChargedHiggsHW/VetoSecond/Vars/BJet1_Pt_" + l).c_str(), ("pT of leading bjet "+ l).c_str(), 50, 20, 500);
        Book(("ChargedHiggsHW/VetoSecond/Vars/BJet2_Pt_" + l).c_str(), ("pT of second leading bjet "+ l).c_str(), 50, 20, 300);
        Book(("ChargedHiggsHW/VetoSecond/Vars/BJet3_Pt_" + l).c_str(), ("pT of third leading bjet "+ l).c_str(), 50, 20, 200);
        Book(("ChargedHiggsHW/VetoSecond/Vars/LJet1_Pt_" + l).c_str(), ("pT of leading light jet "+ l).c_str(), 50, 20, 400);
        Book(("ChargedHiggsHW/VetoSecond/Vars/LJet2_Pt_" + l).c_str(), ("pT of second leading light jet "+ l).c_str(), 50, 20, 200);
        Book(("ChargedHiggsHW/VetoSecond/Vars/Lepton_Pt_" + l).c_str(), ("p_{T} of lepton "+ l).c_str(), 50, 20, 250);
        Book(("ChargedHiggsHW/VetoSecond/Vars/HadronicW_Mass_" + l).c_str(), ("M_{W} reconstructed from jets "+ l).c_str(), 50, 30, 130);
        Book(("ChargedHiggsHW/VetoSecond/Vars/HadronicW_Pt_" + l).c_str(), ("W p_{T} reconstructed from jets "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/WJets_dR_" + l).c_str(), ("dR(j1,j2) "+ l).c_str(), 50, 0, 6);
        Book(("ChargedHiggsHW/VetoSecond/Vars/GenHadronicW_Pt_" + l).c_str(), ("generated W p_{T} reconstructed from jets "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Mass_" + l).c_str(), ("M_{W} reconstructed from lepton+MET "+ l).c_str(), 50, 30, 130);
        Book(("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Pt_" + l).c_str(), ("W p_{T} reconstructed from lepton+MET "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/GenLeptonicW_Pt_" + l).c_str(), ("generated W p_{T} reconstructed from lepton+MET "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/H_Mass_" + l).c_str(), ("Higgs Mass "+ l).c_str(), 50, 75, 175);
        Book(("ChargedHiggsHW/VetoSecond/Vars/H_Pt_" + l).c_str(), ("Higgs p_{T} "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/HBJets_dR_" + l).c_str(), ("dR(b1,b2) "+ l).c_str(), 50, 0, 6);
        Book(("ChargedHiggsHW/VetoSecond/Vars/GenH_Pt_" + l).c_str(), ("generated Higgs p_{T} "+ l).c_str(), 50, 0, 700);
        Book(("ChargedHiggsHW/VetoSecond/Vars/TopVeto_" + l).c_str(), ("Max number of top candidates "+ l).c_str(), 3, -.5, 2.5);
        Book(("ChargedHiggsHW/VetoSecond/Vars/T_Mass_" + l).c_str(), ("Top mass "+ l).c_str(), 50, 75, 500);
        Book(("ChargedHiggsHW/VetoSecond/Vars/T_Pt_" + l).c_str(), ("Top p_{T} "+ l).c_str(), 50, 0, 600);
        Book(("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Mass_" + l).c_str(), ("Charged Higgs Mass "+ l).c_str(), 50, 100, 1300);
        Book(("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Pt_" + l).c_str(), ("Charged Higgs p_{T} "+ l).c_str(), 50, 0, 600);
        Book(("ChargedHiggsHW/VetoSecond/Vars/GenChargedH_Mass_" + l).c_str(), ("generated Charged Higgs Mass "+ l).c_str(), 50, 50, 1300);
        Book(("ChargedHiggsHW/VetoSecond/Vars/GenChargedH_Pt_" + l).c_str(), ("generated Charged Higgs p_{T} "+ l).c_str(), 50, 0, 600);

    }

}

int ChargedHiggsHW::analyze(Event *e, string systname){

    Int_t vetoFirst = VetoFirst(e, systname); 
    Int_t vetoSecond = VetoSecond(e, systname);

    return ((vetoFirst || vetoSecond) ? EVENT_USED : EVENT_NOT_USED);

}

int ChargedHiggsHW::VetoFirst(Event *e,string systname){

#ifdef VERBOSE
    if(VERBOSE > 0) cout << "[ChargedHiggsHW]::[VetoFirst]::[DEBUG] analyze event with VetoFirst and syst: " << systname << endl;
#endif

    string label = GetLabel(e);

    if(e->weight() == 0.) cout << "[ChargedHiggsHW]::[VetoFirst]::[INFO] Even Weight is NULL !!" << endl;

    const Double_t DR = 0., matchDR = 0.4; //We need to check whether we should require > 0.4

    //Define objects and their 4-momentum vectors
    vector<Jet*> orderedLJets, orderedBJets;
    vector<Lepton*> orderedLeptons;
    Jet *bjetH1 = 0, *bjetH2 = 0, *jetW1 = 0, *jetW2 = 0, *bjetT = 0;
    Lepton *leptonW = 0;
    GenParticle *genW1 = 0, *genW2 = 0, *genH = 0, *genCH = 0;
    Met met;

    TLorentzVector vLeptonW, vMet, vNeutrino, vW1, vW2Sol1, vW2Sol2, vjetW1, vjetW2, vH, vbjetT, vHW1, vHW2, vbjetH1, vbjetH2, vT;

    //Set up MET
    met = e->GetMet();
    vMet.SetPtEtaPhiE(met.Pt(), met.Eta(), met.Phi(), met.E());

    // Find gen-level particles
    for(Int_t i = 0; i < e->NGenPar(); i++){
        //FIXME: logic ?!?
        GenParticle *genpar = e->GetGenParticle(i);

        if(abs(genpar->GetPdgId()) == 37){
            if(!genCH) genCH = genpar;
            else if(genpar->DeltaR(*genCH) < DR) genCH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 25){
            if(!genH) genH = genpar;
            else if(genpar->DeltaR(*genH) < DR) genH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 24){
            if(!genW1) genW1 = genpar;
            else if( genpar->DeltaR(*genW1)< DR) genW1 = genpar;
            else if(genW1 && !genW2) genW2 = genpar;
            else if(genW1 && genpar->DeltaR(*genW2)< DR) genW2 = genpar;
        }

    }

    //Fill vectors for easier jet manipulation
    for(Int_t ijet = 0; ijet < e->Njets(); ijet++) {

        Jet *tempjet = e->GetJet(ijet);

        if(tempjet == NULL) break;
            
        //I might need to ignore jets close to isolated leptons or photons here
                
        if(tempjet->IsBJet()) orderedBJets.push_back(tempjet);
                    
        else orderedLJets.push_back(tempjet);

    }

    //Fill vector for easier lepton manipulation
    for(Int_t ilepton = 0; ilepton < e->Nleps(); ilepton++) {

        Lepton *templepton = e->GetLepton(ilepton);

        if(templepton == NULL) break;
            
        //I might need to ignore leptons close to high-pt jets
                
        orderedLeptons.push_back(templepton); 

    }

    //Fill histograms of basic variables
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 0, e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/NLJets_" + label, systname, e->Njets() - e->Bjets(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/NJets_" + label, systname, e->Njets(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/NBJets_" + label, systname, e->Bjets(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/NLeptons_" + label, systname, e->Nleps(), e->weight());

    //Apply first set of requirements (without the distance requirement)
    if(orderedLJets.size() < 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 1, e->weight());
    if(orderedLJets.size() < 2) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 2, e->weight());
    if(orderedLeptons.size() != 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 3, e->weight());
    if(met.E() < 20) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 4, e->weight());
    if(orderedBJets.size() < 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 5, e->weight());
    if(orderedBJets.size() < 2) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 6, e->weight());
    if(orderedBJets.size() < 3) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 7, e->weight());

    //Fill histograms of basic kinematics
    Fill("ChargedHiggsHW/VetoFirst/Vars/BJet1_Pt_" + label, systname, orderedBJets.at(0)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/BJet2_Pt_" + label, systname, orderedBJets.at(1)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/BJet3_Pt_" + label, systname, orderedBJets.at(2)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/LJet1_Pt_" + label, systname, orderedLJets.at(0)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/LJet2_Pt_" + label, systname, orderedLJets.at(1)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/Lepton_Pt_" + label, systname, orderedLeptons.at(0)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/Met_" + label, systname, met.E(), e->weight());

    //Select two jets with reconstructed mass closest to the W
    Double_t minDeltaMW = 999;
    for(Int_t ijet = 0; ijet < orderedLJets.size(); ijet++){

        for(Int_t jjet = ijet + 1; jjet < orderedLJets.size(); jjet++){

            Jet *tempjet1 = orderedLJets.at(ijet), *tempjet2 = orderedLJets.at(jjet);
            TLorentzVector vWtemp;

            vWtemp = tempjet1 -> GetP4() + tempjet2 -> GetP4();

            //if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;
            if(tempjet1 -> DeltaR (tempjet2) < DR) continue;

            Double_t tempDeltaM = fabs(vWtemp.M() - Mw);

            if(tempDeltaM < minDeltaMW){
                minDeltaMW = tempDeltaM;
                jetW1 = tempjet1;
                jetW2 = tempjet2;
            }

        }

    }

    if(!jetW1 || !jetW2) return EVENT_NOT_USED;

    //Set up W 4-momentum vector
    vjetW1.SetPtEtaPhiE(jetW1->Pt(), jetW1->Eta(), jetW1->Phi(), jetW1->E());
    vjetW2.SetPtEtaPhiE(jetW2->Pt(), jetW2->Eta(), jetW2->Phi(), jetW2->E());
    vW1 = vjetW1 + vjetW2;

    //Fill W mass histogram
    Fill("ChargedHiggsHW/VetoFirst/Vars/HadronicW_Mass_" + label, systname, vW1.M(), e->weight());

    //Apply second cut
    if(minDeltaMW > 30) return EVENT_NOT_USED;

    //Fill more W histograms
    Fill("ChargedHiggsHW/VetoFirst/Vars/HadronicW_Pt_" + label, systname, vW1.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/WJets_dR_" + label, systname, jetW1 -> DeltaR ( jetW2) , e->weight());

    if(genW1 && genW2){
        if( vW1 . DeltaR ( genW1 -> GetP4() ) < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenHadronicW_Pt_" + label, systname, genW1->Pt(), e->weight());
        else if(vW1 . DeltaR ( genW2->GetP4() )  < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenHadronicW_Pt_" + label, systname, genW2->Pt(), e->weight());
    }

    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 8 , e->weight());

    //Find longitudinal neutrino momentum
    Double_t pNuZSol1, pNuZSol2;
    leptonW = orderedLeptons.at(0);
    vLeptonW.SetPtEtaPhiE(leptonW->Pt(), leptonW->Eta(), leptonW->Phi(), leptonW->E());
    pNuZSol1 = neutrinoPz(vLeptonW, vMet, 0);
    pNuZSol2 = neutrinoPz(vLeptonW, vMet, 1);

    //Reconstruct Ws and fill histograms
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol1, Sqrt(vMet.E()*vMet.E() + pNuZSol1*pNuZSol1));
    vW2Sol1 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Pt_" + label, systname, vW2Sol1.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Mass_" + label, systname, vW2Sol1.M(), e->weight());
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol2, Sqrt(vMet.E()*vMet.E() + pNuZSol2*pNuZSol2));
    vW2Sol2 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Pt_" + label, systname, vW2Sol2.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/LeptonicW_Mass_" + label, systname, vW2Sol2.M(), e->weight());

    if(genW1 && genW2){
        if( vW2Sol1 . DeltaR ( genW1 ->GetP4() ) < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenLeptonicW_Pt_" + label, systname, genW1->Pt(), e->weight());
        else if( vW2Sol2.DeltaR( genW1 ->GetP4() )  < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenLeptonicW_Pt_" + label, systname, genW1->Pt(), e->weight());
        else if( vW2Sol1.DeltaR (genW2->GetP4()) < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenLeptonicW_Pt_" + label, systname, genW2->Pt(), e->weight());
        else if( vW2Sol2 . DeltaR (genW2->GetP4()) < matchDR) Fill("ChargedHiggsHW/VetoFirst/Vars/GenLeptonicW_Pt_" + label, systname, genW2->Pt(), e->weight());
    }

    //Maybe here we can apply a mass cut for the leptonic Ws

    //Put all jets into one vector (excluding the jets from the W)
    vector<Jet*> allJets;
    for(Int_t x = 0; x < orderedLJets.size(); x++){
        Jet *tempjet = orderedLJets.at(x);
        if(tempjet == jetW1 || tempjet == jetW2) continue;
        allJets.push_back(tempjet);
    }
    for(Int_t x = 0; x < orderedBJets.size(); x++){
        Jet *tempjet = orderedBJets.at(x);
        allJets.push_back(tempjet);
    }

    //Top veto
    Int_t topCandidates = 0;
    TLorentzVector vT1temp, vT21temp, vT22temp;
    bool passedTopVeto = true;
    for(Int_t ijet = 0; ijet < allJets.size(); ijet++){

        for(Int_t jjet = 0; jjet < allJets.size(); jjet++){

            if(ijet == jjet) continue;

            //FIXME: remove the tlorentz vectors
            Jet *tempjet1 = allJets.at(ijet), *tempjet2 = allJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            if(vtemp1 . DeltaR(vtemp2) < DR) continue;
            if(vtemp1 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(leptonW->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(leptonW->GetP4()) < DR) continue;

            vT1temp = vtemp1 + vW1;
            vT21temp = vtemp2 + vW2Sol1;
            vT22temp = vtemp2 + vW2Sol2;

            Double_t tempDeltaM1 = fabs(vT1temp.M() - Mtop);
            Double_t tempDeltaM21 = fabs(vT21temp.M() - Mtop);
            Double_t tempDeltaM22 = fabs(vT22temp.M() - Mtop);

            if(topCandidates < 2 && (tempDeltaM1 < 20 || tempDeltaM21 < 20 || tempDeltaM22 < 20)) topCandidates = 1;

            if(tempDeltaM1 < 20 && (tempDeltaM21 < 20 || tempDeltaM22 < 20)){
                topCandidates = 2;
                passedTopVeto = false;
            }

        }

    }

    //Fill Top Veto histogram
    Fill("ChargedHiggsHW/VetoFirst/Vars/TopVeto_" + label, systname, topCandidates, e->weight());

    //Apply sixth cut (top veto)
    if(!passedTopVeto) return EVENT_NOT_USED;

    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 9, e->weight());

    //Select two bjets with mass closest to the Higgs
    Double_t minDeltaMH = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedBJets.size(); jjet++){
            
            // FIXME: remove tlorentzvectors. seems the same of a loop before
            Jet *tempjet1 = orderedBJets.at(ijet), *tempjet2 = orderedBJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            vHtemp = vtemp1 + vtemp2;

            if(vtemp1 . DeltaR(vtemp2) < DR) continue;
            if(vtemp1 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(leptonW->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(leptonW->GetP4()) < DR) continue;

            Double_t tempDeltaM = fabs(vHtemp.M() - Mh);

            if(tempDeltaM < minDeltaMH){
                minDeltaMH = tempDeltaM;
                bjetH1 = tempjet1;
                bjetH2 = tempjet2;
            }

        }

    }

    if(!bjetH1 || !bjetH2) return EVENT_NOT_USED;

    //Set up Higgs 4-momentum vector
    vbjetH1.SetPtEtaPhiE(bjetH1->Pt(), bjetH1->Eta(), bjetH1->Phi(), bjetH1->E());
    vbjetH2.SetPtEtaPhiE(bjetH2->Pt(), bjetH2->Eta(), bjetH2->Phi(), bjetH2->E());
    vH = vbjetH1 + vbjetH2;

    //Fill Higgs mass histogram
    Fill("ChargedHiggsHW/VetoFirst/Vars/H_Mass_" + label, systname, vH.M(), e->weight());

    //Apply fifth cut
    if(minDeltaMH > 15) return EVENT_NOT_USED;

    //Fill other Higgs histograms
    if(genH) Fill("ChargedHiggsHW/VetoFirst/Vars/GenH_Pt_" + label, systname, genH->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/H_Pt_" + label, systname, vH.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/HBJets_dR_" + label, systname, bjetH1 -> DeltaR(bjetH2) , e->weight());
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 10, e->weight()); // FIXME, put all of the cut in a enum

    Jet *tempT1 = 0, *tempT21 = 0, *tempT22 = 0;
    Double_t minDeltaMT1 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT1){
            minDeltaMT1 = tempDeltaM;
            tempT1 = tempjet;
        }
    }

    Double_t minDeltaMT21 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT21){
            minDeltaMT21 = tempDeltaM;
            tempT21 = tempjet;
        }
    }

    Double_t minDeltaMT22 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol2 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT22){
            minDeltaMT22 = tempDeltaM;
            tempT22 = tempjet;
        }
    }

    //Check whether at least one bjet was used
    if(!tempT1 && !tempT21 && !tempT22) return EVENT_NOT_USED;

    //Select the bjet used for the mass closest to the top
    if(minDeltaMT21 < minDeltaMT1 && minDeltaMT21 < minDeltaMT22) bjetT = tempT21;
    else if(minDeltaMT22 < minDeltaMT1 && minDeltaMT22 < minDeltaMT21) bjetT = tempT22;
    else bjetT = tempT1;

    if(!bjetT) return EVENT_NOT_USED;

    //Set up bjet 4-momentum vector
    vbjetT.SetPtEtaPhiE(bjetT->Pt(), bjetT->Eta(), bjetT->Phi(), bjetT->E());

    //Set up top 4-momentum vector
    if(bjetT == tempT21) vT = vW2Sol1 + vbjetT;
    else if(bjetT == tempT22) vT = vW2Sol2 + vbjetT;
    else vT = vW1 + vbjetT;

    //Fill top mass histogram
    Fill("ChargedHiggsHW/VetoFirst/Vars/T_Mass_" + label, systname, vT.M(), e->weight());

    //Apply seventh requirement
    if(minDeltaMT1 > 30 && minDeltaMT21 > 30 && minDeltaMT22 > 30) return EVENT_NOT_USED;

    //Fill other top histograms
    Fill("ChargedHiggsHW/VetoFirst/Vars/T_Pt_" + label, systname, vT.Pt() , e->weight());
    Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 11, e->weight());

    //Set up Charged Higgs 4-momentum vector
    if(bjetT == tempT1){vHW1 = vW2Sol1 + vH; vHW2 = vW2Sol2 + vH;}
    else vHW1 = vHW2 = vW1 + vH;

    //Fill Charged Higgs histograms
    Fill("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Mass_" + label, systname, vHW1.M(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Mass_" + label, systname, vHW2.M(), e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Pt_" + label, systname, vHW1.Pt(),  e->weight());
    Fill("ChargedHiggsHW/VetoFirst/Vars/ChargedH_Pt_" + label, systname, vHW2.Pt(),  e->weight());

    if(genCH){
        TLorentzVector tempv;
        tempv.SetPtEtaPhiE(genCH->Pt(), genCH->Eta(), genCH->Phi(), genCH->E());
        Fill("ChargedHiggsHW/VetoFirst/Vars/GenChargedH_Mass_" + label, systname, tempv.M(), e->weight());
        Fill("ChargedHiggsHW/VetoFirst/Vars/GenChargedH_Pt_" + label, systname, genCH->Pt(), e->weight());
    }

    //Here we can apply a cut on the mass of the charged Higgs
    //FIXME ... do you need a TH2D histogram?
    if(vHW1.M() > 250) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 12, e->weight());
    if(vHW1.M() > 300) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 13, e->weight());
    if(vHW1.M() > 350) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 14, e->weight());
    if(vHW1.M() > 400) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 15, e->weight());
    if(vHW1.M() > 450) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 16, e->weight());
    if(vHW1.M() > 500) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 17, e->weight());
    if(vHW1.M() > 550) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 18, e->weight());
    if(vHW1.M() > 600) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 19, e->weight());
    if(vHW1.M() > 650) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 20, e->weight());
    if(vHW1.M() > 700) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 21, e->weight());
    if(vHW1.M() > 750) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 22, e->weight());
    if(vHW1.M() > 800) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 23, e->weight());
    if(vHW1.M() > 850) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 24, e->weight());
    if(vHW1.M() > 900) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 25, e->weight());
    if(vHW1.M() > 950) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 26, e->weight());
    if(vHW1.M() > 1000) Fill("ChargedHiggsHW/VetoFirst/CutFlow/CutFlow_" + label, systname, 27, e->weight());

    return EVENT_USED;

}

int ChargedHiggsHW::VetoSecond(Event *e, string systname){

#ifdef VERBOSE
    if(VERBOSE > 0) cout << "[ChargedHiggsHW]::[VetoSecond]::[DEBUG] analyze event with VetoSecond and syst: " << systname << endl;
#endif

    string label = GetLabel(e);

    if(e->weight() == 0.) cout << "[ChargedHiggsHW]::[VetoSecond]::[INFO] Even Weight is NULL !!" << endl;

    const Double_t DR = 0., matchDR = 0.4; //We need to check whether we should require > 0.4

    //Define objects and their 4-momentum vectors
    vector<Jet*> orderedLJets, orderedBJets;
    vector<Lepton*> orderedLeptons;
    Jet *bjetH1 = 0, *bjetH2 = 0, *jetW1 = 0, *jetW2 = 0, *bjetT = 0;
    Lepton *leptonW = 0;
    GenParticle *genW1 = 0, *genW2 = 0, *genH = 0, *genCH = 0;
    Met met;
    TLorentzVector vLeptonW, vMet, vNeutrino, vW1, vW2Sol1, vW2Sol2, vjetW1, vjetW2, vH, vbjetT, vHW1, vHW2, vbjetH1, vbjetH2, vT;

    //Set up MET
    met = e->GetMet();
    vMet.SetPtEtaPhiE(met.Pt(), met.Eta(), met.Phi(), met.E());

    // Find gen-level particles
    for(Int_t i = 0; i < e->NGenPar(); i++){

        GenParticle *genpar = e->GetGenParticle(i);

        if(abs(genpar->GetPdgId()) == 37){
            if(!genCH) genCH = genpar;
            else if( genpar -> DeltaR(genCH) < DR) genCH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 25){
            if(!genH) genH = genpar;
            else if( genpar->DeltaR(genH) < DR) genH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 24){
            if(!genW1) genW1 = genpar;
            else if( genpar -> DeltaR(genW1) < DR) genW1 = genpar;
            else if(genW1 && !genW2) genW2 = genpar;
            else if(genW1 && genpar-> DeltaR(genW2) < DR) genW2 = genpar;
        }

    }

    //Fill vectors for easier jet manipulation
    // --- for(Int_t ijet = 0; ijet < e->Njets(); ijet++) {

    // ---     Jet *tempjet = e->GetJet(ijet);

    // ---     if(tempjet == NULL) break;
    // ---         
    // ---     //I might need to ignore jets close to isolated leptons or photons here
    // ---             
    // ---     if(tempjet->IsBJet()) orderedBJets.push_back(tempjet);
    // ---                 
    // ---     else orderedLJets.push_back(tempjet);

    // --- }

    //Fill vector for easier lepton manipulation
    // --- for(Int_t ilepton = 0; ilepton < e->Nleps(); ilepton++) {

    // ---     Lepton *templepton = e->GetLepton(ilepton);

    // ---     if(templepton == NULL) break;
    // ---         
    // ---     //I might need to ignore leptons close to high-pt jets
    // ---             
    // ---     orderedLeptons.push_back(templepton); 

    // --- }

    //Fill histograms of basic variables
    int nLJets = e->Njets() - e->Bjets();
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 0          , e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/NLJets_" + label    , systname, nLJets     , e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/NJets_" + label     , systname, e->Njets() , e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/NBJets_" + label    , systname, e->Bjets() , e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/NLeptons_" + label  , systname, e->Nleps() , e->weight() );

    //Apply first set of requirements (without the distance requirement)
    if( nLJets < 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 1, e->weight() );
    if( nLJets < 2) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 2, e->weight() );
    if(orderedLeptons.size() != 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 3, e->weight() );
    if(met.Pt() < 20) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 4, e->weight() );
    if( e->Bjets() < 1) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 5, e->weight() );
    if( e->Bjets() < 2) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 6, e->weight() );
    if( e->Bjets() < 3) return EVENT_NOT_USED;
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 7, e->weight() );

    //Fill histograms of basic kinematics
    Fill("ChargedHiggsHW/VetoSecond/Vars/BJet1_Pt_" + label, systname, e->GetBjet(0)->Pt(), e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/BJet2_Pt_" + label, systname, e->GetBjet(1)->Pt(), e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/BJet3_Pt_" + label, systname, e->GetBjet(2)->Pt(), e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/Vars/LJet1_Pt_" + label, systname, e->GetLjet(0)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/LJet2_Pt_" + label, systname, e->GetLjet(1)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/Lepton_Pt_" + label, systname, e->GetLepton(0)->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/Met_" + label, systname, met.Pt(), e->weight());

    //Select two jets with reconstructed mass closest to the W
    Double_t minDeltaMW = 1.e6;
    for(Int_t ijet = 0; ijet < nLJets; ijet++){

        for(Int_t jjet = ijet + 1; jjet < nLJets; jjet++){
            Jet *tempjet1 = e->GetLjet(ijet), *tempjet2 = e->GetLjet(jjet);

            if (tempjet1 -> DeltaR ( *tempjet2 ) < DR ) continue;

            TLorentzVector  vWtemp;

            vWtemp = tempjet1->GetP4() + tempjet2->GetP4();

            float tempDeltaM = fabs(vWtemp.M() - Mw);

            if(tempDeltaM < minDeltaMW){
                minDeltaMW = tempDeltaM;
                jetW1 = tempjet1;
                jetW2 = tempjet2;
            }

        } // end first loop

    } // end second loop

    if(!jetW1 || !jetW2) {
        cout <<"[AnalysisChargedHiggsHW]::[analyze]::[WARNING] didn't find two light-jets with mass close enough to Mw. It's impossible."<<endl;
        return EVENT_NOT_USED;
    }

    //Set up W 4-momentum vector
    vW1 = jetW1->GetP4() + jetW2->GetP4();

    //Fill W mass histogram
    Fill("ChargedHiggsHW/VetoSecond/Vars/HadronicW_Mass_" + label, systname, vW1.M(), e->weight());

    //Apply second cut
    if(minDeltaMW > 30) return EVENT_NOT_USED;

    //Fill more W histograms
    Fill("ChargedHiggsHW/VetoSecond/Vars/HadronicW_Pt_" + label, systname, vW1.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/WJets_dR_" + label, systname, jetW1->DeltaR( *jetW2) , e->weight());

    if(genW1 && genW2){
        if(  genW1->GetP4().DeltaR(vW1) < matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenHadronicW_Pt_" + label, systname, genW1->Pt(), e->weight());
        else if( genW2->GetP4().DeltaR(vW1) < matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenHadronicW_Pt_" + label, systname, genW2->Pt(), e->weight());
    }

    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 8 , e->weight() );

    //Find longitudinal neutrino momentum
    Double_t pNuZSol1, pNuZSol2;
    leptonW = e->GetLepton(0);
    vLeptonW.SetPtEtaPhiE( leptonW->Pt(), leptonW->Eta(), leptonW->Phi(), leptonW->E());
    pNuZSol1 = neutrinoPz( vLeptonW, vMet, 0);
    pNuZSol2 = neutrinoPz( vLeptonW, vMet, 1);

    //Reconstruct Ws and fill histograms
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol1, Sqrt(vMet.E()*vMet.E() + pNuZSol1*pNuZSol1));
    vW2Sol1 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Pt_" + label, systname, vW2Sol1.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Mass_" + label, systname, vW2Sol1.M(), e->weight());
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol2, Sqrt(vMet.E()*vMet.E() + pNuZSol2*pNuZSol2));
    vW2Sol2 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Pt_" + label, systname, vW2Sol2.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/LeptonicW_Mass_" + label, systname, vW2Sol2.M(), e->weight());

    if(genW1 && genW2){
        if(  genW1->GetP4().DeltaR(vW2Sol1) < matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenLeptonicW_Pt_" + label, systname, genW1->Pt(), e->weight() );
        else if( genW1->GetP4().DeltaR( vW2Sol2) < matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenLeptonicW_Pt_" + label, systname, genW1->Pt(), e->weight() );
        else if( genW2->GetP4().DeltaR( vW2Sol1 )< matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenLeptonicW_Pt_" + label, systname, genW2->Pt(), e->weight() );
        else if( genW2->GetP4().DeltaR( vW2Sol2) < matchDR) Fill("ChargedHiggsHW/VetoSecond/Vars/GenLeptonicW_Pt_" + label, systname, genW2->Pt(), e->weight());
    }

    //Maybe here we can apply a mass cut for the leptonic Ws

    /// FIXME, this is identical to the one above: template it!
    //Select two bjets with mass closest to the Higgs
    Double_t minDeltaMH = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedBJets.size(); jjet++){

            Jet *tempjet1 = e->GetBjet(ijet), *tempjet2 = e->GetBjet(jjet);

            if ( tempjet1 == jetW1 ) continue;
            if ( tempjet1 == jetW2 ) continue;
            if ( tempjet2 == jetW1 ) continue;
            if ( tempjet2 == jetW2 ) continue;
            // FIXME: this check should be alreday partially or completely be implemented in the fw
            if(  tempjet1->DeltaR( *tempjet2 ) < DR) continue;
            if(  tempjet1->DeltaR( *jetW1 ) < DR) continue;
            if(  tempjet1->DeltaR( *jetW2 ) < DR) continue;
            if(  tempjet1->DeltaR( *leptonW ) < DR) continue;
            if(  tempjet2->DeltaR( *jetW1) < DR) continue;
            if(  tempjet2->DeltaR( *jetW2) < DR) continue;
            if(  tempjet2->DeltaR( *leptonW) < DR) continue;

            // FIXME, you can use Object::InvariantMass 
            TLorentzVector vHtemp;
            vHtemp = tempjet1->GetP4() + tempjet2->GetP4();
            Double_t tempDeltaM = fabs(vHtemp.M() - Mh);

            if(tempDeltaM < minDeltaMH){
                minDeltaMH = tempDeltaM;
                bjetH1 = tempjet1;
                bjetH2 = tempjet2;
            }

        }

    }

    if(!bjetH1 || !bjetH2) return EVENT_NOT_USED;

    //Set up Higgs 4-momentum vector
    vbjetH1.SetPtEtaPhiE(bjetH1->Pt(), bjetH1->Eta(), bjetH1->Phi(), bjetH1->E());
    vbjetH2.SetPtEtaPhiE(bjetH2->Pt(), bjetH2->Eta(), bjetH2->Phi(), bjetH2->E());
    vH = bjetH1->GetP4() + bjetH2->GetP4();

    //Fill Higgs mass histogram
    Fill("ChargedHiggsHW/VetoSecond/Vars/H_Mass_" + label, systname, vH.M(), e->weight());

    //Apply fifth cut
    if(minDeltaMH > 30) return EVENT_NOT_USED;

    //Fill other Higgs histograms
    if(genH) Fill("ChargedHiggsHW/VetoSecond/Vars/GenH_Pt_" + label, systname, genH->Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/H_Pt_" + label, systname, vH.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/HBJets_dR_" + label, systname, bjetH1->DeltaR( *bjetH2), e->weight() );
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 9, e->weight());

    // FIXME ----- amarini, I was here ABCD
    //Put all jets into one vector (excluding the jets from the W and H)
    vector<Jet*> allJets;
    for(Int_t x = 0; x < e->Njets(); x++){
        Jet *tempjet = e->GetJet(x);
        if(tempjet == jetW1 || tempjet == jetW2) continue;
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;
        allJets.push_back(tempjet);
    }

    //Top veto
    Int_t topCandidates = 0;
    TLorentzVector vT1temp, vT21temp, vT22temp;
    bool passedTopVeto = true;
    for(Int_t ijet = 0; ijet < allJets.size(); ijet++){

        for(Int_t jjet = 0; jjet < allJets.size(); jjet++){

            if(ijet == jjet) continue;
            //FIXME, an ether identical jet loop. Template and functions are adviced

            Jet *tempjet1 = allJets.at(ijet), *tempjet2 = allJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            if(vtemp1 . DeltaR(vtemp2) < DR) continue;
            if(vtemp1 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp1 . DeltaR(leptonW->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW1->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(jetW2->GetP4()) < DR) continue;
            if(vtemp2 . DeltaR(leptonW->GetP4()) < DR) continue;

            vT1temp  = tempjet1 -> GetP4() + vW1;
            vT21temp = tempjet2 -> GetP4() + vW2Sol1;
            vT22temp = tempjet2 -> GetP4() + vW2Sol2;

            Double_t tempDeltaM1 = fabs(vT1temp.M() - Mtop );
            Double_t tempDeltaM21 = fabs(vT21temp.M() - Mtop);
            Double_t tempDeltaM22 = fabs(vT22temp.M() - Mtop);

            if(topCandidates < 2 && (tempDeltaM1 < 20 || tempDeltaM21 < 20 || tempDeltaM22 < 20)) topCandidates = 1;

            if(tempDeltaM1 < 20 && (tempDeltaM21 < 20 || tempDeltaM22 < 20)){
                topCandidates = 2;
                passedTopVeto = false;
            }

        }

    }

    //Fill Top Veto histogram
    Fill("ChargedHiggsHW/VetoSecond/Vars/TopVeto_" + label, systname, topCandidates, e -> weight() );

    //Apply sixth cut (top veto)
    if(!passedTopVeto) return EVENT_NOT_USED;

    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 10, e-> weight());

    Jet *tempT1 = 0, *tempT21 = 0, *tempT22 = 0;
    Double_t minDeltaMT1 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT1){
            minDeltaMT1 = tempDeltaM;
            tempT1 = tempjet;
        }
    }

    Double_t minDeltaMT21 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT21){
            minDeltaMT21 = tempDeltaM;
            tempT21 = tempjet;
        }
    }

    Double_t minDeltaMT22 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol2 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - Mtop);

        if(tempDeltaM < minDeltaMT22){
            minDeltaMT22 = tempDeltaM;
            tempT22 = tempjet;
        }
    }

    //Check whether at least one bjet was used
    if(!tempT1 && !tempT21 && !tempT22) return EVENT_NOT_USED;

    //Select the bjet used for the mass closest to the top
    if(minDeltaMT21 < minDeltaMT1 && minDeltaMT21 < minDeltaMT22) bjetT = tempT21;
    else if(minDeltaMT22 < minDeltaMT1 && minDeltaMT22 < minDeltaMT21) bjetT = tempT22;
    else bjetT = tempT1;

    if(!bjetT) return EVENT_NOT_USED;

    //Set up bjet 4-momentum vector
    vbjetT.SetPtEtaPhiE(bjetT->Pt(), bjetT->Eta(), bjetT->Phi(), bjetT->E());

    //Set up top 4-momentum vector
    if(bjetT == tempT21) vT = vW2Sol1 + vbjetT;
    else if(bjetT == tempT22) vT = vW2Sol2 + vbjetT;
    else vT = vW1 + vbjetT;

    //Fill top mass histogram
    Fill("ChargedHiggsHW/VetoSecond/Vars/T_Mass_" + label, systname, vT.M(), e->weight());

    //Apply seventh requirement
    if(minDeltaMT1 > 30 && minDeltaMT21 > 30 && minDeltaMT22 > 30) return EVENT_NOT_USED;

    //Fill other top histograms
    Fill("ChargedHiggsHW/VetoSecond/Vars/T_Pt_" + label, systname, vT.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 11, e->weight());

    //Set up Charged Higgs 4-momentum vector
    if(bjetT == tempT1){vHW1 = vW2Sol1 + vH; vHW2 = vW2Sol2 + vH;}
    else vHW1 = vHW2 = vW1 + vH;

    //Fill Charged Higgs histograms
    Fill("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Mass_" + label, systname, vHW1.M(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Mass_" + label, systname, vHW2.M(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Pt_" + label, systname, vHW1.Pt(), e->weight());
    Fill("ChargedHiggsHW/VetoSecond/Vars/ChargedH_Pt_" + label, systname, vHW2.Pt(), e->weight());

    if(genCH){
        TLorentzVector tempv;
        tempv.SetPtEtaPhiE(genCH->Pt(), genCH->Eta(), genCH->Phi(), genCH->E());
        Fill("ChargedHiggsHW/VetoSecond/Vars/GenChargedH_Mass_" + label, systname, tempv.M(), e->weight());
        Fill("ChargedHiggsHW/VetoSecond/Vars/GenChargedH_Pt_" + label, systname, genCH->Pt(), e->weight());
    }

    //Here we can apply a cut on the mass of the charged Higgs
    if(vHW1.M() > 250) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 12, e->weight());
    if(vHW1.M() > 300) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 13, e->weight());
    if(vHW1.M() > 350) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 14, e->weight());
    if(vHW1.M() > 400) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 15, e->weight());
    if(vHW1.M() > 450) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 16, e->weight());
    if(vHW1.M() > 500) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 17, e->weight());
    if(vHW1.M() > 550) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 18, e->weight());
    if(vHW1.M() > 600) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 19, e->weight());
    if(vHW1.M() > 650) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 20, e->weight());
    if(vHW1.M() > 700) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 21, e->weight());
    if(vHW1.M() > 750) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 22, e->weight());
    if(vHW1.M() > 800) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 23, e->weight());
    if(vHW1.M() > 850) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 24, e->weight());
    if(vHW1.M() > 900) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 25, e->weight());
    if(vHW1.M() > 950) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 26, e->weight());
    if(vHW1.M() > 1000) Fill("ChargedHiggsHW/VetoSecond/CutFlow/CutFlow_" + label, systname, 27, e->weight());

    return EVENT_USED;

}

Double_t ChargedHiggsHW::neutrinoPz(TLorentzVector vLepton, TLorentzVector vMet, Int_t sol){

    Double_t  Pz;
    Double_t Plx = vLepton.Px(), Ply = vLepton.Py(), Plz = vLepton.Pz(), El = vLepton.E();
    Double_t Emet = vMet.E(), Pnux = vMet.Px(), Pnuy = vMet.Py();

    if(sol==0) Pz = (-(Plz*(4*Power(El,2) + 4*Power(Emet,2) - 4*Power(Mw,2) - 4*Power(Plx,2) - 4*Power(Ply,2) - 4*Power(Plz,2) - 8*Plx*Pnux - 4*Power(Pnux,2) - 8*Ply*Pnuy - 4*Power(Pnuy,2))) + 
        Sqrt(Power(Plz,2)*Power(4*Power(El,2) + 4*Power(Emet,2) - 4*Power(Mw,2) - 4*Power(Plx,2) - 4*Power(Ply,2) - 4*Power(Plz,2) - 8*Plx*Pnux - 4*Power(Pnux,2) - 8*Ply*Pnuy - 4*Power(Pnuy,2),2) - 
          4*(4*Power(El,2) - 4*Power(Plz,2))*(-Power(El,4) + 2*Power(El,2)*Power(Emet,2) - Power(Emet,4) + 2*Power(El,2)*Power(Mw,2) + 2*Power(Emet,2)*Power(Mw,2) - Power(Mw,4) + 2*Power(El,2)*Power(Plx,2) + 
             2*Power(Emet,2)*Power(Plx,2) - 2*Power(Mw,2)*Power(Plx,2) - Power(Plx,4) + 2*Power(El,2)*Power(Ply,2) + 2*Power(Emet,2)*Power(Ply,2) - 2*Power(Mw,2)*Power(Ply,2) - 2*Power(Plx,2)*Power(Ply,2) - Power(Ply,4) + 
             2*Power(El,2)*Power(Plz,2) + 2*Power(Emet,2)*Power(Plz,2) - 2*Power(Mw,2)*Power(Plz,2) - 2*Power(Plx,2)*Power(Plz,2) - 2*Power(Ply,2)*Power(Plz,2) - Power(Plz,4) + 4*Power(El,2)*Plx*Pnux + 
             4*Power(Emet,2)*Plx*Pnux - 4*Power(Mw,2)*Plx*Pnux - 4*Power(Plx,3)*Pnux - 4*Plx*Power(Ply,2)*Pnux - 4*Plx*Power(Plz,2)*Pnux + 2*Power(El,2)*Power(Pnux,2) + 2*Power(Emet,2)*Power(Pnux,2) - 
             2*Power(Mw,2)*Power(Pnux,2) - 6*Power(Plx,2)*Power(Pnux,2) - 2*Power(Ply,2)*Power(Pnux,2) - 2*Power(Plz,2)*Power(Pnux,2) - 4*Plx*Power(Pnux,3) - Power(Pnux,4) + 4*Power(El,2)*Ply*Pnuy + 
             4*Power(Emet,2)*Ply*Pnuy - 4*Power(Mw,2)*Ply*Pnuy - 4*Power(Plx,2)*Ply*Pnuy - 4*Power(Ply,3)*Pnuy - 4*Ply*Power(Plz,2)*Pnuy - 8*Plx*Ply*Pnux*Pnuy - 4*Ply*Power(Pnux,2)*Pnuy + 2*Power(El,2)*Power(Pnuy,2) + 
             2*Power(Emet,2)*Power(Pnuy,2) - 2*Power(Mw,2)*Power(Pnuy,2) - 2*Power(Plx,2)*Power(Pnuy,2) - 6*Power(Ply,2)*Power(Pnuy,2) - 2*Power(Plz,2)*Power(Pnuy,2) - 4*Plx*Pnux*Power(Pnuy,2) - 
             2*Power(Pnux,2)*Power(Pnuy,2) - 4*Ply*Power(Pnuy,3) - Power(Pnuy,4))))/(2.*(4*Power(El,2) - 4*Power(Plz,2)));

    else Pz = (-(Plz*(4*Power(El,2) + 4*Power(Emet,2) - 4*Power(Mw,2) - 4*Power(Plx,2) - 4*Power(Ply,2) - 4*Power(Plz,2) - 8*Plx*Pnux - 4*Power(Pnux,2) - 8*Ply*Pnuy - 4*Power(Pnuy,2))) - 
        Sqrt(Power(Plz,2)*Power(4*Power(El,2) + 4*Power(Emet,2) - 4*Power(Mw,2) - 4*Power(Plx,2) - 4*Power(Ply,2) - 4*Power(Plz,2) - 8*Plx*Pnux - 4*Power(Pnux,2) - 8*Ply*Pnuy - 4*Power(Pnuy,2),2) - 
          4*(4*Power(El,2) - 4*Power(Plz,2))*(-Power(El,4) + 2*Power(El,2)*Power(Emet,2) - Power(Emet,4) + 2*Power(El,2)*Power(Mw,2) + 2*Power(Emet,2)*Power(Mw,2) - Power(Mw,4) + 2*Power(El,2)*Power(Plx,2) + 
             2*Power(Emet,2)*Power(Plx,2) - 2*Power(Mw,2)*Power(Plx,2) - Power(Plx,4) + 2*Power(El,2)*Power(Ply,2) + 2*Power(Emet,2)*Power(Ply,2) - 2*Power(Mw,2)*Power(Ply,2) - 2*Power(Plx,2)*Power(Ply,2) - Power(Ply,4) + 
             2*Power(El,2)*Power(Plz,2) + 2*Power(Emet,2)*Power(Plz,2) - 2*Power(Mw,2)*Power(Plz,2) - 2*Power(Plx,2)*Power(Plz,2) - 2*Power(Ply,2)*Power(Plz,2) - Power(Plz,4) + 4*Power(El,2)*Plx*Pnux + 
             4*Power(Emet,2)*Plx*Pnux - 4*Power(Mw,2)*Plx*Pnux - 4*Power(Plx,3)*Pnux - 4*Plx*Power(Ply,2)*Pnux - 4*Plx*Power(Plz,2)*Pnux + 2*Power(El,2)*Power(Pnux,2) + 2*Power(Emet,2)*Power(Pnux,2) - 
             2*Power(Mw,2)*Power(Pnux,2) - 6*Power(Plx,2)*Power(Pnux,2) - 2*Power(Ply,2)*Power(Pnux,2) - 2*Power(Plz,2)*Power(Pnux,2) - 4*Plx*Power(Pnux,3) - Power(Pnux,4) + 4*Power(El,2)*Ply*Pnuy + 
             4*Power(Emet,2)*Ply*Pnuy - 4*Power(Mw,2)*Ply*Pnuy - 4*Power(Plx,2)*Ply*Pnuy - 4*Power(Ply,3)*Pnuy - 4*Ply*Power(Plz,2)*Pnuy - 8*Plx*Ply*Pnux*Pnuy - 4*Ply*Power(Pnux,2)*Pnuy + 2*Power(El,2)*Power(Pnuy,2) + 
             2*Power(Emet,2)*Power(Pnuy,2) - 2*Power(Mw,2)*Power(Pnuy,2) - 2*Power(Plx,2)*Power(Pnuy,2) - 6*Power(Ply,2)*Power(Pnuy,2) - 2*Power(Plz,2)*Power(Pnuy,2) - 4*Plx*Pnux*Power(Pnuy,2) - 
             2*Power(Pnux,2)*Power(Pnuy,2) - 4*Ply*Power(Pnuy,3) - Power(Pnuy,4))))/(2.*(4*Power(El,2) - 4*Power(Plz,2)));

    if(Pz != Pz) Pz = (-(Plz*(4*Power(El,2) + 4*Power(Emet,2) - 4*Power(Mw,2) - 4*Power(Plx,2) - 4*Power(Ply,2) - 4*Power(Plz,2) - 8*Plx*Pnux - 4*Power(Pnux,2) - 8*Ply*Pnuy -
            4*Power(Pnuy,2))))/(2.*(4*Power(El,2) - 4*Power(Plz,2)));

    return Pz;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
