#include "interface/GenParticle.hpp"
// removed from the header, for the dictionaries
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"

bool GenParticle::IsPromptFinalState() const { return flags_ & BareMonteCarlo::PromptFinalState ;}
bool GenParticle::IsPromptDecayed() const { return flags_ & BareMonteCarlo::PromptDecayed ;}
bool GenParticle::IsDirectPromptTauDecayProductFinalState() const { return flags_ & BareMonteCarlo::DirectPromptTauDecayProductFinalState ;}
bool GenParticle::IsDressed() const { return flags_ & BareMonteCarlo::Dressed;}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
