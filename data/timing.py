#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Scatter


output = "plots/timing.html"

scatter = []

files = [["timing_data/timing", ''],
]


for i in files:
	try:
		data = pd.read_csv(i[0], sep=" ",header=None)
	except:
		print("could not load: " + i)
		exit(1)


	mean = data.groupby(0,as_index=False).mean()

	err = data.groupby(0,as_index=False).sem()

	scatter.append(Scatter(x=mean[0], y=mean[1]/1000, name = i[1],
		error_y=dict(type='data', array=err[1]/1000, visible=True)))

plot_div = plotly.offline.plot({
	"data": scatter,
	"layout" : Layout(#title="probability of success for single path",
		width=600, height=500,
		xaxis=dict(type="log", showline=True, title="box size"),
		yaxis=dict(type="log", showline=True, title="runtime in s"),
		legend=dict(x=0.78))
	},
	filename=output,
	auto_open=False,
	show_link=False)
