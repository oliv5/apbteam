# simu - Robot simulation. {{{
#
# Copyright (C) 2012 Nicolas Schodet
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
"""Guybrush bag of models."""
from simu.model.switch import Switch
from simu.model.position import Position
from simu.model.round_obstacle import RoundObstacle
from simu.model.distance_sensor_sensopart import DistanceSensorSensopart
from simu.model.pneumatic_cylinder import PneumaticCylinder
from simu.robots.guybrush.model.clamps import Clamps
from math import pi
import random

class Bag:

    def __init__ (self, scheduler, table, link_bag):
        self.color_switch = Switch (link_bag.io_hub.contact[0], invert = True)
        self.color_switch.state = random.choice ((False, True))
        self.color_switch.notify ()
        self.jack = Switch (link_bag.io_hub.contact[1], invert = True)
        self.strat_switch = Switch (link_bag.io_hub.contact[2], invert = True)
        self.beacon = RoundObstacle (40, 5)
        table.obstacles.append (self.beacon)
        self.position = Position (link_bag.asserv.position, [ self.beacon ])
        output = link_bag.io_hub.output
        contact = [ Switch (c) for c in link_bag.io_hub.contact[3:] ]
        self.clamps = Clamps (table, self.position, link_bag.mimot.aux[0],
                (PneumaticCylinder (None, output[8], scheduler,
                    0., 30., 150., 75., 30.),
                PneumaticCylinder (None, output[9], scheduler,
                    0., 30., 150., 75., 30.)),
                contact[0:4],
                PneumaticCylinder (output[4], output[5], scheduler,
                    0., 1., 1., 1., 1., contact[5], contact[6]),
                PneumaticCylinder (output[3], output[2], scheduler,
                    0., 1., 1., 1., 0.),
                PneumaticCylinder (None, output[1], scheduler,
                    0., 30., 150., 75., 30.))
        def distance_sensor_exclude (o):
            return o is self.beacon
        self.distance_sensor = [
                DistanceSensorSensopart (link_bag.io_hub.adc[0], scheduler, table,
                    (20, 20), pi * 10 / 180, (self.position, ), 5,
                    distance_sensor_exclude),
                DistanceSensorSensopart (link_bag.io_hub.adc[1], scheduler, table,
                    (20, -20), -pi * 10 / 180, (self.position, ), 5,
                    distance_sensor_exclude),
                DistanceSensorSensopart (link_bag.io_hub.adc[2], scheduler, table,
                    (-20, -20), pi + pi * 10 / 180, (self.position, ), 5,
                    distance_sensor_exclude),
                DistanceSensorSensopart (link_bag.io_hub.adc[3], scheduler, table,
                    (-20, 20), pi - pi * 10 / 180, (self.position, ), 5,
                    distance_sensor_exclude),
                ]
        self.path = link_bag.io_hub.path
        self.pos_report = link_bag.io_hub.pos_report

