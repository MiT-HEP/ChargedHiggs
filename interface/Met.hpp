#ifndef MET_H
#define MET_H

#include "interface/Object.hpp"
#include "interface/Smearable.hpp"

class CorrectorBase;

class Met : 
        virtual public Object, 
        virtual public SmearableComplex
{

    protected:
        // filters:
        bool FullRecommendation{false};

    public:
        Met() : Object(), SmearableComplex() {syst = 0 ;}
        float gen ;

        inline bool passFullRecommendation()const { return FullRecommendation;}
        inline void setFullRecommendation(bool value){ FullRecommendation=value;}

        virtual inline int IsMet() const { return 1; }
        inline int IsObject()const override { return IsMet(); }
        inline void  clearSyst() override { Object::clearSyst(); syst = 0; type=Smearer::NONE;}; // reset smearing
        inline float Pt() const override { if ( syst == 0 ) return p4.Pt();
            else 
            {
            if (syst >0 ) return ptUpSyst[type]  ;
            else return ptDownSyst[type] ;
            }
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
