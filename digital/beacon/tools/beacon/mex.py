# lol - Laser Opponent Location finding system. {{{
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
"""Mex interface to simulated beacon."""

import simu.mex.msg

POSITION_NB = 2

class Mex:
    """Handle communications with stub."""

    class Position:
        """Send a position to the stub."""

        def __init__ (self, node, instance, index):
            self.__node = node
            self.__mtype = node.reserve (instance + ':position')
            self.__index = index
            self.pos = None

        def register_to (self, pos):
            """Register to given observable position."""
            assert self.pos is None
            self.pos = pos
            self.pos.register (self.__update)

        def __update (self):
            """Called on position update."""
            if self.pos.pos is not None:
                m = simu.mex.msg.Msg (self.__mtype)
                m.push ('BHH', self.__index, int (self.pos.pos[0]), int (self.pos.pos[1]))
                self.__node.send (m)

    def __init__ (self, node, instance = 'beacon0'):
        self.position = [ self.Position (node, instance, i)
                for i in xrange (POSITION_NB) ]
