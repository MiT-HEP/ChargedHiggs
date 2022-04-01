#!/usr/bin/env python
import os, sys, re
from optparse import OptionParser
from datetime import datetime

import itertools, copy ## for keywords
import numpy as np
from array import array
import math


if __name__=="__main__":
    usage='''%prog 
    '''
    parser=OptionParser(usage = usage)
    parser.add_option("-v","--verbose",type='int',help="verbose",default=1)

    parser.add_option("-i","--input",type='string',help="input file name", default="HAD.root")

    parser.add_option("-y","--year",type='int',help="year; full run2 == 2020 (12016 = APV 22016=postVFP, 2021 with APV splitting)",default=2016)
    parser.add_option("-c","--category",type='string',help="5 category: BB, BMET, RMET, BBTAG, RBTAG", default="BB")
    parser.add_option("-s","--analysisStra",type='string',help="which one to fit with:mVV, mjj, BDT, ... ? ", default="mjj")
    parser.add_option("-r","--region",type='string',help="region: SR or anti or side",default='SR')

    parser.add_option("","--aqgc",action='store_true',help="AQGC input and datacard [%default]",default=False) ### aqgc code wrappend by this guardw
    parser.add_option("","--aqgc_parameter",help="AQGC parameter [%default]",default='ft7') 
    parser.add_option("","--there",action='store_true',help="Don't save directory in datacards fname. [%default]",default=False) 
    parser.add_option("","--aqgc_interpolate",action='append',help="AQGC extra points to evaluate [%default]",default=[]) ## INTERPOLATE
    parser.add_option("-q","--quote",type='int',help="sig ext.: 1: (WW/WZ/ZZ)+(ewk/qcd/int) all separate; 2: VVAll+(ewk/qcd/int); 3: (WW/WZ/ZZ)-All; 4: VV-All ; 5:AQGC",default=1)

    opt,args=parser.parse_args()

######### after option parser, so we have --help
import ROOT
import FwBinning as FwRebin


lumis={
    12016: 19.52, #APV
    22016: 16.80, #postVFP
    2016: 35.9,
    2017: 41.5,
    2018: 60.0,
    2020: 45.7,
}

if "%d"%opt.year not in opt.input:  raise ValueError('Your file should match the selected year. Year: ' + opt.year+ "not in filename "+opt.input)

