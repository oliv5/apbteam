import sys
sys.path.append (sys.path[0] + '/..')

from drawable import *
from math import pi

class Test (Drawable):

    def draw (self):
	self.draw_rectangle ((0, 0), (5, 5), fill = 'red')
	self.draw_rectangle ((20, 20), (50, 50), fill = '', outline = 'blue')
	self.draw_line ((20, 20), (25, 25), (80, 0), (0, 80), fill = 'green')
	self.draw_line ((20, 20), (25, 25), (80, 0), (0, 80), smooth = True)
	self.draw_circle ((40, -40), 10)
	self.draw_arc ((-40, 0), 20, start = pi / 4, extent = pi / 2)

class App (DrawableCanvas):

    def __init__ (self, master = None):
	DrawableCanvas.__init__ (self, master)
	self.pack (expand = True, fill = 'both')
	self.test = Test (self)
	self.bind ('<Configure>', self.resize)
	self.animated = False
	self.i = 0

    def animate (self):
	self.after (500, self.animate)
	self.test.trans_rotate (-pi/12)
	self.test.trans_translate ((10, 10))
	self.test.trans_scale (1.05)
	self.test.trans_translate ((self.w / 2, self.h / 2))
	self.test.draw ()
	self.test.trans_translate ((-self.w / 2, -self.h / 2))
	self.i += 1
	if self.i == 10:
	    self.test.reset ()

    def resize (self, ev):
	self.w, self.h = ev.width, ev.height
	self.test.reset ()
	if not self.animated:
	    self.animate ()
	    self.animated = True

app = App ()
app.mainloop ()
