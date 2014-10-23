#!/usr/bin/python
from SimpleCV import *
import struct, Image, time, serial, sys
from subprocess import call
from lib import runner


def toSerialInt(integer):
	high, low = divmod(integer, 0x100)
	return chr(high), chr(low)

def capture():
	global cam
	global ser
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

	i = 0
	size = len(capture)
	terminal = '\n'
	buffsize = 50
	while i < size:
		ser.write(capture[i:i+buffsize])
		ser.write(terminal)
		echo = ser.read(1)
		if echo == terminal:
			print("Received from %d, to %d" % ( i, i+buffsize-1) )
			i += buffsize
		else:
			print("Bad", echo, i)


	print(ser.readline()[:-1])

class Capture():
    def __init__(self):
        self.stdin_path = None
        self.stdout_path = None
        self.stderr_path = None
        self.pidfile_path =  "/home/chinposu/eecs473/avicam/tmp/capturepid.tmp"
        self.pidfile_timeout = 5
    def run(self):
		global ser
		global cam
		ser = serial.Serial('/dev/ttyACM0', 19200, timeout=None)
		cam = Camera(prop_set={"width":1280,"height":720}) 
		
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

app = Capture()
daemon_runner = runner.DaemonRunner(app)
daemon_runner.do_action()

