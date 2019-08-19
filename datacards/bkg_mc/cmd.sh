
####### build ws
python script/Hmumu/buildMCws.py -i /eos/user/a/amarini/Hmumu/fwk/2019_07_25_Hmm2016_BdtUCSD_Mjj400_JetPt25/2019_07_25_Hmm2016_BdtUCSD_Mjj400_JetPt25.root,/eos/user/a/amarini/Hmumu/fwk/2019_07_25_Hmm2017_BdtUCSD_Mjj400_JetPt25/2019_07_25_Hmm2017_BdtUCSD_Mjj400_JetPt25.root,/eos/user/a/amarini/Hmumu/fwk/2019_07_25_Hmm2018_BdtUCSD_Mjj400_JetPt25/2019_07_25_Hmm2018_BdtUCSD_Mjj400_JetPt25.root --binput Hmumu/2019_07_25_HmmAll_BdtUCSD_Mjj400_JetPt25/BackgroundModel.root -o Hmumu/2019_07_25_HmmAll_BdtUCSD_Mjj400_JetPt25/MCws.root --hmm hmmExCatMjj --fit --batch

# 
for i in {1..13}; do cp -v cat0_mconly.txt  cat${i}_mconly.txt ; sed -i'' "s/cat0/cat${i}/g" cat${i}_mconly.txt ;done

for i in {0..8}; do 
    cp ../RunII/cat${i}.txt ./RunII_mc/cat${i}.tmp.txt
    cp cat${i}_mconly.txt ./RunII_mc/
    cd RunII_mc
    #combine -M GenerateOnly -t -1 cat${i}.tmp.txt -n Asimov_mu1_cat${i} -m 125 --expectSignal=1 --saveToys
    combine -M FitDiagnostics --saveShapes -t -1 cat${i}.tmp.txt -n Asimov_mu1_cat${i} -m 125 --expectSignal=1 
    cp ./cat${i}.tmp.txt ./cat${i}.Asimov.tmp.txt
    #sed -i'' "s:^shapes\ data_obs.*$:shapes data_obs cat${i} higgsCombineAsimov_mu1_cat${i}.GenerateOnly.mH125.123456.root toys/toy_asimov:g" cat${i}.Asimov.tmp.txt
    #sed -i'' "s:^shapes\ data_obs.*$:shapes data_obs cat${i} fitDiagnosticsAsimov_mu1_cat${i}.root shapes_prefit/cat${i}/total:g" cat${i}.Asimov.tmp.txt
    python ../convertTH1toWs.py -i fitDiagnosticsAsimov_mu1_cat${i}.root --hist shapes_prefit/cat${i}/total_background -o asimov_cat${i}.root --histout asimov_cat${i}
    sed -i'' "s@^shapes\ data_obs.*@shapes data_obs cat${i} asimov_cat${i}.root w:asimov_cat${i}@g" cat${i}.Asimov.tmp.txt
    combineCards.py ch1=cat${i}.tmp.txt mc=cat${i}_mconly.txt > cat${i}.txt 
    combineCards.py ch1=cat${i}.Asimov.tmp.txt mc=cat${i}_mconly.txt > cat${i}.Asimov.txt 
    cd -
    rm RunII_mc/cat${i}.tmp.txt RunII_mc/cat${i}_mconly.txt
    rm RunII_mc/cat${i}.Asimov.tmp.txt
    rm RunII_mc/higgsCombineAsimov_mu1_cat${i}.*root RunII_mc/fitDiagnosticsAsimov_mu1_cat${i}.root
done

#
