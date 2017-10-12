#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Scatter


output = "plots/distance_dependency.html"

scatter = []

files = [["distance_data/combined_15", 'error rate 15%'],
	["distance_data/combined_20", 'error rate 20%'],
	["distance_data/combined_25", 'error rate 25%'],
	["distance_data/combined_30", 'error rate 30%'],
	["distance_data/combined_35", 'error rate 35%'],
]


#add improvement threshold
scatter.append(Scatter(x = [6, 40], y = [0.145,0.145], name = "Threshold for<br>Raussendorf lattice", marker = dict(color = 'black'), fillcolor='rgba(255,255,0,0.2)', fill="tozeroy"))



for i in files:
	try:
		data = pd.read_csv(i[0], sep=" ",header=None)
	except:
		print("could not load: " + i)
		exit(1)


	mean = data.groupby(0,as_index=False).mean()

	err = data.groupby(0,as_index=False).sem()

	scatter.append(Scatter(x=mean[0], y=mean[2], name = i[1],
		error_y=dict(type='data', array=err[2], visible=True)))

plot_div = plotly.offline.plot({
	"data": scatter,
	"layout" : Layout(#title="probability of success for single path",
		width=600, height=500,
		xaxis=dict(type="lin", range=[8,44], showline=True, title="box size"),
		yaxis=dict(type="lin", range=[0,1.1], showline=True, title="output error rate"),
		legend=dict(x=0.78))
	},
	filename=output,
	auto_open=False,
	show_link=False)
