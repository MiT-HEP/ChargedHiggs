#ifndef CHARGEDHIGGSSYNC_H
#define CHARGEDHIGGSSYNC_H

#include "interface/AnalysisBase.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class ChargedHiggsSync:  
    virtual public ChargedHiggsTauNu
{
    string filename_="sync.txt";
    ofstream file_;

    public:
    ChargedHiggsSync():ChargedHiggsTauNu()  {}
    ~ChargedHiggsSync(){file_.close();} // close the file if open

    void SetFileName(string name){filename_=name;}
    void Init() override { file_.open(filename_); };
    int analyze(Event*,string systname) override;
    const string name() const override {return "ChargedHiggsSync";}

    void WriteToFile(const string& s){ file_<<s<<endl;}
    void WriteToFile(const stringstream& s){ file_<<s.str()<<endl;}

    map<string,bool> wrote;
    void WriteOnce( const string &s) ;
    bool oneline{true};

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