## [ssww, osww, zzjj, wzbb, wznn]/[ewk,qcd,ewk-qcd] 
xsecsig = [
{"pro" : "ssWW", "cont": "EWK", "name": "WPMJJWPMJJjj_EWK_LO", "xsec" : 0.1258},
{"pro" : "ssWW", "cont": "QCD", "name": "WPMJJWPMJJjj_QCD_LO", "xsec" : 0.1083},
#{"pro" : "ssWW", "cont": "INT", "name": "WPMJJWPMJJjj_EWK_QCD_LO", "xsec" : 0.2448},
{"pro" : "osWW", "cont": "EWK", "name": "WPJJWMJJjj_EWK_LO", "xsec" : 1.8932},
{"pro" : "osWW", "cont": "QCD", "name": "WPJJWMJJjj_QCD_LO", "xsec" : 160.},###9.79},
#{"pro" : "osWW", "cont": "INT", "name": "WPJJWMJJjj_EWK_QCD_LO", "xsec" : 11.65},
{"pro" : "ZZ", "cont": "EWK", "name": "ZJJZJJjj_EWK_LO", "xsec" : 0.0533},
{"pro" : "ZZ", "cont": "QCD", "name": "ZJJZJJjj_QCD_LO", "xsec" : 1.084},
#{"pro" : "ZZ", "cont": "INT", "name": "ZJJZJJjj_EWK_QCD_LO", "xsec" : 1.143},
{"pro" : "WZ", "cont": "EWK", "name": "ZJJNOBWPMJJjj_EWK_LO", "xsec" : 0.4548},
{"pro" : "WZ", "cont": "QCD", "name": "ZJJNOBWPMJJjj_QCD_LO", "xsec" : 4.7207},
#{"pro" : "WZ", "cont": "INT", "name": "ZJJNOBWPMJJjj_EWK_QCD_LO", "xsec" : 5.1734},
{"pro" : "WZ", "cont": "EWK", "name": "ZBBWPMJJjj_EWK_LO", "xsec" : 0.1236},
{"pro" : "WZ", "cont": "QCD", "name": "ZBBWPMJJjj_QCD_LO", "xsec" : 1.264},
#{"pro" : "WZ", "cont": "INT", "name": "ZBBWPMJJjj_EWK_QCD_LO", "xsec" : 1.388},
{"pro" : "WZ", "cont": "EWK", "name": "ZNUNUWPMJJjj_EWK_LO", "xsec" : 0.1652},
{"pro" : "WZ", "cont": "QCD", "name": "ZNUNUWPMJJjj_QCD_LO", "xsec" : 1.699},
#{"pro" : "WZ", "cont": "INT", "name": "ZNUNUWPMJJjj_EWK_QCD_LO", "xsec" : 1.865},
{"pro" : "ZZ", "cont": "EWK", "name": "ZNUNUZJJjj_EWK_LO", "xsec" : 0.0312},
{"pro" : "ZZ", "cont": "QCD", "name": "ZNUNUZJJjj_QCD_LO", "xsec" : 0.6352},
#{"pro" : "ZZ", "cont": "INT", "name": "ZNUNUZJJjj_EWK_QCD_LO", "xsec" : 0.6698},
####semi-lep
{"pro" : "ssWW", "cont": "EWK", "name": "WMLEPWMHADjj_EWK_LO", "xsec" : 0.0323},
{"pro" : "ssWW", "cont": "EWK", "name": "WPLEPWPHADjj_EWK_LO", "xsec" : 0.0873},
{"pro" : "osWW", "cont": "EWK", "name": "WPHADWMLEPjj_EWK_LO", "xsec" : 0.9464},
{"pro" : "osWW", "cont": "EWK", "name": "WPLEPWMHADjj_EWK_LO", "xsec" : 0.9464},
{"pro" : "WZ", "cont": "EWK", "name": "WPLEPZHADjj_EWK_LO", "xsec" : 0.1814},
{"pro" : "WZ", "cont": "EWK", "name": "WMLEPZHADjj_EWK_LO", "xsec" : 0.0996},
{"pro" : "ssWW", "cont": "QCD", "name": "WMLEPWMHADjj_QCD_LO", "xsec" : 0.0318},
{"pro" : "ssWW", "cont": "QCD", "name": "WPLEPWPHADjj_QCD_LO", "xsec" : 0.0727},
{"pro" : "osWW", "cont": "QCD", "name": "WPHADWMLEPjj_QCD_LO", "xsec" : 76.19},
{"pro" : "osWW", "cont": "QCD", "name": "WPLEPWMHADjj_QCD_LO", "xsec" : 76.19},
{"pro" : "WZ", "cont": "QCD", "name": "WPLEPZHADjj_QCD_LO", "xsec" : 1.782},
{"pro" : "WZ", "cont": "QCD", "name": "WMLEPZHADjj_QCD_LO", "xsec" : 1.087},

########################### AQGC ###################################
# had
{"pro": "WW", "cont": "AQGC", "name" :"WMJJWMJJjj_EWK_LO", "xsec": 0.0304},  ## these are splitted
{"pro": "WW", "cont": "AQGC", "name" :"WPJJWPJJjj_EWK_LO", "xsec": 0.1249},
{"pro" : "ZZ", "cont": "AQGC", "name": "ZJJZJJjj_EWK_LO", "xsec" : 0.0319},
{"pro" : "osWW", "cont": "AQGC", "name": "WPJJWMJJjj_EWK_LO", "xsec" : 1.8139},
{"pro": "WZ", "cont":"AQGC","name":"WMJJZJJjj_EWK_LO" , "xsec":0.1880},
{"pro": "WZ", "cont":"AQGC","name":"WPJJZJJjj_EWK_LO" , "xsec":0.3492},
{"pro": "ZZ", "cont":"AQGC","name":"ZBBZJJNOBjj_EWK_LO" , "xsec":0.0055},
#{} ## ZNuNu -> 2016APV ??
{"pro":"ZZ","cont":"AQGC","name":"ZNUNUZJJNOBjj_EWK_LO","xsec":0.0074},
#lep
{"pro":"osWW","cont":"AQGC","name":"WPHADWMLEPjj_EWK_LO","xsec":0.9067},
{"pro":"osWW","cont":"AQGC","name":"WPLEPWMHADjj_EWK_LO","xsec":0.9067},
{"pro":"WW","cont":"AQGC","name":"WPLEPWPHADjj_EWK_LO","xsec":0.1355},
{"pro":"WW","cont":"AQGC","name":"WMLEPWMHADjj_EWK_LO","xsec":0.0222},
{"pro":"WZ","cont":"AQGC","name":"WPLEPZHADjj_EWK_LO","xsec":0.1746},
{"pro":"WZ","cont":"AQGC","name":"WMLEPZHADjj_EWK_LO","xsec":0.0940},
]
#{"pro" : "WZ", "cont": "AQGC", "name": "ZNuNuWPMJJjj_EWK_LO", "xsec" : 0.17},
## BR: Zjj 70% , Zbb 15%, Znn 20%. -> ZjjnoB=55%

print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
    if "ChargedHiggs" in os.path.basename(mypath):
        basepath = os.path.abspath(mypath)
    mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from morphing import morphing ## INTERPOLATE
from morphing import morphing_vertical ## INTERPOLATE

if opt.aqgc:

    aqgc_quotes=[5]
    if opt.quote not in aqgc_quotes: raise ValueError("AQGC and quote=5 is required")
    ### CONFIGURATION. I need to import the aqgc parameters that I will run on. 
    ### the list of all possible reweightings are in aqgc_names. 
    ### this name structure is  parameter_value with usual masks for strings 
    sys.path.insert(0,"./")
    from ParseDat import aqgc_names
    aqgc_parameters = list(set([ x.split('_')[0] for x in aqgc_names ]))
    aqgc_values = { par:[ name.split('_')[1] for name in aqgc_names if name.split('_')[0] == par ] for par in aqgc_parameters } ## par -> list of value_str , value
    aqgc_fname=re.sub('.root','aqgc.root',opt.input)
    print "Using AQGC file name",aqgc_fname
    #aqgc_par='ft7'
    aqgc_par=opt.aqgc_parameter
    if aqgc_par not in aqgc_parameters: raise ValueError(aqgc_par+" is not in the list of aqgc_parameters. Possible values are: "+ ','.join(aqgc_parameters))
   
    if len(aqgc_values[aqgc_par]) == 0:
        print "--- DEBUG AQGC---"
        print "AQGC Parameter is",aqgc_par
        print "AQGC names", ','.join(aqgc_names[0:20]),"..."
        print "AQGC parameters", ','.join(aqgc_parameters)
        print "AQGC values = ",aqgc_values[aqgc_par]
        print "AQGC values = ",aqgc_values
        print "------------------"
        raise RuntimeError("List of values for parameter is empty")

    ###################### FOR INTERPOLATION ##################
    def aqgc_atof(s):
        return float(s.replace('m','-').replace('p','.'))
    def aqgc_ftoa(f):
        return ('%.2f'%f).replace('-','m').replace('.','p')

    aqgc_values_float = { par: sorted([ aqgc_atof(name.split('_')[1]) for name in aqgc_names if name.split('_')[0] == par ]) for par in aqgc_parameters } ## INTERPOLATE


