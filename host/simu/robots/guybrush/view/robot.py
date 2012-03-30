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
from math import pi, cos
from simu.view.table_eurobot2012 import WHITE, BLACK

COLOR_ROBOT = '#000000'
COLOR_AXES = '#202040'

class Robot (simu.inter.drawable.Drawable):

    def __init__ (self, onto, position_model, clamps_model):
        """Construct and make connections."""
        simu.inter.drawable.Drawable.__init__ (self, onto)
        self.position_model = position_model
        self.clamps_model = clamps_model
        self.position_model.register (self.__position_notified)
        self.clamps_model.register (self.update)

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
            # Draw lower clamps content.
            r = self.clamps_model.lower_clamp_rotation
            if r is not None:
                sx = cos (r)
                for content in self.clamps_model.lower_clamp_content:
                    for e, y in content:
                        x = 117 + sx * 54
                        color = WHITE if e.value else BLACK
                        self.draw_oval ((x, y), sx * e.radius, e.radius,
                                fill = color)
                    sx = - sx
            # Draw robot body.
            self.draw_polygon ((150, 171.5), (-80, 171.5), (-130, 121.5),
                    (-130, -121.5), (-80, -171.5), (150, -171.5),
                    fill = COLOR_ROBOT)
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

