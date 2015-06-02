#include "interface/Smearer.hpp"
int SmearJes::smear(Event *e)
{
	// only on data apply Jes
	if( not e->IsRealData() ) return SMEAR_NA;

	//for (auto j : e->jets_)
	for (auto j : GetJets(e))
		j->syst = syst_;
	//e->met_ .syst = syst_;
	GetMet(e) . syst = syst_;

	return SMEAR_OK;
}

int SmearJer::smear(Event *e)
{
	// only on data apply Jes
	if( e->IsRealData() ) return SMEAR_NA;
	//for (auto j : e->jets_)
	for (auto j : GetJets(e))
		j->syst = syst_;
	return SMEAR_OK;
}
