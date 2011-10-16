# simu - Robot simulation. {{{
# encoding: utf-8
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
"""Intersection between different kind of primitives."""
from simu.utils.vector import vector

def segment_segment (a, b, c, d):
    """Find intersection between [AB] and [CD] line segments.  Return i such
    that A + i * (B - A).unit () gives this intersection.  Return None if no
    intersection found.

    If line segments are parallel or collinear, None is returned.
    """
    a, b, c, d = vector (a), vector (b), vector (c), vector (d)
    # For each point P on the line segment [AB], there is a real u in [0, 1]
    # for which P = A + u (B - A)
    #
    # An intersection point must be on both line segments:
    #
    # A + u (B - A) = C + v (D - C)
    #
    # xa + u (xb - xa) = xc + v (xd - xc)
    # ya + u (yb - ya) = yc + v (yd - yc)
    # 
    #     (xc - xa) (yd - yc) - (xd - xc) (yc - ya)
    # u = -----------------------------------------
    #     (xb - xa) (yd - yc) - (xd - xc) (yb - ya)
    #     (xc - xa) (yb - ya) - (xb - xa) (yc - ya)
    # v = -----------------------------------------
    #     (xb - xa) (yd - yc) - (xd - xc) (yb - ya)
    #
    # u = (vac . vcd.normal()) / (vab . vcd.normal())
    # v = (vac . vab.normal()) / (vab . vcd.normal())
    #
    # If vab . vcd.normal () is 0, AB and CD are parallel.
    vab = b - a
    vcd = d - c
    vac = c - a
    # Cannot test for 0 because we are using float, cannot test for a very
    # small number because we do not know what is small enough, therefore,
    # compare with numerator.
    den = vab * vcd.normal ()
    unum = vac * vcd.normal ()
    if abs (den) <= 1e-6 * abs (unum):
        return None
    else:
        u = unum / den
        if u >= 0 and u <= 1:
            v = vac * vab.normal () / den
            if v >= 0 and v <= 1:
                return u * vab.norm ()
    return None

if __name__ == '__main__':
    import sys
    import math
    v00 = vector (0, 0)
    v02 = vector (0, 2)
    v20 = vector (2, 0)
    v22 = vector (2, 2)
    v44 = vector (4, 4)
    failed = 0
    verbose = True
    def check (test, result):
        r = eval (test)
        if r != result:
            print test, 'is', r, 'but expected', result
            return 1
        elif verbose:
            print test, 'is', r, 'as expected'
            return 0
    v2 = math.sqrt (2)
    failed += check ('segment_segment (v00, v22, v20, v02)', v2)
    failed += check ('segment_segment (v00, v22, v02, v20)', v2)
    failed += check ('segment_segment (v22, v00, v20, v02)', v2)
    failed += check ('segment_segment (v22, v00, v02, v20)', v2)
    failed += check ('segment_segment (v00, v22, v00, v22)', None)
    failed += check ('segment_segment (v00, v22, v00, v44)', None)
    failed += check ('segment_segment (v22, v44, v02, v20)', None)
    failed += check ('segment_segment (v00, v44, v02, v20)', v2)
    failed += check ('segment_segment (v44, v00, v02, v20)', 3 * v2)
    sys.exit (0 if not failed else 1)
