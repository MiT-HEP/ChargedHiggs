#include "interface/Named.hpp"
#include "interface/Logger.hpp"

void Named::Log(const string & function, const string &level, const string &message)
{	
    Logger::getInstance().Log(this,function,level,message );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
