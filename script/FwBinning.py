
import sys,array
import ROOT
ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)


class Rebin:
    
    h = None        # sum of bkgs
    h_ref = None    # reference (TTbar)
    maxStat = 0.3
    
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

#        for i in range(1, self.h.GetNbinsX()+1):
#            print i, self.h.GetBinCenter(i), self.h.GetBinContent(i),self.h.GetBinError(i)/self.h_ref.GetBinContent(i)
#        sys.exit()
                
        return


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
