import os,sys
from array import array
import re,math
from subprocess import call
from optparse import OptionParser, OptionGroup

parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
parser.add_option("-d","--dir",dest='dir',type="string",help="directory [%default]",default="Vars")
parser.add_option("-v","--var",dest='var',type="string",help="variable [%default]",default="Mmm")
parser.add_option("-c","--cat",dest='cat',type="string",help="do cat xxx for bkg [%default]",default="all")
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="Hmumu")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")

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
from hmm import hmm,hmmAutoCat, Stack
systs=['JES','PU']
#systs=['JES']

#extra = OptionGroup(parser,"Extra options:","")
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=1)
opts,args= parser.parse_args()

########### IMPORT ROOT #############
oldArgv=sys.argv[:]
sys.argv=[]
import ROOT

sys.argv=oldArgv[:]

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
if opts.outdir != "":
	ROOT.gROOT.SetBatch()


############# definitions
config= eval(opts.hmm)
config.Print()
categories=config.categories
##for m in [ "BB","BO","BE","OO","OE","EE" ]:
##   for v in ["VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
##      categories.append(v + "_" + m )

#sigMonteCarlos= ["VBF_HToMuMu_M%d","GluGlu_HToMuMu_M%d","ZH_HToMuMu_M%d","WMinusH_HToMuMu_M%d","WPlusH_HToMuMu_M%d","ttH_HToMuMu_M%d"]
sigMonteCarlos = []
for proc in config.processes:
	sigMonteCarlos .append( proc+"_HToMuMu_M%d" )

## 125 first
masses = [125,120,130]

###############################

## get files
fIn = ROOT.TFile.Open(opts.input,"READ")
if fIn == None: 
    print "<*> NO File '%s'"%opts.input
    raise IOError

canvases=[]
garbage=[]



rebin=1
c=ROOT.TCanvas("c_roc","canvas",800,800)
c.SetTopMargin(0.05)
c.SetLeftMargin(0.15)
c.SetRightMargin(0.05)
c.SetBottomMargin(0.15)
garbage.extend([c])
rocs=[]
leg = ROOT.TLegend(.15,.15,.55,.50)
leg.SetFillStyle(0)
leg.SetBorderSize(0)
for idx,var in enumerate(opts.var.split(',')):
    cat ='_'+opts.cat
    if opts.cat == "": cat = ""
    #tmp.cd()

    print "-> Getting data hist","HmumuAnalysis/"+opts.dir+"/" + var +  cat +"_Data"
    hdata=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + var + cat +"_Data" )
    hdata.Rebin(rebin)
    hdata.SetMarkerStyle(20)
    hdata.SetMarkerColor(ROOT.kBlack)
    hdata.SetLineColor(ROOT.kBlack)
    BkgMonteCarlos=["ZZ","WW","WZ","ST","TT","DY","EWK_LLJJ"]
    bkg=None

    #sig=ROOT.THStack()
    sig=None
    garbage.extend([sig,bkg])

    for mc in BkgMonteCarlos:
        print "* Getting bkg","HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc
        h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc )
        if h==None:
            print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc
            continue
        garbage.append(h)
        h.Rebin(rebin)
        h.Scale(config.lumi())
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(2)
        if bkg == None:
            bkg = h.Clone("bkg_"+var+"_"+cat)
        else:
            bkg.Add(h)

    ##end bkg loop
    for mc in reversed(sigMonteCarlos):
        m=125
        print "* Getting sig","HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc%(m) 
        try:
               h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc%(m) )
        except:
            h=None
        if h==None: 
            print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + var + cat +"_"+mc%(m)
            continue
        garbage.append(h)
        h.Rebin(rebin)
        br = config.br()
        if 'GluGlu' in mc:
            xsec=config.xsec("ggH")
        elif 'VBF' in mc:
            xsec=config.xsec("VBF")
        elif 'WMinusH' in mc: 
            xsec=config.xsec("WPlusH")
        elif 'WPlusH' in mc:
            xsec=config.xsec("WMinusH")
        elif 'ZH' in mc:
            xsec=config.xsec("ZH")
        elif 'ttH' in mc:
            xsec=config.xsec("ttH")

        h.Scale(config.lumi())
        h.Scale(xsec*br)
        if sig == None:
            sig = h.Clone("sig"+cat)
        else:
            sig.Add(h)

    roc =ROOT.TGraph();
    roc.SetName("roc_"+var)
    sig.Scale(1./sig.Integral());
    bkg.Scale(1./bkg.Integral());
    for i in range(0,sig.GetNbinsX()):
        roc.SetPoint(roc.GetN(),1.-sig.Integral(1,i+1),bkg.Integral(1,i+1))
    colors=[38,46,ROOT.kGray,ROOT.kBlack,ROOT.kGreen+2,ROOT.kOrange,ROOT.kMagenta]
    roc.SetLineColor(colors[idx])
    rocs.append(roc)
    leg.AddEntry(roc,var,"L")
    ## end mc loop
c.cd()
dummy = ROOT.TH2D("dummy","dummy",100,0,1,100,0,1)
dummy.Draw("AXIS")
dummy.GetXaxis().SetTitle("Sig. Eff.")
dummy.GetXaxis().SetTitleOffset(2.0)
dummy.GetYaxis().SetTitle("Bkg. Rej.")
dummy.GetYaxis().SetTitleOffset(2.0)
dummy.GetXaxis().SetRangeUser( 0,1)
dummy.GetYaxis().SetRangeUser(0,1)

dummy.GetYaxis().SetLabelFont(43)
dummy.GetXaxis().SetLabelFont(43)
dummy.GetYaxis().SetTitleFont(43)
dummy.GetXaxis().SetTitleFont(43)
dummy.GetYaxis().SetLabelSize(20)
dummy.GetXaxis().SetLabelSize(20)
dummy.GetYaxis().SetTitleSize(24)
dummy.GetXaxis().SetTitleSize(24)

for roc in rocs:
    roc.Draw("L SAME")

dummy.Draw("AXIS X+ Y+ SAME")
dummy.Draw("AXIS SAME")

leg.Draw("SAME")

txt=ROOT.TLatex()
txt.SetNDC()
txt.SetTextFont(43)
txt.SetTextSize(20)
txt.SetTextAlign(31)
txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
txt.SetTextSize(30)
txt.SetTextAlign(13)
txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Simulation Preliminary}}")
txt.SetTextSize(20)
txt.SetTextAlign(11)
txt.DrawLatex(.73,.90,"#bf{Cat="+re.sub('_','',cat)+"}")

c.Modify()
c.Update()

if opts.outdir=="":
    raw_input("ok?")
else:
    if cat != "":
        c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + "_roc" + ".pdf")
        c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + "_roc" + ".png")
    else:
        c.SaveAs(opts.outdir + "/" + re.sub(',','_',opts.var) + "_roc" + ".pdf")
        c.SaveAs(opts.outdir + "/" + re.sub(',','_',opts.var) + "_roc" + ".png")
#try to clean
for g in garbage:
    if g == None: continue
    g.Delete()
garbage=[]

fIn.Close()
fIn = ROOT.TFile.Open(opts.input,"READ")
if fIn == None: 
    print "<*> NO File '%s'"%opts.input
    raise IOError
    
#Local Variables:
#mode:python
#indent-tabs-mode:nil
#tab-width:4
#c-basic-offset:4
#End:
#vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
