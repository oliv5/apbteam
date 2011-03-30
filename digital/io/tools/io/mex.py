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
import simu.mex.msg

SERVO_NB = 6
SERVO_VALUE_MAX = 255

ADC_NB = 6

PWM_NB = 1
PWM_VALUE_MAX = 1024

CONTACT_NB = 3
CONTACT_INIT = 0x3f

class Mex:
    """Handle communications with simulated io."""

    class Switch (Observable):
        """Switch input.

        - state: True (open) or False (closed).

        """

        def __init__ (self, node, mtype):
            Observable.__init__ (self)
            self.__node = node
            self.state = None
            node.register (mtype, self.__handle)

        def __handle (self, msg):
            assert self.state is not None
            m = msg
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

            def __init__ (self, node, instance, list):
                self.__list = list
                node.register (instance + ':servo', self.__handle)

            def __handle (self, msg):
                values = msg.pop ('%dB' % len (self.__list))
                for servo, value in zip (self.__list, values):
                    servo.value = float (value) / SERVO_VALUE_MAX
                    servo.notify ()

    class ADC (Observable):
        """Analog to digital input."

        - value: volts.

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.value = None

        class Pack:
            """Handle transmission of several ADC for one message."""

            def __init__ (self, node, instance, list):
                self.__node = node
                self.__list = list
                node.register (instance + ':adc', self.__handle)

            def __handle (self, msg):
                m = msg
                for adc in self.__list:
                    assert adc.value is not None
                    v = int (1024 * adc.value / 5)
                    v = min (v, 1023)
                    v = max (0, v)
                    m.push ('H', v)
                self.__node.response (m)

    class Path (Observable):
        """Path finding algorithm report.

        - path: sequence of (x, y) coordinates (millimeters).

        """

        def __init__ (self, node, instance):
            Observable.__init__ (self)
            self.path = [ ]
            node.register (instance + ':path', self.__handle)

        def __handle (self, msg):
            self.path = [ ]
            while len (msg) >= 4:
                self.path.append (msg.pop ('hh'))
            self.notify ()

    class PWM (Observable):
        """PWM output.

        - value: current PWM value (-1 ... +1).

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.value = None

        class Pack:
            """Handle reception of several PWM for one message."""

            def __init__ (self, node, instance, list):
                self.__list = list
                node.register (instance + ':pwm', self.__handle)

            def __handle (self, msg):
                values = msg.pop ('%dh' % len (self.__list))
                for pwm, value in zip (self.__list, values):
                    pwm.value = float (value) / PWM_VALUE_MAX
                    pwm.notify ()

    class Contact (Observable):
        """Contact input.

        - state: True (open) or False (closed).

        """

        def __init__ (self, pack, index):
            Observable.__init__ (self)
            self.pack = pack
            self.index = index
            self.state = None
            self.register (self.__notified)

        def __notified (self):
            self.pack.set (self.index, self.state)

        class Pack:
            """Handle emission of several contacts for one message."""

            def __init__ (self, node, instance):
                self.node = node
                self.contacts = CONTACT_INIT
                self.mtype = node.reserve (instance + ':contact')

            def set (self, index, state):
                if state is None or state:
                    self.contacts |= 1 << index
                else:
                    self.contacts &= ~(1 << index)
                self.__send ()

            def __send (self):
                m = simu.mex.msg.Msg (self.mtype)
                m.push ('B', self.contacts)
                self.node.send (m)

    class PosReport (Observable):
        """General purpose position report.

        - pos: dict of sequence of (x, y) coordinates (millimeters).  The dict
          is indexed by position identifier.

        """

        def __init__ (self, node, instance):
            Observable.__init__ (self)
            self.pos = { }
            node.register (instance + ':pos-report', self.__handle)

        def __handle (self, msg):
            p = [ ]
            id, = msg.pop ('b')
            while len (msg) >= 4:
                p.append (msg.pop ('hh'))
            self.pos[id] = p
            self.notify ()

    def __init__ (self, node, instance = 'io0'):
        self.jack = self.Switch (node, instance + ':jack')
        self.color_switch = self.Switch (node, instance + ':color')
        self.servo = tuple (self.Servo () for i in range (0, SERVO_NB))
        self.__servo_pack = self.Servo.Pack (node, instance, self.servo)
        self.adc = tuple (self.ADC () for i in range (0, ADC_NB))
        self.__adc_pack = self.ADC.Pack (node, instance, self.adc)
        self.path = self.Path (node, instance)
        self.pwm = tuple (self.PWM () for i in range (0, PWM_NB))
        self.__adc_pwm = self.PWM.Pack (node, instance, self.pwm)
        self.__contact_pack = self.Contact.Pack (node, instance)
        self.contact = tuple (self.Contact (self.__contact_pack, i)
                for i in range (CONTACT_NB))
        self.pos_report = self.PosReport (node, instance)

