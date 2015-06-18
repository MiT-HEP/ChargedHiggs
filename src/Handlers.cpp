#include "interface/Handlers.hpp"
#include <csignal>
#include <cstdlib>

void sigint_handler(int s){
    throw  sigint_exception();
}

void define_handlers()
{
    signal (SIGINT,sigint_handler);
    signal (SIGTERM,sigint_handler);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
