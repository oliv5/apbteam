# simu - Robot simulation. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
"""Eurobot 2012 table."""
from simu.inter.drawable import Drawable
import math
from math import pi

PURPLE = '#a737ff'
RED = '#fe4543'
BLUE = '#01c3ff'
GREEN = '#81ff00'
BLACK = '#1f1b1d'
YELLOW = '#f7ff00'
WHITE = '#f5fef2'
TRANS = '#c0c2b5'
BROWN = '#9d4e01'

def draw_coin (d, coin):
    d.trans_push ()
    color = WHITE if coin.value else BLACK
    d.draw_circle ((0, 0), coin.radius, fill = color)
    d.draw_circle ((0, 0), 7.5, fill = color)
    d.trans_translate ((40.5, 0))
    d.draw_rectangle ((-9, -9), (9, 9), fill = color, outline = TRANS)
    d.trans_pop ()

class Coin (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.pos = self.model.pos
        self.angle = self.model.angle
        self.update ()

    def draw (self):
        self.reset ()
        if self.pos:
            self.trans_translate (self.pos)
            self.trans_rotate (self.angle)
            draw_coin (self, self.model)
            Drawable.draw (self)

def draw_gold_bar (d, gold_bar):
    d.draw_rectangle ((-gold_bar.dim[0] / 2, -gold_bar.dim[1] / 2),
            (gold_bar.dim[0] / 2, gold_bar.dim[1] / 2), fill = YELLOW)
    d.draw_rectangle ((-gold_bar.dim[0] / 2 + 13, -gold_bar.dim[1] / 2 + 13),
            (gold_bar.dim[0] / 2 - 13, gold_bar.dim[1] / 2 - 13), fill = YELLOW)

class GoldBar (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.pos = self.model.pos
        self.angle = self.model.angle
        self.update ()

    def draw (self):
        self.reset ()
        if self.pos:
            self.trans_translate (self.pos)
            self.trans_rotate (self.angle)
            draw_gold_bar (self, self.model)
            Drawable.draw (self)

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        for e in self.model.coins:
            if e.level <= 2:
                Coin (self, e)
        for e in self.model.gold_bars:
            GoldBar (self, e)
        for e in self.model.coins:
            if e.level > 2:
                Coin (self, e)

    def draw_both (self, primitive, *args, **kargs):
        """Draw a primitive on both sides."""
        primitive (*args, **kargs)
        primitive (*((3000 - x, y) for x, y in args), **kargs)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22), (3000 + 22, 2000 + 22), fill = BLUE)
        self.draw_rectangle ((0, 0), (3000, 2000), fill = BLUE)
        #  Start zones.
        self.draw_rectangle ((0, 2000 - 500), (500, 2000), fill = PURPLE)
        self.draw_rectangle ((3000, 2000 - 500), (3000 - 500, 2000), fill = RED)
        #  Black lines.
        pup = 2000 - 500 + 50
        self.draw_both (self.draw_polygon, (500, pup), (650, pup), (650, 0), (630, 0),
                (630, pup - 20), (500, pup - 20), fill = BLACK, outline = 'black')
        #  Ships.
        ba = 0.04995839
        self.draw_both (self.draw_polygon, (0, 2000 - 500), (400, 2000 - 500), (325, 0), (0, 0),
                fill = BROWN, outline = 'black')
        self.draw_both (self.draw_rectangle, (0, 2000 - 500), (400, 2000 - 500 - 18), fill = BROWN)
        cba = 750 * math.cos (ba)
        sba = 750 * math.sin (ba)
        self.draw_both (self.draw_polygon, (325, 0), (325 + sba, cba), (325 + sba + 18, cba), (325 + 18, 0),
                fill = BROWN, outline = 'black')
        self.draw_both (self.draw_rectangle, (-22, -22), (340, 610 - 22), fill = TRANS)
        self.draw_both (self.draw_rectangle, (-4, -4), (340 - 18, 610 - 22 - 18), fill = TRANS)
        #  Peanut island.
        self.draw_both (self.draw_circle, (1500 - 400, 1000), r = 300, fill = YELLOW)
        self.draw_rectangle ((1500 - 400 + 141, 1000 + 265), (1500 + 400 - 141, 1000 - 265), fill = YELLOW, outline = '')
        self.draw_arc ((1500, 1000 + 750), 550, start = pi + 1, extent = pi - 2, fill = BLUE, outline = '')
        self.draw_arc ((1500, 1000 + 750), 550, start = pi + 1.08083, extent = pi - 2 * 1.08083, style = 'arc')
        self.draw_arc ((1500, 1000 - 750), 550, start = 1, extent = pi - 2, fill = BLUE, outline = '')
        self.draw_arc ((1500, 1000 - 750), 550, start = 1.08083, extent = pi - 2 * 1.08083, style = 'arc')
        self.draw_both (self.draw_circle, (1500 - 400, 1000), r = 200, fill = GREEN)
        self.draw_circle ((1500, 1000), r = 75, fill = GREEN)
        self.draw_both (self.draw_rectangle, (1500 - 400 - 125, 1000 - 125), (1500 - 400 + 125, 1000 + 125), fill = BROWN)
        #  Map island.
        self.draw_arc ((1500, 2000), 400, start = pi, extent = pi, fill = YELLOW)
        self.draw_arc ((1500, 2000), 300, start = pi, extent = pi, fill = GREEN)
        #  Map.
        self.draw_rectangle ((1500 - 200 - 22, 2000 + 148), (1500 + 200 + 22, 2000 + 140), fill = BLUE)
        self.draw_rectangle ((1500 - 200, 2000 + 140), (1500 + 200, 2000 - 8), fill = BLUE)
        self.draw_both (self.draw_rectangle, (1500 - 200 - 22, 2000 + 140), (1500 - 200, 2000 + 125), fill = BLUE)
        self.draw_both (self.draw_rectangle, (1500 - 200 - 22, 2000 + 125), (1500 - 200, 2000 - 23), fill = BLUE)
        self.draw_rectangle ((1500 - 22, 2000 + 140), (1500 + 22, 2000 + 125), fill = BLUE)
        self.draw_rectangle ((1500 - 22, 2000 + 125), (1500 + 22, 2000 + 115), fill = BLUE)
        self.draw_rectangle ((1500 - 22, 2000 + 2), (1500 + 22, 2000 - 13), fill = BLUE)
        self.draw_rectangle ((1500 - 22, 2000 - 13), (1500 + 22, 2000 - 23), fill = BLUE)
        # Bottles.
        def draw_bottle (x, color):
            self.draw_rectangle ((x - 100, 0), (x + 100, -44), fill = color)
            self.draw_rectangle ((x - 100, -44), (x + 100, -66), fill = color)
            self.draw_rectangle ((x - 11, 44), (x + 11, 0), fill = color)
            self.draw_rectangle ((x - 50, 22), (x + 100, -44), fill = color)
            self.draw_polygon ((x - 50, 22), (x - 80, 0), (x - 100, 0), (x - 100, -22), (x - 80, -22), (x - 50, -44),
                    fill = color, outline = 'black')
        draw_bottle (640, PURPLE)
        draw_bottle (640 + 477, RED)
        draw_bottle (3000 - 640, RED)
        draw_bottle (3000 - 640 - 477, PURPLE)
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
    import simu.model.table_eurobot2012 as model
    app = Inter ()
    m = model.Table ()
    Table (app.table_view, m)
    app.mainloop ()
