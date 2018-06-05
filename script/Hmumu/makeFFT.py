#!/usr/bin/env python
import sys
from optparse import OptionParser
parser = OptionParser()

parser.add_option("-i","--input",default="",type="string",help="Input file [%default]")
parser.add_option("-o","--output",default="",type="string",help="Output file [%default]")


(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT
from array import array 
import math

fIn=ROOT.TFile.Open(opts.input)
fOut=ROOT.TFile.Open(opts.output,"RECREATE")

h=fIn.Get("HmumuAnalysis/Vars/Mmm_Data")
#mc=["DY","TT","ST"]
#hmc=None
#for mcstr in mc:
#    if hmc==None: 
#        hmc=fIn.Get("HmumuAnalysis/Vars/Mmm_"+mcstr)
#    else:
#        hmc_tmp=fIn.Get("HmumuAnalysis/Vars/Mmm_"+mcstr)
#        hmc.Add(hmc_tmp)

bin0=h.FindBin(110)
bin1=h.GetNbinsX()


#[bin0,bin1]
n = array('i',[bin1-bin0 + 1])  ## 
vfft=ROOT.TVirtualFFT.FFT(1,n,"C2CFORWARD");
#vfft.SetDefaultFFT("fftw3")
l_re=[]
l_zero=[]
for i in range(0,n[0]):
    l_re . append( h.GetBinContent(i+bin0))
    l_zero . append( 0. )
orig_re = array('d', l_re)
orig_im = array('d', l_zero)
vfft.SetPointsComplex(orig_re,orig_im);
vfft.Transform();

out_re = array('d',l_zero)
out_im = array('d',l_zero)
vfft.GetPointsComplex(out_re,out_im);

hOut_re=ROOT.TH1D("re","real part",len(out_re),1,len(out_re))
hOut_im=ROOT.TH1D("im","imal part",len(out_im),1,len(out_im))

for i in range(0,len(out_re)):
    hOut_re.SetBinContent(i+1, out_re[i])
    hOut_im.SetBinContent(i+1, out_im[i])

fOut.cd()
h.Clone("orig").Write()
hOut_re.Write()
hOut_im.Write()


## compute Inverso using a cut
vfft2=ROOT.TVirtualFFT.FFT(1,n,"C2CBACKWARD M K")
for i in range(0,len(orig_re)):
    if i > 40 and i <120 :  
        out_re[i] = 0.
        out_im[i] = 0.
vfft2.SetPointsComplex(out_re,out_im)
vfft2.Transform()
smooth_re = array('d',l_zero)
smooth_im = array('d',l_zero)
vfft2.GetPointsComplex(smooth_re,smooth_im);

hSmooth = h.Clone("smooth")
hSmooth.Reset("ACE")
for i in range(0, len(smooth_re)):
    hSmooth.SetBinContent(i +bin0,smooth_re[i]/n[0])

hSmooth.Write()
