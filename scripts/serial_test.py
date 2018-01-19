#!/usr/bin/env python
import os
import serial
import sys
import time
import glob
import time 
import string 
import numpy as np
import warnings 
import threading

# from serial_plotter import SerialPlotter

import sched

PLOT_DELAY = 0.5

TIME_TOTAL = 1
baudrate = 500000
CURRENT_MEASUREMENT_FLOAT = 4*6
CURRENT_BUFFER_SIZE = 20

serial_template = "/dev/tty.usbserial-AL34*"

# plot = False

class SerialTest:
	ser = None
	monitoring = False
	# plotter = None
	monitoringThread = None
	debug = True
	# selectmap = /False
	busy = False # dont request current because interface in use

	baud, port = None, None
	def __init__(this):

		try:
			# if plot:
			# 	this.plotter = SerialPlotter()

			try:
				this.baud = int(sys.argv[2]) 
				this.port = sys.argv[1]
			except Exception:
				print 'finding serial...'
				this.baud = baudrate
				ports = glob.glob(serial_template)
				# ports = glob.glob("/dev/tty.usbserial-A51ZP5TJ")

				if len(ports) == 0:
					print "no serial ports..."
					sys.exit(0)
				elif len(ports) == 1:
					this.port = ports[0]
				else:
					print "too many serial ports..."
					print ports
					sys.exit(0)

				this.bitFile = "bit_file.bit"
				this.testFile = "bit_file_test.bit"

			this.openSerial();
		except KeyboardInterrupt:
			print 'Interrupted initialisation'
			sys.exit(0)

	def openSerial(this):
		try:
			print 'Opening serial...'
			this.ser = serial.Serial(this.port, this.baud)
			this.remainingMonitor = 0;

			# this.ser.read(this.ser.inWaiting())
			this.ser.reset_input_buffer();

			response = '0'
			this.ser.timeout = 0.5#  * 10
			request = '0'
			
			# read any old data
			this.ser.read(1000) # this will always cause a timeout delay


			this.ser.timeout = 2.5#  * 10
			
			this.ser.reset_input_buffer();
			this.ser.timeout = None

			print 'Serial available'
		except KeyboardInterrupt:
			print 'Interrupted initialisation'
			sys.exit(0)

	def receiveData(this):
		t0 = time.time()
		try:
			print 'receiving for %d s' % TIME_TOTAL
			while (time.time() - t0 < TIME_TOTAL or this.remainingMonitor > 0):
			    if (this.ser.in_waiting > 0): #if incoming bytes are waiting to be read from the serial input buffer
			        data_in = this.ser.read(this.ser.inWaiting())
			        data_str = data_in.decode('ascii') #read the bytes and convert from binary array to ASCII
			        sys.stdout.write(data_str), #print the incoming string without putting a new-line ('\n') automatically after every print()
			        sys.stdout.flush()

			        # if data_str[:2] == "#M":
			        # 	this.remainingMonitor -= 1
			        # 	print 'Remaining:', this.remainingMonitor

			        t0 = time.time()
		    #Put the rest of your code you want here
		except UnicodeDecodeError:
			print data_in
		finally:

			print 'done'

	def startMonitor(this):
		# send command to start monitoring
		this.writeCommand("M")

		this.monitoring = True

		threading.Thread(target=this.waitWhileMonitoring, args=[]).start()		
		# this.remainingMonitor = length


	def endMonitor(this):
		# send command to start monitoring
		this.writeCommand(b'm')

		# flush out the last results
		# this.printAllMeasurements();

		# print "Values:",
		# print this.ser.readline()
		# this.remainingMonitor = 0
		this.monitoring = False

	def writeCommand(this, command):
		# backup current busy state
		bk = this.busy
		this.busy = True
		#  this.ser.reset_input_buffer();
		# print 'sending command', command
		for i in range(5):
			this.ser.timeout = 0.1
			this.ser.write(command)
			if this.confirmCommand(command):
				break
			else:
				print 'repeating command...', command

		this.ser.timeout = None

		this.busy = bk
		# print 'command successful'

	def confirmCommand(this, command):
		this.ser.timeout = 0.5
		response = this.ser.read(1)
		try:
			command = ord(command)
		except TypeError:
			pass
		try:
			response = ord(response)
		except TypeError:
			pass

		if response != command:
			if response == '':
				print 'No response...'
			else:
				print "Received incorrect ack! Command", command, "response '", response, "'"
			if response == 12:
				print 'RESET DETECTED'
				this.ser.close()
				sys.exit(1)
				# time.sleep(10)
			return False

		return True
			# sys.exit(0)

	def printCurrent(this):
		this.writeCommand(b'C')

	def fetchCurrent(this):
		# raise NotImplementedError("not implemented")
		this.writeCommand(b'c')
		
		# fetch results
		print 'fetching'
		# print 'TODO: not implemented'
		# for i in range(50):
		floats = this.ser.read(CURRENT_MEASUREMENT_FLOAT)
		print 'done' 

		return np.frombuffer(floats, dtype=np.float32)

	# fetch as many as are presented
	def fetchNumCurrents(this, number):
		
		this.ser.timeout = 1.0

		results = list()
		# fetch results
		for i in range(number):
			floats = this.ser.read(CURRENT_MEASUREMENT_FLOAT)
			results.append(np.frombuffer(floats, dtype=np.float32))

		return results

	# fetch all available
	def fetchAllCurrent(this):
		this.writeCommand(b'f')
		
		rec = this.ser.read(1)
		numResults = ord(rec)
		# if numResults > 0:
		print 'numResults:', numResults
		if numResults > CURRENT_BUFFER_SIZE:
			print 'unrealistic number of results...'
			this.ser.timeout = 0.5
			this.ser.read(numResults);
			return list()
		else:
			return this.fetchNumCurrents(numResults)

	def printMeasurement(this, measurement):
		print 'printing measurement'
		print measurement

	def printAllMeasurements(this):
		results = this.fetchAllCurrent()
		for res in results:
			print res

	def waitWhileMonitoring(this):

		print 'ready to receive measurements...'

		this.ser.timeout = 0.5
		if this.monitoring:
			try:
				time.sleep(1)
				while this.monitoring:
					if not this.busy:
						this.plotAll()
					time.sleep(1)

			except KeyboardInterrupt:
				print 'done'
				this.ser.timeout = 0.5

				# try:
				# 	this.endMonitor()
				# except:
				# 	print 'could not end monitoring'

				serialTest.ser.close();	
				sys.exit(0)
			except serial.SerialException:
				# probably means port has been closed
				this.monitoring = False
			

	def plotAll(this):
		# try:

		# this.printCurrent();

		results = this.fetchAllCurrent()
		
		for res in results:
			if plot:
				with warnings.catch_warnings():
					warnings.simplefilter("default")

					this.plotter.plot(res)
			else:
				print ['usb', 'monitor', 'wireless', 'mcu', 'vccaux', 'vccint']
				print res

		# means there's nothing to ord
		# except TypeError:
		# 	pass
		# except KeyboardInterrupt:
		# 	this.ser.timeout = 0.5

		# 	try:
		# 		this.endMonitor()
		# 	except:
		# 		print 'could not end monitoring'

		# 	sys.exit(0)

	def redraw(this):
		if plot:
			this.plotter.redraw()


