#ifndef JET_H
#define JET_H

#include "interface/Object.hpp"
#include "interface/Smearable.hpp"
#include <algorithm>
#include <string>
#include <map>

//#include <iostream>
//using namespace std;

// ---
class Jet : virtual public Object, virtual public SmearableComplex
{
    // This class take care of the jet definition in the analysis
    //
    float ptcut_; // ** pt cut on the accepted jets
    float etacut_ ; // ** eta cut on the accepted jets
    float betacut_ ; // ** eta cut on the accepted bjets
    float bcut_; /// ** bcut on the bJets discr
    float etacutcentral_;
    float  qgl_ ; // To Set
    float puidcut_{-100};
    float puId;
    TLorentzVector pp4;

    // qgl vars
    std::map<std::string,float> qglVars_;

    inline int IsJetExceptValidity() const { 
        if( std::isnan(Pt()) ) return 0; 
        if( Pt() < ptcut_ ) return 0; 
        if( fabs(Eta()) >= etacut_) return 0;
        if( puidcut_ > -100 and puidcut_ < 100 and puId < puidcut_ ) return 0; 
        if(puidcut_ == 100 ) { // Loose
            float aeta= abs(Eta());
            float pt = p4.Pt(); // no syst
            if ( aeta< 2.5){
                if (pt >=30 and pt< 50 and puId <-0.89)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.97)  return 0;
            }
            else if (aeta < 2.75){
                if (pt >=30 and pt< 50 and puId <-0.52)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.68)  return 0;
            }
            else if (aeta < 3.00){
                if (pt >=30 and pt< 50 and puId <-0.38)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.53)  return 0;
            }
            else if (aeta < 5.00){
                if (pt >=30 and pt< 50 and puId <-0.30)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.47)  return 0;
            }
        }
        if(puidcut_ == 200 ) { // Medium
            float aeta= abs(Eta());
            float pt = p4.Pt(); // no syst
            if ( aeta< 2.5){
                if (pt >=30 and pt< 50 and puId <+0.61)  return 0;
                if (pt >=10 and pt< 30 and puId <+0.18)  return 0;
            }
            else if (aeta < 2.75){
                if (pt >=30 and pt< 50 and puId <-0.35)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.55)  return 0;
            }
            else if (aeta < 3.00){
                if (pt >=30 and pt< 50 and puId <-0.23)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.42)  return 0;
            }
            else if (aeta < 5.00){
                if (pt >=30 and pt< 50 and puId <-0.17)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.36)  return 0;
            }
        }
        if(puidcut_ == 300 ) { // Tight
            float aeta= abs(Eta());
            float pt = p4.Pt(); // no syst
            if ( aeta< 2.5){
                if (pt >=30 and pt< 50 and puId <+0.86)  return 0;
                if (pt >=10 and pt< 30 and puId <+0.69)  return 0;
            }
            else if (aeta < 2.75){
                if (pt >=30 and pt< 50 and puId <-0.10)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.35)  return 0;
            }
            else if (aeta < 3.00){
                if (pt >=30 and pt< 50 and puId <-0.05)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.26)  return 0;
            }
            else if (aeta < 5.00){
                if (pt >=30 and pt< 50 and puId <-0.01)  return 0;
                if (pt >=10 and pt< 30 and puId <-0.21)  return 0;
            }
        }
        return 1;
    }

    public:

    void SetPuIdCut(float x) {puidcut_=x;}
    void SetPtCut(float x){ptcut_= x;}
    void SetEtaCut(float x){etacut_ = x;}
    void SetEtaCutCentral( float x) {etacutcentral_=x;}
    void SetBCut(float x) {bcut_=x;}
    inline void SetQGL(float qgl){ qgl_ = qgl;}
    inline void SetQGLVar(std::string name,float value){
            std::transform(name.begin(),name.end(),name.begin(),::tolower ) ;
            qglVars_[name] = value;
            };
    void SetPuId(float x) {puId=x;}

    Jet() ; 

    int isValid; // rejected by DR
    int isValidInvIso;

    float bdiscr; // 

    //Gen-level info
    int pdgId;
    int motherPdgId;
    int grMotherPdgId;

    inline int Flavor() const { return pdgId;}

    // ---
    inline float Pt() const override { 
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<"Requested Pt for jet"<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" syst=%d",syst)<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" type=%d",type)<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" UncorrPt=%f",p4.Pt())<<endl;
            // --- if (syst>0) cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" NewPt=%f", ptUpSyst[type])<<endl;
            // --- if (syst<0) cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" NewPt=%f", ptDownSyst[type])<<endl;

            if (syst ==0) return p4.Pt(); 
            //return p4.Pt() *(1.0  + uncSyst[type]*syst );
            if (syst>0 ) return ptUpSyst[type];
            else return ptDownSyst[type];
    }
    inline float E() const override { 
        if (syst == 0) return p4.E(); 
        else return Pt()/p4.Pt() *p4.E();
    }

    inline TLorentzVector & GetP4() override {
        if (syst == 0) return p4;
        if (syst!=0 ) {
            pp4=p4;
            if (p4.Pt()>0) {
                return pp4 *= (Pt()/p4.Pt());
            } else {
                return pp4;
            }
        }
    }

    inline float GetUnc() const { return Pt()/p4.Pt(); }

    inline void  clearSyst() override {Object::clearSyst() ;syst = 0; isValid=1;type=Smearer::NONE;} // reset smearing

    inline float QGL() const { return qgl_; } 
    inline float QGLVar(std::string name) const {
            std::transform(name.begin(),name.end(),name.begin(),::tolower ) ;
            return qglVars_.find(name)->second; // this will throw an exception if not found
            };
    // ---
    inline int IsObject() const override {return IsJet();}

    inline int IsJet() const { 
        if (not isValid) return 0; 
        return IsJetExceptValidity();
    }

    inline int IsJetInvIso() const { 
        if (not isValidInvIso) return 0; 
        return IsJetExceptValidity();
    }

    inline int IsCentralJet() const {
        if ( not IsJet() ) return 0;
        if ( fabs(Eta()) >= etacutcentral_ ) return 0;
        return 1;
    }

    inline int IsCentralJetInvIso() const {
        if ( not IsJetInvIso() ) return 0;
        if ( fabs(Eta()) >= etacutcentral_ ) return 0;
        return 1;
    }

    inline int IsForwardJet() const {
        if ( not IsJet() ) return 0;
        if ( fabs(Eta()) < etacutcentral_ ) return 0;
        return 1;
    }

    inline float Btag() const { return bdiscr ; } // don't use this function, to check if it is a bjet
    inline int IsBJet() const { if( bdiscr > bcut_  and IsJet() and fabs(Eta()) <= betacut_ )   return 1; return 0;}

    inline int IsBJetInvIso() const { if( bdiscr > bcut_  and IsJetInvIso() and fabs(Eta()) <= betacut_ )   return 1; return 0;}

    inline void computeValidity( Object* o, float dR = 0.4,bool inverse=false)
    {
        if (inverse){if ( DeltaR (*o) < dR )  isValidInvIso = 0;}
        else { if ( DeltaR (*o) < dR )  isValid = 0;}
    }
    inline void resetValidity(){isValid=1;isValidInvIso=1;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
