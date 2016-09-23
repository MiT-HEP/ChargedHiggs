import sys,os
from optparse import OptionParser
parser = OptionParser(usage="parse the datacard to get the differences on the nuisance yields")
#parser.add_option("-x","--XX",help=" [%default]",default="XXX")
parser.add_option("-f","--file",help="Input file. combine binary datacard [%default]",default="cms_datacard_ws_qcddata_Augustv4_M200.root")
parser.add_option("-b","--batch",action='store_true',help="Batch [%default]",default=True)
parser.add_option("-x","--no-batch",dest='batch',action='store_false',help="non batch")
parser.add_option("","--bin",dest='bin',help="bin name[%default]",default="cat0")
parser.add_option("","--processes",dest='processes',help="comma separeted processes [%default]",default=','.join(["DY","ST","TT","WJets","WW","WZ","ZZ","QCD","Hplus"]))
parser.add_option("","--collapse",dest='collapse',help="dictonary for counting [%default]",default="EWK:DY,WJets,WW,WZ,ZZ;Top:TT,ST;QCD:QCD;Hplus:Hplus")

opts,args= parser.parse_args()

sys.argv=[]
import ROOT

class WsHandler():
    def __init__(self,fileName):
        self.fileName=fileName
        self.fIn=None
        self.w=None
        self.nuisances=[]
        self.processes=[]
        self.collapse={}
        self.binname="cat0"
        ## bash color
        self.red="\033[01;31m"
        self.green = "\033[01;32m"
        self.yellow = "\033[01;33m"
        self.cyan = "\033[01;36m"
        self.white = "\033[00m"
        pass

    def _printError(self,mex):
        print self.red+"ERROR: "+self.white,mex

    def _printWarning(self,mex):
        print self.yellow+"WARNING: "+self.white,mex

    def _printInfo(self,mex):
        print self.cyan+"INFO: "+self.white,mex


    def _getWorkspace(self):
        ''' Implementation of getting workspace '''
        self.fIn=ROOT.TFile.Open(self.fileName)
        if self.fIn==None:
            self._printError("Unable to open file '" + self.fileName+"'")
            raise IOError
        self.w=self.fIn.Get("w")
        if self.w==None:
            self._printError("Unable to find Workspace 'w'")
            raise IOError
        return self

    def getWorkspace(self):
        ''' Get Workspace '''
        if self.fIn == None or self.w == None:
            return self._getWorkspace()
        else: return self

    def _getNuisances(self): 
        ''' Implementation Get list of nuisances '''
        self.nuisances = []
        argset=self.w.set("ModelConfig_NuisParams")
        if argset == None:
            self._printError("Unable to locate nuisances")
            raise IOError
        #for nuis in argset:
        #    self.nuisances.append( nuis.GetName() )
        iter = argset.createIterator()
        nuis = iter.Next()
        while nuis :
           if 'STAT' not in nuis.GetName():  ## don't look for STAT stuff
              self.nuisances.append( nuis.GetName() ) 
           nuis = iter.Next()
        return self

    def getNuisances(self):
        ''' Get Nuisances '''
        return self.getWorkspace()._getNuisances()

    def _getProcesses(self):
        ''' TODO, Read from datacard'''
        self.processes=["DY","ST","TT","WJets","WW","WZ","ZZ","QCD","Hplus"]
        self.collapse={ "EWK": ["DY","WJets","WW","WZ","ZZ"],"Top":["TT","ST"],"QCD":["QCD"],"Hplus":["Hplus"]}
        return self

    def getProcesses(self):
        if self.processes!=None:
            return self
        else:
            return self._getProcesses()

    def _snapshot(self,name,args=None):
         ''' args={key:value} '''
         if args==None:
            self.w.saveSnapshot(name,','.join(self.nuisances))
         else:
            argset=ROOT.RooArgSet()
            for key in args:
                v=self.w.var(key)
                v.setVal(args[key])
                argset.add(v)
            self.w.saveSnapshot(name,argset)
         return self

    def _getValue(self,snapshot,proc):
         self.w.loadSnapshot("nominal")
         self.w.loadSnapshot(snapshot)
         name="n_exp_final_bin"+self.binname+"_proc_"+proc
         #name="n_exp_bincat0_proc_"+proc
         if self.w.function(name) == None:
              self._printWarning("Unable to get function: '" +name +"'. It is correct if the process is not related to any shape uncertainties")
              name="n_exp_bin"+self.binname+"_proc_"+proc
              if self.w.function(name) == None:
                      self._printError( "Unable to get function: '" +name+ "' process not correct.")
         return self.w.function(name).getVal()

    def _getNorm(self):
         map = {} #construct reverse map in collapse
         for key in self.collapse:
            for proc in self.collapse[key]:
                map[proc]=key
         for proc in self.processes:
             if   proc not in map: 
                  map[proc]="tot"
                  if "tot" not in self.collapse: self.collapse["tot"]=[]
                  self.collapse["tot"].append( proc )

         self._snapshot("nominal")

         self.yields={} ## collapsed name,syst -> nominal,up,down

         for proc in self.processes:
             nominal= self._getValue("nominal",proc)
             #print "Yield for proc",proc,"is",nominal ### DEBUG
             if  (map[proc],None) not in self.yields : self.yields[ (map[proc],None) ] = 0.0
             self.yields[ (map[proc],None) ] += nominal
             for syst in self.nuisances:
                 newvalues={}
                 for shift in [1,-1]:
                     name=proc +"_" +syst + ("Up" if shift>0 else "Down")
                     self._snapshot( name, {syst:shift} )
                     newvalues[shift] = self._getValue(name, proc)
                 self._printInfo("Process " + proc + " Nuisance " + syst+ " is "+ "%.2f"%((newvalues[-1]/nominal-1)*100) + "/"+ "%.2f"%((newvalues[1]/nominal-1)*100))
                 if (map[proc],syst) not in self.yields:
                     self.yields[ map[proc],syst ] = [0.0,0.0,0.0]
                 #self.yields[ map[proc],syst ] += nominal, newvalues[1],newvalues[-1]
                 #print "DEBUG",self.yields[ map[proc],syst ],self.yields[ map[proc],syst ][0],nominal
                 self.yields[ (map[proc],syst) ][0] += nominal
                 self.yields[ (map[proc],syst) ][1] += newvalues[1]
                 self.yields[ (map[proc],syst) ][2] += newvalues[-1]

         return self

    def getNorm(self):
         return self.getProcesses().getNuisances()._getNorm()

    def _printSeparator(self): 
        print "-----------------------------"
        return self

    def printYields(self):
         self.getNorm()._printSeparator()
         #for proc, syst in self.yields:
         for proc in self.collapse :
            for syst in self.nuisances:
             print "Aggregate",proc,"syst",syst,"Yield=",self.yields[proc,syst][0],
             try:
                 print "Up",(self.yields[proc,syst][1]/self.yields[proc,syst][0]-1)*100,
             except ZeroDivisionError:
                 pass
             try:
                 print "Down",(self.yields[proc,syst][2]/self.yields[proc,syst][0]-1)*100,
             except ZeroDivisionError:
                 pass
             print  #close line
         return self

    def printDataYields(self):
         self.getWorkspace()
         #print "DATA YIELDS",self.w.data("data_obs").sumEntries()
         self.yields[ ("data",None) ] = self.w.data("data_obs").sumEntries()
         return self

    def write(self, name="/dev/stdout"):
         self.printYields().printDataYields()
         out = open(name,"w")
         for proc in self.collapse :
             out.write("%s %.1f\n"%(proc,self.yields[proc,None]))
         out.write("%s %.1f\n"%("data",self.yields["data",None]))
         out.close()
         return self

##########################end Handler ##########################

############
### MAIN ###
############

if __name__ == "__main__":
    if opts.batch:
        ROOT.gROOT.SetBatch()
    wh = WsHandler(opts.file)
    wh.binname=opts.bin
    wh.processes=opts.processes.split(',')
    wh.collapse={}
    for s in opts.collapse.split(';'):
          if len(s.split(":")) <2 : continue
          key=s.split(":")[0]
          wh.collapse[key] = s.split(":")[1].split(',')
    #wh.printYields().printDataYields()
    wh.write("yields.txt")

#Local Variables:
#mode:c++
#indent-tabs-mode:nil
#tab-width:4
#c-basic-offset:4
#End:
#vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 

