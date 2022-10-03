import sys,array,copy,math
import ROOT
ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

class SimpleRebin:
    def __init__(self,n):
        self.nmerge=n
    def applyMapping(self, x, h):
        h2=h.Clone()
        h2.Rebin(self.nmerge)
        return h2

gc=[]
class FixedRebin:
    def __init__(self,l):
        self.boundaries_=array.array('d',l)
    def applyMapping(self,x,h):
        global gc
        h1 = h.Rebin(len(self.boundaries_)-1,h.GetName(),self.boundaries_)
        if h != None and h1 == None:
            print "[ERROR], Unable to rebin",h.GetName()
        return h1

class Rebin:
    h = None        # sum of bkgs
    h_ref = None    # reference (TTbar)
    maxStat = 0.3 #original 0.3 miao change

    binMin = 0
    binMax = 0

    def __init__(self, h, h_ref, maxStat):
        
        self.h = h
        self.h_ref = h_ref
        self.maxStat = maxStat
        
        self.binMin = float(self.h.GetBinLowEdge(1)) # min bin
        self.binMax = float(self.h.GetBinLowEdge(h.GetNbinsX()+1)) # max bin
        

    def getHist(self):
        
        return self.h, self.h_ref


    def mergeCriteria(self, i):

        #if self.h.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h.GetBinContent(i) > maxStat: return True
        if self.h.GetBinContent(i) == 0 or self.h_ref.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h_ref.GetBinContent(i) > self.maxStat: return True
        else: return False


    # Recursive rebin function
    def directionalRebin(self, direction = ""):

        if direction == "":
            
            self.directionalRebin("right")
            self.directionalRebin("left")
            return
            
        if direction == "left"  : r = range(1, self.h.GetNbinsX()) # do not include the last bin
        if direction == "right" : r = reversed(range(2, self.h.GetNbinsX()+1)) # start from the right bin towards the left (do not include the zero!)

        for i in r:

            if self.mergeCriteria(i): 

                if direction == "left":
                    
                    self.mergeBins(i+1)
                    self.directionalRebin(direction) 
                    return
                
                else : 

                    self.mergeBins(i)
                    self.directionalRebin(direction)
                    return
             
        #for i in range(1, self.h.GetNbinsX()+1):
        #    print i, self.h.GetBinCenter(i), self.h.GetBinContent(i),self.h.GetBinError(i)/self.h_ref.GetBinContent(i)
        #sys.exit()
            
        return
    
    def rebin(self):

        # first binning on bkg
        self.directionalRebin()

    # function which merges bin i with the left adjecent bin
    def mergeBins(self, i):

        arr = self.getBinArray()

        '''
        ARR INDEX	 0  1    2    3     4     5     6
                    [0, 100, 500, 1000, 2000, 5000, 8000]
        BIN NUMBER	  1    2    3     4     5     6
        '''

        # i represents the bin number, so do i-1

        # construct array of bins from the histo
        arr_new = []
        for j in range(0, len(arr)):

            if j == i-1: continue
            arr_new.append(arr[j])

        # merge and return
        mybins = array.array('d', arr_new)
        self.h = self.h.Rebin(len(mybins)-1, self.h.GetName(), mybins) 
        self.h_ref = self.h_ref.Rebin(len(mybins)-1, self.h_ref.GetName(), mybins) 


    def getBinArray(self):

        ret = [self.binMin]
        for i in range(2, self.h.GetNbinsX()+1):

            x = float(self.h.GetBinLowEdge(i))
            ret.append(x)
        ret.append(self.binMax)
        return ret







