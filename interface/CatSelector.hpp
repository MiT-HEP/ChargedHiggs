#ifndef CATSELECTOR_H
#define CATSELECTOR_H
/*
 * Class design to handle the categories and 
 * category boundaries
 */

#include "interface/Output.hpp"

class CatSelector{
    private:
        DataStore varvalues_;

        int nVars=0;
        int nCat=0;
        

        map<pair<string,int> ,float> boundaries_;

    public:

        int inline GetNcat() const { return nCat;}
        int inline GetNvars() const { return nVars;}

        void AddVariable(string name)
        {
            varvalues_.Add(name,'F');
            ++nVars;
        }

        void inline SetVariable(string name, float value)
            {varvalues_.Set(name,value);}

        void SetBoundary(string name, int cat, float value); // boundary = cat

        void inline reset () { nCat=0;nVars=0; boundaries_.clear() ;} // TODO datastore.clear/reset
        
        int computeCat( );

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
