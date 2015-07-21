#ifndef OUTPUT_H
#define OUTPUT_H

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

#include <map>
using namespace std;

// class design to store tree variables
class DataStore{
        map<string,double> valuesD_;
        map<string,float> valuesF_;
        map<string,int >  valuesI_;

    public:

        void Add(string name, char type);
        bool Exists(string name);
        void* GetPointer(string name);
        
        template<class T>
        void Set(string name, T value);
        void Print();
};

// do we want it implemented as singleton ?

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

        // ---- Tree Operations
    private:
        DataStore varValues_;
        map<string,TTree*> trees_;
    public:

        inline void InitTree(string name){ file_->cd(); trees_[name]= new TTree(name.c_str(),name.c_str());}
        inline void Branch(string tree,string name, char type){ varValues_.Add(name,type); trees_[tree]->Branch(name.c_str(), varValues_.GetPointer(name), (name+"/"+type).c_str() ) ;}
        template<class T>
        inline void SetTreeVar(string name, T value) { varValues_.Set(name,value);}
        inline void FillTree(string tree){ trees_[tree]->Fill();}
        inline void PrintTreeVar(){ varValues_.Print() ;}

        inline TTree* GetTree(string tree){ return trees_[tree] ;}
        inline void PrintTree(string tree){ trees_[tree]->Print();}

};


// --- Templates here
template<class T>
void DataStore::Set(string name, T value)
{
   if( valuesD_.find( name ) != valuesD_.end() ) 
            valuesD_[name] = double( value) ;
   if( valuesF_.find( name ) != valuesF_.end() ) 
            valuesF_[name] = float( value) ;
   if( valuesI_.find( name ) != valuesI_.end() ) 
            valuesI_[name] = int( value) ;
   return ;

}

#endif 
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
