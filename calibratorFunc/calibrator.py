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

file['adj'] = (file['MagData']/7).apply(round).apply(int)

l = list()
for i in range(max(file['adj'])+1):
	l.append(int(round(np.mean(file[file['adj'] == i]['phase']))))

numPerLine = 20

s = '[\n '
for i in range(len(l)):
	s = s+'{!s:>3},'.format(l[i])
	if i % numPerLine is numPerLine:
		s=s+'\n '
s=s+']'

f = open(out, 'w')
f.write(s)
f.close()
