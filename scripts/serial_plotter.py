# from matplotlib import pyplot as pp
# import time
# import warnings 
# import numpy as np

# LIMIT = 500
# HIDE_TOTAL = True
# CURRENT_SENSE_F = 12.

# class SerialPlotter:
# 	graphdata = [list() for i in range(6)]
# 	xs = list()
# 	latestX = 0
# 	updating = False
# 	title = "Elastic Node"
# 	maxy = 0

# 	styles = ['r', 'b', 'g', 'm', 'k', 'c']

# 	hide_wireless = False

# 	def __init__(this):
# 		pp.figure()
# 		this.drawExtras()
# 		this.maxy = 0

# 	def drawExtras(this):
# 		pp.ion()
# 		pp.grid()
# 		pp.xlabel('Time (in s)')
# 		pp.ylabel('Current consumption (in mA)')
# 		# pp.ylabel('Power consumption (in mW)')
# 		pp.title(this.title)
		
# 	def plot(this, datapoint):
# 		try:
# 			this.updating = True

# 			if len(datapoint) != 6:
# 				warnings.warn('datapoint incorrect format')
# 				return False

# 			for i in range(6):
# 				this.graphdata[i].append(datapoint[i])
# 				if len(this.graphdata[i]) > LIMIT:
# 					this.graphdata[i] = this.graphdata[i][-LIMIT:]

# 			this.xs.append(this.latestX)
# 			if len(this.xs) > LIMIT:
# 				# print len(this.xs)
# 				this.xs = this.xs[-LIMIT:]
# 			# 	print len(this.xs)
# 			this.latestX += 1

# 			this.updating = False
# 			return True

# 		except KeyboardInterrupt:
# 			this.updating = False
# 			return False

# 		except ValueError:
# 			print 'Value error in plot'
# 			return False

# 	def redraw(this, timeout=0.05):

# 		if this.updating: return

# 		pp.cla()
# 		this.drawExtras()

# 		for i in range(6):
# 			if not (this.hide_wireless and i == 1):
# 				if not (HIDE_TOTAL and i == 0):
# 					pp.plot(np.array(this.xs) / CURRENT_SENSE_F, this.graphdata[i], this.styles[i])
# 					this.maxy = np.max([this.maxy, np.max(this.graphdata[i])])


# 		minx = np.min(this.xs) / CURRENT_SENSE_F
# 		maxx = np.max(this.xs) / CURRENT_SENSE_F
# 		if minx != maxx:
# 			pp.xlim([minx, maxx])
# 		if this.maxy != 0:
# 			pp.ylim([0, this.maxy * 1.1])

# 		if this.hide_wireless:
# 			pp.legend(['Total', 'MCU', 'FPGA'], loc='center right')
# 		else:
# 			legend = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA AUX', 'FPGA INT']
# 			if HIDE_TOTAL: legend = legend[1:]
# 			pp.legend(legend, loc='upper left')


# 		pp.pause(timeout)

# if __name__ == "__main__":
# 	print 'test SerialPlotter'
# 	plotter = SerialPlotter()

# 	datapoint = np.zeros((len(plotter.styles)))
# 	for i in range(20000):
# 		datapoint[0] = i
# 		plotter.plot(datapoint)
# 		plotter.redraw()

# 	time.sleep(1)