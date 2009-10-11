# simu - Robot simulation. {{{
#
# Copyright (C) 2008 Nicolas Schodet
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
"""Eurobot 2009 table."""
from simu.inter.drawable import Drawable
from simu.utils.trans_matrix import TransMatrix
from math import pi

GREEN = '#62b656'
RED = '#ff1f1f'
BLUE = '#0f84c9'
BLACK = '#404040'
BROWN = '#c96a16'
PLEXI = '#99bbbb'

puck_attr = { False: dict (fill = GREEN),
        True: dict (fill = RED) }

class Puck (Drawable):

    def __init__ (self, onto, attr, model):
        Drawable.__init__ (self, onto)
        self.attr = attr
        self.model = model
        self.model.register (self.__notified)

    def __notified (self):
        self.pos = self.model.pos
        self.update ()

    def draw (self):
        self.reset ()
        if self.pos:
            self.trans_translate (self.pos)
            self.draw_circle ((0, 0), self.model.radius, **self.attr)
            Drawable.draw (self)

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        for p in model.pucks:
            Puck (self, puck_attr[p.color], p)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, 0), (3000 + 22, 2100 + 22), fill = 'white')
        self.draw_rectangle ((-22, -10), (3000 + 22, 0), fill = PLEXI)
        self.draw_rectangle ((0, 0), (3000, 2100), fill = BLUE)
        self.draw_rectangle ((0, 2100 - 500), (500, 2100), fill = GREEN)
        self.draw_rectangle ((3000 - 500, 2100 - 500), (3000, 2100), fill = RED)
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = 'both')
        # Beacons.
        self.draw_rectangle ((-22, 2100 + 22), (-22 - 80, 2100 + 22 + 80), fill = BLACK)
        self.draw_rectangle ((-22, 1050 - 40), (-22 - 80, 1050 + 40), fill = BLACK)
        self.draw_rectangle ((-22, -80 - 10), (-22 - 80, -10), fill = BLACK)
        self.draw_rectangle ((3000 + 22, 2100 + 22), (3000 + 22 + 80, 2100 + 22 + 80), fill = BLACK)
        self.draw_rectangle ((3000 + 22, 1050 - 40), (3000 + 22 + 80, 1050 + 40), fill = BLACK)
        self.draw_rectangle ((3000 + 22, -80 - 10), (3000 + 22 + 80, -10), fill = BLACK)
        # Building areas.
        self.draw_circle ((1500, 1050), 150, fill = BROWN)
        self.draw_rectangle ((1500 - 900, 0), (1500 + 900, 100), fill = BROWN)
        self.draw_rectangle ((1500 - 300, 0), (1500 + 300, 100), fill = BROWN)
        self.draw_rectangle ((1500 - 900 - 22, 0), (1500 - 900, 100), fill = 'white')
        self.draw_rectangle ((1500 + 900 + 22, 0), (1500 + 900, 100), fill = 'white')
        for bx in (-600, 0, +600):
            for dx in (-130 - 65, -65, +65, +130 + 65):
                x = 1500 + bx + dx
                self.draw_rectangle ((x - 7.5, 100), (x + 7.5, 100 + 250), fill = BLACK)
        # Lintel dispensers.
        for x in (1500 - 600, 1500 - 200, 1500 + 200, 1500 + 600):
            self.draw_rectangle ((x - 100, 2100), (x + 100, 2100 + 70 + 22), fill = BLACK)
            if x < 1500:
                color = GREEN
            else:
                color = RED
            self.draw_rectangle ((x - 100, 2100), (x + 100, 2100 + 70), fill = color)
            self.draw_rectangle ((x - 7.5, 2100), (x + 7.5, 2100 - 250), fill = BLACK)
        # Vertical dispensers.
        if self.model.dispensers_card == 1:
            ds = -250
        else:
            ds = +250
        for dpos, dangle, dcolor in (
                ((289, 40), pi / 2, RED),
                ((3000 - 289, 40), pi / 2, GREEN),
                ((40, 1050 + ds), 0, RED),
                ((3000 - 40, 1050 + ds), pi, GREEN),
                ):
            dtm = TransMatrix ()
            dtm.rotate (dangle)
            dtm.translate (dpos)
            if dpos[1] == 40:
                a = 55
            else:
                a = 67
            self.draw_rectangle (dtm.apply ((-a, -25)), dtm.apply ((0, 25)), fill = PLEXI)
            self.draw_rectangle (dtm.apply ((-40, -40)), dtm.apply ((40, 40)), fill = PLEXI)
            self.draw_circle (dtm.apply ((0, 0)), 35, fill = PLEXI, outline = dcolor)
            self.draw_rectangle (dtm.apply ((-40, -40)), dtm.apply ((40, 40)), fill = '')
        # Pucks.
        for m, color in ((-1, GREEN), (1, RED)):
            for x in (400, 650, 900):
                for y in (-125, 75, 275, 475):
                    self.draw_circle ((1500 + m * x, 1050 + y), 35, outline = color)
        # Children.
        Drawable.draw (self)

if __name__ == '__main__':
    from simu.inter.inter import Inter
    import simu.model.table_eurobot2009 as model
    app = Inter ()
    m = model.Table ()
    Table (app.table_view, m)
    app.mainloop ()
