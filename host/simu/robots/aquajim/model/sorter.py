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
"""AquaJim pucks sorter."""
from utils.observable import Observable
from math import pi

class Sorter (Observable):

    def __init__ (self, table, arm_motor_link, elevator_motor_link,
            servo_links, elevator_door_model):
        Observable.__init__ (self)
        self.table = table
        self.arm_motor_link = arm_motor_link
        self.arm_motor_link.register (self.__arm_motor_notified)
        self.__arm_motor_notified ()
        self.elevator_motor_link = elevator_motor_link
        self.elevator_motor_link.register (self.__elevator_motor_notified)
        self.__elevator_motor_notified ()
        self.servo_links = servo_links
        self.elevator_door = elevator_door_model

    def __arm_motor_notified (self):
        self.arm_angle = self.arm_motor_link.angle
        self.notify ()

    def __elevator_motor_notified (self):
        if self.elevator_motor_link.angle is None:
            self.elevator_height = None
        else:
            self.elevator_height = 150 - self.elevator_motor_link.angle * 5.5
        self.notify ()

