#include "interface/UnfirableFilter.hpp"
#define UNPREFIRE_EVENT_FAIL 1
#define UNPREFIRE_EVENT_PASS 0

#include "TChain.h"

void UnprefirableFiltersAnalysis:: Init()
{

    TChain *t = new TChain(tname_.c_str());
    t->Add(fname_.c_str());
    
    Long64_t run, lumi, event;

    t->SetBranchAddress("run",&run);
    t->SetBranchAddress("lumi",&lumi);
    t->SetBranchAddress("event",&event);

    for(int ientry=0;ientry< t->GetEntries();++ientry){
        t->GetEntry(ientry);
        store_.insert( make_tuple(run,lumi,event));
    }
}

int UnprefirableFiltersAnalysis::analyze(Event *e, string systname)
{
    run=e->runNum();
    lumi=e->lumiNum();
    event=e->eventNum();

    if(store_ .find (make_tuple(run,lumi,event)) != store_.end()  ) return UNPREFIRE_EVENT_PASS;
    else return UNPREFIRE_EVENT_FAIL;
}
