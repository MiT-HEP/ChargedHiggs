#include "interface/AnalysisChargedHiggsHW.hpp"

void ChargedHiggsHW::Init()
{

    for ( string l : AllLabel()  ) {
        cout <<"[ChargedHiggsHW]::[Init]::[INFO] Booking Histogram CutFlow_" <<l<<endl;
        Book( ("ChargedHiggsHW/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),10,-.5,10-.5);
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(2,"Requirement 1");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(3,"Requirement 2");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(4,"Requirement 3");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(5,"Requirement 5");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(6,"Requirement 6");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(7,"Requirement 7");
        GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(8,"Requirement 8");

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
    for(Int_t ijet = 0; ijet < e->Totjets(); ijet++) {
            
        Double_t highestLPt=-1, highestBPt=-1;
        Jet *highestPtLJet=0, *highestPtBJet=0;
            
        for(Int_t jjet = 0; jjet < e->Totjets(); jjet++){

            Jet *tempjet = e->GetJet(jjet);

            if(!tempjet) break;

            if(!tempjet->IsJet()) continue;
            
            //I might need to ignore jets close to isolated leptons or photons here
                
            if(tempjet->IsBJet()){
                
                bool isDifferent = true;
                for(UInt_t x=0; x<orderedBJets.size(); x++){
                    if(tempjet == orderedBJets.at(x)) isDifferent = false;
                }
                if(!isDifferent || tempjet->Pt() <= highestBPt) continue;
                
                highestBPt = tempjet->Pt();
                highestPtBJet = tempjet;
                    
            }
                
            else{
                
                bool isDifferent = true;
                for(UInt_t x=0; x<orderedLJets.size(); x++){
                    if(tempjet == orderedLJets.at(x)) isDifferent = false;
                }
                if(!isDifferent || tempjet->Pt() <= highestLPt) continue;
                
                highestLPt = tempjet->Pt();
                highestPtLJet = tempjet;
                    
            }
                
        }
            
        if(highestBPt != -1 && highestPtBJet) orderedBJets.push_back(highestPtBJet);
            
        if(highestLPt != -1 && highestPtLJet) orderedLJets.push_back(highestPtLJet);    
            
    }

    //Fill vector for easier lepton manipulation
    for(Int_t ilepton = 0; ilepton < e->Totleps(); ilepton++) {
            
        Double_t highestPt=-1;
        Lepton *highestPtLepton=0;
            
        for(Int_t jlepton = 0; jlepton < e->Totleps(); jlepton++){

            Lepton *templepton = e->GetLepton(jlepton);

            if(!templepton) break;

            if(!templepton->IsLep()) continue;
            
            //I might need to ignore leptons close to high-pt jets
                
            bool isDifferent = true;
            for(UInt_t x=0; x<orderedLeptons.size(); x++){
                if(templepton == orderedLeptons.at(x)) isDifferent = false;
            }
            if(!isDifferent || templepton->Pt() <= highestPt) continue;
                
            highestPt = templepton->Pt();
            highestPtLepton = templepton;
               
        }
            
        if(highestPt != -1 && highestPtLepton) orderedLeptons.push_back(highestPtLepton);   
    }

    //Fill histograms

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 0 ,e->weight());

    cout << orderedBJets.size() <<  "  " << orderedLJets.size() << "  " <<  orderedLeptons.size() << "  " << met.E() << endl;


    //Apply first cut (without the distance requirement)
    if(orderedBJets.size() < 3 || orderedLJets.size() < 2 || orderedLeptons.size() < 1 || met.E() < 20) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 1 ,e->weight());

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

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 2 ,e->weight());

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

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 3 ,e->weight());

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
    if(minDeltaMH > 15) return 0;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 4 ,e->weight());

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

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 5 ,e->weight());

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

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 6 ,e->weight());

    if(minDeltaMT2 < 30) bjetT = tempT2;
    else bjetT = tempT1;

    vbjetT.SetPtEtaPhiE(bjetT->Pt(), bjetT->Eta(), bjetT->Phi(), bjetT->E());

    //Construct mass observable
    if(bjetT == tempT1) vWH = vW2 + vH;
    else vWH = vW1 + vH;

    //Fill histograms

    //Apply eighth cut
    //if(vWH.M() < --- || vHM.M() > ---) continue;

    Fill("ChargedHiggsHW/CutFlow/CutFlow_"+label,systname, 7 ,e->weight());

    return 1;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
