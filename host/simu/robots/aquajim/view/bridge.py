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
"""AquaJim bridge."""
from simu.inter.drawable import Drawable
from math import pi, cos, sin

from simu.view.table_eurobot2009 import puck_attr

class Bridge (Drawable):

    width = 220
    height = 100

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)
        self.update ()

    def draw (self):
        self.reset ()
        self.trans_rotate (0.95 * pi / 2)
        self.draw_line ((-40, -80), (-40, 80), fill = '#808080')
        self.draw_line ((40, -80), (40, 80), fill = '#808080')
        for i in range (2):
            puck = self.model.bridge_slot[i]
            if puck is not None:
                self.draw_circle ((0, 40 - 80 * i), 35,
                        **puck_attr[puck.color])
        if self.model.bridge_door_servo_value is not None:
            self.draw_arc ((0, -40), 37.5, start = -3 * pi / 4
                    + self.model.bridge_door_servo_value * pi, extent = pi / 2)
        if self.model.bridge_finger_servo_value is not None:
            self.draw_arc ((-40, -50), 50, start = pi / 2, extent = -3 * pi / 4,
                    style = 'arc', outline = '#c0c0c0')
            a = pi / 2 - self.model.bridge_finger_servo_value * 3 * pi / 4
            self.draw_line ((-40, -50), (-40 + 50 * cos (a), -50 + 50 * sin (a)))

