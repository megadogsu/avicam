#!/usr/bin/python
from SimpleCV import *
import struct, Image, time, serial, sys
from subprocess import call
from lib import runner


def toSerialInt(integer):
	mybytes = []
	for i in range(0,4):
		integer, byte = divmod(integer, 0x100)
		mybytes.insert(0,byte)
	return mybytes

def capture():
	global cam
	global ser
	img = cam.getImage()
	#img = img.scale(32,18)
	#d = img.show()
	img.save("tmp/capture.jpg",'JPEG',quality=60) 
	call(["image_optim", "tmp/capture.jpg"])

	size = os.path.getsize("tmp/capture.jpg")
	print("File size:", size, "bytes")
	byte = struct.pack("I",size)
	ser.write(byte)
	print(ser.readline()[:-1])
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
		ser = serial.Serial('/dev/ttyACM0', 57600, timeout=None)
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

