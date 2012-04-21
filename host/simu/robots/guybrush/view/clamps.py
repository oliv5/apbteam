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
from math import pi, cos, sin, radians
from simu.view.table_eurobot2012 import WHITE, BLACK, YELLOW

COIN_BLACK = BLACK

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
        # Draw load.
        self.trans_push ()
        self.trans_translate ((60, 0))
        for i, e in enumerate (self.model.load):
            if e.value == 3:
                self.draw_polygon ((-75, 0), (-75 + 13, 48.5),
                        (75 - 13, 48.5), (75, 0), fill = YELLOW,
                        outline = BLACK)
            else:
                color = WHITE if e.value else COIN_BLACK
                self.draw_oval ((0, 15), 60, 15, fill = color)
            self.trans_translate ((0, 15))
        self.trans_pop ()
        # Draw doors.
        if self.model.door is not None:
            a = self.model.door * 200
            b = 100 + self.model.door * 100
            self.draw_line ((130, a), (130, a + 100))
            self.draw_line ((125, b), (125, b + 100))
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
        # Draw upper clamp.
        ud = self.model.upper_clamp_up_down
        c = self.model.upper_clamp_clamping
        io = self.model.upper_clamp_in_out
        if (ud is not None and c is not None and io is not None):
            self.trans_push ()
            # Approximation: map cylinder extension to angle.
            a = radians (ud * 75 + 87)
            x = 40 + 188 * cos (a)
            y = 75 + 188 * sin (a)
            self.trans_translate ((x, y))
            self.trans_rotate ((1 - ud) * 0.5 * pi)
            self.draw_line ((0, 56), (0, -25), fill = GREY)
            # Draw level 2 clamp.
            for e, y, in self.model.upper_clamp_content:
                if e.level == 2:
                    self.draw_polygon ((-15, -15), (-15 - 70, -15),
                            (-15 - 13 - 44, -15 + 48.5),
                            (-15 - 13, -15 + 48.8), fill = YELLOW,
                            outline = BLACK)
                    break
            self.draw_line ((-15, -10 + c * 5), (-15 - 70, -10 + c * 5))
            # Draw level 3 clamp.
            for e, y in self.model.upper_clamp_content:
                if e.level == 3:
                    self.draw_rectangle ((-12 * io - e.radius * 2, 54),
                            (-12 * io, 50), fill = GREY)
            self.draw_line ((-12 * io - 35, 55), (-12 * io, 55),
                    (-12 * io, 55 - c * 33), (-12 * io - 35, 55 - c * 33))
            self.trans_pop ()
