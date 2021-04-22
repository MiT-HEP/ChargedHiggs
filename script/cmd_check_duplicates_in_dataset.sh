#DATASET=/MET/Run2018B-UL2018_MiniAODv1_NanoAODv2-v1/NANOAOD
DATASET=/MET/Run2018D-UL2018_MiniAODv1_NanoAODv2-v1/NANOAOD
#/BTagCSV/Run2017E-UL2017_MiniAODv1_NanoAODv2-v1/NANOAOD

FILES=$(dasgoclient -query "file dataset=${DATASET}")

#python script/lumi.py $( dasgoclient -query "file dataset=${DATASET}" | while read file ; do echo "root://xrootd-cms.infn.it//${file}"; done )   > lumi_full_SingleMuon_Run2016F-UL2016_MiniAODv1_NanoAODv2-v4.txt

rm cmd_lumi.sh
for file in $FILES; 
do
        echo "python ../lumi.py \"root://xrootd-cms.infn.it//${file}\" > $(echo -n ${file} | tr '/' '_' | sed 's:^_::' | sed 's:.root::' ).txt " >> cmd_lumi.sh
done  

python ../parallel.py -n 30 -f cmd_lumi.sh

################################### STEP 2 ###########################

rm -r comp
mkdir comp
CMD=""
for f in *txt ; do for k in *txt ; do 
    [ "$f" == "$k" ] && continue; 
    echo "compareJSON.py --and $f $k > comp/${f}__${k}" >> comp/cmd_list.sh
done; done ;

echo "executing parallel: comp/cmd_list.sh"
python ../parallel.py -n 20 -f comp/cmd_list.sh
#python ../parallel.py -n 20 -- "$CMD"
#done; done | sed 's:\{\}::' | tr -d '\n'
#for file in  $( dasgoclient -query "file dataset=${DATASET}" | grep 'B95CA7CBEA29\|8BE104926EF0' ) ; do echo $file; python ../lumi.py root://xrootd-cms.infn.it//${file} > $(echo -n ${file} | tr '/' '_' | sed 's:^_::' | sed 's:.root::' ).txt ; done

################################### STEP 3 ###########################
## remove files that contains only {}
cd comp
for f in * ; do 
    grep '{}' $f && rm $f
done
