#!/usr/bin/env python
import os, sys, re
from optparse import OptionParser
from datetime import datetime
import itertools, copy ## for keywords

if __name__=="__main__":
    usage='''%prog 
    '''
    parser=OptionParser(usage = usage)
    parser.add_option("-v","--verbose",type='int',help="verbose",default=1)

    parser.add_option("-i","--input",type='string',help="input file name", default="HAD.root")

    parser.add_option("-y","--year",type='int',help="year; full run2 == 2020",default=2016)
    parser.add_option("-c","--category",type='string',help="5 category: BB, BMET, RMET, BBTAG, RBTAG", default="BB")
    parser.add_option("-s","--analysisStra",type='string',help="which one to fit with:mVV, mjj, BDT, ... ? ", default="mjj")

    parser.add_option("","--aqgc",action='store_true',help="AQGC input and datacard [%default]",default=False) ### aqgc code wrappend by this guardw
    parser.add_option("-q","--quote",type='int',help="sig ext.: 1: (WW/WZ/ZZ)+(ewk/qcd/int) all separate; 2: VVAll+(ewk/qcd/int); 3: (WW/WZ/ZZ)-All; 4: VV-All ; 5:AQGC",default=1)

    opt,args=parser.parse_args()

######### after option parser, so we have --help
import ROOT
import FwBinning as FwRebin


lumis={
    2016: 35.9,
    2017: 41.5,
    2018: 60.0,
    2020: 45.7
}


## [ssww, osww, zzjj, wzbb, wznn]/[ewk,qcd,ewk-qcd] 
xsecsig = [
{"pro" : "ssWW", "cont": "EWK", "name": "WPMJJWPMJJjj_EWK_LO", "xsec" : 0.13},
#{"pro" : "ssWW", "cont": "QCD", "name": "WPMJJWPMJJjj_QCD_LO", "xsec" : 0.11},
#{"pro" : "ssWW", "cont": "INT", "name": "WPMJJWPMJJjj_EWK_QCD_LO", "xsec" : 0.24},
{"pro" : "osWW", "cont": "EWK", "name": "WPJJWMJJjj_EWK_LO", "xsec" : 1.89},
#{"pro" : "osWW", "cont": "QCD", "name": "WPJJWMJJjj_QCD_LO", "xsec" : 9.79},
#{"pro" : "osWW", "cont": "INT", "name": "WPJJWMJJjj_EWK_QCD_LO", "xsec" : 11.65},
{"pro" : "ZZ", "cont": "EWK", "name": "ZJJZJJjj_EWK_LO", "xsec" : 0.06},
#{"pro" : "ZZ", "cont": "QCD", "name": "ZJJZJJjj_QCD_LO", "xsec" : 1.13},
#{"pro" : "ZZ", "cont": "INT", "name": "ZJJZJJjj_EWK_QCD_LO", "xsec" : 1.19},
{"pro" : "WZ", "cont": "EWK", "name": "ZBBWPMJJjj_EWK_LO", "xsec" : 0.13},
#{"pro" : "WZ", "cont": "QCD", "name": "ZBBWPMJJjj_QCD_LO", "xsec" : 1.33},
#{"pro" : "WZ", "cont": "INT", "name": "ZBBWPMJJjj_EWK_QCD_LO", "xsec" : 1.46},
{"pro" : "WZ", "cont": "EWK", "name": "ZNuNuWPMJJjj_EWK_LO", "xsec" : 0.17}
#{"pro" : "WZ", "cont": "QCD", "name": "ZNuNuWPMJJjj_QCD_LO", "xsec" : 1.78},
#{"pro" : "WZ", "cont": "INT", "name": "ZNuNuWPMJJjj_EWK_QCD_LO", "xsec" : 1.95},
]

