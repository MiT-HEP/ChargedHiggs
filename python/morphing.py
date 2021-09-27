import numpy as np
import ROOT
## def find_nearest_index(array, value):
##     idx = (np.abs(array - value)).argmin()
##     return idx
##     #return array[idx]

def find_root(array, value):
    ''' interpolating. return i'''
    if value<array[0]: return -1.
    if value>array[-1]: return len(array)
    i0,x0 = [ (i,x) for i,x in enumerate(array) if x <= value ][-1]
    i1,x1 = [ (i,x) for i,x in enumerate(array) if x >= value ][0]

    if i0==i1: return i0 ## nothing
    if x0==x1: return (i0+i1)/2.
    return i0 + float(i1-i0)/float(x1-x0)*float(value-x0)

def interpolate(array, x):
    ''' find in an array the position x, where x is float '''
    i0 = int(x)
    i1 = int(x)+1
    y0 = array[i0]
    y1 = array[i1]
    return y0+ (y1-y0)/float(i1-i0) * (x-i0)

def morphing(  (p0,h0) , (p1,h1) , p, delta=0.001):
    ''' Evaluate a histogram (h) interpolating it from h0-h1, with corresponding parameters p0,p1 at p'''
    nbins=h0.GetNbinsX()
    if h1.GetNbinsX() != nbins: raise ValueError("cannot interpolate different histograms")
    ## 
    a0= np.array([h0.GetBinContent(i+1) for i in xrange(0,nbins)])
    a1= np.array([h1.GetBinContent(i+1) for i in xrange(0,nbins)])
    ### Normalization
    n0 = np.sum(a0)
    n1 = np.sum(a1)
    ###  interpolate the normalizations
    a = 1.-(p-p0) /(p1-p0) ## tc at a=1 -> pdf0 , and a=0 pdf1
    n = (n1-n0)*(1.-a)  + n0 ## final normalization
    ### PDF
    pdf0 = a0 / n0
    pdf1 = a1 / n1
    ## cdf 
    cdf0 =np.cumsum(pdf0)
    cdf1 =np.cumsum(pdf1)
    ## cdf
    #cdf_values = [ [] for i in xrange(0,nbins) ]
    pdf_values = [ [] for i in xrange(0,nbins) ]
    for y in np.arange(0.+delta,1.,delta ):
        ## find x0 such that cdf0 is y
        x0 = find_root(cdf0, y) ## float 
        x1 = find_root(cdf1, y)

        ## find x1 such that cdf1 is y
        x = a*x0 + (1.-a) * x1
        #cdf_values[int(round(x))].append(y) 
        f0 = interpolate(pdf0,x0)
        f1 = interpolate(pdf1,x1)
        f = f0*f1/ ( a*f1 + (1.-a)*f0  )
        pdf_values[int(round(x))].append(f)

    ## construct the pdf as average
    pdf= np.array([  np.mean(np.array(pdf_values[i])) if len(pdf_values[i])>0 else 0. for i in xrange(0,nbins) ])

    val= pdf* n/np.sum(pdf) # small correction to the sum
    h= h0.Clone(h0.GetName() + "_par_%f"%p)
    [ h.SetBinContent(i+1,val[i]) for i in xrange(0,nbins)]
    return h


if __name__ == "__main__":
    mygaus0 = ROOT.TF1("mygaus0","TMath::Gaus(x,2,.5)",0,10)
    mygaus1 = ROOT.TF1("mygaus1","TMath::Gaus(x,7,2.)",0,10)

    h0 = ROOT.TH1D("h0","h0",100,0,10)
    h1 = ROOT.TH1D("h1","h1",100,0,10)
    h0.FillRandom("mygaus0",5000)
    h1.FillRandom("mygaus1",10000)

    garbage=[]

    canv=ROOT.TCanvas()
    h0.Draw("HIST")
    h1.Draw("HIST SAME")

    h0.SetLineWidth(2)
    h1.SetLineWidth(2)
    h0.SetLineColor(ROOT.kBlack)
    h1.SetLineColor(ROOT.kBlack)
    #for a,c  in zip([0.,0.25,.5,.75,1.],[38,46,8,ROOT.kOrange,ROOT.kMagenta]):
    hists=[]
    for a,c  in zip([0.,.1,.2,.3,.4,.5,.6,.7,.8,.9, 1.],[ROOT.kBlue+i for i in range(0,11)]):
        h=  morphing(  (0,h0) , (1.,h1) , a)
        h.SetLineWidth(1)
        h.SetLineColor(c)
        h.Draw("HIST SAME")
        garbage.append(h)
        hists.append(h)
        print "N0",h0.Integral(),"N1",h1.Integral(),"N=",h.Integral(),"Frac=",a

    h0.Draw("HIST SAME")
    h1.Draw("HIST SAME")
    
    h0.GetYaxis().SetRangeUser(0,500)
    canv.Modified()
    canv.Update()

    raw_input("ok?")
