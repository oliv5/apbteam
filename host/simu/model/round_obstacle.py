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

class RoundObstacle:

    def __init__ (self, radius, level = 0):
        self.pos = None
        self.radius = radius
        self.level = level

    def intersect (self, a, b):
        """If the segment [AB] intersects the obstacle, return distance from a
        to intersection point, else, return None."""
        ab = sqrt ((b[0] - a[0]) ** 2 + (b[1] - a[1]) ** 2) # distance AB.
        n = ((b[0] - a[0]) / ab, (b[1] - a[1]) / ab) # vector of length 1.
        o = self.pos # obstacle center.
        # To check if the line (AB) intersects the circle, compute distance
        # from circle center to line using a dot product.
        vao = (o[0] - a[0], o[1] - a[1]) # vector AO.
        # dot product, (-n[1], n[0]) is perpendicular to n.
        doc = abs (vao[0] * -n[1] + vao[1] * n[0])
        if doc < self.radius:
            # Line intersects, check if segment intersects.
            m = vao[0] * n[0] + vao[1] * n[1]
            f = sqrt (self.radius ** 2 - doc ** 2)
            if m - f > 0 and m - f < ab:
                return m - f
            elif m + f > 0 and m + f < ab:
                return m + f
        return None

