#include "interface/Weight.hpp"
//#define VERBOSE 1

#include "interface/Logger.hpp"
void Weight::Log(const string& function, const string& level, const string& message) const { Logger::getInstance().Log("Weight",function,level,message ); }

void Weight::PrintInfo() {
    Log(__FUNCTION__,"INFO","-------------------------------------");

    Log(__FUNCTION__,"INFO","mcName:" + mcName_);
    Log(__FUNCTION__,"INFO",Form("mcXsec:%lf",mcXsec_));
    Log(__FUNCTION__,"INFO",Form("SumW:%lf",nEvents_));
    Log(__FUNCTION__,"INFO",Form("SF:%lf",sf_));
    Log(__FUNCTION__,"INFO",Form("mcWeight:%lf",mcWeight_));
    Log(__FUNCTION__,"INFO",Form("lumi:%lf",lumi_));
    Log(__FUNCTION__,"INFO",Form("puInt:%lf",puInt_));
    Log(__FUNCTION__,"INFO",Form("scales=%d pdfs=%d",int(scales_),int(pdfs_)));
    Log(__FUNCTION__,"INFO",Form("syst=%d",int(syst) ) );
    Log(__FUNCTION__,"INFO",Form("systPdf=%d",int(systPdf) ) );
    Log(__FUNCTION__,"INFO",Form("pu=%lf", pu_.GetPUWeight(mcName_,puInt_,runNum_) ) );
    Log(__FUNCTION__,"INFO",Form("weight=%le", doWeight() ) );
    
    if (syst == MC::none and systPdf <0 ) 
    {
        //return mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)/ nEvents_; 
        Log(__FUNCTION__,"INFO","--- CENTRAL WEIGHTS ---" );
        Log(__FUNCTION__,"INFO",Form("step 1=%lf",mcWeight_* mcXsec_));
        Log(__FUNCTION__,"INFO",Form("step 2=%lf",mcWeight_* mcXsec_ *lumi_));
        Log(__FUNCTION__,"INFO",Form("step 3=%lf",mcWeight_* mcXsec_ * lumi_ * sf_));
        Log(__FUNCTION__,"INFO",Form("step 4=%lf",mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_)));
        Log(__FUNCTION__,"INFO",Form("step 5=%lf",mcWeight_* mcXsec_ * lumi_ * sf_ * pu_.GetPUWeight(mcName_,puInt_,runNum_) / nEvents_));

    }

    if (syst == MC::none and systPdf>=0)
    {
        Log(__FUNCTION__,"INFO",Form("step 6 (W) =%lf",pdfsWeights_[systPdf] * pdfsNeventReweight_[systPdf]));
        Log(__FUNCTION__,"INFO",Form("step 7 (NeRW)=%lf",pdfsNeventReweight_[systPdf]));
    
    }

    if (syst != MC::none and systPdf<0)
    {
        Log(__FUNCTION__,"INFO",Form("step 6 (W) =%lf",scalesWeights_[syst] * pdfsNeventReweight_[syst]));
        Log(__FUNCTION__,"INFO",Form("step 7 (NeRW)=%lf",scalesNeventReweight_[syst]));
    
    }

    if (syst != MC::none){
        Log(__FUNCTION__,"INFO"," --- SCALES ---");
        for(int i=0;i< MC_MAX_SCALES ;++i)  
            Log(__FUNCTION__,"INFO",Form("   scale %d: w=%lf NeventRwgt=%lf",i,scalesWeights_[i],scalesNeventReweight_[i]));
    }
    if (systPdf >=0 ){
        Log(__FUNCTION__,"INFO"," --- PDFS ---");
        for(int i=0;i< MC_MAX_PDFS ;++i)  
            Log(__FUNCTION__,"INFO",Form("   pdf %d: w=%lf NeventRwgt=%lf",i,pdfsWeights_[i],pdfsNeventReweight_[i]));
    }
    Log(__FUNCTION__,"INFO","-------------------------------------");
}


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

void Weight::AddPtEtaRunSF( string label,double pt1, double pt2 , double eta1, double eta2, unsigned long run1, unsigned long run2, double sf, double err){
    #ifdef VERBOSE
        if(VERBOSE>0) cout <<"[Weight]::[AddPtEtaRunSF]::[DEBUG1] adding sf label '"<<label<<"'"<<endl;
    #endif
    if ( sf_db.find(label) == sf_db.end() )
    {
        sf_db[label] = new SF_PtEtaRun();
        sf_db[label]->label= label;
        #ifdef VERBOSE
            if(VERBOSE>0) cout <<"[Weight]::[AddPtEtaRunSF]::[DEBUG1] Constructing PtEtaRun"<<endl;
        #endif
    }
    SF_PtEtaRun *p =  dynamic_cast<SF_PtEtaRun*> ( sf_db[label] );
    if (p == NULL)
        cout <<"[Weight]::[AddPtEtaRunSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;

    p->add(pt1, pt2, eta1,eta2,run1,run2,sf,err);
    return;
}
void Weight::AddSplineSF(string label, double pt, double sf, double err)
{
    if(sf_db.find(label ) == sf_db.end() )
    {
        sf_db[label] = new SF_PtSpline();
        sf_db[label]->label = label;
    }
    SF_PtSpline *p = dynamic_cast<SF_PtSpline*>( sf_db[label] );
    if (p==NULL) Log(__FUNCTION__,"ERROR","SF"+ label + "is not PtSpline");
    p->add(pt,sf,err);
}

void Weight::AddCSVSF(string label, string filename)
{
    if (sf_db.find(label) != sf_db.end() )
    {
        Log(__FUNCTION__,"ERROR","SF "+ label +" already exists in the database. Not supported for CSV. Ignoring.");
        return;
    }
    SF_CSV *p = new SF_CSV();
    p -> init(filename);
    p -> label = label;
    sf_db[label] = p;
}