if __name__ == '__main__':


	serialTest = SerialTest()
	# serialTest.startMonitor()
	# time.sleep(2)
	# serialTest.endMonitor()
	flash = True
	selectmap = False
	ADDRESS = 0


	if len(sys.argv) > 1:

		targets = sys.argv[1:]

		if len(targets) > 2:
			serialTest.port = targets[0]
			serialTest.baud = targets[1]
			if serialTest.baud.endswith("UL"):
				serialTest.baud = int(serialTest.baud[:-2])

			serialTest.openSerial();

			targets = targets[2:]

		# probably coming from makefile
		if len(targets) == 1:
			targets = targets[0].split(',')

		for i in range(len(targets)):
			target = targets[i].lower()

			print "Starting target:", target

			if target == "current":
				serialTest.printCurrent() #serialTest.fetchCurrent());
			elif target == "fetchcurrent":
				serialTest.printAllMeasurements();
			elif target == "startmonitor":
				serialTest.startMonitor()
			elif target == "endmonitor":
				serialTest.endMonitor()
			elif target == "wait":
				serialTest.busy = True
				
				time.sleep(1) 

				serialTest.busy = False

			elif target == "findcurrent":
				serialTest.findCurrentSensors()

			else:
				print "Target", target, "not defined"
	

	while serialTest.monitoring:
		try:
			time.sleep(0.1)
			serialTest.redraw()
		except ValueError:
			print 'No data to plot'
		except KeyboardInterrupt:
			serialTest.monitoring = False
			# serialTest.endMonitor()

	serialTest.ser.close();

