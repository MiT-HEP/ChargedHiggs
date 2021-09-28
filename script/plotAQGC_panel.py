import os
import sys
import array 
import math
import json

from optparse import OptionParser
parser = OptionParser()
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
(opts,args)=parser.parse_args()

import ROOT

with open(opts.outname + ".json") as fin: 
    results = json.loads(fin.read())

####################################### 
nPoints=10
xmin,xmax=-16.,16.
ytick = 0.05
border=0.01
########################################

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

print "-> Drawing Canvas and pad"
dummy = ROOT.TH2F("dummy","",1000,xmin,xmax,1000,0-0.5,nPoints-0.5); ## [ 0 , N-1] -> +[-0.5, +0.5]
dummy.SetStats(0);
ci = ROOT.TColor.GetColor("#00ff00");
dummy.GetYaxis().SetBinLabel(1,"")
dummy.GetXaxis().SetNdivisions(0)
dummy.SetFillColor(ci);
dummy.GetXaxis().SetTickSize(0.)
dummy.GetYaxis().SetTickSize(0.)
dummy.GetYaxis().SetRangeUser(-0.5,nPoints-0.5)

c=ROOT.TCanvas("c2","c2")
c.SetCanvasSize(800,800)
#c.SetBottomMargin(0.15)
#c.SetLeftMargin(0.10)
#c.SetTopMargin(0.10)
def no_margin(p):
    p.SetTopMargin(0)
    p.SetBottomMargin(0)
    p.SetLeftMargin(0)
    p.SetRightMargin(0)

###########################################################
p0 = ROOT.TPad("pad0","pad",0,0.9,1,1)
no_margin(p0)
p0.SetTopMargin(border/0.1)
p0.SetRightMargin(border)
p0.SetLeftMargin(border)
p0.Draw()
p0.cd()
dummy.Draw("AXIS")

txt=ROOT.TLatex()
txt.SetNDC()
txt.SetTextFont(43)
txt.SetTextSize(20)
txt.SetTextAlign(33)
txt.DrawLatex(1.-border*2,.88,"138 fb^{-1} (13 TeV)")
txt.SetTextSize(29)
txt.SetTextAlign(13)
if opts.paper:
    txt.DrawLatex(.05,.6,"#bf{CMS}")
