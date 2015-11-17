#ifndef GENERAL_FUNCT_H
#define GENERAL_FUNCT_H

#ifndef NULL
    #define NULL 0
#endif

namespace ChargedHiggs{
    // ---
    float deltaPhi(const float phi1,const float phi2);
    // --- call delete and set to null
    template<typename T> 
    void Delete(T& x) { delete x; x=NULL; }

    float mt(const float pt1, const float pt2, const float phi1, const float phi2);


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
