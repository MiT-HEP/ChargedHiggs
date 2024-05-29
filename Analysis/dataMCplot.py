import ROOT
from subprocess import check_output
import sys
import glob

ROOT.ROOT.EnableImplicitMT()
ROOT.gROOT.SetBatch()
RDataFrame = ROOT.RDataFrame
#year = 2018

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

    filename = (directory+'MET.root')
    if "side" in directory:
        filename = (directory+'METside.root')

    for filenames in glob.glob(filename):
        counter+=1
        if(counter > maxFiles): break
        rootFiles.push_back(filenames)

    print(len(rootFiles))
    return rootFiles

myDir='/eos/user/d/dalfonso/AnalysisVBS/NANO/TEST/UL'
#myDir='/eos/user/d/dalfonso/AnalysisVBS/NANO/MARCH9SYST/UL'
#dir='/eos/user/d/dalfonso/AnalysisVBS/NANO/MARCH9SYST/UL2018/METside/'
#/eos/user/d/dalfonso/AnalysisVBS/NANO/MARCH9SYST/UL2018/MET/VBShadSignal_MET_314.root

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
stringCategory='0' #will be passed over
#stringCategory='3' #BMET
#stringCategory='4' #RMET

def loopOverBKG(dfINI,myBkg,year):

    count = dfINI.Count().GetValue()
    print('nevents=',count)

    df = (dfINI.Filter("ana_category=={1} and (({0}==-1 and isRealData==1) or (mc=={0} and mc>0))".format(myBkg,stringCategory)," select on the BKG or data"))

    if True:
        print("writing plots")
        hists = {
            "met_pt":  {"name":"met_pt","title":"missing energy; MET (GeV); N_{Events}","bin":80,"xmin":0.,"xmax":800.},
            "varPTV2":  {"name":"varPTV2","title":"bosV2pt; V2_{pT} (GeV); N_{Events}","bin":80,"xmin":0.,"xmax":800.},
            "bosV2mass":  {"name":"bosV2mass","title":"bosV2mass; V2_{mass} (GeV); N_{Events}","bin":150,"xmin":0.,"xmax":150.},
            "bosV2discr":  {"name":"bosV2discr","title":"bosV2discr; V2_{discr} (GeV); N_{Events}","bin":100,"xmin":0.,"xmax":1.},
            "BDTwithMET":  {"name":"BDTwithMET","title":"BDTwithMET; BDTwithMET (GeV); N_{Events}","bin":100,"xmin":0.,"xmax":1.},
            "varMVV":  {"name":"varMVV","title":"MVV (it's a transverse mass for B/Rmet); MVV (GeV); N_{Events}","bin":100,"xmin":0.,"xmax":1000.},
            "varMjj":  {"name":"varMjj","title":"Mjj; Mjj (GeV); N_{Events}","bin":125,"xmin":0.,"xmax":2500.},
            "varJet1Pt":  {"name":"varJet1Pt","title":"varJet1Pt; jet 1 pT (GeV); N_{Events}","bin":250,"xmin":0.,"xmax":750.},
            "varJet2Pt":  {"name":"varJet2Pt","title":"varJet2Pt; jet 2 pT (GeV); N_{Events}","bin":150,"xmin":0.,"xmax":450.},
            #
#            "bosV2j1Pt":  {"name":"bosV2j1Pt","title":"bosV2j1Pt; V2_{mass} (GeV); N_{Events}","bin":150,"xmin":0.,"xmax":300.},
#            "bosV2j2Pt":  {"name":"bosV2j2Pt","title":"bosV2j2Pt; V2_{mass} (GeV); N_{Events}","bin":150,"xmin":0.,"xmax":150.},
#            "bosV2dR":  {"name":"bosV2dR","title":"bosV2dR; V2_{dR} (GeV); N_{Events}","bin":60,"xmin":0.,"xmax":6.},
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

def anaOverYear(directory,stringCategory):
    
    year = directory
    if directory=='2016': year=22016
    if directory=='2016APV': year=12016    
    
    #https://github.com/MiT-HEP/ChargedHiggs/blob/8450730417bcfb0fc887209878ef197f9bc536e4/src/AnalysisVBShad.cpp#L3504

    '''
    200 = TTbar              greenDark
    300 = ZJetsToNuNu_HT     orange
    310 = WJetsToLNu_HT      redLight
    301 (302,303) = Zinv-Pt  orange
    312 = W-Pt               redLight
    313 = DY-Pt              orangeDark
    361-362 = EWKW and EWKZ  azure azure
    205 = TTX                azureDark
    210 = ST                 azureDark
    '''
    
    mc = [ 200, 300, 310, 301, 312, 313, 361, 362, 110, 205, 210, -1]
    colors = [greenDark, orange, redLight, orange, redLight, orangeDark, azure, azure, azure, azureDark, azureDark, ROOT.kBlack]
    names = ['ttbar', 'ZNuNu_HT', 'WLNu_HT', 'Zinv-Pt', 'W-Pt', 'DY-Pt', 'EWK-W', 'EWK-Z', 'VVV', 'TTX', 'ST', 'data']

    ###########
    ###########
    ###########

    region = ''
#    region = 'side'
    files = listDir(myDir+str(directory)+'/MET'+region+'/')

    df = RDataFrame("tree_vbs", files)

    for sampleNOW in mc:
        print('processing mc = ', sampleNOW)
        loopOverBKG(df, sampleNOW,year)

    if stringCategory=='3': outputFileHisto = "histoOUTname_VBS_"+str(year)+"_BMET"+region+".root"
    elif stringCategory=='4': outputFileHisto = "histoOUTname_VBS_"+str(year)+"_RMET"+region+".root"
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

def makeFinalPlot(histoName,region,stringCategory):

    print('HELLO')
    ROOT.gStyle.SetOptStat(0)
    canv = ROOT.TCanvas("stackcanvas","Stack canvas",800,800)
#    canv.SetLogy(1)
    stack = ROOT.THStack("stack","")
    dataStack = ROOT.THStack("dataStack","")    

    deltax=0
    if histoName=='met_pt_' or histoName=='BDTwithMET_' or histoName=='varMjj_' or histoName=='varJet2Pt_' or histoName=='varJet1Pt_': deltax=0.5
    if stringCategory=='4' and ( histoName=='varPTV2_' or histoName=='bosV2j2Pt_' or histoName=='bosV2dR_' or histoName=='varMVV_'): deltax=0.5

    legend = ROOT.TLegend(0.1+deltax, 0.15, 0.4+deltax, 0.9)
    legend.SetTextFont(42)
    legend.SetFillStyle(0)
    legend.SetBorderSize(0)
    legend.SetTextSize(0.04)
    legend.SetTextAlign(32)

    years = ['2018', '2017', '22016', '12016']
#    years = ['12016']

    sumData=0
    for mc_ in [-1]:
        for y in years:

            print(y)
            if stringCategory=='4': outputFileHisto = "histoOUTname_VBS_"+str(y)+"_RMET"+region+".root"
            elif stringCategory=='3': outputFileHisto = "histoOUTname_VBS_"+str(y)+"_BMET"+region+".root"
            f = ROOT.TFile.Open(outputFileHisto)
            h = f.Get(histoName+str(y)+'_'+str(mc_))
            h.SetBinContent(h.GetNbinsX(), h.GetBinContent(h.GetNbinsX())+h.GetBinContent(h.GetNbinsX()+1))
            h.SetDirectory(0)
            print('DATA = ', h.GetName(),' ',h.Integral())
            dataStack.Add(h)
            sumData+=h.Integral()

    hdata = dataStack.GetStack().Last()
    hdata.SetMarkerStyle(21)
    hdata.SetMarkerColor(1)
    hdata.SetLineColor(1)
    hdata.Draw("p e")
    if hdata and hdata.Integral()>0: legend.AddEntry(hdata, "Data "+str(sumData) ,"lep")


    mc = [ 200, 301, 312, 313, 361, 362, 110, 205, 210]
    colors = [greenDark, orange, redLight, orangeDark, azure, azure, azure, azureDark, azureDark]
    names = ['ttbar', 'Zinv-Pt', 'W-Pt', 'DY-Pt', 'EWK-W', 'EWK-Z', 'VVV', 'TTX', 'ST']
    
    for mc_,color,name in zip(mc,colors,names):

        print('mc_',mc_)

#        for y in ['2018', '2017', '22016', '12016']:
        for y in years:

            print(y)
            if stringCategory=='4': outputFileHisto = "histoOUTname_VBS_"+str(y)+"_RMET"+region+".root"
            elif stringCategory=='3': outputFileHisto = "histoOUTname_VBS_"+str(y)+"_BMET"+region+".root"
            f = ROOT.TFile.Open(outputFileHisto)
            h = f.Get(histoName+str(y)+'_'+str(mc_))
            h.SetDirectory(0)
            print(name,' ', h.GetName(),' ',round(h.Integral(),1), " ", color)
            h.SetLineColor(ROOT.TColor.GetColor(*color))
            h.SetFillColor(ROOT.TColor.GetColor(*color))
            h.SetBinContent(h.GetNbinsX(), h.GetBinContent(h.GetNbinsX())+h.GetBinContent(h.GetNbinsX()+1))
            if h and h.Integral()>0: legend.AddEntry(h, name+" "+str(round(h.Integral(),1)) ,"f")
            stack.Add(h)

    print('number of histograms in the stack: ',stack.GetNhists())
    canv.cd()
#    hs = stack.GetStack()
    stack.Draw("hist same")
#    hTOT = stack.GetStack().Last()
#    hTOT.Draw("hist")
#    print(' TOT = ', round(hTOT.Integral(),1))
    hdata.Draw("p e same")
    legend.Draw("SAME")
    canv.Update()

    if stringCategory=='4': fileName = "Stack"+histoName+"_RMET"+region+".root"
    if stringCategory=='3': fileName = "Stack"+histoName+"_BMET"+region+".root"

    myfile = ROOT.TFile(fileName,"RECREATE")
    hdata.Write()
#    stack.Write()
    myfile.Write()
    myfile.Close()

    if stringCategory=='4': canv.SaveAs("~/www/VBS/test/"+histoName+"_RMET"+region+".png")
    elif stringCategory=='3': canv.SaveAs("~/www/VBS/test/"+histoName+"_BMET"+region+".png")

if __name__ == "__main__":

    stringCategory='3' #BMET
#    stringCategory='4' #RMET

#    anaOverYear(2018,stringCategory)
#    anaOverYear(2017,stringCategory)
#    anaOverYear('2016',stringCategory)
#    anaOverYear('2016APV',stringCategory)
#    exit()

#    region = ''
    region = 'side'

    makeFinalPlot("met_pt_",region,stringCategory)
    makeFinalPlot("bosV2mass_",region,stringCategory)
    makeFinalPlot("bosV2discr_",region,stringCategory)
    makeFinalPlot("varPTV2_",region,stringCategory)
    makeFinalPlot("BDTwithMET_",region,stringCategory)
    makeFinalPlot("varMVV_",region,stringCategory)
    makeFinalPlot("varMjj_",region,stringCategory)
    makeFinalPlot("varJet1Pt_",region,stringCategory)
    makeFinalPlot("varJet2Pt_",region,stringCategory)

    if stringCategory=='4':
        makeFinalPlot("bosV2j1Pt_",region,stringCategory)
        makeFinalPlot("bosV2j2Pt_",region,stringCategory)
        makeFinalPlot("bosV2dR_",region,stringCategory)

    exit()

'''
    myString=''
    f = ROOT.TFile.Open("histoOUTname_VBS.root")

    for sampleIDX,colorIDX in zip(histos,color):
        hf.Get("met_pt_2018_"+sampleIDX).SetLineColor(ROOT.TColor.GetColor(*color))
        h.SetFillColor(ROOT.TColor.GetColor(*color))
        if sampleIDX==200: h.Draw("")
        else h.Draw("hist same")


        label.DrawLatex(50, 0.001, sampleNOW)

    h1.SetMaximum(10*max(h1.GetMaximum(),h2.GetMaximum(),h3.GetMaximum(),h4.GetMaximum()))
    h1.Draw("hist")
    h2.Draw("hist same")
    h3.Draw("hist same")
    h4.Draw("hist same")

    canv.Draw()

    label = ROOT.TLatex();
    label.DrawLatex(50, 0.0001, "312 Wpt")
    label.SetTextFont(42)
    label.SetTextSize(0.03)
    label.SetTextColor(2)
    label.SetTextColor(4)
    label.SetTextColor(1)
    label.DrawLatex(50, 0.00001, "302 Zinv")
    label.SetTextColor(3)
    label.DrawLatex(50, 0.00001, "303 Zinv")
    canv.Update()
    canv.SaveAs("~/www/VBS/may3/"+h1.GetName()+".png")

'''
