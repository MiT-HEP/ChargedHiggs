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
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(10,"H mass");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(11,"Top veto");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(12,"Top mass");

        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histograms of jets pT " <<l<<endl;
        Book( ("ChargedHiggsHW/Vars/bjet1_pt_"+ l ) .c_str(), ("pT of leading bjet "+ l).c_str(),50,20,150);
        Book( ("ChargedHiggsHW/Vars/bjet2_pt_"+ l ) .c_str(), ("pT of second leading bjet "+ l).c_str(),50,20,150);
        Book( ("ChargedHiggsHW/Vars/bjet3_pt_"+ l ) .c_str(), ("pT of third leading bjet "+ l).c_str(),50,20,150);
        Book( ("ChargedHiggsHW/Vars/ljet1_pt_"+ l ) .c_str(), ("pT of leading light jet "+ l).c_str(),50,20,150);
        Book( ("ChargedHiggsHW/Vars/ljet2_pt_"+ l ) .c_str(), ("pT of second leading light jet "+ l).c_str(),50,20,150);

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

    }

}

Double_t deltaR( const Float_t eta1, const Float_t eta2, const Float_t phi1, const Float_t phi2 )
{

    const Float_t pi = 3.14159265358979;

    Float_t etaDiff = (eta1-eta2);
    Float_t phiDiff = fabs(phi1-phi2);
    while ( phiDiff>pi ) phiDiff = fabs(phiDiff-2.0*pi);

    Float_t deltaRSquared = etaDiff*etaDiff + phiDiff*phiDiff;

    return TMath::Sqrt(deltaRSquared);

}

