import ROOT 
import os,sys

fIn=ROOT.TFile.Open("test/Hmumu/Hmumu_2017_03_10_moreSync2/Hmumu_2017_03_10_moreSync2.root")
Dir="HmumuAnalysis/Vars/"


br = 2.176E-04
type="ggH"

if type=="VBF" or type == "qqH" : xsec= 3.782E+00
elif type == "WH": xsec=1.373E+00
elif type == "ZH": xsec =8.839E-01
elif type == "ggH" or type=="GluGlu": xsec =4.858E+01
elif type == "ttH": xsec =5.071E-01
elif type == "bbH" : xsec=4.880E-01
elif type == "WMinusH": xsec = 5.328E-01
elif type == "WPlusH": xsec =8.400E-01
else: xsec=1.
lumi = 35867


for proc in ["Data","GluGlu_HToMuMu_M125"]:
   print "----------",proc,"----------"
   for catStr in [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
     ntries=0.
     S=0.
     for muStr in ["BB","BO","BE","OO","OE","EE"]:
   	  h=fIn.Get(Dir+"Mmm_" +catStr+"_"+ muStr + "_" + proc)
   	  if h==None:
   		  print "None Hist:",Dir+"Mmm_" +catStr+"_"+ muStr + "_GluGlu_HToMuMu_M125"
   	  ntries += h.GetEntries()
   	  S += h.Integral()
     print catStr+":", ntries,
     if proc=="Data": print ""
     else : print "|",S*xsec*br*lumi
