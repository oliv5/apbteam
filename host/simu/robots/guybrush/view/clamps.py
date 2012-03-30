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
"""Guybrush clamps."""
from simu.inter.drawable import Drawable
from math import pi

GREY = '#808080'
BLACK = '#000000'

class ClampsSide (Drawable):

    width = 440
    height = 350

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.update)

    def draw (self):
        self.reset ()
        # Draw base from side.
        self.trans_translate ((50, -150))
        self.draw_line ((-93, 0), (130, 0), fill = GREY)
        # Draw lower clamp.
        if self.model.lower_clamp_rotation is not None:
            self.trans_push ()
            self.trans_translate ((-117, 84))
            self.trans_rotate (-self.model.lower_clamp_rotation)
            self.draw_polygon ((6, -45), (28, 45), (-6, 45), (-28, -45),
                    outline = BLACK, fill = GREY)
            for i in (0, 1):
                c = self.model.lower_clamp_clamping[i]
                if c is not None:
                    self.draw_line ((7, -45), (7, -46 - c * 33),
                            (-28, -46 - c * 33))
                    if self.model.lower_clamp_content[i]:
                        e, y = self.model.lower_clamp_content[i][0]
                        self.draw_rectangle ((7, -45 - c * 15),
                                (7 - e.radius * 2, -49 - c * 15),
                                fill = GREY)
                if not i:
                    self.trans_rotate (pi)
            self.trans_pop ()
