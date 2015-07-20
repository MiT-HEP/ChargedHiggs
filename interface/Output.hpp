#ifndef OUTPUT_H
#define OUTPUT_H

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

#include <map>
using namespace std;

// do we want it implemented as singleton ?
class TmvaTrainer;

class Output{
    protected:
        // 
        TFile *file_;
        map<string,TH1D*> histos_;
        map<string,TH2D*> histos2D_;
        inline TH1D* Get(string name){ return histos_[name];}
        inline TH2D* Get2D(string name){ return histos2D_[name];}

        void CreateDir(string dir); // called by Write
    public:
        Output(){file_=NULL;}
        ~Output(){}
        void Close();
        void Open(string name) ;
        void Write();
        // -- Book Histo
        bool Exists(string name);

        void Book(string name, string title,int nBins, double xmin, double xmax);
        void Book(string name, string title,int nBins, double *xbound);

        void Book2D(string name, string title,int nBins, double xmin, double xmax,int nBins2,double ymin,double ymax);
        //
        virtual void Fill(string name, string syst , double value, double weight=1);
        virtual void Fill2D(string name, string syst , double valueX, double valueY, double weight=1);
        TH1D* Get(string name,string systname);
        TH2D* Get2D(string name,string systname);

        inline TFile * GetFile(){ return file_;} // TMVA wants the file pointer

};

#endif 
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
