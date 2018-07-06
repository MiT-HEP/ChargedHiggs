import ROOT
import re 

from optparse import OptionParser
usage ='''Adjust plots and style of mass postfit plots'''
parser= OptionParser(usage=usage)
parser.add_option("-f","--fname",dest="fname",type='string',help="FileName [%default]",default="CMS-HMM_hmm_combcat_weighted.root");
parser.add_option("-n","--newlegend",dest="newleg",action='store_true',help="NewLegend [%default]",default=False);
parser.add_option("-s","--save",dest="save",action='store_true',help="Save [%default]",default=False);
parser.add_option("","--pas",dest="pas",action='store_true',help="PAS [%default]",default=False);

opts, args = parser.parse_args()

fname = opts.fname
#fname = "CMS-HMM_hmm_combcat_weighted.root"
#c1_n15

print "-> Opening file",fname
fIn = ROOT.TFile.Open(fname)

ROOT.TGaxis.SetMaxDigits(3)

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
            if "B component" not in title and 'cat' not in title: 
                pad . GetListOfPrimitives() . At(i) . SetTextFont(42)
            if 'cat' in title: 
                pad . GetListOfPrimitives() . At(i) . SetX(0.66)
            if "Preliminary" in title and not opts.pas: pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub('Preliminary','',title))
            title = pad . GetListOfPrimitives() . At(i) . GetTitle()
            #if "#hat{m}_{H}" in title: pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub('#hat{m}_{H}','m_{H}',title))
            if "#hat{m}_{H}" in title: 
                pad . GetListOfPrimitives() . At(i) . SetTitle("#hat{#mu}=0.7 for m_{H}=125 GeV")
                pad . GetListOfPrimitives() . At(i) . SetY(0.71)
            title = pad . GetListOfPrimitives() . At(i) . GetTitle()
            if "0.76" in title: pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub('0.76','0.7',title)) #it was 0.7
            if "TeV" in title and '13' in title: 
                pad . GetListOfPrimitives() . At(i) . SetTextAlign(31)
                pad . GetListOfPrimitives() . At(i) . SetX(0.95)
                pad . GetListOfPrimitives() . At(i) . SetY(0.92)
            if "CMS" in title:
                pad . GetListOfPrimitives() . At(i) . SetTextAlign(11) #was 11
                pad . GetListOfPrimitives() . At(i) . SetX(0.20) #was 0.17
                pad . GetListOfPrimitives() . At(i) . SetY(0.83) #was 0.92
            if "#mu#mu" in title or "m_{H}" in title:
                pad . GetListOfPrimitives() . At(i) . SetTextAlign(11)
                pad . GetListOfPrimitives() . At(i) . SetX(0.20)
                print("Y-coordinate Hmumu")
                print(pad . GetListOfPrimitives() . At(i) . GetY())
                if "#mu#mu" in title:
                    pad . GetListOfPrimitives() . At(i) . SetY(0.77)

            print "title is",title
            if re.search('hmm *cat', title):
                print "hmm cat in title"
                mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
                match=0
                for icat,new in enumerate(mCatToCat):
                    if match >0: break
                    catNum="%d"%icat
                    searchStr= "hmm *cat" + catNum +"}"
                    if re.search(searchStr,title): match+=1
                    pad . GetListOfPrimitives() . At(i) . SetTitle(re.sub(searchStr,"cat %d}"%mCatToCat[icat],title))
                    print "new title is",pad . GetListOfPrimitives() . At(i) . GetTitle()


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
            h.GetYaxis().SetNdivisions(7,5,5)
            h.GetYaxis().SetTitleOffset(0.8)
            h.GetYaxis().SetTitleSize(0.)
            h.GetYaxis().SetLabelSize(0.07)
            print "-> Set Style up using th1d,",pad . GetListOfPrimitives() . At(i) . GetName()
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TH1") and pad . GetListOfPrimitives() . At(i) . GetName() == "hdummy":
            h = pad . GetListOfPrimitives() . At(i)
            #h.SetX1NDC(0.1)
            h.GetXaxis().SetLabelSize(0.15)
            h.GetXaxis().SetTitleOffset(1.0)
            h.GetXaxis().SetTitleSize(0.15)
            h.GetYaxis().SetLabelSize(0.13)
            h.GetYaxis().SetNdivisions(4,5,0,1)
            h.SetFillColor(ROOT.kOrange)
            print "-> Set Style ratio "
        if pad . GetListOfPrimitives() . At(i) . InheritsFrom("TLegend") :
            l = pad . GetListOfPrimitives() . At(i) 
            l.SetX1NDC(.66)
            #l.SetY1NDC(.44)
            l.SetX2NDC(.99)
            #l.SetY2NDC(.73)
            #h2s = ROOT.TH1D("d2s","d2s",1,0,1)
            #h2s.SetFillColor(ROOT.kYellow)
            #h2s.SetLineColor(ROOT.kYellow)
            #h1s = ROOT.TH1D("d1s","d1s",1,0,1)
            #h1s.SetFillColor(ROOT.kGreen)
            #h1s.SetLineColor(ROOT.kGreen)
            l.AddEntry(canv.FindObject("pad2").GetListOfPrimitives().At(3),"68% CL","F") ## 0 = Frame, 1 Dummy
            l.AddEntry(canv.FindObject("pad2").GetListOfPrimitives().At(2),"95% CL","F")
            print "-> Added Leg Entries"
        if pad . GetListOfPrimitives(). At(i) . InheritsFrom("TGraph") : #and pad . GetListOfPrimitives(). At(i) . Get(0x28de450) : #Get(0x28dcd90) : 
            graph = pad.GetListOfPrimitives() . At(i)
            print(graph.GetFillColor())
            if (graph.GetFillColor() == 400):  
              graph.SetFillColor(ROOT.kOrange)
            if (graph.GetFillColor() == 416):
              graph.SetFillColor(ROOT.kGreen+1)
            ## loop over entries in the legend
            #for ile in range(0,l.GetListOfPrimitives().GetSize()):
            #    le = l.GetListOfPrimitives().At(ile)
            #    label = le.GetLabel()
            #    print "label is",label
            #    if "hmm cat" in label:
            #        mCatToCat = [0,4,7,1,10,6,9,8,5,3,2,13,11,12,14] ## cat in position 
            #        match=0
            #        for icat in enumerate(mCatToCat):
            #             match+=1
            #             le . SetLabel (re.sub("hmm cat%d"%icat,"cat %d"%mCatToCat[icat],title))
            #        if match >1:
            #            raise ValueError("multiple match")

