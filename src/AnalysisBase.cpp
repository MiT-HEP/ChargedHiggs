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
	R.push_back("TT");
	R.push_back("WW");
	R.push_back("WZ");
	R.push_back("ZZ");
	R.push_back("HBWB_HToTauNu_M-90");
	R.push_back("TBHp_HToTauNu_M-200");
	return R;
}

string AnalysisBase::GetLabel(Event *e){

	// remember if just done
	static int lastRun = -1;
	static string label = "";

	if (lastRun == e->runNum() ) 
		return label;
	
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

void Preselection::Init()
{

	for ( string l : AllLabel()  ) {
		cout <<"[Preselicton]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
		Book( ("CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),100,-.5,100-.5);
		GetHisto(("CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
		GetHisto(("CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(2,"N_{jets} #geq 3");
		GetHisto(("CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(3,"N_{Bjets} #geq 1");
	}

}

int Preselection::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[Preselection]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label = "CutFlow/CutFlow_" + GetLabel(e);

	//cout<<"[Preselection]::[analyze]::[DEBUG] Label for this event is "<<GetLabel(e)<< " w is "<<e->weight()<<endl;
	if(e->weight() == 0. ) cout <<"[Preselection]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

	Fill( label ,systname,0,e->weight());
	//if(systname == "NONE" )cout <<"\t\t\t-> Content after:"<< GetHisto(label,systname)->GetBinContent(1)<<endl;

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

void EventVariables::Init()
{

	for ( string l : AllLabel()  ) {
		Book(    ("Vars/Ht_"+l).c_str(),("Ht "+l).c_str(),1000,0,1000);
		GetHisto(("Vars/Ht_"+l).c_str(),"")->GetXaxis()->SetTitle("H_{T} [GeV]");

		Book(    ("Vars/PtJ1_"+l).c_str(),("Pt Lead Jet "+l).c_str(),1000,0,1000);
		GetHisto(("Vars/PtJ1_"+l).c_str(),"")->GetXaxis()->SetTitle("p_{T}^{j1} [GeV]");

		Book(    ("Vars/PtT1_"+l).c_str(),("Pt Lead Tau "+l).c_str(),1000,0,1000);
		GetHisto(("Vars/PtT1_"+l).c_str(),"")->GetXaxis()->SetTitle("p_{T}^{#tau 1} [GeV]");

		Book(    ("Vars/Mvis_"+l).c_str(),("M vis "+l).c_str(),1000,0,1000);
		GetHisto(("Vars/Mvis_"+l).c_str(),"")->GetXaxis()->SetTitle("m_{vis} [GeV]");

		Book(    ("Vars/Mt_"+l).c_str(),("Mt "+l).c_str(),1000,0,1000);
		GetHisto(("Vars/Mt_"+l).c_str(),"")->GetXaxis()->SetTitle("m_{T} [GeV]");
	}
}

int EventVariables::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[EventVariables]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label =  GetLabel(e);
	Fill( ("Vars/Ht_"+label).c_str() ,systname, e->Ht() ,e->weight());
	int j1 = e->LeadJet();
	if (j1 >=0 ) Fill( ("Vars/PtJ1_"+label).c_str() ,systname, e->GetJet(j1)->Pt() ,e->weight());
	int t1 = e->LeadTau();
	if (t1 >=0 ) Fill( ("Vars/PtT1_"+label).c_str() ,systname, e->GetTau(t1)->Pt() ,e->weight());
	Fill( ("Vars/Mt_"+label).c_str() ,systname, e->Mt() ,e->weight());
	Fill( ("Vars/Mvis_"+label).c_str() ,systname, e->Mvis() ,e->weight());

	return 0;
}
