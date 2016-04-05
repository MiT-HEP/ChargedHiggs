#ifndef NAMED_H
#define NAMED_H

#include <string>
using namespace std;

class Named
{
    public:
        virtual const string name() const =0;
	void Log(const string & function, const string &level, const string &message);
	void LogN(const string & function, const string &level, const string &message,int N);
};

#endif
