import ROOT
from subprocess import call, check_output
import os,sys
import random
import numpy as np
import threading
import time

random.seed = os.urandom(10) #~10^14

####
datacard="/afs/cern.ch/user/a/amarini/public/forSagar/cat0.root"
workdir="/tmp/"+os.environ["USER"]
#func="zmod_cat14_ord1"
mass="125"
#zmod2_cat0_ord5

####

store={} # cat -> list of list
lock=threading.Lock()

def runToy(itoy=0):
    global store
    global lock
    seed=random.randint(0,999999)
    sub_dict={"datacard":datacard,"workdir":workdir,"toy":itoy,"mass":mass,"ntoys":"1","seed":seed}
    if itoy==-1: sub_dict["ntoys"]="-1"
    cmd="cd %(workdir)s && combine -M MultiDimFit -t %(ntoys)s --saveWorkspace -n Toy%(toy)d -s %(seed)d --rMin=-10 --rMax=10 -m %(mass)s  %(datacard)s"%sub_dict
    call(cmd,shell=True)
    with lock:
        fin=ROOT.TFile.Open("%(workdir)s/higgsCombineToy%(toy)s.MultiDimFit.mH%(mass)s.%(seed)d.root"%sub_dict)
        win=fin.Get("w")
        win.loadSnapshot("MultiDimFit")
        #f=w.pdf(func)
        sbpdf=win.pdf("model_s")
        bpdf=win.pdf("model_b")
        x = win.var("mmm")
        chan=win.cat("CMS_channel")
        for ich in range(0,chan.numTypes()):
            chan.setIndex(ich)
            print "-> Channel",chan.getLabel()
            bcatpdf=bpdf.getPdf(chan.getLabel())
            sbcatpdf=sbpdf.getPdf(chan.getLabel())
            sbevents=sbpdf.expectedEvents(ROOT.RooArgSet(x))
            bevents=bpdf.expectedEvents(ROOT.RooArgSet(x))
            sevents=sbevents-bevents
            hb = bcatpdf.createHistogram("hbtmp",x,ROOT.RooFit.Binning(3200))
            l=[]
            for ibin in range(2,hb.GetNbinsX()): #[2,N-1]
                k=-2*hb.GetBinContent(ibin)+hb.GetBinContent(ibin-1)+hb.GetBinContent(ibin+1)
                l.append(k)
            v=[]
            for ibin in range(0,hb.GetNbinsX()): #[0,N-1]
                v.append(hb.GetBinContent(ibin+1))
            d=[]
            for ibin in range(0,hb.GetNbinsX()-1): #[0,N-1]
                s=hb.GetBinContent(ibin+2)- hb.GetBinContent(ibin+1)
                d.append(s)
            
            key=chan.getLabel()+"_curv"
            if key  not in store: store[key]=[]
            store[key].append(l)
            key=chan.getLabel()+"_value"
            if key not in store: store[key]=[]
            store[key].append(v)
            key=chan.getLabel()+"_slope"
            if key not in store: store[key]=[]
            store[key].append(d)
    fin.Close()
    call("rm %(workdir)s/higgsCombineToy%(toy)s.MultiDimFit.mH%(mass)s.%(seed)d.root"%sub_dict,shell=True)
    return

if __name__=="__main__":
    runToy(-1)
    threads=[]
    ncore=10
    ntoys=100
    for itoy in range(0,ntoys):
        while threading.activeCount() > ncore : time.sleep(1)
        t=threading.Thread(target=runToy, args=(itoy,) )
        t.start()
        threads.append(t)
        #runToy(itoy)
    for t in threads: t.join()

    for key in store:
        nbins=len(store[key][0])
        k=ROOT.TH1D("k","k",nbins,0,nbins)
        #a=ROOT.TH1D("a","a",nbins,0,nbins)
        print " ----------------",key,"--------------"
        #print "V=",
        for ibin in range(0,nbins):
            #print "bin",ibin, "toys",len(store[key])-1
            #bin0=store[key][0][ibin] ## Asimov?
            #a.SetBinContent(ibin,bin0)
            #a.SetBinError(ibin,0)
            tmp=[]
            for itoy in range(1,len(store[key])):
                tmp.append(store[key][itoy][ibin])
            y=np.array(tmp)
            mean=np.mean(y)
            #rms=np.sqrt(np.mean(y**2)-np.mean(y))
            rms=np.std(y)
            k.SetBinContent(ibin,mean)
            k.SetBinError(ibin,rms)
            #print "(%f,%f)"%(mean,rms),
        k.SetTitle(key)
        k.SetMarkerStyle(4)
        k.SetMarkerColor(ROOT.kRed)
        k.SetLineColor(ROOT.kBlack)
        k.Draw("PE")
        #a.SetMarkerColor(ROOT.kBlue)
        #a.SetMarkerStyle(24)
        #a.Draw("P SAME")
        raw_input("ok?")

