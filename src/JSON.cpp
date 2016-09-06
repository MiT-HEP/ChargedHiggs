#include "interface/JSON.hpp"

// --- redefining them, 'cause this needs to pass and fail independently from the other analysis

#define JSON_EVENT_FAIL 1
#define JSON_EVENT_PASS 0

void JsonAnalysis::Init(){
    Log(__FUNCTION__,"INFO","Init JSON Analysis");
    if (goodLumis_.empty()) Log(__FUNCTION__,"WARNING","No Good luminosity selection");
    else Log(__FUNCTION__,"INFO",Form("Good Lumi selection based on %u runs ", goodLumis_.size()) ) ;

    Book(    "Json/CutFlow/CutFlow_Data"  , "CutFlow ",100,-.5,100-.5);
}


void JsonAnalysis::addGoodLumi(long run, int lumi1, int lumi2)
{
    goodLumis_[run].push_back( pair<int,int>(lumi1,lumi2) );
}

bool JsonAnalysis::CheckLumi(long run, int lumi){

    auto myrun = goodLumis_.find(run);
    if ( myrun == goodLumis_.end() ) return false;

    for(auto mypair : myrun->second ) 
    {
        if (lumi >= mypair.first and lumi<=mypair.second) return true;	
    }

    return false;
}

int JsonAnalysis::analyze(Event* e,string systname)
{
    // if not Init, return true
    if(goodLumis_.empty() ) return JSON_EVENT_PASS;      

    // -- all MC event should pass
    if (not e->IsRealData() ) return JSON_EVENT_PASS;

    Fill("Json/CutFlow/CutFlow_Data",systname,0,1);

    if ( CheckLumi(e->runNum(), e->lumiNum()  )){
        Fill("Json/CutFlow/CutFlow_Data",systname,1,1); // pass
        return JSON_EVENT_PASS;
    }

    return JSON_EVENT_FAIL;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
