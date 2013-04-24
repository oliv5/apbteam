# simu - Robot simulation. {{{
#
# Copyright (C) 2012 Nicolas Schodet
#
# APBTeam:
#        Web: http://apbteam.org/
#      Email: team AT apbteam DOT org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# }}}
"""Eurobot 2013 table."""
from simu.inter.drawable import Drawable
from math import pi

YELLOW = '#eaea00'
RED = '#ff0000'
BLUE = '#0000e9'
BLACK = '#1f1b1d'
WHITE = '#f5fef2'
TRANS = '#c0c2b5'
PINK = '#e7baca'
FLAME = '#dcf000'
FLAME_OFF = '#9aa800'

colors = { None: WHITE, False: BLUE, True: RED }

class Candle (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        if self.model.pos:
            self.draw_circle (self.model.pos, self.model.radius,
                    fill = colors[self.model.color])
            flame = FLAME if not self.model.state else FLAME_OFF
            self.draw_circle (self.model.pos, self.model.radius * 0.8,
                    fill = flame)
            Drawable.draw (self)

class Glass (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        if self.model.pos:
            self.draw_circle (self.model.pos, self.model.radius,
                    fill = BLACK, outline = TRANS)
            Drawable.draw (self)

class Plate (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        if self.model.pos:
            self.trans_translate (self.model.pos)
            self.trans_rotate (self.model.angle)
            w, h = self.model.dim
            w, h = w/2, h/2
            self.draw_rectangle ((-w, -h), (w, h), fill = PINK)
            self.draw_rectangle ((-w + 22, -h + 22), (w - 22, h - 22),
                    fill = PINK)
            for c in self.model.cherries:
                if c.pos:
                    self.draw_circle (c.pos, c.radius, fill = colors[c.color])
            Drawable.draw (self)

class Cherries (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        for c in self.model.cherries:
            if c.pos:
                self.draw_circle (c.pos, c.radius, fill = colors[c.color])

class Gift (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        if self.model.pos:
            self.trans_translate (self.model.pos)
            w, h = self.model.dim
            if not self.model.state:
                self.draw_rectangle ((-w/2, -11), (w/2, 80),
                        fill = colors[self.model.color])
                self.draw_rectangle ((-w/2, 80), (w/2, 91), fill = WHITE)
            else:
                self.draw_rectangle ((-w/2, -11), (w/2, 11),
                        fill = colors[self.model.color])

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        for e in self.model.candles:
            Candle (self, e)
        for e in self.model.glasses:
            Glass (self, e)
        for e in self.model.plates:
            Plate (self, e)
        Cherries (self, self.model.cherries)
        for e in self.model.gifts:
            Gift (self, e)

    def draw_both (self, primitive, *args, **kargs):
        """Draw a primitive on both sides."""
        primitive (*args, **kargs)
        primitive (*((3000 - x, y) for x, y in args), **kargs)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22), (3000 + 22, 0), fill = YELLOW)
        self.draw_rectangle ((0, 0), (3000, 2000), fill = YELLOW)
        self.draw_rectangle ((-22, 0), (0, 2000), fill = BLUE)
        self.draw_rectangle ((-22, 2000), (1500, 2000 + 22), fill = BLUE)
        self.draw_rectangle ((0, 0), (400, 2000), fill = BLUE)
        self.draw_rectangle ((3000 + 22, 0), (3000, 2000), fill = RED)
        self.draw_rectangle ((3000 + 22, 2000), (1500, 2000 + 22), fill = RED)
        self.draw_rectangle ((3000, 0), (3000 - 400, 2000), fill = RED)
        # Start zones.
        self.draw_both (self.draw_rectangle, (0, 0), (400, 100), fill = WHITE)
        self.draw_both (self.draw_rectangle, (0, 0), (390, 90), fill = WHITE)
        self.draw_both (self.draw_rectangle, (0, 400), (400, 800), fill = WHITE)
        self.draw_both (self.draw_rectangle, (0, 1200), (400, 1600), fill = WHITE)
        self.draw_both (self.draw_rectangle, (0, 1900), (400, 2000), fill = WHITE)
        self.draw_both (self.draw_rectangle, (0, 1910), (390, 2000), fill = WHITE)
        # Black lines.
        self.draw_both (self.draw_line, (0, 1400), (460, 1400), fill = BLACK, width = 20)
        self.draw_both (self.draw_line, (600, 0), (600, 1260), fill = BLACK, width = 20)
        self.draw_both (self.draw_line, (1200, 0), (1200, 300), fill = BLACK, width = 20)
        self.draw_line ((740, 1300), (2260, 1300), fill = BLACK, width = 20)
        self.draw_line ((740, 300), (2260, 300), fill = BLACK, width = 20)
        self.draw_arc ((450, 1250), 150, outline = BLACK, width = 20, style = 'arc', start = 0)
        self.draw_arc ((3000 - 450, 1250), 150, outline = BLACK, width = 20, style = 'arc', start = pi/2)
        self.draw_arc ((750, 1150), 150, outline = BLACK, width = 20, style = 'arc', start = pi/2)
        self.draw_arc ((3000 - 750, 1150), 150, outline = BLACK, width = 20, style = 'arc', start = 0)
        self.draw_arc ((750, 450), 150, outline = BLACK, width = 20, style = 'arc', start = pi)
        self.draw_arc ((3000 - 750, 450), 150, outline = BLACK, width = 20, style = 'arc', start = -pi/2)
        # Cake.
        self.draw_arc ((1500, 2000), 500, fill = PINK, start = pi, extent = pi)
        self.draw_arc ((1500, 2000), 400, start = pi, extent = pi)
        self.draw_arc ((1500, 2000), 300, start = pi, extent = pi)
        self.draw_arc ((1500, 2000), 200, start = pi, extent = pi, width = 5, style = 'arc')
        self.draw_line ((1500, 2000), (1500, 1800), width = 5)
        # Gifts.
        def draw_gift (x):
            self.draw_rectangle ((x - 374/2, -44), (x + 374/2, -22), fill = YELLOW)
            self.draw_rectangle ((x - 374/2, -94), (x - 374/2 + 22, -44), fill = YELLOW)
            self.draw_rectangle ((x + 374/2, -94), (x + 374/2 - 22, -44), fill = YELLOW)
            self.draw_rectangle ((x - 11, -94), (x + 11, -44), fill = YELLOW)
        draw_gift (600)
        draw_gift (1200)
        draw_gift (1800)
        draw_gift (2400)
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = 'both')
        # Beacons.
        self.draw_both (self.draw_rectangle, (-22, 2000 + 22), (-22 - 80, 2000 + 22 + 80), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, 1000 - 40), (-22 - 80, 1000 + 40), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, -80 - 22), (-22 - 80, -22), fill = BLACK)
        # Children.
        Drawable.draw (self)

if __name__ == '__main__':
    from simu.inter.inter import Inter
    import simu.model.table_eurobot2013 as model
    app = Inter ()
    m = model.Table ()
    Table (app.table_view, m)
    app.mainloop ()
