# io-hub - Modular Input/Output. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
"""Mex interface to io-hub."""

from utils.observable import Observable
import simu.mex.msg

ADC_NB = 8

PWM_NB = 6
PWM_VALUE_MAX = 1024

CONTACT_NB = 4
CONTACT_INIT = 0xffffffff

class Mex:
    """Handle communications with simulated io-hub."""

    class ADC (Observable):
        """Analog to digital input."

        - value: volts.

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.value = None

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
                m.push ('L', self.contacts)
                self.node.send (m)

    def __init__ (self, node, instance = 'io-hub0'):
        self.adc = tuple (self.ADC () for i in range (0, ADC_NB))
        self.pwm = tuple (self.PWM () for i in range (0, PWM_NB))
        self.__adc_pwm = self.PWM.Pack (node, instance, self.pwm)
        self.__contact_pack = self.Contact.Pack (node, instance)
        self.contact = tuple (self.Contact (self.__contact_pack, i)
                for i in range (CONTACT_NB))

