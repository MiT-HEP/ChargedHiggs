#ifndef SMEAR_H
#define SMEAR_H

#include "interface/Event.hpp"
#include "interface/Named.hpp"
#include "interface/Systematics.hpp"
#include <memory>
#include <string>

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
        virtual inline vector<FatJet*> &GetFatJets(Event *e){return e->fat_;}
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

class SmearJesPuppiMet : virtual public SmearBase
{
    public:
        SmearJesPuppiMet() : SmearBase(){ name_ = "JESPUPPIMET";}
        int smear(Event*e) override;	
};

class SmearJerPuppiMet : virtual public SmearBase
{
    public:
        SmearJerPuppiMet() : SmearBase(){ name_ = "JERPUPPIMET";}
        int smear(Event*e) override;	
};

class SmearUnclusteredPuppiMet : virtual public SmearBase
{
    public:
        SmearUnclusteredPuppiMet() : SmearBase(){ name_ = "UNCLUSTEREDPUPPIMET";}
        int smear(Event*e) override;	
};


class SmearJesAndCSV : virtual public SmearBase
{
    // constructor: num -> 1,2,3,4,5 for simplicity
    //num_ 7->16
    //syst= 7 -> JES UP, 8 JES Down, 
    //      9 LF UP 10,
    //      11/12/HF UP/Down,
    //      13/14 Stat1, 
    //      15,16 Stat2
    //
    std::unique_ptr<SmearJes> jes;
    int doSmearJes_{1};
    //int num_; // this follow the  CSV Reweight skim, while  the constructor is 1->5
    Systematics::Type num_;
    int bsystType=0; // 0 = CSVReweight, 1= CSV
    std::string bdeep_{"bdeep"};

    public:
        SmearJesAndCSV() : SmearBase(){ 
            num_=Systematics::NA; // --> Systematics::Type
            name_ = Form("CSVR_NA");
        }
        SmearJesAndCSV(int num, int bsyst =0, const std::string& bdeep="bdeep", int doSmearJes=1) : SmearBase(){ 
            // num -> 1,5 (1 is JES
            bsystType=bsyst;
            bdeep_=bdeep;
            num_ = Systematics::NA;
            if (num == 1){num_=Systematics::JESup;name_="JESANDBR";}
            if (num == 2){num_=Systematics::CSVHFup;name_="BRHF";}
            if (num == 3){num_=Systematics::CSVLFup;name_="BRLF";}
            if (num == 4){num_=Systematics::CSVHFStats1up; name_="BRHFSTAT1";}
            if (num == 5){num_=Systematics::CSVHFStats2up; name_="BRHFSTAT2";}
            if (num == 6){num_=Systematics::CSVLFStats1up; name_="BRLFSTAT1";}
            if (num == 7){num_=Systematics::CSVLFStats2up; name_="BRLFSTAT2";}
            if (num == 8){num_=Systematics::CSVCErr1up; name_="BRCFERR1";}
            if (num == 9){num_=Systematics::CSVCErr2up; name_="BRCFERR2";}

            jes . reset (new SmearJes());
            doSmearJes_=doSmearJes;
        }
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

class SmearL1Prefire : virtual public SmearBase
{
    /*
     *Never tested.
     */

    public: 
        SmearL1Prefire() : SmearBase(){ name_="L1Prefire";}
        SmearL1Prefire(string name) : SmearBase(){ name_=name;}
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

#include "interface/SF.hpp"

class SmearWG1 : virtual public SmearBase
{
    /* for simple smearing, correlated use SmearSF*/
    int num_{0};
    string sfname_{"wg1"};


    //const SF_WG1::TYPES type=SF_WG1::THU_2017;
    const SF_WG1::TYPES type=SF_WG1::THU_WG1;

    public:
    // mu, res, mig01, mig12, vbf2j, vbf3j
    // The last numbers are pT dependent uncertainies
        SmearWG1(){}
        SmearWG1(int n){num_=n; 
            switch (num_){
                case 0: name_="NONE"; break;
                case 1: name_="WG1Mu"; break;
                case 2: name_="WG1Res"; break;
                case 3: name_="WG1Mig01"; break;
                case 4: name_="WG1Mig12"; break;
                case 5: name_="WG1VBF2j"; break;
                case 6: name_="WG1VBF3j"; break;
                case 7: {
                        if (type == SF_WG1::THU_WG1) name_="WG1Pt";
                        if (type == SF_WG1::THU_2017) name_="WG1Pt60";
                        break;
                        }
                case 8: {
                        if(type==SF_WG1::THU_WG1) name_="WG1TopMass"; 
                        if(type==SF_WG1::THU_2017) name_="WG1Pt120"; 
                        break;
                        }
                case 9: {
                        if(type==SF_WG1::THU_WG1) name_="WG1NoSyst"; 
                        if(type==SF_WG1::THU_2017) name_="WG1TopMass"; 
                        break;
                        }
                case 10: name_="WG1_VBF_TOT"; break;
                case 11: name_="WG1_VBF_PTH200"; break;
                case 12: name_="WG1_VBF_MJJ60"; break;
                case 13: name_="WG1_VBF_MJJ120"; break;
                case 14: name_="WG1_VBF_MJJ350"; break;
                case 15: name_="WG1_VBF_MJJ700"; break;
                case 16: name_="WG1_VBF_MJJ1000"; break;
                case 17: name_="WG1_VBF_MJJ1500"; break;
                case 18: name_="WG1_VBF_25"; break;
                case 19: name_="WG1_VBF_JET01"; break;
                default: name_="WG1NoSyst"; break; // only  7 uncertainties
            };
        }
        ~SmearWG1(){}
        int smear(Event*e) override;	
        const string sfname(){ return sfname_ ;}

};

class SmearPNetMass : public SmearBase 
{
    public: 
        SmearPNetMass() : SmearBase(){ name_ = "PNetMass";}
        int smear(Event *e) override
        { 
            if( e->IsRealData() ) return SMEAR_NA;

            for (auto j : GetFatJets(e))
            {
                j->systPNetMass_ = syst_;
            }
            return SMEAR_OK;
        }; // 0 = success; 1 not applicable to the event
};

class SmearPNetDiscriminator : virtual public SmearBase
{
    public: 
        SmearPNetDiscriminator() : SmearBase(){ name_ = "PNetXXX";}
        SmearPNetDiscriminator(const string & myname) : SmearBase(){ name_ = myname; if (name_ != "PNetXqq" and name_!="PNetXbb" and name_!="PNetXcc") Log(__FUNCTION__,"ERROR","Not a good discriminator type:"+myname) ;}
        int smear(Event *e) override
        { 
            if( e->IsRealData() ) return SMEAR_NA;

            for (auto j : GetFatJets(e))
            {
                if (name_ =="PNetXqq") j->systPNetXqq_ = syst_;
                if (name_ =="PNetXbb") j->systPNetXbb_ = syst_;
                if (name_ =="PNetXcc") j->systPNetXcc_ = syst_;
            }
            return SMEAR_OK;
        }; // 0 = success; 1 not applicable to the event
};

class SmearResTagger : virtual public SmearBase
{
    public:
        SmearResTagger():SmearBase(){name_="ResTagger";}
        // void, the analysis take care to change it if the name is matched
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
