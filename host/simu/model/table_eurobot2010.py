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
"""Table model for Eurobot 2010."""
import simu.model.table
from random import randint
from simu.model.round_obstacle import RoundObstacle

class Table (simu.model.table.Table):

    def __init__ (self, cards = None):
        simu.model.table.Table.__init__ (self)
        # Draw cards.
        if cards is None:
            cards = (randint (1, 9), randint (1, 4))
        self.cards = cards
        # Well, this is a boring write only code which create every elements.
        # Put corn on the table.
        self.corns = [ ]
        corns_pos = ((0, 5), (0, 3), (0, 1), (1, 4), (1, 2), (1, 0), (2, 3),
                (2, 1), (3, 0), (3, 2))
        black_corns_cards = (
                (None, (1, 4), (0, 4), (2, 4), (2, 3), (0, 3), (1, 3), (1, 6),
                    (0, 6), (2, 6)),
                (None, (5, 8), (7, 8), (5, 9), (7, 9)),
                )
        black_corns = (black_corns_cards[0][cards[0]] +
                black_corns_cards[1][cards[1]])
        for i in xrange (len (corns_pos)):
            if corns_pos[i][0] == 3:
                poss = (corns_pos[i], )
            else:
                poss = (corns_pos[i], (6 - corns_pos[i][0], corns_pos[i][1]))
            for pos in poss:
                corn = RoundObstacle (25, 1)
                corn.pos = (150 + pos[0] * 450, 128 + pos[1] * 250)
                corn.black = i in black_corns
                self.corns.append (corn)
        # Put tomatos on the table.
        self.tomatos = [ ]
        tomatos_pos = ((0, 4), (0, 2), (1, 3), (1, 1), (2, 2), (2, 0), (3, 3),
                (3, 1))
        for tomato_pos in tomatos_pos:
            if tomato_pos[0] == 3:
                poss = (tomato_pos, )
            else:
                poss = (tomato_pos, (6 - tomato_pos[0], tomato_pos[1]))
            for pos in poss:
                tomato = RoundObstacle (50, 1)
                tomato.pos = (150 + pos[0] * 450, 128 + pos[1] * 250)
                self.tomatos.append (tomato)
        # Put oranges.
        self.oranges = [ ]
        self.oranges_pos = ((250 - 80, 70), (250 - 80 - 55, 70 + 75),
                (250 - 55, 70 + 75 + 50), (250 - 80, 70 + 75 + 50 + 100),
                (250 - 80 - 55, 70 + 75 + 50 + 100 + 75),
                (250 - 55, 70 + 75 + 50 + 100 + 75 + 50))
        for pos in self.oranges_pos:
            orange = RoundObstacle (50, 2)
            orange.pos = (1500 - pos[0], 2100 - pos[1])
            self.oranges.append (orange)
            orange = RoundObstacle (50, 2)
            orange.pos = (1500 + pos[0], 2100 - pos[1])
            self.oranges.append (orange)
        # Add everything to obstacles.
        self.obstacles += self.corns
        self.obstacles += self.tomatos
        self.obstacles += self.oranges

