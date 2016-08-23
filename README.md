# eng-controls-acc-vs-lrf-test-rig

Repo for the laser-rangefinder + accelerometer test rig. 

The code in /LRF should go on the Teensy connected to the laser rangefinder, and code in /accelerometer goes on the the accelerometer Teensy.

/data_plot_and_read contains a Go program to read data from both sensors and synchronize it if they send data at diffrent rates (which they always seem to do even if Teensy code suggests otherwise..). It outputs a .csv file with the synchronized data that can be plotted with rcharts.py.

To run the Go program you first need to install Go:
https://golang.org/dl/

Then do "go get github.com/tarm/goserial" in a terminal, and compile by doing "go build" in the /data_plot_and_read directory.

To run rcharts you need Python:
https://www.python.org/downloads/

And plotly:
https://plot.ly/python/getting-started/

Then do "python rcharts.py [name of file to plot]" in a terminal. It should open a webbrowser with your plotted data.
