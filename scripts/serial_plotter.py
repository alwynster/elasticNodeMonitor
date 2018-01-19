from matplotlib import pyplot as pp
import time
import warnings 
import numpy as np

LIMIT = 50

class SerialPlotter:
	graphdata = [list() for i in range(6)]
	xs = list()
	latestX = 0
	updating = False
	title = "Elastic Node"

	styles = ['r', 'b', 'g', 'm', 'k', 'c']

	hide_wireless = False

	def __init__(this):
		pp.figure()
		this.drawExtras()

	def drawExtras(this):
		pp.ion()
		pp.grid()
		pp.xlabel('Time (in s)')
		pp.ylabel('Current consumption (in mA)')
		# pp.ylabel('Power consumption (in mW)')
		pp.title(this.title)
		
	def plot(this, datapoint):
		try:
			this.updating = True

			if len(datapoint) != 6:
				warnings.warn('datapoint incorrect format')
				return False

			for i in range(6):
				this.graphdata[i].append(datapoint[i])
				if len(this.graphdata[i]) > LIMIT:
					this.graphdata[i] = this.graphdata[i][-LIMIT:]

			this.xs.append(this.latestX)
			if len(this.xs) > LIMIT:
				# print len(this.xs)
				this.xs = this.xs[-LIMIT:]
			# 	print len(this.xs)
			this.latestX += 1

			this.updating = False
			return True

		except KeyboardInterrupt:
			this.updating = False
			return False

		except ValueError:
			print 'Value error in plot'
			return False

	def redraw(this, timeout=0.05):

		if this.updating: return

		pp.cla()
		this.drawExtras()

		for i in range(6):
			if not (this.hide_wireless and i == 1):
				pp.plot(this.xs, this.graphdata[i], this.styles[i])

		minx = np.min(this.xs)
		maxx = np.max(this.xs)
		if minx != maxx:
			pp.xlim([minx, maxx])

		if this.hide_wireless:
			pp.legend(['Total', 'MCU', 'FPGA'], loc='center right')
		else:
			pp.legend(['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA AUX', 'FPGA INT'], loc='center right')

		pp.pause(timeout)

if __name__ == "__main__":
	print 'test SerialPlotter'
	plotter = SerialPlotter()

	datapoint = [0,0,0,0]
	for i in range(20):
		datapoint[0] = i
		plotter.plot(datapoint)
		plotter.redraw()

	time.sleep(1)