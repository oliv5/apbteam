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
"""Computed path drawing."""

from Tkinter import *
from simu.inter.drawable import *

from math import pi, cos, sin, sqrt

class Path (Drawable):
    """Computed path drawing."""

    def __init__ (self, onto):
        Drawable.__init__ (self, onto)
        self.path = [ ]

    def draw (self):
        self.reset ()
        if len (self.path) > 2:
            fmt = dict (fill = 'green', arrow = LAST)
            self.draw_line (*self.path, **fmt)
