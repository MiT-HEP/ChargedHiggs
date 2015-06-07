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

void Preselection::Init()
{

	Book("CutFlow_DY","CutFlow DY",100,-.5,100-.5);
	GetHisto("CutFlow_DY","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_DY","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_DY","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_TT","CutFlow TT",100,-.5,100-.5);
	GetHisto("CutFlow_TT","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_TT","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_TT","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_WW","CutFlow WW",100,-.5,100-.5);
	GetHisto("CutFlow_WW","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_WW","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_WW","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_WZ","CutFlow WZ",100,-.5,100-.5);
	GetHisto("CutFlow_WZ","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_WZ","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_WZ","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_ZZ","CutFlow ZZ",100,-.5,100-.5);
	GetHisto("CutFlow_ZZ","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_ZZ","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_ZZ","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_Data","CutFlow Data",100,-.5,100-.5);
	GetHisto("CutFlow_Data","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_Data","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_Data","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_Other","CutFlow Other",100,-.5,100-.5);
	GetHisto("CutFlow_Other","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_Other","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_Other","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	//signals
	Book("CutFlow_HBWB_HToTauNu_M-90","CutFlow HBWB_HToTauNu_M-90",100,-.5,100-.5);
	GetHisto("CutFlow_HBWB_HToTauNu_M-90","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_HBWB_HToTauNu_M-90","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_HBWB_HToTauNu_M-90","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");

	Book("CutFlow_TBHp_HToTauNu_M-200","CutFlow TBHp_HToTauNu_M-200",100,-.5,100-.5);
	GetHisto("CutFlow_TBHp_HToTauNu_M-200","")->GetXaxis()->SetBinLabel(1,"Total");
	GetHisto("CutFlow_TBHp_HToTauNu_M-200","")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
	GetHisto("CutFlow_TBHp_HToTauNu_M-200","")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");
}

int Preselection::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[Preselection]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label = "CutFlow";
	if (e->IsRealData()) label= "CutFlow_Data";
	else if (e->weight_ . GetMC() . find("DY") != string::npos ) label = "CutFlow_DY"; 
	else if (e->weight_ . GetMC() . find("TT") != string::npos ) label = "CutFlow_TT"; 
	else if (e->weight_ . GetMC() . find("WW") != string::npos ) label = "CutFlow_WW"; 
	else if (e->weight_ . GetMC() . find("WZ") != string::npos ) label = "CutFlow_WZ"; 
	else if (e->weight_ . GetMC() . find("ZZ") != string::npos ) label = "CutFlow_ZZ"; 
	else if (e->weight_ . GetMC() . find("HBWB_HToTauNu_M-90") != string::npos ) label = "CutFlow_HBWB_HToTauNu_M-90"; 
	else if (e->weight_ . GetMC() . find("TBHp_HToTauNu_M-200") != string::npos ) label = "CutFlow_TBHp_HToTauNu_M-200"; 
	else label = "CutFlow_Other";

	Fill( label ,systname,0,e->weight());

	if ( e->Njets() <3 ) return 1;
	Fill( label ,systname,1,e->weight());

	if ( e->Bjets() <1 ) return 1;
	Fill( label,systname,2,e->weight());

	// ---  SF examples --
	//SetPtEtaSF("muon",100,1);
	//ApplySF("muon");
	//e->weight();
	// ----	
	return 0;
}
