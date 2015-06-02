#ifndef ANALYSIS_BASE_H
#define ANALYSIS_BASE_H


#include "interface/Event.hpp"
#include "interface/Output.hpp"

class AnalysisBase
{
Output *output_;
public:
	AnalysisBase(){};
	~AnalysisBase(){};
	// 
	virtual void inline SetOutput( Output *o ) { output_ = o ;}
	virtual int analyze(Event*,string systname){return 0;}
	virtual void Init(){}
	virtual const string name(){return "AnalysisBase";}

	// call output_->Book, but add something to name
	void Book(string name, string title,int nBins, double xmin, double xmax);
	void Book(string name, string title,int nBins, double *xbound);
	 void Fill(string name, string syst , double value, double weight=1);
	 TH1D* GetHisto(string name, string systname);
};

class Preselection:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "Preselection";}
};

#endif
