import pandas as pd
import sys

if len(sys.argv) < 2:
	print('needs a file as an argument')
	exit()

file = pd.read_csv(sys.argv[1])

if (len(sys.argv) >= 3):
	out = sys.argv[2]
else:
	out = sys.argv[1]

if (len(sys.argv) == 4):
	skip = int(sys.argv[3])
else:
	skip = 0

file[file['phase'] != 65535][['MagData','phase']].ix[skip:].to_csv(out, index=False)

