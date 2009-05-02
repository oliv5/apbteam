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
"""Table model for Eurobot 2009."""
import simu.model.table
from random import randint
from simu.model.round_obstacle import RoundObstacle

class Table (simu.model.table.Table):

    def __init__ (self, pucks_card = None, dispensers_card = None):
        simu.model.table.Table.__init__ (self)
        # Draw cards.
        if pucks_card is None:
            pucks_card = randint (1, 10)
        if dispensers_card is None:
            dispensers_card = randint (1, 2)
        self.pucks_card = pucks_card
        self.dispensers_card = dispensers_card
        # Put pucks on the table.
        self.pucks = [ ]
        puck_n = { 1: (1, 2, 3), 2: (1, 3, 4), 3: (1, 3, 5), 4: (1, 3, 6),
                5: (2, 3, 4), 6: (2, 3, 5), 7: (2, 3, 6), 8: (3, 4, 5),
                9: (3, 4, 6), 10: (3, 5, 6) }
        for n in puck_n[pucks_card]:
            pos1 = ((n - 1) % 3, (n - 1) / 3)
            pos2 = (pos1[0], 3 - pos1[1])
            for pos in pos1, pos2:
                p = RoundObstacle (35, 1)
                p.pos = (1500 - 400 - 250 * pos[0], 1050 - 125 + 200 * pos[1])
                p.color = False
                self.pucks.append (p)
                p = RoundObstacle (35, 1)
                p.pos = (1500 + 400 + 250 * pos[0], 1050 - 125 + 200 * pos[1])
                p.color = True
                self.pucks.append (p)
        # Put pucks in dispensers.
        if self.dispensers_card == 1:
            ds = -250
        else:
            ds = +250
        for pos, color in (
                ((289, 40), True),
                ((3000 - 289, 40), False),
                ((40, 1050 + ds), True),
                ((3000 - 40, 1050 + ds), False),
                ):
            for i in range (5):
                p = RoundObstacle (35, 1)
                p.pos = (pos[0] + i * 2, pos[1] + i * 2)
                p.color = color
                self.pucks.append (p)
        # Add pucks in obstacles.
        self.obstacles += self.pucks

