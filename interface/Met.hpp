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
        TLorentzVector rawMet;
        TLorentzVector trackMet;

        TLorentzVector puppiMet;
public:
        SmearableComplexP4 puppiMetSyst; // make Private and make calls
private:

        float gen;
        float gen_phi;
        float significance;

    protected:
        // filters:
        bool FullRecommendation{false};

    public:

        Met() : Object(), SmearableComplex() {syst = 0 ;}
        bool filterbadPFMuon{false};
        bool filterbadChHadrons{false};

        inline bool passFullRecommendation()const { return FullRecommendation;}
        inline void setFullRecommendation(bool value){ FullRecommendation=value;}

        virtual inline int IsMet() const { return 1; }
        inline int IsObject()const override { return IsMet(); }
        inline void  clearSyst() override { Object::clearSyst(); syst = 0; type=Smearer::NONE; puppiMetSyst.syst=0; puppiMetSyst.SetSmearType(Smearer::NONE);}; // reset smearing
        inline float Pt() const override { if ( syst == 0 ) return p4.Pt();
            else 
            {
            if (syst >0 ) return ptUpSyst[type]  ;
            else return ptDownSyst[type] ;
            }
        }
        virtual inline float PtGen() const { return gen;} 
        virtual inline void SetGenPt(float pt) {gen=pt;}
        virtual inline void SetGenPhi(float phi) {gen_phi=phi;}
        virtual inline void SetSignificance(float s) {significance=s;}
        virtual void SetRawMetP4(TLorentzVector &x){rawMet=x;}
        TLorentzVector& GetRawMetP4(){return rawMet;}

        virtual void SetTrackMetP4(TLorentzVector &x){trackMet=x;}
        TLorentzVector& GetTrackMetP4(){return trackMet;}

        virtual void SetPuppiMetP4(TLorentzVector &x){puppiMet=x;}
        TLorentzVector& GetBarePuppiMetP4(){return puppiMet;}

        TLorentzVector& GetPuppiMetP4(){
            if (puppiMetSyst.syst ==0) return puppiMet;
            else if (puppiMetSyst.syst >0){
                return puppiMetSyst.p4UpSyst[puppiMetSyst.type];
            }
            else if (puppiMetSyst.syst<0){
                return puppiMetSyst.p4DownSyst[puppiMetSyst.type];
            }
            return puppiMet;
        }

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
