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
"""Obstacle with a polygonal shape."""
from utils.observable import Observable
from simu.utils.vector import vector
from simu.utils.trans_matrix import TransMatrix
from math import pi
import simu.utils.intersect

class PolygonalObstacle (Observable):

    def __init__ (self, *points, **kwargs):
        Observable.__init__ (self)
        self.pos = None
        self.angle = 0
        self.points = points
        self.level = 0
        for i in kwargs:
            if i == 'level':
                self.level = kwargs[i]
            else:
                raise TypeError ("unexpected keyword argument")

    def intersect (self, a, b):
        """If the segment [AB] intersects the obstacle, return distance from a
        to intersection point, else, return None."""
        if self.pos is None or self.angle is None:
            return None
        # Find intersection with each segments.  Return the nearest.
        m = TransMatrix ()
        m.translate (self.pos)
        m.rotate (self.angle)
        p = m.apply (*self.points)
        found = None
        def iter_seg (p):
            for i in xrange (len (p) - 1):
                yield p[i], p[i + 1]
            yield p[-1], p[0]
        for c, d in iter_seg (p):
            i = simu.utils.intersect.segment_segment (a, b, c, d)
            if i is not None:
                if found is not None:
                    found = min (found, i)
                    break
                else:
                    found = i
        return found

