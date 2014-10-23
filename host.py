from lib.linuxkb import *
import struct, Image, time, serial, sys
from xbee import XBee
import atexit


def terminate():
	os.system("./capture.py stop")


def captureInit():
	os.system("./capture.py start")
	atexit.register(terminate)

ser = serial.Serial('/dev/ttyUSB0', 19200)

def getImg():
	jpg_file = open("tmp/test.jpg", 'w')
	ser.write('G')
	byte = ser.read(4)
	#print(''.join('%02x'%ord(i) for i in byte))
	size = struct.unpack("I",byte)[0]
	print("File size is:", size)
	
	i = 0
	terminal = '\n'
	resend = 'R'
	buffsize = 256
	
	while i < size:
		if i > size-buffsize:
			data = ser.read(size % buffsize)
		else:
			data = ser.read(buffsize)
		
		echo = ser.read()
		if echo != terminal:
			ser.write(resend)
			print("Bad",echo, data,i)
			continue
		else:
			ser.write(terminal)
			print("Received",i)
			jpg_file.write(data)
			i += buffsize
	
	print(ser.readline()[:-1])

	jpg_file.close();
	Image.open('tmp/test.jpg').show()

captureInit()

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
		elif key in ('g', 'get'):
			print("Start to receive picture")
			getImg()
		elif key in ('b', 'big'):
			ser.write('B')
			print("Start to receive big image!")
		elif key in ('z', 'size'):
			ser.write('Z')
			print(ser.readline())
		elif key in ('w', 'write'):
			ser.write('W')
			print(ser.readline())
		elif key in ('r', 'read'):
			ser.write('R')
			print(ser.readline())
		elif key in ('q', 'quit'):
			break
	bytesToRead = ser.inWaiting()
	mystring = ser.read(bytesToRead)
	sys.stdout.write(mystring)
	sys.stdout.flush()
