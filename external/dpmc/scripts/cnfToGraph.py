from sys import argv
from cnfParser import parseCNF
from cnfParser import parseMCC21CNF

inF = argv[1]  #input cnf file
outF = argv[2] #output graph file
gType = int(argv[3]) #graph type. 0 - primal; 1 - incidence

def countPrimalEdges(cls):
    total = 0
    for cl in cls:
        total += len(cl)*(len(cl)-1)/2
    return int(total)

def countIncidenceEdges(cls):
    total = 0
    for cl in cls:
        total += len(cl)
    return total    

#nVars, nCls, clauses, weighted, wEncountered, wType, litWts, projected, vpEncountered, projVars = parseCNF(inF)
nVars, nCls, clauses, probType, wEncountered, litWts, projVars, ind = parseMCC21CNF(inF)

oF = open(outF,'w')
if gType == 0:
    oF.write('p tw '+str(nVars)+' '+str(countPrimalEdges(clauses))+'\n')
    for cl in clauses:
        for i in range(len(cl)):
            for j in range(i+1,len(cl)):
                oF.write(str(abs(cl[i]))+' '+str(abs(cl[j]))+'\n')
else:
    oF.write('p tw '+str(nVars+nCls)+' '+str(countIncidenceEdges(clauses))+'\n')
    j = nVars + 1
    for cl in clauses:
        for i in range(len(cl)):
            oF.write(str(abs(cl[i]))+' '+str(abs(j))+'\n')
        j += 1
oF.close()
