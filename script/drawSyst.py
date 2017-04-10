import os,sys
from optparse import OptionParser
from array import array
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="DY.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="ChargedHiggsTauNu/Vars/Mt_WJets")
parser.add_option("","--base2",type='string',help="Base2 (for correlation Kamuca only) [Default=%default]", default="")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)
parser.add_option("-o","--output",type='string',help="output [Default=%default]", default="")
parser.add_option("-b","--batch",action='store_true',help="batch [Default=%default]", default=False)
opts, args = parser.parse_args()

oldSys = sys.argv[:]
sys.argv = []
import ROOT
if opts.batch: ROOT.gROOT.SetBatch()
sys.argv = oldSys [:]

def Rebin(h):
    ''' Rebin with un-even bins '''
    mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
    #h1 = ROOT.TH1D(h.GetName()+"_rebin",h.GetTitle(),len(mybins)-1,mybins)
    h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
    return h1


fIn=ROOT.TFile.Open(opts.input)

if fIn==None:
    print "[ERROR] File", opts.input, "doesn't exist"
    raise IOError

h=fIn.Get(opts.base)

if h==None:
    print "[ERROR] Hist", opts.base, "doesn't exist"
    raise IOError

hL=[]


if opts.base2 != "":
    h2=fIn.Get(opts.base2)
    if h2 == None:
        print "[ERROR] Hist", opts.base2, "doesn't exist"
        raise IOError
    hL2=[]

if opts.syst!='Pdf' and opts.syst != 'Scale' and opts.syst !='Kamuca':
    hUp=fIn.Get(opts.base+"_"+opts.syst+"Up")
    hDown=fIn.Get(opts.base+"_"+opts.syst+"Down")
    if hUp==None or hDown==None:
        print "[ERROR] Hist", opts.base+"_"+opts.syst+"Up/Down", "doesn't exist"
        raise IOError
    hL = [hUp,hDown]
    if opts.base2!="":
        hUp2=fIn.Get(opts.base2+"_"+opts.syst+"Up")
        hDown2=fIn.Get(opts.base2+"_"+opts.syst+"Down")
        hL2 = [hUp2,hDown2]

elif opts.syst=='Scale':
    for w in [ 'R','F','RF']:
        for s in ['Up','Down']:
            hTmp=fIn.Get(opts.base+"_Scale"+w+s)
            if hTmp==None:
                print "[ERROR] Hist", opts.base+"_Scale"+w+s, "doesn't exist"
                raise IOError
            hL.append(hTmp)
            if opts.base2 !="":
                hTmp2=fIn.Get(opts.base2+"_Scale"+w+s)
                if hTmp2==None:
                    print "[ERROR] Hist", opts.base2+"_Scale"+w+s, "doesn't exist"
                    raise IOError
                hL2.append(hTmp2)

elif opts.syst=='Pdf':
    for i in range(0,100):
        hTmp=fIn.Get(opts.base+"_Pdf%d"%i)
        if hTmp==None:
            print "[ERROR] Hist", opts.base+"_Pdf%d"%i, "doesn't exist"
            raise IOError
        hL.append(hTmp)
        if opts.base2 !="":
            hTmp2=fIn.Get(opts.base2+"_Pdf%d"%i)
            if hTmp2==None:
                print "[ERROR] Hist", opts.base2+"_Scale"+w+s, "doesn't exist"
                raise IOError
            hL2.append(hTmp2)

elif opts.syst=='Kamuca':
    for i in range(-133,135):
        if i==0:continue
        for s in ['Up','Down']:
            hTmp=fIn.Get(opts.base+"_KAMUCA%d"%i+s)
            if hTmp==None:
                print "[ERROR] Hist", opts.base+"_KAMUCA%d"%i + s, "doesn't exist"
                raise IOError
            hL.append(hTmp)
            if opts.base2!="":
                hTmp2=fIn.Get(opts.base2+"_KAMUCA%d"%i+s)
                if hTmp2==None:
                    print "[ERROR] Hist", opts.base2+"_KAMUCA%d"%i + s, "doesn't exist"
                    raise IOError
                hL2.append(hTmp2)

if opts.rebin> 999:
    h=Rebin(h)
else:
    h.Rebin(opts.rebin)

for idx,hTmp in enumerate(hL):
    if opts.rebin> 999:
        hL[idx]=Rebin(hTmp)
    else:
        hTmp.Rebin(opts.rebin)

if opts.base2 != "":
    if opts.rebin> 999: h2=Rebin(h2)
    else:h2.Rebin(opts.rebin)
    for idx,hTmp in enumerate(hL2):
        if opts.rebin> 999: hTmp=Rebin(hTmp)
        else:hTmp.Rebin(opts.rebin)


h.SetLineColor(ROOT.kBlack)

if len(hL)==2:
    colors=[ROOT.kBlue,ROOT.kRed]
elif len(hL)==6:
    colors=[ROOT.kBlue,ROOT.kBlue-4,ROOT.kRed,ROOT.kRed-4,ROOT.kGreen+2,ROOT.kGreen]
else:
    colors=[ 
         ROOT.kOrange-7,ROOT.kOrange-4,ROOT.kOrange,ROOT.kOrange+2,
         ROOT.kRed+2,ROOT.kRed,ROOT.kRed-4,ROOT.kRed-7,
         ROOT.kMagenta-7,ROOT.kMagenta-4,ROOT.kMagenta,ROOT.kMagenta+2,
         ROOT.kBlue+2,ROOT.kBlue,ROOT.kBlue-4,ROOT.kBlue-7,
         ROOT.kCyan,
         ROOT.kGreen-7,ROOT.kGreen-4,ROOT.kGreen,ROOT.kGreen+2
         ]
