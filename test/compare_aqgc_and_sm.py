import ROOT
import os,sys


fname_nominal="Datacards/inputs/HADSR_2018_sep12.root"
fname_aqgc="Datacards/inputs/HADSR_2018_sep12aqgc.root"
var="MVV"
cat="BB"


if True:
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
    from ParseDat import aqgc_names
    aqgc_parameters = list(set([ x.split('_')[0] for x in aqgc_names ]))


#signals=["WPMJJWPMJJjj_EWK_LO","WPJJWMJJjj_EWK_LO","ZJJZJJjj_EWK_LO"]
signals=["ZJJZJJjj_EWK_LO"]

fIn=ROOT.TFile.Open(fname_nominal)
fInAQGC=ROOT.TFile.Open(fname_aqgc)

for sig in signals:
    hname = "VBShadAnalysis/"+var+"_"+cat+"_"+sig
    h= fIn.Get(hname)
    h_aqgc=[]
    for par in aqgc_parameters:
        hname2 = "VBShadAnalysis/"+var+"_"+cat+"_"+sig +"_NPle1_aQGC_AQGC_"+par+"_0p00"
        htmp= fInAQGC.Get(hname2)
        if htmp==None: print "Unable to get", hname2, "from", fname_aqgc
        h_aqgc.append((par,htmp))
    
    c=ROOT.TCanvas("c","c",800,800)
    c.SetBottomMargin(0.15)
    c.SetLeftMargin(0.15)
    c.SetTopMargin(0.10)
    c.SetRightMargin(0.05)
    
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetOptStat(0)
    rebin=10

    h.SetLineColor(ROOT.kBlack)
    h.SetLineWidth(3)

    h.Draw("HIST")
    h.Draw("AXIS X+ Y+ SAME")
    h.Rebin(rebin)
    h.GetXaxis().SetTitle(var)

    for p,h2 in h_aqgc:
        if 'ft' in par : 
            h2.SetLineColor(ROOT.kBlue+4)
            h2.SetLineStyle(7)
        elif 'fm' in par : 
            h2.SetLineColor(ROOT.kGreen+4)
            h2.SetLineStyle(2)
        elif 'fs' in par : 
            h2.SetLineColor(ROOT.kOrange)
            h2.SetLineStyle(3)
        else:
            print "Unknown parameter",p
        h2.SetLineWidth(2)
        h2.Draw("HIST SAME")
        h2.Rebin(rebin)
    
    l = ROOT.TLatex()
    l.SetNDC()
    l.SetTextSize(0.06)
    l.SetTextFont(42)
    l.SetTextAlign(13) #inside
    xcms,ycms= 0.18,.88 # inside
    l.DrawLatex(xcms,ycms,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
    l.SetTextSize(0.03)
    l.DrawLatex(xcms,ycms-0.05,"Cat: "+ cat +" #minus "+ sig)

    c.SaveAs("plot_aqgc/"+var+"_"+cat+"_"+sig+"_SM_validation.pdf" )
    c.SaveAs("plot_aqgc/"+var+"_"+cat+"_"+sig+"_SM_validation.png" )



