import pandas as pd
import numpy as np
import sys

if len(sys.argv) < 2:
	print('needs a file as an argument')
	exit()

file = pd.read_csv(sys.argv[1])

if (len(sys.argv) >= 3):
	out = sys.argv[2]
else:
	out = 'output'

if (len(sys.argv) == 4):
	skip = int(sys.argv[3])
else:
	skip = 0

# file[file['phase'] != 65535][['MagData','phase']].ix[skip:].to_csv(out, index=False)

file = file[file['phase'] != 65535][['MagData','phase']].ix[skip:]

# reduce mag data to a usable number of elements by dividing by 7
file['adj'] = (file['MagData']/4).apply(round).apply(int)

def rng(low, high):
	def rngI(x):
		return x >= low and x < high
	return rngI

l = list()
for i in range(max(file['adj'])+1):
	f = file[file['adj'] == i]['phase']
	if max(f) - min(f) > 100:
		# print list(f)
		t = filter(rng(0, 100), f)
		b = filter(rng(600,1000), f)
		if len(t) > len(b):
			f = t
		else:
			f = b
	l.append(int(round(np.mean(f))))

numPerLine = 32

s = '[\n '
for i in range(len(l)):
	s = s+'{!s:>3},'.format(l[i])
	if i % numPerLine is numPerLine-1:
		s=s+'\n '
s=s+']'

print len(l)

f = open(out, 'w')
f.write(s)
f.close()
