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
"""Generic servo motor."""
from simu.inter.drawable import Drawable
from math import pi, sin, cos

class Servo (Drawable):

    def __init__ (self, onto, model, coord = (0, 0), l = 1, start = 0,
            extent = pi / 2):
        Drawable.__init__ (self, onto)
        self.model = model
        self.coord = coord
        self.l = l
        self.start = start
        self.extent = extent
        self.model.register (self.__notified)

    def __notified (self):
        self.value = self.model.value
        self.update ()

    def draw (self):
        self.reset ()
        if self.value is not None:
            self.draw_arc (self.coord, self.l, start = self.start,
                    extent = self.extent, style = 'arc', outline = '#808080')
            a = self.start + self.value * self.extent
            self.draw_line (self.coord, (self.coord[0] + self.l * cos (a),
                self.coord[1] + self.l * sin (a)))
            Drawable.draw (self)

