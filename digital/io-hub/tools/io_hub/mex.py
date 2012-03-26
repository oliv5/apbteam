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

PWM_VALUE_MAX = 1024

CONTACT_INIT = 0xffffffff

class Mex:
    """Handle communications with simulated io-hub."""

    class ADC (Observable):
        """Analog to digital input."

        - value: volts.

        """

        def __init__ (self, node, instance, index):
            Observable.__init__ (self)
            self.value = 0
            self.__node = node
            self.__mtype = node.reserve (instance + ':adc')
            self.__index = index
            self.register (self.__notified)

        def __notified (self):
            m = simu.mex.msg.Msg (self.__mtype)
            v = int (1024 * self.value / 5)
            v = min (v, 1023)
            v = max (0, v)
            m.push ('BH', self.__index, v)
            self.__node.send (m)

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

    class Output (Observable):
        """Simple output.

        - state: True (1) or False (0).

        """

        def __init__ (self):
            Observable.__init__ (self)
            self.state = None

        class Pack:
            """Handle reception of several output for one message."""

            def __init__ (self, node, instance, list):
                self.__list = list
                node.register (instance + ':output', self.__handle)

            def __handle (self, msg):
                mask, = msg.pop ('L')
                for index, output in enumerate (self.__list):
                    new = (False, True)[(mask >> index) & 1]
                    if new != output.state:
                        output.state = new
                        output.notify ()

    class Codebar (Observable):
        """Codebar stub.

        - element_type: 'queen', 'king', or anything else.

        """

        def __init__ (self, pack, index):
            Observable.__init__ (self)
            self.pack = pack
            self.index = index
            self.element_type = None
            self.register (self.__notified)

        def __notified (self):
            self.pack.set (self.index, self.element_type)

        class Pack:
            """Handle emission of several codebar for one message."""

            def __init__ (self, node, instance):
                self.node = node
                self.codebars = [0, 0]
                self.mtype = node.reserve (instance + ':codebar')

            def set (self, index, element_type):
                if element_type == 'queen':
                    self.codebars[index] = 4
                elif element_type == 'king':
                    self.codebars[index] = 8
                else:
                    self.codebars[index] = 0
                self.__send ()

            def __send (self):
                m = simu.mex.msg.Msg (self.mtype)
                for c in self.codebars:
                    m.push ('b', c)
                self.node.send (m)

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

    def __init__ (self, node, instance = 'io-hub0',
            pwm_nb = 0, contact_nb = 0, output_nb = 0, codebar = False):
        self.adc = tuple (self.ADC (node, instance, i) for i in range (0, ADC_NB))
        if pwm_nb:
            self.pwm = tuple (self.PWM () for i in range (0, pwm_nb))
            self.__pwm_pack = self.PWM.Pack (node, instance, self.pwm)
        if contact_nb:
            self.__contact_pack = self.Contact.Pack (node, instance)
            self.contact = tuple (self.Contact (self.__contact_pack, i)
                    for i in range (contact_nb))
        if output_nb:
            self.output = tuple (self.Output () for i in range (0, output_nb))
            self.__output_pack = self.Output.Pack (node, instance,
                    self.output)
        if codebar:
            self.__codebar_pack = self.Codebar.Pack (node, instance)
            self.codebar = tuple (self.Codebar (self.__codebar_pack, i)
                    for i in (0, 1))
        self.path = self.Path (node, instance)
        self.pos_report = self.PosReport (node, instance)

