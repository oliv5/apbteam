# simu - Robot simulation. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
"""Robospierre clamp."""
from utils.observable import Observable
from simu.utils.trans_matrix import TransMatrix
from math import pi, cos, sin

class Slot:
    """Slot which can contain a pawn."""

    def __init__ (self, x, y, z, side, door_motor, contact):
        self.x = x
        self.y = y
        self.z = z
        self.side = side
        self.door_motor = door_motor
        self.contact = contact
        self.set_pawn (None)

    def set_pawn (self, pawn):
        self.pawn = pawn
        self.contact.state = pawn is None
        self.contact.notify ()

class Clamp (Observable):

    ELEVATION_STROKE = 120.0
    ELEVATION_MOTOR_STROKE = 120.0 * 5.0 / 6.0

    ROTATION_STROKE = pi
    ROTATION_MOTOR_STROKE = pi * 115.0 / 12.0

    CLAMPING_STROKE = 10
    CLAMPING_MOTOR_STROKE = pi

    BAY_OFFSET = 150
    BAY_ZOFFSET = 60

    SLOT_FRONT_BOTTOM = 0
    SLOT_FRONT_MIDDLE = 1
    SLOT_FRONT_TOP = 2
    SLOT_BACK_BOTTOM = 3
    SLOT_BACK_MIDDLE = 4
    SLOT_BACK_TOP = 5
    SLOT_SIDE = 6

    def __init__ (self, table, robot_position, elevation_motor,
            rotation_motor, clamping_motor, door_motors, slot_contacts):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.elevation_motor = elevation_motor
        self.rotation_motor = rotation_motor
        self.clamping_motor = clamping_motor
        self.door_motors = (door_motors[0], None, door_motors[1],
                door_motors[2], None, door_motors[3], None)
        self.slots = (
                Slot (self.BAY_OFFSET, 0, 0 * self.BAY_ZOFFSET, 0,
                    door_motors[0], slot_contacts[0]),
                Slot (self.BAY_OFFSET, 0, 1 * self.BAY_ZOFFSET, 0,
                    None, slot_contacts[1]),
                Slot (self.BAY_OFFSET, 0, 2 * self.BAY_ZOFFSET, 0,
                    door_motors[1], slot_contacts[2]),
                Slot (-self.BAY_OFFSET, 0, 0 * self.BAY_ZOFFSET, 1,
                    door_motors[2], slot_contacts[3]),
                Slot (-self.BAY_OFFSET, 0, 1 * self.BAY_ZOFFSET, 1,
                    None, slot_contacts[4]),
                Slot (-self.BAY_OFFSET, 0, 2 * self.BAY_ZOFFSET, 1,
                    door_motors[3], slot_contacts[5]),
                Slot (0, self.BAY_OFFSET, 2 * self.BAY_ZOFFSET, None,
                    None, slot_contacts[6]))
        self.load = None
        self.robot_position.register (self.__robot_position_notified)
        self.elevation_motor.register (self.__elevation_notified)
        self.rotation_motor.register (self.__rotation_notified)
        self.clamping_motor.register (self.__clamping_notified)

    def __robot_position_notified (self):
        # Update bottom slots.
        changed = False
        for sloti in (self.SLOT_FRONT_BOTTOM, self.SLOT_BACK_BOTTOM):
            slot = self.slots[sloti]
            if slot.pawn is None:
                p = self.__get_floor_elements (slot.side)
                if p is not None:
                    slot.set_pawn (p)
                    p.pos = None
                    p.notify ()
                    changed = True
        if changed:
            self.notify ()

    def __elevation_notified (self):
        if self.elevation_motor.angle is None:
            self.elevation = None
        else:
            # Update elevation.
            self.elevation = (self.elevation_motor.angle
                    * self.ELEVATION_STROKE / self.ELEVATION_MOTOR_STROKE)
        self.notify ()

    def __rotation_notified (self):
        if self.rotation_motor.angle is None:
            self.rotation = None
        else:
            # Update rotation.
            self.rotation = (self.rotation_motor.angle * self.ROTATION_STROKE
                    / self.ROTATION_MOTOR_STROKE)
        self.notify ()

    def __clamping_notified (self):
        if self.clamping_motor.angle is None:
            self.clamping = None
        else:
            # Update clamping.
            self.clamping = (self.clamping_motor.angle * self.CLAMPING_STROKE
                    / self.CLAMPING_MOTOR_STROKE)
            if self.clamping == 0 and self.load is None:
                # Load an element.
                slot = self.__get_clamp_slot ()
                if slot and slot.pawn is not None:
                    self.load = slot.pawn
                    slot.set_pawn (None)
            elif self.clamping == self.CLAMPING_STROKE \
                    and self.load is not None:
                # Unload an element.
                slot = self.__get_clamp_slot ()
                if slot and slot.pawn is None:
                    slot.set_pawn (self.load)
                    self.load = None
        self.notify ()

    def __get_floor_elements (self, side):
        """Return an elements in front (side = 0) or in back (side = 1) of the
        robot, on the floor."""
        if self.robot_position.pos is None:
            return None
        # Matrix to transform an obstacle position into robot coordinates.
        m = TransMatrix ()
        m.rotate (-self.robot_position.angle)
        m.translate ((-self.robot_position.pos[0],
            -self.robot_position.pos[1]))
        # Look up elements.
        xoffset = (self.BAY_OFFSET, -self.BAY_OFFSET)[side]
        xmargin = 20
        ymargin = 50
        for o in self.table.obstacles:
            if o.level == 1 and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > xoffset - xmargin
                        and pos[0] < xoffset + xmargin
                        and pos[1] > -ymargin and pos[1] < ymargin):
                    return o
        return None

    def __get_clamp_slot (self):
        """Return the slot in which the clamp is."""
        if self.rotation is None or self.elevation is None:
            return None
        margin = 10
        x = cos (self.rotation) * self.BAY_OFFSET
        y = sin (self.rotation) * self.BAY_OFFSET
        for slot in self.slots:
            if abs (slot.z - self.elevation) < margin \
                    and abs (slot.x - x) < margin \
                    and abs (slot.y - y) < margin:
                return slot
        return None

