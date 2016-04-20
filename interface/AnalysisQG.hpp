#ifndef ANALYSIS_QG_H
#define ANALYSIS_QG_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"

class QGAnalysis: virtual public AnalysisBase
{
    public:
        QGAnalysis() : AnalysisBase () {}
        virtual ~QGAnalysis (){}

        void Init() override;
        void InitMM();
        void InitJJ();
        int analyze(Event*,string systname) override;
        int analyzeMM(Event*,string systname);
        int analyzeJJ(Event*,string systname);
        const string name() const override {return "QGAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet*j) override;

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
        const vector<string> jetVars{"QGL","mult","ptD","axis2"};

        vector<float> ptBins{30,40,50,80,100,120,250,500,8000};
        vector<float> aetaBins{0,2,2.5,3.,4.7};

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
