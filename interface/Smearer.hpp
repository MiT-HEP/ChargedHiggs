#ifndef SMEAR_H
#define SMEAR_H

#include "interface/Event.hpp"
#include "interface/Named.hpp"

#define SMEAR_OK 0
#define SMEAR_NA 1
#define SMEAR_UNKOWN_FAIL 2

// -------------------- DEFINE SMEARERs --------------

class SmearBase : public Named {
    public:

        SmearBase(){};
        ~SmearBase(){};

        // ---
        string name_ = "NONE";
        int syst_{0};
        const inline string name() const {
            if (syst_ == 0 ) return name_;
            else if (syst_ >0 ) return name_ + "Up" ; // this name should be consistent with datacards
            else  return name_ + "Down" ;
        }
        //const inline string operator(){return name();};
        virtual void inline SetSyst(int s){syst_=s;}
        // ---
        virtual int smear(Event *e){return 0;}; // 0 = success; 1 not applicable to the event
        virtual int smear(CorrectorBase *c){return 0;}; // 0 = success; 1 not applicable to the event
        virtual inline vector<Jet*> &GetJets(Event *e){return e->jets_;}
        virtual inline vector<Tau*> &GetTaus(Event *e){return e->taus_;}
        virtual inline Met &GetMet(Event *e){return e->met_;}
};

class SmearLepSF : virtual public SmearBase
{
    public:
        SmearLepSF() :SmearBase(){ Log(__FUNCTION__,"WARNING","Obsolete. Use SmearSF");name_= "LEPSF";}
        int smear(Event *e) override;
};

class SmearJes : virtual public SmearBase
{
    public:
        SmearJes() : SmearBase(){ name_ = "JES";}
        int smear(Event*e) override;	
};

class SmearTauScale : virtual public SmearBase
{
    public:
        SmearTauScale() : SmearBase(){ name_ = "TAUSCALE";}
        int smear(Event*e) override;	
};

class SmearJer:virtual public SmearBase
{
    public:
        SmearJer() : SmearBase(){ name_ = "JER";}
        int smear(Event*e) override;	

};

class SmearUncluster : virtual public SmearBase
{
    public:
        SmearUncluster() : SmearBase(){ name_ = "UNCLUSTER";}
        int smear(Event*e) override;	
};

class SmearSF : virtual public SmearBase
{
    string sfname_{""};
    public:
        SmearSF() :SmearBase(){ name_= "SF";}
        SmearSF(string name,string sf) :SmearBase(){ name_= name; sfname_=sf;}
        int smear(Event *e) override;
};

class SmearPu : virtual public SmearBase
{
    public:
        SmearPu() : SmearBase(){ name_ = "PU";}
        int smear(Event*e) override;	
};

class SmearScales : virtual public SmearBase
{
    /*
     *Never tested, nor implemented the load of the Nevents Reweighted in the MC. 
     *These info  not available at the moment
     */
    bool doRen{true};
    bool doFac{true};

    public: 
        SmearScales() : SmearBase(){ name_="Scale";}
        SmearScales(string name, bool ren, bool fac) : SmearBase(){ name_=name;doRen=ren,doFac=fac;}
        int smear(Event*e) override;	
};

class SmearPdfs : virtual public SmearBase
{
    /*
     *Never tested, nor implemented the load of the Nevents Reweighted in the MC. 
     *These info  not available at the moment
     */
    int num_; // from 0- each round I'll do 2: pos=num*2+(syst_>0)?1:0
    int GetPos() const { return num_*2 + ((syst_>0)?1:0);}
    public: 
        SmearPdfs() : SmearBase(){ name_="Pdf";num_=0;}
        SmearPdfs(int num) : SmearBase(){num_=num; name_="Pdf";}
        const inline string name() const override { return  name_ + Form("%d",GetPos());}
        int smear(Event*e) override;	
};

class SmearBjets : virtual public SmearBase
{
    /* for simple smearing, correlated use SmearSF*/
    bool doL{false},doB{false};
    string sfname_;

    public:
        SmearBjets(){}
        SmearBjets(bool bjets, bool udsc,string sf){ doL=udsc; doB=bjets; name_= string("BTAG") + ((doB)?"B":"") + ((doL)?"L":""); sfname_=sf;}
        ~SmearBjets(){}
        int smear(Event*e) override;	
        const string sfname(){ return sfname_ ;}

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
