#ifndef BARE_COLLECTION_H
#define BARE_COLLECTION_H

// ONLY STD&ROOT INCLUDES

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <vector>
#include <string>
using namespace std;

class BareCollection
{
    protected:
        bool extend_;  // for collection switch to a more inclusive mode
    public:
        BareCollection(){ extend_=false;}
        virtual ~BareCollection() {}
        virtual void clear() = 0;
        virtual void init() {}
        virtual void defineBranches(TTree*) = 0 ;
        virtual void setBranchAddresses(TTree*) = 0;
        virtual string name() = 0;
        virtual unsigned size() const { return 0; }
        virtual void compress(){}; // compress all double pointers
        
        inline void SetExtend(bool value=true){extend_=value;}
        inline bool IsExtend(){return extend_;}
};

// ----------------------------  factory ----------------
#include <map>
#include <iostream>
class BareCreator;

class BareFactory{
    private:
        BareFactory(){} // private constructor
        BareFactory(BareFactory const&)  = delete; // avoid copies around
        void operator=(BareFactory const&)  = delete; // avoid copies around
        std::map<std::string, BareCreator*> table_;
    public:
        static BareFactory& get(){static BareFactory instance; return instance;}
        BareCollection* create(const string& name);
        void inline registerit(string name, BareCreator *creator) { table_[name] = creator;}
};

class BareCreator{
    private:
    public:
        virtual BareCollection* create() = 0;
        BareCreator(const string& name){ std::cout<<"called creator for class"<<name<<std::endl; BareFactory::get() . registerit( name, this); }
};

template <typename T>
class BareCreatorImpl : public BareCreator
{
    public:
        BareCreatorImpl(const string &name) : BareCreator(name) {}
        BareCollection* create() override final { return new T(); } 
};


//Register macros
#define BAREREGISTER(classname)\
    namespace { \
        BareCreatorImpl<classname> _creator_ (#classname); \
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
