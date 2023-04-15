from subprocess import Popen, PIPE
from sys import argv

inF = argv[1]  #graph file
toolCMD = argv[2] # command to run td Solver


with Popen(["cmd", "arg1"], stdout=PIPE, bufsize=1,
           universal_newlines=True) as p:
    for line in p.stdout:
        '''
        if line has treewidth info, capture treewidth
        else write to file
        if checktime < 100
            if tw < 20
                terminate
        else //checkTime > 100
            if tw < 30
                terminate
            
        '''
        print(line, end='')
