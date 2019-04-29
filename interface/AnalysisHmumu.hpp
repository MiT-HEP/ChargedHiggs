#ifndef ANALYSIS_Hmumu_H
#define ANALYSIS_Hmumu_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"
#include "interface/SF_CSVReweight.hpp" // REMOVEME AFTER MERGE OF MR PR
#include "interface/Output.hpp" // DataStore
#include "interface/KinematicFit.hpp" //

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class TRandom;
#include "TPython.h"
#include <memory>

#include "interface/JetResolutionObject.hpp"

class HmumuAnalysis: virtual public AnalysisBase
{
    public:
        int year=2017; // master switch for year configuration

        HmumuAnalysis() : AnalysisBase () {}
        virtual ~HmumuAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        void EndEvent() override;

        const string name() const override {return "HmumuAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet *j) override ;
        void SetTauCuts(Tau *t) override;
        void SetPhotonCuts(Photon*p) override;

        bool Unblind(Event *e) override {if (e->IsRealData() and mass_ > 125-3 and mass_<125+3 ) return unblind; return true;} // if is not data, no need to return something else
        //

        bool doSync{false};
        int catType{0}; //0 = RunISync, 1=AutoCat, 2=Bdt
        bool doEvenOnly{false}; //signal only even events
        bool doOddOnly{false}; //signal only even events


    private:
        //cache event,run, lumi for filling
        unsigned eventNum,runNum,lumiNum;
        bool fillTree{false}; // used to skim output trees.
        
        void updateMjj(); // from selectedJets. Call it each time selectedJets is modified, but only once: D
        // selected Objects
        float mass_;
        float pt_;
        float mt; // with WHLep, only
        Object Hmm;
        Lepton *mu0{NULL},*mu1{NULL};
        vector<Jet*> selectedJets; // final that match to the one above
        vector<pair<float,pair<int,int> > > mjj; // mjj -> ijet, jjet

        map<string,float> jetVar_;

        // this are related to miniIso if run
        vector<Jet*> selectedJetsMiniIso; 
        vector<Lepton*> miniIsoLeptons;
        
        // fatjets VH Hadronic tags (and TODO hadronic top)
        vector<FatJet*> selectedFatjets;
        vector<Jet*> selectedJetsVHHadr;
        vector<float> alphaVHHadr;
        float valueVHHadr;

        //Set soft variables
        bool isMiniIsoLeptons{false};
        

        bool processingSyst_{false}; // used for tree

        bool isSingleMuon{false};
        bool isDoubleMuon{false};

        bool passTrigger{false};
        bool passTrigger1{false};
        bool passTrigger2{false};
        bool passAsymmPtCuts{false};
        bool passLeptonVeto{false};

        // select cuts
        CutSelector cut;

        vector<string> categories_;

        // WARNING modify global event selection 
        string CategoryExclusive(Event *);

        string Category(Lepton*mu0,Lepton*mu1, const vector<Jet*>& jets);
        //string CategoryAutoCat(Lepton*mu0,Lepton*mu1, const vector<Jet*>& jets,float met,float metphi);
        string CategoryBdt(Event *e);
        string CategoryBdtMIT(Event *e);

        float BdtUCSD(int pos=0,int nj=0);
        float BdtUF(int pos=0);

        enum CutFlow{ Total=0, 
            Leptons,
            Trigger,
            Mass,
            MaxCut // do not remove
        };

        std::unique_ptr<TRandom> rnd_;

        float getZPtReweight(float Zpt);
        float getZPtReweight2018(float Zpt);
        std::unique_ptr<TH1D> rzpt_;
        std::unique_ptr<TH1D> rzpt2018_;
        std::unique_ptr<TF1> rzpt2_;

        /************
         *   TMVA   *
         ************/
        DataStore varValues_;
        vector<TMVA::Reader*> readers_;

        void InitTmva(int pos);
        void InitTmvaMIT(int pos);
        void InitTmvaUCSD(int pos,int nj=0);

        void FillSyncTree(const string& label, const string&systname ,const string& category);
        vector<float> bdt;

        // Variables
        template<class T>
        void SetVariable( string name, T value){ varValues_.Set(name, value); }

        // not working
        template<class T>
        T GetVariable( string name);//{ return *(T*)varValues_.GetPointer(name); } ;

        void AddVariable( string name, char type);
        void AddSpectator( string name, char type);
    public:
        vector<string> weights;

        bool doScikit{true};
        bool doUnbinned{true};
    protected:
    /**********************************
     *          SCIKIT                *
     **********************************/
        std::unique_ptr<TPython> py;
        vector<float> x;
        vector<string> discr;
        void InitScikit();
        vector<float> scikit; // like bdt

        bool isExclusiveCat{false};
       
       // Kinematic fit 
        std::unique_ptr<JME::JetResolutionObject> jet_resolution;
        std::unique_ptr<KinematicFit> kf;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