class RebinNeg:
    
    h = None        # sum of bkgs
    h_ref = None    # reference (TTbar)
    h_sig = None
    maxStat = 0.3 #original 0.3 miao change
    
    binMin = 0
    binMax = 0
    
    def __init__(self, h, h_ref, h_sig, maxStat, binning):
        
        htmp = h.Clone("h")
        h_reftmp = h_ref.Clone("href")
        h_sigtmp = h_sig.Clone("hsig")

        self.h = self.Rebin(htmp, binning)
        self.h_ref = self.Rebin(h_reftmp, binning)
        self.h_sig = self.Rebin(h_sigtmp, binning)
        self.maxStat = maxStat
        
        self.binMin = float(self.h.GetBinLowEdge(1)) # min bin
        self.binMax = float(self.h.GetBinLowEdge(h.GetNbinsX()+1)) # max bin
        

    def Rebin(self, h, newbins):
        mybins = array.array('d', newbins)
        h1 = h.Rebin(len(mybins)-1, h.GetName(), mybins)
        return h1

    def getHist(self):
        
        return self.h, self.h_ref


    def mergeCriteria(self, i):
        
        #if self.h.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h.GetBinContent(i) > maxStat: return True
        if self.h.GetBinContent(i) == 0 or self.h_ref.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h_ref.GetBinContent(i) > self.maxStat: return True
        else: return False


    # Recursive rebin function
    def directionalRebin(self, direction = ""):

        if direction == "":
            
            self.directionalRebin("right")
            self.directionalRebin("left")
            return
            
        if direction == "left"  : r = range(1, self.h.GetNbinsX()) # do not include the last bin
        if direction == "right" : r = reversed(range(2, self.h.GetNbinsX()+1)) # start from the right bin towards the left (do not include the zero!)

        for i in r:

            if self.mergeCriteria(i): 

                if direction == "left":
                    
                    self.mergeBins(i+1)
                    self.directionalRebin(direction) 
                    return
                
                else : 

                    self.mergeBins(i)
                    self.directionalRebin(direction)
                    return
             
        #for i in range(1, self.h.GetNbinsX()+1):
        #    print i, self.h.GetBinCenter(i), self.h.GetBinContent(i),self.h.GetBinError(i)/self.h_ref.GetBinContent(i)
        #sys.exit()
            
        return

    def rebin(self):

        # first binning on bkg
        self.directionalRebin()

        # then binning based on signal to remove negative weights
        pos = self.getNegativeContentBin()
        while pos != -1:

          
            #print pos, self.h_sig.GetBinContent(pos)
            self.mergeBins(pos)
            self.directionalRebin()
            pos = self.getNegativeContentBin() # update



    def getNegativeContentBin(self):

        pos = -1
        for b in range(1, self.h_sig.GetNbinsX()+1):

            if self.h_sig.GetBinContent(b) < 0: pos = b

        return pos


    # function which merges bin i with the left adjecent bin
    def mergeBins(self, i):

        arr = self.getBinArray()

        '''
        ARR INDEX	 0  1    2    3     4     5     6
                    [0, 100, 500, 1000, 2000, 5000, 8000]
        BIN NUMBER	  1    2    3     4     5     6
        '''

        # i represents the bin number, so do i-1

        # construct array of bins from the histo
        arr_new = []
        for j in range(0, len(arr)):

            if j == i-1: continue
            arr_new.append(arr[j])

        # merge and return
        mybins = array.array('d', arr_new)
        self.h = self.h.Rebin(len(mybins)-1, self.h.GetName(), mybins) 
        self.h_ref = self.h_ref.Rebin(len(mybins)-1, self.h_ref.GetName(), mybins) 
        self.h_sig = self.h_sig.Rebin(len(mybins)-1, self.h_sig.GetName(), mybins) 


    def getBinArray(self):

        ret = [self.binMin]
        for i in range(2, self.h.GetNbinsX()+1): 

            x = float(self.h.GetBinLowEdge(i))
            ret.append(x)
            
        ret.append(self.binMax)
        return ret





