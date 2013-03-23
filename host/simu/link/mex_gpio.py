# simu - Robot simulation. {{{
#
# Copyright (C) 2013 Nicolas Schodet
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
"""MEX GPIO link."""
from utils.observable import Observable
import mex.msg

class MexGpio (Observable):
    """General purpose Input/Output.

    - state: input or output state, depending on direction.
    - direction: 'input' or 'output', from simulated board.

    """

    def __init__ (self, pack, name):
        Observable.__init__ (self)
        self.pack = pack
        assert name not in self.pack.gpios
        self.pack.gpios[name] = self
        self.name = name
        self.direction = 'input'
        self.output_state = None
        self.input_state = None
        self.state = self.input_state
        self.register (self.__notified)

    def __notified (self):
        self.pack.send_input (self)

    class Pack:
        """Centralise GPIO handling."""

        def __init__ (self, node, instance):
            self.node = node
            self.input_mtype = node.reserve (instance + ':gpio_in')
            output_mtype = node.register (instance + ':gpio_out',
                    self.__handle_output)
            self.gpios = { }

        def send_input (self, gpio):
            if gpio.direction == 'input':
                gpio.input_state = gpio.state
            state = gpio.input_state
            if state is not None:
                m = mex.msg.Msg (self.input_mtype)
                namelen = len (gpio.name)
                m.push ('B%ds' % namelen, state, gpio.name)
                self.node.send (m)

        def __handle_output (self, msg):
            namelen = len (msg) - 2
            direction, output, name = msg.pop ('BB%ds' % namelen)
            gpio = self.gpios[name]
            gpio.direction = ('input', 'output')[direction]
            if direction:
                gpio.output_state = output != 0
                gpio.state = gpio.output_state
                assert gpio.input_state is None
            gpio.notify ()

