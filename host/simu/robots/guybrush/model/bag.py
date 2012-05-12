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

    OUTPUT_UPPER_CLAMP_OPEN = 1
    OUTPUT_UPPER_CLAMP_OUT = 2
    OUTPUT_UPPER_CLAMP_IN = 3
    OUTPUT_UPPER_CLAMP_DOWN = 4
    OUTPUT_UPPER_CLAMP_UP = 5
    OUTPUT_DOOR_OPEN = 6
    OUTPUT_DOOR_CLOSE = 7
    OUTPUT_LOWER_CLAMP_1_CLOSE = 8
    OUTPUT_LOWER_CLAMP_2_CLOSE = 9

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
                (PneumaticCylinder (output[self.OUTPUT_LOWER_CLAMP_1_CLOSE],
                    None, scheduler, 0., 30., 150., 75., 30.),
                PneumaticCylinder (output[self.OUTPUT_LOWER_CLAMP_2_CLOSE],
                    None, scheduler, 0., 30., 150., 75., 30.)),
                contact[0:4], contact[10],
                PneumaticCylinder (output[self.OUTPUT_UPPER_CLAMP_DOWN],
                    output[self.OUTPUT_UPPER_CLAMP_UP], scheduler,
                    0., 1., 1., 1., 1., contact[4], contact[5]),
                PneumaticCylinder (output[self.OUTPUT_UPPER_CLAMP_IN],
                    output[self.OUTPUT_UPPER_CLAMP_OUT], scheduler,
                    0., 1., 1., 1., 0.),
                PneumaticCylinder (None, output[self.OUTPUT_UPPER_CLAMP_OPEN],
                    scheduler, 0., 30., 150., 75., 30.),
                PneumaticCylinder (output[self.OUTPUT_DOOR_CLOSE],
                    output[self.OUTPUT_DOOR_OPEN], scheduler,
                    0., 1., 1., 1., 1., contact[7], contact[6])
                )
        self.distance_sensor = [
                DistanceSensorSensopart (link_bag.io_hub.adc[0], scheduler, table,
                    (120, 0), 0, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.io_hub.adc[1], scheduler, table,
                    (120, 160), 0, (self.position, ), set ([3, 4])),
                DistanceSensorSensopart (link_bag.io_hub.adc[2], scheduler, table,
                    (120, -160), 0, (self.position, ), set ([3, 4])),
                DistanceSensorSensopart (link_bag.io_hub.adc[3], scheduler, table,
                    (-130, 0), pi, (self.position, ), 4),
                ]
        self.path = link_bag.io_hub.path
        self.pos_report = link_bag.io_hub.pos_report
        self.debug_draw = link_bag.io_hub.debug_draw

