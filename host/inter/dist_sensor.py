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
"""Distance sensor."""

from Tkinter import *
from drawable import *

from math import pi, cos, sin, sqrt

class DistSensor (Drawable):
    """A distance sensor."""

    def __init__ (self, onto, pos, angle, range):
	Drawable.__init__ (self, onto)
	self.pos = pos
	self.angle = angle
	self.range = range
	self.target = (pos[0] + cos (angle) * range,
		pos[1] + sin (angle) * range)
	self.obstacles = [ ]
	self.distance = None
	self.hide = False

    def compute (self):
	# Could do better by trans_applying until on the same drawable as the
	# obstacle.
	pos = self.onto.trans_apply (self.pos)
	target = self.onto.trans_apply (self.target)
	n = ((target[0] - pos[0]) / self.range,
		(target[1] - pos[1]) / self.range)
	self.distance = None
	for o in self.obstacles:
	    # Does the line intersect with the obstacle?
	    ao = (o.pos[0] - pos[0], o.pos[1] - pos[1])
	    doc = abs (ao[0] * -n[1] + ao[1] * n[0])
	    if doc < o.radius:
		# Does the segment intersect?
		m = ao[0] * n[0] + ao[1] * n[1]
		f = sqrt (o.radius ** 2 - doc ** 2)
		if m - f > 0 and m - f < self.range:
		    d = m - f
		    if self.distance is None or self.distance > d:
			self.distance = d
		elif m + f > 0 and m + f < self.range:
		    d = m + f
		    if self.distance is None or self.distance > d:
			self.distance = d
	return self.distance

    def draw (self):
	self.compute ()
	self.reset ()
	if self.hide:
	    return
	if self.distance is None:
	    self.draw_line (self.pos, self.target, fill = 'blue', arrow = LAST)
	else:
	    inter = (self.pos[0] + cos (self.angle) * self.distance,
		    self.pos[1] + sin (self.angle) * self.distance)
	    self.draw_line (self.pos, inter, fill = 'red', arrow = LAST)
	    self.draw_line (inter, self.target, fill = 'blue', arrow = LAST)
