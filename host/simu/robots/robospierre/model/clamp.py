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
from simu.model.round_obstacle import RoundObstacle
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
        self.pawn = None

class Clamp (Observable):

    ELEVATION_STROKE = 120.0
    ELEVATION_MOTOR_STROKE = 120.0 * 5.0 / 6.0

    ROTATION_STROKE = pi
    ROTATION_MOTOR_STROKE = 2 * pi * 36.088 / 16

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
        self.front_slots = (
                self.slots[self.SLOT_FRONT_BOTTOM],
                self.slots[self.SLOT_FRONT_MIDDLE],
                self.slots[self.SLOT_FRONT_TOP])
        self.back_slots = (
                self.slots[self.SLOT_BACK_BOTTOM],
                self.slots[self.SLOT_BACK_MIDDLE],
                self.slots[self.SLOT_BACK_TOP])
        self.load = None
        self.robot_position.register (self.__robot_position_notified)
        self.elevation_motor.register (self.__elevation_notified)
        self.rotation_motor.register (self.__rotation_notified)
        self.clamping_motor.register (self.__clamping_notified)

    def __robot_position_notified (self):
        # Compute robot direction.
        direction = self.__get_robot_direction ()
        # Update bottom slots.
        changed = False
        for sloti in (self.SLOT_FRONT_BOTTOM, self.SLOT_BACK_BOTTOM):
            slot = self.slots[sloti]
            if direction == slot.side or direction is None:
                # If pushing, can take new elements.
                if slot.pawn is None:
                    p = self.__get_floor_elements (slot.side)
                    if p is not None:
                        slot.pawn = p
                        p.pos = None
                        p.notify ()
                        changed = True
            else:
                # Else, can drop elements.
                if slot.pawn is not None and slot.door_motor.angle:
                    m = TransMatrix ()
                    m.translate (self.robot_position.pos)
                    m.rotate (self.robot_position.angle)
                    xoffset = (self.BAY_OFFSET, -self.BAY_OFFSET)[slot.side]
                    slot.pawn.pos = m.apply ((xoffset, 0))
                    slot.pawn.notify ()
                    slot.pawn = None
                    changed = True
        if changed:
            self.update_contacts ()
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
                    slot.pawn = None
            elif self.clamping == self.CLAMPING_STROKE \
                    and self.load is not None:
                # Unload an element.
                slot = self.__get_clamp_slot ()
                if slot and slot.pawn is None:
                    slot.pawn = self.load
                    self.load = None
            # Little resources saving hack: done here, all motors are notified
            # at the same time.
            self.check_tower ()
            self.update_contacts ()
        self.notify ()

    def check_tower (self):
        """Check whether several elements can make a tower."""
        for slots in (self.front_slots, self.back_slots):
            if slots[0].pawn is not None and slots[1].pawn is not None:
                assert slots[0].pawn.kind != 'tower'
                tower = RoundObstacle (100, 1)
                tower.kind = 'tower'
                tower.tower = [ slots[0].pawn, slots[1].pawn ]
                slots[0].pawn, slots[1].pawn = tower, None
                self.table.add_pawn (tower)
            if slots[0].pawn is not None and slots[0].pawn.kind == 'tower' \
                    and slots[2].pawn and slots[2].door_motor.angle:
                slots[0].pawn.tower.append (slots[2].pawn)
                slots[2].pawn = None

    def update_contacts (self):
        """Update pawn contacts."""
        for slots in (self.front_slots, self.back_slots):
            slots[0].contact.state = not (slots[0].pawn is not None)
            # A tower at level 0 is seen at level 1.
            slots[1].contact.state = not (
                    slots[1].pawn is not None
                    or (slots[0].pawn is not None
                        and slots[0].pawn.kind == 'tower'))
            # This one is really high.
            slots[2].contact.state = not (slots[2].pawn is not None)
        slot_side = self.slots[self.SLOT_SIDE]
        slot_side.contact.state = slot_side.pawn is None
        clamp_slot = self.__get_clamp_slot ()
        if clamp_slot is not None:
            clamp_slot.contact.state = False
        for slot in self.slots:
            slot.contact.notify ()

    def __get_floor_elements (self, side):
        """Return an elements in front (side = 0) or in back (side = 1) of the
        robot, on the floor."""
        if self.robot_position.pos is None:
            return None
        # Matrix to transform an obstacle position into robot coordinates.
        m = self.__get_robot_matrix ()
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

    def __get_robot_direction (self):
        """Is robot going forward (0), backward (1), or something else
        (None)?"""
        if self.robot_position.pos is None:
            return None
        filt = 5
        if hasattr (self, 'old_robot_position'):
            m = self.__get_robot_matrix ()
            oldrel = m.apply (self.old_robot_position)
            if oldrel[0] < 0 and self.direction_counter < filt:
                self.direction_counter += 1
            elif oldrel[0] > 0 and self.direction_counter > -filt:
                self.direction_counter -= 1
        else:
            self.direction_counter = 0
        self.old_robot_position = self.robot_position.pos
        # Filter oscillations.
        if self.direction_counter > 0:
            return 0
        elif self.direction_counter < 0:
            return 1
        else:
            return None

    def __get_robot_matrix (self):
        """Return robot transformation matrix."""
        m = TransMatrix ()
        m.rotate (-self.robot_position.angle)
        m.translate ((-self.robot_position.pos[0],
            -self.robot_position.pos[1]))
        return m
