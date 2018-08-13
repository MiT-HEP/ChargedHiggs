#ifndef OUTPUT_H
#define OUTPUT_H

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <map>
#include <memory>
#include <set>

using namespace std;

// class design to store tree variables
class DataStore{
        map<string,TLorentzVector*> valuesP4_;
        map<string,double> valuesD_;
        map<string,float> valuesF_;
        map<string,int >  valuesI_;

        // to save arrays
        map<string, std::unique_ptr<double> > valuesDD_;
        map<string, std::unique_ptr<float> > valuesFF_;
        map<string, std::unique_ptr<int> > valuesII_;

    public:
        DataStore(){};
        ~DataStore();

        void Add(string name, char type);
        void Add(string name, char type,int N);
        bool Exists(string name);
        void* GetPointer(string name);
       
        //int float and double can be cast between them 
        template<typename T>
        void Set(string name, const T & value);

        template<typename T>
        void Set(string name, int N,const T & value); // for array

        void Print();
};

// do we want it implemented as singleton ?

class Output{
    protected:
        // 
        TFile *file_;
        map<string,TH1D*> prototypes_;
        map<string,TH2D*> prototypes2D_;

        map<string,TH1D*> histos_;
        map<string,TH2D*> histos2D_;
        


        inline TH1D* Get(string name){ 
            if (not PassFinal(name) ) return dummy.get();
            if ( histos_.find(name) != histos_.end() )  return histos_[name];
            else return prototypes_[name];
        }
        inline TH2D* Get2D(string name){ 
            if (not PassFinal(name) ) return dummy2D.get();
            if ( histos2D_.find(name) != histos2D_.end() ) return histos2D_[name];
            else return prototypes2D_[name];
        }

        void CreateDir(string dir); // called by Write

        // this option can be used to check name against 
        // histo and avoid booking and filling
        // use to make everything faster, don't use too many "final" objects
        set<string> finalHistos_;
        bool onlyFinal_{false};
        inline bool PassFinal(const string&name) { return (not onlyFinal_) or finalHistos_.find(name) != finalHistos_.end() ; }
        // create a fake histogram for not final return pointers
        std::unique_ptr<TH1D> dummy;
        std::unique_ptr<TH2D> dummy2D;

    public:
        Output(){file_=NULL;dummy.reset(new TH1D("dummy","dummy",100,0,1));dummy2D.reset(new TH2D("dummy","dummy",10,0,10,10,0,10));}
        ~Output(){}
        void Close();
        void Open(string name) ;
        void Write();
        // -- Book Histo
        bool Exists(string name);

        void Book(string name, string title,int nBins, double xmin, double xmax);
        void Book(string name, string title,int nBins, double *xbound);

        void Book2D(string name, string title,int nBins, double xmin, double xmax,int nBins2,double ymin,double ymax);
        void Book2D(string name, string title,int nBins, double *xbound,int nBins2,double*ybound);
        //
        virtual void Fill(string name, string syst , double value, double weight=1);
        virtual void Fill2D(string name, string syst , double valueX, double valueY, double weight=1);
        TH1D* Get(string name,string systname);
        TH2D* Get2D(string name,string systname);

        inline TFile * GetFile(){ return file_;} // TMVA wants the file pointer

        // Final objects
        void SetOnlyFinal(bool x=true) {onlyFinal_ = x;}
        void AddFinalHisto(string s){ finalHistos_.insert(s) ;}

        // ---- Tree Operations
    private:
        DataStore varValues_;
        map<string,TTree*> trees_;
    public:

        inline void InitTree(string name){ file_->cd(); trees_[name]= new TTree(name.c_str(),name.c_str());}

        // N used only for arays types
        void Branch(string tree,string name, char type,int MAXN=10,string num="");

        //for vars
        template<class T>
        void SetTreeVar(string name, const T & value) { varValues_.Set(name,value);}

        // for arrays
        template<class T>
        void SetTreeVar(string name, int N,const T & value) { varValues_.Set(name,N,value);}

        inline void FillTree(string tree){ trees_[tree]->Fill();}
        inline void PrintTreeVar(){ varValues_.Print() ;}

        inline TTree* GetTree(string tree){ return trees_[tree] ;}
        inline void PrintTree(string tree){ trees_[tree]->Print();}

};

// --- Template specification declaration
template<>
void DataStore::Set<TLorentzVector>(string name, const TLorentzVector & value);

// --- pointer version of TLorentzVector
template<>
void DataStore::Set<TLorentzVector*>(string name, TLorentzVector* const & value);

// --- Templates here
template<class T>
void DataStore::Set(string name, const T & value)
{
   if( valuesD_.find( name ) != valuesD_.end() ) 
            valuesD_[name] = double( value) ;
   if( valuesF_.find( name ) != valuesF_.end() ) 
            valuesF_[name] = float( value) ;
   if( valuesI_.find( name ) != valuesI_.end() ) 
            valuesI_[name] = int( value) ;
   return ;
}

template<class T>
void DataStore::Set(string name,int N, const T & value)
{
   if( valuesDD_.find( name ) != valuesDD_.end() ) 
            valuesDD_[name].get()[N] = double( value) ;
   if( valuesFF_.find( name ) != valuesFF_.end() ) 
            valuesFF_[name].get()[N] = float( value) ;
   if( valuesII_.find( name ) != valuesII_.end() ) 
            valuesII_[name].get()[N] = int( value) ;
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
