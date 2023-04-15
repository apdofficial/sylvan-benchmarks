from os import sys
import matplotlib.pyplot as plt
import json

#list indexes should be entered as command line arguments
#1-5 dpmc 6:sstd

colors=['green','blue','red','brown','orange']
mkrs= ['dummy','^','+','x','D','o']

def loadData():
	with open("dpmcData.json", 'r') as f:
		dpmcStats = json.load(f)
	with open("sstdData.json", 'r') as f:
		sstdStats = json.load(f)
	return dpmcStats, sstdStats

'''
1 vanilla with htd
2 jp=f sa=2 aa=1
3 jp=f sa=0 aa=1
4 vanilla flowcutter
5 jp=f sa=0 aa=0
'''

def plotComparison(lists):
	j = 0
	for r in [list(range(100)),list(range(100,200)),list(range(200,300)),list(range(300,400))]:
		fig, ax = plt.subplots()
		for l in lists:
			ax.plot([item%100 for item in l[2] if item in r], [1005+10*l[4] for item in l[2] if item in r], marker=l[3], linestyle="", alpha=0.8)#, color="r")		
			ax.bar(range(100), [l[0][i] for i in r], label=l[1]+' '+l[3], alpha=0.8)
			if len(l)>7:
				sC = [i%100 for i in r if l[5][i]==True and l[0][i]>998]
				sA = [i%100 for i in r if l[6][i]==True and l[0][i]>998]
				ax.plot(sC,[l[7]*100-10]*len(sC),marker = 'o', alpha=0.8, linestyle="", color='g')
				ax.plot(sA,[l[7]*100+10]*len(sA),marker = 'x', alpha=0.8, linestyle="",color='g')
		ax.set_ylabel('Time')
		ax.set_title(['MCC21_t1','MCC21_t2','MCC22_t1','MCC22_t2'][j])
		ax.legend()
		ax.set_ylim([0,1040])
		#plt.savefig('graphs/primal-incidence_comparison/mcc21_track1-2.png',bbox_inches='tight')
		
		plt.show()		
		j += 1

dS, sS = loadData()
lists = []
labels=['dummy','vanilla-htd','jp=f,sa=2,aa=1','jp=f,sa=0,aa=1','vanilla-fc','jp=f,sa=0,aa=0','jp=s,sa=2,aa=1','jp=s,sa=0,aa=0 (vanilla 1ca0c commit)','jp=s, sa=0, aa=1','sstd','jp=s for both satfilter and executor (6.5)', 'jp=f,sa=0,aa=1 in-built addabstract only unweighted']
for i in range(1,len(sys.argv)):
	ind = float(sys.argv[i])
	if ind != 6.5 and ind != 3.5:
		ind = int(ind)
		indSub = ind
	elif ind == 6.5:
		indSub = 10
	elif ind == 3.5:
		indSub =11
	
	if ind < 9:
		l = dS[str(ind)] #json always stores keys as strings
	else:
		l = sS['1']
	
	if ind != 3.5:
		times = [item[1][-2] if item[1][-2] != None else 1000 for item in l]
		memouts = [k for k in range(400) if l[k][0] > 29000000]
	else: #only unweighted so pad the weighted indices with timeouts. ignoring memouts.
		allowedExps = list(range(100))+list(range(200,300))
		times = [l[k if k<100 else k-100][1][-2] if k in allowedExps and l[k if k<100 else k-100][1][-2] != None else 1000 for k in range(400)]
		memouts = [(k if k<100 else k-100) for k in range(400) if k in allowedExps and l[k if k<100 else k-100][0] > 29000000]
	for item in memouts:
		assert(l[item][1][-2] == None)
	toAppend = [times,labels[indSub],memouts,mkrs[i],i]
	if ind == 2 or ind == 6 or ind == 6.5: #satfilter was active so check if satfilter was completed
		sC = [item[1][0] for item in l] #filter constructed
		sA = [item[1][1] for item in l] #filter applied
		toAppend.append(sC)
		toAppend.append(sA)
		toAppend.append(ind)
	lists.append(toAppend)	
print("following instances were solved by arjun directly (sstd shows timeouts): 3,4,5,53,200,201,204,206,208,216,217,219,274")		
plotComparison(lists)
