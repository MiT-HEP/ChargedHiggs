import os,sys
from subprocess import call,check_output
from optparse import OptionParser


parser=OptionParser()
parser.add_option("-b","--background",type='int',help="N background [%default]",default=10)
opts,args=parser.parse_args()

sys.argv=[]
import ROOT
def PrepareWorkspace(outname):
    w=ROOT.RooWorkspace("w","w")
    print "-> Create x"
    x=w.factory("x[0,10]")
    print "-> Create bkg"
    b=w.factory("RooPolynomial::bkg(x,{a_0[-.01,-.1,5]})")
    print "-> Create sig"
    s=w.factory("RooGaussian::sig(x,m[5],s[.2])")

    print "-> Create norm bkg"
    b_norm=w.factory("bkg_norm[%d,0,%d]"%(opts.background,2*opts.background))
    #print "-> Create norm sig"
    #s_norm=w.factory("sig_norm[1.])")

    print "-> Create fake data"
    data_th1d=ROOT.TH1D("h","h",100,0,10)
    data_h= ROOT.RooDataHist("data_obs","data",ROOT.RooArgList(x), data_th1d)
    getattr(w,'import')(data_h,ROOT.RooCmdArg())

    w.Print("V")
    w.writeToFile(outname)

def PrepareDatacard(datname,fname):
    datacard = open(datname,"w")
    datacard.write("### automatically produced\n")
    datacard.write("----------------------\n")
    datacard.write("ibin *\n")
    datacard.write("jbin *\n")
    datacard.write("kbin *\n")
    datacard.write("----------------------\n")
    datacard.write("shapes * * %s w:$PROCESS\n"%(fname))
    datacard.write("----------------------\n")
    datacard.write("bin          ch0\n")
    datacard.write("observation  -1\n")
    datacard.write("----------------------\n")
    datacard.write("bin          ch0      ch0\n")
    datacard.write("process      sig      bkg\n")
    datacard.write("process        0        1\n")
    datacard.write("rate           1        1\n")
    datacard.write("----------------------\n")
    
    datacard.close()

def mycall(cmd):
    print "-> Calling cmd: '"+cmd+"'"
    status = call(cmd,shell=True)
    if status != 0:
        print "<*> ERROR: unable to execute '"+cmd+"'"
        raise IOError

if __name__=="__main__":
    wfile="workspace.root"
    datacard="datacard.txt"
    workspace="datacard.root"

    PrepareWorkspace(wfile)
    PrepareDatacard(datacard,wfile)
    mycall("text2workspace.py  -o %s %s"%(workspace,datacard))
    mycall("combine -M FitDiagnostics --expectSignal=0 -t 500 %s --rMin=-5 --rMax=10 -s -1 -S 0"%(workspace))

    fIn=ROOT.TFile.Open("fitDiagnostics.root")
    tIn=fIn.Get("tree_fit_sb")

    tIn.Draw("r/rErr")

    raw_input("ok?")
