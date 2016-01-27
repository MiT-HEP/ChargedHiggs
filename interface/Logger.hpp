#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include "interface/AnalysisBase.hpp"

class Logger
{
private:
	Logger(){};
	Logger(Logger const &) = delete;
	void operator=(Logger const&)=delete;
public:
	~Logger(){};
	static Logger & getInstance(){  static Logger l ; return l;}

	void Log(AnalysisBase*a,const string& function, const string & level, const string& message){
		cout <<"["<<a->name()
			<<"]::["<<function <<"]::["<<level<<"]: "
			<<message<<endl;
	}
	void Log(const string& name,const string& function, const string & level, const string& message){
		cout <<"["<<name
			<<"]::["<<function <<"]::["<<level<<"]: "
			<<message<<endl;
	}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 

