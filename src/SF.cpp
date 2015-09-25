#include "interface/SF.hpp"
#define VERBOSE 2

void SF_PtEta::add(double pt1, double pt2,double eta1, double eta2, double sf, double err)
{
    range r;
    r.pt1 = pt1;
    r.pt2 = pt2;
    r.eta1 = eta1;
    r.eta2 = eta2;

    store[r].first = sf;
    store[r].second = err;

#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[SF_PtEta]::[add]::[DEBUG] Adding SF Pt Eta:"<<pt1<<":"<<pt2<<"|"<<eta1<<":"<<eta2<<"|"<<sf<<":"<<err<<endl;
#endif
}

void SF_PtEta::set( double pt, double eta)
{
    int change = 0;
    for(auto s : store)
    {
        if ( pt < s.first.pt1 ) continue;
        if ( s.first.pt2  <= pt ) continue;
        if ( eta < s.first.eta1 ) continue;
        if ( s.first.eta2  <= eta ) continue;
        sf = s.second.first;	
        err = s.second.second;
        change = 1;
        break;
    }
    if (not change)
    {
        cout<<"[SF_PtEta]::[set]::[ERROR] no PT ETA RANGE for SF '"<<label<<"' in pt="<<pt<<" eta="<<eta<<endl;
        sf = 1.0;
        err = 0.0;
    }
}

const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2)
{
    // on pt1
    if (r1.pt1 < r2.pt1) return true;
    if (r1.pt1 > r2.pt1) return false;
    // -- pt2
    if (r1.pt2 < r2.pt2) return true;
    if (r1.pt2 > r2.pt2) return false;
    // -- eta1
    if (r1.eta1 < r2.eta1) return true;
    if (r1.eta1 > r2.eta1) return false;
    // -- eta2
    if (r1.eta2 < r2.eta2) return true;
    if (r1.eta2 > r2.eta2) return false;
    // they are equal
    return false;
}

void SF_PtEta::print(){
    cout <<" ----- SF Pt Eta ------"<<endl;
    cout <<"label='"<<label<<"'"<<endl;
    for(auto& p : store ) 
    {
    cout << p.first.pt1<<":"<<p.first.pt2<<"|"<<p.first.eta1<<":"<<p.first.eta2<<"||"<< p.second.first<<":"<<p.second.second<<endl;
    }
    cout <<" ----------------------"<<endl;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
