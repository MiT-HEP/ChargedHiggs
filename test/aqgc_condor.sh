#!/bin/bash

aqgc_par="$1"
point="$2" ## cards -> prepare the card; cards-sumbmit: prepare and submit
VAR="MVV"
[ "$3" != "" ] && export VAR="$3"

echo "-> Requested parameter $aqgc_par and point $point"

CMSSW="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src"
CHARGEDHIGGS="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs"
WDIR="AUG12_interpolate"
[ "$3" == "MVVClip" ] && WDIR="AUG12_clip"
DATACARDS="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs/Datacards/${WDIR}"
WORKDIR="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs/Datacards/${WDIR}/AQGC"
SUFFIX="aug12"
year=2021

cd $CMSSW
eval `scramv1 runtime -sh`

mkdir -p $WORKDIR

echo "Running $VAR in dir $WDIR"

INTERPOLATE=""
############## FS #############
############## FM #############
[ "$aqgc_par" == "fm7" ] && { for v in "m5p00" "m4p50" "m3p50" "m3p00" "m2p50" "m1p50" "m1p00" "m0p50" "0p50" "1p00" "1p50" "2p50" "3p00" "3p50" "4p50" "5p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
[ "$aqgc_par" == "fm5" ] && { for v in "m5p00" "m4p50" "m3p50" "m3p00" "m2p50" "m2p00" "m1p50" "m1p00" "m0p50" "0p50" "1p00" "1p50" "2p00" "2p50" "3p00" "3p50" "4p50" "5p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#[ "$aqgc_par" == "fm4" ] && { for v in "m5p00" "m4p50" "m3p50" "m3p00" "m2p50" "m1p50" "m1p00" "m0p50" "0p50" "1p00" "1p50" "2p50" "3p00" "3p50" "4p50" "5p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
[ "$aqgc_par" == "fm4" ] && { for v in "m5p00" "m4p50" "m3p50" "m3p00" "m2p50" "m1p50" "m1p00" "m0p50" "m0p40" "m0p30" "m0p20" "m0p10" "m0p05" "0p05" "0p10" "0p20" "0p30" "0p40" "0p50" "1p00" "1p50" "2p50" "3p00" "3p50" "4p50" "5p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
[ "$aqgc_par" == "fm3" ] && { for v in "m5p00" "m4p50" "m3p50" "m3p00" "m2p50" "m1p50" "m1p00" "m0p50" "0p50" "1p00" "1p50" "2p50" "3p00" "3p50" "4p50" "5p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #m6p00,m4p50,m3p00,m1p50,0p00,1p50,3p00,4p50,6p00
[ "$aqgc_par" == "fm2" ] && { for v in "m0p80" "m0p70" "m0p60" "m0p50" "m0p40" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p40" "0p50" "0p60" "0p70" "0p80" ; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
[ "$aqgc_par" == "fm1" ] && { for v in "m2p00" "m1p80" "m1p60" "m1p20" "m1p00" "m0p80" "m0p60" "m0p50" "m0p40" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p40" "0p50" "0p60" "0p80" "1p00" "1p20" "1p60" "1p80" "2p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #m5p40,m4p50,m3p60,m2p70,m1p80,m0p90,0p00,0p90,1p80,2p70,3p60,4p50,5p40
[ "$aqgc_par" == "fm0" ] && { for v in "m0p80" "m0p70" "m0p60" "m0p50" "m0p40" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p40" "0p50" "0p60" "0p70" "0p80" ; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
############## FT #############
#### #ft9 m1p00,m0p50,0p00,0p50
[ "$aqgc_par" == "ft9" ] && { for v in "m0p40" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p40" ; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #m1p00,m0p50,0p00,0p50,1p00
[ "$aqgc_par" == "ft8" ] && { for v in "m0p40" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p40" ; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #ft7 m1p20,m0p80,m0p40,0p00,0p40,0p80
[ "$aqgc_par" == "ft7" ] && { for v in "m1p00" "m0p90" "m0p70" "m0p60" "m0p50" "m0p30" "m0p20" "m0p10" "0p10" "0p20" "0p30" "0p50" "0p60" "0p70" "0p90" "1p00"; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #m1p00,m0p80,m0p60,m0p40,m0p20,0p00,0p20,0p40,0p60,0p80,1p00
[ "$aqgc_par" == "ft6" ] && { for v in  "m0p30" "m0p25" "m0p15" "m0p10" "m0p05" "0p05" "0p10" "0p15" "0p25" "0p30"  ; do INTERPOLATE+=" --aqgc_interpolate $v"; done ; }  
#### #ft5: ,m1p00,m0p80,m0p60,m0p40,m0p20,0p00,0p20,0p40,
[ "$aqgc_par" == "ft5" ] && { for v in "m0p09" "m0p08" "m0p07" "m0p06" "m0p05" "m0p04" "m0p03" "m0p02" "m0p01" "0p01" "0p02" "0p03" "0p04" "0p05" "0p06" "0p07" "0p08" "0p09" ; do  INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
#### #ft4: m0p30,m0p20,m0p10,0p00,0p10,0p20,0p30,0p40
#### #ft3: m0p40,m0p30,m0p20,m0p10,0p00,0p10,0p20,0p30,0p40 
[ "$aqgc_par" == "ft3" ] && { for v in "m0p45" "m0p35" "m0p25" "m0p15" "m0p05" "0p05" "0p15" "0p25" "0p35" "0p45" ; do  INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
#### #ft2  m0p10,0p00,0p10,0p20
[ "$aqgc_par" == "ft2" ] && { for v in "m0p09" "m0p08" "m0p07" "m0p06" "m0p05" "m0p04" "m0p03" "m0p02" "m0p01" "0p01" "0p02" "0p03" "0p04" "0p05" "0p06" "0p07" "0p08" "0p09" ; do  INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
#### #ft1  m0p05,0p00,0p05,0p10
[ "$aqgc_par" == "ft1" ] && { for v in "m0p09" "m0p08" "m0p07" "m0p06" "m0p04" "m0p03" "m0p02" "m0p01" "0p01" "0p02" "0p03" "0p04" "0p06" "0p07" "0p08" "0p09" ; do  INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
#### #ft0 m0p05,0p00,0p05,0p10
[ "$aqgc_par" == "ft0" ] && { for v in "m0p09" "m0p08" "m0p07" "m0p06" "m0p04" "m0p03" "m0p02" "m0p01" "0p01" "0p02" "0p03" "0p04" "0p06" "0p07" "0p08" "0p09" ; do  INTERPOLATE+=" --aqgc_interpolate $v"; done ; }
#### 

# prepare combined datacard SR+anti+side BB BBtag  BMET  RMET
CARDNAME="cms_vbshad_${year}_final_aqgc_${aqgc_par}_${VAR}.txt"

if [[ "$point" == "cards"* ]] ; then
    echo "-> Preparing Cards"
    cd $CHARGEDHIGGS
    CARDS=""
    CATEGORIES="BB BBtag BMET RMET"
    #CATEGORIES="RMET"
    REGIONS="SR anti side"
    ###CATEGORIES="BB"
    ###REGIONS="SR"
    for cat in $CATEGORIES ; do
        for reg in "SR" "anti" "side"; do 
        #for reg in "SR"; do  ## I don't have anti/side for aqgc
            [[ "$cat" == *"MET" ]]  && [ "$reg" == "anti" ] && continue; 
            echo ">> Cat $cat Reg $reg"
            file="HAD";
            [[ "$cat" == *"MET" ]] && { file="MET"; }
            [ "$cat" == "BBtag" ] && { file="BHAD"; }
            python script/bwsVBSHad.py --there -q 5 --aqgc -r $reg -s ${VAR} -y ${year} -c ${cat} -i ${file}${reg}_${year}_${SUFFIX}.root --aqgc_parameter $aqgc_par ${INTERPOLATE}
            EXITSTATUS=$?
            [ "$EXITSTATUS" == "0" ] || {
                     echo "Cards creation failed. |$EXITSTATUS| " ; 
                     echo "YEAR=${year} CAT=${cat} INPUT=${file}${reg}_${year}_${SUFFIX}.root PARAMETER= $aqgc_par";
                     exit 1; 
                 }

            EXTRA=""
            [ "$reg" == "anti" ] && EXTRA="_anti";
            [ "$reg" == "side" ] && EXTRA="_side";
            CARDS+=" cms_vbshad_${year}_5_aqgc_${aqgc_par}_${VAR}_${cat}_${reg}.txt"
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
    cat test/aqgc_condor.jdl | grep -v '^queue'  | sed 's:espresso:microcentury:g' | grep -v '^JobBatchName' > $WORKDIR/$MYNAME
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

    SM_VALUE="0p00"
    [ -f "higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root" ] || { 
    combine -M GenerateOnly -t -1 --expectSignal=1 -n "_AsimovSM" --keyword-value VALUE=${SM_VALUE} -d $DATACARDS/$CARDNAME --saveToys
    echo "Exit status |$?|";
    }
    touch "${WORKDIR}/${aqgc_par}.cards.done"
fi

if [ "$point" == "watchdog" ] ; then
    cd $WORKDIR
    DONE_JDL=$(ls *jdl | wc -l)
    DONE_DONE=$(ls *.cards.done | wc -l)
    TODO_JDL=$(echo "ft1 ft0 ft2 ft3 ft4 ft5 ft7 ft6 ft9 fm7 fm4 fm5 fm2 fm3 fm0 fm1 fs0 fs1 fs2 ft8" | wc -w) 
    guard="watchdog.done"
    [ "${DONE_JDL}" == "${TODO_JDL}" ] && [ "${TODO_JDL}" == "${DONE_DONE}" ] && [ ! -f $guard ] && {
        touch $guard 
        for jdl in *jdl ; do condor_submit $jdl ; mv -v $jdl log/ ; done
        } 
fi

if [[ "$point" != "cards"* ]] ; then
    cd $DATACARDS
    echo " --- Doing $point --- "
    combine -M MultiDimFit -t -1 --toysFile  $WORKDIR/higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$point -n "_${aqgc_par}_baseline"  -d ${CARDNAME} --saveWorkspace --setParameters r=0 --freezeParameters r 
    EXITSTATUS=$?
    echo "Exit status |${EXITSTATUS}|"
    [ "$EXITSTATUS" != "0" ] && { echo "ERROR running combine1"; exit 1; } 
    combine -M MultiDimFit -t -1 --toysFile  $WORKDIR/higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$point -n "_${aqgc_par}_$point" -d higgsCombine_${aqgc_par}_baseline.MultiDimFit.mH120.VALUE${point}.root  --skipInitialFit --algo fixed --fixedPointPOIs r=1 --snapshotName MultiDimFit
    EXITSTATUS=$?
    echo "Exit status |${EXITSTATUS}|"
    [ "$EXITSTATUS" != "0" ] && { echo "ERROR running combine2"; exit 1; } 

    echo ">Cleaning"
    rm higgsCombine_${aqgc_par}_baseline.MultiDimFit.mH120.VALUE${point}.root
    mv -v higgsCombine_${aqgc_par}_${point}.MultiDimFit.mH120.VALUE${point}.root $WORKDIR/

    cd $WORKDIR
    echo ">Check for completeness"
    # check how many are done
    DONE=$(ls higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root | wc -l)
    TODO=$(cat $DATACARDS/$CARDNAME | tail -n 1 | cut -d' ' -f 3 | tr ',' '\n' | wc -l )  

    echo ">> DONE: $DONE  -- TODO: $TODO "
    [ "${DONE}" == "${TODO}" ] && [ ${DONE} -gt 0 ] && [ ! -f hadd_${aqgc_par}.run ] && {
        touch  hadd_${aqgc_par}.run
        sleep 10s  ## make sure that all other job are well finished
        for f in  higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root; do
    python <<EOF || { echo "ERROR. $f did not produced a valid root file." ; rm hadd_${aqgc_par}.run ; touch hadd_${aqgc_par}.fail ; exit 1; } 
import ROOT
import sys
fIn=ROOT.TFile.Open("${f}")
tIn=fIn.Get("limit")
if tIn: sys.exit(0)
else: sys.exit(1)
EOF
        done
        hadd higgsCombine_${aqgc_par}_all.root higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root 
        rm hadd_${aqgc_par}.run
        rm higgsCombine_${aqgc_par}_*.MultiDimFit.mH120.VALUE*.root
    }
fi


