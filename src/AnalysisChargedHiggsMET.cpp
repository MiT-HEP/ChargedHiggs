#include "interface/AnalysisChargedHiggsMET.hpp"

void ChargedHiggsMET::Init()
{

	for ( string l : AllLabel()  ) {
		cout <<"[ChargedHiggsMET]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
		Book( ("ChargedHiggsMET/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),100,-.5,100-.5);
		GetHisto(("ChargedHiggsMET/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
		

		//Histograms should be booked here for details see src/AnalysisChargedHiggsTauNu.cpp


	}

}

int ChargedHiggsMET::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[ChargedHiggsMET]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label = GetLabel(e);

	if(e->weight() == 0. ) cout <<"[ChargedHiggsMET]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

	Fill(("ChargedHiggsMET/CutFlow/CutFlow_"+label).c_str(),systname,0,e->weight());

	//The event selection should be implemented here for details see src/AnalysisChargedHiggsTauNu.cpp


	return 1;
}
