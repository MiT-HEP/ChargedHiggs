#ifndef DUMPER_H
#define DUMPER_H

#include "interface/GeneralFunctions.hpp"

#include "TTree.h"
#include "TFile.h"
#include <string>
using namespace std;

/*
 * class designed to dump flat ntuples
 * original author: Andrea C. Marini
 * date: 01/2016
 */

#include "interface/Named.hpp"

class BareCollection;

class Dumper: public Named {

    private:


        string outdirname_;
        string basename_{"NeroNtuples_job%d.root"};
        bool dump_{false}; //general flag to activate

        TFile *out_ {NULL}; //current output file
        TTree *tree_ {NULL}; //current output tree


        string cacheMC_{""};

        void OpenNewFile();

    public:
        virtual const string name() const {return "Dumper";}
        Dumper(){}
        ~Dumper(){ Close();}

        void SetOutDir(string dir);
        void SetBaseName(string name);
        void ActivateDump(bool x=true);

        // the dumper should be aware of the file structure
        virtual void NewFile(string mcname){
            if (not dump_) return;
            if (cacheMC_ != mcname)
            {
                cacheMC_=mcname;
                OpenNewFile();
            }
        }

        // init tree
        virtual void InitTree(const vector<BareCollection*> &bare);

        void Fill();

        void Close();

};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
