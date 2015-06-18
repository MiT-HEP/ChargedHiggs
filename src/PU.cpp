#include "interface/PU.hpp"
#include "TStopwatch.h"
//#define VERBOSE 2

PUunit::PUunit(){ runMin = -1 ; runMax=-1; hist = NULL; lumi = -1;}
PUunit::~PUunit(){ if(hist) delete hist; }

PU::PU(){ltot=0;syst=0;}

void PU::AddToContainer(string label, TH1*h, int runMin, int runMax,double lumi)
{
    h->Scale(1./h->Integral());

    if ( container.find(label) == container.end() )
        container[label]=new vector<PUunit*>() ;

    auto v = container[label] ; // SAVE SEARCH
    size_t pos= v->size();
    v -> push_back( new PUunit() );

    v -> at(pos) -> hist=h;
    v -> at(pos) -> runMin=runMin;
    v -> at(pos) -> runMax=runMax;
    if (runMin >=0 and runMax >=0 and lumi >=0 ) 
    {
        v -> at(pos) -> lumi = lumi;
        ltot += lumi;
    }
}

void PU::AddTarget( TH1* h, int runMin, int runMax,double lumi)
{
    AddToContainer("target",h,runMin,runMax,lumi);
}

void PU::AddTarget( TH1* h,string systName, int runMin, int runMax,double lumi)
{
    if (systName != "Up" and systName!="Down")
        cout<<"[PU]::[AddTarget]::[ERROR] systName '"<<systName<<"' can be just 'Up' or 'Down'"<<endl;
    AddToContainer("target_" + systName,h,runMin,runMax,lumi);
}

void PU::AddMC(string label,TH1*h,int runMin,int runMax)
{
    if(label.find("target") != string::npos ) 
    {
        cout<<"[PU]::[AddMC]::[ERROR] MC label cannot be named or contain 'target' "<<endl;
    }
    AddToContainer(label,h,runMin,runMax);
}

bool PU::IsInRange(int run, int runMin, int runMax)
{
    if(run<0 or runMin<0 or runMax <0 ) return true;
    if (runMin<= run and run <=runMax) return true;
    return false;
}

double PU::GetPUWeight(string label, float x, int run)
{
#ifdef VERBOSE
    if(VERBOSE>1) cout<<"[PU]::[GetPUWeight]::[DEBUG] "<<endl;
#endif
    if ( label == "data" or label == "Data" ) 
    {
#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[PU]::[GetPUWeight]::[DEBUG] Returning data weight 1"<<endl;
#endif
        return 1;
    }
    // This function is very slow. Let's cache a bit.
    static int lastSyst=-100; //CACHE
    static vector<PUunit*>  *lastTarget=NULL; //CACHE
    static vector<PUunit*>  *lastMC=NULL; //CACHE
    static string lastMClabel=""; //CACHE

    vector<PUunit*> *currentTarget=NULL;
    vector<PUunit*> *currentMC=NULL;
    // CACHE
    if (lastSyst == syst) {
        currentTarget=lastTarget;
    }
    if (lastMClabel == label ) 
    {
        currentMC=lastMC;
    }
    if ( not currentTarget)
    {
        string targetName = "target";
        if (syst>0) targetName = "target_Up";
        if (syst<0) targetName = "target_Down";

        auto container_target =  container.find(targetName); // SAVE SEARCH
        if(container_target == container.end() ) 
        { 
            static int msgLog0 = 0;
            msgLog0 ++;
            if(msgLog0 <10 ) 
            {
                cout<<"[PU]::[GetPUWeight]::[ERROR] NO TARGET '"<<targetName<<"'= NOT REWEIGHTING !"<<endl; 
                for( auto e : container) cout<<" MAP contains: "<<e.first<<endl;
            }
            return 1;
        }
        currentTarget = container_target -> second;
        lastTarget=currentTarget;
        lastSyst=syst;
    } // end not currentTarget

    if (not currentMC)
    {
        auto container_mc =  container.find(label); //SAVE SEARCH
        if( container_mc == container.end() )
        {
            static int msgLog1 = 0;
            static string msgLabel1 = label;
            if (label != msgLabel1) 
            {
                msgLog1=0;
                msgLabel1 = label;
            }
            if(msgLog1<10) 
            {
                cout<<"[PU]::[GetPUWeight]::[ERROR] NO REWEIGHT FOR MC "<<label<<" = NOT REWEIGHTING! "<<endl;
                for( auto e : container) cout<<" MAP contains: "<<e.first<<endl;
            }
        }
        currentMC = container_mc -> second;
        lastMC=currentMC;
        lastMClabel = label;
    }//end currentMC
    /// ----------------- CHECK DONE --------
    TH1 *target=NULL;
    TH1 *mc=NULL;
    double l = -1;
    //	for(PUunit* p : container[targetName])
    for(PUunit* p : *currentTarget)
    {
        if (IsInRange(run, p->runMin,p->runMax) )
        {
            target=p->hist;
            l = p->lumi; // only relevant for data
        }
    }
    //for(PUunit* p : container[label])
    for(PUunit* p : *currentMC)
    {
        if (IsInRange(run, p->runMin,p->runMax) )
            mc=p->hist;
    }

    if (target ==NULL) cout<<"[PU]::[GetPUWeight]::[ERROR] Null target PU Syst="<<syst <<endl;
    if (mc ==NULL) cout<<"[PU]::[GetPUWeight]::[ERROR] Null mc PU " <<label<<endl;

    // scale them
    //target ->Scale(target->Integral() );
    //mc ->Scale(mc->Integral() );

    double num= target->GetBinContent( target->FindBin(x) );
    double den= mc->GetBinContent( mc->FindBin(x) );	

    double w=num/den ;
    if (l>=0 ) w *= l / ltot;
#ifdef VERBOSE
    if(VERBOSE>1) cout<<"[PU]::[GetPUWeight]::[DEBUG] Returning weight w="<<w<<endl;
#endif
    return w;
}

void PU::clear(){
    for (auto vp : container )
    {
        for( auto p : *vp.second )
            delete p;
        vp.second->clear();
    }
    container.clear();
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
