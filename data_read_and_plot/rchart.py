import plotly
from plotly.graph_objs import Scatter, Layout
import pandas as pd
import numpy as np
import sys

if len(sys.argv) == 1:
	print("please enter file to plot")
	sys.exit()

df = pd.read_csv(sys.argv[1],error_bad_lines=False)
df.head()

plotly.offline.plot({
    "data": [Scatter(y=df["X_acceleration"],name='Raw acceleration')
    		,Scatter(y=df["X_distance"],name='Acc distance')
    		,Scatter(y=df["X_velocity"],name='Acc velocity')
    		,Scatter(y=df["laser_vel"],name='Laser velocity')
    		,Scatter(y=df["laser_dist"],name='Laser distance')
    		,Scatter(y=df["Kalman_1_acc"],name='Kalman acc')
    		,Scatter(y=df["Kalman_X_vel"],name='Kalman vel')],
    "layout": Layout(title="Accelerometer")
})