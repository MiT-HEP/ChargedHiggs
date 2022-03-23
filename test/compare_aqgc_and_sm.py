import ROOT
import os,sys

#TODO

#fname_nominal="Datacards/inputs/SEP22/HADSR_2018_sep22.root"
#fname_aqgc="Datacards/inputs/SEP22/HADSR_2018_sep22aqgc.root"

path="/eos/user/d/dalfonso/AnalysisVBS/NANO/FEB20"
fname_nominal=path+"/UL2016/HAD/HAD.root"
fname_aqgc=path+"aQGC/UL2016/HAD/HAD.root"


var="MVV"
cat="BB"

print "> nominal file",fname_nominal
print "> aqgc file",fname_aqgc
print "> var",var,"cat",cat

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

# available samples to match
# MVV_BB_WPMJJWPMJJjj_EWK_LO;1        
# MVV_BB_aQGC_WMJJWMJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   
# MVV_BB_aQGC_WPJJWPJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   

# MVV_BB_aQGC_WPJJWMJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   
# MVV_BB_WPJJWMJJjj_EWK_LO;1      

# MVV_BB_ZJJZJJjj_EWK_LO;1            * 
# MVV_BB_aQGC_ZJJZJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1       MVV 

# TODO -> MERGE
# MVV_BB_ZJJNOBWPMJJjj_EWK_LO;1       1
# MVV_BB_ZBBWPMJJjj_EWK_LO;1        1
# MVV_BB_aQGC_WMJJZJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1      MVV 
# MVV_BB_aQGC_WPJJZJJjj_EWK_LO_NPle1_AQGC_fm0_0p00;1      MVV 

# partial match?
# MVV_BB_ZNUNUZJJjj_EWK_LO;1       -
# MVV_BB_aQGC_ZNUNUZJJNOBjj_EWK_LO_NPle1_AQGC_fm0_0p00;1  MVV 

# aQGC EXTRA?
# MVV_BB_aQGC_ZBBZJJNOBjj_EWK_LO_NPle1_AQGC_fm0_0p00;1    MVV 

############################################ LEP #########################################
# MVV_BB_WMLEPWMHADjj_EWK_LO;1      
# MVV_BB_aQGC_WMLEPWMHADjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   MVV 

# MVV_BB_WPLEPWPHADjj_EWK_LO;1     
# MVV_BB_aQGC_WPLEPWPHADjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   MVV 

# MVV_BB_WMLEPZHADjj_EWK_LO;1      
# MVV_BB_aQGC_WMLEPZHADjj_EWK_LO_NPle1_AQGC_fm0_0p00;1    MVV 

# MVV_BB_WPLEPZHADjj_EWK_LO;1      
# MVV_BB_aQGC_WPLEPZHADjj_EWK_LO_NPle1_AQGC_fm0_0p00;1    MVV 

## aqgC EXTRA?
# MVV_BB_aQGC_WPHADWMLEPjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   MVV 
# MVV_BB_aQGC_WPLEPWMHADjj_EWK_LO_NPle1_AQGC_fm0_0p00;1   MVV 


signals=["WPJJWMJJjj_EWK_LO","WPMJJWPMJJjj_EWK_LO","ZJJZJJjj_EWK_LO",'WMLEPWMHADjj_EWK_LO','WPLEPWPHADjj_EWK_LO','WMLEPZHADjj_EWK_LO','WPLEPZHADjj_EWK_LO'] 
#signals=["ZJJZJJjj_EWK_LO"]

fIn=ROOT.TFile.Open(fname_nominal)
fInAQGC=ROOT.TFile.Open(fname_aqgc)

for sig in signals:
    hname = "VBShadAnalysis/"+var+"_"+cat+"_"+sig
    h= fIn.Get(hname)
    h_aqgc=[]
    sig_aqgc=None
    if sig == "WPMJJWPMJJjj_EWK_LO":  sig_aqgc = ["WMJJWMJJjj_EWK_LO","WPJJWPJJjj_EWK_LO"]

    for par in aqgc_parameters:
        hname2 = "VBShadAnalysis/"+var+"_"+cat+"_"+'aQGC_'+sig +"_NPle1_AQGC_"+par+"_0p00"
        if sig_aqgc:
            print "Summing ", sig_aqgc, "for",sig
            htmp=None
            for s in sig_aqgc:
                hname2 = "VBShadAnalysis/"+var+"_"+cat+"_"+'aQGC_'+s +"_NPle1_AQGC_"+par+"_0p00"
                if htmp == None:
                    htmp= fInAQGC.Get(hname2)
                    if htmp==None: print "Unable to get", hname2, "from", fname_aqgc
                else:
                    htmp2 = fInAQGC.Get(hname2)
                    if htmp2==None: print "Unable to get", hname2, "from", fname_aqgc
                    htmp.Add(htmp2)
        else:
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



