# simu - Robot simulation. {{{
#
# Copyright (C) 2013 Nicolas Schodet
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
"""Sensopart FT20RA."""
from utils.observable import Observable
from simu.model.distance_sensor import DistanceSensor

class DistanceSensorTrig (Observable, DistanceSensor):

    RANGE = 100

    def __init__ (self, link, scheduler, table, pos, angle, into = None,
            level = 0, exclude = None, factor = 1):
        Observable.__init__ (self)
        DistanceSensor.__init__ (self, table, pos, angle, self.RANGE, into,
                level, exclude)
        self.factor = factor
        self.link = link
        self.scheduler = scheduler
        self.value = None
        self.evaluate ()
        self.register (self.__update)

    def evaluate (self):
        # Compute real distance.
        DistanceSensor.evaluate (self)
        # Convert to sensor voltage.
        d = self.distance
        if self.distance is None:
            d = self.RANGE
        if d < 22.5:
            self.value = -0.014 * d * d + 0.41 * d + 7
        else:
            self.value = -0.657 + 220.431 / d
        self.value *= 0.1 * self.factor
        # Update observers.
        self.notify ()
        # Prepare next update.
        self.scheduler.schedule (self.scheduler.date
                + int (self.scheduler.tick * 0.03), self.evaluate)

    def __update (self):
        self.link.value = self.value
        self.link.notify ()

