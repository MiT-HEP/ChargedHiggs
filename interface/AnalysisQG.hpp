#ifndef ANALYSIS_QG_H
#define ANALYSIS_QG_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

#include "interface/Output.hpp" // DataStore
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class QGAnalysis: virtual public AnalysisBase
{
    public:
        QGAnalysis() : AnalysisBase () {}
        virtual ~QGAnalysis (){}

        void Init() override;
        void InitMM();
        void InitJJ();
        void InitTmva() ;
        int analyze(Event*,string systname) override;
        int analyzeMM(Event*,string systname);
        int analyzeJJ(Event*,string systname);
        const string name() const override {return "QGAnalysis";}

        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet*j) override;
        void SetTauCuts(Tau*) override;
        void SetPhotonCuts(Photon*) override;

        bool doMM{true};
        bool doJJ{true};

    private:
        CutSelector cut;

        enum ZmmCutFlow{ Total=0, 
            Leptons,
            Trigger,
            Mass,
            OneJet,
            DeltaPhi,
            MaxCut // do not remove
        };

        enum DiJetCutFlow { DiJetTotal = 0,
            DiJetTwoJets,
            DiJetDeltaPhi,
            DiJetMaxCut
        };

    const vector<string> jetTypes{"Q","G","U"};
    const vector<string> jetVars{"QGL", "nmult", "cmult", "mult","ptD","axis2", "axis1", "PtDrLog"};
    
    vector<float> ptBins{30,40,50,80,100,120,250,500,8000};
    vector<float> aetaBins{0.0,2.0,2.5,3.,4.7};

    int Rematch(Event *e, Jet *j, float dR=0.4);

    // --- TMVA ----
    public:
        vector<string> weights;
    private:
        DataStore varValues_;
        //TMVA::Reader *reader_ ;
        vector<TMVA::Reader*> readers_;
        // Variables
        template<class T>
        void SetVariable( string name, T value){ varValues_.Set(name, value); }

        void AddVariable( string name, char type);
        ///afs/cern.ch/user/y/ytakahas/public/forQGtagger/weights/   
        int BdtEtaBin(float eta);
        int BdtPtBin(float pt);
        map < pair<int,int> , int > binmap;
        void FillMapAndWeights(string dir);
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
