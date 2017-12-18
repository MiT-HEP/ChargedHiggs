import ROOT 
import math

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
##
##    #cat FWHM S B 2*seff
store={}
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

#S/S+B pure
##0.001034
##0.002603
##0.003271
##0.001608
##0.005298
##0.007068
##0.009051
##0.006295
##0.004984
##0.00373
##0.003027
##0.014302
##0.008796
##0.012502
##0.025259

##weights={}
##for i in range(0,15):
##    weights[i] = store[i][1] / math.sqrt(store[i][2])

## s/s+b *  stot/nsweight, from macro
weights={
    0:  0.168556,
    1:  0.617629,
    2:  0.829896,
    3:  0.392717,
    4:  1.259562,
    5:  0.827407,
    6:  1.105182,
    7:  1.014070,
    8:  0.767930,
    9:  0.442198,
    10:  0.474796,
    11:  2.272860,
    12:  1.426070,
    13:  1.580796,
    14:  4.196775,
}
# get effective sigma from culmalative distribution function

def getEffSigma (mass, pdf,  wmin=120., wmax=130., step=0.002, epsilon=1.e-4):
  #FAST
  return (0,0)
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
    x.setRange(110,140)

    allPdfs=ROOT.RooArgList()
    allWPdfs=ROOT.RooArgList()
    allFractions=ROOT.RooArgList()
    allWFractions=ROOT.RooArgList()
    allWNum=[]
    allMc=None #ROOT.RooDataHist("allMc","allMc",ROOT.RooArgSet(x))
    allWMc=None #ROOT.RooDataHist("allMc","allMc",ROOT.RooArgSet(x))

    ## norm for allMc and allWMc
    SumAllMc=0.0
    SumAllWMc=0.0
    for icat,cat in enumerate(config.categories):
        for proc in config.processes:
            p = w.pdf("pdf_sigmodel_" + proc+ "_"+cat) 
            if p==None:
                print "<*> ERROR: Unable to find pdf","pdf_sigmodel_" + proc+ "_"+cat
            norm = w.function("pdf_sigmodel_" + proc+ "_"+cat + "_norm") . getVal()
            xsec = config.xsec(proc)
            br = config.br()
            lumi = config.lumi()
            SumAllMc += xsec*norm
            SumAllWMc += xsec*norm*weights[icat]

    for icat,cat in enumerate(config.categories):
        print "* Doing cat",cat
        fractions=ROOT.RooArgList()
        pdfs=ROOT.RooArgList()

        mcPerCat=None 

        if abs(w.var("MH").getVal() -125.) > 0.0001 :
            print "<*> ERROR, value of MH mismatch. Unable to set."

        SumPerCat=0.0

        for proc in config.processes:
            p = w.pdf("pdf_sigmodel_" + proc+ "_"+cat) 
            if p==None:
                print "<*> ERROR: Unable to find pdf","pdf_sigmodel_" + proc+ "_"+cat
            norm = w.function("pdf_sigmodel_" + proc+ "_"+cat + "_norm") . getVal()
            xsec = config.xsec(proc)
            br = config.br()
            lumi = config.lumi()
            SumPerCat += xsec*norm

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
            allWPdfs.add(p)
            #var= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm*br*lumi)
            var= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm)
            var.setConstant()


            #print "* adding norm to ",cat ,"(",proc,")" ,"with norm",xsec*norm , "(xsec=",xsec,"ea=",norm,")"
            fractions.add(var)
            allFractions.add(var)

            var2= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm*weights[icat])
            var2.setConstant()
            allWFractions.add(var2)
            objs.append(var2)

            objs.append(p)
            objs.append(var)

            mc=w.data("hist_"+proc+"_"+cat+"_M125")

            if mc == None: continue

            if mcPerCat==None:
                mcPerCat = mc.emptyClone("mcPerCat_"+cat)
            #mcPerCat.add(mc,None,xsec*norm)
            mcPerCat.add(mc,None,xsec*norm/SumPerCat)
            #print "* mc per cat ",cat," adding ",proc,"integral =",mcPerCat.sumEntries(), "(",mc.sumEntries(),")"

            if allMc==None:
                allMc = mc.emptyClone("allMc_"+cat)
            allMc.add(mc,None,xsec*norm/SumAllMc)

            if allWMc==None:
                allWMc = mc.emptyClone("allWMc_"+cat)
            allWMc.add(mc,None,xsec*norm*weights[icat]/SumAllWMc)

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
        p.SetYTitle("a.u.")
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
        txt.DrawLatex(.18,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary Simulation}}")
        txt.SetTextAlign(33)
        txt.SetTextSize(24)
        #txt.DrawLatex(.90,.89,cat)
        mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
        txt.DrawLatex(.90,.89,"cat %d"%mCatToCat[icat])
        txt.DrawLatex(.90,.89-0.05,"FWHM=%.1f GeV"%fwhm)
        #txt.DrawLatex(.90,.89-0.10,"#sigma_{eff}=%.1f GeV"%(high-low))
        c.SaveAs(opts.plot + "/signal_model_" + cat +".pdf")
        c.SaveAs(opts.plot + "/signal_model_" + cat +".png")

    ## normal sum
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
    p.SetYTitle("a.u.")
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
    txt.DrawLatex(.18,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary Simulation}}")
    txt.SetTextAlign(33)
    txt.SetTextSize(24)
    txt.DrawLatex(.90,.89-0.05,"FWHM=%.1f GeV"%fwhm)
    #txt.DrawLatex(.90,.89-0.10,"#sigma_{eff}=%.1f GeV"%(high-low))
    c.SaveAs(opts.plot + "/signal_model_all.pdf")
    c.SaveAs(opts.plot + "/signal_model_all.png")
    ##WW
    print "-> Doing weighted model"
    pdf = ROOT.RooAddPdf("weight model","weight model",allWPdfs,allWFractions)
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
    p.SetYTitle("a.u.")
    allWMc.plotOn(p,ROOT.RooFit.Binning(opts.bins),ROOT.RooFit.MarkerStyle(24),ROOT.RooFit.DataError(ROOT.RooAbsData.SumW2))
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
    txt.DrawLatex(.18,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary Simulation}}")
    txt.SetTextAlign(33)
    txt.SetTextSize(24)
    txt.DrawLatex(.90,.89-0.05,"S/(S+B) weighted")
    txt.DrawLatex(.90,.89-0.10,"FWHM=%.1f GeV"%fwhm)
    #txt.DrawLatex(.90,.89-0.15,"#sigma_{eff}=%.1f GeV"%(high-low))
    c.SaveAs(opts.plot + "/signal_model_weight.pdf")
    c.SaveAs(opts.plot + "/signal_model_weight.png")

    
