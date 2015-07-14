#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include "interface/GeneralFunctions.hpp"

void ChargedHiggsTauNu::Init()
{

    for ( string& l : AllLabel()  ) {
        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(1,"Total");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(2,"N_{taus} #geq 1");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(3,"N_{leps} = 0");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(4,"N_{jets} #geq 3");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(5,"N_{Bjets} #geq 1");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(6,"MET > 60 GeV");
        //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"R_{sr}^{Max} < 1.96");
        //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"R_{sr}^{Max} < 140");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"R_{coll}^{Min}>40");
        GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(8,"R_{bb}^{Min}>40");
        //GetHisto("ChargedHiggsTauNu/CutFlow/CutFlow_"+l,"")->GetXaxis()->SetBinLabel(7,"#Delta #phi (Tau,Jet1)<2.09");
         


        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo NTaus_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/NTaus_"+l,"NTaus "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/NTaus_"+l,"")->GetXaxis()->SetTitle("Number of selected hadronic taus");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Tau1Pt_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Tau1Pt_"+l,"Tau1Pt "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/Tau1Pt_"+l,"")->GetXaxis()->SetTitle("p_{T}^{#tau}(leading #tau) [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Tau1Eta_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Tau1Eta_"+l,"Tau1Eta "+l,100,-5,5);
        GetHisto("ChargedHiggsTauNu/Vars/Tau1Eta_"+l,"")->GetXaxis()->SetTitle("#eta^{#tau}(leading #tau) [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo NJets_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/NJets_"+l,"NJets "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/NJets_"+l,"")->GetXaxis()->SetTitle("Number of selected jets");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Jet1Pt_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Jet1Pt_"+l,"Jet1Pt "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/Jet1Pt_"+l,"")->GetXaxis()->SetTitle("p_{T}^{jet}(leading jet) [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Jet1Eta_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Jet1Eta_"+l,"Jet1Eta "+l,100,-5,5);
        GetHisto("ChargedHiggsTauNu/Vars/Jet1Eta_"+l,"")->GetXaxis()->SetTitle("#eta^{jet}(leading jet) [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo NBjets_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/NBjets_"+l,"NBjets "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/NBjets_"+l,"")->GetXaxis()->SetTitle("Number of selected b jets");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Bjet1Pt_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Bjet1Pt_"+l,"Bjet1Pt "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/Bjet1Pt_"+l,"")->GetXaxis()->SetTitle("p_{T}^{b-tagged jet} [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Bjet1Eta_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Bjet1Eta_"+l,"Bjet1Eta "+l,100,-5,5);
        GetHisto("ChargedHiggsTauNu/Vars/Bjet1Eta_"+l,"")->GetXaxis()->SetTitle("#eta^{b-tagged jet} [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo MaxDEtaBjetJets_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/MaxDEtaBjetJets_"+l,"MaxDEtaBjetJets "+l,100,0,10);
        GetHisto("ChargedHiggsTauNu/Vars/MaxDEtaBjetJets_"+l,"")->GetXaxis()->SetTitle("Max #Delta#eta (b-tagged jet,jets)");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo MaxInvMassBjetJets_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/MaxInvMassBjetJets_"+l,"MaxInvMassBjetJets "+l,2000,0,2000);
        GetHisto("ChargedHiggsTauNu/Vars/MaxInvMassBjetJets_"+l,"")->GetXaxis()->SetTitle("Max invariant mass (b-tagged jet,jets) [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo EtMiss_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/EtMiss_"+l,"EtMiss "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/EtMiss_"+l,"")->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo EtMissVsMt_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/EtMissVsMt_"+l,"EtMissVsMt "+l,1000,0,1000,1000,0,1000);
        GetHisto2D("ChargedHiggsTauNu/Vars/EtMissVsMt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");
        GetHisto2D("ChargedHiggsTauNu/Vars/EtMissVsMt_"+l,"")->GetYaxis()->SetTitle("E_{T}^{miss} [GeV]");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo DPhiJetMetVsDPhiTauMet_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/DPhiJet1MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l,50,0,TMath::Pi(),50,0,TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet1MetVsDPhiTauMet_"+l,"")->GetXaxis()->SetTitle("#Delta #phi(#tau,MET)");
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet1MetVsDPhiTauMet_"+l,"")->GetYaxis()->SetTitle("#Delta #phi(Jet1,MET)");

        Book2D(    "ChargedHiggsTauNu/Vars/DPhiJet2MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l,50,0,TMath::Pi(),50,0,TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet2MetVsDPhiTauMet_"+l,"")->GetXaxis()->SetTitle("#Delta #phi(#tau,MET)");
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet2MetVsDPhiTauMet_"+l,"")->GetYaxis()->SetTitle("#Delta #phi(Jet2,MET)");

        Book2D(    "ChargedHiggsTauNu/Vars/DPhiJet3MetVsDPhiTauMet_"+l,"DPhiJetMetVsDPhiTauMet "+l,50,0,TMath::Pi(),50,0,TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet3MetVsDPhiTauMet_"+l,"")->GetXaxis()->SetTitle("#Delta #phi(#tau,MET)");
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiJet3MetVsDPhiTauMet_"+l,"")->GetYaxis()->SetTitle("#Delta #phi(Jet3,MET)");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RCollMin_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/RCollMin_"+l,"RCollMin "+l,100,0,2*TMath::Pi());
        GetHisto("ChargedHiggsTauNu/Vars/RCollMin_"+l,"")->GetXaxis()->SetTitle("R_{coll}^{min}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RbbMin_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/RbbMin_"+l,"RbbMin "+l,100,0,2*TMath::Pi());
        GetHisto("ChargedHiggsTauNu/Vars/RbbMin_"+l,"")->GetXaxis()->SetTitle("R_{bb}^{min}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RsrMax_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/RsrMax_"+l,"RsrMax "+l,100,0,2*TMath::Pi());
        GetHisto("ChargedHiggsTauNu/Vars/RsrMax_"+l,"")->GetXaxis()->SetTitle("R_{sr}^{max}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo DPhiTauJet1_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/DPhiTauJet1_"+l,"DPhiTauJet1 "+l,50,0,TMath::Pi());
        GetHisto("ChargedHiggsTauNu/Vars/DPhiTauJet1_"+l,"")->GetXaxis()->SetTitle("#Delta #phi (#tau, leading jet)");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RCollMinVsMt_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/RCollMinVsMt_"+l,"RCollMinVsMt "+l,1000,0,1000,100,0,2*TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/RCollMinVsMt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");
        GetHisto2D("ChargedHiggsTauNu/Vars/RCollMinVsMt_"+l,"")->GetYaxis()->SetTitle("R_{Coll}^{min}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RbbMinVsMt_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/RbbMinVsMt_"+l,"RbbMinVsMt "+l,1000,0,1000,100,0,2*TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/RbbMinVsMt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");
        GetHisto2D("ChargedHiggsTauNu/Vars/RbbMinVsMt_"+l,"")->GetYaxis()->SetTitle("R_{bb}^{min}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo RsrMaxVsMt_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/RsrMaxVsMt_"+l,"RsrMaxVsMt "+l,1000,0,1000,100,0,2*TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/RsrMaxVsMt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");
        GetHisto2D("ChargedHiggsTauNu/Vars/RsrMaxVsMt_"+l,"")->GetYaxis()->SetTitle("R_{sr}^{Max}");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo DPhiTauJet1VsMt_" <<l<<endl;

        Book2D(    "ChargedHiggsTauNu/Vars/DPhiTauJet1VsMt_"+l,"DPhiTauJet1VsMt "+l,1000,0,1000,50,0,TMath::Pi());
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiTauJet1VsMt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");
        GetHisto2D("ChargedHiggsTauNu/Vars/DPhiTauJet1VsMt_"+l,"")->GetYaxis()->SetTitle("#Delta #phi (#tau, leading jet)");

        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Mt_" <<l<<endl;
        Book(    "ChargedHiggsTauNu/Vars/Mt_"+l,"Mt "+l,1000,0,1000);
        GetHisto("ChargedHiggsTauNu/Vars/Mt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");


    }

}

int ChargedHiggsTauNu::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsTauNu]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsTauNu]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,0,e->weight());

    Fill("ChargedHiggsTauNu/Vars/NTaus_"+label,systname, e->Ntaus() ,e->weight());


    //At least one hadronic tau

    if ( e->Ntaus() <1 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,1,e->weight());

    Tau* t1 = e->LeadTau();
    if (t1 !=NULL ) 
        {        
            Fill("ChargedHiggsTauNu/Vars/Tau1Pt_"+label,systname, t1->Pt() ,e->weight());
            Fill("ChargedHiggsTauNu/Vars/Tau1Eta_"+label,systname,t1->Eta(),e->weight());
        }


    //Veto against isolated lepton

    if ( e->Nleps() >0 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,2,e->weight());

    Fill("ChargedHiggsTauNu/Vars/NJets_"+label,systname, e->Njets() ,e->weight());
    

    //At least 3 jets

    if ( e->Njets() <3 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,3,e->weight());

    Jet* j1 = e->LeadJet();
    if (j1 !=NULL ) 
        {        
            Fill("ChargedHiggsTauNu/Vars/Jet1Pt_"+label,systname, j1->Pt() ,e->weight());
            Fill("ChargedHiggsTauNu/Vars/Jet1Eta_"+label,systname,j1->Eta() ,e->weight());
        }

    
    Fill("ChargedHiggsTauNu/Vars/NBjets_"+label,systname, e->Bjets() ,e->weight());

    //At least one b-jet

    if ( e->Bjets() <1 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,4,e->weight());
    
    Jet * bj1 = e->LeadBjet();
    if (bj1 != NULL) 
        {        
            Fill("ChargedHiggsTauNu/Vars/Bjet1Pt_"+label,systname, bj1->Pt() ,e->weight());
            Fill("ChargedHiggsTauNu/Vars/Bjet1Eta_"+label,systname,bj1->Eta(),e->weight());
        }

    double DEtaMax=0.;
    double InvMassMax=0.;
    for(int i=0;i!=e->Njets();++i)
        {
            Jet* jet = e->GetJet(i);
            if(bj1->DeltaEta(*jet)>DEtaMax) DEtaMax=bj1->DeltaEta(*jet);
            if(bj1->InvMass(*jet)>InvMassMax) InvMassMax=bj1->InvMass(*jet);
        }

    Fill("ChargedHiggsTauNu/Vars/MaxDEtaBjetJets_"+label,systname, DEtaMax ,e->weight());

    Fill("ChargedHiggsTauNu/Vars/MaxInvMassBjetJets_"+label,systname, InvMassMax ,e->weight());
    

    Fill("ChargedHiggsTauNu/Vars/EtMiss_"+label,systname, e->GetMet().Pt() ,e->weight());

    Fill2D("ChargedHiggsTauNu/Vars/EtMissVsMt_"+label,systname,e->Mt(),e->GetMet().Pt(),e->weight());

    //MET>60GeV

    if ( e->GetMet().Pt() <60 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,5,e->weight());
    

    double DPhiEtMissJet1=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(0))->Phi()));
    double DPhiEtMissJet2=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(1))->Phi()));
    double DPhiEtMissJet3=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(2))->Phi()));
    double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),t1->Phi()));

    Fill2D("ChargedHiggsTauNu/Vars/DPhiJet1MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet1,e->weight());
    Fill2D("ChargedHiggsTauNu/Vars/DPhiJet2MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet2,e->weight());
    Fill2D("ChargedHiggsTauNu/Vars/DPhiJet3MetVsDPhiTauMet_"+label,systname,DPhiEtMissTau,DPhiEtMissJet3,e->weight());


    double RbbMin=min(min(sqrt(pow(DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
    double RCollMin=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(DPhiEtMissTau,2)));
    double RsrMax=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
    double DPhiTauJet1=fabs(ChargedHiggs::deltaPhi(t1->Phi(),(e->GetJet(0))->Phi()));

    Fill("ChargedHiggsTauNu/Vars/RbbMin_"+label,systname,RbbMin,e->weight());

    Fill("ChargedHiggsTauNu/Vars/RCollMin_"+label,systname,RCollMin,e->weight());
    Fill("ChargedHiggsTauNu/Vars/RsrMax_"+label,systname,RsrMax,e->weight());

    Fill("ChargedHiggsTauNu/Vars/DPhiTauJet1_"+label,systname,DPhiTauJet1,e->weight());

    Fill2D("ChargedHiggsTauNu/Vars/RbbMinVsMt_"+label,systname,e->Mt(),RbbMin,e->weight());

    Fill2D("ChargedHiggsTauNu/Vars/RCollMinVsMt_"+label,systname,e->Mt(),RCollMin,e->weight());

    Fill2D("ChargedHiggsTauNu/Vars/RsrMaxVsMt_"+label,systname,e->Mt(),RsrMax,e->weight());

    Fill2D("ChargedHiggsTauNu/Vars/DPhiTauJet1VsMt_"+label,systname,e->Mt(),DPhiTauJet1,e->weight());

    //Angular Cuts

    //if ( RsrMax >1.963 ) return EVENT_NOT_USED;
    //Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,6,e->weight());

    //if ( RsrMax*TMath::RadToDeg()>140) return EVENT_NOT_USED;
    //Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,6,e->weight());

    if ( RCollMin*TMath::RadToDeg() <40 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,6,e->weight());

    if ( RbbMin*TMath::RadToDeg() <40 ) return EVENT_NOT_USED;
    Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,7,e->weight());

    //if ( DPhiTauJet1 > 2.089) return EVENT_NOT_USED;
    //Fill("ChargedHiggsTauNu/CutFlow/CutFlow_"+label,systname,6,e->weight());

    Fill("ChargedHiggsTauNu/Vars/Mt_"+label,systname, e->Mt() ,e->weight());

    return EVENT_USED;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
