import os,sys

## to compute versioning
from hmm import *

## Hmm common configuration

#DoublyChargedHiggsGMmodel_HWW_M...     in HAD_ch.root
#SinglyChargedHiggsGMmodel_HWZ_Znn_M... in MET_ch.root
#SinglyChargedHiggsGMmodel_HWZ_Zbb_M... in BHAD_ch.root

class VBSHadr(HmmConfig):
    def __init__(self,target="",year=2016):
        if target =="":
            print "Allowed targets are: bhad met had"
            raise ValueError()
        HmmConfig.__init__(self)
        self.dirname="VBShadAnalysis/" 
        self.varname="MVV"
        self.sigfit_gaussians={}
        if target == 'had':
            self.sigspec="DoublyChargedHiggsGMmodel_HWW_M%.0f"
            self.categories=["BB"]
            #self.categories=["BB_high","BB_low","RB_high","RB_low"]
            self.processes=["DoublyChargedHiggs"]
            self.xmin = 800
            self.xmax = 3000
            self.xname="mvv" ## -output roorelvar
        elif target =='bhad':
            self.sigspec="SinglyChargedHiggsGMmodel_HWZ_Zbb_M%.0f"
            #self.categories=["RBtag_high","RBtag_low"]
            self.categories=["BBtag"]
            self.processes=["SinglyChargedHiggs"]
            self.xmin = 800
            self.xmax = 3000
            self.xname="mvv" ## -output roorelvar, must be coherent with the one above
        elif target =='met':
            self.sigspec="SinglyChargedHiggsGMmodel_HWZ_Znn_M%.0f"
            self.processes=["SinglyChargedHiggs"]
            #self.categories=["BMET_high","BMET_low","RMET_high","RMET_low"]
            self.categories=["BMET"]
            self.xmin = 200
            self.xmax = 3000
            self.xname="mt" ## -output roorelvar
            self.dcb=True
            #self.sigfit_gaussians[('BMET','SinglyChargedHiggs')]=2

        #self.categories=["BBtag","RBTag"]
        self.year=year

        ### FIT ###

        #self.sig_mass_points=[1000,1500,2000]
        self.sig_mass_points=[1000,1500,2000]
        self.background_input_masks=None

        self.sigfit_scale_unc = {} ## cat, proc -> value or (cat,proc)
        self.sigfit_replace = {} ## (cat,proc) -> (cat,proc)
        self.sigfit_smear_unc = {}

        ## n. gaus
        self.sigfit_gaussians[('BB','DoublyChargedHiggs')]=3
        ## signal fit: dirname + varname + _ signspec
        self.SimpleScaleAndSmear()
        self.computeVersioning()

        ##tmp configuration for background on MC
        self.background_input_masks=['MVV_BB_AsimovB']
        self.background_fitstrategy=1


#create instance
#vbshadr=VBSHadr()
g=[]
for year in [2016,2017,2018]:
    for target in ["had","bhad","met"]:
        exec("vbshadr_"+target+"_%d"%year+"=VBSHadr(\""+target+"\",%d)"%year)
        exec("g.append("+ "vbshadr_"+target+"_%d"%year + ")")
