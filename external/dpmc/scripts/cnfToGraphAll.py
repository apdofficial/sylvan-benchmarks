from runOnAll import runOnAll
from sys import argv

inDir = argv[1]
outDir = argv[2]

cmdList = ['python3','cnfToGraph.py','0']
runOnAll(inDir=inDir,inExt='cnf',cmdList=cmdList,inFIndex=2,outDir=outDir+'/primal/arjun_no-proj/mcc/',outExt='gr',outFIndex=3,cmdOutFile='conversion_logs/convert_arjun_no-proj_mcc_primal.log',skip=True)

cmdList = ['python3','cnfToGraph.py','1']
runOnAll(inDir=inDir,inExt='cnf',cmdList=cmdList,inFIndex=2,outDir=outDir+'/incidence/arjun_no-proj/mcc/',outExt='gr',outFIndex=3,cmdOutFile='conversion_logs/convert_arjun_no-proj_mcc_incidence.log',skip=True)
