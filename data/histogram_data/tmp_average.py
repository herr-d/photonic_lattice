import pandas as pd
import sys

if(len(sys.argv) != 2):
	print("Specify a file")
	exit(1)

data = pd.read_csv(sys.argv[1],sep=" ",header=None)

print(str(data.mean()[0]) + " " + str(data.sem()[0]))