def find_closest_values(par, point):
    '''returns two strings, the one below and the one above p'''
    global aqgc_values_float
    p= aqgc_atof(point)
    lo,hi = None,None
    for i,v in enumerate(aqgc_values_float[par]):
        if v <= p: ## take the last
            lo = v
        if hi==None and v>=p: # take the first
            hi = v
    if lo == None or hi == None: 
        #raise ValueError("Unable to interpolate paremeter", par,"at",p,".",(lo,hi),". Values are: ",','.join(['%f'%x for x in aqgc_values_float[par] ]))
        raise ValueError("Unable to interpolate paremeter", par,"at",p,".",(lo,hi),". Values are: ",','.join(aqgc_values[par]))
    if lo == hi :
        raise ValueError("Unable to interpolate parameter",par,"at",p,".", "Point already in the list of values", ','.join(aqgc_values[par]) )
    return lo,hi
    ##########################################################
        

LikeBins=25
#likelihoodBinning = FwRebin.RebinLikelihood(LikeBins) if not opt.aqgc else FwRebin.SimpleRebin(10)
aqgc_rebin = [500.,750.,1000.,1250.,1500.,2000.,5000.] if opt.category in ["BBtag", "BMET"] and opt.region == "SR" else [500.,750.,1000.,1250.,1500.,1750.,2000.,5000.]
likelihoodBinning = FwRebin.RebinLikelihood(LikeBins) if not opt.aqgc else FwRebin.FixedRebin(aqgc_rebin)

def read_input():
    psig = []

    for sig in xsecsig:
        if not opt.aqgc and 'AQGC' in sig['cont']: continue
        if opt.aqgc and 'EWK' in sig['cont']: continue

        if ("BB" in opt.category) and ("Btag" not in opt.category):
            if ("ZBB" not in sig['name']) and ("ZNUNU" not in sig['name']) and ("LEP" not in sig['name']):
                psig.append(sig)
        elif "MET" in opt.category:
            if ("NUNU" in sig['name']) or ("LEP" in sig['name']):
                psig.append(sig)
        elif "Btag" in opt.category:
            #if ("BB" in sig['name']):
            if ("ZNUNU" not in sig['name']) and ("LEP" not in sig['name']):
                psig.append(sig)

    return psig

