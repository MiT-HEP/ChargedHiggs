#include "interface/Output.hpp"
#include "interface/Logger.hpp"
#include <iostream>
//#define VERBOSE 2
using namespace std;
//
void Output::Fill(string name, string syst , double value, double weight)
{
    if (not PassFinal(name) ) return ;

    string fullname = name + "_" + syst;
    if ( syst == "" or syst == "NONE")
        fullname = name;

    if ( histos_.find(fullname) == histos_.end() )
    {
        if (prototypes_.find(name) == prototypes_.end() )
            cout<<"[Output]::[Fill]::[ERROR] Histo "<<name<<" not booked !"<<endl;
        histos_[ fullname ]  = (TH1D*) prototypes_ [name] -> Clone(fullname.c_str());
        histos_[ fullname ] -> Reset("ICESM");
    }
    histos_[fullname] ->Fill(value,weight);

    return;
}

void Output::Fill2D(string name, string syst , double valueX,double valueY, double weight)
{
    if (not PassFinal(name) ) return ;

    string fullname = name + "_" + syst;
    if ( syst == "" or syst == "NONE")
        fullname = name;

    if ( histos2D_.find(fullname) == histos2D_.end() )
    {
        if (prototypes2D_.find(name) == prototypes2D_.end() )
            cout<<"[Output]::[Fill]::[ERROR] Histo 2D "<<name<<" not booked !"<<endl;
        histos2D_[ fullname ]  = (TH2D*) prototypes2D_ [name] -> Clone(fullname.c_str());
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
    for(auto m : trees_) 
    {
            file_->cd();
            m.second->Write( m.first.c_str()) ; 
    }
}

void Output::Book(string name, string title,int nBins, double xmin, double xmax)
{ 
    if (not PassFinal(name) ) return ;

    if ( Exists(name) )
        cout <<"[Output]::[Book]::[ERROR] a TH1D/TH2D histo with the same name '"<<name<<"' already exist"<<endl;
    prototypes_ [name] = new TH1D( (name+"_proto").c_str(),
            title.c_str(), 
            nBins,xmin,xmax); 
    prototypes_ [name] -> Sumw2();
}

void Output::Book2D(string name, string title,int nBins, double xmin, double xmax,int nBins2, double ymin,double ymax)
{ 
    if (not PassFinal(name) ) return ;

    if ( Exists(name) )
        cout <<"[Output]::[Book2D]::[ERROR] a TH1D/TH2D histo with the same name '"<<name<<"' already exist"<<endl;

    prototypes2D_ [name] = new TH2D( (name+"_proto").c_str(),
            title.c_str(), 
            nBins,xmin,xmax,
            nBins2,ymin,ymax
            ); 
    prototypes2D_ [name] -> Sumw2();
}

void Output::Book(string name, string title,int nBins, double *xbound)
{ 
    if (not PassFinal(name) ) return ;

    if ( Exists(name) )
        cout <<"[Output]::[Book]::[ERROR] a TH1/TH2 histo with the same name '"<<name<<"' already exist"<<endl;

    prototypes_ [name] = new TH1D(
            (name+"_proto").c_str(),
            title.c_str(), 
            nBins,xbound); 
    prototypes_ [name ] -> Sumw2();
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
// ---------------------------- TREE -----------------
void Output::Branch(string tree,string name, char type,int MAXN,string num){ 
    bool isArray = (type =='d' or type=='f' or type =='i');

    // create variables
    if (isArray)
        varValues_.Add(name,type,MAXN); 
    else
        varValues_.Add(name,type); 
    // ---- create branches
    // array: fixed leghth, variable leght
    if (isArray and num =="") 
       trees_[tree]->Branch(name.c_str(), varValues_.GetPointer(name), (name+Form("[%d]",MAXN)+"/"+type).c_str() ) ;
    if (isArray and num !="") 
       trees_[tree]->Branch(name.c_str(), varValues_.GetPointer(name), (name+"["+num+"]/"+type).c_str() ) ;
        
    // float, int double
    if (type == 'D' or type =='F' or type == 'I')
       trees_[tree]->Branch(name.c_str(), varValues_.GetPointer(name), (name+"/"+type).c_str() ) ;
    // TLorentzVectors
    if (type == 'T')
        trees_[tree]->Branch(name.c_str(),"TLorentzVector", varValues_.GetPointer(name)) ;
}

// ----------------------------- DATA STORE -----------------------
DataStore::~DataStore(){
    for(auto& p : valuesP4_ ) delete p.second;
    valuesP4_.clear();
    valuesD_.clear();
    valuesI_.clear();
    valuesF_.clear();
}


bool DataStore::Exists(string name)
{
   if( valuesD_.find( name ) != valuesD_.end() ) return true;
   if( valuesF_.find( name ) != valuesF_.end() ) return true;
   if( valuesI_.find( name ) != valuesI_.end() ) return true;
   if( valuesP4_.find( name ) != valuesP4_.end() ) return true;
   if( valuesDD_.find( name ) != valuesDD_.end() ) return true;
   if( valuesFF_.find( name ) != valuesFF_.end() ) return true;
   if( valuesII_.find( name ) != valuesII_.end() ) return true;
   return false;
}
void DataStore::Add(string name, char type)
{
    if (Exists(name)) return;
    switch (type)
    {
        case 'F': valuesF_[name] = 0.0; break;
        case 'D': valuesD_[name] = 0.0; break;
        case 'I': valuesI_[name] = 0;   break;
        case 'T': valuesP4_[name] = new TLorentzVector(0,0,0,0); break;
        //arrays
        case 'd': Add(name,type,10); break;
        case 'f': Add(name,type,10); break;
        case 'i': Add(name,type,10); break;

        default: Logger::getInstance().Log("DataStore","Add","ERROR", string("type '") + type + "' not defined" ); break;
    }
    return;
}

void DataStore::Add(string name, char type, int N)
{
    if(Exists(name)) return;

    switch (type)
    {
        //arrays
        case 'd': valuesDD_[name] . reset (new double [N]); break;
        case 'f': valuesFF_[name] . reset (new float [N] ); break;
        case 'i': valuesII_[name] . reset (new int [N] ); break;

        default: Logger::getInstance().Log("DataStore","Add","ERROR", string("type '") + type + "' not defined for arrays" ); break;
    }
}

void* DataStore::GetPointer(string name){
   if( valuesD_.find( name ) != valuesD_.end() ) return &valuesD_[ name ];
   if( valuesF_.find( name ) != valuesF_.end() ) return &valuesF_[ name ];
   if( valuesI_.find( name ) != valuesI_.end() ) return &valuesI_[ name ];
   if( valuesP4_.find( name ) != valuesP4_.end() ) return &valuesP4_[ name ];
   if( valuesDD_.find( name ) != valuesDD_.end() ) return valuesDD_[ name ].get();
   if( valuesFF_.find( name ) != valuesFF_.end() ) return valuesFF_[ name ].get();
   if( valuesII_.find( name ) != valuesII_.end() ) return valuesII_[ name ].get();
   return NULL;
}

void DataStore::Print(){
    cout <<" ---- DATASTORE ----"<<endl;
    for(const auto& p :valuesD_ ) cout<<p.first<<"| 'D': "<<p.second<<endl;
    for(const auto& p :valuesF_ ) cout<<p.first<<"| 'F': "<<p.second<<endl;
    for(const auto& p :valuesI_ ) cout<<p.first<<"| 'I': "<<p.second<<endl;
    for(const auto& p :valuesP4_ ) cout<<p.first<<"| 'T': x="<<p.second->Px()<< " y="<< p.second->Py()<< " z=" << p.second->Pz() << " t="<<p.second->E() <<endl;
    for(const auto& p :valuesDD_ ) cout<<p.first<<"| 'DD'[0]: "<<p.second.get()[0]<<endl;
    for(const auto& p :valuesFF_ ) cout<<p.first<<"| 'FF'[0]: "<<p.second.get()[0]<<endl;
    for(const auto& p :valuesII_ ) cout<<p.first<<"| 'II'[0]: "<<p.second.get()[0]<<endl;
    cout <<" -------------------"<<endl;
}

// ------------------ SET Template spec ---------------
template<>
void DataStore::Set<TLorentzVector>(string name, const TLorentzVector & value)
{
   if( valuesP4_.find( name ) != valuesP4_.end() ) 
            (* valuesP4_[name]) = TLorentzVector( value ) ;
   return ;
}

template<>
void DataStore::Set<TLorentzVector*>(string name, TLorentzVector* const & value) { Set(name,*value); } // if in the .h, will be compiled in all the .o


// -------------------------------------------------------------------------------

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
