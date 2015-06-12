#include "interface/AnalysisChargedHiggsHW.hpp"

void ChargedHiggsHW::Init()
{

	for ( string l : AllLabel()  ) {
		cout <<"[ChargedHiggsHW]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
		Book( ("ChargedHiggsHW/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),100,-.5,100-.5);
		GetHisto(("ChargedHiggsHW/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
		

		//Histograms should be booked here for details see src/AnalysisChargedHiggsTauNu.cpp


	}

}

int ChargedHiggsHW::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[ChargedHiggsHW]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label = GetLabel(e);

	if(e->weight() == 0. ) cout <<"[ChargedHiggsHW]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

	Fill(("ChargedHiggsHW/CutFlow/CutFlow_"+label).c_str(),systname,0,e->weight());

	//The event selection should be implemented here for details see src/AnalysisChargedHiggsTauNu.cpp


	return 1;
}