class RebinSsqrtB:
    
    h = None        # sum of bkgs
    h_ref = None    # reference (TTbar)
    h_sig = None
    maxStat = 0.3   ##original 0.3 miao change
    
    binMin = 0
    binMax = 0
    
    def __init__(self, h, h_ref, h_sig, maxStat):
        
        self.h = h
        self.h_ref = h_ref
        self.h_sig = h_sig
        self.maxStat = maxStat
        
        self.binMin = float(self.h.GetBinLowEdge(1)) # min bin
        self.binMax = float(self.h.GetBinLowEdge(h.GetNbinsX()+1)) # max bin
        

    def getHist(self):
        
        return self.h, self.h_ref


    def mergeCriteria(self, i):

        #if self.h.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h.GetBinContent(i) > maxStat: return True
        if self.h.GetBinContent(i) == 0 or self.h_ref.GetBinContent(i) == 0 or self.h.GetBinError(i) / self.h_ref.GetBinContent(i) > self.maxStat: return True
        else: return False


    # Recursive rebin function
    def directionalRebin(self, direction = ""):

        if direction == "":
            
            self.directionalRebin("right")
            self.directionalRebin("left")
            return
            
        if direction == "left"  : r = range(1, self.h.GetNbinsX()) # do not include the last bin
        if direction == "right" : r = reversed(range(2, self.h.GetNbinsX()+1)) # start from the right bin towards the left (do not include the zero!)

        for i in r:

            if self.mergeCriteria(i): 

                if direction == "left":
                    
                    self.mergeBins(i+1)
                    self.directionalRebin(direction) 
                    return
                
                else : 

                    self.mergeBins(i)
                    self.directionalRebin(direction)
                    return
             
        #for i in range(1, self.h.GetNbinsX()+1):
        #    print i, self.h.GetBinCenter(i), self.h.GetBinContent(i),self.h.GetBinError(i)/self.h_ref.GetBinContent(i)
        #sys.exit()
            
        return

    def rebin(self):

        
        while True:

            # search for the bin pair which gives the lowest S/B ratio
            xMin = 1e9
            bMin = -1
            for b in range(1, self.h_sig.GetNbinsX()):

                if self.h.GetBinContent(b) == 0: continue

                x = (self.h_sig.GetBinContent(b)+self.h_sig.GetBinContent(b+1)) / (self.h.GetBinContent(b)+self.h.GetBinContent(b+1))
                if x < xMin: 
                    xMin = x
                    bMin = b

            
            # until S/B > 0
            if xMin > 0: break

            # merge bMin and bMin+1
            self.mergeBins(bMin+1) # merge bin bMin and bMin+1
            

        # then binning based on signal to remove negative weights
        pos = self.getNegativeContentBin()
        while pos != -1:

          
            print pos, self.h_sig.GetBinContent(pos)
            self.mergeBins(pos)
            self.directionalRebin()
            pos = self.getNegativeContentBin() # update


    def getNegativeContentBin(self):

        pos = -1
        for b in range(1, self.h_sig.GetNbinsX()+1):

            if self.h_sig.GetBinContent(b) < 0: pos = b

        return pos


    # function which merges bin i with the left adjecent bin
    def mergeBins(self, i):

        arr = self.getBinArray()

        '''
        ARR INDEX	 0  1    2    3     4     5     6
                    [0, 100, 500, 1000, 2000, 5000, 8000]
        BIN NUMBER	  1    2    3     4     5     6
        '''

        # i represents the bin number, so do i-1

        # construct array of bins from the histo
        arr_new = []
        for j in range(0, len(arr)):

            if j == i-1: continue
            arr_new.append(arr[j])

        # merge and return
        mybins = array.array('d', arr_new)
        self.h = self.h.Rebin(len(mybins)-1, self.h.GetName(), mybins) 
        self.h_ref = self.h_ref.Rebin(len(mybins)-1, self.h_ref.GetName(), mybins) 
        self.h_sig = self.h_sig.Rebin(len(mybins)-1, self.h_sig.GetName(), mybins) 


    def getBinArray(self):

        ret = [self.binMin]
        for i in range(2, self.h.GetNbinsX()+1): 

            x = float(self.h.GetBinLowEdge(i))
            ret.append(x)
            
        ret.append(self.binMax)
        return ret

