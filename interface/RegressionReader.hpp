#ifndef REGRESSION_READER_H
#define REGRESSION_READER_H

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "interface/Output.hpp"
#include "interface/Corrector.hpp"
		
class RegressionReaderBase{
	//Base class to read a regression
	protected:
		DataStore varsValues_;
		TMVA::Reader* reader_{0};
		bool config = false;
	public:
        string fileName="";

		RegressionReaderBase(){reader_ = new TMVA::Reader(); }
		~RegressionReaderBase(){}
		virtual void Init(){config=true; } //Add Variables
		virtual float Eval()=0;
		virtual const string name(){ return "RegressionReaderBase";}

		void AddVariable(string, char type);

        template<class T>
        void SetVariable( string name, T value){ varsValues_.Set(name, value); }
};


class TauRegression : public RegressionReaderBase, public CorrectorBase
{
	public:
		int correct(Event *e) override final;
		const string name() override final { return "TauRegression" ;} 
		void Init() override final;
        float Eval() override final;

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