class DatacardBuilder:
    def __init__(self,verbose=1):
        self.categories={}  # bin_name -> file, path, 
        self.processes={} #
        self.systs={}
        self.fOut=None
        self.verbose=verbose
        self.proc_number={}
        self.garbage=[]
        pass

    def _get_proc_number(self,proc):
        if proc not in self.proc_number: 
            issig=self.processes[proc]['issig']
            pn= 0 if issig else 1

            for p in self.proc_number:
                v=self.proc_number[p]
                if v>0 and not issig: pn+=1
                if v<=0 and issig: pn-=1
            self.proc_number[proc] = pn
        
        return self.proc_number[proc]

    def add_category(self,x,path, basename,obs_suffix,fname=''):
        self.categories[x] = {
                "cat": x, ## keep it also in the dictionary
                "fname": fname,
                "path": path,
                "base": basename,
                "suffix": obs_suffix,
                }
        return self

    def add_process(self,x,issig,suffix=[],cat=[],fname=None):
        ''' add a process merging suffixes for cat in matching regex (empty == all) '''
        if x not in self.processes:
            self.processes[x] ={
                "proc": x, ## keep it also in the dictionary
                "suffix": suffix[:],
                "cat": cat[:],
                "issig":issig, ## this cannot change
                "fname":fname, ## use this fname if set. 
                "keywords": None, ## name -> list of values. { "$MASS" : [125,130]} 
                }
        else:
            self.processes[x]['suffix'] .extend( suffix )
            self.processes[x]['cat'] .extend( cat )

        return self

    def add_systematics(self,x, inname,type='lnN',catproc=(None,None),value=1., fname=[]):
        if x not in self.systs:
            self.systs[x] = {
                "syst": x, ## keep it also in the dictionary
                'inname': [inname], #if shape input name
                'cat-proc': [catproc],
                'value': [value], # the list match the cat-proc regex above
                'fname':[fname],
                'type': type,
                }
        else:
            self.systs[x]['catproc'].append(catproc)
            self.systs[x]['value'].append(value)
            self.systs[x]['fname'].append(fname)
            self.systs[x]['inname'].append(inname)

        return self

    def write_cards(self,outname): #datacard.txt
        outroot=re.sub(".txt.*",".inputs.root",outname)
        if opt.there: outroot=outroot.split('/')[-1]
        now=datetime.now()

        txt= open(outname,"w")
        txt.write("%%%% Automatically created by %s\n"%sys.argv[0])
        txt.write("%%%% date %s\n"%(now.strftime("%Y/%m/%d %H:%M")))

        txt.write("imax * number of channels\n")
        txt.write("jmax * number of background minus 1\n")
        txt.write("kmax * number of nuisance parameters\n")

        txt.write('-'*50+'\n')
        #shapes declaration
        for proc in self.processes:
            d2=self.processes[proc]
            if d2['keywords'] == None: continue ## nothing to be done. The fallback down here is fine.
            key_str="_".join( sorted([key for key in d2['keywords']]) )
            txt.write('shapes %(proc)s * %(file)s $CHANNEL_%(proc)s_%(key)s $CHANNEL_$PROCESS_%(key)s_$SYSTEMATIC\n'%{'file':outroot,'proc':proc,'key':key_str,"proc":proc})
        txt.write('shapes * * %(file)s $CHANNEL_$PROCESS $CHANNEL_$PROCESS_$SYSTEMATIC\n'%{'file':outroot})

        txt.write('-'*50+'\n')
        #observation
        txt.write("bin ")
        txt.write(' '.join([cat for cat in self.categories]))
        txt.write("\n")

        txt.write("observation ")
        txt.write(' '.join(['-1' for cat in self.categories]))
        txt.write("\n")

        txt.write('-'*50+'\n')
        #expectations
        txt.write("bin ")
        txt.write(' '.join([cat for cat in self.categories for proc in self.processes if cat in self.processes[proc]['cat'] ]))
        txt.write("\n")

        txt.write("process ")
        txt.write(' '.join([proc for cat in self.categories for proc in self.processes if cat in self.processes[proc]['cat'] ]))
        txt.write("\n")

        txt.write("process ")
        txt.write(' '.join(["%d"%self._get_proc_number(proc) for cat in self.categories for proc in self.processes if cat in self.processes[proc]['cat'] ]))
        txt.write("\n")

        txt.write("rate ")
        txt.write(' '.join(['-1' for cat in self.categories for proc in self.processes if cat in self.processes[proc]['cat'] ]))
        txt.write("\n")

        txt.write('-'*50+'\n')

        for s in self.systs:
            txt.write("%(syst)s %(type)s "%self.systs[s])
            # write systematic line: 
            # construct a list of all the matches, append -, and take the first match in each cat and proc
            txt.write(' '.join([ 
                (  [ '%f'%v  for (c,p),v in zip(self.systs[s]['cat-proc'],self.systs[s]['value']) if (re.match(c,cat) and re.match(p,proc))] + ['-'])[0] 
                    for cat in self.categories for proc in self.processes if cat in self.processes[proc]['cat'] 
                    ]))
            txt.write("\n")

        ### Add autoMCStats
        txt.write("* autoMCStats 0\n")

        txt.write("\n")
        for proc in self.processes: 
            d2=self.processes[proc]
            if d2['keywords'] != None:
                txt.write('### Keywords available for proc'+proc+"\n")
                for key in d2['keywords']:
                    txt.write('### ' + key +' '+ ','.join(d2['keywords'][key]) + '\n')


    def _manipulate_histo(self,htmp,y,corr):
        htmp.Scale(lumis[y] * 1000 * corr)
        #htmp.Rebin(2); ## REBIN --
        return htmp

    def _get_norm(self,hname):
        for sig in xsecsig:
            if not opt.aqgc and 'AQGC' in sig['cont']: continue
            if opt.aqgc and 'EWK' in sig['cont']: continue
            if sig['name'] in hname:
                return sig['xsec']
        return 1.

    def _get_interpolated_histo(self,fname,rename, (p1,hname1),(p2,hname2),p): ### INTERPOLATE
        ''' Return linearly interpolated histo'''
        h1 = self._get_histo(fname, hname1,"tmp1")
        h1.SetName("tmp1")

        if hname2 == hname1: ## nothing to interpolate
            h1.SetName(rename)
            return h1

        h2 = self._get_histo(fname, hname2,"tmp2")
        h2.SetName("tmp2")

        #a1 = aqgc_atof(p1) 
        #a2 = aqgc_atof(p2) 
        a1 = p1 ## these are already float
        a2 = p2
        a = aqgc_atof(p) 

        # square interpolation
        if True:
            a1 = p1**2
            a2 = p2**2
            a =  aqgc_atof(p)**2

        #h= morphing( (a1,h1),(a2,h2), a)
        h= morphing_vertical( (a1,h1),(a2,h2), a)
        h.SetName(rename)
        h1.Delete()
        h2.Delete()
        return h

    def _get_histo(self,fname,hname,rename=""):
        normalization = self._get_norm(hname)
        #normalization = 1.
        #print hname,normalization

        years = [opt.year]
        if opt.year==2020: years = [2016, 2017, 2018]
        if opt.year==2021: years= [12016,22016,2017,2018]
        print years 
        
        h = None
        for y in years:

            if y == 12016: ftmp = fname.replace(str(opt.year), '2016APV')
            elif y == 22016: ftmp = fname.replace(str(opt.year), '2016')
            else: ftmp = fname.replace(str(opt.year), str(y))

            fIn=ROOT.TFile.Open(ftmp)
            if self.verbose >2: print "DEBUG","opening file:",ftmp
            if self.fOut!=None: self.fOut.cd()
            if self.verbose >2: print "DEBUG","getting histo",hname,"->",rename
            htmp=fIn.Get(hname)
            if htmp==None and self.verbose >0: print "ERROR","unable to get histogram",hname,"from",ftmp
            if htmp==None: continue ## WARNING

            """
            ################### MET ###################################
            if "WJetsToLNu_HT" in hname and "FEB26" in ftmp and "2016APV" not in ftmp:
                ftmpmore = re.sub('FEB26','FEB26vjetNLO',ftmp)
                fInmore = ROOT.TFile.Open(ftmpmore)

		if self.fOut!=None: self.fOut.cd() 
                hnamemore = re.sub('WJetsToLNu_HT','WJetsToLNu_Pt',hname)
                htmpmore=fInmore.Get(hnamemore)

                sfmore = htmp.Integral()/(htmp.Integral()+htmpmore.Integral())
                normalization = sfmore
                print "WJetsToLNu_Pt, sf=", sfmore
                htmp.Add(htmpmore)

                fInmore.Close()


            if "ZJetsToNuNu_HT" in hname and "FEB26" in ftmp and "2016APV" not in ftmp:
                ftmpmore = re.sub('FEB26','FEB26vjetNLO',ftmp)
                fInmore = ROOT.TFile.Open(ftmpmore)
 
		if self.fOut!=None: self.fOut.cd()
                hnamemore = re.sub('ZJetsToNuNu_HT','ZJetsToNuNuPt',hname)
                htmpmore=fInmore.Get(hnamemore)

                sfmore = htmp.Integral()/(htmp.Integral()+htmpmore.Integral())
                normalization = sfmore
                print "ZJetsToNuNuPt, sf=", sfmore
                htmp.Add(htmpmore)

                fInmore.Close()
            ###########################################################
            """

            ### hname cooking, for central, syst e.t.c ###
            if '_BRLFSTAT1Up' in hname:
                namecen  = re.sub('_BRLFSTAT1Up','',hname)
                namedown = re.sub('_BRLFSTAT1Up','_BRLFSTAT1Down',hname)
                htmp     = fIn.Get(namecen)
                hdown    = fIn.Get(namedown)
                htmp.Add(htmp)
                htmp.Add(hdown,-1.)

            ### QCD SF and hist stat. enhancement
            if "_QCD_HT" in hname and "SR" in opt.region and opt.category in ["BB","BBtag"]:
                strategy=0 # 0: A/(A+B) 1: CB/D / (A+B)
                #if opt.category == 'BB': strategy=1
                #if opt.category == 'BBtag': strategy=1

                fInD ={} #
                htmpD = {} ## dictionary to hold additional histograms
                for reg in ["anti","side","antiSide"] if strategy==1 else ["anti"]:
                    fmore = re.sub('(HAD(?!SR)|HADSR)','HAD'+reg,ftmp)
                    fInD[reg] = ROOT.TFile.Open(fmore)
                    if fInD[reg] == None: 
                        print "ERROR: no such file or directory:",fmore

                    if self.fOut!=None: self.fOut.cd()
                    htmpD[reg] = fInD[reg].Get(hname)
                    if htmpD[reg]==None and self.verbose >0: 
                        print "ERROR","unable to get histogram",hname,"from",fmore, "for region",reg
                        planename = re.sub('_QCD_HT.*$','_QCD_HT',hname)
                        print "WARNING","falling back to non systmatic variation", planename
                        htmpD[reg] = fInD[reg].Get(planename)
                        if htmpD[reg] == None:
                            print "ERROR","unable to get histogram",hname,"from",fmore, "for region",reg
                            raise RuntimeError()


                if htmpD["anti"] and (strategy ==0 or (htmpD['side'] and htmpD['antiSide'])):

                    first =1 
                    last= htmp.GetNbinsX()
                    if opt.aqgc: last +=1 ## include overflow

                    # keep track of errors
                    eA=array('d',[0.])
                    eB=array('d',[0.])
                    eC=array('d',[0.])
                    eD=array('d',[0.])

                    # compute SF for proper normaliation
                    a = htmp.IntegralAndError(first, last,eA)
                    b = htmpD['anti'].IntegralAndError(first,last,eB)
                    if strategy == 1:
                        c = htmpD['side'].IntegralAndError(first,last,eC)
                        d = htmpD['antiSide'].IntegralAndError(first,last,eD)
                    
                    rA,rB = eA[0]/a if a>0 else 1, eB[0]/b if b>0 else 1. ## relative errors
                    if strategy == 1: rC,rD  = eC[0]/c if c>0 else 1. , eD[0]/d  if d>0 else 1.
                    rAoB = math.sqrt(rA**2 + rB**2)
                    eAoB = rAoB* a/b
                    
                    ## new
                    if strategy ==1 :
                        sf = c*b/(d*(a+b))
                        rSF = math.sqrt(rC**2 + rD**2 + (eAoB/(a/b+1))**2 ) ## relative error
                        eSF = rSF *sf
                    ## old A/(A+B)
                    if strategy ==0 :
                        sf = a/(a+b)
                        eBoA=rAoB * b/a
                        rSF = abs( eBoA/(1.+b/a)  ) ## relative error
                        eSF = rSF *sf

                    normalization = sf
                    print "-> DEBUG SF",opt.category,y,hname,
                    print "sf is", normalization,'+/-',eSF, "(",rSF*100,"%)"
                    print " ---> A=",a,"+/-",eA[0],"(","%.0f"%(rA*100),"%)",
                    print "B=",b,"+/-",eB[0],"(","%.0f"%(rB*100),"%)",
                    if strategy == 1:
                        print "C=",c,"+/-",eC[0],"(","%.0f"%(rC*100),"%)",
                        print "D=",d,"+/-",eD[0],"(","%.0f"%(rD*100),"%)",
                    print ## newline

                    # take the shape from anti and add it to the SR one. Do this after you compute a and b
                    htmp.Add(htmpD["anti"])

                # close open files
                for f in fInD: fInD[f].Close()

            htmp = self._manipulate_histo(htmp,y,normalization)
            if h: h.Add(htmp.Clone(rename))
            else: h=htmp.Clone(rename)
            if h==None:
                print "ERROR","Unable to get file",fname,hname,"->",rename
            fIn.Close()

        if opt.aqgc and h:
            ## including overflow for aqgc
            h.SetBinContent( h.GetNbinsX(), h.GetBinContent( h.GetNbinsX()) + h.GetBinContent(h.GetNbinsX()+1))

        if h:
            totbin = h.GetNbinsX()
            #h.Rebin(totbin)
            if "Up" not in hname and "Down" not in hname: print hname, h.Integral(), h.GetBinError(1), "unc.", h.GetBinError(1)/h.Integral()*100
            return h
        else: return None

    def _write(self, o ):
        if o==None and self.verbose> 0: print "ERROR","unable to write none object"
        if o == None: return 
        if self.verbose >2: print "DEBUG", "writing object",o.GetName()
        o.Write()
        self.garbage.append(o) ## avoid destruction

    def play_binning(self):
	h = None
	hsig = None
	for cat in self.categories:
            d=self.categories[cat]
            # proc
            for proc in self.processes:
                d2=self.processes[proc]
                if cat not in d2['cat']:
                    if self.verbose >2: print "DEBUG","excluding",cat,proc
                    continue
                for suffix in d2['suffix']:
		    if not d2['issig']:
                        if h==None:
                            h=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                        else:
                            hTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                            h.Add(hTmp)
		    else:
                        if hsig==None:
                            hsig=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                        else:
                            hsigTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                            hsig.Add(hsigTmp)

        #### current bin strategy: sig_err/sig < 20%; bkg_err/bkg < 20%; num_bkg > 1
        #### Not too aggresive: control on tot_bkg yields, but not on individual ones
        mapping = likelihoodBinning.createMapping(h,hsig)
        return mapping

    def creat_QCD_Shape(self,h_nominal,systname="") :
        Area   = 1.52e-01
        mshift = 1.13
        bcut   = 1.06

        if 'BBtag' in systname:
            Area   = -9.27
            mshift = 45410
            bcut   = 100.5

        h=h_nominal.Clone(systname)

        normold = h.Integral()


        for iBin in range(1,h.GetNbinsX()+1):
            cen = h.GetBinContent(iBin)
	    bdt = h.GetBinCenter(iBin)

	    scaleUp = Area * math.log(bdt + mshift) + bcut
	    scaleDn = max(2 - scaleUp, 0.001)

            if 'Up' in systname:   h.SetBinContent(iBin,cen*scaleUp)
	    if 'Down' in systname: h.SetBinContent(iBin,cen*scaleDn)

        normnew = h.Integral()
        h.Scale(normold/normnew)

        return h


    def write_inputs(self,outname): #datacard.txt
        outroot=re.sub(".txt.*",".inputs.root",outname)
        self.fOut=ROOT.TFile.Open(outroot,"RECREATE")
        self.fOut.cd()
        info=ROOT.TNamed("info","date %s\n"%(datetime.now().strftime("%Y/%m/%d %H:%M")) )

        self._write(info)

        if opt.aqgc: 
            LikelihoodMapping = FwRebin.SimpleRebin(10)
        else:
            LikelihoodMapping = self.play_binning()


        for cat in self.categories:
            d=self.categories[cat]
            # data
            data=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_%(suffix)s"%d,"%(cat)s_data_obs"%d)
            if data==None: 
                print "[WARNING] Creating fake data histogram for",cat
                xmin,xmax=0,100
                if opt.aqgc: xmin=500.,5000.
                data=ROOT.TH1D("%(cat)s_data_obs"%d,"fake data histogram with 100 bins",100,xmin,xmax)
                if opt.aqgc:
                    data = likelihoodBinning.applyMapping(LikelihoodMapping, data)
            else:
                data =  likelihoodBinning.applyMapping(LikelihoodMapping, data)
            self._write(data)
            
            # redefine self.processes to write if it has keywords
            myprocesses = {}
            for proc in self.processes:
                d2 = self.processes[proc]
                if d2['keywords'] == None: myprocesses[proc] = d2
                else:
                    # for each key, loop over values
                    l = [ d2['keywords'][key] for key in sorted(d2['keywords']) ] # list of list of all possibilities
                    for aval in itertools.product(*l):
                        newproc = proc +"_" + '_'.join(aval) # proc_$KEY1_$KEY2...
                        dnew= copy.deepcopy(d2) 
                        dnew['proc'] = newproc
                        for isuf,suf in enumerate(dnew['suffix']): 
                            for ikey,key in enumerate(sorted(d2['keywords'])):
                                ## key -> val
                                dnew['suffix'][isuf] = dnew['suffix'][isuf].replace(key,aval[ikey]) 
                        dnew['iskeyword'] = True ## carry out this information. Not used for the moment.
                        dnew['interpolate'] = aval[0] in opt.aqgc_interpolate  ## INTERPOLATE
                        if dnew['interpolate']:
                            if len(d2['keywords']) != 1 : raise ValueError("Unimplemented")
                            dnew['iinfo'] = {} ## interpolation info
                            dnew['iinfo']['point'] = aval[0] ## INTERPOLATE
                            lo,hi= find_closest_values(aqgc_par, dnew['iinfo']['point'])
                            dnew['iinfo']['lo'] = lo
                            dnew['iinfo']['hi'] = hi
                            dnew['iinfo']['suffix'] = []
                            for suf in d2['suffix']:
                                key = sorted(d2['keywords'])[0]
                                suf1 = copy.deepcopy(suf)
                                suf2= copy.deepcopy(suf)
                                suf1=suf1.replace(key, aqgc_ftoa(lo))
                                suf2=suf2.replace(key, aqgc_ftoa(hi))
                                dnew['iinfo']['suffix'].append( (suf1,suf2) )
                        myprocesses[newproc]=dnew
            # proc
            for proc in myprocesses:
                d2=myprocesses[proc]
                if cat not in d2['cat']: 
                    if self.verbose >2: print "DEBUG","excluding",cat,proc
                    continue
                h = None
                for isuf,suffix in enumerate(d2['suffix']):
                    if h==None:
                        ##fname from category
                        if d2['fname'] != None: 
                            print "WARNING","Using file (instead of default) ", "%(fname)s"%d2 ,"for proc", proc
                            if d2['interpolate']:
                                print "WARNING","INTERPOLATING ", "%(fname)s"%d2 ,"for proc", proc, d2['iinfo']
                                fname="%(path)s"%d +"/%(fname)s"%d2
                                hname1 = "%(base)s_"%d+d2['iinfo']['suffix'][isuf][0]
                                hname2 = "%(base)s_"%d+d2['iinfo']['suffix'][isuf][1]
                                h=self._get_interpolated_histo(fname,"%(cat)s_"%d+proc, (d2['iinfo']['lo'],hname1),(d2['iinfo']['hi'],hname2),d2['iinfo']['point'])
                            else:
                                h=self._get_histo("%(path)s"%d +"/%(fname)s"%d2,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                        else:
                            h=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                    else:
                        if d2['fname'] != None: 
                            print "WARNING","Using file (instead of default) ", "%(fname)s"%d2 ,"for proc", proc
                            if d2['interpolate']:
                                print "WARNING","INTERPOLATING ", "%(fname)s"%d2 ,"for proc", proc, d2['iinfo']
                                fname="%(path)s"%d +"/%(fname)s"%d2
                                hname1 = "%(base)s_"%d+d2['iinfo']['suffix'][isuf][0]
                                hname2 = "%(base)s_"%d+d2['iinfo']['suffix'][isuf][1]
                                hTmp=self._get_interpolated_histo(fname,"", (d2['iinfo']['lo'],hname1),(d2['iinfo']['hi'],hname2),d2['iinfo']['point'])
                            else:
                                hTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix,"")
                        else:
                            hTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                        h.Add(hTmp)
                h = likelihoodBinning.applyMapping(LikelihoodMapping, h)
                self._write(h)

                # SYST TODO implement interpolation logic
                for sname in self.systs:
                    s=self.systs[sname]
                    if 'shape' not in s['type']: continue


                    # check if matched
                    matched = False
                    SystName = ''
                    for (c,p),v,f,inh in zip(s['cat-proc'],s['value'],s['fname'],s['inname']) :
                        if re.match(c,cat) and re.match(p,proc) and v > 0:
                            matched = True
                            SystName = inh
                    if not matched: continue

                    hup = None
                    hdn = None
                    for suffix in d2['suffix']:

			# QCDnonclosure shape
                        if 'QCDnonclosure' in sname:
                            hnom = self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                            hup = self.creat_QCD_Shape(hnom,"%(cat)s_"%d+proc+"_"+sname+"Up")
                            hdn = self.creat_QCD_Shape(hnom,"%(cat)s_"%d+proc+"_"+sname+"Down")
			    continue

                        if hup==None:
                            ## todo here, check for systs fname here. Possible symmetrize to the default over there
                            if d2['fname'] != None: 
                                if d2['interpolate']: raise RuntimeError("Unimplemented")
                                # if fname set in proc, use it.
                                hup=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                                hdn=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
                            else:
                                # use category fname
                                hup=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                                hdn=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
                            if hup != None and hdn == None:
                                hdn = hup.Clone(re.sub("Up$","Down",hup.GetName()))
                                hdn.Reset("ACE")
                            if hdn != None and hup == None:
                                hup = hdn.Clone(re.sub("Down$","Up",hdn.GetName()))
                                hup.Reset("ACE")
                        else:
                            if d2['fname'] != None: 
                                if d2['interpolate']: raise RuntimeError("Unimplemented")
                                hupTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                                hdnTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
                            else:
                                hupTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Up","")
                                hdnTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Down","")
                            hup.Add(hupTmp)
                            hdn.Add(hdnTmp)
                            #if matched: print "WARNING", "syst duplicate found","discarding",c,p,v,"matching for",cat,proc
                            #$CHANNEL_$PROCESS_$SYSTEMATIC

                    hup = likelihoodBinning.applyMapping(LikelihoodMapping, hup)
                    hdn = likelihoodBinning.applyMapping(LikelihoodMapping, hdn)
                 
                    hup.Write()
                    hdn.Write()
                    self._write(hup)
                    self._write(hdn)
        self.fOut.Close()
        return self
                            

