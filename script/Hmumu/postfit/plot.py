import ROOT
import math
import re
#from glob import glob
import os,sys

class Stack:
    ''' This is a soft version of thstack that usually crash. From hmm'''
    def __init__(self):
        self.hists_=[]
        self.draw_ =[]
        self.name_="myStack"

    def SetName(self,name):
        self.name_ = name

    def Add(self,h,draw=True):
        if len(self.hists_) == 0:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
        else:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
            self.hists_[-1].Add(self.hists_[-2])
        self.draw_.append(draw)

    def Draw(self,opts=""):
        firstDraw=True
        for idx in reversed(range(0,len(self.hists_))):
            h= self.hists_[idx]
            if not self.draw_[idx]: continue
            if firstDraw or 'SAME' in opts: 
                h.Draw(opts)
                firstDraw=False
            else       : 
                h.Draw(opts +" SAME")
                firstDraw=False
    def GetHist(self):
        return self.hists_[-1]

    def GetFirstHist(self):
        return self.hists_[0]

    def Delete(self):
        for h in self.hists_: h.Delete()
        self.hists_=[]

    def Remap(self,qm):
        hists2=[]
        for h in self.hists_:
            hists2.append(qm.Apply(h) )
        self.hists_ = hists2[:]

    def Print(self):
        print "------",self.name_,"-----------"
        for h in self.hists_:
            print "*",h.GetName()
        print "-------------------------------"


from optparse import OptionParser
parser=OptionParser()

#parser.add_option("-i","--indata",default="worspace.root",help="input prefit for data file (for data) [%default]")
parser.add_option("-i","--indir",default="jobs/",help="toys directory [%default]")
parser.add_option("-o","--outdir",default="plots",help="output directory [%default]")
parser.add_option("-x","--xvar",default="CMS_th1x",help="x variable [%default]")
#parser.add_option("-p","--processes",default="",help="comma separated list of processes [%default]")
opts,args=parser.parse_args()

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPadTickX(1)
ROOT.gStyle.SetPadTickY(1)
ROOT.gROOT.ForceStyle()

OpenFiles={}

def GetFromFile(fname,objname):
    ''' Open a file and return obj. Abort program execution on failure'''
    global OpenFiles
    fIn=OpenFiles[fname] if fname in OpenFiles else None
    if fIn == None: fIn=ROOT.TFile.Open(fname)

    if fIn == None:
        print "ERROR unable to open file:",fname
        sys.exit(1)
    OpenFiles[fname]=fIn

    o = fIn .Get (objname)
    if o == None:
        print "ERROR unable to get object",objname,"from file:",fname
        sys.exit(1)
    return o

def GetFromWs(fname,objname):
    ''' Open file and return obj from ws'''
    w= GetFromFile(fname,'w')
    o=w.data(objname)
    if o != None: return o
    o=w.pdf(objname)
    if o != None: return o
    o=w.var(objname)
    if o != None: return o
    o=w.cat(objname)
    if o != None: return o
    print "ERROR unable or unsupported object",objname,"in workspace w in file",fname
    sys.exit(1)

def ListDir(d):
    return [os.path.join(d, f) for f in os.listdir(d)]

def ResetErrors(h):
    for ibin in range(0,h.GetNbinsX()+2): h.SetBinError(ibin,0)

