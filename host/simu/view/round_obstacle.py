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
"""Obstacle with a round shape."""
from simu.inter.drawable import Drawable

class RoundObstacle (Drawable):

    MARGIN = 250

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()

    def __notified (self):
        self.pos = self.model.pos
        self.update ()

    def draw (self):
        self.reset ()
        if self.pos:
            self.trans_translate (self.pos)
            self.draw_circle ((0, 0), self.model.radius, fill = '#31aa23')
            self.draw_circle ((0, 0), self.model.radius + self.MARGIN,
                    outlinestipple = 'gray25')
            Drawable.draw (self)

