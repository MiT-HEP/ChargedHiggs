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
parser.add_option("","--noSig",dest='noSig',action="store_true",help="don't do sig plots [%default]",default=False)
parser.add_option("","--noBkg",dest='noBkg',action="store_true",help="don't do bkg plots [%default]",default=False)
parser.add_option("","--doRemap",dest='doRemap',action="store_true",help="remap bkg plots in cumulative sig [%default]",default=False)
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="Hmumu")
parser.add_option("-x","--xrange",dest='xrange',type="string",help="xrange [%default]",default="60,150")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")
parser.add_option("","--noRatio",dest='noRatio',action="store_true",help="don't plot ratio in bkg plots [%default]",default=False)
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("","--unblind",dest="blind",default=True,action="store_false",help="Unblinded plots")


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
systs=['JES','PU']
#systs=['JES','PU','BTAGL','BTAGB']
#systs=['JES']

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=10)
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

if opts.cat != "all" and opts.cat != "":
    categories= [opts.cat]

sigMonteCarlos = []
for proc in config.processes:
	sigMonteCarlos .append( proc+"_HToMuMu_M%d" )

## 125 first
masses = [125,120,130]

###############################

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

    ## FIXME FAST
    ### for ibin in range(0,h.GetNbinsX()):
    ###     for jbin in range(ibin+1,h.GetNbinsX()):
    ###         if h.Integral(ibin+1,jbin+1)> q *s:
    ###             if h.GetBinCenter(jbin+1)-h.GetBinCenter(ibin+1) < high -low:
    ###                 low = h.GetBinCenter(ibin+1)
    ###                 high=h.GetBinCenter(jbin+1)
    ###             #break ## j -loop can end here

    ## FWHM
    hm = h.GetMaximum()*0.5;
    hm_low = h.GetBinCenter(h.FindFirstBinAbove(hm));
    hm_high = h.GetBinCenter(h.FindLastBinAbove(hm));

    return (h.GetBinCenter(imax), low, high, hm_high-hm_low )

def SoB(h,hdata,low,high,type=""):
	## xsec at 125
	## these numbers are in pb
	br=config.br();
	#br = 2.176E-04
	xsec=config.xsec(type)
	lumi = config.lumi()

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

doSig=not opts.noSig
sigYields={} ##(cat,proc) -> Nevents, for next sig composition plot
eaStore={} ##(cat,proc,MH) -> ea
if doSig:
  for cat in categories:
    for mc in sigMonteCarlos:
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
        if hdata==None:
        	print "<!> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_Data"
        	continue
        for idx,m in enumerate(masses):
            h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_"+mc%(m) )
            if h==None:
                print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + opts.var + "_" + cat +"_"+mc%(m) 
                continue
            garbage.append(h)
            h.SetLineColor(ROOT.kBlack)
            h.SetLineWidth(2)

            proc =re.sub("_HToMuMu.*","",mc) 
            bin0 = h.FindBin(110)
            bin1 = h.FindBin(150)
            ea = h.Integral(bin0,bin1)
            dea=array('d',[0.])
            #ea = h.Integral()
            #h.IntegralAndError(1,h.GetNbinsX(),dea)
            h.IntegralAndError(bin0,bin1,dea)
            eaStore[(cat,proc,int(m))]= (ea,dea[0])

            if idx==0:
                c.cd()
                h.Draw("AXIS")
                #h.Draw("AXIS X+ Y+ SAME")
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
                mpv, low ,high,fwhm =  MpvAndSigmaEff(h, 0.682689)
                seff = (high - low)/2.0

                ## save for sig composition plot
                sigYields[(cat,proc)] = ea * config.lumi() * config.xsec(proc) *config.br() # proc+"_HToMuMu_M%d" 

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
        txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
        txt.SetTextSize(30)
        txt.SetTextAlign(13)
        if opts.paper:
            txt.DrawLatex(.16,.93,"#bf{CMS}")
        else:
            txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
        txt.SetTextSize(20)
        txt.SetTextAlign(11)
        d=0.04
        txt.DrawLatex(.73,.90 - 0*d,"#bf{Cat="+cat+"}")
        txt.DrawLatex(.73,.90 - 1*d,"Proc="+re.sub("_HToMuMu.*","",mc))
        txt.DrawLatex(.73,.90 - 2*d,"mpv=%.1f GeV"%mpv)
        txt.DrawLatex(.73,.90 - 3*d,"seff=%.1f GeV"%seff)
        try:
            txt.DrawLatex(.73,.90 - 4*d,"FWHM=%.1f GeV"%fwhm)
        except:
            pass
        txt.DrawLatex(.73,.90 - 5*d,"#varepsilon A=%.1f %%"%(ea*100))
        txt.DrawLatex(.73,.90 - 6*d,"S/B = %.1f %%"%(sob*100))
        c.Modify()
        c.Update()
        if opts.outdir=="":
        	raw_input("ok?")
        else:
            ## FIXME FAST
            pass
            c.SaveAs(opts.outdir + "/" + cat + "_" + re.sub("_HToMuMu.*","",mc) + ".pdf")
            c.SaveAs(opts.outdir + "/" + cat + "_" + re.sub("_HToMuMu.*","",mc) + ".png")


