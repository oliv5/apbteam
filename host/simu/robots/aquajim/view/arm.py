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
"""AquaJim arm."""
from simu.inter.drawable import Drawable
from math import pi, cos, sin

class Arm (Drawable):

    width = 1
    height = 1

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.angle = self.model.arm_angle
        self.update ()

    def draw (self):
        self.reset ()
        self.draw_arc ((0, 0), 0.45, start = 7 * pi / 12, extent = 10 * pi / 12,
                style = 'arc', outline = '#808080')
        self.draw_arc ((0, 0), 0.45, start = -5 * pi / 12, extent = 10 * pi / 12,
                style = 'arc', outline = '#808080')
        self.draw_arc ((0, 0), 0.25, start = -7 * pi / 12, extent = 14 * pi / 12,
                style = 'arc', outline = '#808080')
        if self.angle is not None:
            self.trans_scale (0.4)
            self.trans_rotate (-self.angle)
            self.draw_line ((0, 0), (0, 1))
            self.draw_line ((0, 1), (0.3, 1), arrow = 'last', fill = '#808080')
            self.draw_line ((0, 0), (cos (pi / 6), -sin (pi / 6)))
            self.draw_line ((0, 0), (-cos (pi / 6), -sin (pi / 6)))
            Drawable.draw (self)

