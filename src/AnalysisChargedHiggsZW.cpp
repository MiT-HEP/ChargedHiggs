#include "interface/AnalysisChargedHiggsZW.hpp"

void ChargedHiggsZW::Init()
{

    for ( string l : AllLabel()  ) {
        cout <<"[ChargedHiggsZW]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
        Book( ("ChargedHiggsZW/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),100,-.5,100-.5);
        GetHisto(("ChargedHiggsZW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");


        //Histograms should be booked here for details see src/AnalysisChargedHiggsTauNu.cpp


    }

}

int ChargedHiggsZW::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[ChargedHiggsZW]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label = GetLabel(e);

    if(e->weight() == 0. ) cout <<"[ChargedHiggsZW]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

    Fill(("ChargedHiggsZW/CutFlow/CutFlow_"+label).c_str(),systname,0,e->weight());

    //The event selection should be implemented here for details see src/AnalysisChargedHiggsTauNu.cpp


    return EVENT_NOT_USED;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
