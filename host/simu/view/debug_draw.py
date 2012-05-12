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
"""Display general purpose debug drawing."""
from simu.inter.drawable import Drawable

class DebugDraw (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__colors = ('red', 'blue', 'green', 'yellow')

    def __notified (self):
        self.update ()

    def draw (self):
        self.reset ()
        for d in self.model.drawing:
            if d[0] == 'circle':
                self.draw_circle ((d[1], d[2]), d[3],
                        outline = self.__colors[d[4]])
            elif d[0] == 'segment':
                self.draw_line ((d[1], d[2]), (d[3], d[4]),
                        fill = self.__colors[d[5]])
            elif d[0] == 'point':
                s = 15
                x, y = d[1], d[2]
                self.draw_line (
                        (x - s, y - s), (x + s, y + s),
                        (x - s, y + s), (x + s, y - s),
                        (x - s, y - s), fill = self.__colors[d[3]])
            else:
                raise ValueError

