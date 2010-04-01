# simu - Robot simulation. {{{
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
"""Marcel bag of models."""
from simu.model.switch import Switch
from simu.model.position import Position
from simu.model.distance_sensor_sensopart import DistanceSensorSensopart
from math import pi

class Bag:

    def __init__ (self, scheduler, table, link_bag):
        self.jack = Switch (link_bag.io.jack)
        self.color_switch = Switch (link_bag.io.color_switch)
        self.contact = [ Switch (contact) for contact in link_bag.io.contact ]
        self.position = Position (link_bag.asserv.position)
        self.distance_sensor = [
                DistanceSensorSensopart (link_bag.io.adc[0], scheduler, table,
                    (30 - 20, 0), 0, (self.position, )),
                DistanceSensorSensopart (link_bag.io.adc[1], scheduler, table,
                    (20 - 20, 20), pi * 40 / 180, (self.position, )),
                DistanceSensorSensopart (link_bag.io.adc[2], scheduler, table,
                    (20 - 20, -20), -pi * 40 / 180, (self.position, )),
                DistanceSensorSensopart (link_bag.io.adc[3], scheduler, table,
                    (-30 - 20, 0), pi, (self.position, )),
                ]
        link_bag.io.adc[4].value = 0
        link_bag.io.adc[5].value = 0
        self.path = link_bag.io.path

