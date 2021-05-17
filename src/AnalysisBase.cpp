#include "interface/AnalysisBase.hpp"
#include <iostream>

//#define VERBOSE 2
int AnalysisBase::doAnalyze(Event*event,string systname){ 
        e=event;
        SetCuts(e); 
        e->validate(); 
        return analyze(e,systname);
    }

void AnalysisBase::Book(string name, string title, int nBins,double xmin,double xmax)
{
#ifdef VERBOSE
    if(VERBOSE>0)Log(__FUNCTION__,"DEBUG","Booking histo "+name);
#endif
    output_ -> Book(name,title,nBins,xmin,xmax);
}

void AnalysisBase::Book2D(string name, string title, int nBins,double xmin,double xmax,int nBins2,double ymin,double ymax)
{
    output_ -> Book2D(name,title,nBins,xmin,xmax,nBins2,ymin,ymax);
}

void AnalysisBase::Book3D(string name, string title, int nBins,double xmin,double xmax,int nBins2,double ymin,double ymax,int nBins3,double zmin, double zmax)
{
    output_ -> Book3D(name,title,nBins,xmin,xmax,nBins2,ymin,ymax,nBins3,zmin,zmax);
}

void AnalysisBase::Book2D(string name, string title,int nBins, double*xbound,int nBins2, double*ybound)
{
    output_ -> Book2D(name,title,nBins,xbound,nBins2,ybound);
}

void AnalysisBase::Book(string name, string title, int nBins,double *xbound)
{
    output_ -> Book(name,title,nBins,xbound);
}


void AnalysisBase::Fill(string name,string syst, double value, double weight){
    output_ -> Fill(name,syst,value,weight);
}

void AnalysisBase::Fill2D(string name,string syst, double valueX,double valueY, double weight){
    output_ -> Fill2D(name,syst,valueX,valueY,weight);
}

void AnalysisBase::Fill3D(string name,string syst, double valueX,double valueY, double valueZ,double weight){
    output_ -> Fill3D(name,syst,valueX,valueY,weight);
}

TH1D* AnalysisBase::GetHisto(string name,string systname)
{
    return output_ -> Get(name,systname);	
}

TH2D* AnalysisBase::GetHisto2D(string name,string systname)
{
    return output_ -> Get2D(name,systname);	
}

string AnalysisBase::GetLabel(Event *e){

    // remember if just done
    //static int lastRun = -1;
    //static string label = "";
    string label = "";

    //if (lastRun == e->runNum() ) 
    //	return label;

    label = "Other";
    
    if (multipd_ and  e->IsRealData() )
    {
        // filename
        label = "Data"; // general backup option
        for (string& s:  AllLabel() )
        {
            if ( s== "Data") continue;
            if (e->GetName().find(s) != string::npos ) 
            {
                label=s;
                
                // automatize
                if (label == "SingleMuon") pd_ = SingleMuon;
                else if (label == "SingleElectron") pd_ = SingleElectron;
                else if (label == "SinglePhoton") pd_ = SinglePhoton;
                else if (label == "BTagCSV") pd_ = BTagCSV;
                else if (label == "JetHT") pd_ = JetHT;
                else if (label == "DoubleMuon") pd_ = DoubleMuon;
                else if (label == "DoubleElectron") pd_ = DoubleElectron;
                else pd_ = None;

                break; // match the first
            }
        }
    }
    else if (e->IsRealData()) 
    {
        label = "Data";
    }
    else{
        for (string& s:  AllLabel() )
        {
            if (e-> GetWeight() -> GetMC() . find(s) != string::npos ) {
                label = s; 
                break;
            }
        }
            
    } // end else (MC)

    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","mc is '" + e->GetWeight()->GetMC() +"' and label is '"+ label +"'");
    #endif
    return label;
}

//#include "interface/Logger.hpp"
//void AnalysisBase::Log(const string& function, const string& level, const string& message){ Logger::getInstance().Log(this,function,level,message ); }

//#define VERBOSE 2
void AnalysisBase::SetCuts(Event *e)
{
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for Leptons");
    #endif
    for (auto& l : e->leps_) SetLeptonCuts(l);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for Photons");
    #endif
    for (auto& p : e->phos_) SetPhotonCuts(p);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for Taus");
    #endif
    for (auto& t : e->taus_) SetTauCuts(t);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for Jets");
    #endif
    for (auto& j : e->jets_) SetJetCuts(j);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for Fat Jets");
    #endif
    for (auto& j : e->fat_) SetFatJetCuts(j);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts for GP");
    #endif
    for (auto& g : e->genparticles_) SetGenCuts(g);
    #ifdef VERBOSE
    if (VERBOSE>1) Log(__FUNCTION__,"DEBUG","->Setting cuts DONE");
    #endif
    return;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
