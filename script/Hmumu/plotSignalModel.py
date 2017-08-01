import ROOT 
import math

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

# get effective sigma from culmalative distribution function

def getEffSigma (mass, pdf,  wmin=120., wmax=130., step=0.002, epsilon=1.e-4):
  cdf = pdf.createCdf(ROOT.RooArgSet(mass))

  print "Computing effSigma....";
  sw=ROOT.TStopwatch();
  sw.Start();
  point=wmin;
  points = [];
  
  while (point <= wmax):
    mass.setVal(point);
    if pdf.getVal() > epsilon:
      points.append( (point,cdf.getVal()) ) 
    point+=step;
  
  low = wmin;
  high = wmax;
  width = wmax-wmin;
  for i in range(0,len(points)):
    for j in range (i,len(points)):
      wy = points[j][1] - points[i][1];
      if abs(wy-0.683) < epsilon:
        wx = points[j][0] - points[i][0];
        if wx < width:
          low = points[i][0];
          high = points[j][0];
          width=wx;
  sw.Stop();
  print "effSigma: [" , low , "-" , high , "] = " , width/2.
  print "\tTook: ", ; sw.Print();
  return (low,high)

# get FWHHM

def getFWHM(mass, pdf, data, wmin=110., wmax=130., step=0.0004):
 
  print "Computing FWHM...."
  nbins = (wmax-wmin)/step;
  h = ROOT.TH1F("h","h",int(math.floor(nbins+0.5)),wmin,wmax);
  if (data != None):
    pdf.fillHistogram(h,ROOT.RooArgList(mass),data.sumEntries());
  else :
    pdf.fillHistogram(h,ROOT.RooArgList(mass));
  
  hm = h.GetMaximum()*0.5;
  low = h.GetBinCenter(h.FindFirstBinAbove(hm));
  high = h.GetBinCenter(h.FindLastBinAbove(hm));

  print "FWHM: [" , low , "-" , high , "] Max = " , hm
  
  return high-low;


