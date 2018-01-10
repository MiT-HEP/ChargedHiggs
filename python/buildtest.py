import os,sys
import re

import json
import requests

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output

parser = OptionParser(usage = "usage");
#parser.add_option("-y","--yes",dest="yes",type="int",help="Should I assume yes as an answer. [0-3],0 = ask, 1= almost always, 3= yes (dangerous).",default=0);
parser.add_option("-f","--token-file",dest="tokenfile",type="string",help="tokenfile [%default]",default=os.environ["HOME"]+"/.ssh/id_status_token");
parser.add_option("","--pr",dest="pr",type="string",help="PR number to force check [%default]",default="");
parser.add_option("-n","--dryrun",dest="dryrun",action='store_true',help="Only List PR.",default=False);

(opts,args) = parser.parse_args()

#########################################
import NeroProducer.Nero.GitHubHandler as github

class SetupError(Exception):
    pass
class BuildError(Exception):
    pass
class RunError(Exception):
    pass


class Loop:
    ## main loop
    def __init__(self):
        self.gh=github.GitHubHandler("MiT-HEP","ChargedHiggs")
        self.gh.read_token_fromfile(opts.tokenfile)
        self.pr_checked=[]
        
        # tmp area
        self.tmp = "/tmp/"+os.environ['USER'] +"/ChPr/"
        self.log = os.environ["HOME"] +"/www/ChargedHiggs/"
        self.url = "https://" +os.environ['USER']+".web.cern.ch/"+os.environ['USER']+'/ChPr'

    def _call(self,cmd,errtype="build",logdir = ""):
        #cmd += " 2>&1 | tee %(log)s/setup.txt"%dictionary
        if logdir=="":
            final_cmd=cmd
        else:
            final_cmd="{ "+ cmd +" ; } 2>&1 >>" + logdir + "/"+errtype +".txt"
        st = call(cmd,shell=True)
        if st != 0: 
            print "-> Unable to run '"+cmd+"'"
            if errtype=="setup":raise SetupError
            if errtype=="build":raise BuildError
            if errtype=="run": raise RunError
            raise ValueError
        return

    def TestPr(self,pr):

        self.gh.set_status(pr.sha,'pending','setup',self.url +"/" + pr.sha + "/setup.txt")
        self.gh.set_status(pr.sha,'pending','build',self.url +"/" + pr.sha + "/setup.txt")
        self.gh.set_status(pr.sha,'pending','run',self.url +"/" + pr.sha + "/setup.txt")

        ## delete and create target directory
        self._call('[ -d '+ self.tmp +' ] && rm -rf ' + self.tmp + " || true","setup",self.log +"/"+pr.sha)
        self._call(' '.join(["mkdir","-p",self.tmp]),"setup",self.log+"/"+pr.sha)

        ## will be used for substitutions
        dictionary = {'tmp':self.tmp,'origin':pr.origin,'sha':pr.sha,'log':self.log}
        self._call(' '.join(["mkdir","-p",dictionary["log"]]),"setup",self.log+"/"+pr.sha)

        ## get build test configuration
        cmd =  "rm %(tmp)s/config.txt || true ;" %dictionary
        cmd += "wget --no-check-certificate 'https://raw.githubusercontent.com/%(origin)s/%(sha)s/.buildtest.txt' -O %(tmp)s/config.txt"%dictionary 
        self._call(cmd,"setup",self.log+"/"+pr.sha)


        ## parse config file
        dictionary['cmssw'] = 'CMSSW_9_4_1'
        dictionary['tag'] = 'cmssw_94x'
        dictionary['coretag'] = 'CMSSW_92X'
        dictionary['combinetag'] = 'v7.0.5'
        dictionary['combine'] = 'yes'

        conf = open("%(tmp)s/config.txt"%dictionary)
        for line in conf:
            l = line.split('#')[0]
            l = l.split('\n')[0]
            if '=' in l: dictionary[l.split('=')[0]] = l.split('=')[1]
        
        combineString = dictionary['combine']
        if combineString.lower() == 'no':     dictionary['combine']=False
        if combineString.lower() == 'false':  dictionary['combine']=False
        if combineString.lower() == 'yes':    dictionary['combine']=True
        if combineString.lower() == 'true':   dictionary['combine']=True
        if combineString.lower() == 'ok':     dictionary['combine']=True
        
        # set up cmssw
        cmd = "cd %(tmp)s && scramv1 project CMSSW %(cmssw)s" %dictionary
        self._call(cmd,"setup",self.log+"/"+pr.sha)
        
        dictionary['cmsenv'] = "cd %(tmp)s/%(cmssw)s/src && eval `scramv1 runtime -sh`"%dictionary  

        if dictionary['combine']:
            #setup combine
            cmd = "%(cmsenv)s"%dictionary
            cmd +=" && git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit"
            cmd +=" && cd HiggsAnalysis/CombinedLimit"
            cmd +=" && git fetch origin"
            cmd +=" && git checkout %(combinetag)s"%dictionary
            patch ='''
--- a/src/SequentialMinimizer.cc
+++ b/src/SequentialMinimizer.cc
@@ -465,7 +465,7 @@ namespace cmsmath {
         public:
            SubspaceMultiGenFunction(const ROOT::Math::IMultiGenFunction *f, int nDim, const int *idx, double *xi) :
                 f_(f), nDim_(nDim), idx_(idx), x_(xi) {}
-           virtual IBaseFunctionMultiDim * Clone() const { return new SubspaceMultiGenFunction(*this); }
+           virtual ROOT::Math::IBaseFunctionMultiDim * Clone() const { return new SubspaceMultiGenFunction(*this); }
            virtual unsigned int NDim() const { return nDim_; }
         private:
            virtual double DoEval(const double * x) const {
            '''
            cmd += ' &&  patch -p1 <<EOF' 
            cmd += ' && scramv1 b clean; scramv1 b -j 8'
            cmd += "\n" +patch +"\nEOF\n"
            self._call(cmd,"setup",self.log+"/"+pr.sha)

        #setup core/nero
        if True:
            cmd = "%(cmsenv)s"%dictionary
            cmd +=" && git clone git@github.com:MiT-HEP/NeroProducer.git -b %(coretag)s"%dictionary
            cmd +=" && cd NeroProducer/Core"
            cmd +=" && make -j 16"
            self._call(cmd,"setup",self.log+"/"+pr.sha)

        #setup chargedHiggs
        if True:
            cmd = "%(cmsenv)s"%dictionary
            cmd +=" && git clone git@github.com:MiT-HEP/ChargedHiggs.git -b %(tag)s"%dictionary
            self._call(cmd,"setup",self.log+"/"+pr.sha)

        # merge PR
        if True:
            cmd = "%(cmsenv)s"%dictionary
            cmd += " && cd ChargedHiggs"
            cmd += " && git remote add pr git@github.com:%(origin)s"%dictionary
            cmd += " && git fetch pr "
            cmd += " && git merge --no-ff -m 'merge commit' %(sha)s  "%dictionary
            self._call(cmd,"setup",self.log+"/"+pr.sha)
        
        self.gh.set_status(pr.sha,'success','setup',self.url +"/" + pr.sha + "/setup.txt")
        # build PR
        if True:
            cmd = "%(cmsenv)s"%dictionary
            cmd += " && cd ChargedHiggs"
            cmd += " && make -j 4"
            self._call(cmd,"build",self.log+"/"+pr.sha)
        self.gh.set_status(pr.sha,'success','build',self.url +"/"+pr.sha+"/build.txt")

        if True:
            cmd = "%(cmsenv)s"%dictionary
            cmd += " && cd ChargedHiggs"
            cmd += " && python python/Loop.py -v -d dat/configBuildTest.dat"
            self._call(cmd,"run",self.log+"/"+pr.sha)

        self.gh.set_status(pr.sha,'success','run',self.url+"/"+pr.sha+"run.txt")
        ##
        return True

    def check_state(self,sha,to_check=['build','run']):
        state ='ok'
        for context in to_check:
            if (sha,context) not in self.gh.statuses: return 'tocheck'
            if self.gh.statuses[(sha,context)] == 'pending': return 'pend'
            if self.gh.statuses[(sha,context)] != 'success': return 'fail'
        return state

    def main(self):
        # -> check PR
        self.gh.get_pullreq()

        for pr in self.gh.pulls:
            if (pr.number,pr.sha) in self.pr_checked: 
                continue
            if opts.pr !="" and int(pr.number) != int(opts.pr) : 
                print "(V) -> Skipping PR",pr.number,"Request only PR",opts.pr
                continue

            print "(D) -> Considering PR",pr.number,pr.sha
            pr.Print()

            self.gh.get_statuses(pr.sha)

            for sha,context in self.gh.statuses:
                if  sha == pr.sha:
                    print "(V)-> PR has status",context,self.gh.statuses[(sha,context)].state ## success, pending ...
            
            ## define a global state
            state = self.check_state(pr.sha)

            ## if ok
            if state=='tocheck' or int(opts.pr) == int(pr.number) :
                print "(V) -> Testing PR",pr.number,pr.title
                if not opts.dryrun:
                    try:
                        self.TestPr(pr)   
                    except SetupError:
                        self.gh.set_status(pr.sha,'fail','setup',self.url+"/"+pr.sha+"/setup.txt")
                    except BuildError:
                        self.gh.set_status(pr.sha,'fail','build',self.url+"/"+pr.sha+"/build.txt")
                    except RunError:
                        self.gh.set_status(pr.sha,'fail','run',self.url+"/"+pr.sha+"/run.txt")

            if state=='ok': 
                print "(V)-> PR is OK",pr.number
                self.pr_checked.append( (pr.number,pr.sha) )
            if state=='fail': 
                print "(V)-> PR is failed",pr.number
                self.pr_checked.append( (pr.number,pr.sha) )
            if state=='pend':
                print "(V)-> PR is already being checked",pr.number

if __name__=="__main__":
    loop=Loop() 
    loop.main()
