import os,sys
from array import array
import re
from optparse import OptionParser, OptionGroup

parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
parser.add_option("-d","--dir",dest='dir',type="string",help="directory [%default]",default="Vars")
parser.add_option("-v","--var",dest='var',type="string",help="variable [%default]",default="Mmm")
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="Hmumu")

#extra = OptionGroup(parser,"Extra options:","")
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=1)
opts,args= parser.parse_args()

########### IMPORT ROOT #############
sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
if opts.outdir != "":
	ROOT.gROOT.SetBatch()

print "inserting in path cwd/script"
sys.path.insert(0,os.getcwd()+'/script')
print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

############# definitions
categories=[]
for m in [ "BB","BO","BE","OO","OE","EE" ]:
   for v in ["VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
      categories.append(v + "_" + m )

montecarlos= ["VBF_HToMuMu_M%d","GluGlu_HToMuMu_M%d","ZH_HToMuMu_M%d","WMinusH_HToMuMu_M%d","WPlusH_HToMuMu_M%d","ttH_HToMuMu_M%d"]

masses = [125,120,130]

###############################

def MpvAndSigmaEff(h, q=0.68):
    ''' Return mpv and sigma eff'''
    imax=-1
    valmax=0.0
    for ibin in range(0,h.GetNbinsX()):
        if h.GetBinContent(ibin+1) > valmax:
            valmax=h.GetBinContent(ibin+1) 
            imax= ibin+1
    s=h.Integral()

    low=h.GetBinCenter(1)
    high=h.GetBinCenter(h.GetNbinsX())

    for ibin in range(0,h.GetNbinsX()):
        for jbin in range(ibin+1,h.GetNbinsX()):
            if h.Integral(ibin+1,jbin+1)> q *s:
                if h.GetBinCenter(jbin+1)-h.GetBinCenter(ibin+1) < high -low:
                    low = h.GetBinCenter(ibin+1)
                    high=h.GetBinCenter(jbin+1)
                #break ## j -loop can end here
    return (h.GetBinCenter(imax), low, high )

def SoB(h,hdata,low,high,type=""):
	## xsec at 125
	## these numbers are in pb
	br = 2.176E-04
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

	hdata_ = hdata.Clone(hdata.GetName()+"_fit")
	s= h.Integral(h.FindBin(low),h.FindBin(high))
	#f=ROOT.TF1("myfunc","[0]*TMath::Exp(-x*[1])",115,122)
	f=ROOT.TF1("myfunc","[0]*TMath::Exp(-x*[1])",115,122)
	f.SetParameter(0,hdata.Integral( hdata.FindBin(115),hdata.FindBin(122)))
	hdata.Fit("myfunc","QRN")
	hdata.Fit("myfunc","QRMN")
	## fill an histogram to be sure that border conditions are well met
	for ibin in range(0,hdata_.GetNbinsX()):
		if hdata_.GetBinContent(ibin+1)<=0:
			hdata_.SetBinContent( ibin+1,f.Eval(hdata_.GetBinCenter(ibin+1)) ) 
	b= hdata_.Integral(h.FindBin(low),h.FindBin(high))
	if opts.outdir=="":
		c2=ROOT.TCanvas("c2","c2",800,800)
		hdata.Draw("PE")
		f.Draw("L SAME")
		raw_input("fit to data")
	#b= f.Integral(mean-low,mean+high)
	return (s*br*lumi*xsec,b)


## get files
fIn = ROOT.TFile.Open(opts.input,"READ")
if fIn == None: 
	   print "<*> NO File '%s'"%opts.input
	   raise IOError

canvases=[]
garbage=[]
for cat in categories:
    for mc in montecarlos:
        c=ROOT.TCanvas("c_"+cat+"_"+mc,"canvas",800,800)
        c.SetTopMargin(0.05)
        c.SetRightMargin(0.05)
        c.SetBottomMargin(0.15)
        c.SetLeftMargin(0.15)
        mpv=0
        seff=0.0
        ea=0.0
	sob=0.0
        hdata=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_Data" )
        for idx,m in enumerate(masses):
            h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_"+mc%(m) )
	    if h==None:
		    print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_"+mc%(m) 
		    continue
            garbage.append(h)
            h.SetLineColor(ROOT.kBlack)
            h.SetLineWidth(2)
            if idx==0:
		c.cd()
                h.Draw("AXIS")
                h.Draw("AXIS X+ Y+ SAME")
                h.GetXaxis().SetTitle("m^{#mu#mu}[GeV]")
                h.GetXaxis().SetTitleOffset(1.2)
                h.GetYaxis().SetTitle("#varepsilon A")
                h.GetYaxis().SetTitleOffset(1.2)
                h.GetXaxis().SetRangeUser(100,150)
		#color=38
		color=46
                h.SetLineColor(color)
                h.SetLineWidth(2)
                h.SetFillColor(color)
                h.SetFillStyle(3004)
                ea = h.Integral()
                mpv, low ,high =  MpvAndSigmaEff(h, 0.682689)
                seff = (high - low)/2.0

		if hdata !=None:
			s,b=SoB(h,hdata,low,high,re.sub("_HToMuMu.*","",mc))
			c.cd()
		else : 
			s,b = (0,0)

		try:
			sob=s/b
		except ZeroDivisionError:
			sob=-1

                l1 = ROOT.TLine(low,0,low,h.GetMaximum()*0.8)
                l2 = ROOT.TLine(high,0,high,h.GetMaximum()*0.8)
                l1.SetLineWidth(2)
                l2.SetLineWidth(2)
                l1.SetLineColor(ROOT.kGray)
                l2.SetLineColor(ROOT.kGray)
                l1.Draw("L SAME")
                l2.Draw("L SAME")
                garbage.extend([l1,l2])
            h.Draw("HIST SAME")
        txt=ROOT.TLatex()
        txt.SetNDC()
        txt.SetTextFont(43)
        txt.SetTextSize(20)
        txt.SetTextAlign(31)
        txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(35867/1000.)))
        txt.SetTextSize(30)
        txt.SetTextAlign(13)
        txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
        txt.SetTextSize(20)
        txt.SetTextAlign(11)
        d=0.04
        txt.DrawLatex(.73,.90 - 0*d,"#bf{Cat="+cat+"}")
        txt.DrawLatex(.73,.90 - 1*d,"Proc="+re.sub("_HToMuMu.*","",mc))
        txt.DrawLatex(.73,.90 - 2*d,"mpv=%.1f GeV"%mpv)
        txt.DrawLatex(.73,.90 - 3*d,"seff=%.1f GeV"%seff)
        txt.DrawLatex(.73,.90 - 4*d,"#varepsilon A=%.1f %%"%(ea*100))
        txt.DrawLatex(.73,.90 - 5*d,"S/B = %.1f %%"%(sob*100))
	c.Modify()
	c.Update()
	if opts.outdir=="":
		raw_input("ok?")
	else:
		c.SaveAs(opts.outdir + "/" + cat + "_" + re.sub("_HToMuMu.*","",mc) + ".pdf")
		c.SaveAs(opts.outdir + "/" + cat + "_" + re.sub("_HToMuMu.*","",mc) + ".png")

#Local Variables:
#mode:python
#indent-tabs-mode:nil
#tab-width:4
#c-basic-offset:4
#End:
#vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
