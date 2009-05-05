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
"""Very basic motor model."""
from utils.observable import Observable

class MotorBasic (Observable):

    def __init__ (self, link, scheduler,
            speed, min_stop = None, max_stop = None):
        """Motor parameters:
         - speed: rad/s for a 1.0 value.
         - min_stop, max_stop: rad, mechanical stops."""
        Observable.__init__ (self)
        self.scheduler = scheduler
        self.speed = speed
        self.min_stop = min_stop
        self.max_stop = max_stop
        self.angle = 0
        self.link = link
        self.link.register (self.__notified)
        self.value = None
        self.last_update = self.scheduler.date
        self.__notified ()
        self.__timed_update ()

    def __notified (self):
        # Update angle.
        self.__update ()
        # Update value.
        self.value = self.link.value
        self.notify ()

    def __timed_update (self):
        # Update angle, reset limit.
        self.limit = False
        self.__update ()
        self.notify ()
        # Restart timer.
        self.scheduler.schedule (self.scheduler.date
                + int (self.scheduler.tick * 0.1), self.__timed_update)

    def __update (self):
        delta_t = (float (self.scheduler.date - self.last_update) /
                self.scheduler.tick)
        v = self.value or 0
        self.angle += v * self.speed * delta_t
        self.last_update = self.scheduler.date
        if self.angle < self.min_stop:
            self.angle = self.min_stop
            self.limit = True
        elif self.angle > self.max_stop:
            self.angle = self.max_stop
            self.limit = True