if opt.aqgc:

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

    aqgc_quotes=[5]
    if opt.quote not in aqgc_quotes: raise ValueError("AQGC and quote=5 is required")
    ##  examples "fs1_m112p00","fs1_m104p00",
    ### CONFIGURATION. I need to import the aqgc parameters that I will run on. 
    ### the list of all possible reweightings are in aqgc_names. 
    ### this name structure is  parameter_value with usual masks for strings 
    ## TODO check if import correctly
    sys.path.insert(0,"./")
    from ParseDat import aqgc_names
    aqgc_parameters = list(set([ x.split('_')[0] for x in aqgc_names ]))
    aqgc_values = { par:[ name.split('_')[1] for name in aqgc_names if name.split('_')[0] == par ] for par in aqgc_parameters } ## par -> list of value_str , value
    aqgc_fname=re.sub('.root','aqgc.root',opt.input)
    print "Using AQGC file name",aqgc_fname
    aqgc_par='ft7'
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



LikeBins=25
likelihoodBinning = FwRebin.RebinLikelihood(LikeBins) if not opt.aqgc else FwRebin.SimpleRebin(2)



def read_input():
    psig = []

    for sig in xsecsig:
        if ("BB" in opt.category) and ("Btag" not in opt.category):
            if ("WW" in sig['pro']) or ("ZJJZJJ" in sig['name']) or ("WJJZJJ" in sig['name']) :
                psig.append(sig)
        elif "MET" in opt.category:
            if ("NuNu" in sig['name']):
                psig.append(sig)
        elif "Btag" in opt.category:
            #if ("BB" in sig['name']):
            if ("BB" in sig['name']) or ("ZJJZJJ" in sig['name']):
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
            #if opt.aqgc and sig['cont'] == 'EWK' : return 1. ## how they are normalized?
            if sig['name'] in hname:
                return sig['xsec']
        return 1.
            
    def _get_histo(self,fname,hname,rename=""):
        normalization = self._get_norm(hname)
        #normalization = 1.
        #print hname,normalization

        #if ("_QCD_HT" in hname):
        #    fname="/eos/user/h/hum/VBSHad/ResultsV0_BB_CR_2016_QCD.root"
            #normalization = 0.0042
        #    normalization = 0.015

        #if ("JJjj" in hname):
        #    fname="/eos/user/h/hum/VBSHad/ResultV2_taufix_BDT_BB_2018_Sig.root"


        qcd_bb_sf={
                2016: 0.014, #0.0111,#0.0122,
                2017: 0.007, #0.0064, #0.0086,
                2018: 0.02,  #0.0207, #0.0237,
                2020: 0.015
        }

        qcd_bbtag_sf={
            2016: 0.0990, #0.0624,
            2017: 0.0272, #0.0289,
            2018: 0.0988, #0.0899,
            2020: 0.015
        }


        years = [opt.year]
        if opt.year==2020: years = [2016, 2017, 2018]
        print years 

        h = None
        for y in years:
            ftmp = fname.replace(str(opt.year), str(y))

            fIn=ROOT.TFile.Open(ftmp)
            if self.verbose >2: print "DEBUG","opening file:",ftmp
            if self.fOut!=None: self.fOut.cd()
            if self.verbose >2: print "DEBUG","getting histo",hname,"->",rename
            htmp=fIn.Get(hname)
            if htmp==None and self.verbose >0: print "ERROR","unable to get histogram",hname,"from",ftmp
            if htmp==None: return None ## WARNING

            if ("_QCD_HT" in hname and "BBtag" in opt.category): normalization = qcd_bbtag_sf[y]   
            elif ("_QCD_HT" in hname and "BB" in opt.category): normalization = qcd_bb_sf[y]

            htmp = self._manipulate_histo(htmp,y,normalization)
            if h: h.Add(htmp.Clone(rename))
            else: h=htmp.Clone(rename)
            fIn.Close()


        ####
        #for b in range(1, h.GetNbinsX()+1):
        #    if b <= 45: h.SetBinContent(b,0.)
        ####
        print hname, h.Integral()

        return h

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

        #### current bin strategy: sig_err/sig < 20%; bkg_err/bkg < 20%; num_bkg > 1; min bin_width
        #### Not too aggresive: control on tot_bkg yields, but not on individual ones
        mapping = likelihoodBinning.createMapping(h,hsig)
        return mapping


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
                data=ROOT.TH1D("%(cat)s_data_obs"%d,"fake data histogram with 100 bins",100,0,100)
            else:
                data =  likelihoodBinning.applyMapping(LikelihoodMapping, data)
            self._write(data)
            
            # redefine self.processes to write if it has keywords
            print "DEBUG KEYWORD"
            myprocesses = {}
            for proc in self.processes:
                d2 = self.processes[proc]
                if d2['keywords'] == None: myprocesses[proc] = d2
                else:
                    #key_str="_".join( sorted([key for key in d2['keywords']]) )
                    # for each key, loop over values
                    print "DEBUG KW","ORIGINAL SUFFIXES",','.join(d2['suffix'])
                    l = [ d2['keywords'][key] for key in sorted(d2['keywords']) ] # list of list of all possibilities
                    for aval in itertools.product(*l):
                        newproc = proc +"_" + '_'.join(aval) # proc_$KEY1_$KEY2...
                        #dnew= d2.copy()
                        dnew= copy.deepcopy(d2) 
                        print "DEBUG KW","DNEW ",','.join(dnew['suffix'])
                        dnew['proc'] = newproc
                        for isuf,suf in enumerate(dnew['suffix']): 
                            #dnew['suffix'][isuf] = d2['suffix'][isuf][:] ## need to make a copy of this
                            for ikey,key in enumerate(sorted(d2['keywords'])):
                                ## key -> val
                                dnew['suffix'][isuf] = dnew['suffix'][isuf].replace(key,aval[ikey]) #re.sub(key,aval[ikey],dnew['suffix'][isuf]) I may need to escape $
                        print "DEBUG KW","DNEW AFTER SUB",','.join(dnew['suffix'])
                        myprocesses[newproc]=dnew
            # proc
            for proc in myprocesses:
                d2=myprocesses[proc]
                if cat not in d2['cat']: 
                    if self.verbose >2: print "DEBUG","excluding",cat,proc
                    continue
                h = None
                for suffix in d2['suffix']:
                    if h==None:
                        ##fname from category
                        h=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                        if h==None and d2['fname'] != None: 
                            ## try with the fname in proc. 
                            ### TODO: considering first choice iff set?
                            ### TODO: propagate this to syst
                            print "DEBUG","Falling back to ", "%(fname)s"%d2 ,"for proc", proc
                            h=self._get_histo("%(path)s"%d +"/%(fname)s"%d2,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                    else:
                        hTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                        if hTmp==None and d2['fname'] != None: 
                            print "DEBUG","Falling back to ", "%(fname)s"%d2 ,"for proc", proc
                            hTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix,"")
                        h.Add(hTmp)
                h = likelihoodBinning.applyMapping(LikelihoodMapping, h)
                self._write(h)

                # SYST
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
                        if hup==None:
                            hup=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                            hdn=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
                            if hup==None and hdn==None and d2['fname'] != None: 
                                hup=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                                hdn=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
                        else:
                            hupTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Up","")
                            hdnTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix+"_"+SystName+"Down","")
                            if hupTmp==None and hdnTmp==None and d2['fname'] != None: 
                                hupTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Up","%(cat)s_"%d+proc+"_"+sname+"Up")
                                hdnTmp=self._get_histo("%(path)s"%d+"/%(fname)s"%d2,"%(base)s_"%d+suffix+"_"+SystName+"Down","%(cat)s_"%d+proc+"_"+sname+"Down")
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
    
    base_path = '/eos/user/h/hum/VBSHad'
    if os.environ['USER'] == "amarini":
        base_path="Datacards/inputs"

    ## set categories
    ## when no data, "data" can be substituted with any process, will not affect obtaining expected results
    #db.add_category(opt.category,"/eos/user/h/hum/VBSHad","VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"Data",opt.input) 
    db.add_category(opt.category,base_path,"VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"TT_TuneCP5",opt.input)

    ## set bkg processes
    #db.add_process('top',False,['TT_TuneCUETP8M2T4','ST','TTX','TTJets'],[opt.category])
    #db.add_process('ttbar',False,['TTTo2L2Nu_TuneCP5','TTToSemiLeptonic_TuneCP5','TTToHadronic_TuneCP5','TT_Mtt-700to1000_TuneCP5','TT_Mtt-1000toInf_TuneCP5'],[opt.category])
    db.add_process('ttbar',False,['TT_TuneCP5','Other'],[opt.category])
    #db.add_process('triBoson',False,['TRIBOSON'],[opt.category])
    #db.add_process('diBoson',False,['DIBOSON'],[opt.category])
    if("MET" not in opt.category):
        db.add_process('QCD',False,['QCD_HT'],[opt.category])
    else:
        #db.add_process('triBoson',False,['TRIBOSON'],[opt.category])
        db.add_process('Zinv',False,['ZJetsToNuNu_HT'],[opt.category])
        #db.add_process('Winv',False,['WJetsToLNu_HT','WJetsToLNu_NJ'],[opt.category])
        db.add_process('Winv',False,['WJetsToLNu_HT'],[opt.category])
        #db.add_process('EWKV',False,['EWKW','EWKZ2Jets_ZToNuNu'],[opt.category])

    ## set sig processes
    for sig in sigprocess:
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
            #aqgc_parameters = list(set([ x.split('_')[0] for x in aqgc_names ]))
            #aqgc_values
            #def add_process(self,x,issig,suffix=[],cat=[],fname=None):
            #NPle1_aQGC_AQGC_ft7_m10p00   
            #SM='0p00' #should we use the SM value from the Nominal samples or from the aQGC?
            #for par in aqgc_parameters:
            if "EWK" in sig['cont']: 
                db.add_process('VV_AQGC_EWK',True,[sig['name']+'_NPle1_aQGC_AQGC_'+aqgc_par+'_'+'$VALUE'],[opt.category],aqgc_fname)
                db.processes['VV_AQGC_EWK']['keywords']={
                        '$VALUE': [ val for val in aqgc_values[aqgc_par] ] 
                        }
            else: 
                db.add_process('VV_QCD',False,[sig['name']],[opt.category])
                db.add_process('VV_QCD_EWK',False,[sig['name']],[opt.category])


    ## set systs
    db.add_systematics('lumi','','lnN',('.*','.*'),1.025)
    db.add_systematics('QCDScale_ttbar','','lnN',('.*','ttbar'),1.05)
    db.add_systematics('CMS_eff_trigger','','lnN',('.*','.*'),1.025)
    #db.add_systematics('CMS_QCDnonclosure_n','','lnN',('.*','QCD'),1.20)  ## QCD Norm
    #if ("BB" in opt.category) and ("Btag" not in opt.category): db.add_systematics('CMS_QCDnonclosure_s_BB','QCDNonclosure_BB','shape',('.*','QCD'),1.)  ## QCD shape
    #elif ("BBtag" in opt.category): db.add_systematics('CMS_QCDnonclosure_s_BBtag','QCDNonclosure_BBtag','shape',('.*','QCD'),1.)

    db.add_systematics('CMS_pileUp','PU','shape',('.*','.*'),1.)
    db.add_systematics('CMS_scale_j','JES_Total','shape',('.*','.*'),1.)
    db.add_systematics('CMS_scale_AK8j','JESAK8_Total','shape',('.*','.*'),1.)

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
    if opt.aqgc: extra="_aqgc_"+aqgc_par
    db.write_cards('Datacards/NanoSepV2/cms_vbshad_'+str(opt.year)+'_'+str(opt.quote)+extra+'_'+opt.analysisStra+'_'+opt.category+'.txt')
    db.write_inputs('Datacards/NanoSepV2/cms_vbshad_'+str(opt.year)+'_'+str(opt.quote)+extra+'_'+opt.analysisStra+'_'+opt.category+'.txt')

#Local Variables:
#mode:c++
#indent-tabs-mode:nil
#tab-width:8
#c-basic-offset:8
#End:
#vim: tabstop=8 expandtab shiftwidth=8 softtabstop=8