for hTmp in hL:
    mycolor=colors[0]
    colors=colors[1:]+ [mycolor]
    hTmp.SetLineColor(mycolor)

h.SetLineWidth(2)


for hTmp in hL:
    hTmp.SetLineWidth(2)
    hTmp.SetLineStyle(7)


if opts.syst == 'Kamuca':
    mean = ROOT.TH1D("means","mean;mean [GeV];events",200,124,125) #every .1 geV
    if 'EE'  in opts.base or 'OE' in opts.base or 'BE' in opts.base or 'OO' in opts.base:
        sigma =  ROOT.TH1D("sigmas","sigma;sigma [GeV];events",1800,1.,10.0)
    else:
        sigma =  ROOT.TH1D("sigmas","sigma;sigma [GeV];events",200,1.,2.0)
    gaus=ROOT.TF1("myfunc","gaus",122,128) ## for Kamuca
    mL =[]
    sL =[]
    ### w = ROOT.RooWorkspace("w","w")
    ### mmm = ROOT.RooRealVar("mmm","mmm",122,128)
    ### getattr(w,'import')(mmm)
    ### m1=ROOT.RooRealVar("m1","m1",125,122,128);
    ### g1 = RO 
    ### doubleGaus =

c1=ROOT.TCanvas("c1")
h.Draw("HIST")

## keep track of norms
norm=[]
norm2=[]
norm.append( h.Integral() ) 
if opts.base2!="":
    norm2.append( h2.Integral() ) 

for idx,hTmp in enumerate(hL):
    hTmp.Draw("HIST SAME")
    norm.append( hTmp.Integral() ) 
    if opts.syst=='Kamuca':
        hTmp.Fit("myfunc","QLNR")
        hTmp.Fit("myfunc","QMLNR")
        mean.Fill(gaus.GetParameter(1))
        sigma.Fill(gaus.GetParameter(2))
        mL .append( gaus.GetParameter(1) )
        sL .append( gaus.GetParameter(2) )
        #print "-> Filling mean", gaus.GetParameter(1)
    else:
        print "Yields variation for ",hTmp.GetName(),":", "%.1f %%"%( (hTmp.Integral()/h.Integral()-1) *100.)
    if opts.base2!='':
        norm2.append(hL2[idx].Integral())

if opts.base2!='':
    from numpy import corrcoef
    print "Correlation is ", corrcoef(norm,norm2)[0][1]

if opts.base2 != "" and opts.syst == "Kamuca":
    mL2=[]
    sL2=[]
    mean2D= ROOT.TH2D("mean2d","mean2d",200,124,125,200,124,125)
    sigma2D= ROOT.TH2D("sigma2d","sigma2d",200,1,2.0,200,1.,2.0)
    for idx,hTmp in enumerate(hL2):
        hTmp.Fit("myfunc","QLNR")
        hTmp.Fit("myfunc","QMLNR")
        mL2 .append( gaus.GetParameter(1) )
        sL2 .append( gaus.GetParameter(2) )
        mean2D.Fill( mL[idx],mL2[idx])
        sigma2D.Fill( sL[idx],sL2[idx])

c2=ROOT.TCanvas("c2")
r=h.Clone("r")

rL=[]
for hTmp in hL:
    rTmp=hTmp.Clone("r"+hTmp.GetName())
    rL.append(rTmp)

r.Divide(h)
for rTmp in rL:
    rTmp.Divide(h)

r.Draw("HIST")
for rTmp in rL:
    rTmp.Draw("HIST SAME")

r.GetYaxis().SetRangeUser(0.,2.)


if opts.syst == 'Kamuca':
    if opts.base2 == "":
        c3=ROOT.TCanvas("c3","c3",1000,400)
        c3.Divide(2)
    else:
        c3=ROOT.TCanvas("c3","c3",1000,800)
        c3.Divide(2,2)
    c3.cd(1)
    mean.SetLineColor(ROOT.kBlack) 
    mean.SetLineWidth(2)
    mean.Draw("HIST")
    if 'EE'  in opts.base or 'OE' in opts.base or 'BE' in opts.base or 'OO' in opts.base:
        pass
    else:
        mean.GetXaxis().SetRangeUser(124.75,124.95)
    c3.cd(2)
    sigma.SetLineColor(ROOT.kBlack) 
    sigma.SetLineWidth(2)
    sigma.Draw("HIST")
    ltx=ROOT.TLatex()
    ltx.SetNDC()
    ltx.SetTextFont(43)
    ltx.SetTextSize(24)
    ltx.SetTextAlign(13)
    if opts.base2 != "":
        c3.cd(3)
        mean2D.Draw("COLZ")
        mean2D.GetXaxis().SetRangeUser(124.75,124.95)
        mean2D.GetYaxis().SetRangeUser(124.75,124.95)
        ltx.DrawLatex(.15,.85,"Correlation=%.2f"%(mean2D.GetCorrelationFactor()))
        c3.cd(4)
        sigma2D.Draw("COLZ")
        ltx.DrawLatex(.15,.85,"Correlation=%.2f"%(sigma2D.GetCorrelationFactor()))



if opts.output!="": 
    c1.SaveAs (opts.output + "_c1.pdf")
    c2.SaveAs (opts.output + "_c2.pdf")
    if opts.syst == "Kamuca":
        c3.SaveAs (opts.output + "_c3.pdf")
        import re
        fname=re.sub('/[^/]*$','',opts.output) + '/scales.txt'
        print "Writing on file:",fname
        txt = open( fname,'a')
        print >> txt, re.sub('.*/Mmm_','',opts.base),mean.GetRMS(),sigma.GetRMS()
else:
    raw_input("ok?")


