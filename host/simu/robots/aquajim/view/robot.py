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
"""AquaJim robot view."""
import simu.inter.drawable

class Robot (simu.inter.drawable.Drawable):

    def __init__ (self, onto, position_model):
        """Construct and make connections."""
        simu.inter.drawable.Drawable.__init__ (self, onto)
        self.position_model = position_model
        self.position_model.register (self.__position_notified)
        self.__position_notified ()

    def __position_notified (self):
        """Called on position modifications."""
        self.pos = self.position_model.pos
        self.angle = self.position_model.angle
        self.update ()

    def draw (self):
        """Draw the robot."""
        self.reset ()
        if self.pos is not None:
            self.trans_rotate (self.angle)
            self.trans_translate (self.pos)
            # Draw robot body.
            # TODO
            self.draw_polygon ((150, 150), (150, -150), (-150, -150),
                    (-150, 150), (150, 150))
            # Draw Robot axis.
            axes_fill = '#404040'
            self.draw_line ((-150, 0), (150, 0), fill = axes_fill,
                    arrow = 'last')
            # Draw Robot wheels.
            f = 150 # Wheel spacing
            wr = 63 / 2 # Wheel diameter
            self.draw_line ((0, +f / 2), (0, -f / 2), fill = axes_fill)
            self.draw_line ((-wr, f / 2), (+wr, f / 2), fill = axes_fill)
            self.draw_line ((-wr, -f / 2), (+wr, -f / 2), fill = axes_fill)
            # Extends.
            simu.inter.drawable.Drawable.draw (self)

