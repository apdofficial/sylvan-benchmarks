from sys import exit
from math import isclose

def parseMCC21CNF(inF, verbose = 1):
	if verbose >= 1: print('Parsing file ',inF)
	clauses = []
	nVars = -1
	nCls = -1
	f = open(inF,'r')
	clsCounter = 0
	probType = None
	pEncountered = False #projection vars encountered
	headerEncountered = False #p cnf line encountered
	wEncountered = False #weights encountered
	
	litWts = {}
	projVars = set()
	inds = set() #independent set
	for line in f:
		if line.strip() == '':
			print('WARN: Empty line encountered while parsing CNF. Skipping..')
			continue
		if line.startswith('c'):
			if line.startswith('c t '):
				probType = line.split()[2]
				assert probType in ['mc', 'wmc', 'pmc', 'pwmc']
				continue
			elif line.startswith('c p weight'):
				wEncountered = True
				wds = line.split()
				lit = int(wds[3])
				litWts[lit] = float(wds[4])
				continue
			elif line.startswith('c p show'):
				pEncountered = True
				wds = line.split()
				for i in range(3,len(wds)):
					var = int(wds[i])
					if var != 0:
						projVars.add(var)
				continue
			elif line.startswith('c ind'):
				wds = line.split()
				for i in range(2,len(wds)):
					var = int(wds[i])
					if var != 0:
						inds.add(var)
				continue
			else:
				continue
		if line.startswith('p'):
			if headerEncountered:
				print ("Error: Already encountered line starting with p. Second Line: ",line)
				exit(1)
			else:
				headerEncountered = True
			wds = line.split()
			assert 'cnf' in wds[1]
			nVars = int(wds[2])
			nCls = int(wds[3])
			continue
		if not headerEncountered:
			print ("Error: Encountered clause line:",line," before header p cnf..")
			exit(1)
		clsCounter += 1
		wds = line.split()
		cl = []
		for wd in wds:
			cl.append(int(wd))
		assert(cl[-1]==0)
		cl.pop()
		clauses.append(cl)
	assert clsCounter == nCls
	f.close()
	if wEncountered: #fill up values of remaining lits
		keys = litWts.keys()
		for el in keys:
			assert el <= nVars and el > 0
			val = litWts[el]
			assert isclose(val,1) or isclose(val,0) or (val < 1 and val > 0)
			if -el not in keys:
				litWts[-el] = 1 - val
			else:
				assert isclose(litWts[-el], 1 - val)
	for el in projVars:
		assert el <= nVars and el > 0
	for el in inds:
		#assert el <= nVars and el > 0
		if not (el <= nVars and el > 0):
			print("Ind set element out of bounds",el)
			exit(1)
	if verbose >= 1: print('File parsed!')
	return nVars, nCls, clauses, probType, wEncountered, litWts, projVars, inds

def parseCNF(inF, verbose = 1):
	'''
	For C2D or Cachet style CNF format
	'''
	if verbose >= 1: print('Parsing file ',inF)
	weighted = False
	projected = False
	clauses = []
	nVars = -1
	nCls = -1
	f = open(inF,'r')
	clsCounter = 0
	pEncountered = False
	wEncountered = False
	vpEncountered = False
	wType = None
	litWts = {}
	projVars = set()
	for line in f:
		if line.strip() == '':
			print("WARN: Empty line found in CNF file. Skipping")
			continue
		if line.startswith('c'):
			if line.startswith('c weights'):
				assert wType == None #to ensure that not encountered a weight line of different type or another c weight line
				wType = 'c weights'
				# not checking for how many weights are specified here because p cnf line may be later
				wEncountered = True
				wds = line.split()
				for i in range(2,len(wds)):
					lit = int(i/2) if i%2 == 0 else -int(i/2)
					litWts[lit] = float(wds[i])
					assert(litWts[lit]>=0)
				continue
			else:
				continue
		if line.startswith('p'):
			if pEncountered:
				print ("Error: Already encountered line starting with p. Second Line: ",line)
				exit(1)
			else:
				pEncountered = True
			wds = line.split()
			assert 'cnf' in wds[1]
			if 'w' in wds[1]:
				weighted = True
			if 'p' in wds[1]:
				projected = True
			nVars = int(wds[2])
			nCls = int(wds[3])
			continue
		if line.startswith('w'):
			assert wType != 'c weights'
			wType = 'w lit wt'
			wEncountered = True
			wds = line.split()
			litWts[int(wds[1])] = float(wds[2])
			assert(len(wds)<4 or wds[3] == '0')
			continue
		if line.startswith('vp'):
			vpEncountered = True
			wds = line.split()
			assert(wds[-1] == '0')
			for i in range(1, len(wds)):
				projVars.add(int(wds[i]))
			continue
		if not pEncountered:
			print ("Error: Encountered clause line:",line," before header p cnf..")
			exit(1)
		clsCounter += 1
		wds = line.split()
		cl = []
		for wd in wds:
			cl.append(int(wd))
		assert(cl[-1]==0)
		cl.pop()
		clauses.append(cl)
	if projected == False and vpEncountered == True:
		if verbose >= 1: print ("WARNING: projection vars encountered in a file with header p without pcnf")
	if weighted == False and wEncountered == True:
		if verbose >= 1: print ("WARNING: weights encountered in a file with header p without wcnf")
	if wType == 'c weights':
		assert len(litWts) == 2*nVars
	f.close()
	if verbose >= 1: print('File parsed!')
	return nVars, nCls, clauses, weighted, wEncountered, wType, litWts, projected, vpEncountered, projVars


