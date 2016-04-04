#include "interface/AnalysisChargedHiggsEWKEmbedding.hpp"
#include "interface/GeneralFunctions.hpp"
#define VERBOSE 0

void ChargedHiggsEWKEmbedding::Init()
{
    /* Init tree dumping */
    if (doTree){
        cout <<"[ChargedHiggsEWKEmbedding]::[Init]::[INFO] doTree"<<endl;
        InitTree(treename);

        Branch(treename,"muPt",'F');
        Branch(treename,"muEta",'F');
        Branch(treename,"muPhi",'F');
        Branch(treename,"run",'I');
        Branch(treename,"lumi",'I');
        Branch(treename,"event",'I');
    } //end doTree

    //for ( string& l : AllLabel()  ) {}
    
    if (doMerge){
        cout <<"[ChargedHiggsEWKEmbedding]::[Init]::[INFO] doMerge"<<endl;
        // init additional tree for merging
        
        tMerge = new TChain("nero/events");
        eventInfo e;
        tMerge->SetBranchAddress("originalRun", & e.run) ;
        tMerge->SetBranchAddress("originalLumi", & e.lumi) ;
        tMerge->SetBranchAddress("originalEvent", & e.event) ;

        cout <<"[ChargedHiggsEWKEmbedding]::[Init]::[INFO] Adding trees to the merge chain:"<<endl;
        for (string &f : embeddingTrees)
            {
            tMerge->Add(f.c_str() );
            cout<<"\t\t* "<<f<<endl;
            }

        for(unsigned ientry = 0 ;ientry < tMerge->GetEntries() ;++ientry)
            {
                tMerge->GetEntry(ientry);
                treeMap[ e ] = ientry; 
            }

        // Init Histos
        string l = "EWKEmbedding";
        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Mt_" <<l<<endl;
        Book(    "ChargedHiggsEWKEmbedding/Vars/Mt_"+l,"Mt "+l,1000,0,1000);
        GetHisto("ChargedHiggsEWKEmbedding/Vars/Mt_"+l,"")->GetXaxis()->SetTitle("m_{T} [GeV]");

        // INIT Bare Taus
        cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] BareTaus init"<<endl;
        bt = new BareTaus();
        bt ->SetExtend();
        bt ->SetMatch();
        bt -> setBranchAddresses( tMerge ) ;
    
    }// end doMerge
// end init
}

