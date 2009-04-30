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
"""Generic distance sensor model."""
from simu.inter.drawable import Drawable
from math import sin, cos

class DistanceSensor (Drawable):

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.distance = self.model.distance
        self.update ()

    def draw (self):
        self.reset ()
        if self.distance is None:
            self.draw_line (self.model.pos, self.model.target, fill = 'blue',
                    arrow = 'last')
        else:
            inter = (self.model.pos[0] + cos (self.model.angle) * self.distance,
                    self.model.pos[1] + sin (self.model.angle) * self.distance)
            self.draw_line (self.model.pos, inter, fill = 'red',
                    arrow = 'last')
            self.draw_line (inter, self.model.target, fill = 'blue',
                    arrow = 'last')
        Drawable.draw (self)
