import sys,array,copy,math
import ROOT
ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

class SystematicSmoother:

    # rebins histogram such that at least one entry is present in each bin
    hNomOrig = None
    hNom = None     # original nominal
    hUp = None      # original up --> will be overwritten by smoothed value
    hDown = None    # original down --> will be overwritten by smoothed value

    rUp = None
    rDown = None

    rUpSmoothed = None
    rDownSmoothed = None

    saveDir = None
    name = ""

    maxStat = 0.15

    binMin = 0
    binMax = 0
    firstNonZeroBin = 1

    nameUp = ""
    nameDown = ""

    # aux vars
    h = None  # tmp var needed for rebin functions (switch between up/down)

    def __init__(self, hNom_, hUp_, hDown_, saveDir_ = None):

        ROOT.gErrorIgnoreLevel = ROOT.kWarning

        self.nameUp = hUp_.GetName()
        self.nameDown = hDown_.GetName()

        self.hNomOrig = hNom_.Clone("hNomTmp") # used to re-calculate the absolute variations (unbinned)
        self.hNom = hNom_.Clone("hNom") # rebin on this
        self.hUp = hUp_.Clone("hUp") # left unbinned
        self.hDown = hDown_.Clone("hDown") # left unbinned



        self.saveDir = saveDir_ # if !none: write plot of ratios to this folder
        self.name = hUp_.GetName().replace("up", "").replace("Up", "").replace("/", "_")

        hUp_.Delete()
        hDown_.Delete()

        # make ratios (original)
        self.rUp = self.hUp.Clone("up")
        self.rDown = self.hDown.Clone("down")
        self.rUp.Divide(self.hNom)
        self.rDown.Divide(self.hNom)

        # save min/max bins (needed for rebinning)
        self.binMin = float(self.hNom.GetBinLowEdge(1)) # min bin
        self.binMax = float(self.hNom.GetBinLowEdge(self.hNom.GetNbinsX()+1)) # max bin



    def smooth(self):

        self.rebin()

        # new histograms containing the smoothed ratios --> after rebinning
        self.rUpSmoothed = self.rUp.Clone("upSmoothed")
        self.rDownSmoothed = self.rDown.Clone("downSmoothed")

        for i in range(1, self.firstNonZeroBin):
            self.rUpSmoothed.SetBinContent(i, 1)
            self.rDownSmoothed.SetBinContent(i, 1)

        #self.smoothGaussianKernel() # smooth it
        #self.smoothFit()
        #self.smoothLowess()
        self.smoothFit()
        self.plot() # plot it

        # construct the absolute up/down variations
        for i in range(1, self.hNomOrig.GetNbinsX()+1):

            #self.hUp.SetBinContent(i, self.hNomOrig.GetBinContent(i)*self.rUpSmoothed.GetBinContent(i))
            #self.hDown.SetBinContent(i, self.hNomOrig.GetBinContent(i)*self.rDownSmoothed.GetBinContent(i))

            self.hUp.SetBinContent(i, self.hNomOrig.GetBinContent(i)*self.rUpSmoothed.GetBinContent(self.rUpSmoothed.FindBin(self.hNomOrig.GetBinCenter(i))))
            self.hDown.SetBinContent(i, self.hNomOrig.GetBinContent(i)*self.rDownSmoothed.GetBinContent(self.rDownSmoothed.FindBin(self.hNomOrig.GetBinCenter(i))))



        self.hUp.SetName(self.nameUp)
        self.hDown.SetName(self.nameDown)

        return self.hUp, self.hDown



    def smoothLowess(self):

        grUp = ROOT.TGraphErrors(self.rUp.GetNbinsX())
        grDown = ROOT.TGraphErrors(self.rDown.GetNbinsX())

        for i in range(1, self.rUp.GetNbinsX()+1):

            #grUp.SetPoint(i, self.rUp.GetBinCenter(i), self.rUp.GetBinContent(i))
            #grDown.SetPoint(i, self.rDown.GetBinCenter(i), self.rDown.GetBinContent(i))

            # equidistant points
            grUp.SetPoint(i, i, self.rUp.GetBinContent(i))
            grDown.SetPoint(i, i, self.rDown.GetBinContent(i))


        tUp = ROOT.TGraphSmooth()
        grUpSmoothed = tUp.SmoothLowess(grUp, "" , 0.2)

        tDown = ROOT.TGraphSmooth()
        grDownSmoothed = tDown.SmoothLowess(grDown, "" , 0.2)



        smUp = grUpSmoothed.GetY()
        smDown = grDownSmoothed.GetY()

        for i, val in enumerate(smUp): self.rUpSmoothed.SetBinContent(i, val)
        for i, val in enumerate(smDown): self.rDownSmoothed.SetBinContent(i, val)





    def smoothFit(self):

        print "[SystematicSmoother] fit smoothing"

        if self.rUp.GetNbinsX() < 5: return

        prev=0.
        func=None
        for n  in range(1,6):

            myfunc=None
            if n==1: myfunc=ROOT.TF1("myfunc1","[0]+x*[1]",0, self.rUp.GetNbinsX())
            elif n==2:myfunc=ROOT.TF1("myfunc2","[0]+x*[1]+x*x*[2]",0, self.rUp.GetNbinsX())
            elif n==3:myfunc=ROOT.TF1("myfunc3","[0]+x*[1]+x*x*[2]+[3]*x*x*x",0, self.rUp.GetNbinsX())
            elif n==4:myfunc=ROOT.TF1("myfunc4","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x",0, self.rUp.GetNbinsX())
            elif n==5:myfunc=ROOT.TF1("myfunc5","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x",0, self.rUp.GetNbinsX())
            self.rUp.Fit("myfunc%d"%n,"WQN")
            self.rUp.Fit("myfunc%d"%n,"WQNM")
            chi2=myfunc.GetChisquare()
            ftest= (prev-chi2)/1. / (chi2/( self.rUp.GetNbinsX()-n))
            prob = 1.-ROOT.TMath.FDistI(ftest,1, self.rUp.GetNbinsX()-n)
            prev=chi2
            if n==5: func=myfunc
            if n>1:print "-> fTest prob for degree",n,"prob=",prob
            if (prob >0.02 and n > 1) or n==5: # was 0.02
                # exit strategy
                print "-> fTest choose degree",n,"for up variation","prob=",prob
                for ibin in range(1, self.rUp.GetNbinsX()+1):
                    self.rUpSmoothed.SetBinContent(ibin, func.Eval(self.rUp.GetBinCenter(ibin)))
                break
            func=myfunc

        prev=0.
        func=None
        for n  in range(1,6):

            myfunc=None
            if n==1: myfunc=ROOT.TF1("myfunc1","[0]+x*[1]",0, self.rDown.GetNbinsX())
            elif n==2:myfunc=ROOT.TF1("myfunc2","[0]+x*[1]+x*x*[2]",0, self.rDown.GetNbinsX())
            elif n==3:myfunc=ROOT.TF1("myfunc3","[0]+x*[1]+x*x*[2]+[3]*x*x*x",0, self.rDown.GetNbinsX())
            elif n==4:myfunc=ROOT.TF1("myfunc4","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x",0, self.rDown.GetNbinsX())
            elif n==5:myfunc=ROOT.TF1("myfunc5","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x",0, self.rDown.GetNbinsX())
            self.rDown.Fit("myfunc%d"%n,"WQN")
            self.rDown.Fit("myfunc%d"%n,"WQNM")
            chi2=myfunc.GetChisquare()
            ftest= (prev-chi2)/1. / (chi2/( self.rDown.GetNbinsX()-n))
            prob = 1.-ROOT.TMath.FDistI(ftest,1, self.rDown.GetNbinsX()-n)
            prev=chi2
            if n==5: func=myfunc
            if n>1:print "-> fTest prob for degree",n,"prob=",prob
            if (prob >0.02 and n > 1) or n==5:
                # exit strategy
                print "-> fTest choose degree",n,"for up variation","prob=",prob
                for ibin in range(1, self.rDown.GetNbinsX()+1):
                    self.rDownSmoothed.SetBinContent(ibin, func.Eval(self.rDown.GetBinCenter(ibin)))
                break
            func=myfunc


    def smoothGaussianKernel(self):

        # linear adaptive smoothing: sigma = ax + b
        sigma = 50

        print "[SystematicSmoother] linear adaptive Gaussian kernel smoothing with sigma = %d" % (sigma)



        for i in range(self.firstNonZeroBin, self.rUp.GetNbinsX()+1):

            numUp = 0.0
            numDn = 0.0
            denUp = 0.0
            denDown = 0.0

            for j in range(self.firstNonZeroBin, self.rUp.GetNbinsX()+1):

                weightUp = math.exp( -(i-j)**2/(2.*sigma))
                weightDown = math.exp( -(i-j)**2/(2.*sigma))
                #print i, j, weightUp,

                denUp += weightUp
                denDown += weightDown
                numUp += self.rUp.GetBinContent(j)*weightUp
                numDn += self.rDown.GetBinContent(j)*weightDown

            self.rUpSmoothed.SetBinContent(i, numUp/denUp)
            self.rDownSmoothed.SetBinContent(i, numDn/denDown)
            #sys.exit()


    def rebin(self):

        # rebin the nominal histo such that enough statistics is in the bin

        self.h = self.hNom
        b = self.h.GetNbinsX()
        pos = self.mergeCriteria()
        while pos != -1:

            if pos == 1 and self.h.GetNbinsX() == 1: break # in case of need to rebin in one big bin
            if pos == self.h.GetNbinsX(): self.mergeBins(pos) # if uttermost right bin: merge pos and pos-1
            else: self.mergeBins(pos+1) # merge pos and pos+1
            pos = self.mergeCriteria() # update



        a = self.h.GetNbinsX()
        print "[SystematicSmoother] rebin nominal histogram from %d bins to %d bins" % (b, a)

        self.hNom = self.h




        # rebin the ratio histograms --> make them again
        mybins = array.array('d', self.getBinArray())
        print mybins
        #self.hUp = self.hUp.Rebin(len(mybins)-1, self.hUp.GetName(), mybins)
        #self.hDown = self.hDown.Rebin(len(mybins)-1, self.hDown.GetName(), mybins)


        # make ratios (original)
        self.rUp = self.hUp.Clone("up").Rebin(len(mybins)-1, "up", mybins)
        self.rDown = self.hDown.Clone("down").Rebin(len(mybins)-1, "down", mybins)
        self.rUp.Divide(self.hNom)
        self.rDown.Divide(self.hNom)
        return


        self.hNom = self.hNom.Rebin(160)
        self.h = self.hNom
        self.rUp = self.hUp.Clone("up").Rebin(160)
        self.rDown = self.hDown.Clone("down").Rebin(160)
        self.rUp.Divide(self.hNom)
        self.rDown.Divide(self.hNom)





    def plot(self):

        if self.saveDir == None: return


        self.rUp.SetLineWidth(0)
        self.rUp.SetLineColor(ROOT.kRed)
        self.rUp.SetLineColorAlpha(ROOT.kRed, 0.4)
        self.rUp.SetMarkerColor(ROOT.kRed)
        self.rUp.SetMarkerStyle(20)
        self.rUp.SetMarkerSize(0.6)

        self.rDown.SetLineWidth(0)
        self.rDown.SetLineColor(ROOT.kBlue)
        self.rDown.SetLineColorAlpha(ROOT.kBlue, 0.4)
        self.rDown.SetMarkerColor(ROOT.kBlue)
        self.rDown.SetMarkerStyle(20)
        self.rDown.SetMarkerSize(0.6)

        self.rUp.GetYaxis().SetRangeUser(0.7, 1.3)
