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
"""Sensopart UT20 model."""
from utils.observable import Observable
from simu.model.distance_sensor import DistanceSensor
from math import pi

class DistanceSensorSensopart (Observable):

    # Nominal range of the sensor.
    RANGE = 700
    # Reflection quality.  If 1, the sensor will see at most exactly to the
    # nominal distance, which is almost never the case, it is always more than
    # that.
    QUALITY = 1.6
    # Secondary rays.  The simulated sensor will use several lines to sense
    # objects, this are for each secondary line, its angle and distance ratio
    # (see sensor area diagram).
    SECONDARY = ((0.07, 0.8), (0.28, 0.4))
    # Calibrated values.
    MIN = 100
    MAX = 700
    # Output levels for MIN/MAX.
    OMIN = 250 * 0.004
    OMAX = 250 * 0.020

    def __init__ (self, link, scheduler, table, pos, angle, into = None,
            level = 0, exclude = None):
        Observable.__init__ (self)
        self.rays = [ ]
        range = self.RANGE * self.QUALITY
        self.rays.append (DistanceSensorSensopartRay (table, pos, angle,
            range, into, level, exclude))
        for s in self.SECONDARY:
            for i in (-1, 1):
                self.rays.append (DistanceSensorSensopartRay (table, pos,
                    angle + s[0] * i, range * s[1], into, level, exclude))
        self.link = link
        self.scheduler = scheduler
        self.value = None
        self.evaluate ()
        self.register (self.__update)

    def evaluate (self):
        # Compute real distance.
        d = None
        for r in self.rays:
            r.evaluate ()
            if r.distance is not None and (d is None or r.distance < d):
                d = r.distance
        # Convert to voltage.
        if d is None or d > self.MAX:
            self.value = self.OMAX
        elif d < self.MIN:
            self.value = self.OMIN
        else:
            self.value = (self.OMIN
                    + (d - self.MIN) / (self.MAX - self.MIN)
                    * (self.OMAX - self.OMIN))
        # Update observers.
        self.notify ()
        # Prepare next update.
        self.scheduler.schedule (self.scheduler.date
                + int (self.scheduler.tick * 0.030), self.evaluate)

    def __update (self):
        self.link.value = self.value
        self.link.notify ()

class DistanceSensorSensopartRay (Observable, DistanceSensor):

    def __init__ (self, *args):
        Observable.__init__ (self)
        DistanceSensor.__init__ (self, *args)

    def evaluate (self):
        old = self.distance
        # Compute real distance.
        DistanceSensor.evaluate (self)
        # Update observers.
        if self.distance != old:
            self.notify ()

