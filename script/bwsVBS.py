import ROOT, os, sys, re
from optparse import OptionParser
from datetime import datetime
import FwBinning as FwRebin



if __name__=="__main__":
    usage='''%prog 
    '''
    parser=OptionParser(usage = usage)
    parser.add_option("-v","--verbose",type='int',help="verbose",default=1)

    parser.add_option("-i","--input",type='string',help="input file name", default="HAD.root")

    parser.add_option("-y","--year",type='int',help="year; full run2 == 2020",default=2016)
    parser.add_option("-c","--category",type='string',help="5 category: BB, BMET, RMET, BBTAG, RBTAG", default="BB")
    parser.add_option("-s","--analysisStra",type='string',help="which one to fit with:mVV, mjj, BDT, ... ? ", default="mjj")

    parser.add_option("-q","--quote",type='int',help="sig ext.: 1: (WW/WZ/ZZ)+(ewk/qcd/int) all separate; 2: VVAll+(ewk/qcd/int); 3: (WW/WZ/ZZ)-All; 4: VV-All",default=1)

    opt,args=parser.parse_args()


lumis={
    2016: 35.9,
    2017: 41.5,
    2018: 60.0,
    2020: 137
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


LikeBins=25
likelihoodBinning = FwRebin.RebinLikelihood(LikeBins)



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
            if ("BB" in sig['name']):
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

    def add_process(self,x,issig,suffix=[],cat=[]):
        ''' add a process merging suffixes for cat in matching regex (empty == all) '''
        if x not in self.processes:
            self.processes[x] ={
                "proc": x, ## keep it also in the dictionary
                "suffix": suffix[:],
                "cat": cat[:],
                "issig":issig, ## this cannot change
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
        txt.write("* autoMCStats 0")

    def _manipulate_histo(self,htmp,corr ):
        htmp.Scale(lumis[opt.year] * 1000 * corr)
        #htmp.Rebin(2); ## REBIN --
        return htmp

    def _get_norm(self,hname):
        for sig in xsecsig:
            if hname in sig['name']:
                return sig['xsec']
        return 1.
            
    def _get_histo(self,fname,hname,rename=""):
        if self.verbose >2: print "DEBUG","opening file:",fname
        normalization = self._get_norm(hname)

        #if ("_BB_QCD_HT" in hname):
        #    fname="FinalHisto/HADanti.root"
        #    normalization = 0.0042

        fIn=ROOT.TFile.Open(fname)
        if self.fOut!=None: self.fOut.cd()
        if self.verbose >2: print "DEBUG","getting histo",hname,"->",rename
        htmp=fIn.Get(hname)
        if htmp==None and self.verbose >0: print "ERROR","unable to get histogram",hname,"from",fname
        if htmp==None: return None ## WARNING
        htmp = self._manipulate_histo(htmp,normalization)
        h=htmp.Clone(rename)
        fIn.Close()
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

        #### current bin strategy: sig_err/sig < 20%; bkg_err/bkg < 20%; num_bkg > 1
        #### Not too aggresive: control on tot_bkg yields, but not on individual ones
        mapping = likelihoodBinning.createMapping(h,hsig)
        return mapping


    def write_inputs(self,outname): #datacard.txt
        outroot=re.sub(".txt.*",".inputs.root",outname)
        self.fOut=ROOT.TFile.Open(outroot,"RECREATE")
        self.fOut.cd()
        info=ROOT.TNamed("info","date %s\n"%(datetime.now().strftime("%Y/%m/%d %H:%M")) )

        self._write(info)

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
            # proc
            for proc in self.processes:
                d2=self.processes[proc]
                if cat not in d2['cat']: 
                    if self.verbose >2: print "DEBUG","excluding",cat,proc
                    continue
                h = None
                for suffix in d2['suffix']:
                    if h==None:
                        h=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                    else:
                        hTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
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

    ## set categories
    ## when no data, "data" can be substituted with any process, will not affect obtaining expected results
    #db.add_category(opt.category,"/eos/user/h/hum/VBSHad","VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"Data",opt.input) 
    db.add_category(opt.category,"/eos/user/h/hum/VBSHad","VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,"Data",opt.input)

    ## set bkg processes
    #db.add_process('top',False,['TT_TuneCUETP8M2T4','ST','TTX','TTJets'],[opt.category])
    #db.add_process('ttbar',False,['TTTo2L2Nu_TuneCP5','TTToSemiLeptonic_TuneCP5','TTToHadronic_TuneCP5','TT_Mtt-700to1000_TuneCP5','TT_Mtt-1000toInf_TuneCP5'],[opt.category])
    db.add_process('ttbar',False,['TTTo2L2Nu_TuneCP5','TTToSemiLeptonic_TuneCP5','TTToHadronic_TuneCP5'],[opt.category])
    #db.add_process('triBoson',False,['TRIBOSON'],[opt.category])
    #db.add_process('diBoson',False,['DIBOSON'],[opt.category])
    #db.add_process('Zinv',False,['ZJetsToNuNu'],[opt.category])
    #db.add_process('Winv',False,['WJetsToLNu'],[opt.category])
    if("MET" not in opt.category): db.add_process('QCD',False,['QCD_HT'],[opt.category])


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


    ## set systs
    db.add_systematics('lumi','','lnN',('.*','.*'),1.025)
    db.add_systematics('QCDScale_ttbar','','lnN',('.*','ttbar'),1.05)
    db.add_systematics('CMS_eff_trigger','','lnN',('.*','.*'),1.05)
    db.add_systematics('CMS_QCDnonclosure_n','','lnN',('.*','QCD'),1.20)  ## QCD Norm
    if ("BB" in opt.category) and ("Btag" not in opt.category): db.add_systematics('CMS_QCDnonclosure_s_BB','QCDNonclosure_BB','shape',('.*','QCD'),1.)  ## QCD shape
    elif ("BBtag" in opt.category): db.add_systematics('CMS_QCDnonclosure_s_BBtag','QCDNonclosure_BBtag','shape',('.*','QCD'),1.)
    db.add_systematics('CMS_pileUp','PU','shape',('.*','.*'),1.)
    db.add_systematics('CMS_scale_j','JES_Total','shape',('.*','.*'),1.)
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

    db.write_cards('cms_vbshad_'+str(opt.year)+'_'+opt.analysisStra+'_'+opt.category+'.txt')
    db.write_inputs('cms_vbshad_'+str(opt.year)+'_'+opt.analysisStra+'_'+opt.category+'.txt')



