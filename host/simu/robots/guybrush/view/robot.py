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
"""Guybrush robot view."""
import simu.inter.drawable
from math import pi

COLOR_ROBOT = '#000000'
COLOR_AXES = '#202040'

class Robot (simu.inter.drawable.Drawable):

    def __init__ (self, onto, position_model):
        """Construct and make connections."""
        simu.inter.drawable.Drawable.__init__ (self, onto)
        self.position_model = position_model
        self.position_model.register (self.__position_notified)

    def __position_notified (self):
        """Called on position modifications."""
        self.pos = self.position_model.pos
        self.angle = self.position_model.angle
        self.update ()

    def draw (self):
        """Draw the robot."""
        self.reset ()
        if self.pos is not None:
            self.trans_translate (self.pos)
            self.trans_rotate (self.angle)
            # Draw robot body.
            self.draw_polygon ((0, 190), (150, 110), (95, 95), (55, 55),
                    (40, -0), (55, -55), (95, -95), (150, -110), (0, -190),
                    (-150, -110), (-95, -95), (-55, -55), (-40, -0),
                    (-55, 55), (-95, 95), (-150, 110), (0, 190),
                    fill = COLOR_ROBOT)
            self.draw_arc ((0, 0), 190, start = pi * 35 / 180,
                    extent = pi * 110 / 180, style = 'chord',
                    outline = COLOR_ROBOT, fill = COLOR_ROBOT)
            self.draw_arc ((0, 0), 190, start = pi + pi * 35 / 180,
                    extent = pi * 110 / 180, style = 'chord',
                    outline = COLOR_ROBOT, fill = COLOR_ROBOT)
            # Draw Robot axis.
            self.draw_line ((-50, 0), (50, 0), fill = COLOR_AXES,
                    arrow = 'last')
            # Draw Robot wheels.
            f = 221 # Wheel spacing
            wr = 60 / 2 # Wheel radius
            self.draw_line ((0, +f / 2), (0, -f / 2), fill = COLOR_AXES)
            self.draw_line ((-wr, f / 2), (+wr, f / 2), fill = COLOR_AXES)
            self.draw_line ((-wr, -f / 2), (+wr, -f / 2), fill = COLOR_AXES)
            # Extends.
            simu.inter.drawable.Drawable.draw (self)

