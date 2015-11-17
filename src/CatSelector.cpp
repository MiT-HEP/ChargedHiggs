#include "interface/CatSelector.hpp"
#include "interface/CutSelector.hpp"

void CatSelector::SetBoundary(string name,int cat , float value)
{
    boundaries_[ make_pair(name,cat) ] = value; 
    if( cat >= nCat ) nCat= cat+1;
};

int CatSelector::computeCat(){
    CutSelector sel(nCat);
    sel.SetCut( (1<< (nCat+1)) -1 ) ; // set all cuts to 1

    for(const auto& b : boundaries_)
    {
        const string& name = b.first.first;
        const int& cat = b.first.second;
        const float& bound= b.second;
        float myval = * (float*) varvalues_.GetPointer(name);
        if (myval < bound) // WARNING-> cat are defined >=
        {
            sel.UnSetCutBit(cat); 
        }

    }

    for(int i=0;i<nCat;++i)
    {
        //return the first it passes -> first is best cat
        if(sel.pass(i)) return i;
    }
    return -1;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
