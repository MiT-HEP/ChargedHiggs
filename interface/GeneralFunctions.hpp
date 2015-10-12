#ifndef GENERAL_FUNCT_H
#define GENERAL_FUNCT_H

#ifndef NULL
    #define NULL 0
#endif

namespace ChargedHiggs{
    // ---
    float deltaPhi(float phi1,float phi2);
    // --- call delete and set to null
    template<typename T> 
    void Delete(T& x) { delete x; x=NULL; }


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