def adaptHist(h):
    '''Use this macro to make hist adjustments.Pass to hist before writing.'''
    name= h.GetName()
    print "-> Adapting histogram:",name,
    if name.startswith("err2_"):
        print "is 2s"
        h.SetFillColor(ROOT.kYellow)
        #h.SetFillStyle(ROOT.kSolid)
        h.SetLineColor(ROOT.kYellow)
    elif name.startswith("err_"):
        print "is 1s"
        h.SetFillColor(ROOT.kGreen)
        #h.SetFillStyle(ROOT.kSolid)
        h.SetLineColor(ROOT.kGreen)
    elif name.startswith("toynominal_"):
        print "is nominal"
        h.SetLineColor(ROOT.kRed)
        h.SetLineWidth(2)
    elif name.startswith("data_"):
        print "is data"
        h.SetMarkerStyle(20)
        h.SetMarkerSize(1.3)
        h.SetMarkerColor(ROOT.kBlack)
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(2)
    else:
        ## process
        name = name.split('__')[0] ## remove var name in case
        pname = "_".join(name.split("_")[:-1])
        num = int(name.split("_")[-1])
        h.SetLineColor(ROOT.kBlack)
        print "is process",pname,"num=",num
    
        ## from tau-tau
        #if num==-1: pass
        #elif num == 0: h.SetFillColor(ROOT.TColor.GetColor(250,202,255))
        #elif num == 1: h.SetFillColor(ROOT.TColor.GetColor(222,90,106))
        #elif num == 2: h.SetFillColor(ROOT.TColor.GetColor(155,152,204))
        #elif num == 3: h.SetFillColor(ROOT.TColor.GetColor(248,206,104))
        #else: h.SetFillColor(ROOT.kGray)

        ##from hmm
        if num ==-1 : pass
        elif num==0:h.SetFillColor(ROOT.kBlue-10)
        elif num==1:h.SetFillColor(ROOT.kRed-10)
        elif num==2:h.SetFillColor(ROOT.kGreen-10)
        elif num==2:h.SetFillColor(ROOT.kOrange-10)
        else: h.SetFillColor(ROOT.kGray)

    return h

def Quantile(sigma):
    return 0.5*(1.0+ ROOT.TMath.Erf(sigma/math.sqrt(2)))

nominal=opts.indir+"/"+"higgsCombinecombout_step3_done_nominal.root"
opts.indata=nominal

datain=GetFromWs(opts.indata,'data_obs')
toynominal=GetFromFile(opts.indata,"toys/toy_asimov")

## to have the proc splitting
sbpdf=GetFromWs(opts.indata,'model_s')
bpdf=GetFromWs(opts.indata,'model_b')

#TODO: implement multiple xvar
x=GetFromWs(opts.indata,opts.xvar)

chan=GetFromWs(opts.indata,'CMS_channel')

pwd=os.environ['PWD']
ROOT.gSystem.mkdir(opts.outdir,True);
ROOT.gSystem.cd(opts.outdir);

quant =   {"-2s":Quantile(-2.0),
           "-1s":Quantile(-1.0),
           "med":0.5,
           "1s":Quantile(1.0),
           "2s":Quantile(2.0),
           }

outfile = ROOT.TFile.Open("output.root","RECREATE")

