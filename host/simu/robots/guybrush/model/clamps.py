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

class Clamps (Observable):

    def __init__ (self, table, robot_position, lower_clamp_motor,
            lower_clamp_cylinders):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.lower_clamp_motor = lower_clamp_motor
        self.lower_clamp_cylinders = lower_clamp_cylinders
        self.lower_clamp_clamping = [ None, None ]
        self.lower_clamp_motor.register (self.__lower_clamp_notified)
        for c in self.lower_clamp_cylinders:
            c.register (self.__lower_clamp_notified)

    def __lower_clamp_notified (self):
        self.lower_clamp_rotation = self.lower_clamp_motor.angle
        for i, c in enumerate (self.lower_clamp_cylinders):
            if c.pos is None:
                self.lower_clamp_clamping[i] = None
            else:
                self.lower_clamp_clamping[i] = (1.0
                        - (c.pos - c.pos_in) / (c.pos_out - c.pos_in))
        self.notify ()
