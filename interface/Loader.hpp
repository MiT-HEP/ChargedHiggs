#ifndef LOADER_H
#define LOADER_H

// THIS CLASS IS SUPPOSED TO CHANGE THE FILLING OF THE EVENT: TODO

#include "interface/Named.hpp"
#include "interface/Handlers.hpp"

#include "TTree.h"
#include "TChain.h"

#include <string>
#include <map>
#include <iostream>

#define NO_TCHAIN

class Event;

class Loader : public Named {
    protected:
        Event *event_{NULL}; // not owned
        int year=2016; // ?? for NanoAOD
        bool isData=false; // for NanoAOD
#ifdef NO_TCHAIN
        TTree *tree_{0}; // not opened but set, NOT OWNED
        string fname;
#else
        TChain *tree_{0}; // not opened but set, NOT OWNED
        int isTreeActive_{false}; // keep track if tree is active
#endif
    public:
        Loader(){};
        ~Loader(){ }
#ifdef NO_TCHAIN
        inline void SetFileName(const string& name){fname=name;}
        inline void SetTree(TTree *t) { 
            tree_=t; 
        }
#else
        inline void SetTree(TChain *t) { 
            tree_=t; 
            isTreeActive_ = false;
        }
#endif
        inline void SetEventPtr(Event*e) { event_=e; }
        const string name() const override { return "Loader" ;} 
        virtual const string chain() const = 0; // name of the tree in the file
        virtual int InitTree()  = 0 ;
        virtual int FillEvent() = 0 ;
        virtual void FillEventInfo() = 0 ; // pre new file check: fill isReal data
        virtual void NewFile() = 0 ;
        virtual void Clear() {};

        virtual void SetYear(int y) {
            Log(__FUNCTION__,"DEBUG",Form("year=%d",y));
            year=y;
        }

        virtual void SetData(bool d) {
            Log(__FUNCTION__,"DEBUG",Form("isdata=%d",int(d)));
            isData=d;
        }
#ifndef NO_TCHAIN
        inline void SetTreeAsActive(bool x=true){isTreeActive_=x;}
#endif
        
};

#include "NeroProducer/Core/interface/BareCollection.hpp"

class LoadNero : public Loader{
    protected:
        vector<BareCollection*> bare_;
        map<string,int> names_;
    public:
        const string name() const override { return "LoadNero" ;} 
        const string chain() const override{ return "nero/events";} // name of the tree in the file
        vector<BareCollection*> & GetBare(){  return bare_;}

        int InitTree() override; 
        int FillEvent() override;
        void FillEventInfo() override;
        void Clear() override { for (auto c : bare_) c->clear() ;}

        void FillJets();
        //void FillTrackJets();
        void FillFatJets();
        void FillLeptons();
        void FillPhotons();
        void FillTaus();
        void FillMet();
        void FillMC();
        void FillTrigger();
        void NewFile() override; // should take care of loading the trigger names

};


// ---------------------------- FACTORY -----------------------
class LoaderCreator;

class LoaderFactory // Singleton
{
    public:
        static LoaderFactory& get(){ static LoaderFactory instance; return instance; } // singleton
        void inline registerit(const std::string& classname, LoaderCreator* creator){
            std::cout<<"[LoaderFactory]"<<"::[INFO]"<<" Registering "<<classname<<std::endl;
            table_[classname] = creator;
        }
        Loader* create(const std::string &name ) ;
    private:
        LoaderFactory(){}; // Constructor
        LoaderFactory(LoaderFactory const&)  = delete; // avoid copies around
        void operator=(LoaderFactory const&)  = delete; // avoid copies around
        std::map<std::string, LoaderCreator*> table_;

};

class LoaderCreator{
    public:
        virtual Loader* create()=0; // method to create stuff
        //Constructor -- will register the creator in the factory
        LoaderCreator(const std::string& classname) { std::cout<<" called creator const for"<<classname<<std::endl; LoaderFactory::get() . registerit(classname,this) ; }
};

// Implementation of the creators
template <typename T>
class LoaderCreatorImpl : public LoaderCreator
{
    public:
        LoaderCreatorImpl(const std::string&classname) : LoaderCreator(classname){}
        Loader* create( ) override final{ return new T(); }
};

//Register macros
#define REGISTER(classname)\
    namespace { \
        LoaderCreatorImpl<classname> _creator_ (#classname); \
    };
// ------------------------------------------------- END FACTORY ----------------------------


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
