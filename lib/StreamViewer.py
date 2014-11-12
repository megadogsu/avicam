from Tkinter import *

class StreamViewer(Frame):
     def __init__(self, root):
        root.title("Test Application")
        self.addFrame(root)
        
     def addFrame(self,root):
          frame = Frame(root,  background="#FFFFFF")
          self.addCanvas(frame)
          frame.pack(fill=BOTH, expand=YES)
        
     def addCanvas(self, frame):
        self.canvas = Canvas(frame)
        self.canvas.pack(fill=BOTH, expand=YES)
        self.canvas.pack()
        
     def addImage(self, photoimage):
          self.canvas.create_image(320,180, image=photoimage, anchor=SE)

