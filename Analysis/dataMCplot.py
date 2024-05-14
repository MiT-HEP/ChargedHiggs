import ROOT
from subprocess import check_output
import sys
import glob

'''
    if (category.find("BB")   !=string::npos) SetTreeVar("ana_category",1);
    if (category.find("RB")   !=string::npos) SetTreeVar("ana_category",2);
    if (category.find("BMET") !=string::npos) SetTreeVar("ana_category",3);
    if (category.find("RMET") !=string::npos) SetTreeVar("ana_category",4);
    if (category.find("BBtag")   !=string::npos) SetTreeVar("ana_category",5);
    if (category.find("RBtag")   !=string::npos) SetTreeVar("ana_category",6);
'''


def listDir(directory):
    print('HELLO')
    print(directory)
    counter = 0
    rootFiles = ROOT.vector('string')()
    maxFiles = 1000000000

    filename = (directory+'VBShadSignal_MET*.root')

    for filenames in glob.glob(filename):
        counter+=1
        if(counter > maxFiles): break
        rootFiles.push_back(filenames)

    print(len(rootFiles))
    return rootFiles


myDir='/eos/user/d/dalfonso/AnalysisVBS/NANO/MARCH9SYST/UL'
#dir='/eos/user/d/dalfonso/AnalysisVBS/NANO/MARCH9SYST/UL2018/METside/'

redDark = (191, 34, 41)
#   redMed = (255, 40, 0)

redMed = (237, 41, 57)
#   redMed = (220,0,5)
#   redLight = (255,61,65)

redLight = (255,82,82)
orange = (255, 204, 153)
orangeDark = (255,150,79)
gray = (136,139,141)
azure = (100, 192, 232)
azureDark = (96, 147, 172)
green = (144, 238, 144)
greenDark = (98, 172, 141)
gold = (212 ,175, 55)
violet = (181 ,100, 227)

lumis={
    12016: 19.52, #APV
    22016: 16.80, #postVFP
    2016: 35.9,
    2017: 41.5,
    2018: 60.0,
    2020: 45.7,
}

histos = []
hdata = []

def loopOverBKG(dfINI,myBkg,year):

    count = dfINI.Count().GetValue()
    print('nevents=',count)

    stringCategory='ana_category==3' #BMET
#    stringCategory='ana_category==4' #RMET    
    
    df = (dfINI.Filter("{1} and (({0}==-1 and isRealData==1) or (mc=={0} and mc>0))".format(myBkg,stringCategory)," select on the BKG or data"))
    
    if True:
        print("writing plots")
        hists = {
            "met_pt":  {"name":"met_pt","title":"missing energy; MET (GeV); N_{Events}","bin":80,"xmin":0.,"xmax":800.},
            "bosV2mass":  {"name":"bosV2mass","title":"bosV2mass; V2_{mass} (GeV); N_{Events}","bin":150,"xmin":0.,"xmax":150.},
            "bosV2discr":  {"name":"bosV2discr","title":"bosV2discr; V2_{discr} (GeV); N_{Events}","bin":100,"xmin":0.,"xmax":1.},            
        }

    for h in hists:
        model1d = (hists[h]["name"]+"_"+str(year)+"_"+str(myBkg), hists[h]["title"], hists[h]["bin"], hists[h]["xmin"], hists[h]["xmax"])
        h1d = df.Histo1D(model1d, hists[h]["name"],"weight")
        h1d.SetLineWidth(3)
        if myBkg>0:
            h1d.Scale(lumis[year] * 1000)
            histos.append(h1d)
        if myBkg<0: hdata.append(h1d)
        print("h1d append")

def anaOverYear(directory):
    
    year = directory
    if directory=='2016': year=22016
    if directory=='2016APV': year=12016    
    
    #https://github.com/MiT-HEP/ChargedHiggs/blob/8450730417bcfb0fc887209878ef197f9bc536e4/src/AnalysisVBShad.cpp#L3504
    '''
    200 = TTbar
    300 = ZJetsToNuNu_HT 
    310 = WJetsToLNu_HT
    301,302,303 = Zinv-Pt
    312 = W-Pt
    313 = DY-Pt
    '''
    
    mc = [ 200, 300, 310, 312, 301, -1]
    colors = [greenDark, gray, orange, violet, redLight, ROOT.kBlack]
    names = ['ttbar', 'ZNuNu_HT', 'WLNu_HT','Zinv-Pt', 'W-Pt', 'data']

    ###########
    ###########
    ###########
    
    files = listDir(myDir+str(directory)+'/MET/')

    df = ROOT.RDataFrame("tree_vbs", files)

    for sampleNOW in mc:
        print('processing mc = ', sampleNOW)
        loopOverBKG(df, sampleNOW,year)

    outputFileHisto = "histoOUTname_VBS_"+str(year)+".root"
    myfile = ROOT.TFile(outputFileHisto,"RECREATE")

    for h in histos:
        h_ = h.GetValue()
        h_.Write()
    for h in hdata:
        hdata_ = h.GetValue()
        hdata_.Write()
        
    myfile.Write()
    myfile.Close()
    print("h in a file ", outputFileHisto)

def makeFinalPlot():

    print('HELLO')
    ROOT.gStyle.SetOptStat(0)
    canv = ROOT.TCanvas("stackcanvas","Stack canvas",800,800)
    canv.SetLogy(1)
    stack = ROOT.THStack("stack","")
    dataStack = ROOT.THStack("dataStack","")    

#    mc = [ 200, 300, 310, 312, 301]
    mc = [ 200, 312, 301]    
#    colors = [greenDark, gray, orange, violet, redDark]
    colors = [greenDark, orange, redLight, ROOT.kBlack]
#redLight
#    names = ['ttbar', 'ZNuNu_HT', 'WLNu_HT', 'Zinv-Pt ', 'W-Pt'] 
    names = ['ttbar','Zinv-Pt', 'W-Pt', 'data']
    
    for mc_,color,name in zip(mc,colors,names):

        print('mc_',mc_)
        
        for y in ['2018', '2017', '22016', '12016']:

            print(y)
            outputFileHisto = "histoOUTname_VBS_"+str(y)+".root"                    
            f = ROOT.TFile.Open(outputFileHisto)
            h = f.Get('met_pt_'+str(y)+'_'+str(mc_))
            h.SetDirectory(0)
            print(name,' ', h.GetName(),' ',round(h.Integral(),1), " ", color)
            h.SetLineColor(ROOT.TColor.GetColor(*color))
            h.SetFillColor(ROOT.TColor.GetColor(*color))
            stack.Add(h)
#            print(name,' ', h.GetName(),' ',round(h.Integral(),1))

    print('number of histograms in the stack: ',stack.GetNhists())
    canv.cd()
    stack.Draw("hist") 

    for mc_ in [-1]:
        for y in ['2018', '2017', '22016', '12016']:

            print(y)
            outputFileHisto = "histoOUTname_VBS_"+str(y)+".root"
            f = ROOT.TFile.Open(outputFileHisto)
            h = f.Get('met_pt_'+str(y)+'_'+str(mc_))
            h.SetDirectory(0)            
            print('DATA = ', h.GetName(),' ',h.Integral())
            dataStack.Add(h)            

    hs = dataStack.GetStack().Last() 
    hs.Draw("p e same")

    canv.SaveAs("~/www/VBS/may3/met_pt_"+".png")

if __name__ == "__main__":

#    anaOverYear(2018)
#    anaOverYear(2017)
#    anaOverYear('2016')
#    anaOverYear('2016APV')        

    makeFinalPlot()
    exit()
