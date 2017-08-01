import ROOT 
import math

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
    opts,args= parser.parse_args()

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
    for cat in config.categories:
        print "* Doing cat",cat
        fractions=ROOT.RooArgList()
        pdfs=ROOT.RooArgList()
        mh=w.var("MH").setVal(125.)
        x = w.var("mmm")

        #hist_GluGlu_cat0_M125

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
            var= ROOT.RooRealVar("f_"+cat+"_"+proc,"fraction",xsec*norm*br*lumi)
            var.setConstant()
            fractions.add(var)
            objs.append(p)
            objs.append(var)
        pdf = ROOT.RooAddPdf("tot model","tot model",pdfs,fractions)
        low,high = getEffSigma (x, pdf) #,  wmin=120., wmax=130., step=0.002, epsilon=1.e-4)
        fwhm= getFWHM(x,pdf,None)
        print "-> Eff sigma for",cat,"is",high - low,"FWHM=",fwhm

    
