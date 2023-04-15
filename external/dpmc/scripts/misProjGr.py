from sys import argv
from cnfParser import parseMCC21CNF
from disjoint_set import DisjointSet


def countIncidenceEdges(cls):
    total = 0
    for cl in cls:
        total += len(cl)
    return total    


#compute X , Y clause sets
def getClauseSets(clauses, Y):
	Ycls = []
	Xcls = []
	for cl in clauses:
		found = False
		for var in cl:
			if abs(var) in Y:
				Ycls.append(cl)
				found = True
				break
		if not found:
			Xcls.append(cl)
	return Xcls, Ycls
#print (Ycls,"\n\n", Xcls)

def computeComponents(Y, Xcls, Ycls, clauses):		
	ds = DisjointSet()
	for cl in Ycls:
		canon = None
		for var in cl:
			if abs(var) not in Y:
				continue
			if canon == None:
				canon = ds.find(abs(var))
			else:
				ds.union(canon,abs(var))
	clMap = {}
	for cl in Ycls:
		xvars = []
		canon = None
		for var in cl:
			if abs(var) not in Y:
				xvars.append(abs(var))
				continue
			if canon == None:
				canon = ds.find(abs(var))
				if canon in clMap.keys():
					clMap[canon][0].append(cl)
				else:
					clMap[canon]=[[cl],set()]
		clMap[canon][1].update(xvars) #in place union
	return clMap
	

def countPrimalEdges(cls):
    total = 0
    for cl in cls:
        total += len(cl)*(len(cl)-1)/2
    return int(total)


def writeGraph(nVars, Xcls, Ycls, clMap, outF):
	totEdges = 0
	for k,v in clMap.items():
		totEdges += countPrimalEdges(v[0])
	totEdges += countPrimalEdges(Xcls)
	for k,v in clMap.items(): # virtual clauses
		totEdges += len(v[1])*(len(v[1])-1)/2
		totEdges = int(totEdges)
	oF = open(outF,'w')
	oF.write('p tw '+str(nVars)+' '+str(totEdges)+'\n')
	
	# Write Y components
	for k,v in clMap.items():
		#write primal graph
		for cl in v[0]: #v[0] Ycls component
			for i in range(len(cl)):
				for j in range(i+1,len(cl)):
					oF.write(str(abs(cl[i]))+' '+str(abs(cl[j]))+'\n')
	# Write Xcls
	for cl in Xcls:
		for i in range(len(cl)):
			for j in range(i+1,len(cl)):
				oF.write(str(abs(cl[i]))+' '+str(abs(cl[j]))+'\n')
	# Write virtual clauses
	for k,v in clMap.items():
		cl = list(v[1]) #v[1] is vars in virtual clause for that component 
		for i in range(len(cl)):
			for j in range(i+1,len(cl)):
				oF.write(str(abs(cl[i]))+' '+str(abs(cl[j]))+'\n')
	oF.close()

def main():
	inF = argv[1]  #input cnf file
	outF = argv[2] #output graph file

	nVars, nCls, clauses, probType, wEncountered, litWts, projVars, inds = parseMCC21CNF(inF)
	# X - sum vars Y - exist vars
	X = inds
	allVars = set(range(1,nVars+1))
	Y = allVars.difference(X)	
	
	# Y clauses - clauses that contain at least one Y var ; X clauses - all other clauses
	Xcls, Ycls = getClauseSets(clauses, Y)
	
	# do component analysis of Y clauses
	print("Computing components..")
	clMap = computeComponents(Y, Xcls, Ycls, clauses)
	
	print("Writing graph file..")
	writeGraph(nVars, Xcls, Ycls, clMap, outF)
	print("Done!")

#gType = int(argv[3]) #graph type. 0 - primal; 1 - incidence
if __name__ == "__main__":
	main()
