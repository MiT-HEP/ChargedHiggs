#include "interface/Logger.hpp"

void Logger::LogN( const string & name, const string& function, const string & level, const string & message,int N,const string&extra){
	string all= "[" + name + "]::[" + function + "]::[" + level + "]: " + message ; //except extra
	if (counter_[all]< N) { Log(name,function,level,message+extra) ; counter_[all] +=1 ; }
	return;
}

