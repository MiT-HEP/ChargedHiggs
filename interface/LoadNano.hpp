#ifndef LOADNANO_H
#define LOADNANO_H

#include "interface/Loader.hpp"
#include <map>
#include <string>


//class Nano;
class nanov8;
#include <memory>

class LoadNano : public Loader
{
    protected:
        std::unique_ptr<nanov8> nano; 
        int version_{8};
        
    public:
    const string name() const override { return "LoadNano" ;} 
    const string chain() const override{ return "Events";} // name of the tree in the file

    int InitTree()  override ;
    int FillEvent() override  ;
    void FillEventInfo() override  ; // pre new file check: fill isReal data
    void NewFile() override ;
    void Clear() override;

    void SetYear(int y) override ;
    void SetData(bool x) override ;
    void SetNanoV(int v) ;// NanoVersion is only in nano
};

#endif
