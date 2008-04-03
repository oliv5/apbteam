from Tkinter import *


class Table (Canvas):

    WIDTH = 3000 + 2 * 22 + 2 * 80 + 2 * 250 + 2 * 10
    HEIGHT = 2100 + 2 * 22 + 2 * 80 + 2 * 10
    XOFFSET = 250 + 80 + 22 + 10
    YOFFSET = 80 + 22 + 2100 + 10

    def __init__ (self, master):
	Canvas.__init__ (self, master, borderwidth = 1, relief = 'sunken', background = 'white')
	self.bind ('<Configure>', self.resize)

    def resize (self, ev):
	# Compute new scale.
	w, h = float (ev.width), float (ev.height)
	self.__scale = min (w / self.WIDTH, h / self.HEIGHT)
	self.__xoffset = (w / self.__scale - (self.WIDTH - self.XOFFSET * 2)) / 2 * self.__scale
	self.__yoffset = (h / self.__scale - (self.HEIGHT - self.YOFFSET * 2)) / 2 * self.__scale
	#print '%dx%d %f+%d+%d' % (ev.width, ev.height, self.__scale, self.__xoffset, self.__yoffset)
	# Redraw.
	self.delete (*self.find_all ())
	self.draw_rectangle ((-22, -22 - 80), (3000 / 2, 2100 + 22), fill = '#ff1f1f')
	self.draw_rectangle ((3000 / 2, -22 - 80), (3000 + 22, 2100 + 22), fill = '#201fff')
	self.draw_rectangle ((0, 0), (3000, 2100), fill = '#a49d8b')
	self.draw_rectangle ((0, -22 - 80), (3000, -22), fill = '#a49d8b')
	self.draw_rectangle ((0, 2100 - 500), (500, 2100), fill = '#201fff')
	self.draw_rectangle ((3000 - 500, 2100 - 500), (3000, 2100), fill = '#ff1f1f')
	self.draw_line ((3000 / 2, -22 - 80), (3000 / 2, 2100 + 22))
	# Beacons and baskets.
	self.draw_rectangle ((-22, 2100), (-22 - 80, 2100 + 80), fill = '#5b5b5d')
	self.draw_rectangle ((-22, 1050 - 40), (-22 - 80, 1050 + 40), fill = '#5b5b5d')
	self.draw_rectangle ((-22, 500), (-22 - 80, 500 + 80), fill = '#5b5b5d')
	self.draw_rectangle ((-22, -80), (-22 - 80, 0), fill = '#5b5b5d')
	self.draw_rectangle ((-22, 0), (-22 - 80, 500), fill = '#5b5b5d')
	self.draw_rectangle ((-22 - 80 - 250, 0), (-22 - 80, 500), fill = '#6d6dad', stipple = 'gray75')
	self.draw_rectangle ((3000 + 22, 2100), (3000 + 22 + 80, 2100 + 80), fill = '#5b5b5d')
	self.draw_rectangle ((3000 + 22, 1050 - 40), (3000 + 22 + 80, 1050 + 40), fill = '#5b5b5d')
	self.draw_rectangle ((3000 + 22, 500), (3000 + 22 + 80, 500 + 80), fill = '#5b5b5d')
	self.draw_rectangle ((3000 + 22, -80), (3000 + 22 + 80, 0), fill = '#5b5b5d')
	self.draw_rectangle ((3000 + 22, 0), (3000 + 22 + 80, 500), fill = '#5b5b5d')
	self.draw_rectangle ((3000 + 22 + 80 + 250, 0), (3000 + 22 + 80, 500), fill = '#6d6dad', stipple = 'gray75')
	# Vertical dispensers.
	self.draw_rectangle ((-22, 2100 - 750 - 85 / 2), (0, 2100 - 750 + 85 / 2), fill = '#5b5b5b')
	self.draw_oval ((0, 2100 - 750 - 80 / 2), (80, 2100 - 750 + 80 / 2))
	self.draw_rectangle ((750 - 85 / 2, 2100), (750 + 85 / 2, 2100 + 22), fill = '#5b5b5b')
	self.draw_oval ((750 - 80 / 2, 2100 - 80), (750 + 80 / 2, 2100))
	self.draw_rectangle ((3000 + 22, 2100 - 750 - 85 / 2), (3000, 2100 - 750 + 85 / 2), fill = '#5b5b5b')
	self.draw_oval ((3000, 2100 - 750 - 80 / 2), (3000 - 80, 2100 - 750 + 80 / 2))
	self.draw_rectangle ((3000 - 750 + 85 / 2, 2100), (3000 - 750 - 85 / 2, 2100 + 22), fill = '#5b5b5b')
	self.draw_oval ((3000 - 750 + 80 / 2, 2100 - 80), (3000 - 750 - 80 / 2, 2100))
	# Horizontal dispenser.
	self.draw_rectangle ((3000 / 2 - 924 / 2, 2100 + 22), (3000 / 2 + 924 / 2, 2100 + 22 + 80 + 22), fill = '#5b5b5b')
	self.draw_rectangle ((3000 / 2 - 924 / 2 + 22, 2100 + 22), (3000 / 2 + 924 / 2 - 22, 2100 + 22 + 80), fill = '#5b5b5b')
	self.draw_rectangle ((3000 / 2 - 880 / 2 - 35 - 60, 2100), (3000 / 2 - 880 / 2 - 35, 2100 + 44), fill = '#5b5b5b')
	self.draw_rectangle ((3000 / 2 + 880 / 2 + 35 + 60, 2100), (3000 / 2 + 880 / 2 + 35, 2100 + 44), fill = '#5b5b5b')
	# Balls.
	balls = [ (800, 200, 'rb'), (800, 400, 'RB'), (800, 600, 'ww'),
		(1300, 200, 'rb'), (1300, 400, 'rb'), (1300, 600, 'ww'),
		(520, 800, 'WW'), (750, 40, 'RB'), (40, 750, 'WW'),
		(450, 1120, 'ww'), (750, 1070, 'ww'), (1050, 1020, 'ww'),
		(1500 - 72 / 2, -22 - 80 / 2, 'BR'),
		(1500 - 72 / 2 - 1 * 73, -22 - 80 / 2, 'RB'),
		(1500 - 72 / 2 - 2 * 73, -22 - 80 / 2, 'BR'),
		(1500 - 72 / 2 - 3 * 73, -22 - 80 / 2, 'RB'),
		(1500 - 72 / 2 - 4 * 73, -22 - 80 / 2, 'BR'),
		(1500 - 72 / 2 - 5 * 73, -22 - 80 / 2, 'RB'),
		(1500, 1000, 'W'),
		]
	balls_config = { 'r': { 'outline': '#bf4141' }, 'R': { 'fill': '#bf4141' },
		'b': { 'outline': '#4241bf' }, 'B': { 'fill': '#4241bf' },
		'w': { 'outline': '#bfbfbf' }, 'W': { 'fill': '#bfbfbf' } }
	for b in balls:
	    self.draw_oval ((3000 - b[0] - 72 / 2, 2100 - b[1] - 72 / 2),
		    (3000 - b[0] + 72 / 2, 2100 - b[1] + 72 / 2),
		    **balls_config[b[2][0]])
	    if len (b[2]) > 1:
		self.draw_oval ((b[0] - 72 / 2, 2100 - b[1] - 72 / 2),
			(b[0] + 72 / 2, 2100 - b[1] + 72 / 2),
			**balls_config[b[2][1]])

    def draw_rectangle (self, p1, p2, **kw):
	p1p, p2p = self.coord (p1, p2)
	return self.create_rectangle (p1p, p2p, **kw)

    def draw_line (self, *p, **kw):
	pp = self.coord (*p)
	return self.create_line (*pp, **kw)

    def draw_oval (self, *p, **kw):
	pp = self.coord (*p)
	return self.create_oval (*pp, **kw)

    def coord (self, *args):
	return [ (i[0] * self.__scale + self.__xoffset,
	    -i[1] * self.__scale + self.__yoffset) for i in args ]

class Application (Frame):
    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = 1, fill = 'both')
        self.createWidgets ()

    def createWidgets (self):
	self.quitButton = Button (self, text = 'Quit', command = self.quit)
	self.quitButton.pack (side = 'right', anchor = 'n')
	self.table = Table (self)
	self.table.pack (expand = 1, fill = 'both')

app = Application()
app.mainloop()
