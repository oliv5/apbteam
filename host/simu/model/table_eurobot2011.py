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
"""Table model for Eurobot 2011."""
import simu.model.table
from random import randrange
from simu.model.round_obstacle import RoundObstacle

class Table (simu.model.table.Table):

    def __init__ (self, cards = None):
        simu.model.table.Table.__init__ (self)
        # Draw cards.
        cards = [ ]
        while len (cards) != 3:
            card = randrange (20)
            if card not in cards:
                cards.append (card)
        self.cards = cards
        def pos (card):
            king_pos = card // 4
            queen_pos = card % 4
            if queen_pos >= king_pos:
                queen_pos += 1
            return (king_pos, queen_pos)
        # Well, this is a boring write only code which create every elements.
        self.pawns = [ ]
        # Put pawns in green zones.
        green_pos = pos (cards[0])
        for i in xrange (5):
            if i == green_pos[0]:
                kind = 'king'
            elif i == green_pos[1]:
                kind = 'queen'
            else:
                kind = 'pawn'
            pawn = RoundObstacle (100, 1)
            pawn.pos = (200, 10 + 280 * (5 - i))
            pawn.kind = kind
            self.pawns.append (pawn)
            pawn = RoundObstacle (100, 1)
            pawn.pos = (3000 - 200, 10 + 280 * (5 - i))
            pawn.kind = kind
            self.pawns.append (pawn)
        # Put pawns in playing zone.
        kind = 'pawn'
        for j in xrange (2):
            play_pos = pos (cards[2 - j])
            for i in xrange (5):
                if i in play_pos:
                    pawn = RoundObstacle (100, 1)
                    pawn.pos = (1500 - 350 * (1 + j), 350 * (5 - i))
                    pawn.kind = kind
                    self.pawns.append (pawn)
                    pawn = RoundObstacle (100, 1)
                    pawn.pos = (1500 + 350 * (1 + j), 350 * (5 - i))
                    pawn.kind = kind
                    self.pawns.append (pawn)
        # Put center pawn.
        kind = 'pawn'
        pawn = RoundObstacle (100, 1)
        pawn.pos = (1500, 350 * 3)
        pawn.kind = kind
        self.pawns.append (pawn)
        # Add everything to obstacles.
        self.obstacles += self.pawns

    def add_pawn (self, pawn):
        self.pawns.append (pawn)
        self.obstacles.append (pawn)
        self.notify ()

