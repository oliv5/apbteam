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
"""MEX ADC link."""
from utils.observable import Observable
import mex.msg

class MexAdcChannel (Observable):
    """Analog to Digital Converter channel.

    - value: input value, in range [0.0, 1.0].

    """

    def __init__ (self, pack, name):
        Observable.__init__ (self)
        self.pack = pack
        assert name not in self.pack.channels
        self.pack.channels[name] = self
        self.name = name
        self.value = 0.0
        self.register (self.__notified)

    def __notified (self):
        self.pack.send (self)

    class Pack:
        """Centralise ADC handling."""

        def __init__ (self, node, instance):
            self.node = node
            self.mtype = node.reserve (instance + ':adc_channel')
            self.channels = { }

        def send (self, channel):
            ivalue = int ((1 << 31) * channel.value)
            ivalue = min (ivalue, (1 << 31) - 1)
            ivalue = max (ivalue, 0)
            m = mex.msg.Msg (self.mtype)
            namelen = len (channel.name)
            m.push ('l%ds' % namelen, ivalue, channel.name)
            self.node.send (m)

