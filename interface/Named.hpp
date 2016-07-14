#ifndef NAMED_H
#define NAMED_H

#include <string>
using namespace std;

class Named
{
    public:
        virtual const string name() const =0;
	void Log(const string & function, const string &level, const string &message) const;
	void LogN(const string & function, const string &level, const string &message,int N) const;
	void LogErr(const string & function, const string &level, const string &message) const;
};

#endif
