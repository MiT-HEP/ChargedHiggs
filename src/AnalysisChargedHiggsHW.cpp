#include "interface/AnalysisChargedHiggsHW.hpp"

void ChargedHiggsHW::Init()
{

    for ( string l : AllLabel()  ) {
        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram CutFlow_" <<l<<endl;
        Book( ("ChargedHiggsHW/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),12,-.5,12-.5);
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(2,"1 light jets");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(3,"2 light jets");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(4,"1 lepton");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(5,"met > 20");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(6,"1 bjets");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(7,"2 bjets");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(8,"3 bjets");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(9,"W mass");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(10,"Top veto");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(11,"H mass");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(12,"Top mass");

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histograms of dR between jets " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/bjets12_dR_"+ l ) .c_str(), ("dR(b1,b2) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/bjets13_dR_"+ l ) .c_str(), ("dR(b1,b3) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/bjets23_dR_"+ l ) .c_str(), ("dR(b2,b3) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/jetsW_dR_"+ l ) .c_str(), ("dR(j1,j2) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/bjetsH_dR_"+ l ) .c_str(), ("dR(b1,b2) "+ l).c_str(),50,0,10);

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histograms of Higgs bjets " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/bjetsH_dEta_"+ l ) .c_str(), ("dEta(b1,b2) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/bjetsH_dPhi_"+ l ) .c_str(), ("dPhi(b1,b2) "+ l).c_str(),50,0,10);
        Book( ("ChargedHiggsHW/Vars/bjetsH_Pt1_"+ l ) .c_str(), ("pT1 "+ l).c_str(),50,20,150);
        Book( ("ChargedHiggsHW/Vars/bjetsH_Pt2_"+ l ) .c_str(), ("pT2 "+ l).c_str(),50,20,150);

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram of dPhi lepton and met " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/leptonMet_dPhi_"+ l ) .c_str(), ("dPhi(l,MET) "+ l).c_str(),50,0,10);

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram of MET " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/Met_"+ l ) .c_str(), ("MET "+ l).c_str(),50,0,150);

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram of reconstructed Higgs mass " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/HMass_"+ l ) .c_str(), ("H Mass "+ l).c_str(),50,100,150);

        Book( ("ChargedHiggsHW/Vars/bjets_eta_"+ l ) .c_str(), ("Eta of bjets "+ l).c_str(),50,-6,6);
        Book( ("ChargedHiggsHW/Vars/ljets_eta_"+ l ) .c_str(), ("Eta of light jets "+ l).c_str(),50,-6,6);
        Book( ("ChargedHiggsHW/Vars/lepton_eta_"+ l ) .c_str(), ("Eta of lepton "+ l).c_str(),50,-6,6);

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram for cut variables " <<l<<endl;
        Book( ("ChargedHiggsHW/CutVars/bjet1_pt_"+ l ) .c_str(), ("pT of leading bjet "+ l).c_str(),50,20,300);
        Book( ("ChargedHiggsHW/CutVars/bjet2_pt_"+ l ) .c_str(), ("pT of second leading bjet "+ l).c_str(),50,20,200);
        Book( ("ChargedHiggsHW/CutVars/bjet3_pt_"+ l ) .c_str(), ("pT of third leading bjet "+ l).c_str(),50,20,200);
        Book( ("ChargedHiggsHW/CutVars/ljet1_pt_"+ l ) .c_str(), ("pT of leading light jet "+ l).c_str(),50,20,300);
        Book( ("ChargedHiggsHW/CutVars/ljet2_pt_"+ l ) .c_str(), ("pT of second leading light jet "+ l).c_str(),50,20,200);
        Book( ("ChargedHiggsHW/CutVars/NBJets_"+ l ) .c_str(), ("Number of bjets "+ l).c_str(),6,-.5,5.5);
        Book( ("ChargedHiggsHW/CutVars/NLJets_"+ l ) .c_str(), ("Number of light jets "+ l).c_str(),9,-.5,8.5);
        Book( ("ChargedHiggsHW/CutVars/NLeptons_"+ l ) .c_str(), ("Number of leptons "+ l).c_str(),6,-.5,5.5);
        Book( ("ChargedHiggsHW/CutVars/LeptonPt_"+ l ) .c_str(), ("p_{T} of leading lepton "+ l).c_str(),50,0,150);
        Book( ("ChargedHiggsHW/CutVars/Met_"+ l ) .c_str(), ("MET "+ l).c_str(),50,0,225);
        Book( ("ChargedHiggsHW/CutVars/WMassHadronic_"+ l ) .c_str(), ("M_{W} reconstructed from jets "+ l).c_str(),50,30,130);
        Book( ("ChargedHiggsHW/CutVars/WPtHadronic_"+ l ) .c_str(), ("W p_{T} reconstructed from jets "+ l).c_str(),50,0,700);
        Book( ("ChargedHiggsHW/CutVars/WPtLeptonic_"+ l ) .c_str(), ("W p_{T} reconstructed from lepton+MET "+ l).c_str(),50,0,700);
        Book( ("ChargedHiggsHW/CutVars/WEtaLeptonic_"+ l ) .c_str(), ("#eta_{W} reconstructed from lepton+MET "+ l).c_str(),50,-5,5);
        Book( ("ChargedHiggsHW/CutVars/WPhiLeptonic_"+ l ) .c_str(), ("#varphi_{W} reconstructed from lepton+MET "+ l).c_str(),50,-5,5);
        Book( ("ChargedHiggsHW/CutVars/HMass_"+ l ) .c_str(), ("Higgs Mass "+ l).c_str(),50,75,175);
        Book( ("ChargedHiggsHW/CutVars/HPt_"+ l ) .c_str(), ("Higgs p_{T} "+ l).c_str(),50,0,700);
        Book( ("ChargedHiggsHW/CutVars/TopVeto_"+ l ) .c_str(), ("Max number of top candidates "+ l).c_str(),3,-.5,2.5);
        Book( ("ChargedHiggsHW/CutVars/TopMass_"+ l ) .c_str(), ("Top mass "+ l).c_str(),50,123,223);
        Book( ("ChargedHiggsHW/CutVars/TopPt_"+ l ) .c_str(), ("Top p_{T} "+ l).c_str(),50,0,200);
        Book( ("ChargedHiggsHW/CutVars/ChargedHMass_"+ l ) .c_str(), ("Charged Higgs Mass "+ l).c_str(),50,100,1300);
        Book( ("ChargedHiggsHW/CutVars/ChargedHPt_"+ l ) .c_str(), ("Charged Higgs p_{T} "+ l).c_str(),50,0,350);
        Book( ("ChargedHiggsHW/CutVars/jetsW_dR_"+ l ) .c_str(), ("dR(j1,j2) "+ l).c_str(),50,0,6);
        Book( ("ChargedHiggsHW/CutVars/bjetsH_dR_"+ l ) .c_str(), ("dR(b1,b2) "+ l).c_str(),50,0,6);
        Book( ("ChargedHiggsHW/CutVars/genHPt_"+ l ) .c_str(), ("generated Higgs p_{T} "+ l).c_str(),50,0,700);
        Book( ("ChargedHiggsHW/CutVars/genChargedHMass_"+ l ) .c_str(), ("generated Charged Higgs Mass "+ l).c_str(),50,50,1300);
        Book( ("ChargedHiggsHW/CutVars/genChargedHPt_"+ l ) .c_str(), ("generated Charged Higgs p_{T} "+ l).c_str(),50,0,350);
        Book( ("ChargedHiggsHW/CutVars/genWPtHadronic_"+ l ) .c_str(), ("generated W p_{T} reconstructed from jets "+ l).c_str(),50,0,700);
        Book( ("ChargedHiggsHW/CutVars/genWPtLeptonic_"+ l ) .c_str(), ("generated W p_{T} reconstructed from lepton+MET "+ l).c_str(),50,0,700);

    }

}

