#include "interface/EventVariablesAnalysis.hpp"

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

        Book(    ("Vars/Rho_"+l).c_str(),("Rho "+l).c_str(),1000,0,100);
        GetHisto(("Vars/Rho_"+l).c_str(),"")->GetXaxis()->SetTitle("#rho [GeV]");
    }
}

int EventVariables::analyze(Event*e,string systname)
{
#ifdef VERBOSE
    if(VERBOSE>0)cout<<"[EventVariables]::[analyze]::[DEBUG] analyze event with syst: "<<systname<<endl;
#endif
    string label =  GetLabel(e);

    Fill( "Vars/Rho_"+label, systname, e->Rho() , e->weight() );

    Fill( ("Vars/Ht_"+label).c_str() ,systname, e->Ht() ,e->weight());
    Jet *j1 = e->LeadJet();
    if (j1 != NULL ) Fill( ("Vars/PtJ1_"+label).c_str() ,systname, j1->Pt() ,e->weight());
    Tau *t1 = e->LeadTau();
    if (t1 != NULL ) Fill( ("Vars/PtT1_"+label).c_str() ,systname, t1->Pt() ,e->weight());
    Fill( ("Vars/Mt_"+label).c_str() ,systname, e->Mt() ,e->weight());
    Fill( ("Vars/Mvis_"+label).c_str() ,systname, e->Mvis() ,e->weight());

    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