setStyle(canv)
    
newlabel = ROOT.TText(0.07,0.15,"S/(S+B) Weighted Events / 0.5 GeV");
#newlabel.SetFillColor(18)
newlabel.SetTextFont(42)
#newlabel.AddText("newlabel")
newlabel.SetTextSize(0.05)
newlabel.SetTextAngle(90)
newlabel.Draw()
 
print "-> Remove X/Y Tick"
canv.SetTickx(0)
canv.FindObject("pad1").SetTickx(0)
canv.SetTicky(0)
canv.FindObject("pad1").SetTicky(0)
canv.FindObject("pad2").SetTicky(0)

canv.FindObject("pad1").SetLeftMargin(0.15)
canv.FindObject("pad2").SetLeftMargin(0.15)
canv.FindObject("pad1").SetRightMargin(0.05)
canv.FindObject("pad2").SetRightMargin(0.05)
canv.FindObject("pad2").SetBottomMargin(0.35)

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
    xmin = 0.56 #was 0.61
    ymax = .72
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
    oneSigma.SetFillColor(ROOT.kGreen+1)
    twoSigma = ROOT.TPave(xmin-delta*2/3.,y_b-delta/2.,xmin+delta*2/3.,y_b+delta/2.,0,"NDC")
    twoSigma.SetFillColor(ROOT.kOrange)
    obj . extend([b,oneSigma,twoSigma])
    twoSigma.Draw()
    oneSigma.Draw()
    b.Draw("SAME")
    ltx.DrawLatex(xmin +textSep,y_b,"B component #pm 1 s.d. (2 s.d.)")


       

canv.SetCanvasSize(700,500)

#void SetPad(Double_t xlow, Double_t ylow, Double_t xup, Double_t yup)
#print "PAD1 (Y H)",canv.FindObject("pad1").GetYlowNDC(),canv.FindObject("pad1").GetHNDC()
#print "PAD2 (Y H)",canv.FindObject("pad2").GetYlowNDC(),canv.FindObject("pad2").GetHNDC()
#GetHNDC
#print "PAD1 (BTLR)",canv.FindObject("pad1").GetBottomMargin(),canv.FindObject("pad1").GetTopMargin(),canv.FindObject("pad1").GetLeftMargin(),canv.FindObject("pad1").GetRightMargin()
#print "PAD2 (BTLR)",canv.FindObject("pad2").GetBottomMargin(),canv.FindObject("pad2").GetTopMargin(),canv.FindObject("pad2").GetLeftMargin(),canv.FindObject("pad2").GetRightMargin()
#PAD1 (BTLR) 0.18 0.10000000149 0.15000000596 0.0500000007451
#PAD2 (BTLR) 0.25 9.99999974738e-06 0.15000000596 0.0500000007451
#canv.SetBottomMargin(0.05)
#canv.SetLeftMargin(0.15)
#canv.SetTopMargin(0.10)
#canv.SetRightMargin(0.05)
#canv.FindObject("pad1").SetTopMargin(0.10/0.75)


canv.Modified()
canv.Update()

if opts.save:
    base=""
    if opts.pas: base="Preliminary/"
    canv.SaveAs(base+re.sub('.root','.pdf',fname))
    canv.SaveAs(base+re.sub('.root','.png',fname))
else:
    raw_input("ok?")