int ChargedHiggsHW::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsHW]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsHW]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    Fill(("ChargedHiggsHW/CutFlow/CutFlow_"+label).c_str(),systname,0,e->weight());

    const Double_t DR = 0.4;

    //Define objects and their 4-momenta
    vector<Jet*> orderedLJets, orderedBJets;
    vector<Lepton*> orderedLeptons;
    Jet *bjetH1=0, *bjetH2=0, *jetW1=0, *jetW2=0, *bjetT=0;
    Lepton *leptonW=0;
    Met met;
    TLorentzVector vleptonW, vmet, vNeutrino, vW1, vW2, vjetW1, vjetW2, vH, vbjetT, vWH, vbjetH1, vbjetH2;

    //Set up MET
    met = e->GetMet();
    vmet.SetPtEtaPhiE(met.Pt(), met.Eta(), met.Phi(), met.E());

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

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 0 , 1);

    if(orderedBJets.size() > 0) Fill("ChargedHiggsHW/Vars/bjet1_pt_"+label,systname, orderedBJets.at(0)->Pt() ,e->weight());
    if(orderedBJets.size() > 1) Fill("ChargedHiggsHW/Vars/bjet2_pt_"+label,systname, orderedBJets.at(1)->Pt() ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/Vars/bjet3_pt_"+label,systname, orderedBJets.at(2)->Pt() ,e->weight());
    if(orderedLJets.size() > 0) Fill("ChargedHiggsHW/Vars/ljet1_pt_"+label,systname, orderedLJets.at(0)->Pt() ,e->weight());
    if(orderedLJets.size() > 1) Fill("ChargedHiggsHW/Vars/ljet1_pt_"+label,systname, orderedLJets.at(1)->Pt() ,e->weight());

    if(orderedBJets.size() > 1) Fill("ChargedHiggsHW/Vars/bjets12_dR_"+label,systname, deltaR(orderedBJets.at(0)->Eta(), orderedBJets.at(1)->Eta(), orderedBJets.at(0)->Phi(), orderedBJets.at(1)->Phi()) ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/Vars/bjets13_dR_"+label,systname, deltaR(orderedBJets.at(0)->Eta(), orderedBJets.at(2)->Eta(), orderedBJets.at(0)->Phi(), orderedBJets.at(2)->Phi()) ,e->weight());
    if(orderedBJets.size() > 2) Fill("ChargedHiggsHW/Vars/bjets23_dR_"+label,systname, deltaR(orderedBJets.at(1)->Eta(), orderedBJets.at(2)->Eta(), orderedBJets.at(1)->Phi(), orderedBJets.at(2)->Phi()) ,e->weight());

    Fill("ChargedHiggsHW/Vars/Met_"+label,systname, met.E() ,e->weight());

    if(orderedLeptons.size() > 0) Fill("ChargedHiggsHW/Vars/leptonMet_dPhi_"+label,systname, fabs(orderedLeptons.at(0)->Phi()-met.Phi()) ,e->weight());

    //cout << orderedBJets.size() <<  "  " << orderedLJets.size() << "  " <<  orderedLeptons.size() << "  " << met.E() << endl;

    //Apply first cut (without the distance requirement)

    if(orderedLJets.size() < 1) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 1 , 1);

    if(orderedLJets.size() < 2) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 2 , 1);

    if(orderedLeptons.size() < 1) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 3 , 1);

    if(met.E() < 20) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 4 , 1);

    if(orderedBJets.size() < 1) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 5 , 1);

    if(orderedBJets.size() < 2) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 6 , 1);

    if(orderedBJets.size() < 3) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 7 , 1);  

    //Select two jets with mass closest to the W
    Double_t minDeltaMW = 999;
    for(Int_t ijet = 0; ijet < orderedLJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedLJets.size(); jjet++){
            Jet *tempjet1 = orderedLJets.at(ijet), *tempjet2 = orderedLJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vWtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            vWtemp = vtemp1 + vtemp2;

            if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;

            Double_t tempDeltaM = fabs(vWtemp.M() - 80.385);

            if(tempDeltaM < minDeltaMW){
                minDeltaMW = tempDeltaM;
                jetW1 = tempjet1;
                jetW2 = tempjet2;
            }
        }
    }

    //Fill histograms


    //Apply second cut
    if(minDeltaMW > 30) return 0;

    Fill("ChargedHiggsHW/Vars/jetsW_dR_"+label,systname, deltaR(jetW1->Eta(), jetW2->Eta(), jetW1->Phi(), jetW2->Phi()) ,e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 8 , 1);

    vjetW1.SetPtEtaPhiE(jetW1->Pt(), jetW1->Eta(), jetW1->Phi(), jetW1->E());
    vjetW2.SetPtEtaPhiE(jetW2->Pt(), jetW2->Eta(), jetW2->Phi(), jetW2->E());

    //Find longitudinal neutrino momentum
    Double_t pNuZSol1, pNuZSol2;
    leptonW = orderedLeptons.at(0);
    vleptonW.SetPtEtaPhiE(leptonW->Pt(), leptonW->Eta(), leptonW->Phi(), leptonW->E());
    pNuZSol1 = sqrt(-80.385*80.385+(vleptonW.E()+vmet.E())*(vleptonW.E()+vmet.E())-(vleptonW.Px()+vmet.Px())*(vleptonW.Px()+vmet.Px())-(vleptonW.Py()+vmet.Py())*(vleptonW.Py()+vmet.Py()))-vleptonW.Pz();
    pNuZSol2 = -sqrt(-80.385*80.385+(vleptonW.E()+vmet.E())*(vleptonW.E()+vmet.E())-(vleptonW.Px()+vmet.Px())*(vleptonW.Px()+vmet.Px())-(vleptonW.Py()+vmet.Py())*(vleptonW.Py()+vmet.Py()))-vleptonW.Pz();

    //Fill histograms

    //Apply third "cut" (it doesn't cut anything, just requires real solutions)
    if(pNuZSol1 != pNuZSol1 && pNuZSol2 != pNuZSol2) pNuZSol1 = pNuZSol2 = -vleptonW.Pz();

    //Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 3 ,e->weight());

    //Select two bjets with mass closest to the H
    Double_t minDeltaMH = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        for(Int_t jjet = ijet+1; jjet < orderedBJets.size(); jjet++){
            Jet *tempjet1 = orderedBJets.at(ijet), *tempjet2 = orderedBJets.at(jjet);
            TLorentzVector vtemp1, vtemp2, vHtemp;
            vtemp1.SetPtEtaPhiE(tempjet1->Pt(), tempjet1->Eta(), tempjet1->Phi(), tempjet1->E());
            vtemp2.SetPtEtaPhiE(tempjet2->Pt(), tempjet2->Eta(), tempjet2->Phi(), tempjet2->E());

            vHtemp = vtemp1 + vtemp2;

            if(deltaR(vtemp1.Eta(), vtemp2.Eta(), vtemp1.Phi(), vtemp2.Phi()) < DR) continue;

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

    //Fill histograms

    //Apply fifth cut (the fourth one was skipped due to the low H- mass)
    if(minDeltaMH > 25) return 0;

    Fill("ChargedHiggsHW/Vars/bjetsH_dR_"+label,systname, deltaR(bjetH1->Eta(), bjetH2->Eta(), bjetH1->Phi(), bjetH2->Phi()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_dEta_"+label,systname, fabs(bjetH1->Eta() - bjetH2->Eta()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_dPhi_"+label,systname, fabs(bjetH1->Phi() - bjetH2->Phi()) ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_Pt1_"+label,systname, bjetH1->Pt() ,e->weight());
    Fill("ChargedHiggsHW/Vars/bjetsH_Pt2_"+label,systname, bjetH2->Pt() ,e->weight());

    vbjetH1.SetPtEtaPhiE(bjetH1->Pt(), bjetH1->Eta(), bjetH1->Phi(), bjetH1->E());
    vbjetH2.SetPtEtaPhiE(bjetH2->Pt(), bjetH2->Eta(), bjetH2->Phi(), bjetH2->E());
    vH = vbjetH1 + vbjetH2;
    Fill("ChargedHiggsHW/Vars/HMass_"+label,systname, vH.M() ,e->weight());

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 9 , 1);

    //Reconstruct W's 4-momenta
    vNeutrino = vmet;
    vNeutrino.SetPz(pNuZSol1);

    vW1 = vjetW1 + vjetW2;
    vW2 = vleptonW + vNeutrino;

    vector<Jet*> allJets;
    for(Int_t x = 0; x < orderedLJets.size(); x++){
        Jet *tempjet = orderedLJets.at(x);
        if(tempjet == jetW1 || tempjet == jetW2) continue;
        allJets.push_back(tempjet);
    }
    for(Int_t x = 0; x < orderedBJets.size(); x++){
        Jet *tempjet = orderedBJets.at(x);
        if(tempjet == bjetH1 || tempjet == bjetH2) continue;
        allJets.push_back(tempjet);
    }

    //Top veto
    TLorentzVector vT1temp, vT2temp;
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
            vT2temp = vtemp2 + vW2;

            Double_t tempDeltaM1 = fabs(vT1temp.M() - 173.34);
            Double_t tempDeltaM2 = fabs(vT1temp.M() - 173.34);

            if(tempDeltaM1 < 20 && tempDeltaM2 < 20) passedTopVeto = false;
        }
    }

    //Fill histograms

    //Apply sixth cut (top veto)
    if(!passedTopVeto) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 10 , 1);

    Jet *tempT1, *tempT2;
    Double_t minDeltaMT1 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW1 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - 173.34);

        if(tempDeltaM < minDeltaMT1){
            minDeltaMT1 = tempDeltaM;
            tempT1 = tempjet;
        }
    }

    Double_t minDeltaMT2 = 999;
    for(Int_t ijet = 0; ijet < orderedBJets.size(); ijet++){

        Jet *tempjet = orderedBJets.at(ijet);

        TLorentzVector vtemp;
        vtemp.SetPtEtaPhiE(tempjet->Pt(), tempjet->Eta(), tempjet->Phi(), tempjet->E());

        TLorentzVector vTopTemp = vW2 + vtemp;

        Double_t tempDeltaM = fabs(vTopTemp.M() - 173.34);

        if(tempDeltaM < minDeltaMT2){
            minDeltaMT2 = tempDeltaM;
            tempT2 = tempjet;
        }
    }

    //Fill histograms

    //Apply seventh requirement
    if(minDeltaMT1 > 30 && minDeltaMT2 > 30) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 11, 1);

    if(minDeltaMT2 < 30) bjetT = tempT2;
    else bjetT = tempT1;

    vbjetT.SetPtEtaPhiE(bjetT->Pt(), bjetT->Eta(), bjetT->Phi(), bjetT->E());

    //Construct mass observable
    if(bjetT == tempT1) vWH = vW2 + vH;
    else vWH = vW1 + vH;

    //Fill histograms

    //Apply eighth cut
    //if(vWH.M() < --- || vHM.M() > ---) continue;

    //Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 7 ,e->weight());

    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
