#ifndef KAMUCA_H
#define KAMUCA_H

#include "interface/Corrector.hpp"
#include "interface/Smearable.hpp"
#include "interface/KalmanMuonCalibrator.hpp"

#include <memory>
#include <map>
#include <string>

class KalmanMuonResolution{
    private:
        // files
        map<string,std::unique_ptr<TFile> > files_;
        map<string,std::unique_ptr<TH1> > hists_;
        float getL(float eta);
        float getRes(float a, float b, float c, float e, float p2, float L);

        float res_mc_;
        float res_data_;
        float res_mc_error_;
        float res_data_error_;

    public:
        KalmanMuonResolution(){init();}
        KalmanMuonResolution(const string& dir){init(dir);}
	    static constexpr float L0=103.8; // maximum track length
	    static constexpr float rmax=108.0;
        void init(const string& dir="aux/");
        void setPtEta(float pt, float eta ) ;
        inline float getResMC() const {return res_mc_;}
        inline float getResData() const {return res_data_;}
        inline float getErrorResMC() const {return res_mc_error_;}
        inline float getErrorResData() const {return res_data_error_;}
        inline float getSF() const { return res_data_/res_mc_;} 
};

class KaMuCa : public CorrectorBase,
    public SmearableBase
{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "KaMuCa";}
		void Init() override;
        bool doResolution{true};
    private:
        //
		std::unique_ptr<KalmanMuonCalibrator> correctorMC_;
		std::unique_ptr<KalmanMuonCalibrator> correctorDATA_;
        std::unique_ptr<KalmanMuonResolution> resolution_;
};

#include "interface/Smearer.hpp"

class SmearKaMuCa : public SmearBase
{
    int num_; // from 0 to c.getN()
    public:
        SmearKaMuCa(): SmearBase() { name_ = "KAMUCA"; num_=0;}
        SmearKaMuCa(int num): SmearBase() { name_ = "KAMUCA"; num_=num;}
        const inline string name() const override { return  name_ + Form("%d",num_);}
        int smear(CorrectorBase*c) override; 	
    
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
