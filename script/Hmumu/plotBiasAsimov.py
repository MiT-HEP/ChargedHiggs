import sys,os
from optparse import OptionParser, OptionGroup
usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-f","--file" ,dest='file',type='string',help="Input MLFIT file [Default=%default]",default="mlfit.root")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory [Default=%default]",default="shapes_fit_s/ch1_cat8")
parser.add_option("","--data" ,dest='data',type='string',help="File:workspace:hist [Default=%default]",default=None)
parser.add_option("","--unblind" ,dest='blind',action='store_false',help="Blind data [Default=%default]",default=True)
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fIn=ROOT.TFile.Open(opts.file)

r=fIn.Get(opts.dir+"/data").Clone("r")
BKG=fIn.Get(opts.dir+"/BKG").Clone("BKG")

data=None
if opts.data != None and len(opts.data.split(':'))>2:
    fData=ROOT.TFile.Open(opts.data.split(':')[0])
    w= fData.Get(opts.data.split(':')[1])
    data_roo=w.data(opts.data.split(':')[2])
    data=data_roo.createHistogram("mmm")
    data.Rebin(5)
    for i in range(0,data.GetNbinsX()):
        x=data.GetBinCenter(i)
        ydata=data.GetBinContent(i)
        ybkg=BKG.GetBinContent(BKG.FindBin(data.GetBinCenter(i)))
        w= data.GetBinWidth(i)
        if ybkg == 0 : continue

        if x > 120 and x < 130 and opts.blind: 
            print "Blinding bin",i
            data.SetBinContent(i,1)
            data.SetBinError(i,0)
        else:
            #print "Center=",data.GetBinCenter(i),"Content=",data.GetBinContent(i),"BKG=",BKG.GetBinContent(BKG.FindBin(data.GetBinCenter(i)))
            data.SetBinContent(i, ydata/ (ybkg*w)) 
            data.SetBinError(i, data.GetBinError(i) / (ybkg*w)) 

    data.SetMarkerStyle(20)
    data.SetMarkerColor(ROOT.kGreen-4)
    data.SetLineColor(ROOT.kGreen-4)


for i in range(0,r.GetN()):
    r.GetY()[i] = r.GetY()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;
    r.GetEYhigh()[i] = r.GetEYhigh()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;
    r.GetEYlow()[i] = r.GetEYlow()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;

c=ROOT.TCanvas("c","c",800,800)
c.SetTopMargin(0.05)
c.SetBottomMargin(0.15)
c.SetRightMargin(0.05)
c.SetLeftMargin(0.15)

r.Draw("APE")
r.GetXaxis().SetRangeUser(110,150)
r.GetYaxis().SetRangeUser(.9,1.2)
r.GetXaxis().SetTitle("m_{#mu#mu} [GeV]")
r.GetXaxis().SetTitleOffset(1.4)
r.GetYaxis().SetTitle("Test function / fit function")
r.GetYaxis().SetTitleOffset(1.4)

rb=fIn.Get(opts.dir+"/BKG").Clone("rb")
for i in range(1,rb.GetNbinsX()+1):
    rb.SetBinContent(i, rb.GetBinContent(i)/BKG.GetBinContent(i));
    rb.SetBinError(i, rb.GetBinError(i)/BKG.GetBinContent(i));

rb.SetFillColor(ROOT.kRed)
rb.SetFillStyle(3001)
rb.Draw("E2 SAME")

# redraw only the points, not the errors
r2=r.Clone("r2")
for i in range(1,r.GetN()):
    r2.GetEYhigh()[i] = 0.0 
    r2.GetEYlow()[i] = 0.0
r2.Draw("P SAME")

tree=fIn.Get("tree_fit_sb")
tree.Scan("mu:muErr")
tree.Draw("mu","","goff")

rs = fIn.Get(opts.dir+"/total_signal").Clone("rs")
for i in range(1,rs.GetNbinsX()+1):
    rs.SetBinContent(i, 1+rs.GetBinContent(i)/tree.GetV1()[0]/BKG.GetBinContent(i));
rs.Draw("HIST SAME")

if data != None:
    data.Draw("PE SAME")

ltx=ROOT.TLatex()
ltx.SetNDC()
ltx.SetTextFont(43)
ltx.SetTextSize(28)
ltx.SetTextAlign(13)
ltx.DrawLatex(.18,.93,"#bf{CMS} #it{#scale[0.8]{Simulation Preliminary}}")

leg=ROOT.TLegend(.7,.7,.9,.9)
leg.AddEntry(r,"Asimov Dataset","PE")
leg.AddEntry(rb,"Background fit","F")
leg.AddEntry(rs,"Signal (pre-fit)","L")

if data != None:
    leg.AddEntry(data,"Data","PE")

leg.Draw()

c.Modify()
c.Update()

raw_input("ok?")
