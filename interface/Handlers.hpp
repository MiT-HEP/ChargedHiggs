#ifndef HANDLERS_H
#define HANDLERS_H

#include <exception>

using namespace std;

// handler for sigint: throw exception
void sigint_handler(int s);


//this functions calls the definition in signal in order to deal with SIGINT ...
void define_handlers() ;


//define here all type of execptions that can go across the all program
class sigint_exception: public exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Caught signal CTRL+C";
        }
} ;

// don't catch this exception, may be something  we don't want
class abort : public exception
{
    public:
        virtual const char*what() const throw()
            {
            return "Aborting..." ;
            }
};

class slow : public exception
{
    public:
        virtual const char*what() const throw()
            {
            return "Slow machine..." ;
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
