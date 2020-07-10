import os,sys

## to compute versioning
from hmm import *

## Hmm common configuration

class VBSHadr(HmmConfig):
    def __init__(self):
        HmmConfig.__init__(self)
        self.dirname="VBShadAnalysis/" 
        self.varname="MVV"
        #MVV_BBtag_DoublyChargedHiggsGMmodel_HWW_M1500_13TeV-madgraph
        self.sigspec="DoublyChargedHiggsGMmodel_HWW_M%.0f_13TeV-madgraph"
        #self.categories=["BBtag","RBTag"]
        self.categories=["BB"]
        self.year=2016

        ### FIT ###
        self.xmin = 800
        self.xmax = 3000
        self.xname="mvv" ## -output roo rela var

        self.sig_mass_points=[1500]
        self.processes=["DoublyChargedHiggs"]
        self.sigfit_gaussians={}
        self.background_input_masks=None

        self.sigfit_scale_unc = {} ## cat, proc -> value or (cat,proc)
        self.sigfit_replace = {} ## (cat,proc) -> (cat,proc)
        self.sigfit_smear_unc = {}

        ## n. gaus
        self.sigfit_gaussians[('BB','DoublyChargedHiggs')]=3
        #self.sigfit_gaussians[('RBtag','DoublyChargedHiggs')]=1
        ## signal fit: dirname + varname + _ signspec
        self.SimpleScaleAndSmear()
        self.computeVersioning()

        ##tmp configuration for background on MC
        self.background_input_masks=['MVV_BB_AsimovB']
        self.background_fitstrategy=1


#create instance
vbshadr=VBSHadr()
