# Triangle - Triangulation beacon system study. {{{
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
from math import *
from numpy import *
import Gnuplot, Gnuplot.funcutils

B1 = array ([3000, 0])
B2 = array ([3000, 2100])
B3 = array ([0, 2100 / 2])

def norm (v):
    """Compute vector norm."""
    return sqrt (sum (v ** 2))

def rotate (v):
    """Rotate a vector by pi/2."""
    return array ([-v[1], v[0]])

def angle (o, a, b):
    """Compute angle (v_oa, v_ob)."""
    v_oa = a - o
    v_ob = b - o
    # Use dot product to compute angle.
    angle = acos (dot (v_oa, v_ob) / (norm (v_oa) * norm (v_ob)))
    # Use cross product to determine angle sign.
    if v_oa[0] * v_ob[1] - v_ob[0] * v_oa[1] >= 0:
        return angle
    else:
        return 2 * pi - angle

theta1 = angle (B1, B2, B3)
theta2 = angle (B2, B3, B1)
theta3 = angle (B3, B1, B2)
b1 = norm (B1 - B2)
b2 = norm (B2 - B3)
b3 = norm (B3 - B1)

def solve1 (alpha1, alpha2, alpha3):
    """Solve using beta1."""
    beta1 = atan (
            (-b1 + b3 * sin (alpha3 + theta2) / sin (alpha3))
            / (b1 * cos (alpha1) / sin (alpha1)
                + b3 * cos (alpha3 + theta1) / sin (alpha3))
            )
    a1 = sin (pi - beta1 - alpha1) * b1 / sin (alpha1)
    o = array ([B1[0] - sin (beta1) * a1, B1[1] + cos (beta1) * a1])
    return (o, beta1, a1)

def solve2 (alpha1, alpha2, alpha3):
    """Solve using beta2 (rotation of solve1)."""
    beta2 = atan (
            (-b2 + b1 * sin (alpha1 + theta3) / sin (alpha1))
            / (b2 * cos (alpha2) / sin (alpha2)
                + b1 * cos (alpha1 + theta2) / sin (alpha1))
            )
    a2 = sin (pi - beta2 - alpha2) * b2 / sin (alpha2)
    o = array ([B2[0] - sin (theta2 - beta2) * a2, B2[1] - cos (theta2 - beta2) * a2])
    return (o, beta2, a2)

def solve2m (alpha1, alpha2, alpha3):
    """Solve using gamma1 (mirror of solve1)."""
    gamma1 = atan (
            (-b1 + b2 * sin (alpha2 + theta1) / sin (alpha2))
            / (b1 * cos (alpha1) / sin (alpha1)
                + b2 * cos (alpha2 + theta2) / sin (alpha2))
            )
    a2 = sin (pi - gamma1 - alpha1) * b1 / sin (alpha1)
    o = array ([B2[0] - sin (gamma1) * a2, B2[1] - cos (gamma1) * a2])
    return (o, gamma1, a2)

def solve3 (alpha1, alpha2, alpha3):
    """Solve using beta3 (rotation of solve1)."""
    beta3 = atan (
            (-b3 + b2 * sin (alpha2 + theta1) / sin (alpha2))
            / (b3 * cos (alpha3) / sin (alpha3)
                + b2 * cos (alpha2 + theta3) / sin (alpha2))
            )
    a3 = sin (pi - beta3 - alpha3) * b3 / sin (alpha3)
    o = array ([B3[0] - cos (beta3 - theta3 / 2) * a3, B3[1] + sin (beta3 - theta3 / 2) * a3])
    return (o, beta3, a3)

def solve_cvra (alpha1, alpha2, alpha3):
    """Solve using CVRA method, slightly modified for simpler vector based
    computations."""
    # Find circle on which B1, B2 and o are located.
    v = (B2 - B1) / 2
    cc2 = B1 + v + rotate (v) / tan (alpha1)
    # Find circle on which B1, B3 and o are located.
    v = (B3 - B1) / 2
    cc1 = B1 + v - rotate (v) / tan (alpha3)
    # Find the circles intersection... knowing that B1 is a solution.
    v = cc2 - B1
    n = (cc1 - cc2) / norm (cc1 - cc2)
    o = B1 + (v + n * dot (-v, n)) * 2
    return (o, cc2[0], cc2[1], cc1[0], cc1[1])

def trace (o, f, output, factor):
    """Trace f()[output] * factor for alphan corresponding to the point o."""
    # Compute angles.
    alpha1 = angle (o, B1, B2)
    alpha2 = angle (o, B2, B3)
    alpha3 = angle (o, B3, B1)
    # Return.
    return f (alpha1, alpha2, alpha3)[output] * factor

def compute_prec (o, f, prec):
    """Return an aproximation of distance error with the given angle error."""
    # Compute angles.
    alpha1 = angle (o, B1, B2)
    alpha2 = angle (o, B2, B3)
    alpha3 = angle (o, B3, B1)
    # Evaluate error.
    e = 0
    for i in ((-1, -1, -1), (-1, -1, 1), (-1, 1, -1), (-1, 1, 1),
            (1, -1, -1), (1, -1, 1), (1, 1, -1), (1, 1, 1)):
        o2 = f (alpha1 + i[0] * prec, alpha2 + i[1] * prec, alpha3 + i[2] * prec)[0]
        e = max (norm (o - o2), e)
    return e

if __name__ == '__main__':
    # Parameters.
    method = solve_cvra
    prec = 2 * pi / 2200
    plot = 'prec'
    style = '3d'
    hardcopy = None
    zrange = (0, 150)
    # Setup gnuplot.
    persist = False
    g = Gnuplot.Gnuplot (persist = persist)
    g ('set term x11')
    g ('set data style lines')
    if style == '3d':
        pass
    else:
        g ('set view map')
        g ('set nosurface')
        if style == 'map':
            g ('set pm3d')
        elif style == 'iso':
            g ('set contour')
    g.set_range ('xrange', (0,3000))
    g.set_range ('yrange', (0,2100))
    if zrange:
        g.set_range ('zrange', zrange)
    x = arange (25, 3000, 50)
    y = arange (25, 2100, 50)
    # Plot:
    if plot == 'angle':
        g ('set cntrparam levels incremental 0, 5, 360')
        g.splot (Gnuplot.funcutils.compute_GridData (x, y,
            lambda x, y: trace (array ([x, y]), method, 1, 180 / pi), binary=0))
    elif plot == 'prec':
        g ('set cntrparam levels discrete 5, 10, 20, 30, 40, 50, 100, 150, 200, 500')
        g ('set cbrange [0:100]')
        g.splot (Gnuplot.funcutils.compute_GridData (x, y,
            lambda x, y: compute_prec (array ([x, y]), method, prec), binary=0))
    else:
        g.splot (Gnuplot.funcutils.compute_GridData (x, y,
            lambda x, y: trace (array ([x, y]), method, plot, 1), binary=0))
    # Hardcopy:
    if hardcopy:
        g.hardcopy (filename = hardcopy, terminal = 'png')
    if not persist:
        raw_input ("Pause...")
