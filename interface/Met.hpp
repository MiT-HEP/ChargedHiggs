#ifndef MET_H
#define MET_H

class Met: virtual public Object
{
    public:
        Met(){syst = 0 ;}
        float ptUp; // TOFILL
        float ptDown; // TOFILL
        int syst ;
        float gen ;

        virtual inline int IsMet(){ return 1; }
        virtual inline int IsObject(){ return IsMet(); }
        virtual inline void  clearSyst(){syst = 0;}; // reset smearing
        virtual inline float Pt(){ if ( syst == 0 ) return p4.Pt();
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