void Weight::AddTh2fSF(string label, string filename)
{
    if (sf_db.find(label) != sf_db.end() )
    {
        Log(__FUNCTION__,"ERROR","SF "+ label +" already exists in the database. Not supported for Th2f.");
        return;
    }
    SF_TH2F *p = new SF_TH2F();
    if (filename.find(":") !=string::npos)
    {
        string fname=filename.substr(0,filename.find(":"));
        string hname=filename.substr(filename.find(":")+1);  
        if (hname.find(":") == string::npos) p->init(fname,hname);
        else { // errhist is present
            string errname = hname.substr( hname.find(":") +1);
            hname  = hname.substr(0,hname.find(":") );
            p->init(fname,hname,errname);
        }
    }
    else
    {
        p -> init(filename);
    }
    p -> label = label;
    sf_db[label] = p;
}

void Weight::AddTF1SF(string label, string formula,string errFormula)
{
    if (sf_db.find(label) != sf_db.end() )
    {
        Log(__FUNCTION__,"ERROR","SF "+ label +" already exists in the database. Not supported for TF1.");
        return;
    }
    SF_TF1 *p = new SF_TF1();
    p -> label = label; // before formula
    p -> init(formula,errFormula);
    sf_db[label] = p;
}
void Weight::AddTF2SF(string label, string formula,string errFormula)
{
    if (sf_db.find(label) != sf_db.end() )
    {
        Log(__FUNCTION__,"ERROR","SF "+ label +" already exists in the database. Not supported for TF2.");
        return;
    }
    SF_TF2 *p = new SF_TF2();
    p -> label = label; // before formula
    p -> init(formula,errFormula);
    sf_db[label] = p;
}


void Weight::resetSystSF(){
    for (auto o : sf_db)
        o.second->reset();
    //o.second->syst = 0;
}

void Weight::SetPtEtaSF(const string& label,double pt, double eta)
{
    #ifdef VERBOSE
        if(VERBOSE>0) cout <<"[Weight]::[SetPtEtaSF]::[DEBUG1] label='"<<label<<"'"<<endl;
    #endif
    SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
    SF_PtSpline *p2 =  dynamic_cast<SF_PtSpline*> ( sf_db[label] );
    SF_CSV *p3 =  dynamic_cast<SF_CSV*> ( sf_db[label] );
    SF_TF1 *p4 =  dynamic_cast<SF_TF1*> ( sf_db[label] );
    SF_TF2 *p5 =  dynamic_cast<SF_TF2*> ( sf_db[label] );

    if (p == NULL and p2 == NULL and p3==NULL and p4== NULL and p5==NULL)
        Log(__FUNCTION__,"ERROR", " SF '" + label + "' is not Pt Eta dependent or pt spline, or csv" );

    if (p) p->set(pt,eta);
    if (p2) p2->set(pt);
    if (p3) p3->set(pt,eta);
    if (p4) p4->set(pt);
    if (p5) p5->set(pt,eta);
    return;
}

// ------------- CVS  ---
void Weight::SetWPSF(const string& label, int wp)
{
    SF_CSV *p =  dynamic_cast<SF_CSV*> ( sf_db[label] );
    if (p==NULL) Log(__FUNCTION__,"ERROR", " SF '" + label + "' is not CSV" );
    p->setWP(wp);
}
void Weight::SetJetFlavorSF(const string& label, int flavor)
{
    SF_CSV *p =  dynamic_cast<SF_CSV*> ( sf_db[label] );
    if (p==NULL) Log(__FUNCTION__,"ERROR", " SF '" + label + "' is not CSV" );
    p->setJetFlavor(flavor);
}

// -----------------------
string Weight::LoadMC( string label) 
{ 
    if (label == "data" or label == "Data" or label == "Tau" or label == "MET" or label == "SingleElectron" or label == "SingleMuon")
    {
        mcName_ = label;
        mcXsec_ = 1.0;
        nEvents_ = 1.0;
        for (int i=0; i<MC_MAX_SCALES;++i) scalesWeights_[i] =1.0;
        for (int i=0; i<MC_MAX_SCALES;++i) scalesNeventReweight_[i] =1.0;
        for (int i=0; i<MC_MAX_PDFS;++i) pdfsWeights_[i] =1.0;
        for (int i=0; i<MC_MAX_PDFS;++i) pdfsNeventReweight_[i] =1.0;
        return "data";
    }

    if( mc_db.find(label) == mc_db.end() )
        return "";
    mcName_ = label; 
    mcXsec_= mc_db[label]->xsec; 
    nEvents_ = mc_db[label] -> nevents; 
    
    // Load Scale info
    auto iter = mc_db.find(label);
    for (int i=0; i<MC_MAX_SCALES;++i) scalesNeventReweight_[i] = iter->second->scalesNeventsReweight[i];
    for (int i=0; i<MC_MAX_PDFS;++i) pdfsNeventReweight_[i] = iter->second->pdfsNeventsReweight[i];

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

void Weight::ApplySF(const string& label){
        if (sf_db[label] -> get() < 1e-5) 
        {
            Log(__FUNCTION__,"WARNING",Form("SF for %s is very little: %f",label.c_str(),sf_db[label] -> get() ));
            return;
        }
        //Log(__FUNCTION__,"DEBUG",Form("Apply SF for '%s': %f",label.c_str(),sf_db[label] -> get() ));
        sf_ *= sf_db[label] -> get(); 
    }

void Weight::SetSystSF( const string & label, int s)
{
    if ( not ExistSF(label) ) Log(__FUNCTION__,"ERROR", string("SF syst ") + label+" not in the sf db" );
    sf_db[label] -> syst = s;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
