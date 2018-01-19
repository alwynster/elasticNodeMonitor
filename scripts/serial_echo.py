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

while not quit:
	try:
		while True:
			ser.write("M")
			while ser.in_waiting > 0:
				# print ser.in_waiting
				print ser.readline(), # bt = ser.read(1)[0]
			time.sleep(2)
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