class RebinLikelihood:

    h_bkg = None        # sum of bkgs
    h_sig = None
    hbkg = []
    lbins = 100
    binMin = 0
    binMax = 0

    maxSigStat = 0.2  #### for mini we use 0.3
    maxStat = 0.2

    def __init__(self, lbins):

        self.lbins = lbins


    # rebin signal such that no negative entries are present

    def preRebin(self):
        b = self.h_bkg.GetNbinsX()
        pos = self.getZeroContentBin()
        while pos != -1:
            if pos == self.h_bkg.GetNbinsX(): self.mergeBins(pos) # merge pos and pos-1
            else: self.mergeBins(pos+1) # merge pos and pos+1
            pos = self.getZeroContentBin() # update

        a = self.h_bkg.GetNbinsX()
        print "Remove empty events of background by rebinning (from %d bins to %d bins)" % (b, a)
        b = self.h_sig.GetNbinsX()
        pos = self.getNegativeContentBin()
        while pos != -1:
            if pos == self.h_sig.GetNbinsX(): self.mergeBins(pos) # merge pos and pos-1
            else: self.mergeBins(pos+1) # merge pos and pos+1
            pos = self.getNegativeContentBin() # update

        a = self.h_sig.GetNbinsX()
        print "Remove negative events of signal by rebinning (from %d bins to %d bins)" % (b, a)


    def mergeCriteria(self, i):
        if self.h_bkg.GetBinContent(i) <= 0 or self.h_bkg.GetBinError(i) / self.h_bkg.GetBinContent(i) > self.maxStat or self.h_bkg.GetBinContent(i) < 1 or self.h_sig.GetBinContent(i) <= 0 or self.h_sig.GetBinError(i) / self.h_sig.GetBinContent(i) > self.maxSigStat: return True
	hall = self.h_sig.Clone("hall")
	hall.Add(self.h_bkg)
        for hbkgi in self.hbkg:
            if hbkgi.GetBinContent(i) <= 0.: return True
            if hbkgi.GetBinError(i)/hbkgi.GetBinContent(i) > 0.5: return True
	#if self.h_bkg.GetBinContent(i) <= 1 or self.h_sig.GetBinContent(i) <= 0 or hall.GetBinError(i) / hall.GetBinContent(i) > self.maxSigStat: return True
	#if self.h_bkg.GetBinContent(i) <= 1 or self.h_sig.GetBinContent(i) <= 0 or hall.GetBinError(i) / hall.GetBinContent(i) > self.maxSigStat or self.h_sig.GetBinContent(i) < 0.05*math.sqrt(self.h_bkg.GetBinContent(i)): return True
	if self.h_bkg.GetBinWidth(i) < 0.05: return True	

        #if self.h_sig.Integral(1,i) < 0.5*self.h_sig.Integral(): return True

	###if self.h_bkg.GetBinContent(i) <= 0 or self.h_sig.GetBinContent(i) <= 0 or (self.h_bkg.GetBinError(i) / self.h_bkg.GetBinContent(i) > self.maxStat and self.h_sig.GetBinError(i) / self.h_sig.GetBinContent(i) > self.maxSigStat): return True
        else: return False
    

    # Recursive rebin function
    def directionalRebin(self, direction = ""):
        if direction == "":

            self.directionalRebin("right")
            self.directionalRebin("left")
            return

        if direction == "left" : r = range(1, self.h_bkg.GetNbinsX()) # do not include the last bin
        if direction == "right" : r = reversed(range(2, self.h_bkg.GetNbinsX()+1)) # start from the right bin towards the left (do not include the zero!)
        for i in r:
            if self.mergeCriteria(i):
                if direction == "left":

                    self.mergeBins(i+1)
                    self.directionalRebin(direction)
                    return

                else :
                    self.mergeBins(i)
                    self.directionalRebin(direction)
                    return
	#sys.exit()
        return


    def createMapping(self, h_bkg, h_sig, hbkg):

        #print h_bkg.GetEntries(), h_bkg.Integral()
        self.h_bkg = h_bkg
        self.h_sig = h_sig
        self.hbkg  = hbkg
        self.binMin = float(self.h_bkg.GetBinLowEdge(1)) # min bin
        self.binMax = float(self.h_bkg.GetBinLowEdge(self.h_bkg.GetNbinsX()+1)) # max bin


        self.directionalRebin()

        return self.getBinArray()

        likelihood = ROOT.TH1D("likelihood", "likelihood", self.lbins, 0, 1) # likelihood histogram
        for i in range(1, self.h_sig.GetNbinsX()+1): # map HT distributions to likelihood

            if self.h_sig.GetBinContent(i) == 0.0 and self.h_bkg.GetBinContent(i) == 0.0: continue # no interest in empty bins
            x = self.h_sig.GetBinContent(i) / (self.h_sig.GetBinContent(i) + self.h_bkg.GetBinContent(i))
            likelihood.Fill(x)

        mapping = [] # projection of HT to the bin number

        for i in range(1, h_sig_copy.GetNbinsX()+1): # loop over all initial HT bins (800)

            ht = h_sig_copy.GetBinCenter(i) # get the current HT value
            b = self.h_sig.FindBin(ht) # this is the bin of the raw HT value after pre-rebinning
            if self.h_sig.GetBinContent(b) == 0.0 and self.h_bkg.GetBinContent(b) == 0.0: mapping.append(1) # zero bins go to the zero likelihood
            else:

                x = self.h_sig.GetBinContent(b) / (self.h_sig.GetBinContent(b) + self.h_bkg.GetBinContent(b))
                mapping.append(likelihood.FindBin(x))

                if x > 1:
		    print "new check" ,i,x, self.h_sig.GetBinContent(b), self.h_bkg.GetBinContent(b)
          

        return mapping



    def applyMapping(self, mapping, h):

        mybins = array.array('d', mapping)
	
      	return h.Rebin(len(mybins)-1, "", mybins)
        

        hNew = ROOT.TH1D("", "", self.lbins, 0, 1) # likelihood histogram
        hNew.Sumw2()
        for b in range(1, self.lbins+1): hNew.SetBinError(b, 0);
        for b in range(1, h.GetNbinsX()+1):

            x = int(h.GetBinCenter(b))
            lbin = mapping[b-1] # arrays start from zero, bins from 1
            hNew.SetBinContent(lbin, hNew.GetBinContent(lbin) + h.GetBinContent(b))
            hNew.SetBinError(lbin, math.sqrt(hNew.GetBinError(lbin)*hNew.GetBinError(lbin) + h.GetBinError(b)*h.GetBinError(b)))
        if abs(h.Integral()-hNew.Integral()) > 0.0001:
            print "NOT EQUAL",before, after
            sys.exit()

        hNew.SetName(h.GetName())
        return hNew

    def applyMappingTrimmed(self, mapping, h):

        smapping = sorted(list(set(mapping))) # sorted list of unique mapping values
        hNew = ROOT.TH1D("", "", len(smapping), 0, len(smapping)) # likelihood histogram
        hNew.Sumw2()
        for b in range(1, len(smapping)+1): hNew.SetBinError(b, 0);
        for b in range(1, h.GetNbinsX()+1):

            x = int(h.GetBinCenter(b)) # return HT/BDT value
            lbin = smapping.index(mapping[b-1])+1 # mapping[b-1] returns the likelihood bin number
            #print "dddd",x, mapping[b-1], lbin
            hNew.SetBinContent(lbin, hNew.GetBinContent(lbin) + h.GetBinContent(b))
            hNew.SetBinError(lbin, math.sqrt(hNew.GetBinError(lbin)*hNew.GetBinError(lbin) + h.GetBinError(b)*h.GetBinError(b)))
        if abs(h.Integral()-hNew.Integral()) > 0.0001:
            print "NOT EQUAL",before, after
            sys.exit()

        hNew.SetName(h.GetName())
        return hNew

    def getNegativeContentBin(self):

        pos = -1
        for b in range(1, self.h_sig.GetNbinsX()+1):

            if self.h_sig.GetBinContent(b) < 0: pos = b

        return pos


    def getZeroContentBin(self):

        pos = -1
        for b in range(1, self.h_bkg.GetNbinsX()+1):

            if self.h_bkg.GetBinContent(b) <= 0: pos = b

        return pos


    # function which merges bin i with the left adjecent bin

    def mergeBins(self, i):
        arr = self.getBinArray()
        '''
        ARR INDEX 0 1 2 3 4 5 6
                    [0, 100, 500, 1000, 2000, 5000, 8000]
        BIN NUMBER 1 2 3 4 5 6
        '''
        # i represents the bin number, so do i-1 construct array of bins from the histo
        arr_new = []
        for j in range(0, len(arr)):
            if j == i-1: continue
            arr_new.append(arr[j])

        # merge and return
        mybins = array.array('d', arr_new)
        #print "BIN VECTOR ", i, mybins
        self.h_bkg = self.h_bkg.Rebin(len(mybins)-1, self.h_bkg.GetName(), mybins)
        self.h_sig = self.h_sig.Rebin(len(mybins)-1, self.h_sig.GetName(), mybins)
        for k in range(0,len(self.hbkg)):
            self.hbkg[k] = self.hbkg[k].Rebin(len(mybins)-1, self.hbkg[k].GetName(), mybins)
        #print "BIINNS", self.h_bkg.GetNbinsX()

    def getBinArray(self):
        ret = [self.binMin]

        for i in range(2, self.h_bkg.GetNbinsX()+1):
            x = float(self.h_bkg.GetBinLowEdge(i))
            ret.append(x)

        ret.append(self.binMax)
        return ret
