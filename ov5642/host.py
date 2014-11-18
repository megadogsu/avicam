from lib.linuxkb import *
from StringIO import StringIO
from xbee import XBee
from lib.StreamViewer import StreamViewer 
import struct, time, serial, sys
import atexit
import Tkinter 
import Image, ImageTk
import thread

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

def _resize_image(self,event):
    new_width = event.width
    new_height = event.height
    self.image = self.img_copy.resize((new_width, new_height))
    self.background_image = ImageTk.PhotoImage(self.image)
    self.background.configure(image = self.background_image)


def startVideo():	 
	data = ""
	print(ser.readline()[:-1])
	window = Tkinter.Tk()  
	window.geometry("640x360")
	tbk = StreamViewer(window)
	#thread.start_new_thread(updateWindow, ()

	while True:
		if kbhit():
			key = getch()
			if key in ('q', 'quit'):
				ser.write('Q')
				print("I write a QQQQQQ")
				print(ser.readline()[-3:-1])
				window.destroy()
				break
		bytesToRead = ser.inWaiting()
		buff = ser.read(bytesToRead)
		data += buff
		a = data.find('\xff\xd8')
		b = data.find('\xff\xd9')
		if a != -1 and b != -1:
			print("Image Received")
			jpg_data = data[a:b+2]
			data= data[b+2:]
			try:
				img = Image.open(StringIO(jpg_data))
				img = img.resize((window.winfo_width(), window.winfo_height()), Image.BICUBIC)  
				tkimage = ImageTk.PhotoImage(img)
				tbk.addImage(tkimage)
				window.update()
			except IOError:
				print("Bad Packet")
				continue
	
			#Tkinter.Label(window, image=tkimage).pack()


bytesToRead = ser.inWaiting()
mystring = ser.read(bytesToRead)
sys.stdout.write(mystring)
sys.stdout.flush()
print("press 't' to trigger the shot, 'z' to see its size, 'g' to get it back!, 'b' for full resolution image through RF")

while True:
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
		elif key in ('v', 'video'):
			ser.write('V')
			startVideo()
		elif key in ('q', 'quit'):
			break
#	echo = ser.read(ser.inWaiting())
#	if echo:
#		print(echo)
	bytesToRead = ser.inWaiting()
	mystring = ser.read(bytesToRead)
	sys.stdout.write(mystring)
	sys.stdout.flush()
