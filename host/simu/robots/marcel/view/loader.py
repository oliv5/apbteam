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
"""Marcel loader."""
from simu.inter.drawable import Drawable

from simu.view.table_eurobot2010 import RED, corn_attr

class Loader (Drawable):

    width = 420
    height = 370

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)

    def __notified (self):
        self.update ()

    def draw (self):
        self.reset ()
        self.trans_translate ((-100, -125))
        self.draw_line ((25, 100), (25, 50), (300, 0), fill = '#808080')
        if self.model.load:
            # Draw load.
            y = 0
            x = 0
            for e in self.model.load:
                if hasattr (e, 'black'):
                    attr = corn_attr[e.black]
                else:
                    attr = dict (fill = RED)
                cx = x + e.radius
                cy = y + cx * 50 / 300 + e.radius
                self.draw_circle ((300 - cx, cy), e.radius, **attr)
                x += e.radius
                if x > 200:
                    x = 0
                    y += 40
        if self.model.gate_angle is not None:
            ratio = 1 - self.model.gate_angle / self.model.GATE_STROKE
            # Draw gate.
            for i in xrange (0, 4):
                self.draw_line ((100, 125 + 5 * i),
                        (300, 25 + 25 * i + (150 - 20 * i) * ratio))
        if self.model.elevator_height is not None:
            self.trans_identity ()
            self.trans_rotate (-self.model.elevator_angle)
            self.trans_translate ((-100, -100 + self.model.elevator_height))
            # Draw clamp load.
            if self.model.clamp_load:
                elements = self.model.clamp_load
                for e in elements:
                    if hasattr (e, 'black'):
                        self.draw_rectangle ((-25, -25), (-75, 125),
                                **corn_attr[e.black])
                    else:
                        self.draw_circle ((-50, 0), e.radius, fill = RED)
            # Draw clamp.
            self.draw_line ((-100, -25), (0, -25), (0, 25), (-100, 25))
