# simu - Robot simulation. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
"""Table model for Eurobot 2012."""
import simu.model.table
from simu.model.round_obstacle import RoundObstacle
from simu.model.rectangular_obstacle import RectangularObstacle
from math import pi
import math
import random

class Table (simu.model.table.Table):

    def __init__ (self, cards = None):
        simu.model.table.Table.__init__ (self)
        # Well, this is a boring write only code which create every elements.
        # Add coins.
        self.coins = [ ]
        def add_coin (pos, angle, level = 1):
            coin = RoundObstacle (60, level)
            coin.pos = pos
            coin.angle = angle
            coin.value = 1
            self.coins.append (coin)
        def add_coin_circle (center, radius, start, step, n, level = 1):
            angle = start
            for i in xrange (n):
                pos = (center[0] + radius * math.cos (angle),
                        center[1] + radius * math.sin (angle))
                add_coin (pos, angle, level)
                angle += step
        add_coin ((1000, 1500), 0)
        add_coin ((2000, 1500), pi)
        add_coin ((450, 300), pi)
        add_coin ((3000 - 450, 300), 0)
        add_coin_circle ((1500, 300), 90, 0, pi / 2, 4)
        add_coin_circle ((1500 - 400, 1000), 300 - 60, pi / 4, pi / 4, 7)
        add_coin_circle ((1500 + 400, 1000), 300 - 60, pi + pi / 4, pi / 4, 7)
        add_coin_circle ((1500 - 400, 1000), 115, pi / 4, pi / 2, 4)
        add_coin_circle ((1500 + 400, 1000), 115, pi / 4, pi / 2, 4)
        add_coin_circle ((1500 - 400, 1000), 105, pi / 4, pi / 2, 4, 3)
        add_coin_circle ((1500 + 400, 1000), 105, pi / 4, pi / 2, 4, 3)
        # Add gold bars.
        self.gold_bars = [ ]
        def add_gold_bar (pos, angle, level = 1):
            gold_bar = RectangularObstacle ((150, 70), level)
            gold_bar.pos = pos
            gold_bar.angle = angle
            gold_bar.value = 3
            self.gold_bars.append (gold_bar)
        add_gold_bar ((1500, 647), 0)
        add_gold_bar ((1500 - 400, 1000 + 125 - 35), 0, 2)
        add_gold_bar ((1500 + 400, 1000 + 125 - 35), 0, 2)
        add_gold_bar ((1500 - 400, 1000 - 125 + 35), 0, 2)
        add_gold_bar ((1500 + 400, 1000 - 125 + 35), 0, 2)
        ba = pi / 2 - 0.04995839
        cba = math.cos (ba)
        sba = math.sin (ba)
        gbx = 400 - (285 + 75) * cba + 35
        gby = 1500 - (285 + 75) * sba
        add_gold_bar ((gbx, gby), ba)
        add_gold_bar ((3000 - gbx, gby), pi - ba)
        # Set random black coins.
        nblack = 0
        while nblack < 4:
            coin = random.choice (self.coins[2:])
            if coin.value:
                coin.value = 0
                nblack += 1
        # Add everything to obstacles.
        self.obstacles += self.coins
        self.obstacles += self.gold_bars
        # Add totem as obstacles.
        for i in (-1, 1):
            o = RectangularObstacle ((250, 250), 3)
            o.pos = (1500 + i * 400, 1000)
            self.obstacles.append (o)

