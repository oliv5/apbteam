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
import asserv.mex
from simu.link.mex_gpio import MexGpio

class Bag:

    def __init__ (self, node, instance = 'robot0'):
        self.asserv = asserv.mex.Mex (node, '%s:asserv0' % instance,
                aux_nb = 0)
        self.io_hub = io_hub.mex.Mex (node, '%s:io0' % instance, gpios = True)
        gpios = ('raw_jack', 'ihm_color', 'ihm_strat', 'ihm_robot_nb',
                'ihm_lol', 'ihm_emerg_stop', 'glass_contact',
                'cherry_plate_left_contact', 'cherry_plate_right_contact',
                'cherry_bad_out', 'cherry_bad_in', 'cherry_plate_up',
                'cherry_plate_down', 'cherry_plate_clamp', 'cake_arm_out',
                'cake_arm_in', 'cake_push_far_out', 'cake_push_far_in',
                'cake_push_near_out', 'cake_push_near_in',
                'glass_lower_clamp_close', 'glass_lower_clamp_open',
                'glass_upper_clamp_close', 'glass_upper_clamp_open',
                'glass_upper_clamp_up', 'glass_upper_clamp_down', 'gift_out',
                'gift_in', 'ballon_funny_action', 'pneum_open')
        for gpio in gpios:
            setattr (self, gpio, MexGpio (self.io_hub.gpios, gpio))
