from lib.linuxkb import *
import struct, Image, time, serial, sys
from xbee import XBee

ser = serial.Serial('/dev/ttyUSB0', 19200)

def getImg():
	jpg_file = open("etc/tmp.jpg", 'w')
	ser.write('G')
	size = struct.unpack("h",ser.read(2))[0]
	print("File size is:", size)
	
	i = 0
	terminal = '\n'
	resend = 'R'
	buffsize = 512
	
	while i < size:
		if i > size-buffsize:
			data = ser.read(size % buffsize)
		else:
			data = ser.read(buffsize)
		
		echo = ser.read()
		if echo != terminal:
			ser.write(resend)
			continue
		else:
			ser.write(terminal)
			print("Received",i)
			jpg_file.write(data)
			i += buffsize
	
	print(ser.readline())

	jpg_file.close();
	Image.open('etc/tmp.jpg').show()



bytesToRead = ser.inWaiting()
mystring = ser.read(bytesToRead)
sys.stdout.write(mystring)
sys.stdout.flush()
print("press 't' to trigger the shot, 'z' to see its size, 'g' to get it back!, 'b' for full resolution image through RF")

while 1:
	# Poll keyboard 
	if kbhit():
		key = getch()
		if key in ('t', 'capture'):
			ser.write('T')
			print(ser.readline())
		if key in ('g', 'get'):
			print("Start to receive picture")
			getImg()
		if key in ('b', 'big'):
			ser.write('B')
			print("Start to receive big image!")
		if key in ('z', 'size'):
			ser.write('Z')
			print(ser.readline())
		if key in ('q', 'quit'):
			break
	bytesToRead = ser.inWaiting()
	mystring = ser.read(bytesToRead)
	sys.stdout.write(mystring)
	sys.stdout.flush()
