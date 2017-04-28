#ifndef ANALYSIS_Hmumu_H
#define ANALYSIS_Hmumu_H

#include "interface/AnalysisBase.hpp"
#include "interface/CutSelector.hpp"
#include "interface/SF_CSVReweight.hpp" // REMOVEME AFTER MERGE OF MR PR
#include "interface/Output.hpp" // DataStore

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class TRandom;
#include "TPython.h"
#include <memory>

class HmumuAnalysis: virtual public AnalysisBase
{
    public:
        HmumuAnalysis() : AnalysisBase () {}
        virtual ~HmumuAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override {return "HmumuAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet *j) override ;
        void SetTauCuts(Tau *t) override;
        void SetPhotonCuts(Photon*p) override;

        float mass_;
        bool Unblind(Event *e) override {if (e->IsRealData() and mass_ > 125-3 and mass_<125+3 ) return unblind; return true;} // if is not data, no need to return something else

        bool doSync{false};
        int catType{0}; //0 = RunISync, 1=AutoCat, 2=Bdt
        bool doEvenOnly{false}; //signal only even events
        bool doOddOnly{false}; //signal only even events

    private:
        // select cuts
        CutSelector cut;

        vector<string> categories_;

        string Category(Lepton*mu0,Lepton*mu1, const vector<Jet*>& jets);
        string CategoryAutoCat(Lepton*mu0,Lepton*mu1, const vector<Jet*>& jets,float met);
        string CategoryBdt(Lepton*mu0,Lepton*mu1, const vector<Jet*>& jets,float met);
        double dimu_dPhiStar(Lepton* mu0, Lepton*mu1);  // from UF

        enum CutFlow{ Total=0, 
            Leptons,
            Trigger,
            Mass,
            MaxCut // do not remove
        };

        std::unique_ptr<TRandom> rnd_;

        float getZPtReweight(float Zpt);
        std::unique_ptr<TH1D> rzpt_;
        std::unique_ptr<TF1> rzpt2_;

        /************
         *   TMVA   *
         ************/
        DataStore varValues_;
        vector<TMVA::Reader*> readers_;

        void InitTmva();
        vector<float> bdt;

        // Variables
        template<class T>
        void SetVariable( string name, T value){ varValues_.Set(name, value); }
        void AddVariable( string name, char type);
        void AddSpectator( string name, char type);
    public:
        vector<string> weights;

        bool doScikit{true};
    protected:
    /**********************************
     *          SCIKIT                *
     **********************************/
        std::unique_ptr<TPython> py;
        vector<float> x;
        vector<string> discr;
        void InitScikit();
        vector<float> scikit; // like bdt

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
