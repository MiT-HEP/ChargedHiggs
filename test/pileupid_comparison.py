
import sys,os

loose="Hmumu/2019_03_22_Hmm2017_LoosePUID/signal_yields.txt"
medium="Hmumu/2019_03_22_Hmm2017_MediumPUID/signal_yields.txt"
tight="Hmumu/2019_03_22_Hmm2017_TightPUID/signal_yields.txt"
noiseveto="Hmumu/2019_03_22_Hmm2017_LoosePUID_EENoise/signal_yields.txt"

def readYields(fname):
    print "-> opening",fname
    R={}
    fin = open(fname)
    for line in fin:
        R[(line.split()[0],line.split()[1])]=float(line.split()[2])
    fin.close()
    return R

store_l=readYields(loose)
store_m=readYields(medium)
store_t=readYields(tight)
store_v=readYields(noiseveto)

## and arrange them in a dictionary. [cut][cat,proc]->yields
store={}
store[0]=store_l
store[1]=store_m
store[2]=store_t
store[3]=store_v

#store (cat,proc) -> yield

if sys.argv[1] != "":
    cat=sys.argv[1]
else: cat='cat1'

maxSignal=100.
if cat=='cat1': maxSignal=5.
elif cat=='cat0':maxSignal=0.25
elif cat=='cat2':maxSignal=20.
elif cat=='cat3':maxSignal=56.
elif cat=='cat4':maxSignal=12.
elif cat=='cat5':maxSignal=70.
elif cat=='cat6':maxSignal=90.
elif cat=='cat7':maxSignal=4.
elif cat=='cat8':maxSignal=26.
elif cat=='cat9':maxSignal=35.
elif cat=='cat10':maxSignal=2.
elif cat=='cat11':maxSignal=1.
elif cat=='cat12':maxSignal=0.3
elif cat=='cat13':maxSignal=2.
elif cat=='cat14':maxSignal=2.


labels=['loosePUID','mediumPUID','tightPUID','noiseVeto']
processes=['GluGlu','VBF','ZH','WPlusH','WMinusH','ttH']
garbage=[]
import ROOT
if True:
    colors = [ ROOT.kBlue, 8, ROOT.kCyan+2, ROOT.kAzure-6,ROOT.kOrange+7,ROOT.kMagenta];
    c0=ROOT.TCanvas("c_"+cat+"_comparison","canvas",1600,800)

    c0.Range(-14.67532,-1.75,11.2987,15.75);
    c0.SetFillColor(0);
    c0.SetBorderMode(0);
    c0.SetBorderSize(2);
    c0.SetTopMargin(0.16);
    c0.SetLeftMargin(0.15);
    c0.SetRightMargin(0.05);
    c0.SetFrameBorderMode(0);
    c0.SetFrameBorderMode(0);
    c0.Draw()
    c0.cd()

    nLabels=len(labels)
    dummy = ROOT.TH2F("dummy","",1000,0.,maxSignal,nLabels,1-0.5,nLabels+0.5);
    dummy.SetStats(0);
    ci = ROOT.TColor.GetColor("#00ff00");
    dummy.SetFillColor(ci);
    dummy.SetTitle(cat)
    for il,label in enumerate(labels):
        dummy.GetYaxis().SetBinLabel(nLabels-il,label);

    dummy.GetXaxis().SetTickLength(0.01);
    dummy.GetYaxis().SetTickLength(0);
    dummy.GetXaxis().SetTitle("Expected Events");
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

    ymin = 0.0
    width = 0.34

    #maxSignal=0.

    for il,label in enumerate(labels):
        ybin = nLabels-il; ## 1-1 mapping
        ybinmin = ybin-width;
        ybinmax = ybin+width;
        xbinmin = ymin;
        xbinmax = ymin;
        S=0.
        for iproc,proc in enumerate(processes):
            try:
                nevt=store[il][(cat,proc)]
            except KeyError: nevt=0.
            S+= nevt
        for iproc,proc in enumerate(processes):
            #nevt=store[il][(cat,proc)]
            try:
                nevt=store[il][(cat,proc)]
            except KeyError: nevt=0.
            xbinmax += nevt
            pave = ROOT.TPave(xbinmin,ybinmin,xbinmax,ybinmax);
            pave.SetFillColor(colors[iproc]);
            print "* Drawing pave for sel",label,"proc",proc,"->",xbinmin,xbinmax,"y ->",ybinmin,ybinmax
            pave.Draw();
            pave.SetBorderSize(0);
            garbage.append(pave)
            ## 
            if nevt / S > .1:
                print "  - Drawing text"
                pvt = ROOT.TPaveText(xbinmin+0.5,ybinmin,xbinmin+maxSignal*.05,ybinmax);
                if il==0:
                    pvt.AddText("%.2f"%store[il][(cat,proc)])
                else: 
                    pvt.AddText("%.2f (%.1f%%)"%(store[il][(cat,proc)], (store[il][(cat,proc)]/store[0][(cat,proc)]-1.)*100))
                pvt.SetTextColor(0);
                pvt.SetFillStyle(0);
                pvt.SetFillColor(0);
                pvt.SetLineColor(0);
                pvt.SetBorderSize(0);
                pvt.SetTextAlign(13);
                pvt.SetTextFont(62);
                pvt.SetTextSize(0.03);
                pvt.Draw()
                garbage.append(pvt)
            ## update
            xbinmin += nevt
        #pavetext.Draw()
        #garbage.append(pavetext)
        #maxSignal=max(maxSignal,S)

    #dummy.GetXaxis().SetRangeUser(0,maxSignal*1.2)
    print "Max signal is",maxSignal

    ## legend
    tex_m=ROOT.TLatex();
    tex_m.SetNDC();
    tex_m.SetTextAlign(12);
    tex_m.SetTextSize(0.025);
    tex_m.SetLineWidth(2);

    ## Legend
    processName=[]
    for iproc, proc in enumerate(processes):
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

    c0.Modify()
    c0.Update()
raw_input("ok?")


