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
