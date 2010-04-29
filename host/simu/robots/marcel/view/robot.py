# simu - Robot simulation. {{{
#
# Copyright (C) 2010 Nicolas Schodet
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
"""Marcel robot view."""
import simu.inter.drawable
from math import cos

from simu.view.table_eurobot2010 import RED, corn_attr

class Robot (simu.inter.drawable.Drawable):

    def __init__ (self, onto, position_model, loader_model):
        """Construct and make connections."""
        simu.inter.drawable.Drawable.__init__ (self, onto)
        self.position_model = position_model
        self.position_model.register (self.__position_notified)
        self.loader_model = loader_model
        self.loader_model.register (self.__loader_notified)

    def __position_notified (self):
        """Called on position modifications."""
        self.pos = self.position_model.pos
        self.angle = self.position_model.angle
        self.update ()

    def __loader_notified (self):
        """Called on loader modifications."""
        self.clamp_pos = self.loader_model.clamp_pos
        self.update ()

    def draw (self):
        """Draw the robot."""
        self.reset ()
        if self.pos is not None:
            self.trans_rotate (self.angle)
            self.trans_translate (self.pos)
            # Draw robot body.
            self.draw_polygon ((120, 155), (-95, 155), (-160, 90),
                    (-160, -90), (-95, -155), (120, -155))
            # Draw Robot axis.
            axes_fill = '#404040'
            self.draw_line ((-150, 0), (150, 0), fill = axes_fill,
                    arrow = 'last')
            # Draw Robot wheels.
            f = 142 + 2 * 31.5 - 13.5 # Wheel spacing
            wr = 63 / 2 # Wheel diameter
            self.draw_line ((0, +f / 2), (0, -f / 2), fill = axes_fill)
            self.draw_line ((-wr, f / 2), (+wr, f / 2), fill = axes_fill)
            self.draw_line ((-wr, -f / 2), (+wr, -f / 2), fill = axes_fill)
            # Draw robot clamp.
            if (self.clamp_pos[0] is not None
                    and self.clamp_pos[1] is not None):
                l = self.loader_model.CLAMP_LENGTH * cos (
                        self.loader_model.elevator_angle)
                y = self.loader_model.CLAMP_WIDTH / 2 - self.clamp_pos[0]
                self.draw_line ((120, y - 10), (120 + 0.15 * l, y),
                        (120 + 0.85 * l, y), (120 + l, y - 10))
                y = -self.loader_model.CLAMP_WIDTH / 2 + self.clamp_pos[1]
                self.draw_line ((120, y + 10), (120 + 0.15 * l, y),
                        (120 + 0.85 * l, y), (120 + l, y + 10))
                # Draw clamp load.
                if self.loader_model.clamp_load:
                    elements = self.loader_model.clamp_load
                    tickness = sum (e.radius * 2 for e in elements)
                    y = tickness / 2
                    for e in elements:
                        if hasattr (e, 'black'):
                            attr = corn_attr[e.black]
                        else:
                            attr = dict (fill = RED)
                        self.draw_circle ((120 + l / 2, y - e.radius),
                                e.radius, **attr)
                        y -= e.radius * 2
            # Extends.
            simu.inter.drawable.Drawable.draw (self)

