#ifndef CHARGEDHIGGSEWKEMBEDDING_H
#define CHARGEDHIGGSEWKEMBEDDING_H
#include "interface/AnalysisBase.hpp"
#include "NeroProducer/Core/interface/BareTaus.hpp"
#include "TChain.h"
#include "TTree.h"
#include <map>

/*
 * This analysis is design to replicate the charged higgs analysis and to dump the
 * variables that will be used to shoot the embedding MC
 */

class ChargedHiggsEWKEmbedding:  virtual public AnalysisBase
{
    public:
        ChargedHiggsEWKEmbedding() : AnalysisBase() {}
        virtual ~ChargedHiggsEWKEmbedding(){}
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() override {return "ChargedHiggsEWKEmbedding";}
        /* Configuration to run the dump of the events*/
        bool doTree = true;
        string treename ="embedding";

        /* Configuration to merge the events*/
        bool doMerge = false;
        vector<string> embeddingTrees ; // 

    private:
        class eventInfo{
            public:
                unsigned run; unsigned lumi; unsigned event;
                eventInfo(unsigned r, unsigned l, unsigned e){ run = r; lumi=l; event = e;}
                eventInfo(){run = -1; lumi=-1; event = -1;}
                const inline bool operator<(const eventInfo&x) const {
                        if (event <x.event) return true;
                        if (event> x.event) return false;
                        if (lumi< x.lumi) return true;
                        if (lumi> x.lumi) return false;
                        if (run < x.run) return true;
                        if (run > x.run) return false;
                        return false;
                        }
            };
        map<eventInfo,unsigned> treeMap;
        TChain *tMerge;
        BareTaus *bt ;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
