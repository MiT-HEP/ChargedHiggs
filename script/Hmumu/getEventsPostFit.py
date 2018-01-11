#!/usr/bin/env python
import os
import sys
import array 

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
parser.add_option("-d","--debug",dest="debug",default=False,action='store_true',help="Debug")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

def MpvAndSigmaEff(h, q=0.68):
    ''' Return mpv and sigma eff and FWHM'''
    imax=-1
    valmax=0.0
    for ibin in range(0,h.GetNbinsX()):
        if h.GetBinContent(ibin+1) > valmax:
            valmax=h.GetBinContent(ibin+1) 
            imax= ibin+1
    s=h.Integral()

    low=h.GetBinCenter(1)
    high=h.GetBinCenter(h.GetNbinsX())

    hm = h.GetMaximum()*0.5;
    hm_low = h.GetBinCenter(h.FindFirstBinAbove(hm));
    hm_high = h.GetBinCenter(h.FindLastBinAbove(hm));

    hm_low_bin=h.FindFirstBinAbove(hm)
    hm_high_bin=h.FindLastBinAbove(hm)
    delta=hm_high_bin-hm_low_bin

    ##FIXME FAST
    #for ibin in range(0,h.GetNbinsX()):
    #    for jbin in range(ibin+1,h.GetNbinsX()):
    xfact=0.5
    for ibin in range(int(hm_low_bin-delta*xfact),int(hm_low_bin+delta*xfact)):
        for jbin in range(int(hm_high_bin-delta*xfact),int(hm_high_bin+delta*xfact)):
            if h.Integral(ibin+1,jbin+1)> q *s:
                if h.GetBinCenter(jbin+1)-h.GetBinCenter(ibin+1) < high -low:
                    low = h.GetBinCenter(ibin+1)
                    high=h.GetBinCenter(jbin+1)
                #break ## j -loop can end here
    ## FWHM
    return (h.GetBinCenter(imax), low, high, hm_low, hm_high )

fIn = ROOT.TFile.Open(opts.file)
w = fIn.Get("w")
w.loadSnapshot("MultiDimFit")
#w.loadSnapshot("clean")
w.var("r").setVal(1)
w.var("MH").setVal(125)

datain = w . data ("data_obs")
sbpdf = w . pdf ("model_s")
bpdf = w . pdf ("model_b")
chan = w . cat("CMS_channel")
x = w . var("mmm")

for i in range(0,chan.numTypes() ):
    chan.setIndex(i)
    if opts.debug: print "-> Doing channel",i,chan.getLabel()
    bcatpdf = bpdf . getPdf( chan.getLabel() ) 
    sbcatpdf = sbpdf . getPdf( chan.getLabel() ) 
    sbevents = sbpdf . expectedEvents(ROOT.RooArgSet(x))
    bevents = bpdf . expectedEvents(ROOT.RooArgSet(x))
    sevents = sbevents - bevents
    if opts.debug: print "[INFO] number of s events",sevents,"b events",bevents,"sbevents",sbevents
    hsbtmp = sbcatpdf.createHistogram("hsbtmp_"+chan.getLabel(),x,ROOT.RooFit.Binning(3200))
    hbtmp = bcatpdf.createHistogram("hbtmp_"+chan.getLabel(),x,ROOT.RooFit.Binning(3200))
    #hstmp = sbevents * hsbtmp - bevents*hbtmp
    hstmp =  hsbtmp - hbtmp
    #hstmp = hsbtmp - hbtmp
    if opts.debug: print "[INFO] histogram integrals S",hstmp.Integral(),"B",hbtmp.Integral(),"SB",hsbtmp.Integral()
    info =  MpvAndSigmaEff(hstmp, q=0.68)
    mpv = info[0]
    low, high = info[3],info[4]
    rangename = "fwhm_range_"+ chan.getLabel()
    x . setRange(rangename, low,high)
    bdiff = bcatpdf.createIntegral(ROOT.RooArgSet(x),ROOT.RooArgSet(x),rangename).getVal()*bevents
    if opts.debug:print "[INFO] bkg under fwhm", bcatpdf.createIntegral(ROOT.RooArgSet(x),ROOT.RooArgSet(x),rangename).getVal(),"*",bevents,"=",bdiff

    sbdiff = sbcatpdf.createIntegral(ROOT.RooArgSet(x),ROOT.RooArgSet(x),rangename).getVal()*sbevents
    sdiff = sbdiff -bdiff
    if opts.debug: print "[INFO] sb under fwhm", sbdiff,"sevents",sevents

    if opts.debug:
        c= ROOT.TCanvas()
        c.Divide(2)
        c.cd(1)
        hstmp.SetLineColor(ROOT.kRed)
        hstmp.Draw("L")
        c.cd(2)

        hsbtmp.Draw("L")
        hsbtmp.SetLineColor(ROOT.kBlack)
        hstmp.Draw("L SAME")
        hbtmp.SetLineColor(ROOT.kBlue)
        hbtmp.Draw("L SAME")
        raw_input("ok?")

    #if i==0:
    #    print "[LINE]","channel","S","B","FWHM,(high,low),MPV"
    #print "[LINE]",chan.getLabel(),sdiff,bdiff,high-low,"(",high ,low,")",mpv

    #if i==0:
    #    print "[LINE]","channel","FWHM","S","B under FWHM"
    #print "[LINE]",chan.getLabel(),high-low,sdiff,bdiff
    if i==0:
        print "[LINE]","channel","FWHM","S","B under FWHM","Seff"
    print "[LINE]",chan.getLabel(),high-low,sdiff,bdiff,(info[2]-info[1])