inline Double_t deltaR( const Float_t eta1, const Float_t eta2, const Float_t phi1, const Float_t phi2 )
{

    const Float_t pi = 3.14159265358979;

    Float_t etaDiff = (eta1-eta2);
    Float_t phiDiff = fabs(phi1-phi2);
    while ( phiDiff>pi ) phiDiff = fabs(phiDiff-2.0*pi);

    Float_t deltaRSquared = etaDiff*etaDiff + phiDiff*phiDiff;

    return TMath::Sqrt(deltaRSquared);

}

using namespace TMath;

Double_t neutrinoPz(TLorentzVector vLepton, TLorentzVector vMet, Int_t sol){

    Double_t Mw = 80.385, Pz;
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

int ChargedHiggsHW::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsHW]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsHW]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    const Double_t DR = 0.4;

    //Define objects and their 4-momenta
    vector<Jet*> orderedLJets, orderedBJets;
    vector<Lepton*> orderedLeptons;
    Jet *bjetH1=0, *bjetH2=0, *jetW1=0, *jetW2=0, *bjetT=0;
    Lepton *leptonW;
    Met met;
    TLorentzVector vLeptonW, vMet, vNeutrino, vW1, vW2Sol1, vW2Sol2, vjetW1, vjetW2, vH, vbjetT, vHW1, vHW2, vbjetH1, vbjetH2, vT;

    //Set up MET
    met = e->GetMet();
    vMet.SetPtEtaPhiE(met.Pt(), met.Eta(), met.Phi(), met.E());

    // Find gen-level particles
    GenParticle *genW1=0, *genW2=0, *genH=0, *genCH=0;

    for(Int_t i = 0; i < e->NGenPar(); i++){
        GenParticle *genpar = e->GetGenParticle(i);

        if(abs(genpar->GetPdgId()) == 37){
            if(!genCH) genCH = genpar;
            else if(deltaR(genpar->Eta(), genCH->Eta(), genpar->Phi(), genCH->Phi()) < DR) genCH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 25){
            if(!genH) genH = genpar;
            else if(deltaR(genpar->Eta(), genH->Eta(), genpar->Phi(), genH->Phi()) < DR) genH = genpar;
        }

        else if(abs(genpar->GetPdgId()) == 24){
            if(!genW1) genW1 = genpar;
            else if(deltaR(genpar->Eta(), genW1->Eta(), genpar->Phi(), genW1->Phi()) < DR) genW1 = genpar;
            else if(genW1 && !genW2) genW2 = genpar;
            else if(genW1 && deltaR(genpar->Eta(), genW2->Eta(), genpar->Phi(), genW2->Phi()) < DR) genW2 = genpar;
        }

    }

    //Fill vectors for easier jet manipulation
    for(Int_t ijet = 0; ijet < e->Njets(); ijet++) {

        Jet *tempjet = e->GetJet(ijet);

        if(!tempjet) break;
            
        //I might need to ignore jets close to isolated leptons or photons here
                
        if(tempjet->IsBJet()) orderedBJets.push_back(tempjet);
                    
        else orderedLJets.push_back(tempjet);

    }

    //Fill vector for easier lepton manipulation
    for(Int_t ilepton = 0; ilepton < e->Nleps(); ilepton++) {

        Lepton *templepton = e->GetLepton(ilepton);

        if(!templepton) break;
            
        //I might need to ignore leptons close to high-pt jets
                
        orderedLeptons.push_back(templepton); 

    }

    //Fill histograms

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 0 , e->weight());

    if(orderedBJets.size() > 0) Fill("ChargedHiggsHW/CutVars/bjet1_pt_"+label,systname, orderedBJets.at(0)->Pt() ,e->weight());
    if(orderedBJets.size() > 1) Fill("ChargedHiggsHW/CutVars/bjet2_pt_"+label,systname, orderedBJets.at(1)->Pt() ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/CutVars/bjet3_pt_"+label,systname, orderedBJets.at(2)->Pt() ,e->weight());
    if(orderedLJets.size() > 0) Fill("ChargedHiggsHW/CutVars/ljet1_pt_"+label,systname, orderedLJets.at(0)->Pt() ,e->weight());
    if(orderedLJets.size() > 1) Fill("ChargedHiggsHW/CutVars/ljet2_pt_"+label,systname, orderedLJets.at(1)->Pt() ,e->weight());

    if(orderedBJets.size() > 1) Fill("ChargedHiggsHW/Vars/bjets12_dR_"+label,systname, deltaR(orderedBJets.at(0)->Eta(), orderedBJets.at(1)->Eta(), orderedBJets.at(0)->Phi(), orderedBJets.at(1)->Phi()) ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/Vars/bjets13_dR_"+label,systname, deltaR(orderedBJets.at(0)->Eta(), orderedBJets.at(2)->Eta(), orderedBJets.at(0)->Phi(), orderedBJets.at(2)->Phi()) ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/Vars/bjets23_dR_"+label,systname, deltaR(orderedBJets.at(1)->Eta(), orderedBJets.at(2)->Eta(), orderedBJets.at(1)->Phi(), orderedBJets.at(2)->Phi()) ,e->weight());

    Fill("ChargedHiggsHW/Vars/Met_"+label,systname, met.E() ,e->weight());

    if(orderedLeptons.size() > 0) Fill("ChargedHiggsHW/Vars/leptonMet_dPhi_"+label,systname, fabs(orderedLeptons.at(0)->Phi()-met.Phi()) ,e->weight());

    //cout << orderedBJets.size() <<  "  " << orderedLJets.size() << "  " <<  orderedLeptons.size() << "  " << met.E() << endl;

    //Apply first cut (without the distance requirement)

    Fill("ChargedHiggsHW/CutVars/NLJets_"+label,systname, e->Njets() - e->Bjets(), e->weight());

    if(orderedLJets.size() < 1) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 1 , e->weight());

    if(orderedLJets.size() < 2) return 0;

    Fill("ChargedHiggsHW/CutVars/NLeptons_"+label,systname, e->Nleps(), e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 2 , e->weight());

    if(orderedLeptons.size() < 1) return 0;

    Lepton *templep = orderedLeptons.at(0);

    Fill("ChargedHiggsHW/CutVars/LeptonPt_"+label,systname, templep->Pt(), e->weight());

    Fill("ChargedHiggsHW/CutVars/Met_"+label,systname, met.E(), e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 3 , e->weight());

    if(met.E() < 20) return 0;

    Fill("ChargedHiggsHW/CutVars/NBJets_"+label,systname, e->Bjets(), e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 4 , e->weight());

    if(orderedBJets.size() < 1) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 5 , e->weight());

    if(orderedBJets.size() < 2) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 6 , e->weight());

    if(orderedBJets.size() < 3) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 7 , e->weight());  

    //Select two jets with mass closest to the W
    Double_t minDeltaMW = 999;
    for(Int_t ijet = 0; ijet < orderedLJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedLJets.size(); jjet++){
            Jet *tempjet1 = orderedLJets.at(ijet), *tempjet2 = orderedLJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vWtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            vWtemp = vtemp1 + vtemp2;

            //if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;

            Double_t tempDeltaM = fabs(vWtemp.M() - 80.385);

            if(tempDeltaM < minDeltaMW){
                minDeltaMW = tempDeltaM;
                jetW1 = tempjet1;
                jetW2 = tempjet2;
            }
        }
    }

    if(!jetW1 || !jetW2) return 0;

    Fill("ChargedHiggsHW/Vars/ljets_eta_"+label,systname, jetW1->Eta(), e->weight());
    Fill("ChargedHiggsHW/Vars/ljets_eta_"+label,systname, jetW2->Eta(), e->weight());

    Fill("ChargedHiggsHW/Vars/bjets_eta_"+label,systname, orderedBJets.at(0)->Eta(), e->weight());
    Fill("ChargedHiggsHW/Vars/bjets_eta_"+label,systname, orderedBJets.at(1)->Eta(), e->weight());
    Fill("ChargedHiggsHW/Vars/bjets_eta_"+label,systname, orderedBJets.at(2)->Eta(), e->weight());

    Fill("ChargedHiggsHW/Vars/lepton_eta_"+label,systname, e->GetLepton(0)->Eta(), e->weight());

    //cout << "jets from hadronic W: " << jetW1->motherPdgId << " " << jetW1->grMotherPdgId << "      " << jetW2->motherPdgId << " " << jetW2->grMotherPdgId << endl;

    vjetW1.SetPtEtaPhiE(jetW1->Pt(), jetW1->Eta(), jetW1->Phi(), jetW1->E());
    vjetW2.SetPtEtaPhiE(jetW2->Pt(), jetW2->Eta(), jetW2->Phi(), jetW2->E());

    vW1 = vjetW1 + vjetW2;

    //Fill histograms
    Fill("ChargedHiggsHW/CutVars/WMassHadronic_"+label,systname, vW1.M(), e->weight());

    //Apply second cut
    if(minDeltaMW > 30) return 0;

    Fill("ChargedHiggsHW/CutVars/WPtHadronic_"+label,systname, vW1.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutVars/jetsW_dR_"+label,systname, deltaR(jetW1->Eta(), jetW2->Eta(), jetW1->Phi(), jetW2->Phi()) , e->weight());

    if(genW1 && genW2){
        if(deltaR(genW1->Eta(), vW1.Eta(), genW1->Phi(), vW1.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtHadronic_"+label,systname, genW1->Pt(), e->weight());
        else if(deltaR(genW2->Eta(), vW1.Eta(), genW2->Phi(), vW1.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtHadronic_"+label,systname, genW2->Pt(), e->weight());
    }

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 8 , e->weight());

    //Find longitudinal neutrino momentum
    Double_t pNuZSol1, pNuZSol2;
    leptonW = orderedLeptons.at(0);
    vLeptonW.SetPtEtaPhiE(leptonW->Pt(), leptonW->Eta(), leptonW->Phi(), leptonW->E());
    pNuZSol1 = neutrinoPz(vLeptonW, vMet, 0);
    pNuZSol2 = neutrinoPz(vLeptonW, vMet, 1);

    //Fill histograms
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol1, Sqrt(vMet.E()*vMet.E() + pNuZSol1*pNuZSol1));
    vW2Sol1 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/CutVars/WPtLeptonic_"+label,systname, vW2Sol1.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutVars/WEtaLeptonic_"+label,systname, vW2Sol1.Eta(), e->weight());
    Fill("ChargedHiggsHW/CutVars/WPhiLeptonic_"+label,systname, vW2Sol1.Phi(), e->weight());
    vNeutrino.SetPxPyPzE(vMet.Px(), vMet.Py(), pNuZSol2, Sqrt(vMet.E()*vMet.E() + pNuZSol2*pNuZSol2));
    vW2Sol2 = vLeptonW + vNeutrino;
    Fill("ChargedHiggsHW/CutVars/WPtLeptonic_"+label,systname, vW2Sol2.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutVars/WEtaLeptonic_"+label,systname, vW2Sol2.Eta(), e->weight());
    Fill("ChargedHiggsHW/CutVars/WPhiLeptonic_"+label,systname, vW2Sol2.Phi(), e->weight());

    if(genW1 && genW2){
        if(deltaR(genW1->Eta(), vW2Sol1.Eta(), genW1->Phi(), vW2Sol1.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtLeptonic_"+label,systname, genW1->Pt(), e->weight());
        else if(deltaR(genW1->Eta(), vW2Sol2.Eta(), genW1->Phi(), vW2Sol2.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtLeptonic_"+label,systname, genW1->Pt(), e->weight());
        else if(deltaR(genW2->Eta(), vW2Sol1.Eta(), genW2->Phi(), vW2Sol1.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtLeptonic_"+label,systname, genW2->Pt(), e->weight());
        else if(deltaR(genW2->Eta(), vW2Sol2.Eta(), genW2->Phi(), vW2Sol2.Phi()) < DR) Fill("ChargedHiggsHW/CutVars/genWPtLeptonic_"+label,systname, genW2->Pt(), e->weight());
    }

    //Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 3 ,e->weight());

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

        for(Int_t jjet = ijet+1; jjet < allJets.size(); jjet++){
            Jet *tempjet1 = allJets.at(ijet), *tempjet2 = allJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;

            if(deltaR(vtemp1.Eta(), jetW1->Eta(), vtemp1.Phi(), jetW1->Phi()) < DR) continue;
            if(deltaR(vtemp1.Eta(), jetW2->Eta(), vtemp1.Phi(), jetW2->Phi()) < DR) continue;
            if(deltaR(vtemp1.Eta(), leptonW->Eta(), vtemp1.Phi(), leptonW->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), jetW1->Eta(), vtemp2.Phi(), jetW1->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), jetW2->Eta(), vtemp2.Phi(), jetW2->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), leptonW->Eta(), vtemp2.Phi(), leptonW->Phi()) < DR) continue;

            vT1temp = vtemp1 + vW1;
            vT21temp = vtemp2 + vW2Sol1;
            vT22temp = vtemp2 + vW2Sol2;

            Double_t tempDeltaM1 = fabs(vT1temp.M() - 173.34);
            Double_t tempDeltaM21 = fabs(vT21temp.M() - 173.34);
            Double_t tempDeltaM22 = fabs(vT21temp.M() - 173.34);

            if(topCandidates == 0 && (tempDeltaM1 < 20 || tempDeltaM21 < 20 || tempDeltaM22 < 20)) topCandidates = 1;
            if(topCandidates < 2 && tempDeltaM1 < 20 && (tempDeltaM21 < 20 || tempDeltaM22 < 20)) topCandidates = 2;

            if((tempDeltaM1 < 20 && tempDeltaM21 < 20) || (tempDeltaM1 < 20 && tempDeltaM22 < 20)) passedTopVeto = false;

        }
    }

    //Fill histograms
    Fill("ChargedHiggsHW/CutVars/TopVeto_"+label,systname, topCandidates, e->weight());

    //Apply sixth cut (top veto)
    if(!passedTopVeto) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 9 , e->weight());

    //Select two bjets with mass closest to the H
    Double_t minDeltaMH = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedBJets.size(); jjet++){
            Jet *tempjet1 = orderedBJets.at(ijet), *tempjet2 = orderedBJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            vHtemp = vtemp1 + vtemp2;

            //if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;

            if(deltaR(vtemp1.Eta(), jetW1->Eta(), vtemp1.Phi(), jetW1->Phi()) < DR) continue;
            if(deltaR(vtemp1.Eta(), jetW2->Eta(), vtemp1.Phi(), jetW2->Phi()) < DR) continue;
            if(deltaR(vtemp1.Eta(), leptonW->Eta(), vtemp1.Phi(), leptonW->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), jetW1->Eta(), vtemp2.Phi(), jetW1->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), jetW2->Eta(), vtemp2.Phi(), jetW2->Phi()) < DR) continue;
            if(deltaR(vtemp2.Eta(), leptonW->Eta(), vtemp2.Phi(), leptonW->Phi()) < DR) continue;

            Double_t tempDeltaM = fabs(vHtemp.M() - 125);

            if(tempDeltaM < minDeltaMH){
                minDeltaMH = tempDeltaM;
                bjetH1 = tempjet1;
                bjetH2 = tempjet2;
            }
        }
    }

    if(!bjetH1 || !bjetH2) return 0;

    //cout << "bjets from Higgs: " << bjetH1->motherPdgId << " " << bjetH1->grMotherPdgId << "      " << bjetH2->motherPdgId << " " << bjetH2->grMotherPdgId << endl;

    vbjetH1.SetPtEtaPhiE(bjetH1->Pt(), bjetH1->Eta(), bjetH1->Phi(), bjetH1->E());
    vbjetH2.SetPtEtaPhiE(bjetH2->Pt(), bjetH2->Eta(), bjetH2->Phi(), bjetH2->E());
    vH = vbjetH1 + vbjetH2;

    //Fill histograms
    Fill("ChargedHiggsHW/CutVars/HMass_"+label,systname, vH.M(), e->weight());

    //Apply fifth cut (the fourth one was skipped due to the low H- mass)
    if(minDeltaMH > 25) return 0;

    if(genH) Fill("ChargedHiggsHW/CutVars/genHPt_"+label,systname, genH->Pt(), e->weight());

    Fill("ChargedHiggsHW/CutVars/HPt_"+label,systname, vH.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutVars/bjetsH_dR_"+label,systname, deltaR(bjetH1->Eta(), bjetH2->Eta(), bjetH1->Phi(), bjetH2->Phi()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_dEta_"+label,systname, fabs(bjetH1->Eta() - bjetH2->Eta()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_dPhi_"+label,systname, fabs(bjetH1->Phi() - bjetH2->Phi()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_Pt1_"+label,systname, bjetH1->Pt() ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_Pt2_"+label,systname, bjetH2->Pt() ,e->weight());
  
    Fill("ChargedHiggsHW/Vars/HMass_"+label,systname, vH.M() ,e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 10 , e->weight());

    Jet *tempT1=0, *tempT21=0, *tempT22=0;
    Double_t minDeltaMT1 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet==bjetH1 || tempjet==bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - 173.34);

        if(tempDeltaM < minDeltaMT1){
            minDeltaMT1 = tempDeltaM;
            tempT1 = tempjet;
        }
    }

    Double_t minDeltaMT21 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet==bjetH1 || tempjet==bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - 173.34);

        if(tempDeltaM < minDeltaMT21){
            minDeltaMT21 = tempDeltaM;
            tempT21 = tempjet;
        }
    }

    Double_t minDeltaMT22 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);
        if(tempjet==bjetH1 || tempjet==bjetH2) continue;

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2Sol2 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - 173.34);

        if(tempDeltaM < minDeltaMT22){
            minDeltaMT22 = tempDeltaM;
            tempT22 = tempjet;
        }
    }

    if(!tempT1 && !tempT21 && !tempT22) return 0;

    //Fill histograms

    if(minDeltaMT21 < minDeltaMT1 && minDeltaMT21 < minDeltaMT22) bjetT = tempT21;
    else if(minDeltaMT22 < minDeltaMT1 && minDeltaMT22 < minDeltaMT21) bjetT = tempT22;
    else bjetT = tempT1;

    if(!bjetT) return 0;

    vbjetT.SetPtEtaPhiE(bjetT->Pt(), bjetT->Eta(), bjetT->Phi(), bjetT->E());

    if(bjetT == tempT21) vT = vW2Sol1 + vbjetT;
    else if(bjetT == tempT22) vT = vW2Sol2 + vbjetT;
    else vT = vW1 + vbjetT;

    Fill("ChargedHiggsHW/CutVars/TopMass_"+label,systname, vT.M(), e->weight());

    //Apply seventh requirement
    if(minDeltaMT1 > 30 && minDeltaMT21 > 30 && minDeltaMT22 > 30) return 0;

    Fill("ChargedHiggsHW/CutVars/TopPt_"+label,systname, vT.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 11, e->weight());

    //Construct mass observable
    if(bjetT == tempT1){ vHW1 = vW2Sol1 + vH; vHW2 = vW2Sol2 + vH;}
    else vHW1 = vHW2 = vW1 + vH;

    //Fill histograms
    Fill("ChargedHiggsHW/CutVars/ChargedHMass_"+label,systname, vHW1.M(), e->weight());
    Fill("ChargedHiggsHW/CutVars/ChargedHMass_"+label,systname, vHW2.M(), e->weight());

    Fill("ChargedHiggsHW/CutVars/ChargedHPt_"+label,systname, vHW1.Pt(), e->weight());
    Fill("ChargedHiggsHW/CutVars/ChargedHPt_"+label,systname, vHW2.Pt(), e->weight());

    if(genCH){
        TLorentzVector tempv;
        tempv.SetPtEtaPhiE(genCH->Pt(), genCH->Eta(), genCH->Phi(), genCH->E());
        Fill("ChargedHiggsHW/CutVars/genChargedHMass_"+label,systname, tempv.M(), e->weight());
        Fill("ChargedHiggsHW/CutVars/genChargedHPt_"+label,systname, genCH->Pt(), e->weight());
    }

    //Apply eighth cut
    //if(vWH.M() < --- || vHM.M() > ---) continue;

    //Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 7 ,e->weight());

    return EVENT_USED;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 