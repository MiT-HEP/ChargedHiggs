import ROOT
import os,sys
from array import array
from optparse import OptionParser
parser=OptionParser()
parser.add_option("-y","--year",help="Year [%default]",default="2018")
parser.add_option("-j","--nj",help="Year [%default]",default="0")
parser.add_option("-s","--save",action='store_true',help="Save. don't change![%default]",default=False)
opts,args=parser.parse_args()

if opts.save or opts.year=="all":
    ROOT.gROOT.SetBatch()

if opts.year=="all":
    from subprocess import call
    from datetime import datetime
    r=open("zpt.h","w")
    r.write("#ifndef ZPT_H\n")
    r.write("#define ZPT_H\n")
    r.write("// VERSION "+datetime.now().strftime("%Y %b %d")+"\n")
    r.write("class zptutils\n")
    r.write("{\n")
    for year in ["2016","2017","2018"]:
        for nj in ["0","1","2"]:
            r.write("std::unique_ptr<TH1D> zpt_"+year+"_"+nj+"j;\n")
    r.write("public:\n")
    r.write("inline float getZPtReweight(float Zpt,int year,int nj){\n")
    for year in ["2016","2017","2018"]:
        for nj in ["0","1","2"]:
            r.write("   if (year=="+year+" and nj=="+nj+") return getZPtReweight"+year+"_"+nj+"j(Zpt);\n")
    r.write("return 1;\n")
    r.write("}\n")
    r.write("protected:\n")
    r.close()
    for year in ["2016","2017","2018"]:
        for nj in ["0","1","2"]:
            print "-> calling","python "+sys.argv[0]+" -y "+year + " -j "+nj +" -s"
            call("python "+sys.argv[0]+" -y "+year + " -j "+nj +" -s",shell=True)
    r=open("zpt.h","a")
    r.write("};\n")
    r.write("#endif\n")
    r.close()
    sys.exit(0)

year=opts.year

fIn=ROOT.TFile.Open("/eos/cms/store/user/amarini/Hmumu/fwk/2019_11_06_Hmm"+year+"_ForZPtReweight/2019_11_06_Hmm"+year+"_ForZPtReweight.root")

lumi=35920
if year=="2017": lumi=41530
if year=="2018": lumi=59740

#
#nj="0"
#nj="1"
#nj="2"
nj=opts.nj
d = "HmumuAnalysis/Vars"

data=fIn.Get(d+"/"+"PtOnZ"+nj+"j_Data").Clone("data")
dy=fIn.Get(d+"/"+"PtOnZ"+nj+"j_DY").Clone("dy")
tt=fIn.Get(d+"/"+"PtOnZ"+nj+"j_TT").Clone("tt")
st=fIn.Get(d+"/"+"PtOnZ"+nj+"j_ST").Clone("st")

def Rebin(h):
    ''' Rebin with un-even bins '''
    #mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
    #mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,500,50)+ [500,1000,2000,8000])
    if year=="2016":
        if nj=="0":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) )
        elif nj=="1":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,500,50)+ [500,1000])
        elif nj=="2":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,500,50)+ [500,1000])
        else:return h
    elif year=="2017":
        if nj=="0":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) )
        elif nj=="1":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,500,50)+ [500,1000])
        elif nj=="2":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,500,50)+ [500,1000])
        else:return h
    elif year=="2018":
        if nj=="0":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,20) )
        elif nj=="1":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,300,50)+ [300,1000])
        elif nj=="2":mybins=array('d',range(0,40) + range(40,60,2) + range(60,100,5) + range(100,200,20) + range(200,300,50)+ [300,1000])
        else:return h
    else:return h
    #h1 = ROOT.TH1D(h.GetName()+"_rebin",h.GetTitle(),len(mybins)-1,mybins)
    h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
    return h1

if True:
    print "-> Rebinnig"
    data=Rebin(data)
    dy=Rebin(dy)
    tt=Rebin(tt)
    st=Rebin(st)


dy.Scale(lumi)
tt.Scale(lumi)
st.Scale(lumi)

tt.Add(st) ## tt contains all bkg
data.Add(tt,-1) ## data bkg subtracted


data.Divide(dy)
alpha=None

if True:
    print "-> Rematch cross sections"
    idy=dy.Integral()
    s=0
    for ibin in range(0,dy.GetNbinsX()):
        c=dy.GetBinContent(ibin+1)
        w=data.GetBinContent(ibin+1)
        s+=c*w
    alpha=idy/s
    print "-> Reweight factor is",alpha
    data.Scale(alpha)

c=ROOT.TCanvas("c","c",800,800)
data.GetYaxis().SetRangeUser(0,2)
data.Draw("HIST")
txt=ROOT.TLatex()
if alpha!=None:
    txt.SetNDC()
    txt.SetTextSize(0.04)
    txt.SetTextFont(42)
    txt.SetTextAlign(13)
    txt.DrawLatex(0.15,.89,"Reweight factor #alpha=%.2f"%alpha)

if opts.save:
    c.SaveAs("zpt_"+year+"_"+nj+"j.pdf")
    c.SaveAs("zpt_"+year+"_"+nj+"j.png")

    r=open("zpt.h","a")
    r.write("float getZPtReweight"+year+"_"+nj+"j(float Zpt  ){\n")
    #r.write("std::unique_ptr<TH1D> zpt_"+year+"_"+nj"j;\n")
    name="zpt_"+year+"_"+nj+"j";
    nbins=data.GetNbinsX();
    r.write("if ("+name+".get() == nullptr){\n")
    r.write("Double_t xAxis[%d]={"%(nbins+1)+ ','.join("%.0f"%data.GetBinLowEdge(i+1) for i in range(0,nbins+1)) +"};\n")
    r.write("    "+name+".reset( new TH1D(\""+name+"\",\"title\",%d,xAxis));\n"%nbins)
    for ibin in range(0,nbins):
        r.write("    "+name+"->SetBinContent(%d,%f);\n"%(ibin+1,data.GetBinContent(ibin+1)))
    r.write("};// endif\n")
    r.write("return "+name+"->GetBinContent(std::min("+name+"->FindBin(Zpt),%d));\n"%(nbins))
    r.write("};// end function\n");
    r.close()
else:
    raw_input("ok?")
