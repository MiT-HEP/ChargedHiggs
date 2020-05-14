#ifndef SDMass_H
#define SDMass_H

#include "interface/Corrector.hpp"

class SDMasscorr : public CorrectorBase{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "SDMasscorr";}
		void Init() override;
                float getPUPPIweight(float puppipt, float puppieta);
        int year{2017};

    protected:

  std::unique_ptr<TFile> _file;
  std::unique_ptr<TF1> puppisd_corrGEN;
  std::unique_ptr<TF1> puppisd_corrRECO_cen;
  std::unique_ptr<TF1> puppisd_corrRECO_for;


};
#endif
