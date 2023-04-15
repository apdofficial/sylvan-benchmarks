from sys import argv, stdin

inp = stdin.readlines()
stdin.close()
f = open(argv[1],'r')
for line in f:
	print(line,end='',flush=True)
f.close()
print('=',flush=True)
while True:
	pass
