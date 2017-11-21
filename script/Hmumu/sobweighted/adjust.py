import ROOT
import re 

from optparse import OptionParser
usage ='''Adjust plots and style of mass postfit plots'''
parser= OptionParser(usage=usage)
parser.add_option("-f","--fname",dest="fname",type='string',help="FileName [%default]",default="CMS-HMM_hmm_combcat_weighted.root");
parser.add_option("-n","--newlegend",dest="newleg",action='store_true',help="NewLegend [%default]",default=False);
parser.add_option("-s","--save",dest="save",action='store_true',help="Save [%default]",default=False);

opts, args = parser.parse_args()

fname = opts.fname
#fname = "CMS-HMM_hmm_combcat_weighted.root"
#c1_n15

print "-> Opening file",fname
fIn = ROOT.TFile.Open(fname)

cname = "c1_n18"
for ik in range(0,fIn.GetListOfKeys().GetSize()):
    #print " considering key",fIn.GetListOfKeys()[ik].GetName()
    o = fIn.Get( fIn.GetListOfKeys()[ik].GetName() )
    if o.InheritsFrom("TCanvas"):
        cname = fIn.GetListOfKeys()[ik].GetName()
        print "-> Changing cname to",cname

canv = fIn.Get(cname)

print "-> Drawing"
canv.Draw()

print "->removing Preliminary"
## Loop Over and remove Preliminary
def removePreliminary(pad):
    for i in range(0, pad.GetListOfPrimitives().GetSize()):
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TPad"): removePreliminary(pad . GetListOfPrimitives() . At(i))
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TText"):
            title = pad . GetListOfPrimitives() . At(i) . GetTitle()
            if "Preliminary" in title: pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub('Preliminary','',title))
            if "#hat{m}_{H}" in title: pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub('#hat{m}_{H}','m_{H}',title))
removePreliminary(canv)

## def legLoop(leg):
##     for il in range(0,leg.GetListOfPrimitives().GetSize() ):
##         o = leg.GetListOfPrimitives().At(il)
##         if "B component" in o.GetLabel() :
##             o.SetObject(None)

print "-> SetTitle ecc"
def setStyle(pad):
    for i in range(0, pad.GetListOfPrimitives().GetSize()):
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TPad"): setStyle(pad . GetListOfPrimitives() . At(i))
        #if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TH1D") and pad . GetListOfPrimitives() . At(i) . GetName() == "frame_cc50080":
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TH1D") and 'frame_' in pad . GetListOfPrimitives() . At(i) . GetName():
            h = pad . GetListOfPrimitives() . At(i)
            h.GetYaxis().SetTitleOffset(0.8)
            h.GetYaxis().SetTitleSize(0.05)
            print "-> Set Style up using th1d,",pad . GetListOfPrimitives() . At(i) . GetName()
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TH1") and pad . GetListOfPrimitives() . At(i) . GetName() == "hdummy":
            h = pad . GetListOfPrimitives() . At(i)
            h.GetXaxis().SetLabelSize(0.1)
            h.GetXaxis().SetTitleOffset(0.9)
            print "-> Set Style ratio "
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TLegend") :
            l = pad . GetListOfPrimitives() . At(i) 
            #h2s = ROOT.TH1D("d2s","d2s",1,0,1)
            #h2s.SetFillColor(ROOT.kYellow)
            #h2s.SetLineColor(ROOT.kYellow)
            #h1s = ROOT.TH1D("d1s","d1s",1,0,1)
            #h1s.SetFillColor(ROOT.kGreen)
            #h1s.SetLineColor(ROOT.kGreen)
            l.AddEntry(canv.FindObject("pad2").GetListOfPrimitives().At(3),"#pm 1#sigma","F") ## 0 = Frame, 1 Dummy
            l.AddEntry(canv.FindObject("pad2").GetListOfPrimitives().At(2),"#pm 2#sigma","F")
            print "-> Added Leg Entries"

setStyle(canv)

obj=[]
if opts.newleg:  #new legend version
    print "-> Adding NEW Legend"
    canv.FindObject("TPave").Delete()
    canv.FindObject("pad1").cd()
    ltx = ROOT.TLatex()
    obj.append(ltx)
    ltx . SetNDC()
    ltx . SetTextSize(0.05)
    ltx . SetTextFont(42)
    ltx . SetTextAlign(12)
    xmin = 0.56
    ymax = .75
    textSep = 0.05
    delta = 0.045
    entryDelta = 0.07

    dataPoint =  ROOT.TMarker(xmin,ymax,20)
    dataPoint.SetMarkerColor(ROOT.kBlack)
    dataPoint.SetNDC()
    dataLine =  ROOT.TLine(xmin, ymax-delta/2. ,xmin, ymax + delta/2.)
    dataLine.SetNDC()
    dataLine.SetLineColor(ROOT.kBlack)
    dataLine.SetLineWidth(1)
    dataLine2 =  ROOT.TLine(xmin-0.003, ymax-delta/2. ,xmin+0.003, ymax - delta/2.)
    dataLine2.SetNDC()
    dataLine3 =  ROOT.TLine(xmin-0.003, ymax+delta/2. ,xmin+0.003, ymax + delta/2.)
    dataLine3.SetNDC()
    obj += [dataPoint,dataLine,dataLine2,dataLine3]
    ## Draw data
    dataPoint.Draw("SAME")
    dataLine.Draw("SAME")
    dataLine2.Draw("SAME")
    dataLine3.Draw("SAME")
    ltx.DrawLatex(xmin+ textSep,ymax,"Data")

    ## S+B Fit: Red 2
    y_sb = ymax - entryDelta
    sb = ROOT.TLine(xmin,y_sb -delta/2., xmin,y_sb+delta/2.)
    sb.SetNDC()
    sb.SetLineColor(ROOT.kRed)
    sb.SetLineWidth(3)
    obj . extend([sb])
    sb.Draw("SAME")
    ltx.DrawLatex(xmin +textSep,y_sb,"S+B fit")
    ## B Fit: Red 2, dashed + 1s +2s
    y_b = ymax - 2*entryDelta
    b = ROOT.TLine(xmin,y_b -delta/2., xmin,y_b+delta/2.)
    b.SetNDC()
    b.SetLineColor(ROOT.kRed)
    b.SetLineStyle(2)
    b.SetLineWidth(3)

    oneSigma = ROOT.TPave(xmin-delta/3.,y_b-delta/2.,xmin+delta/3.,y_b+delta/2.,0,"NDC")
    oneSigma.SetFillColor(ROOT.kGreen)
    twoSigma = ROOT.TPave(xmin-delta*2/3.,y_b-delta/2.,xmin+delta*2/3.,y_b+delta/2.,0,"NDC")
    twoSigma.SetFillColor(ROOT.kYellow)
    obj . extend([b,oneSigma,twoSigma])
    twoSigma.Draw()
    oneSigma.Draw()
    b.Draw("SAME")
    ltx.DrawLatex(xmin +textSep,y_b,"B component #pm 1#sigma (2#sigma)")

canv.Modified()
canv.Update()
raw_input("ok?")

if opts.save:
    canv.SaveAs(re.sub('.root','.pdf',fname))
    canv.SaveAs(re.sub('.root','.png',fname))
