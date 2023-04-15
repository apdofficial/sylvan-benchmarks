import os, sys, subprocess

def runOnAll(inDir, inExt, cmdList, inFIndex, outDir, outExt, outFIndex, cmdOutFile, skip=False, createDirs = True):
	i = 1
	for d, subds, fnames in os.walk(inDir):
		for fname in fnames:
			if not fname.endswith('.'+inExt):
				continue
			fpath = os.path.join(d,fname)
			cmd = list(cmdList)
			cmd.insert(inFIndex,fpath)
			ofname = os.path.splitext(fname)[0]+'.'+outExt
			ofDir = os.path.join(outDir,os.path.relpath(d,inDir))
			ofpath = os.path.join(ofDir,ofname)
			if createDirs:
				print(ofDir)
				os.system("mkdir -p {0}".format(ofDir))
			if skip:
				if os.path.isfile(ofpath):
					print('Not executing command ',cmd, ' as output file already exists.')
					continue
			cmd.insert(outFIndex,ofpath)
			print (str(i)+': Executing command: ',' '.join(cmd))
			p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
			out, err = p.communicate()
			f = open(cmdOutFile,'a')
			f.write('\n\n======================================\n')
			f.write('File: '+fpath+'\n')
			f.write('Out: \n')
			f.write(str(out))
			f.write('Err: \n')
			f.write(str(err))
			f.close()
			i += 1