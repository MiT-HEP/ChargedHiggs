#include "interface/Weight.hpp"
//#define VERBOSE 1

void Weight::AddMC( string label, string dir, double xsec, double nevents)
{
    mc_db[label] = new MC();
    mc_db[label]->dir =dir;
    mc_db[label]->xsec = xsec;
    mc_db[label]->nevents = nevents;
}

void Weight::AddSF( string label, double sf, double err){
    sf_db[label] = new SF();
    sf_db[label]->sf = sf;
    sf_db[label]->err = err;
    sf_db[label]->syst = 0;
    sf_db[label]->label = label;
}

void Weight::AddPtEtaSF( string label,double pt1, double pt2 , double eta1, double eta2, double sf, double err){
    #ifdef VERBOSE
        if(VERBOSE>0) cout <<"[Weight]::[AddPtEtaSF]::[DEBUG1] adding sf label '"<<label<<"'"<<endl;
    #endif
    if ( sf_db.find(label) == sf_db.end() )
    {
        sf_db[label] = new SF_PtEta();
        sf_db[label]->label= label;
        #ifdef VERBOSE
            if(VERBOSE>0) cout <<"[Weight]::[AddPtEtaSF]::[DEBUG1] Constructing PtEta"<<endl;
        #endif
    }
    SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
    if (p == NULL)
        cout <<"[Weight]::[AddPtEtaSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;

    p->add(pt1, pt2, eta1,eta2,sf,err);
    return;
}

void Weight::resetSystSF(){
    for (auto o : sf_db)
        o.second->syst = 0;
}

void Weight::SetPtEtaSF(string label,double pt, double eta)
{
    #ifdef VERBOSE
        if(VERBOSE>0) cout <<"[Weight]::[SetPtEtaSF]::[DEBUG1] label='"<<label<<"'"<<endl;
    #endif
    SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
    if (p == NULL)
        cout <<"[Weight]::[SetPtEtaSF]::[ERROR] SF '"<<label<<"' is not Pt Eta dependent"<<endl;

    #ifdef VERBOSE
        if(VERBOSE>0)cout <<"[Weight]::[SetPtEtaSF]::[DEBUG1] p->label='"<<p->label<<"'"<<endl;
    #endif 

    p->set(pt,eta);
    return;
}


string Weight::LoadMC( string label) 
{ 
    if (label == "data" or label == "Data")
    {
        mcName_ = label;
        mcXsec_ = 1.0;
        nEvents_ = 1.0;
        return "data";
    }

    if( mc_db.find(label) == mc_db.end() )
        return "";
    mcName_ = label; 
    mcXsec_= mc_db[label]->xsec; 
    nEvents_ = mc_db[label] -> nevents; 
    cout<<"[Weight]::[LoadMC]::[INFO] Loaded MC with weigths:"<<mcName_<<"| xSec "<<mcXsec_ <<" | SumW "<<nEvents_<<" | sf"<<sf_<<" | lumi "<<lumi_<<endl;
    return mc_db[label]->dir;
}
string Weight::LoadMCbyDir( string dir )	 // return "" if failed otherwise label
{
    string label = "";
    for( auto p : mc_db )
    {
        if(p.second -> dir == dir) 
        {
            label = p.first;
            break;
        }
    }
    LoadMC(label);
    return label;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
