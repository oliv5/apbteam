# Triangle - Triangulation beacon system study. {{{
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

def solve (Ba, Bb, alphaa, alphab):
    """Solve using angles."""
    l = norm (Ba - Bb)
    d = l * sin (alphaa) * sin (alphab) / sin (alphaa + alphab)
    b = d / tan (alphaa)
    v_n = Bb - Ba
    v_n = v_n / norm (v_n)
    o = Ba + v_n * b + rotate (v_n) * d
    return o

def trace (Ba, Bb, o, f, output, factor):
    """Trace f()[output] * factor for alphan corresponding to the point o."""
    # Compute angles.
    alphaa = angle (Ba, Bb, o)
    alphab = angle (Bb, o, Ba)
    # Return.
    return f (Ba, Bb, alphaa, alphab)[output] * factor

def compute_prec (Ba, Bb, o, f, prec):
    """Return an aproximation of distance error with the given angle error."""
    # Compute angles.
    alphaa = angle (Ba, Bb, o)
    alphab = angle (Bb, o, Ba)
    # Evaluate error.
    e = 0
    for i in ((-1, -1), (-1, 1), (1, -1), (1, 1)):
        o2 = f (Ba, Bb, alphaa + i[0] * prec, alphab + i[1] * prec)
        e = max (norm (o - o2), e)
    return e

if __name__ == '__main__':
    # Parameters.
    Ba = B1
    Bb = B2
    method = solve
    prec = 2 * pi / 720
    plot = 'prec'
    style = 'map'
    hardcopy = None
    zrange = None
    # Setup gnuplot.
    persist = False
    g = Gnuplot.Gnuplot (persist = persist)
    g ('set term x11')
    g ('set style data lines')
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
    if plot == 'prec':
        g ('set cntrparam levels discrete 5, 10, 20, 30, 40, 50, 100, 150, 200, 500')
        g ('set cbrange [0:100]')
        g.splot (Gnuplot.funcutils.compute_GridData (x, y,
            lambda x, y: compute_prec (Ba, Bb, array ([x, y]), method, prec), binary=0))
    else:
        g.splot (Gnuplot.funcutils.compute_GridData (x, y,
            lambda x, y: trace (Ba, Bb, array ([x, y]), method, plot, 1), binary=0))
    # Hardcopy:
    if hardcopy:
        g.hardcopy (filename = hardcopy, terminal = 'png')
    if not persist:
        raw_input ("Pause...")
