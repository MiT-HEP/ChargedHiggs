#include "interface/One.hpp"

#define VERBOSE 0

void OneAnalysis::Init(){
    if (VERBOSE)Log(__FUNCTION__,"DEBUG","Init");

	Log(__FUNCTION__,"INFO","Booking Histograms");
    for ( string l : AllLabel()  ) {
	    Book ("OneAnalysis/One_"+ l ,"One;bin;Events", 10,0.5,10.5);
        
        GetHisto("OneAnalysis/One_"+l,"")->GetXaxis()->SetBinLabel(1,"SumEvents/SumGen"); // bin shift
        GetHisto("OneAnalysis/One_"+l,"")->GetXaxis()->SetBinLabel(2,"CrossSection"); // bin shift
        GetHisto("OneAnalysis/One_"+l,"")->GetXaxis()->SetBinLabel(3,"Total"); // bin shift
    }
}

int OneAnalysis::analyze(Event *event, string systname)
{
    if ( not doSyst and (systname == "" or systname =="NONE")) return EVENT_NOT_USED;

    string label = GetLabel(e);
    
    //Not implemented systs
    Fill("OneAnalysis/One_"+ label,systname, 1. ,e->GetWeight()->GetBareMCWeight()/e->GetWeight()->GetBareNevents() );
    Fill("OneAnalysis/One_"+ label,systname, 2. ,e->GetWeight()->GetBareMCWeight()*e->GetWeight()->GetBareMCXsec()/e->GetWeight()->GetBareNevents() );
    Fill("OneAnalysis/One_"+ label,systname, 3. ,e->weight());
    // TODO: check sum for systs.
    return EVENT_NOT_USED;
}
