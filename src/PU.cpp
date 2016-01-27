#include "interface/PU.hpp"
#include "TStopwatch.h"
#include "interface/Logger.hpp"

#define LogMe(type,message) Logger::getInstance().Log(typeid(*this).name(),__FUNCTION__,type,message)
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
try{
#ifdef VERBOSE
    LogMe("DEBUG","Start");
#endif
    if ( label == "data" or label == "Data" ) 
    {
#ifdef VERBOSE
    LogMe("DEBUG","Data : Returning PUWeight = 1 ");
#endif
        return 1;
    }

    string targetName = "target";
    if (syst>0) targetName = "target_Up";
    if (syst<0) targetName = "target_Down";

#ifdef VERBOSE
    LogMe("DEBUG","Target is ='"+targetName+"'");
#endif

    // This function is very slow. Let's cache a bit.
    static int lastSyst=-100; //CACHE
    static vector<PUunit*>  *lastTarget=NULL; //CACHE
    static vector<PUunit*>  *lastMC=NULL; //CACHE
    static string lastMClabel=""; //CACHE

    vector<PUunit*> *currentTarget=NULL;
    vector<PUunit*> *currentMC=NULL;

    // CHECK IF CHACHED IS OK
    if (lastSyst == syst) {
        currentTarget=lastTarget;
        #ifdef VERBOSE
            LogMe("DEBUG",Form("Using lastTarget address=%lx", lastTarget)  );
        #endif
    }
    if (lastMClabel == label ) 
    {
        currentMC=lastMC;
        #ifdef VERBOSE
            LogMe("DEBUG",Form("Using lastMC address=%lx", lastMC)  );
        #endif
    }

    if ( currentTarget == NULL)
    {
        #ifdef VERBOSE
            LogMe("DEBUG",Form("Reloading current Target")  );
        #endif

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

    if ( currentMC == NULL)
    {
        #ifdef VERBOSE
            LogMe("DEBUG",Form("Reloading current MC")  );
        #endif
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

    #ifdef VERBOSE
        LogMe("DEBUG",Form("Caching check done. Getting histos")  );
    #endif
    /// ----------------- CHECK DONE --------
    TH1 *target=NULL; // get histos
    TH1 *mc=NULL;
    double l = -1;

    int targetNum=0;
    for(PUunit* p : *currentTarget)
    {
        if (IsInRange(run, p->runMin,p->runMax) )
        {
            target=p->hist;
            l = p->lumi; // only relevant for data
            break;
        }
        targetNum += 1;
    }


    for(PUunit* p : *currentMC)
    {
        if (IsInRange(run, p->runMin,p->runMax) )
            mc=p->hist;
    }

    if (target ==NULL) cout<<"[PU]::[GetPUWeight]::[ERROR] Null target PU Syst="<<syst <<endl;
    if (mc ==NULL) cout<<"[PU]::[GetPUWeight]::[ERROR] Null mc PU " <<label<<endl;

    #ifdef VERBOSE
        LogMe("DEBUG",Form("TargetNum=%d/%u",targetNum,currentTarget->size())  );
        LogMe("DEBUG","Getting normalization factors" );
    #endif

    // scale them
    //target ->Scale(target->Integral() );
    //mc ->Scale(mc->Integral() );
   
    // get normalization factors to preserve xSec
    string normName=Form("%s,%s",label.c_str(),targetName.c_str());

    vector<double>* currentNorm ;
    if ( norm.find(normName) == norm.end())
    {
        norm[ normName ] = new vector<double>();
    }
    currentNorm = norm[ normName ];

    #ifdef VERBOSE
        LogMe("DEBUG","NormName='" + normName +"'" );
    #endif

    if ( currentNorm->size() < targetNum +1 ) currentNorm->resize(targetNum+1,-1);

    if (currentNorm->at(targetNum) <0){
        target -> Scale(1./target->Integral() );     
        mc -> Scale( 1./mc ->Integral() );
        double sum= 0.;
        
        for(int i=0; i <= target->GetNbinsX() + 1 ; ++i)
        {
            double num= target -> GetBinContent( i);  
            double den= mc -> GetBinContent( mc->FindBin( target->GetBinCenter(i) ) );  
            double rw = Ratio(num,den); 
            sum += den*rw;
        }
        currentNorm->at(targetNum) = sum;
        cout <<"[PU]::[GetPUWeight]::[INFO]  |B| Target='"<<targetName << "' label='" << label << "' targetNum="<<targetNum<<" reweight Sum ="<<sum<<endl;
    }

    double sum = currentNorm->at(targetNum);
    double num= target->GetBinContent( target->FindBin(x) );
    double den= mc->GetBinContent( mc->FindBin(x) );	

    double w = Ratio(num,den);
    w /= sum;

    if (l>=0 ) w *= l / ltot;
#ifdef VERBOSE
    if(VERBOSE>1) cout<<"[PU]::[GetPUWeight]::[DEBUG] Returning weight w="<<w<<" | "<<num<<"/"<<den<<"*"<<sum<<endl;
#endif
    return w;
}
catch (std::exception &e)
    {
        cout <<"[PU]::[GetPUWeight]::[Exception]"<<endl;
        cout <<e.what() <<endl;
        throw e;
    }
// -----
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
