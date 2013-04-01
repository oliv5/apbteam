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
"""APBirthday bag of links."""
import io_hub.mex
import io_hub.apbirthday
import asserv.mex
from simu.link.mex_gpio import MexGpio
from simu.link.mex_adc_channel import MexAdcChannel

class Bag:

    def __init__ (self, node, instance = 'robot0'):
        self.asserv = asserv.mex.Mex (node, '%s:asserv0' % instance,
                aux_nb = 0)
        self.io_hub = io_hub.mex.Mex (node, '%s:io0' % instance, gpios = True,
                adc_channels = True)
        for gpio in io_hub.apbirthday.gpios:
            setattr (self, gpio, MexGpio (self.io_hub.gpios, gpio))
        adc_channels = self.io_hub.adc_channels
        self.adc_dist = [
                MexAdcChannel (adc_channels, 'dist0'),
                MexAdcChannel (adc_channels, 'dist1'),
                MexAdcChannel (adc_channels, 'dist2'),
                MexAdcChannel (adc_channels, 'dist3'),
                ]
        self.adc_cake_front = MexAdcChannel (adc_channels, 'cake_front')
        self.adc_cake_back = MexAdcChannel (adc_channels, 'cake_back')
