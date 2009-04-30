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
"""Giboulee balls sorter."""
from simu.inter.drawable import Drawable
from simu.view.servo import Servo
from math import pi

class Sorter (Drawable):

    width = 1
    height = 1

    def __init__ (self, onto, sorter_model):
        Drawable.__init__ (self, onto)
        self.traps = [
                Servo (self, sorter_model.traps[0],
                    (-2.5, -1), 0.8, 0, pi/2),
                Servo (self, sorter_model.traps[1],
                    (-1.5, -0.9), 0.8, 0, pi/2),
                Servo (self, sorter_model.traps[2],
                    (-0.5, -0.8), 0.8, 0, pi/2),
                Servo (self, sorter_model.traps[3],
                    (0.5, -0.8), 0.8, pi, -pi/2),
                Servo (self, sorter_model.traps[4],
                    (1.5, -0.9), 0.8, pi, -pi/2),
                ]
        self.door = Servo (self, sorter_model.door,
                (-2.5, 1.3), 0.8, -pi/6, pi/3),

    def draw (self):
        self.reset ()
        self.trans_scale (0.9/5)
        self.draw_line ((-0.5, 1.5), (-0.5, 0.5), (-2.5, 0.2),
                fill = '#808080')
        self.draw_line ((-2.5, -1.2), (-2.5, -2.3), (2.5, -2.3), (2.5, 0.2),
                (0.5, 0.5), (0.5, 1.5), fill = '#808080')
        for i in (-1.5, -0.5, 0.5, 1.5):
            self.draw_line ((i, -2.3), (i, -2), fill = '#808080')
        Drawable.draw (self)

