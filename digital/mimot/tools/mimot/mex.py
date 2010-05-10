# mimot - Mini motor control, with motor driver. {{{
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
"""Mex interface to mimot."""

from utils.observable import Observable
import simu.mex.msg

ID_AUX = 0xc8
ID_LIMITS = 0xcc

class Mex:
    """Handle communications with simulated mimot."""

    class Aux (Observable):
        """Auxiliary motor angle.

        - angle: radian.

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.angle = None

        class Pack:
            """Handle reception of several Aux for one message."""

            def __init__ (self, node, list):
                self.__list = list
                node.register (ID_AUX, self.__handle)

            def __handle (self, msg):
                angles = msg.pop ('%dl' % len (self.__list))
                for aux, angle in zip (self.__list, angles):
                    aux.angle = float (angle) / 1024
                    aux.notify ()

    class Limits (Observable):
        """Motor limits.

        - min, max: limits in radian.

        """

        def __init__ (self, pack, index):
            Observable.__init__ (self)
            self.pack = pack
            self.index = index
            self.min = None
            self.max = None
            self.register (self.__notified)

        def __notified (self):
            self.pack.set (self.index, self.min, self.max)

        class Pack:
            """Handle emission of several limits for one message."""

            def __init__ (self, node):
                self.node = node
                self.limits = [ None, None, None, None ]

            def set (self, index, min, max):
                self.limits[index * 2] = min
                self.limits[index * 2 + 1] = max
                self.__send ()

            def __send (self):
                m = simu.mex.msg.Msg (ID_LIMITS)
                for l in self.limits:
                    if l is None:
                        li = -1
                    else:
                        li = int (l * 1024)
                    m.push ('l', li)
                self.node.send (m)

    def __init__ (self, node):
        self.aux = (self.Aux (), self.Aux ())
        self.__aux_pack = self.Aux.Pack (node, self.aux)
        self.__limits_pack = self.Limits.Pack (node)
        for index, aux in enumerate (self.aux):
            aux.limits = self.Limits (self.__limits_pack, index)

