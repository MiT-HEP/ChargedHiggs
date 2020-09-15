#ifndef ANALYSIS_Hbbg_H
#define ANALYSIS_Hbbg_H

#include "interface/AnalysisBase.hpp"
#include "interface/SF_CSVReweight.hpp" // REMOVEME AFTER MERGE OF MR PR
#include "interface/Output.hpp" // DataStore
#include "interface/KinematicFit.hpp" //

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class TRandom;
#include <memory>

#include "interface/JetResolutionObject.hpp"
#include "interface/zpt.hxx"

class HbbgAnalysis: virtual public AnalysisBase
{
    public:
        int year=2016; // master switch for year configuration
        int debug=0; // activate debug log
        int doTree=0; // save minitree

        HbbgAnalysis() : AnalysisBase () {}
        virtual ~HbbgAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        void EndEvent() override;

        const string name() const override {return "HbbgAnalysis";}
        void SetLeptonCuts(Lepton *l) override ; 
        void SetJetCuts(Jet *j) override ;
        void SetTauCuts(Tau *t) override;
        void SetPhotonCuts(Photon*p) override;

        bool Unblind(Event *e) override {if (e->IsRealData() and mass_ > 125-3 and mass_<125+3 ) return unblind; return true;} // if is not data, no need to return something else

        //

    private:

        //cache event,run, lumi for filling
        unsigned eventNum,runNum,lumiNum;
        string label;
       
        // 
        void reset();

        void fillHists(const string & s, const string &l,const string &syst);
        void updateEventVar( );
        void updateTreeVar( );
        void matchToGen();

        // selected Objects
        float mass_;
        float pt_;

        Photon *pho{nullptr};
        vector<Jet*> selectedBJets; // final that match to the one above
        vector<Jet*> selectedJets; // final that match to the one above
        map<string,float> eventVar_;

        bool passTrigger{false};

        vector<string> categories_;
        std::unique_ptr<TRandom> rnd_;
        std::unique_ptr<JME::JetResolutionObject> jet_resolution;

        vector<GenParticle*> genMatching;
        bool isSignal{false};
        void printDebugInfo();
        void printObject(const string &s,Object*o) const;

        std::unique_ptr<KinematicFit2> kf;

        // MVA reader
        //DataStore varValues;
        //vector<TMVA::Reader*> readers_;
        //map< pair<string,int>, int > mvavars_; //keep track of the variables added to the mva

    public:

    protected:
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
