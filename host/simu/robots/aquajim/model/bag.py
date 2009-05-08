# simu - Robot simulation. {{{
#
# Copyright (C) 2009 Nicolas Schodet
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
"""AquaJim bag of models."""
from simu.model.switch import Switch
from simu.model.position import Position
from simu.model.motor_basic import MotorBasic
from simu.model.distance_sensor_sharps import DistanceSensorSharps
from simu.robots.aquajim.model.sorter import Sorter
from math import pi

class Bag:

    def __init__ (self, scheduler, table, link_bag):
        self.jack = Switch (link_bag.io.jack)
        self.color_switch = Switch (link_bag.io.color_switch)
        self.position = Position (link_bag.asserv.position)
        self.elevator_door = MotorBasic (link_bag.io.pwm[0], scheduler,
                2 * pi, 0, pi / 2)
        self.sorter = Sorter (table, link_bag.asserv.aux[0],
                link_bag.asserv.aux[1], link_bag.io.servo[0:2],
                self.elevator_door)
        self.distance_sensor = [
                DistanceSensorSharps (link_bag.io.adc[0], scheduler, table,
                    (50, 130), 0, (self.position, )),
                DistanceSensorSharps (link_bag.io.adc[1], scheduler, table,
                    (50, -130), 0, (self.position, )),
                DistanceSensorSharps (link_bag.io.adc[2], scheduler, table,
                    (50, 0), 0, (self.position, )),
                DistanceSensorSharps (link_bag.io.adc[3], scheduler, table,
                    (0, 130), pi, (self.position, )),
                DistanceSensorSharps (link_bag.io.adc[4], scheduler, table,
                    (0, -130), pi, (self.position, )),
                ]
        self.path = link_bag.io.path

