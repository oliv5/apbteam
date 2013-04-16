# simu - Robot simulation. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
"""Obstacle with a rectangular shape."""
from utils.observable import Observable
from simu.utils.vector import vector
from math import pi
import simu.utils.intersect

class RectangularObstacle (Observable):

    def __init__ (self, dim, level = 0):
        Observable.__init__ (self)
        self.pos = None
        self.angle = 0
        self.dim = dim
        self.level = level

    def intersect (self, a, b):
        """If the segment [AB] intersects the obstacle, return distance from a
        to intersection point, else, return None."""
        if self.pos is None or self.angle is None:
            return None
        # Find intersection with each rectangle segments.  There is at most
        # two intersections, return the nearest.
        u = vector.polar (self.angle, self.dim[0] / 2.)
        v = vector.polar (self.angle + pi / 2, self.dim[1] / 2.)
        o = vector (self.pos)
        p1 = o + u + v
        p2 = o - u + v
        p3 = o - u - v
        p4 = o + u - v
        found = None
        for c, d in ((p1, p2), (p2, p3), (p3, p4), (p4, p1)):
            i = simu.utils.intersect.segment_segment (a, b, c, d)
            if i is not None:
                if found is not None:
                    found = min (found, i)
                    break
                else:
                    found = i
        return found

    def inside (self, a):
        """If A is inside obstacle, return True."""
        # Map point in obstacle coordinates.
        u = vector.polar (self.angle, 1)
        o = vector (self.pos)
        a = vector (a)
        oa = a - o
        x = oa * u / (.5 * self.dim[0])
        y = oa * u.normal () / (.5 * self.dim[1])
        return x > -1 and x < 1 and y > -1 and y < 1

