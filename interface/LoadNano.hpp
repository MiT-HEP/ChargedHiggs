#ifndef LOADNANO_H
#define LOADNANO_H

#include "interface/Loader.hpp"
#include <map>
#include <string>

#define MAX_MUON 10
#define MAX_JET 20

class Nano;
#include <memory>

class LoadNano : public Loader
{
    protected:
        std::unique_ptr<Nano> nano; 
        
    public:
    const string name() const override { return "LoadNano" ;} 
    const string chain() const override{ return "Events";} // name of the tree in the file

    int InitTree()  override ;
    int FillEvent() override  ;
    void FillEventInfo() override  ; // pre new file check: fill isReal data
    void NewFile() override ;
    void Clear() override;
};

#endif
