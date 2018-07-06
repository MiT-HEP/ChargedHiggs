#ifndef MET_H
#define MET_H

#include "interface/Object.hpp"
#include "interface/Smearable.hpp"

class CorrectorBase;

class Met : 
        virtual public Object, 
        virtual public SmearableComplex
{

        TLorentzVector pp4;
    protected:
        // filters:
        bool FullRecommendation{false};

    public:
        Met() : Object(), SmearableComplex() {syst = 0 ;}
        float gen ;
        bool filterbadPFMuon{false};
        bool filterbadChHadrons{false};

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

        inline TLorentzVector & GetP4() override {
            if (syst == 0) return p4;
            else if (syst!=0 ) {
                pp4=p4;
                if (p4.Pt()>0) {
                    return pp4 *= (Pt()/p4.Pt());
                } else {
                    return pp4;
                }
            }
        }
};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
