#!/bin/bash

aqgc_par="$1"
point="$2" ## cards -> prepare the card; cards-sumbmit: prepare and submit

echo "-> Requested parameter $aqgc_par and point $point"

CMSSW="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src"
CHARGEDHIGGS="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs"
DATACARDS="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs/Datacards/SEP22"
WORKDIR="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs/Datacards/SEP22/AQGC"
SUFFIX="sep22"
year=2018

cd $CMSSW
eval `scramv1 runtime -sh`

mkdir -p $WORKDIR

# prepare combined datacard SR+anti+side BB BBtag  BMET  RMET
CARDNAME="cms_vbshad_${year}_final_aqgc_${aqgc_par}_MVV.txt"

if [[ "$point" == "cards"* ]] ; then
    echo "-> Preparing Cards"
    cd $CHARGEDHIGGS
    CARDS=""
    ###CATEGORIES="BB BBtag BMET RMET"
    ###REGIONS="SR anti side"
    CATEGORIES="BB"
    REGIONS="SR"
    for cat in $CATEGORIES ; do
        #for reg in "SR" "anti" "side"; do 
        for reg in "SR"; do  ## I don't have anti/side for aqgc
            echo ">> Cat $cat Reg $reg"
            file="HAD";
            [[ "$cat" == *"MET" ]] && { file="MET"; }
            [ "$cat" == "BBtag" ] && { file="BHAD"; }
            python script/bwsVBSHad.py --there -q 5 --aqgc -s MVV -y ${year} -c ${cat} -i ${file}${reg}_${year}_${SUFFIX}.root --aqgc_parameter $aqgc_par
            EXITSTATUS=$?
            [ "$EXITSTATUS" == "0" ] || {
                     echo "Cards creation failed. |$EXITSTATUS| " ; 
                     echo "YEAR=${year} CAT=${cat} INPUT=${file}${reg}_${year}_${SUFFIX}.root PARAMETER= $aqgc_par";
                     exit 1; 
                 }

            EXTRA=""
            [ "$reg" == "anti" ] && EXTRA="_anti";
            [ "$reg" == "side" ] && EXTRA="_side";
            CARDS+=" cms_vbshad_${year}_5${EXTRA}_aqgc_${aqgc_par}_MVV_${cat}.txt"
    done
    done
    cd $DATACARDS
    combineCards.py ${CARDS} > ${CARDNAME}
    echo "## CONFIGURATION $CATEGORIES $REGIONS" >> ${CARDNAME}
    for card in ${CARDS} ; do tail -n 1 ${card} >> ${CARDNAME} ; break ;done ## copy the configuration line. Only for the first card
fi;

if [[ "$point" == *"submit" ]] ; then
#cat cms_vbshad_2018_5_aqgc_ft7_MVV_BB.txt | tail -n 1 | cut -d ' ' -f 3 | tr ',' '\n' | while read val; do
    cd $CHARGEDHIGGS
    MYNAME=aqgc_condor_${aqgc_par}.jdl
    [ -f $WORKDIR/$MYNAME ] && { echo "already submitted. file $WORKDIR/$MYNAME exist. refusing to continue"; exit 0; } 
    [ -f $WORKDIR/aqgc_condor.sh ] || cp -v test/aqgc_condor.sh $WORKDIR/
    ## copy and remove and queue command
    cat test/aqgc_condor.jdl | grep -v '^queue'  | grep -v '^JobBatchName' > $WORKDIR/$MYNAME
    echo "JobBatchName = \"point ${aqgc_par}\"" >> $WORKDIR/$MYNAME
    echo "model=${aqgc_par}" >> $WORKDIR/$MYNAME
    echo -n "queue point in (" >> $WORKDIR/$MYNAME 
    cat $DATACARDS/$CARDNAME | tail -n 1 | cut -d' ' -f 3 | tr ',' ' ' | tr -d '\n' >> $WORKDIR/$MYNAME 
    echo ")" >> $WORKDIR/$MYNAME 
    cd $WORKDIR
    mkdir log
    #queue dataset, logname from ( ..) 
    #echo ">> Going To submit $WORKDIR/$MYNAME"
    #condor_submit $MYNAME
fi

if [[ "$point" != "cards"* ]] ; then
    cd $DATACARDS
    echo " --- Doing $point --- "
    combine -M MultiDimFit -t -1 --toysFile  higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$point -n "_baseline"  -d ${CARDNAME} --saveWorkspace --setParameters r=0 --freezeParameters r 
    combine -M MultiDimFit -t -1 --toysFile  higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$point -n "_${aqgc_par}_$point" -d higgsCombine_baseline.MultiDimFit.mH120.VALUE${point}.root  --skipInitialFit --algo fixed --fixedPointPOIs r=1 --snapshotName MultiDimFit
    rm higgsCombine_baseline.MultiDimFit.mH120.VALUE${point}.root
    mv -v higgsCombine_${aqgc_par}_${point}.MultiDimFit.mH120.VALUE${point}.root $WORKDIR/

    cd $WORKDIR
    # check how many are done
    DONE=$(ls higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root | wc -l)
    TODO=$(cat $DATACARDS/$CARDNAME | tail -n 1 | cut -d' ' -f 3 | tr ',' '\n' | wc -l )  
    [ "${DONE}" == "${TODO}" ] && [ ${DONE} -gt 0 ] && {
        hadd higgsCombine_${aqgc_par}_all.root higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root 
        rm higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root
    }
fi


