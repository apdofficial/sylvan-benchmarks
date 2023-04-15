from runOnAll import runOnAll
from sys import argv

#inDir = argv[1]
#outDir = argv[2]

inDir = "/home/adi/Downloads/prob_inf/benchmarks/cnf/arjun/"
outDir = "/home/adi/Downloads/prob_inf/benchmarks/gr/arjun_mis_proj/"

cmdList = ['python','misProjGr.py']
runOnAll(inDir=inDir,inExt='cnf',cmdList=cmdList,inFIndex=2,outDir=outDir,outExt='gr',outFIndex=3,cmdOutFile='conversion_logs/convert_arjun_proj_cnfs.log',skip=True)

