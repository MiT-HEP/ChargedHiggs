#include "interface/AnalysisChargedHiggsEWKPurity.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/CutSelector.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include <string>

void ChargedHiggsEWKPurity::Init()
{
    // -- simplified selection
    for ( string& l : AllLabel()  ) 
    {
            string hist= "EtMiss_Full";
            Book( dir + hist+"_"+ l  , ("EtMissFull"+ l).c_str(),1000,0.,1000.); 
            hist= "EtMiss_Loose";
            Book( dir + hist+"_"+ l  , ("EtMissLoose"+ l).c_str(),1000,0.,1000.); 

            hist= "Recoil_Full";
            Book( dir + hist+"_"+ l  , ("RecoilFull"+ l).c_str(),1000,0.,1000.); 

            hist= "HT_Full";
            Book( dir + hist+"_"+ l  , ("HTFull"+ l).c_str(),1000,0.,1000.); 
    }
}

int ChargedHiggsEWKPurity::analyze(Event*e,string systname)
{
    string label = GetLabel(e);
    e->ApplyTopReweight();
    e->ApplyWReweight();
    CutSelector muon; 
    muon.SetMask(ChargedHiggsTauNu::MaxCut-1);
    muon.SetCut(ChargedHiggsTauNu::Selection(e,true,true));
    bool passLoose=muon.passAllUpTo(ChargedHiggsTauNu::ThreeJets);
    if ( not passLoose) return EVENT_NOT_USED;
    Fill( dir + "EtMiss_Loose_" + label,systname,e->GetMet().Pt(),e->weight() );

#warning Muon90EWKPurity
    if (muon.passAllExcept(ChargedHiggsTauNu::Met) and e->GetMuon(0)->Pt() >90) 
    {
    	Fill( dir + "EtMiss_Full_" + label,systname,e->GetMet().Pt(),e->weight() );
    	Fill( dir + "HT_Full_" + label,systname,e->Ht(),e->weight() );
        TLorentzVector r (e->GetMuon(0)->GetP4() );
        r += e->GetMet().GetP4();
    	Fill( dir + "Recoil_Full_" + label,systname,r.Pt(),e->weight() );
    
    }
    return EVENT_NOT_USED;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
