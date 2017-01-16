
for i in /data/smptuples/xniu/LumiStability/2016/Muons/12p3_invfb_2016BCD/IsoMu22/Mu_Jul20_IsoMu22_DataSet_*/MuHLTEff/1MG/eff.root ; do echo cp -v $i ./eff_MG_DataSet_$(echo $i | sed 's/.*DataSet_//' | sed 's:/.*::').root ; done 
for i in /data/smptuples/xniu/LumiStability/2016/Muons/12p3_invfb_2016BCD/IsoMu22/Mu_Jul20_IsoMu22_DataSet_*/MuHLTEff/1CT/eff.root ; do echo cp -v $i ./eff_CT_DataSet_$(echo $i | sed 's/.*DataSet_//' | sed 's:/.*::').root ; done 

for i in /data/smptuples/xniu/LumiStability/2016/Muons/12p3_invfb_2016BCD/IsoMu22/Mu_Jul20_IsoMu22_DataSet_*/MuHLTEff/2MG/eff.root ; do echo cp -v $i ./eff_MG_DataSet_$(echo $i | sed 's/.*DataSet_//' | sed 's:/.*::').root ; done 
for i in /data/smptuples/xniu/LumiStability/2016/Muons/12p3_invfb_2016BCD/IsoMu22/Mu_Jul20_IsoMu22_DataSet_*/MuHLTEff/2CT/eff.root ; do echo cp -v $i ./eff_CT_DataSet_$(echo $i | sed 's/.*DataSet_//' | sed 's:/.*::').root ; done 

for i in {1..488}; do [ -f eff_MG_DataSet_$i.root ] || echo ln -s eff_MG_DataSet_$((i-1)).root  eff_MG_DataSet_$i.root  ; done 