if doSig: ## signal composition plot
    #colors = [ ROOT.kGreen+3, ROOT.kRed+2, ROOT.kCyan+2, ROOT.kAzure-6,ROOT.kOrange+7,ROOT.kBlue-4];
    colors = [ ROOT.kBlue, 8, ROOT.kCyan+2, ROOT.kAzure-6,ROOT.kOrange+7,ROOT.kMagenta];
    c0=ROOT.TCanvas("c_"+cat+"_sig_composition","canvas",1600,800)
    ## old -> new

    pad0 = ROOT.TPad("p0","P0",0,0,.5,1)
    pad0.Range(-14.67532,-1.75,11.2987,15.75);
    pad0.SetFillColor(0);
    pad0.SetBorderMode(0);
    pad0.SetBorderSize(2);
    pad0.SetTopMargin(0.16);
    #pad0.SetLeftMargin(0.25);
    pad0.SetLeftMargin(0.15);
    pad0.SetRightMargin(0.05);
    pad0.SetFrameBorderMode(0);
    pad0.SetFrameBorderMode(0);
    pad0.Draw()
    pad0.cd()

    nCats=len(categories)
    dummy = ROOT.TH2F("dummy","",10,0.,100.,nCats,1-0.5,nCats+0.5);
    dummy.SetStats(0);
    ci = ROOT.TColor.GetColor("#00ff00");
    dummy.SetFillColor(ci);

    for ic,cat in enumerate(categories):
        #dummy.GetYaxis().SetBinLabel(nCats-ic,cat);
        dummy.GetYaxis().SetBinLabel(nCats-ic,"cat %d"%ic);

    dummy.GetXaxis().SetTickLength(0.01);
    dummy.GetYaxis().SetTickLength(0);
    dummy.GetXaxis().SetTitle("Signal Fraction [%]");
    dummy.GetXaxis().SetNdivisions(510);
    dummy.GetXaxis().SetLabelFont(42);
    dummy.GetXaxis().SetLabelSize(0.045);
    dummy.GetXaxis().SetTitleSize(0.045);
    dummy.GetXaxis().SetTitleOffset(0.95);
    dummy.GetXaxis().SetTitleFont(42);
    dummy.GetYaxis().SetNdivisions(510);
    dummy.GetYaxis().SetLabelSize(0.035);
    dummy.GetYaxis().SetTitleSize(0.045);
    dummy.GetYaxis().SetTitleOffset(1.1);
    dummy.GetYaxis().SetTitleFont(42);
    dummy.GetZaxis().SetLabelFont(42);
    dummy.GetZaxis().SetLabelSize(0.035);
    dummy.GetZaxis().SetTitleSize(0.035);
    dummy.GetZaxis().SetTitleFont(42);
    dummy.Draw("");
    #sigYields[(cat,proc)] = ea * config.lumi() * config.xsec(proc) *config.br() # proc+"_HToMuMu_M%d" 

    ## normalized stacked fractions
    ymin = 0.0
    width = 0.34

    sigFrac = {}

    for ic,cat in enumerate(categories):
        S=0.0
        for proc in config.processes:
            try:
                S+=sigYields[(cat,proc)]
            except KeyError:
                sigYields[(cat,proc)] =0
                print "* Ignoring",(cat,proc)
        print " ** Cat=",cat,":",
        for proc in config.processes:
            try:
                sigFrac[(cat,proc)] = sigYields[(cat,proc)]/S*100.; #sigYields becomes fractions here!
            except ZeroDivisionError: 
                print "[WARNING] ZeroDivisionError in ",cat,proc
                sigFrac[(cat,proc)] = 0.
            print proc,"(",sigFrac[(cat,proc)],"%)",
        print
        sys.stdout.flush()
        #mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
        ybin = nCats-ic; ## 1-1 mapping
        #ybin= nCats - mCatToCat[ic]
        print "ybin for cat",cat,"is",ybin
        ybinmin = ybin-width;
        ybinmax = ybin+width;
        xbinmin = ymin;
        xbinmax = ymin;
        pavetext = ROOT.TPaveText(xbinmin+0.5,ybinmin,xbinmin+30.5,ybinmax);
        pavetext.AddText("%.1f total expected signal"%S)
        pavetext.SetTextColor(0);
        pavetext.SetFillStyle(0);
        pavetext.SetFillColor(0);
        pavetext.SetLineColor(0);
        pavetext.SetBorderSize(0);

        for iproc,proc in enumerate(config.processes):
            xbinmax += sigFrac[(cat,proc)]
            pave = ROOT.TPave(xbinmin,ybinmin,xbinmax,ybinmax);
            pave.SetFillColor(colors[iproc]);
            pave.Draw();
            pave.SetBorderSize(0);
            xbinmin +=sigFrac[(cat,proc)]
            garbage.append(pave)
        pavetext.Draw()
        garbage.append(pavetext)
    ## legend
    tex_m=ROOT.TLatex();
    tex_m.SetNDC();
    tex_m.SetTextAlign(12);
    tex_m.SetTextSize(0.025);
    tex_m.SetLineWidth(2);

    ## Legend
    processName=[]
    for iproc, proc in enumerate(config.processes):
       if proc == 'GluGlu': processName.append("ggH") 
       elif proc=='VBF' : processName.append("qqH")
       elif proc=='WMinusH' : processName.append("W^{-}H")
       elif proc=='WPlusH' : processName.append("W^{+}H")
       else: processName.append(proc)

    starts=[0.28,0.4,0.52,.64,.76,.88]
    
    for i in range(0,len(starts)):
        pave=ROOT.TPave(starts[i],0.85,starts[i]+.03,0.85+0.03,0,"NDC");
        pave.SetFillColor(colors[i]);
        pave.Draw();
        tex_m.DrawLatex(starts[i]+0.04,0.84+0.025,processName[i]);
        garbage.append(pave)

    pad0.Modify()
    pad0.Update()
    #if opts.outdir=="":
    #    raw_input("ok?")
    #else:
    #    extra=""
    #    if opts.cat !="all" and opts.cat != "": extra ="_"+cat
    #    c.SaveAs(opts.outdir + "/signal_composition"+extra+"_v2.pdf")
    #    c.SaveAs(opts.outdir + "/signal_composition"+extra+"_v2.png")

    #if doSig: ## second part of sig_composition plots
    store={}
    ## HARD CODED
    #cat FWHM S B 2*seff
    store[0]=(4.225,13.7011591071,13231.7157693	    ,4.6   )
    store[1]=(6.9625,7.94811874212,3045.8045022	    ,7.0875)
    store[2]=(7.6125,3.65736690377,1114.51171432	,7.175 )
    store[3]=(7.225,15.6108564484,9691.20230885	    ,6.925 )
    store[4]=(7.125,3.92614388493,737.111398075	    ,7.1625)
    store[5]=(2.8,9.55409583467,1342.2596781    	,2.9375)
    store[6]=(2.975,8.75569409378,958.643661642 	,3.0625)
    store[7]=(4.2125,13.6841024699,2160.16332991	,4.275 )
    store[8]=(4.0375,19.6326181152,3919.49608056	,4.1   )
    store[9]=(2.875,8.42564316187,2250.5907442	    ,2.9875)
    store[10]=(4.075,27.6634639092,9111.17905663	,4.1375)
    store[11]=(3.975,5.54844144425,382.397889426	,4.375 )
    store[12]=(4.2125,13.5373697551,1525.50287759	,4.3375)
    store[13]=(3.1625,9.32283638139,736.375775813	,3.1625)
    store[14]=(4.1375,8.96665127646,346.018640557	,4.4   )
    store[15]=(0.00,.00,1.00	,0.0   )
    store[16]=(0.00,.00,1.00	,0.0   )
    store[17]=(0.00,.00,1.00	,0.0   )
    store[18]=(0.00,.00,1.00	,0.0   )
    store[19]=(0.00,.00,1.00	,0.0   )
    #c=ROOT.TCanvas("c_"+cat+"_sig_composition 2","canvas",800,800)
    ## old -> new

    #c.Range(-14.67532,-1.75,11.2987,15.75);
    #c.SetFillColor(0);
    #c.SetBorderMode(0);
    #c.SetBorderSize(2);
    #c.SetTopMargin(0.16);
    #c.SetLeftMargin(0.25);
    #c.SetRightMargin(0.05);
    #c.SetFrameBorderMode(0);

    #pad1 = ROOT.TPad("p1","p1",0,0,.5,1)
    #pad2 = ROOT.TPad("p2","p2",0.5,0,1,1)
    c0.cd()
    pad1 = ROOT.TPad("p1","p1",0.5,0,.75,1)
    pad2 = ROOT.TPad("p2","p2",0.75,0,1,1)
    pad1.SetFillColor(0);
    pad1.SetBorderMode(0);
    pad1.SetBorderSize(2);
    pad1.SetTopMargin(0.16);
    pad1.SetLeftMargin(0.05);
    pad1.SetRightMargin(0.05);
    pad1.SetFrameBorderMode(0);

    pad2.SetFillColor(0);
    pad2.SetBorderMode(0);
    pad2.SetBorderSize(2);
    pad2.SetTopMargin(0.16);
    pad2.SetLeftMargin(0.05);
    pad2.SetRightMargin(0.05);
    pad2.SetFrameBorderMode(0);

    pad1.Draw()
    pad2.Draw()

    ci = ROOT.TColor.GetColor("#00ff00");

    nCats=len(categories)
    dummy1 = ROOT.TH2F("dummy1","",10,0.,10.,nCats,1-0.5,nCats+0.5);
    dummy1.SetStats(0);
    dummy1.SetFillColor(ci);

    dummy2 = ROOT.TH2F("dummy2","",10,0.,0.49,nCats,1-0.5,nCats+0.5);
    dummy2.SetStats(0);
    dummy2.SetFillColor(ci);

    for ic,cat in enumerate(categories):
        #dummy.GetYaxis().SetBinLabel(nCats-ic,cat);
        #dummy1.GetYaxis().SetBinLabel(nCats-ic,"cat %d"%ic);
        #dummy2.GetYaxis().SetBinLabel(nCats-ic,"cat %d"%ic);
        dummy1.GetYaxis().SetBinLabel(nCats-ic,"");
        dummy2.GetYaxis().SetBinLabel(nCats-ic,"");

    dummy1.GetXaxis().SetTitle("Width [GeV]");
    dummy2.GetXaxis().SetTitle("S/#sqrt{B} in FWHM");
    for d in [dummy1,dummy2]:
        d.GetXaxis().SetTickLength(0.01);
        d.GetYaxis().SetTickLength(0);
        d.GetXaxis().SetLabelFont(42);
        d.GetXaxis().SetLabelSize(0.045*2);
        d.GetXaxis().SetLabelOffset( -0.03);
        d.GetXaxis().SetTitleSize(0.045*2);
        d.GetXaxis().SetTitleOffset(0.95/2.);
        d.GetXaxis().SetTitleFont(42);
        d.GetYaxis().SetLabelSize(0.035*2);
        d.GetYaxis().SetTitleSize(0.045*2);
        d.GetYaxis().SetTitleOffset(1.1);
        d.GetYaxis().SetTitleFont(42);
        d.GetZaxis().SetLabelFont(42);
        d.GetZaxis().SetLabelSize(0.035*2);
        d.GetZaxis().SetTitleSize(0.035*2);
        d.GetZaxis().SetTitleFont(42);
        d.GetYaxis().SetNdivisions(510);
    dummy1.GetXaxis().SetNdivisions(510);
    dummy2.GetXaxis().SetNdivisions(505);

    ## normalized stacked fractions
    ymin = 0.0
    width = 0.34

    sigFrac = {}

    pad1.cd()
    dummy1.Draw()
    for ic,cat in enumerate(categories):
        S=0.0
        #mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
        ybin = nCats-ic; ## 1-1 mapping
        #ybin= nCats - mCatToCat[ic]
        print "ybin for cat",cat,"is",ybin
        ybinmin = ybin-width;
        #ybinmax = ybin+width;
        ybinmax = ybin;
        xbinmin = 0;
        xbinmax = store[ic][0]/2.;

        pave = ROOT.TPave(xbinmin,ybinmin,xbinmax,ybinmax);
        pave.SetFillColor(ROOT.kBlue+2);
        pave.Draw();
        pave.SetBorderSize(0);
        garbage.append(pave)

        ybinmin = ybin
        ybinmax = ybin+width;
        xbinmin = 0;
        xbinmax = store[ic][3]/2.;

        pave = ROOT.TPave(xbinmin,ybinmin,xbinmax,ybinmax);
        pave.SetFillColor(ROOT.kRed+2);
        pave.Draw();
        pave.SetBorderSize(0);
        garbage.append(pave)

    tex_m=ROOT.TLatex();
    tex_m.SetNDC();
    tex_m.SetTextAlign(12);
    tex_m.SetTextSize(0.05);
    tex_m.SetLineWidth(2);
    starts=[0.28,0.5]
    labels=["#sigma_{HM}","#sigma_{eff}"]
    colors=[ROOT.kBlue+2,ROOT.kRed+2]
    for i in range(0,len(starts)):
        pave=ROOT.TPave(starts[i],0.85,starts[i]+.06,0.85+0.03,0,"NDC");
        pave.SetFillColor(colors[i]);
        pave.Draw();
        tex_m.DrawLatex(starts[i]+0.08,0.84+0.025,labels[i]);
        garbage.append(pave)

    pad2.cd()
    dummy2.Draw()
    for ic,cat in enumerate(categories):
        S=0.0
        #mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
        ybin = nCats-ic; ## 1-1 mapping
        #ybin= nCats - mCatToCat[ic]
        print "ybin for cat",cat,"is",ybin
        ybinmin = ybin-width;
        ybinmax = ybin+width;
        xbinmin = 0;
        #xbinmax = store[ic][1]/(store[ic][1]+store[ic][2]) *100.;
        xbinmax = store[ic][1]/(math.sqrt(store[ic][2]));

        pave = ROOT.TPave(xbinmin,ybinmin,xbinmax,ybinmax);
        pave.SetFillColor(46);
        pave.Draw();
        pave.SetBorderSize(0);
        garbage.append(pave)

    pad2.cd()
    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(30)
    txt.SetTextAlign(31)
    #txt.DrawLatex(.93,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
    txt.SetTextSize(48)
    txt.SetTextAlign(11)
    pad0.cd()
    if opts.paper:
        txt.DrawLatex(.01,.93,"#bf{CMS}")
    else:
        txt.DrawLatex(.01,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")

    c0.Modify()
    c0.Update()
    if opts.outdir=="":
        raw_input("ok?")
    else:
        extra=""
        if opts.cat !="all" and opts.cat != "": extra ="_"+cat
        c0.SaveAs(opts.outdir + "/signal_composition"+extra+"_v4.pdf")
        c0.SaveAs(opts.outdir + "/signal_composition"+extra+"_v4.png")


if doSig:## EA plot
    c=ROOT.TCanvas("c_"+cat+"_ea","canvas",800,800)
    ### TODO
    dummy = ROOT.TH2F("dummy","",10,120,130,100,0,1);
    dummy.SetStats(0);
    g={}
    g["tot"]=ROOT.TGraphErrors()
    g["tot"].SetName("ea tot")

    for proc in config.processes:
        g[proc] = ROOT.TGraphErrors()
        g[proc] . SetName("ea "+proc)

    for m in sorted(masses):
        Ntot=0.0
        Stot=0.0
        Etot=0.0
        for proc in config.processes:
            #print "--- proc",proc
            S=0.0
            E=0.0
            Ntot += (config.xsec(proc,m)*config.br(m) )
            for cat in categories:
                try:
                    ea,dea=eaStore[(cat,proc,m)]
                except KeyError:
                    ea,dea = 0.,0.
                #print "m",m,"ea",ea,"dea",dea,"xsec",config.xsec(proc,m),"br",config.br(m)
                S += ea
                E += (dea**2)
                Stot += (ea*config.xsec(proc,m)*config.br(m) )
                Etot += (dea*config.xsec(proc,m)*config.br(m))**2
            n=g[proc].GetN()
            g[proc].SetPoint(n,m,S)
            g[proc].SetPointError(n,m,math.sqrt(E))
        n=g['tot'].GetN() 
        #print "----------"
        #print "m=",m,"Stot=",Stot,"Ntot=",Ntot,"Etot",math.sqrt(Etot)
        #print "=========="
        g['tot'].SetPoint(n,m,Stot/Ntot)
        g['tot'].SetPointError(n,0.,math.sqrt(Etot)/Ntot)

    ## Set Style and draw
    c.SetTopMargin(0.05)
    c.SetRightMargin(0.05)
    c.SetBottomMargin(0.15)
    c.SetLeftMargin(0.15)

    dummy.Draw("AXIS")
    dummy.GetXaxis().SetTitle("m_{H} [GeV]")
    dummy.GetXaxis().SetTitleOffset(1.2)
    dummy.GetYaxis().SetTitle("#varepsilon A")
    dummy.GetYaxis().SetTitleOffset(1.2)
    dummy.GetYaxis().SetRangeUser(.25,.75)

    g["tot"].SetLineColor(ROOT.kBlack)
    g["tot"].SetLineWidth(2)
    g["tot"].SetFillColor(ROOT.kOrange)

    g["stat"]=g["tot"].Clone("stat")
    g["stat"].SetLineWidth(0)
    g["stat"].SetLineColor(ROOT.kOrange)

    x0=.5
    y0=.20
    leg = ROOT.TLegend(x0,y0,x0+.4,y0+.15)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetNColumns(2)
    leg.AddEntry(g['tot'],"SM H","L")
    leg.AddEntry(g['stat'],"stat. err","F")
    for proc in config.processes:
        g[proc].SetLineStyle(7)
        g[proc].SetLineWidth(2)
        g[proc].SetFillColor(ROOT.kGray)
        if 'GluGlu' in proc:
            g[proc].SetLineColor(38)
            leg.AddEntry(g[proc],"ggH","L")
        elif 'VBF' in proc:
            g[proc].SetLineColor(46)
            leg.AddEntry(g[proc],"qqH","L")
        elif 'WMinusH' in proc: 
            g[proc].SetLineColor(ROOT.kGreen+2)
            leg.AddEntry(g[proc],"W^{+}H","L")
        elif 'WPlusH' in proc:
            g[proc].SetLineColor(ROOT.kGreen+2)
            leg.AddEntry(g[proc],"W^{-}H","L")
        elif 'ZH' in proc:
            g[proc].SetLineColor(ROOT.kCyan)
            leg.AddEntry(g[proc],"ZH","L")
        elif 'ttH' in proc:
            g[proc].SetLineColor(ROOT.kOrange)
            leg.AddEntry(g[proc],"ttH","L")
        g[proc] . Draw("3 SAME")
        g[proc] . Draw("LX SAME")

    g['tot'] . Draw("3 SAME")
    g['tot'] . Draw("LX SAME")
    leg.Draw()

    dummy.Draw("AXIS SAME")
    #dummy.Draw("AXIS X+ Y+ SAME")
    c.Modify()
    c.Update()
    if opts.outdir=="":
    	raw_input("ok?")
    else:
    	c.SaveAs(opts.outdir + "/effAcc.pdf")
    	c.SaveAs(opts.outdir + "/effAcc.png")

if doSig:## EA plot inclusive only

    c=ROOT.TCanvas("c_"+cat+"_ea","canvas",800,800)
    ### 
    dummy = ROOT.TH2F("dummy","",10,120,130,100,0,1);
    dummy.SetStats(0);

    ## Set Style and draw
    c.SetTopMargin(0.05)
    c.SetRightMargin(0.05)
    c.SetBottomMargin(0.15)
    c.SetLeftMargin(0.15)

    dummy.Draw("AXIS")
    dummy.GetXaxis().SetTitle("m_{H} [GeV]")
    dummy.GetXaxis().SetTitleOffset(1.3)
    dummy.GetYaxis().SetTitle("#varepsilon A")
    dummy.GetYaxis().SetTitleOffset(1.5)
    dummy.GetYaxis().SetRangeUser(.26,.75)

    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    #txt.SetTextSize(20)
    #txt.SetTextAlign(31)
    #txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
    txt.SetTextSize(30)
    txt.SetTextAlign(13)
    if opts.paper:
        txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Simulation}}")
    else:
        txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Simulation Preliminary}}")

    x0=.5
    y0=.20
    leg = ROOT.TLegend(x0,y0,x0+.4,y0+.15)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetNColumns(2)
    leg.AddEntry(g['tot'],"SM H","L")
    leg.AddEntry(g['stat'],"stat. err","F")
    g['tot'] . Draw("3 SAME")
    g['tot'] . Draw("LX SAME")
    leg.Draw()

    dummy.Draw("AXIS SAME")
    #dummy.Draw("AXIS X+ Y+ SAME")
    c.Modify()
    c.Update()
    if opts.outdir=="":
    	raw_input("ok?")
    else:
    	c.SaveAs(opts.outdir + "/effAcc_incl.pdf")
    	c.SaveAs(opts.outdir + "/effAcc_incl.png")

