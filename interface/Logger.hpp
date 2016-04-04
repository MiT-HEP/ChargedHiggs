#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include "interface/Named.hpp"
#include "interface/AnalysisBase.hpp"


class Logger
{
private:
	Logger(){}; // Singleton
	Logger(Logger const &) = delete; // Singleton
	void operator=(Logger const&)=delete; // Singleton

    map<string,int> counter_;
public:
	~Logger(){}; // Singleton
	static Logger & getInstance(){  static Logger l ; return l;} // Singleton

    // ---
    void Log(Named*a ,const string & function, const string &level, const string & message)
    {
        return Log(a->name(), function,level,message);
    }

	void Log(const string& name,const string& function, const string & level, const string& message){
		cout <<"["<<name
			<<"]::["<<function <<"]::["<<level<<"]: "
			<<message<<endl;
	}

    void LogN( const string & name, const string& function, const string & level, const string & message,int N);
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 

