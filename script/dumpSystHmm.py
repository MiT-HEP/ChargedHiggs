import os,sys
from optparse import OptionParser
import ROOT
from array import array
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="Hmm.root")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--outname",type='string',help="OutSystName [Default=%default]", default="cms_pu")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmmWithTTH")
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
from hmm import *

#config=hmmAutoCat
config= eval(opts.hmm)
config.Print()

fIn=ROOT.TFile.Open(opts.input)
if fIn==None:
    print "[ERROR] File", opts.input, "doesn't exist"
    raise IOError

## get nominal histogram
base="HmumuAnalysis/Vars/Mmm"


categories=config.categories
#for muStr in ["BB","BO","BE","OO","OE","EE"]:
#    for catStr in [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
#        categories . append(catStr +"_" + muStr) 
#processes=['BKG','GluGlu','VBF','WPlusH','WMinusH','ttH','ZH']
processes= config.datacard_procs

binline  = ["bin"]
procline = ["process"]
procline2= ["process"]

systsline=[opts.outname,"lnN"]

def appendToLine(l,valDown,valUp):
    if abs(valDown-1) <0.01 and abs(valUp-1) <0.01:
        l.append("-")
    else:
        l.append("%.3f/%.3f"%(valDown,valUp))

empty=ROOT.TH1D("empty","empty",1000,0,1000)

for idx,cat in enumerate(categories):
   for pi,proc in enumerate(processes):
      name= base +"_"+ cat + "_" + proc +"_" + "HToMuMu_M125"
      h=fIn.Get(name)
      if h==None and proc == "WH":
        name= base +"_"+ cat + "_WPlusH_" + "HToMuMu_M125"
        h=fIn.Get(name)
        name= base +"_"+ cat + "_WMinusH_" + "HToMuMu_M125"
        hTmp=fIn.Get(name)
        h.Add(hTmp)
        name= base +"_"+ cat + "_" + proc +"_" + "HToMuMu_M125"

      binline . append( "cat%d"%idx)
      procline. append( proc )
      if proc == "BKG":
          systsline .append( "-" )
          continue
      elif h == None:
          print >> sys.stderr, "[ERROR] Hist", name, "doesn't exist"
          systsline .append( "-")
          continue
          #if( cat =="cat16" and (proc =="VBF" or proc=="GluGlu")) : 
          #      continue ## ok for this
          #raise IOError
      elif opts.syst!= 'Scale' and opts.syst != 'Pdf':
          hUp=fIn.Get(name+"_"+opts.syst+"Up")
          hDown=fIn.Get(name+"_"+opts.syst+"Down")
          if hUp == None and hDown == None and proc == "WH":
             name= base +"_"+ cat + "_WPlusH_" + "HToMuMu_M125" + "_" + opts.syst + "Up"
             hUp=fIn.Get(name)
             name= base +"_"+ cat + "_WPlusH_" + "HToMuMu_M125" + "_" + opts.syst + "Down"
             hDown=fIn.Get(name)
             ###
             name= base +"_"+ cat + "_WMinusH_" + "HToMuMu_M125" + "_" + opts.syst + "Up"
             hUp.Add(fIn.Get(name))
             name= base +"_"+ cat + "_WMinusH_" + "HToMuMu_M125" + "_" + opts.syst + "Down"
             hDown.Add(fIn.Get(name))
             name= base +"_"+ cat + "_" + proc +"_" + "HToMuMu_M125"

          if hUp==None or hDown==None:
            if hUp==None:print>>sys.stderr, "[ERROR] Hist", name+"_"+opts.syst+"Up", "doesn't exist"
            if hDown==None:print>>sys.stderr, "[ERROR] Hist", name+"_"+opts.syst+"Down", "doesn't exist"
            systsline .append( "-")
            continue
            #raise IOError
          if h.Integral() == 0:
            print>>sys.stderr, "[ERROR] Hist", name, "has zero integral"
            systsline .append( "-")
            continue
          appendToLine(systsline , hDown.Integral()/h.Integral(), hUp.Integral()/h.Integral())
      elif opts.syst=='Scale':
          #fully correlated
          low=1.000
          high=1.000
          for w in [ 'R','F','RF']:
              for s in ['Up','Down']:
                  hTmp=fIn.Get(name+"_Scale"+w+s)
                  if hTmp == None and proc == "WH":
                    name= base +"_"+ cat + "_WPlusH_" + "HToMuMu_M125" + "_Scale" + w+ s
                    hTmp = fIn.Get(name)
                    name= base +"_"+ cat + "_WMinusH_" + "HToMuMu_M125" + "_Scale" + w + s
                    hTmp.Add(fIn.Get(name))
                  if hTmp==None:
                      print "[ERROR] Hist", name+"_Scale"+w+s, "doesn't exist"
                      #systsline .append( "-")
                      continue
                      #raise IOError
                  sTmp= hTmp.Integral()/h.Integral()
                  if low> sTmp: low=sTmp
                  if high< sTmp: high=sTmp
          appendToLine(systsline,low,high)

      elif opts.syst=='Pdf':
          #fully correlated
          low=1.000
          high=1.000
          for i in range(0,100):
              hTmp=fIn.Get(name+"_Pdf%d"%i)
              if hTmp == None and proc == "WH":
                    name= base +"_"+ cat + "_WPlusH_" + "HToMuMu_M125" + "_Pdf%d"%i
                    hTmp = fIn.Get(name)
                    name= base +"_"+ cat + "_WMinusH_" + "HToMuMu_M125" + "_Pdf%d"%i
                    hTmp.Add(fIn.Get(name))
              if hTmp==None:
                   print "[ERROR] Hist",name+"_Pdf%d"%i, "doesn't exist"
                   #systsline .append( "-")
                   continue
                   #raise IOError
              sTmp= hTmp.Integral()/h.Integral()
              if low> sTmp: low=sTmp
              if high< sTmp: high=sTmp
          appendToLine(systsline,low,high)

print  ' ' .join(binline)
print  ' ' .join(procline)
print  ' ' .join(systsline)




