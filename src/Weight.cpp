#include "interface/Weight.hpp"

void Weight::AddMC( string label, string dir, double xsec, double nevents)
{
	mc_db[label] = new MC();
	mc_db[label]->dir =dir;
	mc_db[label]->xsec = xsec;
	mc_db[label]->nevents = nevents;
}

void Weight::AddSF( string label, double sf, double err){
	sf_db[label] = new SF();
	sf_db[label]->sf = sf;
	sf_db[label]->err = err;
	sf_db[label]->syst = 0;
}

void Weight::AddPtEtaSF( string label,double pt1, double pt2 , double eta1, double eta2, double sf, double err){
	if ( sf_db.find(label) == sf_db.end() )
		{
		sf_db[label] = new SF_PtEta();
		}
	SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
	if (p == NULL)
		cout <<"[Weight]::[AddPtEtaSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;

	p->add(pt1, pt2, eta1,eta2,sf,err);
	return;
}

void Weight::resetSystSF(){
	for (auto o : sf_db)
		o.second->syst = 0;
}

void Weight::SetPtEtaSF(string label,double pt, double eta)
{
	 SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
         if (p == NULL)
                 cout <<"[Weight]::[SetPtEtaSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;
	 p->set(pt,eta);
	 return;
}


string Weight::LoadMC( string label) 
{ 
	if( mc_db.find(label) == mc_db.end() )
		return "";
	mcName_ = label; 
	mcXsec_= mc_db[label]->xsec; 
	nEvents_ = mc_db[label] -> nevents; 
	return mc_db[label]->dir;
}
string Weight::LoadMCbyDir( string dir )	 // return "" if failed otherwise label
{
	string label = "";
	for( auto p : mc_db )
		{
		if(p.second -> dir == dir) 
			{
			label = p.first;
			break;
			}
		}
	LoadMC(label);
	return label;
}
