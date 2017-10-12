#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Scatter


output = "plots/purified_vs_orig.html"

plot_data = []

files = [["purified_vs_orig_data/dist_12", 'box size 12'],
	["purified_vs_orig_data/dist_16", 'box size 16'],
	["purified_vs_orig_data/dist_20", 'box size 20'],
	["purified_vs_orig_data/dist_28", 'box size 28'],
	["purified_vs_orig_data/dist_36", 'box size 36'],
]

#add improvement threshold
plot_data.append(Scatter(x = [0,0.5], y = [0,0.5], name = "improvement<br>threshold", marker = dict(color = 'black'), fillcolor='rgba(255,255,0,0.2)', fill="tozeroy"))



for i in files:
	# prepare some data
	try:
		data = pd.read_csv(i[0], sep=" ",header=None)
	except:
		print(i[0] + "does not exist")
		exit(1)

	mean = data.groupby(1,as_index=False).mean()

	err = data.groupby(1,as_index=False).sem()

	plot_data.append(Scatter(x=mean[1], y=mean[2], name=i[1],
		error_y=dict(type='data', array=err[2], visible=True)))





plotly.offline.plot({
	"data": plot_data,
	"layout" : Layout(#title="error rate after purification",
		width=600, height=500,
		xaxis=dict(type="lin", range=[0,0.51], showline=True, dtick = 0.1, title="input error rate"),
		yaxis=dict(type="lin", range=[0,1.05], showline=True, title="output error rate"),
		legend=dict(x=0.06,y=1.05))
	},
	filename=output,
	auto_open=False,
	show_link=False)
	
