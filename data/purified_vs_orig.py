#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Scatter


output = "plots/purified_vs_orig.html"

plot_data = []

files = [["distance_3", 'distance 3'],
	["distance_6", 'distance 6'],
	["distance_9", 'distance 9'],
	["distance_12", 'distance 12']]

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
	"layout" : Layout(title="error rate after purification",
		width=700, height=500,
		xaxis=dict(type="log", title="input error rate"),
		yaxis=dict(type="log",range=[-2.5,0], title="output error rate"),
		legend=dict(x=0.02))
	},
	filename=output,
	auto_open=False,
	show_link=False)
	
