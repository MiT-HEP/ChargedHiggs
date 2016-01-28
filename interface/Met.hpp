#ifndef MET_H
#define MET_H

#include "interface/Object.hpp"

class CorrectorBase;

class Met: virtual public Object
{

    public:
        Met() : Object() {syst = 0 ;}
        float ptUp; // TOFILL -> TODO COPY OPERATOR FOR SYST
        float ptDown; // TOFILL
        int syst ;
        float gen ;

        virtual inline int IsMet() const { return 1; }
        inline int IsObject()const override { return IsMet(); }
        inline void  clearSyst() override { Object::clearSyst(); syst = 0;}; // reset smearing
        inline float Pt() const override { if ( syst == 0 ) return p4.Pt();
            else if (syst >0 ) return ptUp * syst ;
            else return ptDown * syst;
        }
        virtual inline float PtGen(){ return gen;} 
};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
