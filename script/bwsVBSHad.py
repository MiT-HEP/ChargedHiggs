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

    parser.add_option("-c","--category",type='string',help="5 category: BB, BMET, RMET, BBTAG, RBTAG", default="BB")
    parser.add_option("-s","--analysisStra",type='string',help="which one to fit with:mVV, mjj, BDT, ... ? ", default="mjj")


    opt,args=parser.parse_args()


lumis=[]
lumis.append(137)


LikeBins=25
likelihoodBinning = FwRebin.RebinLikelihood(LikeBins)



def read_input():
    sig = []
    signame = []

    if ("BB" in opt.category) and ("Btag" not in opt.category):
        sig.append("WPhadWPhadJJ_EWK_LO")
        signame.append("SSWW")
    elif "MET" in opt.category:
        sig.append("ZnnZhadJJ_EWK_LO")
        sig.append("ZNuNuWPMJJjj_EWK_LO")
        sig.append("ZNuNuWPMJJjj_QCD_LO")
        signame.append("ZZ")
        signame.append("WZEWK")
        signame.append("WZQCD")
    elif "Btag" in opt.category:
        sig.append("ZbbZhadJJ_EWK_LO")
        sig.append("ZbbWpmJJ_EWK_LO")
        sig.append("ZbbWpmJJ_QCD_LO")
        signame.append("ZZ")
        signame.append("WZEWK")
        signame.append("WZQCD")

    return sig,signame

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
        htmp.Scale(lumis[0] * 1000 * corr)
        #htmp.Rebin(2); ## REBIN --
        return htmp

    def _get_histo(self,fname,hname,rename=""):
        if self.verbose >2: print "DEBUG","opening file:",fname
        normalization = 1.
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
	for cat in self.categories:
            d=self.categories[cat]
            # proc
            for proc in self.processes:
                d2=self.processes[proc]
                if cat not in d2['cat']:
                    if self.verbose >2: print "DEBUG","excluding",cat,proc
                    continue
                for suffix in d2['suffix']:
                    if h==None:
                        h=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"%(cat)s_"%d+proc)
                    else:
                        hTmp=self._get_histo("%(path)s/%(fname)s"%d,"%(base)s_"%d+suffix,"")
                        h.Add(hTmp)

        mapping = likelihoodBinning.createMapping(h,h)
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


    sigprocess, signame = read_input() 


    db=DatacardBuilder(opt.verbose)
    #def add_category(self,x,path, basename,obs_suffix,fname=opts.fname):
    #db.add_category("BMET",'FinalHisto','VBShadAnalysis/MVV_BMET','ZnnZhadJJ_EWK_LO','METmulticlass.root')
    db.add_category(opt.category,"FinalHisto/Dec21","VBShadAnalysis/"+opt.analysisStra+"_"+opt.category,sigprocess[0],opt.input) ## if have data, sigprocess[0] be substituted with Data
    #db.add_category("BBtag",'FinalHisto','VBShadAnalysis/MVV_BBtag','ZbbZhadJJ_EWK_LO','BHAD.root')


    #add_process(self,x,issig,suffix=[],cat=[]):


    db.add_process('ttbar',False,['ST'],[opt.category])
    #db.add_process('ttbar',False,['TT_TuneCUETP8M2T4','ST','TTX','TTJets'],[opt.category])
    db.add_process('VVV',False,['TRIBOSON'],[opt.category])
    db.add_process('VV',False,['DIBOSON'],[opt.category])
    if("MET" not in opt.category): db.add_process('QCD',False,['QCD_HT'],[opt.category])
    db.add_process('Zinv',False,['ZJetsToNuNu'],[opt.category])
    db.add_process('Winv',False,['WJetsToLNu'],[opt.category])


    for sig,name in zip(sigprocess,signame):
        if sig == sigprocess[0]: db.add_process(name,True,[sig],[opt.category])
	else: db.add_process(name,False,[sig],[opt.category])

        #db.add_process('ZZ',True,['ZnnZhadJJ_EWK_LO'],['metR'])
        #db.add_process('WZEWK',False,['ZNuNuWPMJJjj_EWK_LO'],['metR'])
        #db.add_process('WZQCD',False,['ZNuNuWPMJJjj_QCD_LO'],['metR'])
        #db.add_process('ZZ',True,['ZbbZhadJJ_EWK_LO'],['RBtag'])
        #db.add_process('WZEWK',False,['ZbbWpmJJ_EWK_LO'],['RBtag'])
        #db.add_process('WZQCD',False,['ZbbWpmJJ_QCD_LO'],['RBtag'])



    #db.add_systematics('lumi','','lnN',('.*','.*'),1.025)
    #db.add_systematics('QCDScale_ttbar','','lnN',('.*','ttbar'),1.05)
    #db.add_systematics('scale_j','JES','shape',('.*','.*'),1.)


    ######################################
    ###            WRITE               ###
    ######################################

    db.write_cards('Datacards/SYST/cms_vbshad_'+opt.analysisStra+'_'+opt.category+'.txt')
    db.write_inputs('Datacards/SYST/cms_vbshad_'+opt.analysisStra+'_'+opt.category+'.txt')