rebin=opts.rebin
doBkg=not opts.noBkg
if doBkg:
  if opts.cat == "all":
    for cat in categories: 
        print "-> calling","python %s -i %s -d %s -v %s -c %s --noSig -o %s"% (sys.argv[0],opts.input,opts.dir,opts.var,cat,opts.outdir)
        call("python %s -i %s -d %s -v %s -c %s --noSig -o %s"% (sys.argv[0],opts.input,opts.dir,opts.var,cat,opts.outdir),shell=True)
  else:
    cat ='_'+opts.cat
    if opts.cat == "": cat = ""
    #tmp.cd()
    c=None
    pup=None
    if opts.noRatio:
        c=ROOT.TCanvas("c"+cat+"_bkg","canvas",800,800)
        pup=ROOT.TPad("up"+cat,"up",0,.0,1,1)
    else:
        c=ROOT.TCanvas("c"+cat+"_bkg","canvas",800,1000)
        pup=ROOT.TPad("up"+cat,"up",0,.2,1,1)

    pdown=ROOT.TPad("down"+cat,"up",0,0,1,.2)

    pup.SetTopMargin(0.05)
    pup.SetRightMargin(0.05)
    pup.SetLeftMargin(0.15)
    pup.SetBottomMargin(0)
    
    pdown.SetTopMargin(0.0)
    pdown.SetRightMargin(0.05)
    pdown.SetBottomMargin(0.50)
    pdown.SetLeftMargin(0.15)

    pup.Draw()

    if opts.noRatio:
        pup.SetBottomMargin(.15)
    else:
        pdown.Draw()

    garbage.extend([c,pup,pdown])

    leg = ROOT.TLegend(.68,.65,.92,.85)
    if opts.cat =="":
        leg = ROOT.TLegend(.68,.70,.92,.93)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    ## column in the right order
    leg1=[]
    leg2=[]

    print "-> Getting data hist","HmumuAnalysis/"+opts.dir+"/" + opts.var +  cat +"_Data"
    hdata=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_Data" )
    if hdata==None:print "[ERROR]","Unable to fetch data histogram"
    hdata.Rebin(rebin)
    hdata.SetMarkerStyle(20)
    hdata.SetMarkerColor(ROOT.kBlack)
    hdata.SetLineColor(ROOT.kBlack)
    leg.AddEntry(hdata,"Data","PE")
    #mcs=["DY","TT","ST","WZ","WW","ZZ"]
    #BkgMonteCarlos=["ZZ","WW","WZ","ST","TT","DY","EWK_LLJJ"]
    if opts.noRatio:
        BkgMonteCarlos=["ZZZ","WZZ","WWZ","WWW","TTW","TTZ","TTG","TTTT","ZZ","WW","WZ","ST","TT","DY"]
    else:
        BkgMonteCarlos=["ZZZ","WZZ","WWZ","WWW","TTW","TTZ","TTG","TTTT","ZZ","WW","WZ","ST","TT","DY","EWK_LLJJ"]
    #BkgMonteCarlos=["ZZZ","WZZ","WWZ","WWW","TTTT","ZZ","WW","WZ","ST","TT","DY","EWK_LLJJ"]
    mcAll=None

    #bkg=ROOT.THStack()
    bkg=Stack()
    bkg.SetName("bkgmc"+cat)

    b_systs_up=[ Stack() for x in systs]
    b_systs_down=[ Stack() for x in systs]
    for idx,s in enumerate(systs):
        b_systs_up[idx].SetName("bkgmc_"+s+"Up"+cat)
        b_systs_down[idx].SetName("bkgmc_"+s+"Down"+cat)

    #sig=ROOT.THStack()
    sig=Stack()
    sig.SetName("sigmc"+cat)

    garbage.extend([sig,bkg,leg])

    ## BLIND 120-130
    blind=opts.blind
    if opts.var != "Mmm":blind=False
    if blind:
        ibin0= hdata.FindBin(120)
        ibin1= hdata.FindBin(130)
        for ibin in range(ibin0,ibin1+1):
            hdata.SetBinContent(ibin,0)
            hdata.SetBinError(ibin,0)

    for mc in BkgMonteCarlos:
        print "* Getting bkg","HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc
        h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc )
        if h==None:
            print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc
            continue
        garbage.append(h)
        h.Rebin(rebin)
        h.Scale(config.lumi())
        for idx,s in enumerate(systs):
            hup=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc + "_" + s +"Up" )
            hdown=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc + "_" + s +"Down" )
            if hup==None or hdown == None:
                print "<*> Unable to find syst",s,"trying to ignore"
                continue
            hup.Rebin(rebin)
            hup.Scale(config.lumi())
            hdown.Rebin(rebin)
            hdown.Scale(config.lumi())
            b_systs_up[idx].Add(hup)
            b_systs_down[idx].Add(hdown)

        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(2)
        draw=True
        if mc == 'DY':
            h.SetFillColor(ROOT.kBlue-10)
            leg1.append( (h,"DY","F") )
            #leg.AddEntry(h,"DY","F")
        elif mc == 'TT':
            h.SetFillColor(ROOT.kRed-10)
            leg1.append( (h,"t#bar{t}+st","F"))
            #leg.AddEntry(h,"t#bar{t}+st","F")
        elif mc == 'ST':
            h.SetLineWidth(0)
            draw=False
            h.SetLineColor(ROOT.kRed-10)
            h.SetFillColor(ROOT.kRed-10)
        elif mc == 'WW' or mc == "ZZ":
            h.SetLineWidth(0)
            draw=False
            h.SetLineColor(ROOT.kGreen-10)
            h.SetFillColor(ROOT.kGreen-10)
        elif mc == 'WZ' :
            h.SetFillColor(ROOT.kGreen-10)
            leg1.append((h,"VV","F"))
        elif mc == 'EWK_LLJJ' :
            h.SetFillColor(ROOT.kOrange-4)
            leg1.append((h,"EWK_LLJJ","F"))
        elif mc == 'TTTT' :
            h.SetFillColor(ROOT.kGray)
            leg1.append((h,"TTX","F"))
        elif mc == 'TTW' or mc == 'TTZ' or mc =='TTG' :
            h.SetLineWidth(0)
            draw=False
            h.SetFillColor(ROOT.kGray)
            h.SetLineColor(ROOT.kGray)
        elif mc == 'WWZ' or mc == "ZZZ" or mc == "WZZ":
            h.SetLineWidth(0)
            draw=False
            h.SetLineColor(ROOT.kViolet-9)
            h.SetFillColor(ROOT.kViolet-9)
        elif mc == 'WWW' :
            h.SetFillColor(ROOT.kViolet-9)
            leg1.append((h,"VVV","F"))

        bkg.Add(h,draw)

        if mcAll==None:
            mcAll=h.Clone("mcAll"+cat)
        else:
            mcAll.Add(h)
    ##end bkg loop
    for mc in reversed(sigMonteCarlos):
        m=125
        print "* Getting sig","HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc%(m) 
        try:
               h=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc%(m) )
        except:
            h=None
        if h==None: 
            print "<*> Ignoring", "HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_"+mc%(m)
            continue
        garbage.append(h)
        h.Rebin(rebin)
        br = config.br()
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(3)
        h.SetFillStyle(0)
        draw=True
        if 'GluGlu' in mc:
            h.SetLineColor(38)
            xsec=config.xsec("ggH")
            #leg.AddEntry(h,"ggH","L")
            leg2.append((h,"ggH","L"))
        elif 'VBF' in mc:
            h.SetLineColor(46)
            h.SetLineStyle(2)
            #leg.AddEntry(h,"qqH","L")
            leg2.append((h,"qqH","L"))
            xsec=config.xsec("VBF")
        elif 'WMinusH' in mc: 
            h.SetLineColor(ROOT.kGreen+2)
            h.SetLineStyle(2)
            xsec=config.xsec("WPlusH")
            #leg.AddEntry(h,"W^{+}H","L")
            leg2.append((h,"W^{+}H","L"))
        elif 'WPlusH' in mc:
            h.SetLineColor(ROOT.kGreen+2)
            h.SetLineStyle(7)
            xsec=config.xsec("WMinusH")
            #leg.AddEntry(h,"W^{-}H","L")
            leg2.append((h,"W^{-}H","L"))
        elif 'ZH' in mc:
            h.SetLineColor(ROOT.kCyan)
            h.SetLineStyle(3)
            #leg.AddEntry(h,"ZH","L")
            leg2.append((h,"ZH","L"))
            xsec=config.xsec("ZH")
        elif 'ttH' in mc:
            h.SetLineColor(ROOT.kOrange)
            h.SetLineStyle(3)
            #leg.AddEntry(h,"ttH","L")
            leg2.append((h,"ttH","L"))
            xsec=config.xsec("ttH")

        h.Scale(config.lumi())
        h.Scale(xsec*br)
        sig.Add(h,draw)

    ## end mc loop
    lines=[]
    values=[]
    if 'BdtOnH' in opts.var:
        ymin,ymax=1.e-2,2e3
        if opts.noRatio:
            ymin=1e-1
        for x in [-.4,0.05,.25,.40,.65,.73]:
            l = ROOT.TLine(x,ymin,x,ymax)
            l.SetLineWidth(2)
            l.SetLineColor(ROOT.kGray+2)
            l.SetLineStyle(7)
            lines.append(l)
            #values.append(x)

    if opts.doRemap:
        qm = QuantileMapping()
        qm.nbins=25
        qm.SetBase(sig.GetHist() )
        sig.Remap(qm)
        bkg.Remap(qm)
        for x in b_systs_up: x.Remap(qm)
        for x in b_systs_down: x.Remap(qm)
        hdata = qm.Apply(hdata)
        mcAll = qm.Apply(mcAll)
        lines=[]
        values=[]
        ymin,ymax=1e-2, 1e4
        if opts.noRatio:
            ymin=1e-1
        if 'BdtOnH' in opts.var:
            for x0 in [-.4,0.05,.25,.40,.65,.73]:
                x=qm.ConvertPoint(x0)
                print "x=",x,"<- x0=",x0
                l = ROOT.TLine(x,ymin,x,ymax)
                l.SetLineWidth(2)
                l.SetLineColor(ROOT.kGray+2)
                #l.SetLineStyle(7)
                lines.append(l)
                if not opts.noRatio:
                    values.append(x)

    ##
    c.cd()
    pup.cd()
    #bkg.Draw("HIST") THSTack
    dummy = bkg.GetHist()
    dummy.Draw("AXIS")
    dummy.GetXaxis().SetTitle("m^{#mu#mu}[GeV]")

    if 'BdtOnH' in opts.var:
        dummy.GetXaxis().SetTitle("BDT Output")

    if 'BdtOnH' in opts.var and opts.doRemap:
        dummy.GetXaxis().SetTitle("transformed BDT")

    if 'Met' in opts.var:
        dummy.GetXaxis().SetTitle("E_{T}^{miss}[GeV]")
    elif 'NJets' in opts.var:
        dummy.GetXaxis().SetTitle("N_{jets}")
    elif 'NBJets' in opts.var:
        dummy.GetXaxis().SetTitle("N_{b jets}")
    elif 'EtaJet1' in opts.var:
        dummy.GetXaxis().SetTitle("#eta_{1 jet}")
    elif 'EtaJet2' in opts.var:
        dummy.GetXaxis().SetTitle("#eta_{2 jet}")
    elif 'PtJet1' in opts.var:
        dummy.GetXaxis().SetTitle("p_{T}^{1 jet} [GeV]")
    elif 'PtJet2' in opts.var:
        dummy.GetXaxis().SetTitle("p_{T}^{2 jet} [GeV]")
    elif 'Mjj' in opts.var:
        dummy.GetXaxis().SetTitle("m_{jj} [GeV]")
    elif 'PtOnH' in opts.var:
        dummy.GetXaxis().SetTitle("p_{T}^{#mu#mu} [GeV]")

    dummy.GetXaxis().SetTitleOffset(2.0)
    dummy.GetYaxis().SetTitle("Events")
    dummy.GetYaxis().SetTitleOffset(2.0)
    dummy.GetXaxis().SetRangeUser( float(opts.xrange.split(',')[0]),float(opts.xrange.split(',')[1]))
    if opts.noRatio:
        dummy.GetYaxis().SetRangeUser(1.05e-1,dummy.GetMaximum()*100)
    else:
        dummy.GetYaxis().SetRangeUser(1.05e-1,dummy.GetMaximum()*10)

    dummy.GetYaxis().SetLabelFont(43)
    dummy.GetXaxis().SetLabelFont(43)
    dummy.GetYaxis().SetTitleFont(43)
    dummy.GetXaxis().SetTitleFont(43)
    dummy.GetYaxis().SetLabelSize(20)
    dummy.GetXaxis().SetLabelSize(20)
    dummy.GetYaxis().SetTitleSize(24)
    dummy.GetXaxis().SetTitleSize(24)

    bkg.Draw("HIST SAME")
    #color=38
    sig.Draw("HIST SAME")
    sig.Print()

    for l in lines:
        l.Draw("L SAME")

    hdata.Draw("P E X0 SAME")

    #dummy.Draw("AXIS X+ Y+ SAME")
    dummy.Draw("AXIS SAME")

    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(20)
    txt.SetTextAlign(31)
    txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
    txt.SetTextSize(30)
    txt.SetTextAlign(13)
    if opts.paper:
        txt.DrawLatex(.18,.92,"#bf{CMS}")
    else:
        txt.DrawLatex(.16,.92,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
    txt.SetTextSize(20)
    txt.SetTextAlign(11)
    if cat != "":
        txt.DrawLatex(.73,.90,"#bf{Cat="+re.sub('_','',cat)+"}")


    # prepare err
    errAll = mcAll.Clone("errAll") ## with stat uncertainties
    for ibin in range(0,mcAll.GetNbinsX()):
        mcAll.SetBinError(ibin+1,0)
    #errAll = mcAll.Clone("errAll") ## without stat uncertainties
    errAll.SetFillStyle(3001)
    errAll.SetFillColor(ROOT.kGray)
    errAll.SetMarkerColor(ROOT.kGray)

    leg.SetNColumns(2)

    if not opts.noRatio:
        leg.AddEntry(errAll,"syst.","F")
    while True:
        if len(leg2) !=0:
            h,label,opt = leg2.pop()
            leg.AddEntry(h,label,opt)
        if len(leg1) !=0:
            h,label,opt = leg1.pop()
            leg.AddEntry(h,label,opt)
        if len(leg1) ==0 and len(leg2)==0:
            break

    leg.Draw("SAME")
    pup.SetLogy()
    c.Modify()
    c.Update()

    #################### DOWN #########
    if not opts.noRatio:
        pdown.cd()
        r=hdata.Clone("r")

        
        errAll.Divide(mcAll)
        r.Divide(mcAll)
        r.Draw("AXIS")
        #r.Draw("AXIS X+ Y+ SAME")

        txt.SetTextSize(16)
        txt.SetTextAlign(23)
        txt.SetTextColor(ROOT.kGray+2)
        txt.SetNDC(False)
        for x in values:
            x0=x
            if x0> .9 and x0 <.92: x0 =.9
            if x0> .93 : x0 =.97
            txt.DrawLatex(x0,1.45,"%.0f%%"%(x*100))

        for idx,s in enumerate(systs):
            try:
                hSystUp = b_systs_up[idx].GetHist()
            except:
                hSystUp=mcAll.Clone("nosystup")
            try:
                hSystDown=  b_systs_down[idx].GetHist()
            except:
                hSystDown=  mcAll.Clone("nosystdn")

            hSystUp.Divide(mcAll)
            hSystDown.Divide(mcAll)

            #hSystUp.SetLineColor(ROOT.kRed)
            #hSystUp.SetLineWidth(2)
            #hSystDown.SetLineColor(ROOT.kRed+2)
            #hSystDown.SetLineWidth(2)
            #hSystUp.Draw("HIST SAME")
            #hSystDown.Draw("HIST SAME")
            garbage.extend([hSystUp,hSystDown])

            for ibin in range(0,errAll.GetNbinsX()):
                mc = mcAll.GetBinContent(ibin+1)
                cont = errAll.GetBinContent(ibin+1)
                e = errAll.GetBinError(ibin+1)
                eUp = cont + e - 1.
                eDown = abs(cont - e - 1.)
                up=hSystUp.GetBinContent(ibin+1) -1.
                down = hSystDown.GetBinContent(ibin+1) -1.
                if up<down:
                    x=down
                    down=up
                    up=x
                if up > 0:
                    eUp = math.sqrt( up**2 + eUp**2)  +1
                else:
                    eUp +=1.
                if down<0:
                    eDown = 1-math.sqrt( down**2 + eDown**2) 
                else:
                    eDown = 1 - eDown
                errAll.SetBinContent(ibin+1,(eUp+eDown)/2.)
                errAll.SetBinError(ibin+1,abs(eUp-eDown)/2.)


        for i in range(0,errAll.GetNbinsX()):
            cont= errAll.GetBinContent(ibin+1)
            e = errAll.GetBinContent(ibin+1)
            if cont> 1.5:
                down = cont -e
                errAll.SetBinContent(ibin+1,1.5)
                errAll.SetBinError(ibin+1,1.5-down)
            if cont< 0.5:
                up = cont +e
                errAll.SetBinContent(ibin+1,0.5)
                errAll.SetBinError(ibin+1,0.5+up)
                
        errAll.Draw("E2 SAME")
        #r.GetXaxis().SetTitle("m^{#mu#mu}[GeV]")
        r.GetXaxis().SetTitle(dummy.GetXaxis().GetTitle())
        r.GetXaxis().SetTitleOffset(1.2)
        r.GetYaxis().SetTitle("Events")
        r.GetYaxis().SetTitleOffset(2.0)
        r.GetXaxis().SetTitleOffset(5.0)
        #r.GetXaxis().SetRangeUser(60,150)
        r.GetXaxis().SetRangeUser( float(opts.xrange.split(',')[0]),float(opts.xrange.split(',')[1]))
        r.GetYaxis().SetRangeUser(0.5,1.50)
        r.GetYaxis().SetLabelFont(43)
        r.GetXaxis().SetLabelFont(43)
        r.GetYaxis().SetTitleFont(43)
        r.GetXaxis().SetTitleFont(43)
        r.GetYaxis().SetLabelSize(20)
        r.GetXaxis().SetLabelSize(20)
        r.GetYaxis().SetTitleSize(24)
        r.GetXaxis().SetTitleSize(24)
        r.GetYaxis().SetNdivisions(502)

        txt.SetTextColor(ROOT.kBlack)
        txt.SetTextFont(43)
        txt.SetTextSize(20)
        txt.SetNDC(True)
        r.GetYaxis().SetLabelSize(0)
        txt.SetTextAlign(31)
        #pdown.SetBottomMargin(0.50)
        txt.DrawLatex(.145,.50,"0.5")
        txt.SetTextAlign(32)
        txt.DrawLatex(.145,.75,"1.0")
        txt.SetTextAlign(33)
        txt.DrawLatex(.145,.99,"1.5")

        g=ROOT.TGraph()
        g.SetName("1"+cat)
        g.SetPoint(0,60,1)
        g.SetPoint(1,150,1)
        g.SetLineColor(ROOT.kGray+2)
        g.SetLineStyle(7)
        g.SetLineWidth(2)
        g.Draw("L SAME")

        r.Draw("P E X0 SAME")

        garbage.extend([g,r])

    if opts.outdir=="":
        raw_input("ok?")
    else:
        extra = ""
        if opts.doRemap: extra += "_QM"
        if opts.noRatio: extra += "_noratio"
        if cat != "":
            c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + extra+ "_bkg" + ".pdf")
            c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + extra+ "_bkg" + ".png")
        else:
            c.SaveAs(opts.outdir + "/" + opts.var +extra+ "_bkg" + ".pdf")
            c.SaveAs(opts.outdir + "/" + opts.var +extra+ "_bkg" + ".png")
    #try to clean
    for g in garbage:
        g.Delete()
    garbage=[]

    fIn.Close()
    fIn = ROOT.TFile.Open(opts.input,"READ")
    if fIn == None: 
        print "<*> NO File '%s'"%opts.input
        raise IOError
    ## end cat loop
  ## end doBkg
    
#Local Variables:
#mode:python
#indent-tabs-mode:nil
#tab-width:4
#c-basic-offset:4
#End:
#vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
