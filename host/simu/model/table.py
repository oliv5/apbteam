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
"""Table model."""
from utils.observable import Observable

class Intersect:

    def __init__ (self, obstacle, distance):
        self.obstacle = obstacle
        self.distance = distance

class Table (Observable):

    def __init__ (self):
        Observable.__init__ (self)
        self.obstacles = [ ]

    def intersect (self, a, b, level = None, comp = None, exclude = None):
        i = None
        for o in self.obstacles:
            if ((level is None or level == o.level)
                    and (exclude is None or not exclude (o))):
                d = o.intersect (a, b)
                if d is not None and (i is None or comp (d, i.distance)):
                    i = Intersect (o, d)
                    if comp is None:
                        return i
        return i

    def nearest (self, pos, level = None, max = None, exclude = None):
        """Return nearest object."""
        no = None
        nds = None
        for o in self.obstacles:
            if (o.pos is not None and (level is None or level == o.level)
                    and (exclude is None or not exclude (o))):
                ds = (pos[0] - o.pos[0]) ** 2 + (pos[1] - o.pos[1]) ** 2
                if (max is None or ds < max ** 2) and (nds is None or ds < nds):
                    no = o
                    nds = ds
        return no

