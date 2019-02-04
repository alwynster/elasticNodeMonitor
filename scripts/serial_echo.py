#!/usr/bin/env python

import time
import serial
import sys

baud = sys.argv[2]
if baud.endswith('UL'): baud = baud[:-2]
baud = int(baud) 

# print sys.argv

ser = serial.Serial(sys.argv[1], baud)

quit = False

ser.write("W")
while ser.in_waiting > 0:
	# print ser.in_waiting
	print ser.readline(), # bt = ser.read(1)[0]		while True:

while not quit:
	try:
		ser.write("a")
		ser.read(1) # echo
		# while ser.in_waiting > 0:
			# print ser.in_waiting
		print ser.readline(), # bt = ser.read(1)[0]		while True:



		# time.sleep(.2)
			# print bt
			# print format(ord(bt), "02x")
		# print bt
	except serial.serialutil.SerialException:
		print 'serial port lost'
		time.sleep(.25)
		print 'reopening'
		try:
			ser = serial.Serial(sys.argv[1], baud)
		except:
			continue
	except IOError:
		print "IOError"
		continue

ser.close
quit = True
print 'done'
