import os,sys
from optparse import OptionParser
import ROOT
from array import array
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="Hmm.root")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--outname",type='string',help="OutSystName [Default=%default]", default="cms_pu")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)
opts, args = parser.parse_args()

#python script/drawSyst.py -i test/Hmumu/Hmumu_2017_03_07_JES_BTAG/Hmumu_2017_03_07_JES_BTAG.root --base=HmumuAnalysis/Vars/Mmm_OneB_BB_GluGlu_HToMuMu_M125 --syst=BTAGL

print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from hmm import hmm

fIn=ROOT.TFile.Open(opts.input)
if fIn==None:
    print "[ERROR] File", opts.input, "doesn't exist"
    raise IOError

## get nominal histogram
base="HmumuAnalysis/Vars/Mmm"


categories=hmm.categories
#for muStr in ["BB","BO","BE","OO","OE","EE"]:
#    for catStr in [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
#        categories . append(catStr +"_" + muStr) 
#processes=['BKG','GluGlu','VBF','WPlusH','WMinusH','ttH','ZH']
processes= hmm.datacard_procs

binline = ["bin"]
procline = ["process"]
procline2=["processes"]

systsline=[opts.outname,"lnN"]

def appendToLine(l,valDown,valUp):
    if abs(valDown-1) <0.001 and abs(valUp-1) <0.001:
	    l.append("-")
    else:
	    l.append("%.3f/%.3f"%(valDown,valUp))

for idx,cat in enumerate(categories):
   for pi,proc in enumerate(processes):
      name= base +"_"+ cat + "_" + proc +"_" + "HToMuMu_M125"
      h=fIn.Get(name)
      binline . append( "cat%d"%idx)
      procline. append( proc )
      if proc == "BKG":
	  systsline .append( "-" )
	  continue
      elif h == None:
          print >> sys.stderr, "[ERROR] Hist", name, "doesn't exist"
          raise IOError
      elif opts.syst!= 'Scale' and opts.syst != 'Pdf':
          hUp=fIn.Get(name+"_"+opts.syst+"Up")
          hDown=fIn.Get(name+"_"+opts.syst+"Down")
          if hUp==None or hDown==None:
            if hUp==None:print>>sys.stderr, "[ERROR] Hist", name+"_"+opts.syst+"Up", "doesn't exist"
            if hDown==None:print>>sys.stderr, "[ERROR] Hist", name+"_"+opts.syst+"Down", "doesn't exist"
	    systsline .append( "-")
            continue
            #raise IOError
          appendToLine(systsline , hDown.Integral()/h.Integral(), hUp.Integral()/h.Integral())
      elif opts.syst=='Scale':
          #fully correlated
          low=1.000
          high=1.000
          for w in [ 'R','F','RF']:
              for s in ['Up','Down']:
                  hTmp=fIn.Get(name+"_Scale"+w+s)
                  if hTmp==None:
                      print "[ERROR] Hist", name+"_Scale"+w+s, "doesn't exist"
                      raise IOError
                  sTmp= hTmp.Integral()/h.Integral()
                  if low< sTmp: low=sTmp
                  if high< sTmp: high=sTmp
          appendToLine(systsline,low,high)

      elif opts.syst=='Pdf':
          #fully correlated
          low=1.000
          high=1.000
          for i in range(0,100):
              hTmp=fIn.Get(name+"_Pdf%d"%i)
              if hTmp==None:
                   print "[ERROR] Hist",name+"_Pdf%d"%i, "doesn't exist"
                   raise IOError
              sTmp= hTmp.Integral()/h.Integral()
              if low< sTmp: low=sTmp
              if high< sTmp: high=sTmp
          appendToLine(systsline,low,high)

print  ' ' .join(binline)
print  ' ' .join(procline)
print  ' ' .join(systsline)




