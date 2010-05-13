# simu - Robot simulation. {{{
#
# Copyright (C) 2010 Nicolas Schodet
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
"""Marcel food loader."""
from utils.observable import Observable
from simu.utils.trans_matrix import TransMatrix
from math import pi

class Loader (Observable):

    CLAMP_PULLEY_RADIUS = 6.5
    CLAMP_WIDTH = 198
    CLAMP_LENGTH = 75

    ELEVATOR_PULLEY_RADIUS = 11.15
    ELEVATOR_LINEAR_STROKE = 32
    ELEVATOR_ROTATING_STROKE = 200 - 32

    GATE_STROKE = -3.0124 * 2 * pi

    FRONT_ZONE_X_MIN = 120 - 15
    FRONT_ZONE_X_MAX = 120 + CLAMP_LENGTH - 15

    def __init__ (self, table, robot_position, left_clamp_link, right_clamp_link,
            elevator_link, gate_link, element_contacts):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.clamp_link = (left_clamp_link, right_clamp_link)
        self.elevator_link = elevator_link
        self.gate_link = gate_link
        self.element_contacts = element_contacts
        self.element_contacts[1].state = True
        self.element_contacts[1].notify ()
        self.clamp_load = [ ]
        self.load = [ ]
        self.front_elements = [ ]
        self.tickness = None
        self.clamp_pos = [ None, None ]
        self.robot_position.register (self.__robot_position_notified)
        self.clamp_link[0].register (self.__left_clamp_notified)
        self.clamp_link[1].register (self.__right_clamp_notified)
        self.elevator_link.register (self.__elevator_notified)
        self.gate_link.register (self.__gate_notified)

    def __robot_position_notified (self):
        c = self.element_contacts[0]
        if self.clamp_load:
            if c.state:
                c.state = False
                c.notify ()
        elif (self.robot_position.pos is not None
                and self.elevator_height < self.ELEVATOR_LINEAR_STROKE):
            # Is there elements?
            self.front_elements = self.__get_front_elements ()
            # Update contact.
            if c.state and self.front_elements:
                c.state = False
                c.notify ()
            elif not c.state and not self.front_elements:
                c.state = True
                c.notify ()
        else:
            if not c.state:
                c.state = True
                c.notify ()
        # Update motors limits.
        elements = self.clamp_load if self.clamp_load else self.front_elements
        tickness = sum (e.radius * 2 for e in elements)
        if tickness != self.tickness:
            limit = (((self.CLAMP_WIDTH - tickness) * 0.5 + 5)
                    / self.CLAMP_PULLEY_RADIUS)
            for l in self.clamp_link:
                l.limits.max = limit
                l.limits.notify ()
            self.tickness = tickness

    def __elevator_notified (self):
        if self.elevator_link.angle is None:
            self.elevator_height = None
            self.elevator_angle = None
        else:
            # Update elevator position.
            self.elevator_height = (self.elevator_link.angle
                    * self.ELEVATOR_PULLEY_RADIUS)
            if self.elevator_height < self.ELEVATOR_LINEAR_STROKE:
                self.elevator_angle = 0
            else:
                self.elevator_angle = ((self.elevator_height -
                    self.ELEVATOR_LINEAR_STROKE)
                    / self.ELEVATOR_ROTATING_STROKE * pi)
            # If elevator low, may detect new elements.
            if self.elevator_height < self.ELEVATOR_LINEAR_STROKE:
                self.__robot_position_notified ()
        self.notify ()

    def __clamp_notified (self, clamp):
        if self.clamp_link[clamp].angle is None:
            self.clamp_pos[clamp] = None
        else:
            # Update clamp position.
            self.clamp_pos[clamp] = (self.clamp_link[clamp].angle
                    * self.CLAMP_PULLEY_RADIUS)
            # If elevator is low, pick elements.
            if (not self.clamp_load
                    and self.elevator_height < self.ELEVATOR_LINEAR_STROKE
                    and self.front_elements):
                elements = self.front_elements
                black = sum (hasattr (e, 'black') and e.black for e in
                        elements)
                tickness = sum (e.radius * 2 for e in elements)
                if (not black and self.CLAMP_WIDTH - sum (self.clamp_pos)
                        <= tickness):
                    for e in elements:
                        e.pos = None
                        e.notify ()
                    self.clamp_load = elements
                    self.front_elements = [ ]
            # If elevator is high, drop elements.
            if (self.clamp_load
                    and self.elevator_height > self.ELEVATOR_LINEAR_STROKE +
                    self.ELEVATOR_ROTATING_STROKE * 2 / 3):
                tickness = sum (e.radius * 2 for e in self.clamp_load)
                if self.CLAMP_WIDTH - sum (self.clamp_pos) > tickness:
                    self.load += self.clamp_load
                    self.clamp_load = [ ]
                    self.__robot_position_notified ()
        self.notify ()

    def __left_clamp_notified (self):
        self.__clamp_notified (0)

    def __right_clamp_notified (self):
        self.__clamp_notified (1)

    def __gate_notified (self):
        self.gate_angle = self.gate_link.angle
        if self.gate_angle is not None and self.robot_position is not None:
            # If gate is high, drop elements.
            if self.load and self.gate_angle > self.GATE_STROKE / 2:
                m = TransMatrix ()
                m.rotate (self.robot_position.angle)
                m.translate (self.robot_position.pos)
                pos = m.apply ((-250, 0))
                for e in self.load:
                    e.pos = pos
                    e.notify ()
                self.load = [ ]
        self.notify ()

    def __get_front_elements (self):
        """Return a list of elements in front of the robot, between clamp."""
        elements = [ ]
        if (self.robot_position is None
                or self.clamp_pos[0] is None
                or self.clamp_pos[1] is None):
            return elements
        # Matrix to transform an obstacle position into robot coordinates.
        m = TransMatrix ()
        m.translate ((-self.robot_position.pos[0],
            -self.robot_position.pos[1]))
        m.rotate (-self.robot_position.angle)
        # Look up elements.
        # This could be used if clamp blocking is handled or elements are
        # pushed:
        #ymin = -self.CLAMP_WIDTH / 2 + self.clamp_pos[1]
        #ymax = self.CLAMP_WIDTH / 2 - self.clamp_pos[0]
        ymin = -self.CLAMP_WIDTH + 20
        ymax = self.CLAMP_WIDTH - 20
        for o in self.table.obstacles:
            if o.level == 1 and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > self.FRONT_ZONE_X_MIN
                        and pos[0] < self.FRONT_ZONE_X_MAX
                        and pos[1] > ymin and pos[1] < ymax):
                    elements.append (o)
        return elements
