import os,sys

## to compute versioning
import hashlib

## Hmm common configuration

class HmmConfig():
    ''' Hmm configuration for scripts. 
        All the scripts except the Analyzer can should take the configuration from here.
    '''
    def __init__(self):
        ## CATEGORIES and MAP ##
        self.muCategories=["BB","BO","BE","OO","OE","EE"]
        #self.procCategories= [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]
        self.procCategories= [ "VBF0","GF","VBF1","Untag0","Untag1"]
        self.categories=[]
        for muStr in self.muCategories:
              for catStr in self.procCategories:
                  self.categories.append(catStr+"_"+muStr);

        ### FIT ###
        self.xmin = 110
        self.xmax=150
        self.sig_mass_points=[120,125,130]
        self.processes=["GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]
        self.sigfit_gaussians={}
        #self.sigfit_gaussians[("Untag0_BB","GluGlu")] = 3
        self.scale ={} #("cat","proc") = mean, sigma

        for p in ["GluGlu","ZH","WPlusH","WMinusH"]: # all except vbf
            self.sigfit_gaussians[("VBF0_EE",p)] = 2  ## 25
            self.sigfit_gaussians[("GF_EE",p)] = 2  ## 26

        self.sigfit_gaussians[("GF_BE","GluGlu")] = 2  ## 11
        self.sigfit_gaussians[("VBF0_OE","GluGlu")] = 2  ## 20
        self.sigfit_gaussians[("VBF0_OE","WMinusH")] = 2  ## 20
        self.sigfit_gaussians[("Untag1_EE","WMinusH")] = 2  ## 29

        self.sigfit_gaussians[("GF_OE","ttH")] = 1  ## 20
        self.sigfit_gaussians[("VBF0_EE","ttH")] = 1  ## 25
        self.sigfit_gaussians[("GF_EE","ttH")] = 1  ## 26
        self.sigfit_gaussians[("VBF1_EE","ttH")] = 1  ## 27
        self.sigfit_gaussians[("Untag0_EE","ttH")] = 1  ## 28
        self.sigfit_gaussians[("Untag1_EE","ttH")] = 1  ## 29
        
        #self.readScaleUnc()        

        ## DATACARD specific ##

        self.datacard_procs=['BKG','GluGlu','VBF','WPlusH','WMinusH','ttH','ZH']
    
        self.computeVersioning()

    def catToStr(self,num):
        return self.categories[num]

    def computeVersioning(self):
        catInputs=",".join(self.categories)
        catInputs+=";"+",".join(self.muCategories)
        catInputs+=";"+",".join(self.procCategories)
        catInputs+=";"+",".join(self.processes)
        self.catVersion=hashlib.md5(catInputs).hexdigest()
        fitInputs=catInputs
        fitInputs+=";"+",".join(["%d"%x for x in self.sig_mass_points])
        fitInputs+=";"+",".join(self.processes);
        fitInputs+=";"+",".join([cat+"_"+proc+"=%d"%self.sigfit_gaussians[(cat,proc)] for cat,proc in self.sigfit_gaussians ])
        fitInputs += ';' + ",".join(["m%.5f-s%.5f"%self.scale[(cat,proc)] for cat,proc in self.scale])
        self.fitVersion=hashlib.md5(fitInputs).hexdigest()

    def Print(self):
        print "--- Hmm Configurator ---"
        print "Name:",self.__class__.__name__
        for x in ["xmin","xmax","muCategories","procCategories","categories","sig_mass_points","processes","datacard_procs","catVersion","fitVersion"]:
            print x+"=",eval("self."+x)
        print "------------------------"

    def br(self,mass=125):
        ''' only 125 '''
        br = 2.176E-04
        return br

    def xsec(self,type,mass=125):
        ''' only 125 '''
        if type=="VBF" or type == "qqH" : xsec= 3.782E+00 
        elif type == "WH": xsec=1.373E+00
        elif type == "ZH": xsec =8.839E-01
        elif type == "ggH" or type=="GluGlu": xsec =4.858E+01
        elif type == "ttH": xsec =5.071E-01
        elif type == "bbH" : xsec=4.880E-01
        elif type == "WMinusH": xsec = 5.328E-01
        elif type == "WPlusH": xsec =8.400E-01
        else: xsec=1.
        return xsec

    def lumi(self):
        return 35867

    def readScaleUnc(self,f="Hmumu/syst/scale/scales.txt"):
        self.scale ={} #("cat","proc") = mean, sigma
        txt = open(f)
        for line in txt:
            info = line.split()[0]
            mean = float(line.split()[1])
            sigma = float(line.split()[2])
            procCat = info.split('_')[0]
            muCat   = info.split('_')[1]
            proc    = info.split('_')[2]
            cat = procCat +"_" + muCat
            self.scale[ (cat, proc) ] = (mean, sigma)

hmm=HmmConfig()

class HmmConfigAutoCat(HmmConfig):
    def __init__(self):
        HmmConfig.__init__(self)
        self.categories=[ "cat%d"%x for x in range(0,16)]
        self.procCategories=[]
        self.muCategories=[]
        self.sigfit_gaussians=[]
        #self.readScaleUnc()        

        self.computeVersioning()

hmmAutoCat =HmmConfigAutoCat()

if __name__=="__main__":
    hmm.Print()
    hmmAutoCat.Print()

class Stack:
    ''' This is a soft version of thstack that usually crash'''
    def __init__(self):
        self.hists_=[]
        self.name_="myStack"
    def SetName(self,name):
        self.name_ = name
    def Add(self,h):
        if len(self.hists_) == 0:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
        else:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
            self.hists_[-1].Add(self.hists_[-2])
    def Draw(self,opts=""):
        for idx in reversed(range(0,len(self.hists_))):
            h= self.hists_[idx]
            if idx ==len(self.hists_)-1 or 'SAME' in opts: h.Draw(opts)
            else       : h.Draw(opts +" SAME")
    def GetHist(self):
        return self.hists_[-1]

    def GetFirstHist(self):
        return self.hists_[0]

    def Delete(self):
        for h in self.hists_: h.Delete()
        self.hists_=[]

    def Print(self):
        print "------",self.name_,"-----------"
        for h in self.hists_:
            print "*",h.GetName()
        print "-------------------------------"



import re
class StringToCpp():
    def __init__(self):
        ## string cut to c-cut
        self.map={'bdt_score':'bdt[0]',
                'dimu_avg_abs_eta':'mu_ave_eta',
                'dimu_max_abs_eta':'mu_max_eta',
                }
        self.operator={'gt':'>=','lt':'<'}
    def parse_(self,string,result={}):
        if string =="" : return result
        op= string.split('_')[0]
        if op not in self.operator:
            opStr= '?'
        else:
            opStr=self.operator[op]
        
        newstring = '_'.join(string.split('_')[1:])
        var="???"
        value=0.00

        for key in self.map:
            if re.match(key,newstring): ## beginning of the line
                var = self.map[key]
                newstring = re.sub('^'+key+'_','',newstring)
                valueStr =newstring.split('_')[0]
                newstring = '_'.join(newstring.split('_')[1:] )
                newValueStr=re.sub('n','-',re.sub('p','.',valueStr))
                #print "DEBUG: converting for",var,opStr,"->",newValueStr
                value = float (newValueStr) 
        if var=='???': raise ValueError 
        if (var,opStr) in result:
            if opStr == '>=' or opStr=='>':
                if value >= result[(var,opStr)]: result[(var,opStr)]=value
            if opStr == '<' or opStr== "<=":
                if value <= result[(var,opStr)]: result[(var,opStr)]=value
        else:
            result[(var,opStr)] = value
        return self.parse_(newstring,result)

    def Parse(self,string):  
        result= self.parse_(string) 
        l = []
        for  var,opStr in result:
            l.append( var +" "+opStr+ " " +"%.3f"%result[(var,opStr)] )
        return "if ( " + ' and '.join(l) + " )"

    def ParseAll(self):
        cat="icat"
        print self.Parse("gt_bdt_score_0p395_gt_bdt_score_0p727_gt_dimu_avg_abs_eta_1p954"),cat,"=",0,";"
        print self.Parse("lt_bdt_score_0p395_lt_bdt_score_0p051_lt_bdt_score_n0p399"),cat,"=",1,";"
        print self.Parse("lt_bdt_score_0p395_gt_bdt_score_0p051_gt_dimu_max_abs_eta_0p915_gt_bdt_score_0p246_gt_dimu_max_abs_eta_1p902"),cat,"=",2,";"
        print self.Parse("lt_bdt_score_0p395_lt_bdt_score_0p051_gt_bdt_score_n0p399_gt_dimu_max_abs_eta_1p965"),cat,"=",3,";"
        print self.Parse("gt_bdt_score_0p395_lt_bdt_score_0p727_lt_bdt_score_0p645_gt_dimu_max_abs_eta_0p917_gt_dimu_max_abs_eta_1p787"),cat,"=",4,";"
        print self.Parse("lt_bdt_score_0p395_lt_bdt_score_0p051_gt_bdt_score_n0p399_lt_dimu_max_abs_eta_1p965_lt_bdt_score_n0p115"),cat,"=",5,";"
        print self.Parse("lt_bdt_score_0p395_lt_bdt_score_0p051_gt_bdt_score_n0p399_lt_dimu_max_abs_eta_1p965_gt_bdt_score_n0p115"),cat,"=",6,";"
        print self.Parse("lt_bdt_score_0p395_gt_bdt_score_0p051_lt_dimu_max_abs_eta_0p915_lt_bdt_score_0p261"),cat,"=",7,";"
        print self.Parse("gt_bdt_score_0p395_lt_bdt_score_0p727_lt_bdt_score_0p645_gt_dimu_max_abs_eta_0p917_lt_dimu_max_abs_eta_1p787_lt_bdt_score_0p527"),cat,"=",8,";"
        print self.Parse("gt_bdt_score_0p395_lt_bdt_score_0p727_lt_bdt_score_0p645_gt_dimu_max_abs_eta_0p917_lt_dimu_max_abs_eta_1p787_gt_bdt_score_0p527"),cat,"=",9,";"
        print self.Parse("lt_bdt_score_0p395_gt_bdt_score_0p051_lt_dimu_max_abs_eta_0p915_gt_bdt_score_0p261"),cat,"=",10,";"
        print self.Parse("lt_bdt_score_0p395_gt_bdt_score_0p051_gt_dimu_max_abs_eta_0p915_gt_bdt_score_0p246_lt_dimu_max_abs_eta_1p902"),cat,"=",11,";"
        print self.Parse("lt_bdt_score_0p395_gt_bdt_score_0p051_gt_dimu_max_abs_eta_0p915_lt_bdt_score_0p246"),cat,"=",12,";"
        print self.Parse("gt_bdt_score_0p395_lt_bdt_score_0p727_gt_bdt_score_0p645"),cat,"=",13,";"
        print self.Parse("gt_bdt_score_0p395_lt_bdt_score_0p727_lt_bdt_score_0p645_lt_dimu_max_abs_eta_0p917"),cat,"=",14,";"
        print self.Parse("gt_bdt_score_0p395_gt_bdt_score_0p727_lt_dimu_avg_abs_eta_1p954"),cat,"=",15,";"
        return 




