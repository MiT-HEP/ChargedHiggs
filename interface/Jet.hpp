#ifndef JET_H
#define JET_H

#include "interface/Object.hpp"
#include "interface/Smearable.hpp"
#include "interface/Handlers.hpp"
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
    float unc_ ; // not used in the systematic smearing, but in the kinematic fits. Relative unceratainty

    float puidcut_{-100};
    float puId;

    bool  eenoise_{false};// reject noise from ee with cuts

    float deepB; // ** deep btag discriminator (B+BB)
    float deepbcut_ {-100};

    float deepC;
    float deepccut_ {-100};

    float nemf_,cemf_;
    float area_;

    float bcorr_{1.}, bcorrunc_{1.};

    TLorentzVector pp4;

    int hadFlavor_;

    // qgl vars
    std::map<std::string,float> qglVars_;

    public:

    ///@brief return if it is a jet, except object cleaning
    int IsJetExceptValidity() const ;

    int PassPuId() const ;
    int PassEENoise() const;

    ///@brief set ee noise cut on
    void SetEENoiseCut(bool x){eenoise_ = x;}
    ///@brief set the pu id cut
    void SetPuIdCut(float x) {puidcut_=x;}
    ///@brief set the pt cut 
    void SetPtCut(float x){ptcut_= x;}
    ///@brief set the pt cut 
    float GetPtCut() const {return ptcut_;}
    ///@brief set the eta cut
    void SetEtaCut(float x){etacut_ = x;}
    ///@brief get the eta cut
    float GetEtaCut() const { return etacut_ ;}
    ///@brief set the the eta cut for the "central" region
    void SetEtaCutCentral( float x) {etacutcentral_=x;}
    ///@brief set the the csv cut (only one between standard and deep requirement can be set). To unset use -100.
    void SetBCut(float x) {bcut_=x;}
    ///@brief set the deep b csv tagger (deep b+bb). only one between std and deep can be set. To unset use -100
    void SetDeepBCut(float x) {deepbcut_=x;}
    ///@brief set the deep c csv tagger . only one between std and deep can be set. To unset use -100
    void SetDeepCCut(float x) {deepccut_=x;}
    ///@brief set the B Regressions and unc
    void SetBCorrection(float x, float unc){bcorr_=x;bcorrunc_=unc;}
    ///@brief set the QGL discriminator
    inline void SetQGL(float qgl){ qgl_ = qgl;}
    ///@brief set variable 
    inline void SetQGLVar(std::string name,float value){
            std::transform(name.begin(),name.end(),name.begin(),::tolower ) ;
            qglVars_[name] = value;
            };
    /// @brief set the pileup id
    void SetPuId(float x) {puId=x;}
    /// @brief get the pu id value
    inline float GetPuId() const {return puId;}
    /// @brief set the hadron flavour
    void SetHadFlavor(const int& x) {hadFlavor_=x;}

    ///@brief  set deepB with sf
    //inline float SetDeepBSF(float x){ deepB_SF=x; isDeepBSFSet=1;}
    /// @brief get the deep B tag discr (B+BB)
    //inline float GetDeepB() const {if (isDeepBSFSet) return deepB_SF; else return deepB;}
    //inline float GetBareDeepB() const {return deepB;}
    inline float GetDeepB() const { return deepB;}
    /// @brief set the deep B discri (B+BB)
    void SetDeepB(const float& x) {deepB=x;}

    //@set and obtain deep C tag
    void SetDeepC(const float& x) {deepC=x;}
    inline float GetDeepC() const { return deepC;}

    ///@brief get bregression
    inline float GetBCorrection() const{return bcorr_;}
    inline float GetBCorrectionUncertainty() const{return bcorrunc_;}

    //float nemf_,cemf_;
    void SetNEMF(const float x) {nemf_=x;}
    void SetCEMF(const float x) {cemf_=x;}
    inline float GetNEMF()const {return nemf_;}
    inline float GetCEMF()const {return cemf_;}

    //Set and obtain jet area
    void SetArea(const float x) {area_=x;}
    inline float GetArea() const {return area_;}

    /// @brief constructor
    Jet() ; 

    /// @brief rjected by DR with other analysis objects
    int isValid; // rejected by DR
    /// @brief reject by DR with CR analysis objects
    int isValidInvIso;

    /// @brief std btag discriminator
    float bdiscr; // 
    /// @brief rawPt
    float rawPt;

    //Gen-level info
    int pdgId;
    int motherPdgId;
    int grMotherPdgId;

    ///@brief return parton flavor. >94X will be from ghosts
    inline int Flavor() const { return pdgId;}
    ///@brief return hadron flavor
    inline int hadFlavor() const { return hadFlavor_;}
    
    //@brief get JES uncertainty
    inline float GetJESUnc() const { return (ptUpSyst[Smearer::JES]/p4.Pt()-1.0) ;}
    /// @brief return pt, corrected by systematics
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

    /// @brief return Energy corrected by systematics
    inline float E() const override { 
        if (syst == 0) return p4.E(); 
        else return Pt()/p4.Pt() *p4.E();
    }
    
    /// @brief return p4 corrected by systematics
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
    
    /// @brief return systematic uncertainty under consideration
    inline float GetUnc() const { return Pt()/p4.Pt(); }

    /// @brief clear systematics
    inline void  clearSyst() override {Object::clearSyst() ;syst = 0; isValid=1;type=Smearer::NONE;} // reset smearing

    /// @brief return qgl discriminator value
    inline float QGL() const { return qgl_; } 
    inline float QGLcorrect() const { 
        // pythia
        if (fabs(Eta())> 2.0 ) return qgl_;
        if (Flavor() == 21)  return -55.7067*TMath::Power(qgl_,7) + 113.218*TMath::Power(qgl_,6) -21.1421*TMath::Power(qgl_,5) -99.927*TMath::Power(qgl_,4) + 92.8668*TMath::Power(qgl_,3) -34.3663*qgl_*qgl_ + 6.27*qgl_ + 0.612992;
        if (Flavor() !=0 and abs(Flavor())<5)return -0.666978*qgl_*qgl_*qgl_ + 0.929524*qgl_*qgl_ -0.255505*qgl_ + 0.981581;
        return qgl_;
    };

    /// @brief return variable (name) of qgl discriminator
    inline float QGLVar(std::string name) const {
            std::transform(name.begin(),name.end(),name.begin(),::tolower ) ;
            bool doLog=false;
            if (name.find("log:") != string::npos) { doLog=true; name=name.substr(4);}
            if (not doLog)return qglVars_.find(name)->second; // this will throw an exception if not found
            else return (qglVars_.find(name)->second >0 )?-TMath::Log( qglVars_.find(name)->second):0.;
            };
    // ---
    inline int IsObject() const override {return IsJet();}

    /// @brief return true if it is a jet to be considered. (pass all selection)
    inline int IsJet() const { 
        if (not isValid) return 0; 
        return IsJetExceptValidity();
    }
    
    /// @brief return true if it is a jet to be considered in CR (inverse isolation validation)
    inline int IsJetInvIso() const { 
        if (not isValidInvIso) return 0; 
        return IsJetExceptValidity();
    }

    /// @brief return true if jet is central and pass full selection
    inline int IsCentralJet() const {
        if ( not IsJet() ) return 0;
        if ( fabs(Eta()) >= etacutcentral_ ) return 0;
        return 1;
    }

    /// @brief return true if jet is central and pass inverse isolation selection (CR)
    inline int IsCentralJetInvIso() const {
        if ( not IsJetInvIso() ) return 0;
        if ( fabs(Eta()) >= etacutcentral_ ) return 0;
        return 1;
    }
    
    /// @brief return true if jet is forward (non central) and pass full selection
    inline int IsForwardJet() const {
        if ( not IsJet() ) return 0;
        if ( fabs(Eta()) < etacutcentral_ ) return 0;
        return 1;
    }

    /// @brief return btag discriminator. Don't use this function to check if it is a bjet
    inline float Btag() const { return bdiscr ; } // don't use this function, to check if it is a bjet

    /// @brief return true if it is a tagged bjet
    int IsBJet() const ;

    /// @brief return true
    int IsBJetInvIso() const ;

    /// @brief update validity of jet (dR) with respect to an object.. inverse will use it for the inverse isolation requirements.
    inline void computeValidity( Object* o, float dR = 0.4,bool inverse=false)
    {
        if (inverse){if ( DeltaR (*o) < dR )  isValidInvIso = 0;}
        else { if ( DeltaR (*o) < dR )  isValid = 0;}
    }
    /// @brief reset valitidy status.
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
