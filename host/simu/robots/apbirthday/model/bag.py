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
"""APBirthday bag of models."""
from simu.model.switch import Switch
from simu.model.position import Position
from simu.model.round_obstacle import RoundObstacle
from simu.model.distance_sensor_sensopart import DistanceSensorSensopart
from math import pi
import random

class Bag:

    def __init__ (self, scheduler, table, link_bag):
        self.color_switch = Switch (link_bag.ihm_color, invert = True)
        self.color_switch.state = random.choice ((False, True))
        self.color_switch.notify ()
        self.jack = Switch (link_bag.raw_jack, invert = True)
        self.beacon = RoundObstacle (40, 5)
        table.obstacles.append (self.beacon)
        self.position = Position (link_bag.asserv.position, [ self.beacon ])
        self.distance_sensor = [
                DistanceSensorSensopart (link_bag.adc_dist[0], scheduler, table,
                    (100, 120), 0, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[1], scheduler, table,
                    (100, -120), 0, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[2], scheduler, table,
                    (-50, 120), pi, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[3], scheduler, table,
                    (-50, -120), pi, (self.position, ), 4),
                ]