int ChargedHiggsEWKEmbedding::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if (label != "Data" ) return EVENT_NOT_USED;

    // *locating the first muons that should be the only lepton
    if ( e->Nleps() != 1 ) return EVENT_NOT_USED;

    Lepton* l1 = e->GetLepton(0);
    if (l1->Pt() <51) return EVENT_NOT_USED;
    if ( fabs( l1->Eta() ) >2.1 ) return EVENT_NOT_USED;

    // is it a muon ? 
    if ( l1 -> type  != 13 )  return EVENT_NOT_USED;
    

    //At least 3 jets

    if ( e->Njets() <3 ) return EVENT_NOT_USED;

    Jet* j1 = e->LeadJet();

    //At least one b-jet

    if ( e->Bjets() <1 ) return EVENT_NOT_USED;
    
    Jet * bj1 = e->LeadBjet();


    //MET>130GeV .. trigger
    if ( e->GetMet().Pt() <130 ) return EVENT_NOT_USED;
    
    // HERE the tau is replaced with the muon!!!
    double DPhiEtMissJet1=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(0))->Phi()));
    double DPhiEtMissJet2=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(1))->Phi()));
    double DPhiEtMissJet3=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(2))->Phi()));
    double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),l1->Phi())); // WARNING <-> this is called Tau to keep the similar structure as the taunu analysis but it is a MUON

    double RbbMin=min(min(sqrt(pow(DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
    double RCollMin=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(DPhiEtMissTau,2)));
    double RsrMax=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));


    //Angular Cuts
    if ( RCollMin*TMath::RadToDeg() <40 ) return EVENT_NOT_USED;
    if ( RbbMin*TMath::RadToDeg() <40 ) return EVENT_NOT_USED;
  
    if (doTree){ 
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] Filling tree "<<endl;
            #endif
        SetTreeVar("run",e->runNum());  
        SetTreeVar("lumi",e->lumiNum());  
        SetTreeVar("event",e->eventNum());  
        SetTreeVar("muPt",l1->Pt());  
        SetTreeVar("muEta",l1->Eta());  
        SetTreeVar("muPhi",l1->Phi());  

        FillTree(treename);
    }

    if (doMerge){
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] do Merge "<<  e->runNum()<<":"<<e->lumiNum()<<":"<< e->eventNum() <<endl;
            #endif
        // substitute the Tau in the event
        unsigned ientry = treeMap[ eventInfo( e->runNum(), e->lumiNum(), e->eventNum() )];
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] -> going to get entry"<<ientry <<" in the merged tree"<<endl;
            #endif
        tMerge->GetEntry(ientry);

        // clear Taus
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] -> clear taus"<<endl;
            #endif
        for (auto o :  e->taus_ ) delete o;
        e -> taus_ . clear();
        //  fill taus with the modified tree
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] -> fill taus"<<endl;
            #endif
        for (int iL = 0; iL<bt -> p4 -> GetEntries() ;++iL)
        {
            Tau *t = new Tau();
            t->SetP4( *(TLorentzVector*) ((*bt->p4)[iL]) );
            t-> iso = (*bt->iso) [iL];
            t-> charge = bt -> Q -> at(iL);
            t-> type = 15;
            t-> id =  (bt -> selBits -> at(iL) ) & BareTaus::Selection::TauDecayModeFinding;
            t-> iso2 = bt -> isoDeltaBetaCorr -> at(iL);
            t-> id_ele = (bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstEleLoose ; 
            t-> id_mu = ( bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstMuLoose; 
            t-> match = bt -> match -> at(iL);
            e -> taus_ . push_back(t);
        }

        // ---------------- SELECTION -------------
            #ifdef VERBOSE
                if(VERBOSE>1)cout<<"[ChargedHiggsEWKEmbedding]::[analyze]::[DEBUG] -> selection"<<endl;
            #endif
        // event has been modified, return event used
        //if ( e->Nleps() != 1 ) return EVENT_USED;
        Tau* t1 = e->GetTau(0);
        if (t1 == NULL ) return EVENT_USED;
        if (t1->Pt() <51) return EVENT_USED;
        if ( fabs( t1->Eta() ) >2.1 ) return EVENT_USED;
        
        // --- USELESS, already done above, lepts also done, requiring only 1
        // --- //At least 3 jets
        // --- if ( e->Njets() <3 ) return EVENT_USED;
        // --- Jet* j1 = e->LeadJet();
        // --- //At least one b-jet
        // --- if ( e->Bjets() <1 ) return EVENT_USED;
        // --- Jet * bj1 = e->LeadBjet();
        // --- //MET>130GeV .. trigger
        // --- if ( e->GetMet().Pt() <130 ) return EVENT_USED;
        
        // and now overwrite it with the tau!
        double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),t1->Phi())); // WARNING <-> this is called Tau to keep the similar structure as the taunu analysis but it is a MUON

        double RbbMin=min(min(sqrt(pow(DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));
        double RCollMin=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(DPhiEtMissTau,2)));
        double RsrMax=min(min(sqrt(pow(TMath::Pi()-DPhiEtMissJet1,2)+pow(TMath::Pi()-DPhiEtMissTau,2)),sqrt(pow(TMath::Pi()-DPhiEtMissJet2,2)+pow(TMath::Pi()-DPhiEtMissTau,2))),sqrt(pow(TMath::Pi()-DPhiEtMissJet3,2)+pow(TMath::Pi()-DPhiEtMissTau,2)));


        //Angular Cuts
        if ( RCollMin*TMath::RadToDeg() <40 ) return EVENT_USED;
        if ( RbbMin*TMath::RadToDeg() <40 ) return EVENT_USED;
        
        string label = "EWKEmbedding";
        Fill("ChargedHiggsEWKEmbedding/Vars/Mt_"+label,systname, e->Mt() ,e->weight());
        
        }

    return EVENT_USED;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
