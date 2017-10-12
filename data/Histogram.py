#!/bin/python3
import pandas as pd
import sys

import plotly
from plotly.graph_objs import Layout, Histogram


output = "plots/histogram.html"

hist = []

files = [["histogram_data/distribution_16", "distance 16"],
		["histogram_data/distribution_20", "distance 20"],
		["histogram_data/distribution_24", "distance 24"],
]


for i in files:
	try:
		data = pd.read_csv(i[0], sep=" ",header=None)
	except:
		print(i[0] + "does not exist")
		exit(1)
	

	hist.append(Histogram(x=data[0],opacity=.5,name=i[1],histnorm='probability'))


plot_div = plotly.offline.plot({
	"data": hist,
	"layout" : Layout(
		barmode='overlay',
		width=600, height=500,
		xaxis=dict(range=[5,60], showline=True, dtick = 5,title="path length"),
		yaxis=dict(title="probability", showline=True),
		legend=dict(x=0.75,y=.95))
	},
	filename=output,
	auto_open=False,
	show_link=False)
