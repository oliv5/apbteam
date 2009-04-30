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
"""Giboulee bag of views."""
from simu.view.switch import Switch
from simu.view.distance_sensor import DistanceSensor
from simu.robots.giboulee.view.robot import Robot
from simu.robots.giboulee.view.arm import Arm
from simu.robots.giboulee.view.sorter import Sorter

class Bag:

    def __init__ (self, table, actuator_view, sensor_frame, model_bag):
        self.jack = Switch (sensor_frame, model_bag.jack, 'Jack')
        self.color_switch = Switch (sensor_frame, model_bag.color_switch,
                'Color')
        self.robot = Robot (table, model_bag.position)
        self.arm = Arm (actuator_view.add_view (Arm.width, Arm.height),
                model_bag.arm)
        self.sorter = Sorter (actuator_view.add_view (Sorter.width,
            Sorter.height), model_bag.sorter)
        self.distance_sensor = [DistanceSensor (self.robot, ds)
                for ds in model_bag.distance_sensor]

