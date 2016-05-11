#include "interface/AnalysisChargedHiggsSync.hpp"
#include "interface/GeneralFunctions.hpp"

void ChargedHiggsSync::WriteOnce( const string &s)
{
    if (wrote[s] )return; 
    if (s.find("###")!=string::npos)WriteToFile("###############"); 
    WriteToFile(s); 
    if (s.find("###")!=string::npos)WriteToFile("###############"); 
    
    wrote[s] = true;

}


int ChargedHiggsSync::analyze(Event*e,string systname)
{
    WriteOnce("### Event Info ###");
    WriteOnce("# (run,lumi,event)");

    stringstream s;
    s<<"("<<e->runNum()<<","<<e->lumiNum()<<","<<e->eventNum()<<")";
    if (not oneline) s<<endl;

    CutSelector direct;
    direct.reset();
    direct.SetMask(MaxCut-1) ;
    direct.SetCutBit( Total ) ;
    direct.SetCut( Selection(e,true) );

    CutSelector inviso;
    inviso.reset();
    inviso.SetMask(MaxCut-1) ;
    inviso.SetCutBit( Total ) ;
    inviso.SetCut( Selection(e,false) );

    WriteOnce("### Selection ###");
    WriteOnce("#|DS| direct");
    WriteOnce("#|IS| invert iso");
    WriteOnce("#LSB Total, OoneTau, NoLep, ThreeJets, OneBjet, Trigger, Met, AngColl, AngRbb, MaxCut");

    s   <<"|DS|"<<ChargedHiggs::printBinary(direct.raw(),MaxCut);
    if (not oneline) s<<endl;

    s   <<"|IS|"<<ChargedHiggs::printBinary(inviso.raw(),MaxCut);
    if (not oneline) s<<endl;

    // ----------------- Bare TAU --------------
    if (true)
    {
        WriteOnce("### BareTaus ###");
        WriteOnce("#|BTidx|Pt:Eta:Phi:Iso:pass");
        unsigned idx=0;
        Tau*bt;
        while( (bt=e->GetBareTau(idx)) != NULL){
            s<<"|BT"<<idx<<"|"<<bt->Pt()<<":"<<bt->Eta()<<":"<<bt->Phi()<<":"<<bt->iso2<<":"<<bt->IsTau();
            ++idx;
            if (not oneline) s<<endl;
        }
    }

    // ----------------- TAU --------------
    if (true){
        WriteOnce("### Lead Tau Selected ###");
        WriteOnce("#|T|Pt:Eta:Phi");
        Tau*t=e->GetTau(0);

        if (t!=NULL){
            s<<"|T|"<<t->Pt()<<":"<<t->Eta()<<":"<<t->Phi();
            if (not oneline) s<<endl;
        }
    }

    // ----------------- Bare Jet --------------
    if (true)
    {
        WriteOnce("### BareJets ###");
        WriteOnce("#|BJidx|Pt:Eta:Phi:BTag:pass");
        unsigned idx=0;
        Jet*bj;
        while( (bj=e->GetBareJet(idx)) != NULL){
            s<<"|BJ"<<idx<<"|"<<bj->Pt()<<":"<<bj->Eta()<<":"<<bj->Phi()<<":"<<bj->bdiscr<<":"<<bj->IsJet();
            ++idx;
            if (not oneline) s<<endl;
        }
    }

    // ----------------- Bare Lepton --------------
    if (true)
    {
        WriteOnce("### BareLeptons ###");
        WriteOnce("#|BJidx|Pt:Eta:Phi:passMuon:passElectron");
        unsigned idx=0;
        Lepton*bl;
        while( (bl=e->GetBareLepton(idx)) != NULL){
            s<<"|BL"<<idx<<"|"<<bl->Pt()<<":"<<bl->Eta()<<":"<<bl->Phi()<<":"<<bl->IsMuon()<<":"<<bl->IsElectron();
            ++idx;
            if (not oneline) s<<endl;
        }
    }

    // ----------------- MET --------------
    if (true){
        WriteOnce("### MET ###");
        WriteOnce("#|M|Pt:Eta:Phi");

        s<<"|M|"<<e->GetMet().Pt()<<":"<<e->GetMet().Eta()<<":"<<e->GetMet().Phi();
        if (not oneline) s<<endl;
    }

    // --------------- NPV ----------------
    if (true){
        WriteOnce("### NPV ###");
        WriteOnce("#|Npv|npv");

        s<<"|Npv|"<<e->Npv();
        if (not oneline) s<<endl;
    }

    // --------------- MT ----------------
    if (true){
        WriteOnce("### MT ###");
        WriteOnce("#|MT|mt (-1 if no tau)");

        s<<"|Mt|"<<e->Mt();
        if (not oneline) s<<endl;
    }

    // extra new line
    if (not oneline) s<<endl;
    WriteToFile(s);

    //Log(__FUNCTION__,"DEBUG","Writing"+s.str() );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
