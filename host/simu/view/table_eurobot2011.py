# simu - Robot simulation. {{{
#
# Copyright (C) 2009 Nicolas Schodet
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
"""Eurobot 2011 table."""
from simu.inter.drawable import Drawable
from simu.utils.trans_matrix import TransMatrix
from math import pi

RED = '#d42626'
BLUE = '#2626d4'
GREEN = '#268126'
BLACK = '#181818'
YELLOW = '#cccc00'

def draw_pawn (d, radius, kind):
    d.draw_circle ((0, 0), radius, fill = YELLOW)
    if kind == 'king':
        a = 0.1 * radius
        b = 0.5 * radius
        d.draw_line ((a, b), (a, a), (b, a), (b, -a), (a, -a), (a, -b),
                (-a, -b), (-a, -a), (-b, -a), (-b, a), (-a, a), (-a, b),
                (a, b))
    elif kind == 'queen':
        d.draw_circle ((0, 0), 0.5 * radius)
        d.draw_circle ((0, 0), 0.4 * radius)

class Pawn (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.pos = self.model.pos
        self.update ()

    def draw (self):
        self.reset ()
        if self.pos:
            self.trans_translate (self.pos)
            draw_pawn (self, self.model.radius, self.model.kind)
            Drawable.draw (self)

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        for e in model.pawns:
            Pawn (self, e)

    def draw_both (self, primitive, *args, **kargs):
        """Draw a primitive on both sides."""
        primitive (*args, **kargs)
        primitive (*((3000 - x, y) for x, y in args), **kargs)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22), (3000 + 22, 2100 + 22), fill = BLACK)
        self.draw_rectangle ((0, 0), (3000, 2100), fill = BLACK)
        self.draw_both (self.draw_rectangle, (0, 0), (400, 2100 - 400 - 22), fill = GREEN)
        self.draw_rectangle ((0, 2100 - 400), (400, 2100), fill = RED)
        self.draw_rectangle ((0, 2100 - 400 - 22), (400, 2100 - 400), fill = RED)
        self.draw_rectangle ((3000, 2100 - 400), (3000 - 400, 2100), fill = BLUE)
        self.draw_rectangle ((3000, 2100 - 400 - 22), (3000 - 400, 2100 - 400), fill = BLUE)
        self.draw_rectangle ((1500 - 3 * 350, 0), (1500 + 3 * 350, 2100), fill = BLUE)
        for i in xrange (-3, 3):
            for j in xrange (0, 6):
                if (i + j) % 2:
                    self.draw_rectangle ((1500 + i * 350, j * 350),
                            (1850 + i * 350, 350 + j * 350), fill = RED)
        # Bonus.
        for i, j in ((0, 0), (1, 2), (1, 4)):
            self.draw_circle ((1500 - 175 - i * 350, 175 + j * 350), 50, fill = BLACK)
            self.draw_circle ((1500 + 175 + i * 350, 175 + j * 350), 50, fill = BLACK)
        # Protected zones.
        self.draw_both (self.draw_rectangle, (1500 - 3 * 350, 0), (1500 - 350, 120), fill = BLACK)
        self.draw_both (self.draw_rectangle, (1500 - 3 * 350, 120), (1500 - 3 * 350 + 22, 120 + 130), fill = BLACK)
        self.draw_both (self.draw_rectangle, (1500 - 350, 120), (1500 - 350 - 22, 120 + 130), fill = BLACK)
        self.draw_both (self.draw_rectangle, (1500 - 350, 120 + 130), (1500 - 350 - 20, 350), fill = BLACK)
        self.draw_both (self.draw_rectangle, (1500 - 3 * 350, 350 - 20), (1500 - 350, 350), fill = BLACK)
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = 'both')
        # Beacons.
        self.draw_both (self.draw_rectangle, (-22, 2100 + 22), (-22 - 80, 2100 + 22 + 80), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, 1050 - 40), (-22 - 80, 1050 + 40), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, -80 - 22), (-22 - 80, -22), fill = BLACK)
        # Children.
        Drawable.draw (self)

if __name__ == '__main__':
    from simu.inter.inter import Inter
    import simu.model.table_eurobot2011 as model
    app = Inter ()
    m = model.Table ()
    Table (app.table_view, m)
    app.mainloop ()
