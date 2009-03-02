# io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
"""Mex interface to io."""

from utils.observable import Observable

ID_JACK = 0xb0
ID_COLOR = 0xb1
ID_SERVO = 0xb2
ID_ADC = 0xb3
ID_PATH = 0xb4

SERVO_NB = 6
SERVO_VALUE_MAX = 255

ADC_NB = 5

class Mex:
    """Handle communications with simulated io."""

    class Switch (Observable):
        """Switch input.

        - state: True (open) or False (closed).

        """

        def __init__ (self, node, id):
            Observable.__init__ (self)
            self.__node = node
            self.state = None
            node.register (id, self.__handle)

        def __handle (self, msg):
            assert self.state is not None
            m = Msg (msg.mtype)
            m.push ('B', self.state)
            self.__node.response (m)

    class Servo (Observable):
        """Servo motor.

        - value: current servo position (hardware unit).

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.value = None

        class Pack:
            """Handle reception of several Servo for one message."""

            def __init__ (self, node, list):
                self.__list = list
                node.register (ID_SERVO, self.__handle)

            def __handle (self, msg):
                values = msg.pop ('%dB' % len (self.__list))
                for servo, value in zip (self.__list, values):
                    servo.value = float (value) / SERVO_VALUE_MAX
                    servo.notify ()

    class ADC (Observable):
        """Analog to digital input."

        - value: hardware unit.

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.value = None

        class Pack:
            """Handle transmission of several ADC for one message."""

            def __init__ (self, node, list):
                self.__node = node
                self.__list = list
                node.register (ID_ADC, self.__handle)

            def __handle (self, msg):
                m = Msg (msg.mtype)
                for adc in self.__list:
                    assert adc.value is not None
                    m.push ('H', adc.value)
                self.__node.response (m)

    class Path (Observable):
        """Path finding algorithm report.

        - path: sequence of (x, y) coordinates (millimeters).

        """

        def __init__ (self, node):
            Observable.__init__ (self)
            self.path = [ ]
            node.register (ID_PATH, self.__handle)

        def __handle (self, msg):
            self.path = [ ]
            while len (msg) >= 4:
                self.path.append (msg.pop ('hh'))
            self.notify ()

    def __init__ (self, node):
        self.jack = self.Switch (node, ID_JACK)
        self.color_switch = self.Switch (node, ID_COLOR)
        self.servo = tuple (self.Servo () for i in range (0, SERVO_NB))
        self.__servo_pack = self.Servo.Pack (node, self.servo)
        self.adc = tuple (self.ADC () for i in range (0, ADC_NB))
        self.__adc_pack = self.ADC.Pack (node, self.adc)
        self.path = self.Path (node)

