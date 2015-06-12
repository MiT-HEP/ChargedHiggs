#include "interface/AnalysisChargedHiggsTauNu.hpp"

void ChargedHiggsTauNu::Init()
{

	for ( string l : AllLabel()  ) {
		cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo CutFlow_" <<l<<endl;
		Book( ("ChargedHiggsTauNu/CutFlow/CutFlow_"+ l ) .c_str(), ("CutFlow "+ l).c_str(),100,-.5,100-.5);
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(1,"Total");
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(2,"N_{taus} #geq 1");
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(3,"N_{leps} = 0");
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(4,"N_{jets} #geq 3");
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(5,"N_{Bjets} #geq 1");
		GetHisto(("ChargedHiggsTauNu/CutFlow/CutFlow_"+l).c_str(),"")->GetXaxis()->SetBinLabel(6,"MET > 60 GeV");


		cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo EtMiss_" <<l<<endl;
		Book(    ("ChargedHiggsTauNu/Vars/EtMiss_"+l).c_str(),("EtMiss "+l).c_str(),1000,0,1000);
		GetHisto(("ChargedHiggsTauNu/Vars/EtMiss_"+l).c_str(),"")->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");


		cout <<"[ChargedHiggsTauNu]::[Init]::[INFO] Boking Histo Mt_" <<l<<endl;
		Book(    ("ChargedHiggsTauNu/Vars/Mt_"+l).c_str(),("Mt "+l).c_str(),1000,0,1000);
		GetHisto(("ChargedHiggsTauNu/Vars/Mt_"+l).c_str(),"")->GetXaxis()->SetTitle("m_{T} [GeV]");


	}

}

int ChargedHiggsTauNu::analyze(Event*e,string systname)
{
#ifdef VERBOSE
	if(VERBOSE>0)cout<<"[ChargedHiggsTauNu]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
	string label = GetLabel(e);

	if(e->weight() == 0. ) cout <<"[ChargedHiggsTauNu]::[analyze]::[INFO] Even Weight is NULL !!"<< e->weight() <<endl;

	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,0,e->weight());



	//At least one hadronic tau

	if ( e->Ntaus() <1 ) return 0;
	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,1,e->weight());


	//Veto against isolated lepton

	if ( e->Nleps() >0 ) return 0;
	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,1,e->weight());


	//At least 3 jets

	if ( e->Njets() <3 ) return 0;
	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,1,e->weight());


	//At least one b-jet

	if ( e->Bjets() <1 ) return 0;
	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,2,e->weight());

	Fill(("ChargedHiggsTauNu/Vars/EtMiss_"+label).c_str() ,systname, e->GetMet().Pt() ,e->weight());


	//MET>60GeV

	if ( e->GetMet().Pt() <60 ) return 0;
	Fill(("ChargedHiggsTauNu/CutFlow/CutFlow_"+label).c_str(),systname,2,e->weight());

	Fill(("ChargedHiggsTauNu/Vars/Mt_"+label).c_str() ,systname, e->Mt() ,e->weight());

	return 1;
}
