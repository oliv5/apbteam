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
"""Generic distance sensor model."""
from math import cos, sin
from simu.utils.trans_matrix import TransMatrix

class DistanceSensor:

    def __init__ (self, table, pos, angle, range, into = None, level = 0):
        self.table = table
        self.pos = pos
        self.angle = angle
        self.range = range
        self.target = (pos[0] + cos (angle) * range,
                pos[1] + sin (angle) * range)
        self.into = into or ()
        self.level = level
        self.distance = None

    def evaluate (self):
        # Transform in the table base.
        pos, target = self.pos, self.target
        m = TransMatrix ()
        for i in self.into:
            if i.pos is None:
                self.distance = None
                return
            m.translate (i.pos)
            m.rotate (i.angle)
        pos, target = m.apply (pos, target)
        # Find intersection.
        i = self.table.intersect (pos, target, level = self.level,
                comp = lambda a, b: a < b)
        if i is not None:
            self.distance = i.distance
        else:
            self.distance = None

if __name__ == '__main__':
    from simu.model.table import Table
    from simu.model.round_obstacle import RoundObstacle
    from math import pi
    t = Table ()
    ro1 = RoundObstacle (0.5)
    ro1.pos = (0, 0)
    t.obstacles.append (ro1)
    ro2 = RoundObstacle (1)
    ro2.pos = (1, 0)
    t.obstacles.append (ro2)
    ds = DistanceSensor (t, (-1, 0), 0, 1)
    ds.evaluate ()
    assert ds.distance == 0.5
    ds = DistanceSensor (t, (-1, -1), 0, 1)
    ds.evaluate ()
    assert ds.distance is None
    ds = DistanceSensor (t, (3, 0), pi, 3)
    ds.evaluate ()
    assert ds.distance == 1.0
