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

    def __init__ (self, table,
            arm_motor_link, cylinder_puck_contact,
            servo_links, bridge_puck_contact,
            elevator_motor_link, elevator_door_model, elevator_door_contact,
            into = None):
        Observable.__init__ (self)
        self.table = table
        self.into = into or ()
        self.arm_slot = [ None, None, None ]
        self.bridge_slot = [ None, None ]
        self.elevator_slot = [ ]
        self.lost = [ ]
        self.arm_motor_link = arm_motor_link
        self.arm_motor_link.register (self.__arm_motor_notified)
        self.__arm_motor_notified ()
        self.cylinder_puck_contact = cylinder_puck_contact
        self.servo_links = servo_links
        self.bridge_puck_contact = bridge_puck_contact
        self.servo_links[0].register (self.__bridge_door_servo_notified)
        self.__bridge_door_servo_notified ()
        self.servo_links[1].register (self.__bridge_finger_servo_notified)
        self.__bridge_finger_servo_notified ()
        self.elevator_motor_link = elevator_motor_link
        self.elevator_motor_link.register (self.__elevator_motor_notified)
        self.__elevator_motor_notified ()
        self.elevator_door = elevator_door_model
        self.elevator_door_contact = elevator_door_contact
        self.elevator_door.register (self.__elevator_door_notified)
        self.__elevator_door_notified ()

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
                # Update puck cylinder contact.
                self.cylinder_puck_contact.state = front_puck is None
                self.cylinder_puck_contact.notify ()
                # For each arm.
                for i in range (3):
                    a = (self.arm_angle + i * 2 * pi / 3) % (2 * pi)
                    # If arm is leaving, take puck.
                    if (front_puck is not None and self.arm_slot[i] is None
                            and a > 0 and a < pi / 16):
                        self.arm_slot[i] = front_puck
                        front_puck.pos = None
                        front_puck.notify ()
                    # If arm is up, pass puck to bridge.
                    if (self.arm_slot[i] is not None
                            and a > pi and a < 17 * pi / 16):
                        if self.bridge_slot[0] is None:
                            self.bridge_slot[0] = self.arm_slot[i]
                        else:
                            self.lost.append (self.arm_slot[i])
                        self.arm_slot[i] = None
        self.notify ()

    def __bridge_door_servo_notified (self):
        self.bridge_door_servo_value = self.servo_links[0].value
        if self.bridge_door_servo_value is not None:
            if (self.bridge_door_servo_value < 0.1
                    and self.bridge_slot[0] is not None
                    and self.bridge_slot[1] is None):
                # Pass the door.
                self.bridge_slot[1] = self.bridge_slot[0]
                self.bridge_slot[0] = None
        self.__bridge_puck_update ()
        self.notify ()

    def __bridge_finger_servo_notified (self):
        self.bridge_finger_servo_value = self.servo_links[1].value
        if (self.bridge_finger_servo_value is not None
                and self.bridge_door_servo_value is not None):
            if (self.bridge_door_servo_value > 0.9
                    and self.bridge_finger_servo_value > 0.5
                    and self.bridge_slot[1] is not None):
                # Pass to elevator.
                self.elevator_slot.append (self.bridge_slot[1])
                self.bridge_slot[1] = None
        self.__bridge_puck_update ()
        self.notify ()

    def __bridge_puck_update (self):
        # Update bridge puck contact.
        self.bridge_puck_contact.state = self.bridge_slot[1] is None
        self.bridge_puck_contact.notify ()

    def __elevator_motor_notified (self):
        if self.elevator_motor_link.angle is None:
            self.elevator_height = None
        else:
            self.elevator_height = 150 - self.elevator_motor_link.angle * 5.5
        self.notify ()

    def __elevator_door_notified (self):
        # Contact.
        self.elevator_door_contact.state = (self.elevator_door.angle
                != self.elevator_door.max_stop)
        self.elevator_door_contact.notify ()
        # Puck exit.
        if (self.elevator_door.angle == self.elevator_door.max_stop
                and self.elevator_slot):
            exit_pos = self.__transform ((-150, 0))
            if exit_pos is not None:
                for i in range (len (self.elevator_slot)):
                    self.elevator_slot[i].pos = exit_pos
                    self.elevator_slot[i].notify ()
                    exit_pos = (exit_pos[0] + 2, exit_pos[1] + 2)
                self.elevator_slot = [ ]

