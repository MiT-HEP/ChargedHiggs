#include "interface/Event.hpp"

void Event::ClearEvent(){

	for (auto o :  jets_ ) delete o;
	for (auto o :  leps_ ) delete o;
	for (auto o :  taus_ ) delete o;

	jets_ . clear();
	leps_ . clear();
	taus_ . clear();

}
