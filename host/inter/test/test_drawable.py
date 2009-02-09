import sys
sys.path.append (sys.path[0] + '/../..')

from inter.drawable import *
from math import pi

class Test (Drawable):

    def draw (self):
        self.draw_rectangle ((-100, -100), (100, 100), fill = '', outline = 'gray')
        self.draw_rectangle ((0, 0), (5, 5), fill = 'red')
        self.draw_rectangle ((20, 20), (50, 50), fill = '', outline = 'blue')
        self.draw_line ((20, 20), (25, 25), (80, 0), (0, 80), fill = 'green')
        self.draw_line ((20, 20), (25, 25), (80, 0), (0, 80), smooth = True)
        self.draw_circle ((40, -40), 10)
        self.draw_arc ((-40, 0), 20, start = pi / 4, extent = pi / 2)

class App (DrawableCanvas):

    def __init__ (self, master = None):
        DrawableCanvas.__init__ (self, 300, 300, 20, 20, master)
        self.pack (expand = True, fill = 'both')
        self.test = Test (self)
        self.animated = False
        self.i = 0

    def animate (self):
        # Real user should reset at each redraw.
        self.after (500, self.animate)
        self.test.draw ()
        self.test.trans_rotate (-pi/12)
        self.test.trans_translate ((10, 10))
        self.test.trans_scale (1.05)
        self.i += 1
        if self.i == 10:
            self.test.reset ()

    def draw (self):
        if not self.animated:
            self.animate ()
            self.animated = True

app = App ()
app.mainloop ()
