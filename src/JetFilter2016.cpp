#include "interface/JetFilter2016.hpp"
#include "TFile.h"

//#define DEBUG 2

#ifdef DEBUG
    #include <iostream>
#endif

void JetFilter2016::Init(){
    TFile *f=TFile::Open("aux/hotjets-runBCDEFGH.root");
    h.reset( (TH2D*)f->Get("h2jet")->Clone() ) ;
    //f->Close();
}

int JetFilter2016::correct(Event *e) 
{
#ifdef DEBUG
    if (DEBUG>1){
        std::cout <<" BEFORE CORRECTIONS:";
        for(int ijet =0; ;ijet++)
        {
            Jet *j= e->GetJet(ijet);
            if (j==NULL) break;
            std::cout << "("<<j->Pt()<<","<<j->Eta()<<","<<j->Phi()<<") ";

        }
        std::cout << std::endl;
    }
#endif

    for (auto&j : GetJetVector(e) )
    {
        if ( h->GetBinContent(h->FindBin(j->Eta(), j->Phi())) > 0)  
        {
            // bad 
            Scale(*j,0.0);
        }
    }
#ifdef DEBUG
    if (DEBUG>1){
        std::cout <<" AFTER CORRECTIONS:";
        for(int ijet =0; ;ijet++)
        {
            Jet *j= e->GetJet(ijet);
            if (j==NULL) break;
            std::cout << "("<<j->Pt()<<","<<j->Eta()<<","<<j->Phi()<<") ";

        }
        std::cout << std::endl;
    }
#endif
}
