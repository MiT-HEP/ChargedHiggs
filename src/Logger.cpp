#include "interface/Logger.hpp"

void Logger::LogN( const string & name, const string& function, const string & level, const string & message,int N){
	string all= "[" + name + "]::[" + function + "]::[" + level + "]: " + message ;
	if (counter_[all]< N) { Log(name,function,level,message) ; counter_[all] +=1 ; }
	return;
}

