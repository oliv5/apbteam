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
from simu.model.distance_sensor_trig import DistanceSensorTrig
from simu.model.distance_sensor_sensopart import DistanceSensorSensopart
from simu.model.pneumatic_cylinder import PneumaticCylinder
from simu.robots.apbirthday.model.cake_arm import CakeArm
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
                    (102, 84), 0, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[1], scheduler, table,
                    (102, -84), 0, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[2], scheduler, table,
                    (-78, 104), pi, (self.position, ), 4),
                DistanceSensorSensopart (link_bag.adc_dist[3], scheduler, table,
                    (-83, -120), pi, (self.position, ), 4),
                ]
        self.cake_front = DistanceSensorTrig (link_bag.adc_cake_front,
                scheduler, table, (80, 136), pi / 2, (self.position, ), 0)
        self.cake_back = DistanceSensorTrig (link_bag.adc_cake_back,
                scheduler, table, (-66, 139), pi / 2, (self.position, ), 0)
        self.cake_arm = CakeArm (table, self.position,
                PneumaticCylinder (
                    link_bag.cake_arm_in,
                    link_bag.cake_arm_out,
                    scheduler, 0., 1., 1., 1., 1.),
                PneumaticCylinder (
                    link_bag.cake_push_far_in,
                    link_bag.cake_push_far_out,
                    scheduler, 0., 1., 10., 10., 0.),
                PneumaticCylinder (
                    link_bag.cake_push_near_in,
                    link_bag.cake_push_near_out,
                    scheduler, 0., 1., 10., 10., 0.),
                link_bag.cake_arm_out_contact, link_bag.cake_arm_in_contact)
        self.pos_report = link_bag.io_hub.pos_report

