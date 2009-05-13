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
from simu.utils.trans_matrix import TransMatrix
from math import pi

class Sorter (Observable):

    def __init__ (self, table, arm_motor_link, elevator_motor_link,
            servo_links, elevator_door_model, into = None):
        Observable.__init__ (self)
        self.table = table
        self.into = into or ()
        self.arm_motor_link = arm_motor_link
        self.arm_motor_link.register (self.__arm_motor_notified)
        self.__arm_motor_notified ()
        self.elevator_motor_link = elevator_motor_link
        self.elevator_motor_link.register (self.__elevator_motor_notified)
        self.__elevator_motor_notified ()
        self.servo_links = servo_links
        self.elevator_door = elevator_door_model
        self.arm_slot = [ None, None, None ]

    def __transform (self, pos):
        m = TransMatrix ()
        for i in self.into:
            if i.pos is None:
                return None
            m.rotate (i.angle)
            m.translate (i.pos)
        return m.apply (pos)

    def __arm_motor_notified (self):
        # Update angle.
        self.arm_angle = self.arm_motor_link.angle
        if self.arm_angle is not None:
            # Is there a puck at entry?
            entry_pos = self.__transform ((150 - 40, 0))
            if entry_pos is not None:
                front_puck = self.table.nearest (entry_pos, level = 1, max = 35)
                # For each arm.
                for i in range (3):
                    a = (self.arm_angle + i * 2 * pi / 3) % (2 * pi)
                    # If arm is leaving, take puck.
                    if (front_puck is not None and self.arm_slot[i] is None
                            and a > 0 and a < pi / 16):
                        self.arm_slot[i] = front_puck
                        front_puck.pos = None
        self.notify ()

    def __elevator_motor_notified (self):
        if self.elevator_motor_link.angle is None:
            self.elevator_height = None
        else:
            self.elevator_height = 150 - self.elevator_motor_link.angle * 5.5
        self.notify ()

