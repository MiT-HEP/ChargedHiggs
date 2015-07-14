#include "interface/Output.hpp"
#include <iostream>
//#define VERBOSE 2
using namespace std;
//
void Output::Fill(string name, string syst , double value, double weight)
{
    string fullname = name + "_" + syst;
    if ( syst == "" or syst == "NONE")
        fullname = name;

    if ( histos_.find(fullname) == histos_.end() )
    {
        if (histos_.find(name) == histos_.end() )
            cout<<"[Output]::[Fill]::[ERROR] Histo "<<name<<" not booked !"<<endl;
        histos_[ fullname ]  = (TH1D*) histos_ [name] -> Clone(fullname.c_str());
        histos_[ fullname ] -> Reset("ICESM");
    }
    histos_[fullname] ->Fill(value,weight);

    return;
}

void Output::Fill2D(string name, string syst , double valueX,double valueY, double weight)
{
    string fullname = name + "_" + syst;
    if ( syst == "" or syst == "NONE")
        fullname = name;

    if ( histos2D_.find(fullname) == histos2D_.end() )
    {
        if (histos2D_.find(name) == histos2D_.end() )
            cout<<"[Output]::[Fill]::[ERROR] Histo 2D "<<name<<" not booked !"<<endl;
        histos2D_[ fullname ]  = (TH2D*) histos2D_ [name] -> Clone(fullname.c_str());
        histos2D_[ fullname ] -> Reset("ICESM");
    }
    histos2D_[fullname] ->Fill(valueX,valueY,weight);

    return;
}

void Output::Open(string name)
{ 
#ifdef VERBOSE
    if(VERBOSE >1 ) cout<<"[Output]::[Open]::[DEBUG] Opening file: '"<<name<<"'"<<endl;
#endif
    file_ = TFile::Open(name.c_str(),"RECREATE"); 
    if (file_ == NULL ) 
        cout<<"[Output]::[Open]::[ERROR] Unable to open file: '"<<name<<"'"<<endl;
    file_->cd();
#ifdef VERBOSE
    if(VERBOSE >1 ) cout<<"[Output]::[Open]::[DEBUG] DONE "<<endl;
#endif
    return;
}

void Output::Close()
{	
    Write();
    if(file_)  file_->Close() ; 
    file_= NULL;
}

void Output::CreateDir(string dir)
{
    // It's already recursive
    file_->mkdir(dir.c_str());

}

void Output::Write(){ 
    if(file_) file_->cd() ; 
    else return;

    for(auto m : histos_) 
    {
        if ( m.first.find("/") != string::npos) // there is a directory
        {
            size_t  last = m.first.rfind("/");
            string dir = m.first.substr(0,last); // substr get len
            string name = m.first.substr(last+1,string::npos);
            //if (! file_ ->cd (dir.c_str()) )
            if ( file_ -> Get(dir.c_str()) == NULL )
            {
                CreateDir(dir);
            }
            file_ ->cd (dir.c_str()) ;
            m.second->Write( name.c_str()) ; 
        }
        else
        {
            file_->cd() ; 
            m.second->Write( m.first.c_str()) ; 
        }
#ifdef VERBOSE
        if (VERBOSE>1) {
            cout <<"[Output]::[Write]::[DEBUG] Writing Histo "<< m.first<<endl;
            m.second->Print("all");
            cout <<"[Output]::[Write]::[DEBUG] -------------- "<< m.first<<endl;
        }
#endif
    }

    for(auto m : histos2D_) 
    {
        if ( m.first.find("/") != string::npos) // there is a directory
        {
            size_t  last = m.first.rfind("/");
            string dir = m.first.substr(0,last); // substr get len
            string name = m.first.substr(last+1,string::npos);
            //if (! file_ ->cd (dir.c_str()) )
            if ( file_ -> Get(dir.c_str()) == NULL )
            {
                CreateDir(dir);
            }
            file_ ->cd (dir.c_str()) ;
            m.second->Write( name.c_str()) ; 
        }
        else
        {
            file_->cd();
            m.second->Write( m.first.c_str()) ; 
        }
    }
}

void Output::Book(string name, string title,int nBins, double xmin, double xmax)
{ 
    if ( Exists(name) )
        cout <<"[Output]::[Book]::[ERROR] a TH1D/TH2D histo with the same name '"<<name<<"' already exist"<<endl;
    histos_ [name] = new TH1D(name.c_str(),
            title.c_str(), 
            nBins,xmin,xmax); 
    histos_ [name] -> Sumw2();
}

void Output::Book2D(string name, string title,int nBins, double xmin, double xmax,int nBins2, double ymin,double ymax)
{ 
    if ( Exists(name) )
        cout <<"[Output]::[Book2D]::[ERROR] a TH1D/TH2D histo with the same name '"<<name<<"' already exist"<<endl;

    histos2D_ [name] = new TH2D(name.c_str(),
            title.c_str(), 
            nBins,xmin,xmax,
            nBins2,ymin,ymax
            ); 
    histos2D_ [name] -> Sumw2();
}

void Output::Book(string name, string title,int nBins, double *xbound)
{ 
    if ( Exists(name) )
        cout <<"[Output]::[Book]::[ERROR] a TH1/TH2 histo with the same name '"<<name<<"' already exist"<<endl;

    histos_ [name] = new TH1D(
            name.c_str(),
            title.c_str(), 
            nBins,xbound); 
    histos_ [name ] -> Sumw2();
}

TH1D* Output::Get(string name,string systname)
{ 
    if (systname == "" or systname == "NONE") 
        return Get(name);
    return Get(name + "_" + systname);
}

TH2D* Output::Get2D(string name,string systname)
{ 
    if (systname == "" or systname == "NONE") 
        return Get2D(name);
    return Get2D(name + "_" + systname);
}

bool Output::Exists(string name){
    if ( histos2D_.find(name) != histos2D_.end() ) return true;
    if ( histos_.find(name) != histos_.end() ) return true;
    return false;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
