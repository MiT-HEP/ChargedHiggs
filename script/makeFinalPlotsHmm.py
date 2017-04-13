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
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="Hmumu")
parser.add_option("-x","--xrange",dest='xrange',type="string",help="xrange [%default]",default="60,150")
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
        txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
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

rebin=10
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
    c=ROOT.TCanvas("c"+cat+"_bkg","canvas",800,1000)
    pup=ROOT.TPad("up"+cat,"up",0,.2,1,1)
    pdown=ROOT.TPad("down"+cat,"up",0,0,1,.2)

    pup.SetTopMargin(0.05)
    pup.SetRightMargin(0.05)
    pup.SetBottomMargin(0)
    pup.SetLeftMargin(0.15)

    pdown.SetTopMargin(0.0)
    pdown.SetRightMargin(0.05)
    pdown.SetBottomMargin(0.50)
    pdown.SetLeftMargin(0.15)

    pup.Draw()
    pdown.Draw()
    garbage.extend([c,pup,pdown])

    leg = ROOT.TLegend(.68,.65,.92,.85)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    ## column in the right order
    leg1=[]
    leg2=[]

    print "-> Getting data hist","HmumuAnalysis/"+opts.dir+"/" + opts.var +  cat +"_Data"
    hdata=fIn.Get("HmumuAnalysis/"+opts.dir+"/" + opts.var + cat +"_Data" )
    hdata.Rebin(rebin)
    hdata.SetMarkerStyle(20)
    hdata.SetMarkerColor(ROOT.kBlack)
    hdata.SetLineColor(ROOT.kBlack)
    leg.AddEntry(hdata,"Data","PE")
    #mcs=["DY","TT","ST","WZ","WW","ZZ"]
    BkgMonteCarlos=["ZZ","WW","WZ","ST","TT","DY","EWK_LLJJ"]
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
    blind=True
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
            h.SetLineColor(ROOT.kRed-10)
            h.SetFillColor(ROOT.kRed-10)
        elif mc == 'WW' or mc == "ZZ":
            h.SetLineWidth(0)
            h.SetLineColor(ROOT.kGreen-10)
            h.SetFillColor(ROOT.kGreen-10)
        elif mc == 'WZ' :
            h.SetFillColor(ROOT.kGreen-10)
            leg1.append((h,"VV","F"))
        elif mc == 'EWK_LLJJ' :
            h.SetFillColor(ROOT.kOrange-4)
            leg1.append((h,"EWK_LLJJ","F"))

        bkg.Add(h)

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
        sig.Add(h)
    ## end mc loop
    c.cd()
    pup.cd()
    #bkg.Draw("HIST") THSTack
    dummy = bkg.GetHist()
    dummy.Draw("AXIS")
    dummy.GetXaxis().SetTitle("m^{#mu#mu}[GeV]")
    dummy.GetXaxis().SetTitleOffset(2.0)
    dummy.GetYaxis().SetTitle("Events")
    dummy.GetYaxis().SetTitleOffset(2.0)
    dummy.GetXaxis().SetRangeUser( float(opts.xrange.split(',')[0]),float(opts.xrange.split(',')[1]))
    dummy.GetYaxis().SetRangeUser(1.e-1,dummy.GetMaximum()*10)

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
    hdata.Draw("P E X0 SAME")

    dummy.Draw("AXIS X+ Y+ SAME")
    dummy.Draw("AXIS SAME")

    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(20)
    txt.SetTextAlign(31)
    txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(config.lumi()/1000.)))
    txt.SetTextSize(30)
    txt.SetTextAlign(13)
    txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
    txt.SetTextSize(20)
    txt.SetTextAlign(11)
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
    pdown.cd()
    r=hdata.Clone("r")

    
    errAll.Divide(mcAll)
    r.Divide(mcAll)
    r.Draw("AXIS")
    r.Draw("AXIS X+ Y+ SAME")

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
    r.GetXaxis().SetTitle("m^{#mu#mu}[GeV]")
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
        if cat != "":
            c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + "_bkg" + ".pdf")
            c.SaveAs(opts.outdir + "/" + re.sub('_','',cat) + "_bkg" + ".png")
        else:
            c.SaveAs(opts.outdir + "/" + opts.var + "_bkg" + ".pdf")
            c.SaveAs(opts.outdir + "/" + opts.var + "_bkg" + ".png")
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
