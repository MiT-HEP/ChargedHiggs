import ROOT
import sys,os
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

cat="cat12"
dir="cat12"

if len(sys.argv)>1:
   dir=sys.argv[1]
   cat=dir.split('_')[0]

c=ROOT.TCanvas("c","c")
t1=ROOT.TTree("t1","t1")
t1.ReadFile(dir+"/multidimtree1_"+cat+".txt","mu:muErr",' ')
t1.SetLineColor(ROOT.kBlack)
t1.SetLineWidth(2)
t1.Draw("(mu-1)/muErr>>h(200,-10,10)")
#t1.Draw("(mu-1)/muErr>>h(200,-10,10)","muErr>3")
l = []
n=t1.GetSelectedRows()
for i in range(0,n):
	l.append(t1.GetV1()[i])
l=sorted(l)

ltx=ROOT.TLatex()
ltx.SetNDC()
ltx.SetTextFont(43)
ltx.SetTextSize(28)
ltx.SetTextAlign(33)
ltx.DrawLatex(0.89,.89,"median=%.2f"%l[n/2])

print "median is",l[n/2]

c.Update()

c2=ROOT.TCanvas("c2","c2")
t1.Draw("muErr>>h2(200,0,20)")
c2.Update()

raw_input("ok?")

c.SaveAs(dir+"/plot.pdf")
c.SaveAs(dir+"/plot.png")

