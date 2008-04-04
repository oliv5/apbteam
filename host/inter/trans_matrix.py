# inter - Robot simulation interface. {{{
#
# Copyright (C) 2008 Nicolas Schodet
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
"""TransMatrix utility."""
from math import sin, cos, sqrt, atan2

class TransMatrix:
    """Define a matrix to be used for transformations on the plane.
    
    This is a "special" kind of matrix, because the last column is omitted as
    it is always (0, 0, 1)."""

    IDENTITY = ((1, 0), (0, 1), (0, 0))

    def __init__ (self, *m):
	"""Initialise the matrix, with optional initial value.
	
	>>> TransMatrix ()
	((1, 0), (0, 1), (0, 0))
	>>> TransMatrix ((1, 2), (3, 4), (5, 6))
	((1, 2), (3, 4), (5, 6))
	"""
	if m:
	    assert len (m) == 3
	    for i in m:
		assert len (i) == 2
	    self.matrix = m
	else:
	    self.matrix = self.IDENTITY

    def identity (self):
	"""Set to identity.

	>>> a = TransMatrix ()
	>>> a.translate ((2, 3))
	>>> a.identity (); a
	((1, 0), (0, 1), (0, 0))
	"""
	self.matrix = self.IDENTITY

    def rotate (self, angle):
	"""Transform the current matrix to do a rotation.
	
	>>> from math import pi
	>>> a = TransMatrix ()
	>>> a.rotate (pi / 3); a         # doctest: +ELLIPSIS
	((0.5..., 0.866...), (-0.866..., 0.5...), (0.0, 0.0))
	"""
	s = sin (angle)
	c = cos (angle)
	m = TransMatrix ((c, s), (-s, c), (0, 0))
	self *= m

    def translate (self, by):
	"""Transform the current matrix to do a translation.
	
	>>> a = TransMatrix ()
	>>> a.translate ((2, 3)); a
	((1, 0), (0, 1), (2, 3))
	"""
	m = TransMatrix ((1, 0), (0, 1), by)
	self *= m

    def scale (self, factor):
	"""Transform the current matrix to do a scaling.
	
	>>> a = TransMatrix ()
	>>> a.scale (2); a
	((2, 0), (0, 2), (0, 0))
	"""
	m = TransMatrix ((factor, 0), (0, factor), (0, 0))
	self *= m

    def __imul__ (self, other):
	"""Multiply by an other matrix.
	
	>>> a = TransMatrix ((1, 0), (0, 1), (1, 0))
	>>> b = TransMatrix ((0, 1), (1, 0), (0, 1))
	>>> a *= b; a
	((0, 1), (1, 0), (0, 2))
	"""
	s = self.matrix
	o = other.matrix
	self.matrix = (
		(s[0][0] * o[0][0] + s[0][1] * o[1][0],
		    s[0][0] * o[0][1] + s[0][1] * o[1][1]),
		(s[1][0] * o[0][0] + s[1][1] * o[1][0],
		    s[1][0] * o[0][1] + s[1][1] * o[1][1]),
		(s[2][0] * o[0][0] + s[2][1] * o[1][0] + o[2][0],
		    s[2][0] * o[0][1] + s[2][1] * o[1][1] + o[2][1]))
	return self

    def apply (self, *args):
	"""Apply (multiply) the matrix to all the given arguments.

	>>> m = TransMatrix ((1, 2), (4, 8), (16, 32))
	>>> m.apply ((1, 0))
	(17, 34)
	>>> m.apply ((0, 1), (1, 1))
	((20, 40), (21, 42))
	"""
	r = tuple (
	    (i[0] * self.matrix[0][0] + i[1] * self.matrix[1][0]
		+ self.matrix[2][0],
		i[0] * self.matrix[0][1] + i[1] * self.matrix[1][1]
		+ self.matrix[2][1])
	    for i in args)
	if len (args) == 1:
	    return r[0]
	else:
	    return r

    def apply_angle (self, angle):
	"""Apply the matrix to an angle.

	>>> from math import pi
	>>> a = TransMatrix ()
	>>> a.rotate (pi / 6)
	>>> a.translate ((2, 3))
	>>> a.scale (4)
	>>> a.apply_angle (pi / 6), pi / 3    # doctest: +ELLIPSIS
	(1.0471..., 1.0471...)
	"""
	o, m = self.apply ((0, 0), (cos (angle), sin (angle)))
	v = (m[0] - o[0], m[1] - o[1])
	vl = sqrt (v[0] ** 2 + v[1] ** 2)
	v = (v[0] / vl, v[1] / vl)
	return atan2 (v[1], v[0])

    def apply_distance (self, distance):
	"""Apply the matrix to a distance.

	>>> from math import pi
	>>> a = TransMatrix ()
	>>> a.rotate (pi / 6)
	>>> a.translate ((2, 3))
	>>> a.scale (4)
	>>> round (a.apply_distance (2))
	8.0
	"""
	o, m = self.apply ((0, 0), (distance, 0))
	v = (m[0] - o[0], m[1] - o[1])
	vl = sqrt (v[0] ** 2 + v[1] ** 2)
	return vl

    def __repr__ (self):
	return self.matrix.__repr__ ()

def _test ():
    import doctest
    doctest.testmod ()

if __name__ == '__main__':
    _test()