else:
    #0.21 0.86 on p1
    # 0.01 .95 on p5
    txt.DrawLatex(.05,.6,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
###########################################################
p1 = ROOT.TPad("pad1","pad",0,0,0.25,0.9)
p2 = ROOT.TPad("pad2","pad",0.5,0,.75,0.9)
p3 = ROOT.TPad("pad1","pad",0.25,0,0.5,0.9)
p4 = ROOT.TPad("pad2","pad",0.75,0,1,0.9)

no_margin(p1)
p1.SetLeftMargin(border/0.25)
p1.SetBottomMargin(border/0.9)
no_margin(p2)
p2.SetBottomMargin(border/0.9)
no_margin(p3)
p3.SetBottomMargin(border/0.9)
no_margin(p4)
p4.SetBottomMargin(border/0.9)
p4.SetRightMargin(border/0.25)
print ">pads"
c.cd()
p1.Draw()
p1.cd()
dummy.Draw("AXIS")

c.cd()
p2.Draw()
p2.cd()
dummy.Draw("AXIS")

c.cd()
p3.Draw()
p3.cd()
dummy.Draw("AXIS")

c.cd()
p4.Draw()
p4.cd()
dummy.Draw("AXIS")

gc=[]

print ">continue"
def draw_axis_tick(x,y,precision="%.1f"):
    global gc,txt
    xl,xh = x,x
    yl,yh = y,y+0.1
    line= ROOT.TLine(xl,yl,xh,yh)
    line.SetLineColor(ROOT.kBlack) 
    line.SetLineWidth(1)
    line.Draw()
    gc.append(line)
    txt.SetNDC(False)
    txt.SetTextSize(12)
    txt.SetTextAlign(21)
    txt.DrawLatex(x,yh+0.01,precision%x)

def draw_pad(y,xmin,xmax,side,drawAxis=True):
    global dummy, gc,c
    mydummy = dummy.Clone('dummy_%d'%len(gc))
    mydummy.GetXaxis().SetRangeUser(xmin,xmax)
    mydummy.GetYaxis().SetRangeUser(y-0.5,y+0.5)
    gc.append(mydummy)
    if side=='left': 
        padmin,padmax= 0.25,.5
    if side=='right': 
        padmin,padmax= 0.75,1.-border
    c.cd()
    padymin = (1.-0.1-border)/nPoints*y  + border ## Global coordinates
    padymax = (1.-0.1-border)/nPoints*(y+1) + border

    if y==0 and side == "left":
        print "> DEBUG", padymin,padymax,padmin,padmax, side

    p=ROOT.TPad("pad_%d"%len(gc),"",padmin,padymax,padmax,padymin)
    no_margin(p)
    #if side=='right':p.SetRightMargin(border/0.25)
    p.Draw()
    p.cd()
    mydummy.Draw("AXIS")
    mydummy.Draw("AXIS X+ Y+ SAME")

    gc.append(p)

    if drawAxis: 
        if abs(xmin+16)<0.1 : ## -16,16
            for x in [-15,-10,-5,0,5,10,15]: draw_axis_tick(x,y-0.5,"%.0f")
        elif abs(xmin+10)<0.1: ## -10,10
            for x in [-8,-6,-4,-2,-1,0,2,4,6,8]: draw_axis_tick(x,y-0.5,"%.0f")
        elif abs(xmin+5)<0.1: ## -5,5
            for x in [-4,-3,-2,-1,0,1,2,3,4]: draw_axis_tick(x,y-0.5,"%.0f")
        elif abs(xmin+1)<0.01: ## -1,1
            for x in [-.8,-.6,-.4,.2,0.,.2,.4,.6,.8]: draw_axis_tick(x,y-0.5,"%.1f")
        elif abs(xmin+0.5)<0.001: ## -0.5,.5
            #for x in [-0.4,-0.3,-0.2,-0.1,0,0.1,.2,.3,.4]: draw_axis_tick(x,y-0.5,"%.1f")
            for x in [-0.4,-0.2,0,0.2,.4]: draw_axis_tick(x,y-0.5,"%.1f")
        elif abs(xmin+0.1)<0.001: ## -0.1,0.1
            for x in [-0.1,-0.075,-0.025,0.,0.025,0.05,0.075,.1]: draw_axis_tick(x,y-0.5,"%.2f")
        else:
            print ">Requested range",xmin,xmax
            raise ValueError("Unknown range")
    return p

def set_style(o,style):
    if o.InheritsFrom("TMarker") and style=='68':
        o.SetMarkerColor(ROOT.kBlack)
        o.SetMarkerStyle(20)
        o.SetMarkerSize(0.8)
    if o.InheritsFrom("TLine"):
        if style=='68':
            o.SetLineColor(ROOT.kBlack)
            o.SetLineWidth(2)
        if style=='95':
            o.SetLineColor(ROOT.kRed+2)
            o.SetLineWidth(1)
    if o.InheritsFrom("TArrow"):
        if style=='68':
            o.SetFillColor(ROOT.kBlack);
            o.SetAngle(40);
            o.SetArrowSize(0.02)
            o.SetOption(">")
        if style=='95':
            o.SetFillColor(ROOT.kRed+2);
            o.SetAngle(40);
            o.SetArrowSize(0.03)
            o.SetOption("|>")

def draw_line( x, y, dxl,dxh, pad='left',style='68'):
    global gc
    global xmin,xmax
    if pad=='left': p3.cd()
    if pad=='right': p4.cd()
    print "DEBUG", "Drawing line @",x,y,dxl,dxh,pad,style
    
    xlow= x-dxl 
    xhigh= x+dxh
    
    openLeft=False 
    openRight=False 
    if xlow< xmin:
        openLeft=True
        xlow=xmin
    if xhigh> xmax:
        openRight=True
        xhigh=xmax
    
    line= ROOT.TLine(xlow,y,xhigh,y)
    set_style(line,style)
    line.Draw()
    gc.append(line)
    
    if style == '68' and xmin <x and x<xmax: 
        m= ROOT.TMarker(x,y,20)    
        set_style(m,style)
        m.Draw()
        gc.append(m)

    if not openLeft:
        t1 = ROOT.TLine(xlow,y -ytick,xlow ,y+ytick)
        set_style(t1,style)
        t1.Draw()
        gc.append(t1)
    else:
        a1 = ROOT.TArrow(xlow+ytick,y,xlow,y) #,0.02,"|>")### ytick is epsilon here
        set_style(a1,style)
        a1.Draw()
        gc.append(a1)

    if not openRight:
        t1 = ROOT.TLine(xhigh,y -ytick,xhigh ,y+ytick)
        set_style(t1,style)
        t1.Draw()
        gc.append(t1)
    else: 
        a1 = ROOT.TArrow(xhigh-ytick,y,xhigh,y) #,0.02,"|>")### ytick is epsilon here
        set_style(a1,style)
        a1.Draw()
        gc.append(a1)

txt2=ROOT.TLatex()
txt2.SetNDC(False)
txt2.SetTextFont(43)
#txt2.SetTextSize(0.08)
txt2.SetTextSize(18)
txt2.SetTextAlign(11)

def draw_label(label, x,y, dxl, dxh, dxl2,dxh2,side):
    global gc
    global txt2
    if side=='left': p1.cd()
    if side=='right': p2.cd()
    #newlabel=label[0] + "_{" +label[1:].upper()+"}"+"/#Lambda^{4}" # "[TeV^{-4}]"
    newlabel="#voidb" + "_{"+label[1].upper() + "#kern[0.3]{"+label[2:]+"}}"+"#kern[0.3]{/}#Lambda^{4}" # "[TeV^{-4}]"
    txt2.SetNDC(False)
    txt2.SetTextAlign(12)
    txt2.DrawLatex(xmin+0.1*(xmax-xmin),y,newlabel + "  %.1f^{#font[122]{+}%.1f}_{#font[122]{-}%.1f}  ({}^{#font[122]{+}%.1f}_{#font[122]{-}%.1f})"%(x,dxh,dxl,dxh2,dxl2) )

#fm0 fm1 fm2 fm3 fm4 fm5 fm7    fs0 fs1 
#ft0 ft1 ft2 ft5 ft6 ft7 ft8 ft9    fs2

left=['fm0','fm1','fm2','fm3','fm4','fm5','fm7','empty','fs0','fs1']
right=['ft0','ft1','ft2','ft5','ft6','ft7','ft8','ft9','empty','fs2']

ranges={ 
        "fm0":[-10,10.],
        "fs0":[-16,16.],
        "ft0":[-0.5,0.5],
        "ft1":[-0.5,0.5],
        "ft2":[-0.5,0.5],
        "ft5":[-0.5,0.5],
        "ft6":[-5,5],
        "ft7":[-5,5],
        "ft8":[-5,5],
        "ft9":[-5,5],
        "fm2":[-5,5],
        "fm1":[-5,5],
        "fm0":[-5,5],
        "fm4":[-5,5],
        "default":[-16,16],
        }

print "> Drawing Empties as well"
y = left.index('empty') 
pe0=draw_pad( y,-16.,16.,'left',False)
pe0.SetFillColor(ROOT.kGray)
y = right.index('empty') 
pe1=draw_pad( y,-16.,16.,'right',False)
pe1.SetFillColor(ROOT.kGray)

print "> Drawing Lines and Labels"
for label in results:
    print ">>>Doing label",label
    if label not in left and label not in right:
        raise ValueError("label '"+label+"' not in: " +','.join(left)+','.join(right))

    side= 'left' if label in left else 'right' if label in right else None
    y = left.index(label) if label in left else right.index(label) if label in right else None
    draw_pad( y , ranges[label if label in ranges else "default"][0],ranges[label if label in ranges else "default"][1],side)

    side = 'pad' ## use previous defined pad
    draw_line( results[label]['bf'], y, results[label]['95'][0] ,results[label]['95'][1], side,'95')
    draw_line( results[label]['bf'], y, results[label]['68'][0] ,results[label]['68'][1], side,'68')

    side= 'left' if label in left else 'right' if label in right else None
    ylabel= y
    draw_label( label, results[label]['bf'], ylabel, results[label]['68'][0] ,results[label]['68'][1],  results[label]['95'][0] ,results[label]['95'][1], side)

### Draw the legend
print "> Drawing Legend"
c.cd()
p0 .cd ()
delta=0.5
oneline=True
if oneline:
    draw_line((xmax-xmin)*.74+xmin , nPoints*.3   ,delta,delta,'pad','68')
    draw_line((xmax-xmin)*.85+xmin , nPoints*.3   ,delta,delta,'pad','95')
    txt2.DrawLatex( (xmax-xmin)*.74+xmin +1.5*delta, nPoints*.3, "68%  (" )
    txt2.DrawLatex( (xmax-xmin)*.85+xmin +1.5*delta, nPoints*.3, "95%) [TeV^{-4}]" )
else: ## multi lines
    draw_line((xmax-xmin)*.8+xmin , nPoints*.5   ,delta,delta,'pad','68')
    draw_line((xmax-xmin)*.8+xmin , nPoints*.2   ,delta,delta,'pad','95')
    txt2.DrawLatex( (xmax-xmin)*.8+xmin +1.5*delta, nPoints*.5, "68% [TeV^{-4}]" )
    txt2.DrawLatex( (xmax-xmin)*.8+xmin +1.5*delta, nPoints*.2, "95% [TeV^{-4}]" )
### Draw text on Canvas

print "> Save"
c.SaveAs(opts.outname +"_summary"+ ".pdf")
c.SaveAs(opts.outname +"_summary"+ ".png")

raw_input("ok?")
