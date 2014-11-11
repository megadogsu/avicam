from lib.linuxkb import *
import struct, Image, time, serial, sys
from xbee import XBee
import atexit



ser = serial.Serial('/dev/ttyUSB0', 115200)

def getImg():
	jpg_file = open("../tmp/test.jpg", 'w')
	ser.write('G')
	print(ser.readline()[:-1])
	byte = ser.read(4)
	print(''.join('%02x'%ord(i) for i in byte))
	size = struct.unpack("I",byte)[0]
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
			print("Bad",echo, data,i)
			continue
		else:
			ser.write(terminal)
			print("Received",i)
			jpg_file.write(data)
			i += buffsize
	
	print(ser.readline()[:-1])

	jpg_file.close();
	Image.open('../tmp/test.jpg').show()

def startVideo():
	jpg_file = open("../tmp/test.jpg", 'w')
	ser.write('S')
	print(ser.readline()[:-1])
	byte = ser.read(4)
	print(''.join('%02x'%ord(i) for i in byte))
	size = struct.unpack("I",byte)[0]
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
			print("Bad",echo, data,i)
			continue
		else:
			ser.write(terminal)
			print("Received",i)
			jpg_file.write(data)
			i += buffsize
	
	print(ser.readline()[:-1])

	jpg_file.close();
	Image.open('../tmp/test.jpg').show()

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
#			print(ser.readline()[:-1])
#			print(ser.readline()[:-1])
#			print(ser.readline()[:-1])
#			print(ser.readline()[:-1])
#			print(ser.readline()[:-1])
#			print(ser.readline()[:-1])
#			hexstring = ser.readline()[:-1]
#			hexstring = hexstring[len(hexstring)%2:]
#			hex_data = hexstring.decode("hex")
#			print(repr(hex_data))
#			with open("hex1.txt", "wb") as f:
#				f.write(hex_data)
		elif key in ('g', 'get'):
			print("Start to receive picture")
			getImg()
		elif key in ('s', 'setting'):
			ser.write('S')
			sys.stdin.read(1)
			print(ser.readline()[:-1])
			ser.write(sys.stdin.readline());
		elif key in ('o', 'open'):
			ser.write('O')
		elif key in ('c', 'close'):
			ser.write('C')
		elif key in ('z', 'size'):
			ser.write('Z')
			print(ser.readline())
		elif key in ('w', 'write'):
			ser.write('W')
			print(ser.readline())
		elif key in ('r', 'remove'):
			ser.write('R')
			sys.stdin.read(1)
			ser.write(sys.stdin.readline());
		elif key in ('l', 'list'):
			ser.write('L')
		elif key in ('m', 'memory'):
			ser.write('M')
		elif key in ('e', 'erase'):
			ser.write('E')
		elif key in ('q', 'quit'):
			break
#	echo = ser.read(ser.inWaiting())
#	if echo:
#		print(echo)
	bytesToRead = ser.inWaiting()
	mystring = ser.read(bytesToRead)
	sys.stdout.write(mystring)
	sys.stdout.flush()
