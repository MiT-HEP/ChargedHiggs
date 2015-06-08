#ifndef OUTPUT_H
#define OUTPUT_H

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <map>
using namespace std;

// do we want it implemented as singleton ?

class Output{
protected:
	// 
	TFile *file_;
	map<string,TH1D*> histos_;
	inline TH1D* Get(string name){ return histos_[name];}

	void CreateDir(string dir); // called by Write
public:
	Output(){file_=NULL;}
	~Output(){}
	void Close();
	void Open(string name) ;
	void Write();
	// -- Book Histo
	void Book(string name, string title,int nBins, double xmin, double xmax);
	void Book(string name, string title,int nBins, double *xbound);
	//
	 virtual void Fill(string name, string syst , double value, double weight=1);
	 TH1D* Get(string name,string systname);

};

#endif 
