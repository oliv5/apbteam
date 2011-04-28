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
"""Eurobot 2010 table."""
from simu.inter.drawable import Drawable
from math import pi

GREEN = '#00ad00'
RED = '#f80404'
BLUE = '#0000ee'
BLACK = '#404040'
YELLOW = '#e5e500'
WHITE = '#c9c9d0'
ORANGE = '#f6991f'
PLEXI = '#99bbbb'
BROWN = '#99bbbb'

corn_attr = { False: dict (fill = WHITE),
        True: dict (fill = BLACK) }

class Round (Drawable):

    def __init__ (self, onto, attr, model):
        Drawable.__init__ (self, onto)
        self.attr = attr
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
            self.draw_circle ((0, 0), self.model.radius, **self.attr)
            Drawable.draw (self)

class Corn (Round):
    pass

class Tomato (Round):
    pass

class Orange (Round):
    pass

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        for e in model.corns:
            Corn (self, corn_attr[e.black], e)
        for e in model.tomatos:
            Tomato (self, dict (fill = RED), e)
        for e in model.oranges:
            Orange (self, dict (fill = ORANGE), e)

    def draw_both (self, primitive, *args, **kargs):
        """Draw a primitive on both sides."""
        primitive (*args, **kargs)
        primitive (*((3000 - x, y) for x, y in args), **kargs)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22), (3000 + 22, 2100 + 22), fill = BLACK)
        self.draw_rectangle ((0, -22), (3000, 2100), fill = GREEN)
        self.draw_rectangle ((500, -22), (2500, 0), fill = BLACK)
        self.draw_rectangle ((0, 2100 - 500), (500, 2100), fill = BLUE)
        self.draw_rectangle ((3000 - 500, 2100 - 500), (3000, 2100), fill = YELLOW)
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = 'both')
        # Bags.
        self.draw_rectangle ((-20, -22), (500 + 20, -250 - 20), fill = YELLOW)
        self.draw_rectangle ((3000 + 20, -22), (3000 - 500 - 20, -250 - 20), fill = BLUE)
        self.draw_both (self.draw_rectangle, (0, -22), (500, -250), fill = PLEXI)
        # Beacons.
        self.draw_both (self.draw_rectangle, (-22, 2100 + 22), (-22 - 80, 2100 + 22 + 80), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, 1050 - 40), (-22 - 80, 1050 + 40), fill = BLACK)
        self.draw_both (self.draw_rectangle, (-22, -80 - 22), (-22 - 80, -22), fill = BLACK)
        # Hill.
        self.draw_rectangle ((750 - 100, 2100), (3000 - 750 + 100, 2100 - 500), fill = WHITE)
        self.draw_both (self.draw_line, (750, 2100), (750, 2100 - 500))
        self.draw_both (self.draw_line, (750 + 500, 2100), (750 + 500, 2100 - 500))
        self.draw_rectangle ((740, 2100 - 500), (3000 - 740, 2100 - 500 - 22), fill = BLACK)
        self.draw_both (self.draw_line, (740 + 500, 2100 - 500), (740 + 500, 2100 - 500 - 22))
        self.draw_both (self.draw_line, (740, 2100 + 22), (740, 2100))
        self.draw_both (self.draw_line, (740 + 500, 2100 + 22), (740 + 500, 2100))
        # Trees.
        for pos in self.model.oranges_pos:
            self.draw_circle ((1500 - pos[0], 2100 - pos[1]), 25)
            self.draw_circle ((1500 + pos[0], 2100 - pos[1]), 25)
        # Children.
        Drawable.draw (self)

if __name__ == '__main__':
    from simu.inter.inter import Inter
    import simu.model.table_eurobot2010 as model
    app = Inter ()
    m = model.Table ()
    Table (app.table_view, m)
    app.mainloop ()
