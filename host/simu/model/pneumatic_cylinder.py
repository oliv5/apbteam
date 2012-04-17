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
"""Pneumatic cylinder model."""
from utils.observable import Observable

class PneumaticCylinder (Observable):

    def __init__ (self, link_in, link_out, scheduler,
            pos_in, pos_out, speed_in, speed_out, init_pos,
            switch_in = None, switch_out = None):
        """Cylinder parameters:
         - link_in, link_out: output conected to valve, if one is none, use
           simple effect.
         - pos_in, pos_out: maximum position in each direction (minimum and
           maximum length) (mm).
         - speed_in, speed_out: linear speed (mm/s).
         - init_pos: initial position (mm).
         - switch_in, switch_out: limit switch models if present."""
        Observable.__init__ (self)
        self.link_in, self.link_out = link_in, link_out
        self.scheduler = scheduler
        self.pos_in, self.pos_out = pos_in, pos_out
        self.speed_in, self.speed_out = speed_in, speed_out
        self.switch_in, self.switch_out = switch_in, switch_out
        self.last_update = self.scheduler.date
        self.speed = 0
        self.pos = init_pos
        if self.link_in:
            self.link_in.register (self.__notified)
        if self.link_out:
            self.link_out.register (self.__notified)
        self.__timed_update ()

    def __notified (self):
        # Update position.
        self.__update ()
        # Update current speed.
        if self.link_in and self.link_out:
            i = self.link_in.state
            o = self.link_out.state
        elif self.link_in:
            i = self.link_in.state
            o = not i
        elif self.link_out:
            o = self.link_out.state
            i = not o
        if i == o:
            self.speed = 0
        elif i:
            self.speed = -self.speed_in
        elif o:
            self.speed = self.speed_out

    def __timed_update (self):
        # Update position.
        self.__update ()
        # Restart timer.
        self.scheduler.schedule (self.scheduler.date
                + int (self.scheduler.tick * 0.1), self.__timed_update)

    def __update (self):
        delta_t = (float (self.scheduler.date - self.last_update) /
                self.scheduler.tick)
        old_pos = self.pos
        self.pos += self.speed * delta_t
        self.last_update = self.scheduler.date
        if self.pos < self.pos_in:
            self.pos = self.pos_in
        elif self.pos > self.pos_out:
            self.pos = self.pos_out
        if old_pos != self.pos:
            self.notify ()
            if self.switch_in:
                self.switch_in.state = self.pos == self.pos_in
                self.switch_in.notify ()
            if self.switch_out:
                self.switch_out.state = self.pos == self.pos_out
                self.switch_out.notify ()

