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
"""Guybrush clamps."""
from utils.observable import Observable
from simu.utils.trans_matrix import TransMatrix
from simu.utils.vector import vector
from math import pi
import random

class Clamps (Observable):

    def __init__ (self, table, robot_position, lower_clamp_motor,
            lower_clamp_cylinders, lower_clamp_sensors,
            upper_clamp_up_down_cylinder, upper_clamp_in_out_cylinder,
            upper_clamp_open_cylinder, door_cylinder):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.lower_clamp_motor = lower_clamp_motor
        self.lower_clamp_cylinders = lower_clamp_cylinders
        self.lower_clamp_sensors = lower_clamp_sensors
        self.lower_clamp_clamping = [ None, None ]
        self.lower_clamp_content = [ [ ], [ ] ]
        self.upper_clamp_up_down_cylinder = upper_clamp_up_down_cylinder
        self.upper_clamp_in_out_cylinder = upper_clamp_in_out_cylinder
        self.upper_clamp_open_cylinder = upper_clamp_open_cylinder
        self.upper_clamp_content = [ ]
        self.door_cylinder = door_cylinder
        self.load = [ ]
        self.lower_clamp_motor.register (self.__lower_clamp_notified)
        for c in self.lower_clamp_cylinders:
            c.register (self.__lower_clamp_notified)
        self.upper_clamp_up_down_cylinder.register (self.__upper_clamp_notified)
        self.upper_clamp_in_out_cylinder.register (self.__upper_clamp_notified)
        self.upper_clamp_open_cylinder.register (self.__upper_clamp_notified)
        self.door_cylinder.register (self.__door_notified)
        self.robot_position.register (self.__robot_position_notified)

    def __robot_position_notified (self):
        if self.__compute_lower_clamp ():
            self.notify ()

    def __lower_clamp_notified (self):
        self.lower_clamp_rotation = self.lower_clamp_motor.angle
        for i, c in enumerate (self.lower_clamp_cylinders):
            if c.pos is None:
                self.lower_clamp_clamping[i] = None
            else:
                self.lower_clamp_clamping[i] = ((c.pos - c.pos_in)
                        / (c.pos_out - c.pos_in))
        self.__compute_lower_clamp ()
        self.notify ()

    def __upper_clamp_notified (self):
        self.upper_clamp_up_down = 1 - self.upper_clamp_up_down_cylinder.pos # 1. is down.
        self.upper_clamp_in_out = self.upper_clamp_in_out_cylinder.pos # 1. is out.
        c = self.upper_clamp_open_cylinder
        if c.pos is None:
            self.upper_clamp_clamping = None
        else:
            self.upper_clamp_clamping = c.pos / c.pos_out
        self.__compute_upper_clamp ()
        self.notify ()

    def __door_notified (self):
        self.door = self.door_cylinder.pos # 1. is open.
        if self.door > 0.5 and self.load:
            for e in self.load:
                e.pos = (vector (self.robot_position.pos)
                        - vector.polar (self.robot_position.angle
                            + random.uniform (-pi/8, pi/8),
                            200 + random.uniform (0, 70)))
                e.notify ()
            self.load = [ ]
        self.notify ()

    def __compute_lower_clamp (self):
        """Compute all operations of lower clamps."""
        changed = False
        if self.lower_clamp_rotation is None:
            return False
        # If a lower clamp is on the floor:
        rot_mod = self.lower_clamp_rotation % (2 * pi)
        if rot_mod < pi * 0.2 or rot_mod >= pi * 1.9:
            floor_clamp = 0
        elif rot_mod >= pi * 0.9 and rot_mod < pi * 1.2:
            floor_clamp = 1
        else:
            floor_clamp = None
        if floor_clamp is not None:
            top_clamp = 1 - floor_clamp
            # Examine floor and add found element to the clamp content.
            for e, y in self.__get_floor_elements ():
                self.lower_clamp_content[floor_clamp].append ((e, y))
                e.pos = None
                e.notify ()
                changed = True
            # Drop element if possible.
            if self.lower_clamp_clamping[top_clamp] > 0.1:
                self.__add_load ([ e
                    for e, y in self.lower_clamp_content[top_clamp]])
                self.lower_clamp_content[top_clamp] = [ ]
                changed = True
        # Update sensors view.
        old_state = self.lower_clamp_sensors[0].state
        if floor_clamp is None:
            new_state = True
        else:
            new_state = not self.lower_clamp_content[floor_clamp]
        if new_state != old_state:
            for s in self.lower_clamp_sensors:
                s.state = new_state
                s.notify ()
        return changed

    def __compute_upper_clamp (self):
        """Compute all operations of upper clamps."""
        if (self.upper_clamp_up_down is None
                or self.upper_clamp_in_out is None
                or self.upper_clamp_clamping is None):
            return
        if (self.upper_clamp_clamping < 0.1
                and self.upper_clamp_up_down > 0.9):
            # Examine elements in front of the robot.
            for e, y in self.__get_upper_elements ():
                self.upper_clamp_content.append ((e, y))
                e.pos = None
                e.notify ()
        if (self.upper_clamp_clamping > 0.9
                and self.upper_clamp_up_down < 0.1):
            # Drop elements.
            self.__add_load ([ e for e, y in self.upper_clamp_content])
            self.upper_clamp_content = [ ]

    def __add_load (self, elements):
        """Add element list to load."""
        for e in elements:
            e.level = 1
        self.load.extend (elements)

    def __get_floor_elements (self):
        """Return an elements in front of the robot, on the floor, with its y
        coordinate in robot base."""
        if self.robot_position.pos is None:
            return
        # Matrix to transform an obstacle position into robot coordinates.
        m = self.__get_robot_matrix ()
        # Look up elements.
        xoffset = 117 - 6
        xmargin = 25
        ymargin = 171.5 - 60
        for o in self.table.obstacles:
            if o.level == 1 and o.value <= 1 and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > xoffset + o.radius - xmargin
                        and pos[0] < xoffset + o.radius + xmargin
                        and pos[1] > -ymargin and pos[1] < ymargin):
                    yield (o, pos[1])

    def __get_upper_elements (self):
        """Return elements in front of the robot in the upper clamps, with its
        y coordinate in the robot base."""
        if self.robot_position.pos is None:
            return
        # Matrix to transform an obstacle position into robot coordinates.
        m = self.__get_robot_matrix ()
        # Look up elements.
        xoffset = 150
        xmargin = 20
        ymargin2 = 20
        ymargin3 = 171.5
        for o in self.table.obstacles:
            if o.level == 2 and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > xoffset + 35 - xmargin
                        and pos[0] < xoffset + 35 + xmargin
                        and pos[1] > -ymargin2 and pos[1] < ymargin2):
                    yield (o, 0)
            if o.level == 3 and hasattr (o, 'value') and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > xoffset + o.radius - xmargin
                        and pos[0] < xoffset + o.radius + xmargin
                        and pos[1] > -ymargin3 and pos[1] < ymargin3):
                    yield (o, pos[1])

    def __get_robot_matrix (self):
        """Return robot transformation matrix."""
        m = TransMatrix ()
        m.rotate (-self.robot_position.angle)
        m.translate ((-self.robot_position.pos[0],
            -self.robot_position.pos[1]))
        return m

