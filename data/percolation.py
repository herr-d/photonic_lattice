#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Scatter


output = "plots/percolation.html"

scatter = []

files = [#["combined_15", 'error rate 15%'],
#	["combined_20", 'error rate 20%'],
#	["combined_25", 'error rate 25%'],
	["percolation_20", 'size 20'],
	["percolation_80", 'size 80']
#	["comb", 'error rate 25%'],
#	["combined_30", 'error rate 30%']
]


for i in files:
	try:
		data = pd.read_csv(i[0], sep=" ",header=None)
	except:
		print("could not load: " + i)
		exit(1)


	mean = data.groupby(1,as_index=False).mean()

	err = data.groupby(1,as_index=False).sem()

	scatter.append(Scatter(x=mean[1], y=mean[2], name = i[1],
		error_y=dict(type='data', array=err[2], visible=True)))

plot_div = plotly.offline.plot({
	"data": scatter,
	"layout" : Layout(title="probability of success for single path",
		width=900, height=500,
		legend=dict(x=0.78))
	},
	filename=output,
	auto_open=False,
	show_link=False)
