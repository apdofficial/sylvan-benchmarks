from cnfParser import parseCNF
import sys

inF = sys.argv[1]
outF = sys.argv[2]

nVars, nCls, clauses, weighted, wEncountered, wType, litWts, projected, vpEncountered, projVars = parseCNF(inF)

# If both lits have equal weight we set both weights to 1

with open(outF,'w') as o:
	o.write('p cnf '+str(nVars)+' '+str(nCls)+'\n')
	if wEncountered:
		lits = list(litWts.keys())
		lits = sorted(lits,key=abs)
		i = 0
		while i<len(lits):
			currLit = lits[i]
			if i<len(lits)-1:
				nexLit = lits[i+1]
				if abs(currLit) == abs(nexLit):
					'''
					if litWts[currLit] == litWts[nexLit]:
						#scale both up to 1
						o.write('c p weight '+str(currLit)+' 1\n')
						o.write('c p weight '+str(nexLit)+' 1\n')
						i += 2
						continue
					'''
			o.write('c p weight '+str(currLit)+' '+str(litWts[currLit])+'\n')
			i+=1
		assert(len(clauses) == nCls)
		for cl in clauses:
			for v in cl:
				o.write(str(v)+' ')
			o.write('0\n')
