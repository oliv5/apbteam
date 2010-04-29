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

    CLAMP_PULLEY_RADIUS = 5.5
    CLAMP_WIDTH = 250
    CLAMP_LENGTH = 100

    ELEVATOR_PULLEY_RADIUS = 10
    ELEVATOR_LINEAR_STROKE = 50
    ELEVATOR_ROTATING_STROKE = 100

    FRONT_ZONE_X_MIN = 120 + 15
    FRONT_ZONE_X_MAX = 120 + CLAMP_LENGTH - 15

    def __init__ (self, table, robot_position, left_clamp_link, right_clamp_link,
            elevator_link):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.clamp_link = (left_clamp_link, right_clamp_link)
        self.elevator_link = elevator_link
        self.clamp_load = [ ]
        self.load = [ ]
        self.clamp_pos = [ None, None ]
        self.clamp_link[0].register (self.__left_clamp_notified)
        self.clamp_link[1].register (self.__right_clamp_notified)
        self.elevator_link.register (self.__elevator_notified)

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
                    and self.elevator_height < self.ELEVATOR_LINEAR_STROKE):
                elements = self.__get_front_elements ()
                if elements:
                    black = sum (hasattr (e, 'black') and e.black for e in
                            elements)
                    tickness = sum (e.radius * 2 for e in elements)
                    if (not black and self.CLAMP_WIDTH - sum (self.clamp_pos)
                            <= tickness):
                        for e in elements:
                            e.pos = None
                            e.notify ()
                        self.clamp_load = elements
            # If elevator is high, drop elements.
            if (self.clamp_load
                    and self.elevator_height > self.ELEVATOR_LINEAR_STROKE +
                    self.ELEVATOR_ROTATING_STROKE * 2 / 3):
                tickness = sum (e.radius * 2 for e in self.clamp_load)
                if self.CLAMP_WIDTH - sum (self.clamp_pos) > tickness:
                    self.load += self.clamp_load
                    self.clamp_load = [ ]
        self.notify ()

    def __left_clamp_notified (self):
        self.__clamp_notified (0)

    def __right_clamp_notified (self):
        self.__clamp_notified (1)

    def __get_front_elements (self):
        """Return a list of elements in front of the robot, between clamp."""
        elements = [ ]
        if (self.robot_position is None
                or self.clamp_pos[0] is None
                or self.clamp_pos[1] is None):
            return None
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
        ymin = -self.CLAMP_WIDTH + 40
        ymax = self.CLAMP_WIDTH - 40
        for o in self.table.obstacles:
            if o.level == 1 and o.pos is not None:
                pos = m.apply (o.pos)
                if (pos[0] > self.FRONT_ZONE_X_MIN
                        and pos[0] < self.FRONT_ZONE_X_MAX
                        and pos[1] > ymin and pos[1] < ymax):
                    elements.append (o)
        return elements
