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
#include <string>

#define NO_TCHAIN

/*!
 * @brief Loop class, called by python/Loop.py
 * @author Andrea C. Marini
 */

class Looper{

    private:
#ifdef NO_TCHAIN
        vector<std::string> file_list_;
        vector<std::string> branch_list_;
        TTree *tree_{nullptr}; 
#else
        TChain *tree_; 
#endif
        int fNumber{-1};
        long fEntry;

        //
        int minEntries_{-1};

        vector<AnalysisBase*> analysis_;

        vector<SmearBase*> systs_;

        vector<CorrectorBase*> correctors_;

        Event * event_;

        Output *output_;

        TStopwatch sw_;

        Dumper *dump_;

        Loader* loader_; // translate the information in the ntuples in the information in the fwk

        void Log(const string& func, const string& state, const string& mex)  { Logger::getInstance().Log(name(),func,state, mex); };
        void LogErr(const string& func, const string& state, const string& mex) { Logger::getInstance().LogErr(name(),func,state, mex); };

        bool xrdcp_{false}; // xrdcp xroodfiles
        std::string local_{""}; // name of copied file

    protected:
        //
        /// @brief Performs actions when a NewFile is encountered
        void NewFile();

        // ---- call by LOOP
        /// @brief Fill the event by calling the Loader
        void FillEvent();
        /// @brief Clear the content of the event
        void ClearEvent();
        /// @brief Write output to files, by calling output methods
        void Write(){output_->Write();}	
        /// @brief Close output files, by calling  output methods
        void Close(){output_->Close();}

    public:
        // -- constructor
        Looper();
        ~Looper(){ClearEvent();}

        /// @brief Set minimum number of entries per second
        void SetEntryPerSecond(int n){minEntries_=n;}

        /// @brief return class name for logging
        const string name(){return "Looper";}

        /// @brief Init Looperi with loader 'name'
        /// @param name loader name
        inline void InitLoader(string name,int year=2016){ 
                    loader_ = LoaderFactory::get().create(name); 
                    loader_->SetYear(year);
#ifdef NO_TCHAIN
                    tree_=nullptr;
#else
                    tree_=new TChain(loader_->chain().c_str());
#endif
        }

        /// @brief Do XRDCP before running on xrootd remote files
        inline void SetXrdcp(bool xrdcp=true){xrdcp_=xrdcp;local_="";}

        /// @brief Add file to chain, Loader need to be init
        int AddToChain( string name );

        /// @brief Add analysis to chain
        inline int AddAnalysis( AnalysisBase* a ) {analysis_ . push_back(a); return 0;}

        /// @brief Add smearer
        int AddSmear(SmearBase *s) ;
        /// @brief Add simple smearers JES/JER/PU/NONE.
        int AddSmear(string name);
        /// @brief Add Corrector to the event
        inline int AddCorrector(CorrectorBase *c) { correctors_.push_back(c) ; return 0;}

        //-------------
        // DUMPER
        /// @brief Set dump directory
        void SetDumpDir(string dir){dump_->SetOutDir(dir);}
        /// @brief activate dump. Replica of the nutples with the loaded branches only.
        void ActivateDump(bool x=true){dump_->ActivateDump(x);}
        //------------

        /// @brief InitTree (Set Branch Address...)
        int InitTree () ;
        /// @brief Activate branch bname
        /// @param bname branch to set status to 1
        inline void ActivateBranch(string bname){ 
#ifdef NO_TCHAIN
            branch_list_.push_back( bname);
#else
            tree_ -> SetBranchStatus(bname.c_str(),1); 
#endif
            return; 
        }
        /// @brief add some standard smear .Obsolete don't use
        int InitSmear() ;
        /// @brief call init on all correctors
        int InitCorrector() ;
        /// @brief initialize all the analysis
        int InitAnalysis() ;
        /// @brief Print information on the std output
        int PrintInfo();
        /// @brief init output files
        int InitOutput(string name){output_ -> Open(name); return 0;}
        /// @brief  mark histograms name as final
        void AddFinalHisto(const string&name) {output_ -> AddFinalHisto(name); }
        /// @brief save only histograms marked as final
        void SetOnlyFinal(bool x=true) { output_->SetOnlyFinal(x) ; }

        /// @brief loop over the ntries
        void Loop();

        /// @brief add mc for xsec scaling
        /// @param label MC label
        /// @param dir MC directory
        /// @param xsec MC xsec
        /// @param nevents sum of mc event weights for normalization
        inline void AddMC( string label, string dir, double xsec, double nevents){event_ -> GetWeight() -> AddMC(label,dir,xsec,nevents); }
        inline void AddMCScale(string label, int x, double rw){ event_->GetWeight()->AddMCScale(label, MC::SCALES(x), rw);}
        inline void AddMCPdf(string label, int x, double rw){ event_->GetWeight()->AddMCPdf(label, x, rw);}
        inline void AddMCAQGC(string label, const string& name, double rw){ event_->GetWeight()->AddMCAQGC(label, name, rw);}

        /// @brief add sf with label value and error
        inline void AddSF( string label, double sf, double err){ event_->GetWeight() -> AddSF(label,sf,err);}
        inline void AddWG1SF( string label){ event_->GetWeight() -> AddWG1SF(label);}
        inline void AddNNLOPSSF( string label, string filename) {event_->GetWeight() -> AddNNLOPSSF(label,filename);}
        inline void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err)
        {event_ -> GetWeight()  -> AddPtEtaSF(label,pt1,pt2,eta1,eta2,sf,err); }
        inline void AddPtEtaEff( string label, double pt1,double pt2 ,double eta1 ,double eta2,double dataEff, double mcEff,double dataErr,double mcErr)
        {event_ -> GetWeight()  -> AddPtEtaEff(label,pt1,pt2,eta1,eta2,dataEff,mcEff,dataErr,mcErr); }
        inline void AddSplineSF(string label, double pt, double sf, double err){ event_->GetWeight() -> AddSplineSF(label,pt,sf,err);}
        inline void AddCSVSF(string label, string filename){event_->GetWeight() ->AddCSVSF(label,filename);}
        inline void AddCSVReweightSF(string label){event_->GetWeight() ->AddCSVReweightSF(label);}
        inline void AddTh2fSF(string label, string filename){event_->GetWeight() ->AddTh2fSF(label,filename);}
        inline void AddTh2fSF(string label, string filename,string effData, string effMc, string errData="", string errMc=""){ event_->GetWeight() -> AddTh2fSF(label, filename,effData, effMc, errData, errMc);}

        inline void AddTF2SF(string label, string formula,string err=""){event_->GetWeight() ->AddTF2SF(label,formula,err);}
        inline void AddTF1SF(string label, string formula,string err=""){event_->GetWeight() ->AddTF1SF(label,formula,err);}

        // -- PU Reweight
        /// @brief add target for pu reweight
        inline void AddTarget( TH1*h, int runMin=-1, int runMax =-1,double lumi=-1){ event_ -> GetWeight() -> AddTarget(h,runMin,runMax,lumi);}
        /// @brief add syst target for pu reweight
        inline void AddTarget( TH1*h, string systName, int runMin=-1, int runMax =-1,double lumi=-1){ event_ ->GetWeight() -> AddTarget(h,systName, runMin,runMax);}
        ///@ brief add mc pu distribution for pu reweights
        inline void AddPuMC( string label, TH1*h, int runMin=-1, int runMax =-1){ event_ ->GetWeight() -> AddMC(  label, h, runMin, runMax ); }
        /// @brief Get SF from labels
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
