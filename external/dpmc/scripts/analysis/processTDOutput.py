from outputFileProcessors import processTDFile, processHTBFile
import json
from sys import argv
from os.path import exists

tdType = int(argv[1])

def processFCOutput(resDir):
	'''
	Process Flowcutter output files
	'''
	tws = []
	fcDir = resDir + '/mcc/'
	expectedName = None
	#vanilla primal
	for i in range(400):
		fp = open(fcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws.append([nvars,twList])
		fp.close()
	#vanilla incidence	
	for i in range(400,800):
		fp = open(fcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws[i-400].append(twList)
		fp.close()
	
	pmcDir = resDir + '/pmc/mcc/'
	#pmc primal unweighted
	for i in list(range(100))+list(range(200,300)):
		fp = open(pmcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws[i].append(twList)
		fp.close()
	#pmc incidence unweighted
	for i in list(range(400,500))+list(range(600,700)):
		fp = open(pmcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws[i-400].append(twList)
		fp.close()
	return tws

def processTamakiHTDOutput(resDir):
	tws = []
	fcDir = resDir + '/mcc/'
	expectedName = None
	#vanilla primal
	for i in range(400):
		fp = open(fcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws.append([nvars,twList])
		fp.close()
	#vanilla incidence	
	for i in range(400,800):
		fp = open(fcDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws[i-400].append(twList)
		fp.close()
	return tws

def processArjunHTDOutput(resDir, nFiles):
	tws = []
	expectedName = None
	for i in range(nFiles):
		fp = open(resDir+'td_array_'+str(i)+'.out')
		nvars, twList = processTDFile(fp, expectedName)
		tws.append([nvars,twList])
		fp.close()
	return tws

def processColAMDOutput(resDir):
	expectedName = None
	res = {}
	for sdir in ['7bmt','m7bmt','7bet','m7bet']:
		tws = []
		for i in range(400):
			fp = open(resDir+'/'+sdir+'/td_array_'+str(i)+'.out')
			nvars, twList = processHTBFile(fp, expectedName)
			tws.append([nvars,twList])
			fp.close()
		res[sdir]=tws
	return res

if tdType == 1:	
	print("Process fc...")
	fcDir = '/home/adi/Downloads/prob_inf/results/td/flowcutter/output/'
	tws = processFCOutput(fcDir)
	with open("fcData.json", 'w') as f:
		json.dump(tws, f, indent=2) 
elif tdType == 2:
	print("Process tamaki...")
	tamakiDir = '/home/adi/Downloads/prob_inf/results/td/tamaki/output/'
	tws = processTamakiHTDOutput(tamakiDir)
	with open("tamakiData.json", 'w') as f:
		json.dump(tws, f, indent=2) 
elif tdType == 3:
	print("Process htd...")
	htdDir = '/home/adi/Downloads/prob_inf/results/td/htd/output/'
	tws = processTamakiHTDOutput(htdDir)
	with open("htdData.json", 'w') as f:
		json.dump(tws, f, indent=2)
elif tdType == 4:
	print("Process arjun htd...")
	htdDir = '/home/adi/Downloads/prob_inf/results/td/htd/output/arjun/mcc/'
	tws = processArjunHTDOutput(htdDir, 400)
	with open("arjun-mis-proj_Data.json", 'w') as f:
		json.dump(tws, f, indent=2)

elif tdType == 5:
	print("Process arjun_no-proj htd...")
	htdDir = '/home/adi/Downloads/prob_inf/results/td/htd/output/arjun_no-proj/mcc/'
	tws = processArjunHTDOutput(htdDir, 800)
	with open("arjun_no-proj_Data.json", 'w') as f:
		json.dump(tws, f, indent=2)

elif tdType == 6:
	print("Process arjun_no-proj colamd htb...")
	htdDir = '/home/adi/Downloads/prob_inf/results/td/colamd/output/arjun_no-proj/mcc/'
	res = processColAMDOutput(htdDir)
	with open("colamd_Data.json", 'w') as f: #colamd was also run on arjun_no-proj .
		json.dump(res, f, indent=3)
