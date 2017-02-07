#include "interface/SplitMC.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#define SPLITMC_EVENT_FAIL 1
#define SPLITMC_EVENT_PASS 0

// init from FileName
void SplitMCAnalysis::Init(){
    Log(__FUNCTION__,"INFO","Init SplitMC Analysis with file " + fileName );
    ifstream in(fileName);
    if(!in) {Log(__FUNCTION__,"ERROR","Cannot open File:"+fileName );}
    string line;
    stringstream l;
    while (std::getline(in, line)) 
    {
	if (line.find("mc") !=string::npos)continue; // description line
   	l.clear();
	l.str(line);
	unsigned mc; l>>mc;
	unsigned run; l>>run;
	unsigned lumi; l>>lumi;
	uint64_t event; l>>event;
	addBadEvent(mc,run,lumi,event);
    }

    // 0 should not be there
   	Book(Form("SplitMC/CutFlow/CutFlow_%u",0),"split mc cut flow",2,0,2); 
    for( const unsigned &mc :mc_)
    {
   	    Book(Form("SplitMC/CutFlow/CutFlow_%u",mc),"split mc cut flow",2,0,2); 
    }


}

unsigned SplitMCAnalysis::findMC(Event*e)
{
	string label = GetLabel(e);
	//copied from TB, need to have the same labels
	unsigned mc=0;
	if (label.find("HplusToTB") !=string::npos)  //sig
	{
		//            mc = 0;
		// low mass
		if (label.find("M-180") !=string::npos) mc = 1;
		if (label.find("M-200") !=string::npos) mc = 2;
		if (label.find("M-220") !=string::npos) mc = 3;
		if (label.find("M-250") !=string::npos) mc = 4;
		if (label.find("M-300") !=string::npos) mc = 5;
		if (label.find("M-350") !=string::npos) mc = 6;
		if (label.find("M-400") !=string::npos) mc = 7;
		if (label.find("M-450") !=string::npos) mc = 8;
		if (label.find("M-500") !=string::npos) mc = 9;
		// high mass
		if (label.find("M-750") !=string::npos) mc = 10;
		if (label.find("M-800") !=string::npos) mc = 11;
		if (label.find("M-1000")!=string::npos) mc = 12;
		if (label.find("M-2000")!=string::npos) mc = 13;
		if (label.find("M-3000")!=string::npos) mc = 14;


	} else if (label.find("HplusToTauNu") !=string::npos) //sig TauNu
	{
		if (label.find("M-300") !=string::npos) mc = 55;
		if (label.find("M-500") !=string::npos) mc = 59;
		if (label.find("M-800") !=string::npos) mc = 61;
		if (label.find("M-1000")!=string::npos) mc = 62;
		if (label.find("M-2000")!=string::npos) mc = 63;
		if (label.find("M-3000")!=string::npos) mc = 64;

	} else  // bkg
	{
		if(label.find("TTJets_DiLept") !=string::npos) mc =101 ;
		if(label.find("TTJets_SingleLeptFrom") !=string::npos) mc =102 ;
		if(label.find("ST") !=string::npos) mc =111 ;
		if(label.find("tZq") !=string::npos) mc =116 ;
		if(label.find("TTZ") !=string::npos) mc = 121;
		if(label.find("TTW") !=string::npos) mc =123 ;
		if(label.find("ttH") !=string::npos) mc =125 ;
		if(label.find("TTTT") !=string::npos) mc =127 ;

		// V+jets
		//            mc = 200;
		if(label.find("DYJets-madgraph") !=string::npos) mc =221 ;
		if(label.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8")!=string::npos) mc =222;

		// EWK
		// missing tribosons
		if(label.find("WW") !=string::npos) mc =331 ;
		if(label.find("WZ") !=string::npos) mc =333 ;
		if(label.find("ZZ") !=string::npos) mc =337 ;
		if(label.find("VHToNonbb_M125") !=string::npos) mc =340 ;
		if(label.find("WH_HToBB_WToLNu_M125") !=string::npos) mc =341 ;

	}
	return mc;
}

int SplitMCAnalysis::analyze(Event* e,string systname)
{
    // protect real data
    if (e->IsRealData() ) return SPLITMC_EVENT_PASS;

    unsigned mc=findMC(e);
    //* find the mc number matching
    //end

    if (systname=="" or systname=="NONE" )Fill(Form("SplitMC/CutFlow/CutFlow_%u",mc),systname,0,1);

    if ( isBadEvent( mc, e->runNum(),e->lumiNum(),e->eventNum() ) ) {
        return SPLITMC_EVENT_FAIL;
    }
    else {
        if (systname=="" or systname=="NONE" )Fill(Form("SplitMC/CutFlow/CutFlow_%u",mc),systname,1,1); // pass
        return SPLITMC_EVENT_PASS;
    }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
