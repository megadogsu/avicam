#!/usr/bin/python
from SimpleCV import *
import struct, Image, time, serial, sys
from subprocess import call
from daemon import runner

ser = serial.Serial('/dev/ttyACM0', 19200, timeout=None)

cam = Camera(prop_set={"width":1280,"height":720}) 

def toSerialInt(integer):
	high, low = divmod(integer, 0x100)
	return chr(high), chr(low)

def capture():
#	global cam
#	global ser
	print(456)
	img = cam.getImage()
	img = img.scale(320,180)
	#d = img.show()
	img.save("tmp/capture.jpg",'JPEG',quality=60) 
	call(["image_optim", "tmp/capture.jpg"])

	size = os.path.getsize("tmp/capture.jpg")
	print("File size:", size, "bytes")
	high, low = toSerialInt(size)
	ser.write(high)
	ser.write(low)
	print(high, low)
	print(ord(high), ord(low))

	capture = open("tmp/capture.jpg","rb").read()
	i = -1
	terminal = '\n'
	buffsize = 63
	while i <= len(capture):
		i += 1
		if i == len(capture):
			print("At end", i)
			ser.write(terminal)
			echo = ser.read(1)
			if echo != terminal:
				i-=(i%8)
				print("Bad", echo, i)
				continue;
			else:
				print("Finish")
				break;
		if(i < 0) or (i >= len(capture)):
			print(i)
		ser.write(capture[i])
		if i % buffsize == (buffsize - 1):
			ser.write(terminal)
			echo = ser.read(1)
			#print(ser.readline())
			if echo == terminal:
				print("Received", i)
				#time.sleep(1)
				continue
			else:
				print("Bad", echo, i)
				i-=8
				continue
	print(ser.readline())

#class App():
#    def __init__(self):
#        self.stdin_path = '/dev/null'
#        self.stdout_path = '/dev/tty'
#        self.stderr_path = '/dev/tty'
#        self.pidfile_path =  '/tmp/capture.pid'
#        self.pidfile_timeout = 5
#    def run(self):

#global ser
#global cam

#cam = camera(prop_set={"width":12,"height":8}) 
#cam = camera() 
bytesToRead = ser.inWaiting()
mystring = ser.read(bytesToRead)
sys.stdout.write(mystring)
sys.stdout.flush()

while True:
	key = ser.read()
	print(repr(key))
	if(key == 'C'):
		go = ser.read()
		if go == '#':
			capture()
		elif go == '!':
			print("File Error")
		else:
			print("Connection Error")

#app = App()
#daemon_runner = runner.DaemonRunner(app)
#daemon_runner.do_action()


