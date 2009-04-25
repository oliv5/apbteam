# asserv - Position & speed motor control on AVR. {{{
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
"""Mex interface to asserv."""

from utils.observable import Observable

ID_POSITION = 0xa0
ID_PWM = 0xa1
ID_AUX = 0xa8

class Mex:
    """Handle communications with simulated asserv."""

    class Position (Observable):
        """Robot position.

        - pos: (x, y) millimeters.
        - angle: radian.

        """

        def __init__ (self, node):
            Observable.__init__ (self)
            self.pos = None
            self.angle = None
            node.register (ID_POSITION, self.__handle)

        def __handle (self, msg):
            x, y, a = msg.pop ('hhl')
            self.pos = (x, y)
            self.angle = float (a) / 1024
            self.notify ()

    class PWM (Observable):
        """Motor PWM.

        - pwm: current PWM values (hardware unit).

        """

        def __init__ (self, node):
            Observable.__init__ (self)
            self.pwm = None
            node.register (ID_PWM, self.__handle)

        def __handle (self, msg):
            self.pwm = msg.pop ('hhhh')
            self.notify ()

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

    def __init__ (self, node):
        self.position = self.Position (node)
        self.pwm = self.PWM (node)
        self.aux = (self.Aux (), self.Aux ())
        self.__aux_pack = self.Aux.Pack (node, self.aux)