#        self.rUp.GetYaxis().SetRangeUser(0.5, 1.5)
        #self.rUp.GetYaxis().SetRangeUser(0.1, 5)
        self.rUp.GetXaxis().SetRangeUser(self.binMin, self.binMax)

        c = ROOT.TCanvas("c", "c", 800, 800)
        c.SetGrid()

        leg = ROOT.TLegend(0.6, 0.8, 0.9, 0.9)
        leg.SetBorderSize(1)
        leg.SetFillStyle(1001) #1001
        leg.SetFillColor(0)

        legUp = "up (#Delta = %.4f %%)" % (100.*abs(self.rUpSmoothed.Integral() - self.rUp.Integral())/self.rUp.Integral())
        legDown = "down (#Delta = %.4f %%)" % (100.*abs(self.rDownSmoothed.Integral() - self.rDown.Integral())/self.rDown.Integral())

        leg.AddEntry(self.rUpSmoothed, legUp, "L")
        leg.AddEntry(self.rDownSmoothed, legDown, "l")



        self.rUp.Draw("P")
        self.rDown.Draw("SAME P")


        self.rUpSmoothed.SetLineWidth(2)
        self.rUpSmoothed.SetLineColor(ROOT.kRed)
        self.rUpSmoothed.Draw("SAME HIST")

        self.rDownSmoothed.SetLineWidth(2)
        self.rDownSmoothed.SetLineColor(ROOT.kBlue)
        self.rDownSmoothed.Draw("SAME HIST")


        c.SetTopMargin(.06)
        c.SetRightMargin(0.05)
        #c.SetBottomMargin(0.0)
        c.SetLeftMargin(0.12)


        # top text left
        left = ROOT.TLatex()
        left.SetNDC()
        left.SetTextFont(43)
        left.SetTextSize(26)
        left.SetTextAlign(11)
        #left.DrawLatex(.12,.95, "")

        # top text right
        right = ROOT.TLatex()
        right.SetNDC()
        right.SetTextFont(43)
        right.SetTextSize(26)
        right.SetTextAlign(31)
        right.DrawLatex(.95,.95, self.name)


        # CMS preliminary
        right.SetTextSize(30)
        right.SetTextAlign(13)
        #right.DrawLatex(.13,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")




        self.rUp.GetYaxis().SetTitle("Variation / nominal")
        self.rUp.GetXaxis().SetTitle("DNN discriminator")
        self.rUp.GetYaxis().SetTitleOffset(1.5)



        leg.Draw()
        c.SaveAs(self.saveDir + "/" + self.name + ".pdf")
        c.SaveAs(self.saveDir + "/" + self.name + ".png")


    # get the first zero/negative bin id starting from the right
    def mergeCriteria(self):

        pos = -1
        for b in reversed(range(self.firstNonZeroBin, self.h.GetNbinsX()+1)):

            if self.h.GetBinContent(b) == 0 or self.h.GetBinError(b) / self.h.GetBinContent(b) > self.maxStat:

                #print b, self.h.GetBinError(b), self.h.GetBinContent(b), self.h.GetNbinsX()
                pos = b
                break

        return pos


    # function which merges bin i with the left adjecent bin
    def mergeBins(self, i):

        arr = self.getBinArray()

        '''
        ARR INDEX        0  1    2    3     4     5     6
                    [0, 100, 500, 1000, 2000, 5000, 8000]
        BIN NUMBER        1    2    3     4     5     6
        '''

        # i represents the bin number, so do i-1

        # construct array of bins from the histo
        arr_new = []
        for j in range(0, len(arr)):

            if j == i-1: continue
            arr_new.append(arr[j])

        # merge and return
        mybins = array.array('d', arr_new)

        #self.h = self.h.Rebin(len(mybins)-1, self.h.GetName(), mybins)


        name = self.h.GetName()
        self.h.SetName("tmpp")
        tmp = self.h.Rebin(len(mybins)-1, name, mybins)
        self.h.Delete() # necessary to reduce memory
        self.h = tmp





    def getBinArray(self):

        ret = [self.binMin]
        for i in range(2, self.h.GetNbinsX()+1):

            x = float(self.h.GetBinLowEdge(i))
            ret.append(x)

        ret.append(self.binMax)
        return ret






