#include "interface/Dumper.hpp"
#include "interface/Logger.hpp"

#include "NeroProducer/Core/interface/BareCollection.hpp"
void Dumper::SetOutDir(string dir)
{ 
    Log(__FUNCTION__,"INFO","Output Dir Set to " + dir );
    outdirname_ = dir;
}
void Dumper::SetBaseName(string name){
    Log(__FUNCTION__,"INFO","Base Name Set to " + name );
    basename_=name;
}
void Dumper::ActivateDump(bool x) 
{
    if (x ) Log(__FUNCTION__,"INFO","Dumper has been activated " );
    else    Log(__FUNCTION__,"INFO","Dumper has been de-activated " );
    dump_=x;
}

void Dumper::InitTree(const vector<BareCollection*> &bare)
{
    if (not dump_) return;
    for(auto &b : bare)
    {
        b->defineBranches(tree_);	
    }
}

void Dumper::Fill(){
    if (not dump_) return;
    nbytes_+=tree_->Fill();

    if (nbytes_ > 1LL<<30) // 1G
    {
        Close();
        OpenNewFile();
    }
}

void Dumper::Close(){
    // if open close, and set pointer to 0
    if (out_) {
        out_->cd("nero");
        tree_->Write("",TObject::kOverwrite);
        out_->Write(); 
        out_->Close(); 
        Log(__FUNCTION__,"INFO","Dumper is closing files");
    }
    ChargedHiggs::Delete(out_);
    //ChargedHiggs::Delete(tree_);
}

void Dumper::TriggerNames( const vector<string>& triggerNames )
{
    out_->cd("nero");
    string tn="";
    for (unsigned i=0;i<triggerNames.size();++i)
    {
        if (i!=0) tn+=",";

        tn += triggerNames[i]; 
    }
    TNamed n("triggerNames",tn.c_str());
    n.Write("",TObject::kOverwrite);
    return;
}

void Dumper::OpenNewFile()
{
    if (not dump_) return;
    Close();
    //
    nbytes_= 0; //reset number of bytes written
    // Open the new file
    string dname=outdirname_ +"/"+ cacheMC_  ;

    if ( dname[0] == '/' or dname.find("root://") == string::npos) system( ("mkdir -p " + dname).c_str() );


    string fname=outdirname_ +"/"+ cacheMC_ + "/" + Form(basename_.c_str() ,0) ;
    for(int i=0;i<10000;++i)
    {
        fname=outdirname_ +"/"+ cacheMC_ + "/" + Form(basename_.c_str() ,i) ;
        //* check if file exists, and if not break
        TFile *tmp = TFile::Open(fname.c_str());
        if (tmp == NULL){
            break; 
            }
        else{
            tmp->Close();
        }
    }

    Log(__FUNCTION__,"INFO","Opening Dump File:" + fname);
    
    out_=TFile::Open(fname.c_str(), "RECREATE");
    out_->mkdir("nero");
    out_->cd("nero");
    tree_=new TTree("events","events");
    //this is not working with subdirs
    //tree_->SetMaxTreeSize( 10ULL<<30 ); //10Gb
    //TTree::SetMaxTreeSize( 1LL<<30 ); //1Gb
    //copy TriggerNames

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
