#ifndef CUTSELECTOR_H
#define CUTSELECTOR_H

#include <cstdint>
#include <bitset>

class CutSelector{
    private:
        uint32_t cut_=0; // actual cuts passed 
        uint32_t mask_=0; // max number of bit to consider, 00111111
    public:
        // --- constructor and destructor
        CutSelector(){}
        CutSelector(size_t maxn){SetMask(maxn);}
        CutSelector(uint32_t cut, size_t maxn) { SetCut(cut); SetMask(maxn);}
        ~CutSelector(){}
        // ---
        uint32_t raw() const { return cut_;}
        uint32_t mask() const { return mask_;}
        // ---
        void SetMask(size_t maxn) {mask_ = (1<<(maxn+1)) -1 ;}//{ mask_=0; for(size_t i=0; i<maxn ;++i) mask_ |= (1<<i) ;  }
        void SetCut(uint32_t cut) {cut_= cut;}
        void SetCutBit(size_t i) { cut_ |= (1<<i) ;}
        void UnSetCutBit(size_t i) { cut_ &= ~(1<<i) ;}
        inline void reset(){ SetCut(0); } 
        // ---
        bool passAll() const  { return !( ~cut_ & mask_);} 
        bool passAllExcept(const size_t i) const { uint32_t mymask= mask_; mymask &= ~( 1<<i) ; return  !( ~cut_ & mymask); }
        bool pass(const size_t i) const  { return (cut_ & mask_ & (1<<i) );} // apply mask anyway, for consistency
        bool passAllUpTo(const size_t i) const  { uint32_t mymask = (1<<(i+1)) -1; return !( ~cut_ & mymask); } // apply mask anyway, for consistency
        friend ostream& operator<<(ostream& os, const CutSelector &cs){ os << "Cut="<< std::bitset<32>(cs.cut_) <<" Mask="<< std::bitset<32>(cs.mask_) ; return os; }

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
