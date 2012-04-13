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
"""Robot position model."""
from utils.observable import Observable

class Position (Observable):

    def __init__ (self, link, obstacles = None):
        Observable.__init__ (self)
        self.link = link
        self.obstacles = obstacles or [ ]
        self.link.register (self.__notified)

    def __notified (self):
        self.pos = self.link.pos
        self.angle = self.link.angle
        self.notify ()
        # Also update attached obstacles.
        for o in self.obstacles:
            o.pos = self.pos
            o.angle = self.angle
            o.notify ()

