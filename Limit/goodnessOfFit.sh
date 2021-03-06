# Base off ./HIG16006/input/mssm_protocol.txt



# We look at the Goodness of Fit for three different algorithms. 
# The saturated model (saturated), Anderson-Darling (AD) and 
# Kolmogorow-Smirnow (KS). For the AD and KS it is sufficient to 
# run the fits for the combined cards as the test-statitic for 
# the individual channels can be extracted from these results. 
# For the saturated model it is necessary to run them independtly 
# of each other.


###  THESE COMMANDS PRODUCE UNBLINDED RESULTS!!!  ###
echo ""
echo ""
echo "THESE COMMANDS PRODUCE UNBLINDED RESULTS!!!"
echo ""
echo ""




# Do Anderson Darling first
ALGO=AD
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO.toys" --fixedSignalStrength=1 -t 25 -s 0:19:1 --parallel 12
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO" --fixedSignalStrength=1

combineTool.py -M CollectGoodnessOfFit --input higgsCombine.${ALGO}.GoodnessOfFit.mH14.root higgsCombine.${ALGO}.toys.GoodnessOfFit.mH14.*.root -o collectGoodness_${ALGO}.json

python ../../../../../CombineTools/scripts/plotGof.py --statistic ${ALGO} --mass 14.0 collectGoodness_${ALGO}.json --title-right="35.9 fb^{-1} (13 TeV)" --output='-AD'





# Do KS test
ALGO=KS
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO.toys" --fixedSignalStrength=1 -t 25 -s 0:19:1 --parallel 12
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO" --fixedSignalStrength=1

combineTool.py -M CollectGoodnessOfFit --input higgsCombine.${ALGO}.GoodnessOfFit.mH14.root higgsCombine.${ALGO}.toys.GoodnessOfFit.mH14.*.root -o collectGoodness_${ALGO}.json

python ../../../../../CombineTools/scripts/plotGof.py --statistic ${ALGO} --mass 14.0 collectGoodness_${ALGO}.json --title-right="35.9 fb^{-1} (13 TeV)" --output='-KS'






# Do Saturated test
ALGO=saturated
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO.toys" --fixedSignalStrength=1 -t 25 -s 0:19:1 --parallel 12
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 14 --there -d workspace.root -n ".$ALGO" --fixedSignalStrength=1

combineTool.py -M CollectGoodnessOfFit --input higgsCombine.${ALGO}.GoodnessOfFit.mH14.root higgsCombine.${ALGO}.toys.GoodnessOfFit.mH14.*.root -o collectGoodness_${ALGO}.json

python ../../../../../CombineTools/scripts/plotGof.py --statistic ${ALGO} --mass 14.0 collectGoodness_${ALGO}.json --title-right="35.9 fb^{-1} (13 TeV)" --output='Saturated'







