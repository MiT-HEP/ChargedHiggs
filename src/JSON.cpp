#include "interface/JSON.cpp"

// --- redefining them, 'cause this needs to pass and fail independently from the other analysis

#define JSON_EVENT_FAIL 1
#define JSON_EVENT_PASS 0



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

void JsonAnalysis::Init(){
}

int JsonAnalysis::analyze(Event*,string systname)
{
    // if not Init, return true
    if(goodLumis_.empty() ) return JSON_EVENT_PASS;      

	// -- all MC event should pass
	if (not e->IsRealData() ) return JSON_EVENT_PASS;

    if ( CheckLumi(e->Run(), e->Lumi()  )) return JSON_EVENT_PASS;

	return JSON_EVENT_FAIL;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
