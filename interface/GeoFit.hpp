#ifndef GEOFIT_H
#define GEOFIT_H
namespace PtGeoCor{

  //float PtGeoFit(float d0, float pt, float eta, int year) ;
  float PtGeoFit_mod(float d0, float pt, float eta, int year) ; // this one
}

#include "interface/Corrector.hpp"

class GeoFit : public CorrectorBase{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "GeoFit";}
		void Init() override;

        int year{2017};
    protected:
};
#endif