if __name__ == "__main__":
    import os,sys
    from array import array
    import re,math
    from subprocess import call
    from optparse import OptionParser, OptionGroup
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
    from hmm import hmm,hmmAutoCat,hmmWithTTH, Stack
    parser=OptionParser()
    parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
    parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmmWithTTH")
    parser.add_option("-p","--plot",dest="plot",type="string",help="PlotDirectory [%default]",default="Hmumu/2017/sigfit/final")
    parser.add_option("","--bins",dest="bins",type="int",help="Number of bins [%default]",default=40)
    opts,args= parser.parse_args()

    try:
        os.mkdir(opts.plot)
    except OSError:
        pass

    print "GetWS w from file", opts.input
    fIn = ROOT.TFile.Open(opts.input)
    if fIn == None:
        print "<*> Error,",opts.input,"no such file or directory"
    w = fIn.Get("w")
    if w == None:
        print "<*> Error, unable to locate workspace w in input file"

    config= eval(opts.hmm)
    config.Print()

    objs=[]
    mh=w.var("MH").setVal(125.)
    x = w.var("mmm")
    x.SetTitle("m_{#mu#mu} [GeV]")

    allPdfs=ROOT.RooArgList()
    allFractions=ROOT.RooArgList()
    allMc=None #ROOT.RooDataHist("allMc","allMc",ROOT.RooArgSet(x))


    for cat in config.categories:
        print "* Doing cat",cat
        fractions=ROOT.RooArgList()
        pdfs=ROOT.RooArgList()

        mcPerCat=None 

        if abs(w.var("MH").getVal() -125.) > 0.0001 :
            print "<*> ERROR, value of MH mismatch. Unable to set."

        for proc in config.processes:
            p = w.pdf("pdf_sigmodel_" + proc+ "_"+cat) 
            if p==None:
                print "<*> ERROR: Unable to find pdf","pdf_sigmodel_" + proc+ "_"+cat
            norm = w.function("pdf_sigmodel_" + proc+ "_"+cat + "_norm") . getVal()
            xsec = config.xsec(proc)
            br = config.br()
            lumi = config.lumi()
            pdfs.add(p)
            allPdfs.add(p)
            #var= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm*br*lumi)
            var= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm)
            var.setConstant()

            #print "* adding norm to ",cat ,"(",proc,")" ,"with norm",xsec*norm , "(xsec=",xsec,"ea=",norm,")"
            fractions.add(var)
            allFractions.add(var)

            objs.append(p)
            objs.append(var)

            mc=w.data("hist_"+proc+"_"+cat+"_M125")

            if mc == None: continue

            if mcPerCat==None:
                mcPerCat = mc.emptyClone("mcPerCat_"+cat)
            mcPerCat.add(mc,None,xsec*norm)
            #print "* mc per cat ",cat," adding ",proc,"integral =",mcPerCat.sumEntries(), "(",mc.sumEntries(),")"

            if allMc==None:
                allMc = mc.emptyClone("allMc_"+cat)
            allMc.add(mc,None,xsec*norm)

        pdf = ROOT.RooAddPdf("tot_model_"+cat,"tot model",pdfs,fractions)
        fwhm= getFWHM(x,pdf,None)
        low,high = getEffSigma (x, pdf) #,  wmin=120., wmax=130., step=0.002, epsilon=1.e-4)
        print "-> Eff sigma for",cat,"is",high - low,"FWHM=",fwhm

        print "-> Plot" 
        c= ROOT.TCanvas("c_"+cat,"c_"+cat,800,800)
        c.SetLeftMargin(0.15)
        c.SetRightMargin(0.05)
        c.SetBottomMargin(0.15)
        c.SetTopMargin(0.05)
        p= x.frame()
        mcPerCat.plotOn(p,ROOT.RooFit.Binning(opts.bins),ROOT.RooFit.MarkerStyle(24),ROOT.RooFit.DataError(ROOT.RooAbsData.SumW2))
        hist = p.getObject(int(p.numItems()-1));
        p.GetYaxis().SetTitleOffset(1.7)
        maxH = 0.0
        for i in range(0,hist.GetN()):
            if maxH< hist.GetY()[i]: maxH=hist.GetY()[i]
        p.GetYaxis().SetRangeUser(0,maxH*1.4)
        pdf.plotOn(p)
        p.Draw()

        txt = ROOT.TLatex()
        txt.SetNDC()
        txt.SetTextFont(43)
        txt.SetTextSize(30)
        txt.SetTextAlign(13)
        txt.DrawLatex(.18,.93,"#bf{CMS}, #scale[0.7]{#it{Preliminary Simulation}}")
        txt.SetTextAlign(33)
        txt.SetTextSize(24)
        txt.DrawLatex(.90,.89,cat)
        txt.DrawLatex(.90,.89-0.05,"FWHM=%.1f GeV"%fwhm)
        txt.DrawLatex(.90,.89-0.10,"#sigma_{eff}=%.1f GeV"%(high-low))
        c.SaveAs(opts.plot + "/signal_model_" + cat +".pdf")
        c.SaveAs(opts.plot + "/signal_model_" + cat +".png")

    pdf = ROOT.RooAddPdf("tot model","tot model",allPdfs,allFractions)
    fwhm= getFWHM(x,pdf,None)
    low,high = getEffSigma (x, pdf) #,  wmin=120., wmax=130., step=0.002, epsilon=1.e-4)
    print "-> Eff sigma for cat all is",high - low,"FWHM=",fwhm
    print "-> Plot" 
    c= ROOT.TCanvas("c_"+cat,"c_"+cat,800,800)
    c.SetLeftMargin(0.15)
    c.SetRightMargin(0.05)
    c.SetBottomMargin(0.15)
    c.SetTopMargin(0.05)
    p= x.frame()
    allMc.plotOn(p,ROOT.RooFit.Binning(opts.bins),ROOT.RooFit.MarkerStyle(24),ROOT.RooFit.DataError(ROOT.RooAbsData.SumW2))
    hist = p.getObject(int(p.numItems()-1));
    p.GetYaxis().SetTitleOffset(1.7)
    maxH = 0.0
    for i in range(0,hist.GetN()):
        if maxH< hist.GetY()[i]: maxH=hist.GetY()[i]
    p.GetYaxis().SetRangeUser(0,maxH*1.4)
    pdf.plotOn(p)
    p.Draw()
    txt = ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(30)
    txt.SetTextAlign(13)
    txt.DrawLatex(.18,.95,"#bf{CMS}, #scale[0.7]{#it{Preliminary Simulation}}")
    txt.SetTextAlign(33)
    txt.SetTextSize(24)
    txt.DrawLatex(.90,.89-0.05,"FWHM=%.1f GeV"%fwhm)
    txt.DrawLatex(.90,.89-0.10,"#sigma_{eff}=%.1f GeV"%(high-low))
    c.SaveAs(opts.plot + "/signal_model_all.pdf")
    c.SaveAs(opts.plot + "/signal_model_all.png")

    
