import ROOT
from optparse import OptionParser

usage = "%prog options HIST1 HIST2 .."
parser=OptionParser(usage=usage)
parser.add_option("-l","--luminosity",help="Luminosity scaling [%default]",type='float',default=1.)
parser.add_option("-i","--input",help="Input ROOT File [%default]",default="Input.root")
parser.add_option("-o","--output",help="Output ROOT File [%default]",default="Output.root")
parser.add_option("-r","--rebin",help="Rebin Factor [%default]",default=None)
parser.add_option("-n","--outname",help="Output HIST NAME [%default]",default="MVV_AllMC")
opts,args=parser.parse_args()

def Rebin(h,val):
    if val !=None and val !=1:
        h.Rebin(val)
    return h

def Scale(h,val):
    if val !=None:
        h.Scale(val)
    return h

fIn=ROOT.TFile.Open(opts.input)
fOut=ROOT.TFile.Open(opts.output,"RECREATE")

if fIn==None: 
    print "File",opts.input,"does not exist"
    raise RuntimeError()

hOut=None
for hname in args:
    h= fIn.Get(hname)
    if h==None: 
        print "HIST",hname,"does not exist"
        raise RuntimeError()
    h = Scale(Rebin(h,opts.rebin),opts.luminosity)
    if hOut==None: hOut=h.Clone(opts.outname)
    else: hOut.Add(h)
fOut.cd()
hOut.Write()
fOut.Close()
    


