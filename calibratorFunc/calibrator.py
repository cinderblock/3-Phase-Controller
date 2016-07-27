from __future__ import division
import pandas as pd
import numpy as np
import sys

if len(sys.argv) < 2:
	print('needs a file as an argument')
	exit()

file = pd.read_csv(sys.argv[1])

if (len(sys.argv) >= 3):
	out = '../LookupTable/'+sys.argv[2]
else:
	out = '../LookupTable/'+'output.cpp'

if (len(sys.argv) == 4):
	skip = int(sys.argv[3])
else:
	skip = 0

# file[file['phase'] != 65535][['MagData','phase']].ix[skip:].to_csv(out, index=False)

file = file[file['phase'] != 65535][['MagData','phase']].ix[skip:]

# reduce mag data to a usable number of elements by dividing by 7
file['adj'] = (file['MagData']/4).apply(round).apply(int)

maxValue = 768
elctricalPerMechanical = 7

def rng(low, high):
	def rngI(x):
		return x >= low and x < high
	return rngI

l = list()
elecToMech = 0#elctricalPerMechanical-1

for i in range(max(file['adj'])+1):

	f = file[file['adj'] == i]['phase']
	if f.empty:
		print i, " is missing!"
		continue
	elif max(f) - min(f) > 100:
		# print list(f)
		t = filter(rng(0, 100), f)
		b = filter(rng(600,1000), f)

		value = sum(b) + sum(t) + len(t) * maxValue
		value = value / len(f)

		value = int(round(value))

		if(value >= maxValue):
			value = value - maxValue

		# if len(t) > len(b):
		# 	f = t
		# else:
		# 	f = b
	else:
		value = int(round(np.mean(f)))

	if len(l) > 0 and abs(value - (l[len(l)-1] & 0x0FFF)) > 500:
		print value, l[len(l)-1]
		elecToMech = elecToMech + 1

		if elecToMech == elctricalPerMechanical:
			elecToMech = 0#elctricalPerMechanical -1
	
	l.append(value + elecToMech * pow(2, 12))

numPerLine = 20
shift = 0



s = '// File generated with python in calibratorFunc Folder'+'\n'
s+= '#include "../LookupTable.h"'+'\n'
s+='\n'
s+='\n'
s+='/**'+'\n'
s+=' * 12-bit lookup table for magnetometer Alpha value to Phase value'+'\n'
s+=' */'+'\n'
s+='const u2 Lookup::table[] PROGMEM = {'+'\n\t'

for i in range(len(l)):
	temp = i + shift
	if temp >= len(l):
		temp -= len(l)
	elif temp < 0:
		temp += len(l)

	s = s+'0x{:04x},'.format(l[temp])
	# s = s+'{!s:>3},'.format(l[i], '#04x')
	if i % numPerLine is numPerLine-1:
		s=s+'\n\t'

s+='\n'
s+='};\n'
s+='\n'
print len(l)

f = open(out, 'w')
f.write(s)
f.close()
