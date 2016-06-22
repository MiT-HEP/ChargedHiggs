#ifndef LOOP_H
#define LOOP_H

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"


// Charged Higgs
#include "interface/AnalysisBase.hpp"
#include "interface/Smearer.hpp"
#include "interface/Corrector.hpp"
#include "interface/Event.hpp"
#include "interface/Dumper.hpp"

#include "interface/Loader.hpp"

// Bare Structures
#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "interface/Output.hpp"

#include "TStopwatch.h"
#include "interface/Logger.hpp"

#include <sstream>

class Looper{

    private:
        TChain *tree_; 
        int fNumber;

        //vector<BareCollection*> bare_; // should be in the Loader?
        //map<string,int> names_;

        vector<AnalysisBase*> analysis_;

        vector<SmearBase*> systs_;

        vector<CorrectorBase*> correctors_;

        Event * event_;

        Output *output_;

        TStopwatch sw_;

        Dumper *dump_;

        Loader* loader_; // translate the information in the ntuples in the information in the fwk

        void Log(const string& func, const string& state, const string& mex){ Logger::getInstance().Log(name(),func,state, mex); };

    protected:
        //
        void NewFile();
        // ---- call by LOOP
        void FillEvent();
        void ClearEvent();
        void Write(){output_->Write();}	
        void Close(){output_->Close();}

    public:
        // -- constructor
        Looper();
        ~Looper(){ClearEvent();}
        // ---
        const string name(){return "Looper";}
        // ---
        inline void InitLoader(string name){ 
                    loader_ = LoaderFactory::get().create(name); 
                    tree_=new TChain(loader_->chain().c_str());
        }
        inline int AddToChain( string name ){ return tree_ -> Add( name.c_str() ) ; }
        inline int AddAnalysis( AnalysisBase* a ) {analysis_ . push_back(a); return 0;}
        inline int AddSmear(SmearBase *s) { systs_ .push_back(s) ; return 0; }
        int AddSmear(string name);
        inline int AddCorrector(CorrectorBase *c) { correctors_.push_back(c) ; return 0;}
        //-------------
        // DUMPER
        void SetDumpDir(string dir){dump_->SetOutDir(dir);}
        void ActivateDump(bool x=true){dump_->ActivateDump(x);}
        //------------

        int InitTree () ;
        inline void ActivateBranch(string bname){ tree_ -> SetBranchStatus(bname.c_str(),1); return; }
        int InitSmear() ;
        int InitCorrector() ;
        int InitAnalysis() ;//{ for(auto a : analysis_ ) { a->SetOutput(output_); a->doInit() ;}  return 0;}
        int PrintInfo();
        int InitOutput(string name){output_ -> Open(name); return 0;}
        //
        void Loop();

        inline void AddMC( string label, string dir, double xsec, double nevents){event_ -> GetWeight() -> AddMC(label,dir,xsec,nevents); }
        inline void AddMCScale(string label, int x, double rw){ event_->GetWeight()->AddMCScale(label, MC::SCALES(x), rw);}
        inline void AddMCPdf(string label, int x, double rw){ event_->GetWeight()->AddMCPdf(label, x, rw);}
        inline void AddSF( string label, double sf, double err){ event_->GetWeight() -> AddSF(label,sf,err);}
        inline void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err)
        {event_ -> GetWeight()  -> AddPtEtaSF(label,pt1,pt2,eta1,eta2,sf,err); }
        inline void AddSplineSF(string label, double pt, double sf, double err){ event_->GetWeight() -> AddSplineSF(label,pt,sf,err);}
        inline void AddCSVSF(string label, string filename){event_->GetWeight() ->AddCSVSF(label,filename);}
        inline void AddTh2fSF(string label, string filename){event_->GetWeight() ->AddTh2fSF(label,filename);}

        inline void AddTF2SF(string label, string formula,string err=""){event_->GetWeight() ->AddTF2SF(label,formula,err);}
        inline void AddTF1SF(string label, string formula,string err=""){event_->GetWeight() ->AddTF1SF(label,formula,err);}

        // -- PU Reweight
        inline void AddTarget( TH1*h, int runMin=-1, int runMax =-1,double lumi=-1){ event_ -> GetWeight() -> AddTarget(h,runMin,runMax,lumi);}
        inline void AddTarget( TH1*h, string systName, int runMin=-1, int runMax =-1,double lumi=-1){ event_ ->GetWeight() -> AddTarget(h,systName, runMin,runMax);}
        inline void AddPuMC( string label, TH1*h, int runMin=-1, int runMax =-1){ event_ ->GetWeight() -> AddMC(  label, h, runMin, runMax ); }
        // use to set veto, ecc... in general don't use it directly
        inline SF* GetSF(string label){ return event_->GetWeight() -> GetSF(label) ;}


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