### all channels TODO: the loop here is not optimal, but who cares ?
for ich in range (0,chan.numTypes()):
    chan.setIndex(ich)
    print "[INFO]: Channel",ich,chan.getLabel()
    outfile.mkdir(chan.getLabel())
    outfile.cd(chan.getLabel())

    bcatpdf = bpdf.getPdf(chan.getLabel())
    sbcatpdf = sbpdf.getPdf(chan.getLabel())

    #hsbtmp=sbcatpdf.createHistogram("hsbtmp",x)
    #hbtmp=sbcatpdf.createHistogram("hbtmp",x)
    #htmp= ROOT.TH1D(hsbtmp-hbtmp)

    datacat= getattr(datain,'reduce')("CMS_channel==CMS_channel::%d"%chan.getIndex())
    data_hist = datacat.createHistogram("data_%d"%ich,x)
    toycat=getattr(toynominal,'reduce')("CMS_channel==CMS_channel::%d"%chan.getIndex())
    mc_hist=toycat.binnedClone().createHistogram("toynominal_%d"%ich,x)
    
    #from bcatpdf
    #.pdfList()
    #print "DEBUG:",bcatpdf.Print()
    #print "DEBUG:",bcatpdf.Print("V")
    bpdf_add = None
    isRooRealSumPdf=False

    for p in bcatpdf.pdfList():
        #print "DEBUG Considering p",p.GetName()
        #p.Print("")
        if isinstance(p,ROOT.RooAddPdf):
            bpdf_add = p
            isRooRealSumPdf=False
            break
        if isinstance(p,ROOT.RooRealSumPdf):
            # take the CMS_HistPropagator instead
            bpdf_add = p.funcList()[0]
            isRooRealSumPdf=True
            break

    if bpdf_add ==None:
        print "ERROR: Unable to find b RooAddPdf"

    procs_hist={}
    
    if isRooRealSumPdf:
        nproc = bpdf_add.funcList().getSize()
    else:
        nproc = bpdf_add.pdfList().getSize()

    for iproc in range(0,nproc):
        n = bpdf_add.coefList()[iproc]
        norm=n.getVal()

        if isRooRealSumPdf:
            p = bpdf_add.funcList()[iproc]
            print "-> BASE is ", bpdf_add.Print()
            print "-> PROC",iproc," is ", p.GetName(),p.GetTitle()
            #norm*=p.wrapper().getVal()
            integral=p.createIntegral(ROOT.RooArgSet(x))
            x.setVal(1.e10)
            norm*=integral.getVal()
            print "-> NORM",iproc," is ",n.GetName(),n.getVal(),integral.getVal()
        else:
            p = bpdf_add.pdfList()[iproc]

        
        #n_exp_binhmm_13TeV_cat0_proc_BKG 
        pname = re.sub('.*proc_','',n.GetName()) + "_%d" %iproc
        h = p.createHistogram(pname,x)
        h.Scale(1./h.Integral())
        h.Scale(norm)

        procs_hist[pname] = h


    #sbevents = sbcatpdf->expectedEvents(*mass);    
    #bevents = bcatpdf->expectedEvents(*mass);
    #sevents = sbevents - bevents;

    ### how to read toys
    ## loop over toys

    ## Get file list
    #dirs=ListDir(opts.indir)
    dirs=ListDir(pwd+"/"+opts.indir) 
    files = []
    for f in dirs:
        #print "DEBUG: Considering ",f,"for toys"
        if not re.search('.root',f): continue
        if not re.search('step3',f): continue
        if re.search('nominal',f): continue
        files.append(f)

    #print "DEBUG: Toy files=",files
    alltoys=[] ## double list: bin -> entries
    ntoys=0
    for fname in files:
        f=ROOT.TFile.Open(fname)
        if f==None: 
            print "skipping"
            continue
        toy=f.Get("toys/toy_asimov")
        if toy == None:
            print "skipping"
            continue
        ntoys+=1

        toycat = toy.reduce("CMS_channel==CMS_channel::%d"%chan.getIndex()) 
        hsbtmp=toycat.binnedClone().createHistogram("hsbtmp",x)

        for i in range(0,hsbtmp.GetNbinsX()):
            y=hsbtmp.GetBinContent(i+1)
            if len(alltoys)<i+1: alltoys.append([])
            if y<1.e10: alltoys[i].append(y)

        ## do something
        f.Close()
        del toy

    
    err_hist = mc_hist.Clone("err_hist")
    err2_hist = mc_hist.Clone("err2_hist")

    err_hist.Reset("ACE")
    err2_hist.Reset("ACE")

    #print "DEBUG: allToys",alltoys
    for i in range(0,len(alltoys)):
        ## error are the 1s band. should I do the expaction 
        ##  
        #if i==1:
        #    print "DEBUG"
        #    print "------------------------------------"
        #    print "bin=1","toys=",len(alltoys[i])
        #    print alltoys[i]
        entries = alltoys[i]
        entries.sort()
        minustwo = entries[ int(quant["-2s"] *len(entries))]
        minusone = entries[ int(quant["-1s"] *len(entries))]
        median = entries[  int(quant["med"] *len(entries))]
        plusone = entries[ int(quant["1s"] *len(entries))]
        plustwo = entries[ int(quant["2s"] *len(entries))]
    

        nom= mc_hist.GetBinContent(i+1)
        
        ymax= max(nom,plusone,minusone)
        ymin= min(nom,plusone,minusone)

        err_hist.SetBinContent(i+1,(ymax+ymin)/2.0)
        err_hist.SetBinError(i+1,abs(ymax-ymin)/2.0)

        ymax= max(nom,plustwo,minustwo)
        ymin= min(nom,plustwo,minustwo)
        err2_hist.SetBinContent(i+1,(ymax+ymin)/2.0)
        err2_hist.SetBinError(i+1,abs(ymax-ymin)/2.0)

    ## WRITE ##
    data_hist.SetBinErrorOption(ROOT.TH1.kPoisson)
    data_hist=adaptHist(data_hist)
    mc_hist=adaptHist(mc_hist)
    err_hist=adaptHist(err_hist)
    err2_hist=adaptHist(err2_hist)

    ResetErrors(mc_hist)
    mc_hist.Sumw2(False)
    
    outfile.cd()
    for pname in procs_hist:
        h=procs_hist[pname]
        ResetErrors(h)
        h.Sumw2(False)
        h=adaptHist(h)
        h.Write()

    data_hist.Write()
    mc_hist.Write()
    err_hist.Write()
    err2_hist.Write()

    ## DRAW CANVAS ##
    drawRatio=True
    canv= ROOT.TCanvas("canv_"+chan.getLabel(),"canvas",800,1000)
    canv.SetBottomMargin(0.15)
    canv.SetLeftMargin(0.15)
    canv.SetTopMargin(0.05)
    canv.SetRightMargin(0.05)
    pad1=ROOT.TPad("pad1_"+chan.getLabel(),"pad1",0,0,1,.3) # down
    pad2=ROOT.TPad("pad2_"+chan.getLabel(),"pad2",0,0.3,1,1) # up
    if drawRatio:
        for p in [pad1,pad2]:
            p.SetBottomMargin(0.15)
            p.SetLeftMargin(0.15)
            p.SetTopMargin(0.05)
            p.SetRightMargin(0.05)
            p.Draw()
        pad2.SetBottomMargin(0)
        pad1.SetTopMargin(0)
        pad1.SetBottomMargin(0.3)
        pad2.cd()

    data_hist.Draw("AXIS")
    data_hist.GetXaxis().SetTitle("x")
    data_hist.GetYaxis().SetTitle("Events")

    data_hist.GetXaxis().SetLabelFont(43)
    data_hist.GetXaxis().SetLabelSize(22)
    data_hist.GetYaxis().SetLabelFont(43)
    data_hist.GetYaxis().SetLabelSize(22)
    data_hist.GetXaxis().SetTitleFont(43)
    data_hist.GetXaxis().SetTitleSize(28)
    data_hist.GetYaxis().SetTitleFont(43)
    data_hist.GetYaxis().SetTitleSize(28)

    data_hist.GetYaxis().SetTitleOffset(1.4)
    data_hist.GetXaxis().SetTitleOffset(1.4)

    s=Stack()
    for pname in procs_hist:
        h=procs_hist[pname]
        #h.Draw("H SAME")
        s.Add(h)

    s.Draw("H SAME")

    err2_hist.Draw("E2 SAME") 
    err_hist.Draw("E2 SAME") 
    
    mc_hist.Draw("H SAME")

    data_hist.Draw("AXIS SAME")
    data_hist.Draw("AXIS X+ Y+ SAME")
    data_hist.Draw("PE X0 SAME")

    txt = ROOT.TLatex()
    txt.SetTextSize(0.06)
    txt.SetNDC()
    txt.SetTextFont(42)
    txt.SetTextAlign(33)
    txt.DrawLatex(0.92,.92,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")

    if drawRatio:
        data_hist.GetYaxis().SetRangeUser(0.001,data_hist.GetMaximum()*1.25)
        pad1.cd()
        base=mc_hist.Clone()
        ResetErrors(base)
        dataR = data_hist.Clone("dataR_%d"%ich)
        mcR   = mc_hist.Clone("mcR_%d"%ich)
        errR  = err_hist.Clone("errR_%d"%ich)
        err2R  = err2_hist.Clone("err2R_%d"%ich)

        dataR.Divide(base)
        mcR.Divide(base)
        ResetErrors(mcR)
        errR.Divide(base)
        err2R.Divide(base)
        
        dataR.GetYaxis().SetNdivisions(504)
        dataR.GetYaxis().SetTitle("Data/MC")
        dataR.GetXaxis().SetTitleOffset(3)

        dataR.Draw("AXIS")
        dataR.GetYaxis().SetRangeUser(0.3,1.6)
        err2R.Draw("E2 SAME") 
        errR.Draw("E2 SAME") 
        mcR.Draw("H SAME")

        dataR.Draw("AXIS SAME")
        dataR.Draw("AXIS X+ Y+ SAME")
        dataR.Draw("PE X0 SAME")

    raw_input("ok?")
    canv.Write()
