#include "interface/AnalysisBase.hpp"
#include <iostream>

//#define VERBOSE 1

void AnalysisBase::Book(string name, string title, int nBins,double xmin,double xmax)
{
	output_ -> Book(name,title,nBins,xmin,xmax);
}

void AnalysisBase::Book(string name, string title, int nBins,double *xbound)
{
	output_ -> Book(name,title,nBins,xbound);
}


void AnalysisBase::Fill(string name,string syst, double value, double weight){
	output_ -> Fill(name,syst,value,weight);
}

TH1D* AnalysisBase::GetHisto(string name,string systname)
{
	return output_ -> Get(name,systname);	
}

vector<string> AnalysisBase::AllLabel()
{
	vector<string> R;
	R.push_back("Other");
	R.push_back("Data");
	R.push_back("DY");
	R.push_back("TTJets");
	R.push_back("WW");
	R.push_back("WZ");
	R.push_back("ZZ");
	R.push_back("HBWB_HToTauNu_M-90");
	R.push_back("TBHp_HToTauNu_M-200");
	return R;
}

string AnalysisBase::GetLabel(Event *e){

	// remember if just done
	//static int lastRun = -1;
	//static string label = "";
	string label = "";

	//if (lastRun == e->runNum() ) 
	//	return label;
	
	label = "Other";

	if (e->IsRealData()) label = "Data";
	else{
		for (string& s:  AllLabel() )
		{
		if (e->weight_ . GetMC() . find(s) != string::npos ) {
			label = s; 
			break;
			}
		}
	} // end else (MC)
	return label;
}