if __name__=="__main__":

    sigprocess = read_input() 

    db=DatacardBuilder(opt.verbose)
   
    base_path = '/eos/user/d/dalfonso/AnalysisVBS/NANO/MAR24syst'
    #base_path = '/eos/user/h/hum/VBSHad'
    if os.environ['USER'] == "amarini":
        base_path="Datacards/inputs/MAR24" 

    ## set categories
    ## when no data, "data" can be substituted with any process, will not affect obtaining expected results
    #db.add_category(opt.category,"/eos/user/h/hum/VBSHad","VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"Data",opt.input) 
    db.add_category(opt.category,base_path,"VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"TT_TuneCP5",opt.input)

    ## set bkg processes
    #db.add_process('top',False,['TT_TuneCUETP8M2T4','ST','TTX','TTJets'],[opt.category])
    #db.add_process('ttbar',False,['TTTo2L2Nu_TuneCP5','TTToSemiLeptonic_TuneCP5','TTToHadronic_TuneCP5','TT_Mtt-700to1000_TuneCP5','TT_Mtt-1000toInf_TuneCP5'],[opt.category])
    #db.add_process('ttbar',False,['TTTo2L2Nu_TuneCP5','TTToSemiLeptonic_TuneCP5','TTToHadronic_TuneCP5'],[opt.category])
    db.add_process('ttbar',False,['TT_TuneCP5'],[opt.category])
    #db.add_process('triBoson',False,['TRIBOSON'],[opt.category])
    #db.add_process('diBoson',False,['DIBOSON'],[opt.category])
    #db.add_process('Zinv',False,['ZJetsToNuNu'],[opt.category])
    #db.add_process('Winv',False,['WJetsToLNu'],[opt.category])
    if("MET" not in opt.category):
        db.add_process('QCD',False,['QCD_HT'],[opt.category])
    else:
        #db.add_process('diBoson',False,['DIBOSON'],[opt.category])
        #db.add_process('triBoson',False,['TRIBOSON'],[opt.category])
        db.add_process('Zinv',False,['ZJetsToNuNu_HT'],[opt.category])
        #db.add_process('Winv',False,['WJetsToLNu_HT','WJetsToLNu_NJ'],[opt.category])
        db.add_process('Winv',False,['WJetsToLNu_HT'],[opt.category])
        db.add_process('EWKV',False,['EWKW','EWKZ2Jets_ZToNuNu'],[opt.category])

    ## set sig processes
    for sig in sigprocess:
        if not opt.aqgc and 'AQGC' in sig['cont']: continue ## alerady in read_inputs

        if opt.quote == 1:
            if sig == sigprocess[0]: db.add_process(sig['pro']+sig['cont'],True,[sig['name']],[opt.category])
            else: db.add_process(sig['pro']+sig['cont'],False,[sig['name']],[opt.category])
        elif opt.quote == 2:
            if "EWK" in sig['cont']: db.add_process('VVEWK',True,[sig['name']],[opt.category])
            else: db.add_process('VVOther',False,[sig['name']],[opt.category])
        elif opt.quote == 3:
            if sig['pro'] == sigprocess[0]['pro']: db.add_process(sig['pro'],True,[sig['name']],[opt.category])
            else: db.add_process(sig['pro'],False,[sig['name']],[opt.category])
        elif opt.quote == 4:
            db.add_process('VV',True,[sig['name']],[opt.category])

        elif opt.quote == 5 and opt.aqgc: ## AQGC
            if "AQGC" in sig['cont']: 
                #MVV_BB_aQGC_WMJJWMJJjj_EWK_LO_NPle1_AQGC_ft9_0p00
                db.add_process('VV_AQGC_EWK',True,[ 'aQGC_'+sig['name']+'_NPle1_AQGC_'+aqgc_par+'_'+'$VALUE'],[opt.category],aqgc_fname)
                db.processes['VV_AQGC_EWK']['keywords']={
                        #'$VALUE': [ val for val in aqgc_values[aqgc_par] ] 
                        '$VALUE': [ val for val in aqgc_values[aqgc_par] ]  + opt.aqgc_interpolate  ### INTERPOLATE
                        }
            elif 'EWK' not in sig['cont']: 
                db.add_process('VV_QCD',False,[sig['name']],[opt.category])
                #db.add_process('VV_QCD_EWK',False,[sig['name']],[opt.category])
            else: 
                pass ## EWK is in AQGC, but there is a difference in the dataset names


    ## set systs
    db.add_systematics('lumi','','lnN',('.*','.*'),1.025)
    db.add_systematics('QCDScale_ttbar','','lnN',('.*','ttbar'),1.05)
    db.add_systematics('CMS_eff_trigger','','lnN',('.*','.*'),1.025)
    db.add_systematics('CMS_eff_l','','lnN',('.*','.*'),1.04)

    if "BBtag" in opt.category:
        if "side" not in opt.region: db.add_systematics('CMS_QCDnonclosure_s_BBtag','QCDNonclosure_BBtag','shape',('.*','QCD'),1.)  ## QCD shape
        if "anti" not in opt.region: db.add_systematics('CMS_QCDnonclosure_n_BBtag','','lnN',('.*','QCD'),1.20)  ## QCD Norm
    elif "RBtag" in opt.category:
        if "anti" not in opt.region: db.add_systematics('CMS_QCDnonclosure_n_RBtag','','lnN',('.*','QCD'),1.20)  ## QCD Norm
    elif "BB" in opt.category:
        if "side" not in opt.region: db.add_systematics('CMS_QCDnonclosure_s_BB','QCDNonclosure_BB','shape',('.*','QCD'),1.)  ## QCD shape
        if "anti" not in opt.region: db.add_systematics('CMS_QCDnonclosure_n_BB','','lnN',('.*','QCD'),1.20)


    proc_regex = '^((?!AQGC).)*$' if opt.aqgc else '.*'
    db.add_systematics('CMS_pileUp','PU','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_scale_j','JES_Total','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_scale_AK8j','JESAK8_Total','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_L1Prefire','L1Prefire','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_scale_uncluster','UNCLUSTER','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_CFERR1','BRCFERR1','shape',('.*',proc_regex),1.)
    #db.add_systematics('CMS_btag_CFERR2','BRCFERR2','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_HF','BRHF','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_HFSTAT1','BRHFSTAT1','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_HFSTAT2','BRHFSTAT2','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_LFSTAT1','BRLFSTAT1','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_LFSTAT2','BRLFSTAT2','shape',('.*',proc_regex),1.)
    db.add_systematics('CMS_btag_LF','BRLF','shape',('.*',proc_regex),1.)

    ## break down JES sources
    #db.add_systematics('jes_FlavorQCD','JES_FlavorQCD','shape',('.*','.*'),1.)
    #db.add_systematics('jes_RelativeBal','JES_RelativeBal','shape',('.*','.*'),1.)
    #db.add_systematics('jes_HF','JES_HF','shape',('.*','.*'),1.)
    #db.add_systematics('jes_BBEC1','JES_BBEC1','shape',('.*','.*'),1.)
    #db.add_systematics('jes_EC2','JES_EC2','shape',('.*','.*'),1.)
    #db.add_systematics('jes_Absolute','JES_Absolute','shape',('.*','.*'),1.)


    ######################################
    ###            WRITE               ###
    ######################################

    extra =""
    if opt.aqgc: extra+="_aqgc_"+aqgc_par

    db.write_cards('Datacards/MAR24/cms_vbshad_'+str(opt.year)+'_'+str(opt.quote)+extra+'_'+opt.analysisStra+'_'+opt.category+'_'+opt.region+'.txt')
    db.write_inputs('Datacards/MAR24/cms_vbshad_'+str(opt.year)+'_'+str(opt.quote)+extra+'_'+opt.analysisStra+'_'+opt.category+'_'+opt.region+'.txt')

#Local Variables:
#mode:c++
#indent-tabs-mode:nil
#tab-width:8
#c-basic-offset:8
#End:
#vim: tabstop=8 expandtab shiftwidth=8 softtabstop=8

