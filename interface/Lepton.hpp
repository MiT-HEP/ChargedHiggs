#ifndef LEPTON_H
#define LEPTON_H

#include "interface/Object.hpp"

class Lepton : virtual public Object
{
    protected:
        float isocut_;
        float ptcut_;
    public:
        Lepton() ;

        float iso; // isolation 
        int charge; // charge +1 -1
        int type;// abspdgid 11 o 13 

        virtual inline int IsLep(){ 
            if ( iso > isocut_) return 0;
            if ( Pt() < ptcut_ ) return 0;

            return 1;
        }
        virtual inline bool IsElectron(){ return IsLep() and (type == 11); }
        virtual inline bool IsMuon(){ return IsLep() and (type == 13); }
        virtual inline int   IsObject(){return IsLep();}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
