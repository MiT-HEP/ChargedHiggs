#ifndef TRACKJET_H
#define TRACKJET_H

#include "interface/Object.hpp"
#include "interface/Handlers.hpp"
#include <algorithm>
#include <string>

class TrackJet : virtual public Object
{
    // This class take care of the track jet definition in the analysis
    // it is used to compute the low track activity variables (NJets, Ht)

    //
    int isValid; // rejected by DR

    public:

    ///@brief return if it is a jet, except object cleaning
    inline int IsTrackJetExceptValidity() const {return true;}
    ///@brief return if it is a jet, to be considered in the analysis
    inline int IsTrackJet() const {return isValid;}

    /// @brief constructor
    TrackJet() ; 
    /// @brief destructor
    ~TrackJet() ; 

    /// @brief rjected by DR with other analysis objects

    /// @brief clear systematics
    inline void  clearSyst() override {Object::clearSyst() ; isValid=1;} 

    /// @brief update validity of jet (dR) with respect to an object.. inverse will use it for the inverse isolation requirements.
    inline void computeValidity( Object* o, float dR = 0.4){
         if ( DeltaR (*o) < dR )  isValid = 0;
    }
    /// @brief reset valitidy status.
    inline void resetValidity(){isValid=1;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
