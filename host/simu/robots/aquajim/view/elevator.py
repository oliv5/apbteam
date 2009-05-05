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
"""AquaJim elevator."""
from simu.inter.drawable import Drawable

class Elevator (Drawable):

    width = 320 / 1.2
    height = 320

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)
        self.__notified ()
        self.door_model = model.elevator_door
        self.door_model.register (self.__door_notified)
        self.__door_notified ()

    def __notified (self):
        self.height = self.model.elevator_height
        self.update ()

    def __door_notified (self):
        m = self.door_model
        if m.angle is None:
            self.door_value = None
            self.door_limit = None
        else:
            self.door_value = m.angle / (m.max_stop - m.min_stop) - m.min_stop
            self.door_limit = self.door_model.limit
        self.update ()

    def draw (self):
        self.reset ()
        self.trans_translate ((-35, -150))
        self.draw_line ((-10, 300), (-10, 0), (70, 0), fill = '#808080')
        if self.height is not None:
            self.trans_translate ((0, self.height))
            self.draw_line ((0, 150), (0, 0), (70, 0))
            if self.door_value is not None:
                self.trans_translate ((5 + self.door_value * 70, 0))
                self.draw_line ((0, 120), (0, 0),
                        fill = self.door_limit and 'red' or 'black')

