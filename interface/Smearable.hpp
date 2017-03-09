#ifndef SMEARABLE_H
#define SMEARABLE_H
// -------------------- DEFINE OBJECTS THAT CAN BE SMEARED ---------------
namespace Smearer{
    // this types are used by complex object to define the systematic shift
    enum smearType{
        JES=0,
        JER,
        TAUESCALE,
        UNCLUSTER,
        SOURCES,
        NONE
    };

};

class SmearableBase{
    protected:
    public: 
        SmearableBase(){syst=0;}
        ~SmearableBase(){}
        int syst;

};

class SmearableComplex: virtual public SmearableBase{
    protected:
        bool filled[Smearer::NONE]; // warn if the syst shifts are not filled correctly
        float ptUpSyst[Smearer::NONE];
        float ptDownSyst[Smearer::NONE];
        Smearer::smearType type{Smearer::NONE};
    public: 
        SmearableComplex():SmearableBase(){}
        ~SmearableComplex(){}
        void SetSmearType(Smearer::smearType x){ type= x;}
        bool IsFilled() const { return filled[type] ; }

        inline void SetFilled(Smearer::smearType x,bool value=true){filled[x]=value;}
        inline void SetValueUp(Smearer::smearType x,float value=true){ptUpSyst[x]=value;}
        inline void SetValueDown(Smearer::smearType x,float value=true){ptDownSyst[x]=value;}


};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
