from datetime import timedelta
from dateutil import parser

def processTDFile(fp, expectedFName=None):
	fp.readline()
	fp.readline()
	fp.readline()
	fp.readline()
	fName = fp.readline()
	if expectedFName!=None:
		assert(fName == expectedFName)
	fp.readline()
	twList = []
	nvars = 0
	for line in fp:
		if line.startswith("s td"):
			tw = int(line.split()[3])
			nvars = int(line.split()[4])
			twList.append(tw)
	return nvars, twList

def processHTBFile(fp, expectedFName=None):
	fp.readline()
	fp.readline()
	fp.readline()
	fp.readline()
	fName = fp.readline()
	if expectedFName!=None:
		assert(fName == expectedFName)
	fp.readline()
	twList = []
	nvars = 0
	for line in fp:
		if line.startswith("c joinTreeWidth"):
			tw = int(line.split()[2])
			nvars = None #numvars is not output by htb
			twList.append(tw)
	return nvars, twList

def processDPMCFile(fp, expectedFName=None, expectedFPath=None):
	fp.readline()
	fp.readline()
	stTime = parser.parse(fp.readline())
	fp.readline()
	fp.readline()
	fPath = fp.readline()
	fName = fPath.split('/')[-1]
	if expectedFPath != None:
		assert (fPath == expectedFPath)
	if expectedFName!=None:
		assert (fName == expectedFName)
	totTime = None
	sfConstructed = False
	sfApplied = False
	for line in fp:
		if 'c Done constructing SatFilter.' in line:
			sfConstructed = True
		elif 'c Done Applying SatFilter!' in line:
			sfApplied = True	
		elif line.startswith("c seconds"):
			totTime = float(line.split()[-1])
		elif "slurm job finished at" in line:
			endTime = parser.parse(fp.readline())
		elif "DUE TO TIME LIMIT" in line:
			endTime = parser.parse(line.split("DUE")[0].split()[-1]).replace(tzinfo=stTime.tzinfo)
	duration = (endTime - stTime)/timedelta(seconds=1)
	return sfConstructed, sfApplied, totTime, duration
	
def processSSTDFile(fp, expectedFName=None, expectedFPath=None):
	fp.readline()
	fp.readline()
	stTime = parser.parse(fp.readline())
	fp.readline()
	fp.readline()
	fPath = fp.readline()
	fName = fPath.split('/')[-1]
	if expectedFPath != None:
		assert (fPath == expectedFPath)
	if expectedFName!=None:
		assert (fName == expectedFName)
	totTime = None
	for line in fp:
		if line.startswith('c o Solved.'):
			totTime = float(line.split()[-1])
		elif "slurm job finished at" in line:
			endTime = parser.parse(fp.readline())
		elif "DUE TO TIME LIMIT" in line:
			endTime = parser.parse(line.split("DUE")[0].split()[-1]).replace(tzinfo=stTime.tzinfo)
	duration = (endTime - stTime)/timedelta(seconds=1)
	return totTime, duration


#fp = open('/home/adi/Downloads/prob_inf/results/dpmc/arjun_no-proj/output/1/dpmc_array_0.out','r')
#a = processDPMCFile(fp)
#print (a)
