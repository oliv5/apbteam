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
"""Obstacle with a round shape."""
from math import pi, cos, sin, sqrt
from utils.observable import Observable
from simu.utils.vector import vector

class RoundObstacle (Observable):

    def __init__ (self, radius, level = 0):
        Observable.__init__ (self)
        self.pos = None
        self.radius = radius
        self.level = level

    def intersect (self, a, b):
        """If the segment [AB] intersects the obstacle, return distance from a
        to intersection point, else, return None."""
        if self.pos is None:
            return None
        a, b = vector (a), vector (b)
        vab = b - a
        ab = abs (vab) # distance AB.
        n = vab.unit () # vector of length 1.
        o = vector (self.pos) # obstacle center.
        # To check if the line (AB) intersects the circle, compute distance
        # from circle center to line using a dot product.
        vao = o - a # vector AO.
        # abs of dot product.
        doc = abs (vao * n.normal ())
        if doc < self.radius:
            # Line intersects, check if segment intersects.
            m = vao * n
            f = sqrt (self.radius ** 2 - doc ** 2)
            if m - f > 0 and m - f < ab:
                return m - f
            elif m + f > 0 and m + f < ab:
                return m + f
        return None

