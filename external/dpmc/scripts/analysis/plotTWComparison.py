import matplotlib.pyplot as plt
import json

colors=['green','blue','red','brown','orange']
mkrs= ['o','^','+','x','D']

def loadData():
	with open("fcData.json", 'r') as f:
		fcTWs = json.load(f)
	with open("tamakiData.json", 'r') as f:
		tamakiTWs = json.load(f)
	with open("htdData.json", 'r') as f:
		htdTWs = json.load(f)
	return fcTWs, tamakiTWs, htdTWs

def plotBars(plList, thresh, plotPMC = False):
	print(plList)
	fig, ax = plt.subplots()
	ax.bar(range(0,200,2), [0 if len(el[1])==0 else el[1][-1] if el[1][-1] < thresh else thresh for el in plList], label='Primal')
	ax.bar(range(0,200,2), [0 if len(el[2])==0 else el[2][-1] if el[2][-1] < thresh else thresh for el in plList], label='Incidence',alpha=0.8)
	if plotPMC == True:
		ax.bar(range(1,200,2), [0 if len(el[3])==0 else el[3][-1] if el[3][-1] < thresh else thresh for el in plList], label='PMC Primal',alpha=0.8)
		ax.bar(range(1,200,2), [0 if len(el[4])==0 else el[4][-1] if el[4][-1] < thresh else thresh for el in plList], label='PMC Incidence',alpha=0.8)
	ax.set_ylabel('TWs')
	ax.set_title('Benchmarks')
	ax.legend()
	#plt.savefig('graphs/primal-incidence_comparison/mcc21_track1-2.png',bbox_inches='tight')
	plt.show()

def plotFC(tws):
	#sort by l[0] for sorting by number of vars. But the graphs therein did not look very informative. So using the first primal tw returned. This might lead to some heuristic for when to switch to incidence, preprocessing etc
	mcc21_t1 = sorted([[tws[i][0],tws[i][1],tws[i][2],tws[i][3],tws[i][4]] for i in range(100)], key=lambda l:l[1][0] if len(l[1]) >0 else 0)
	mcc21_t2 = sorted([[tws[i][0],tws[i][1],tws[i][2]] for i in range(100,200)], key=lambda l:l[1][0] if len(l[1]) >0 else 0)
	mcc22_t1 = sorted([[tws[i][0],tws[i][1],tws[i][2],tws[i][3],tws[i][4]] for i in range(200,300)], key=lambda l:l[1][0] if len(l[1]) >0 else 0)
	mcc22_t2 = sorted([[tws[i][0],tws[i][1],tws[i][2]] for i in range(300,400)], key=lambda l:l[1][0] if len(l[1]) >0 else 0)

	plotBars(mcc21_t1,300,True)
	plotBars(mcc21_t2,300,False)
	plotBars(mcc22_t1,300,True)
	plotBars(mcc22_t2,300,False)
	#print(tws)

def plotSolverComparison(fl,tl,hl, twType, thresh):
	#for each of 4 sets
		#for each of hl tl fl
			#create list
			#for each of 100 benchmarks
				#add time to list
		#plot bars
	j = 0
	assert twType == 1 or twType == 2 #primal or incidence
	for r in [list(range(100)),list(range(100,200)),list(range(200,300)),list(range(300,400))]:
		fig, ax = plt.subplots()
		i = 0
		for l in [fl, tl, hl]:
			outL = []
			for ind in r:
				outL.append(0 if len(l[ind][twType])==0 else l[ind][twType][-1] if l[ind][twType][-1] < thresh else thresh)
			ax.bar(range(i,300,3), outL, label=['Flow','Tamaki','HTD'][i])
			i += 1
		ax.set_ylabel('TWs')
		ax.set_title(['MCC21_t1','MCC21_t2','MCC22_t1','MCC22_t2'][j])
		ax.legend()
		#plt.savefig('graphs/primal-incidence_comparison/mcc21_track1-2.png',bbox_inches='tight')
		plt.show()		
		j += 1
fl,tl, hl = loadData()
#plotFC(fl)
plotSolverComparison(fl,tl,hl,2,300)
