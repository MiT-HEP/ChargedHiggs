#include "interface/AnalysisChargedHiggsQCDPurity.hpp"

void ChargedHiggsQCDPurity::Init()
{
    for ( string& l : AllLabel()  ) 
    for (size_t iBin = 0 ; iBin + 1 < PtBins.size() ; ++iBin )
    {
        Book( dir + HistName(PtBins[iBin], true )+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
        Book( dir + HistName(PtBins[iBin], false)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
    }
    // --- for event not in the PtBins 
    for ( string& l : AllLabel()  ) 
    {
        Book( dir + HistName(-1, true )+"_"+ l  , ("EtMiss "+ l).c_str(),250,0.,500);
        Book( dir + HistName(-1, false)+"_"+ l  , ("EtMissIsoInv "+ l).c_str(),250,0.,500.);
    }

}

int ChargedHiggsQCDPurity::analyze(Event*e,string systname)
{
    string label = GetLabel(e);
    // do not distinguish between data and mc, 
    // so we can run closures
        // Fill Real Iso 
    Tau *t = e->GetTau(0);
        // Fill Inverted Iso
    Tau *tInv = e->GetTauInvIso(0);

    // TODO:
    // * what do I do with event with a Tau and an Inv tau? -> DY ? 
    // * put a limit on the TauInv sideband ? 10/20 GeV ? 
    //
    if (not e->IsTriggered("HLT_LooseIsoPFTau50_Trk30") )  {
        return EVENT_NOT_USED;
    }
    cout <<" EVENT TRIGGERED"<<endl;

    if (t != NULL and t->Pt()>=51 and fabs(t->Eta())<2.1)
        {
        float pt = t->Pt();
        if (pt  > 8000 or pt <0 ) 
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[INFO] strange event:  tau Pt="<<pt<<endl;
        string hist = HistName(pt,true);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        }
    if (tInv != NULL and tInv->Pt()>=51 and fabs(tInv->Eta())<2.1)
        {
        float pt = tInv->Pt();
        if (pt  > 8000 or pt <0 ) 
            cout <<"[ChargedHiggsQCDPurity]::[analyze]::[INFO] strange event:  tau (inv iso) Pt="<<pt<<endl;
        string hist = HistName(pt,false);
        Fill( dir+hist +"_"+label,systname, e->GetMet().Pt(), e->weight() );
        }

    return EVENT_NOT_USED;
}

int ChargedHiggsQCDPurity::FindBin(float pt)
{
    for(size_t iBin=0;iBin + 1<PtBins.size() ;++iBin)
        if ( pt>= PtBins[iBin] and pt< PtBins[iBin+1] ) return iBin;
    return -1;
}

string ChargedHiggsQCDPurity::HistName(float pt, bool Direct)
{
   int iBin=FindBin(pt);
   string name;

   if (iBin<0 ) name = "EtMiss_BinNotFound"; 
   else name= Form("EtMiss_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1]);

   if (not Direct) name += "_IsoInv";

   return name;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
