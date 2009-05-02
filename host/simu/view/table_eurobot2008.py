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
"""Eurobot 2008 table."""
from simu.inter.drawable import Drawable

class Table (Drawable):
    """The table and its elements."""

    def __init__ (self, onto, table_model):
        Drawable.__init__ (self, onto)

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22 - 80), (3000 / 2, 2100 + 22), fill = '#ff1f1f')
        self.draw_rectangle ((3000 / 2, -22 - 80), (3000 + 22, 2100 + 22), fill = '#201fff')
        self.draw_rectangle ((0, 0), (3000, 2100), fill = '#a49d8b')
        self.draw_rectangle ((0, -22 - 80), (3000, -22), fill = '#a49d8b')
        self.draw_rectangle ((0, 2100 - 500), (500, 2100), fill = '#201fff')
        self.draw_rectangle ((3000 - 500, 2100 - 500), (3000, 2100), fill = '#ff1f1f')
        self.draw_line ((3000 / 2, -22 - 80), (3000 / 2, 2100 + 22))
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = 'both')
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
        self.draw_circle ((40, 2100 - 750), 40)
        self.draw_rectangle ((700 - 85 / 2, 2100), (700 + 85 / 2, 2100 + 22), fill = '#5b5b5b')
        self.draw_circle ((700, 2100 - 40), 40)
        self.draw_rectangle ((3000 + 22, 2100 - 750 - 85 / 2), (3000, 2100 - 750 + 85 / 2), fill = '#5b5b5b')
        self.draw_circle ((3000 - 40, 2100 - 750), 40)
        self.draw_rectangle ((3000 - 700 + 85 / 2, 2100), (3000 - 700 - 85 / 2, 2100 + 22), fill = '#5b5b5b')
        self.draw_circle ((3000 - 700, 2100 - 40), 40)
        # Horizontal dispenser.
        self.draw_rectangle ((3000 / 2 - 924 / 2, 2100 + 22), (3000 / 2 + 924 / 2, 2100 + 22 + 80 + 22), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 - 924 / 2 + 22, 2100 + 22), (3000 / 2 + 924 / 2 - 22, 2100 + 22 + 80), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 - 880 / 2 - 35 - 60, 2100), (3000 / 2 - 880 / 2 - 35, 2100 + 44), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 + 880 / 2 + 35 + 60, 2100), (3000 / 2 + 880 / 2 + 35, 2100 + 44), fill = '#5b5b5b')
        # Balls.
        balls = [ (800, 200, 'rb'), (800, 400, 'RB'), (800, 600, 'ww'),
                (1300, 200, 'rb'), (1300, 400, 'rb'), (1300, 600, 'ww'),
                (520, 800, 'WW'), (700, 40, 'RB'), (40, 750, 'WW'),
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
            self.draw_circle ((3000 - b[0], 2100 - b[1]), 72 / 2,
                    **balls_config[b[2][0]])
            if len (b[2]) > 1:
                self.draw_circle ((b[0], 2100 - b[1]), 72 / 2,
                        **balls_config[b[2][1]])
        Drawable.draw (self)

if __name__ == '__main__':
    from simu.inter.inter import Inter
    app = Inter ()
    Table (app.table_view, None)
    app.mainloop ()
