# simu - Robot simulation. {{{
#
# Copyright (C) 2012 Nicolas Schodet
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
"""Table model for Eurobot 2013."""
import simu.model.table
from simu.model.round_obstacle import RoundObstacle
from simu.model.rectangular_obstacle import RectangularObstacle
from math import pi
import math
import random

class Table (simu.model.table.Table):

    def __init__ (self, cards = None):
        simu.model.table.Table.__init__ (self)
        # Candles.
        self.candles = [ ]
        def add_candle (pos, level, color):
            candle = RoundObstacle (40, level)
            candle.pos = pos
            candle.color = color
            candle.state = False
            self.candles.append (candle)
        def add_candle_circle (center, radius, start, step, colors, level):
            angle = start
            for color in colors:
                pos = (center[0] + radius * math.cos (angle),
                        center[1] + radius * math.sin (angle))
                add_candle (pos, level, color)
                angle += step
        colors_r = [ False, False, False, True, True, True ]
        random.shuffle (colors_r)
        colors = [ False ] + colors_r + [ True ]
        add_candle_circle ((1500, 2000), 350, pi + pi / 16, pi / 8, colors, 3)
        colors_r = [ False, False, False, True, True, True ]
        random.shuffle (colors_r)
        colors = ([ False ] + colors_r[0:3] + [ None, None, None, None ]
                + colors_r[3:6] + [ True ])
        add_candle_circle ((1500, 2000), 450, pi + pi / 24, pi / 12, colors, 2)
        cake = RoundObstacle (500, 0)
        cake.pos = (1500, 2000)
        # Glasses.
        self.glasses = [ ]
        def add_glass (pos):
            glass = RoundObstacle (40, 0)
            glass.pos = pos
            self.glasses.append (glass)
            glass = RoundObstacle (40, 0)
            glass.pos = (3000 - pos[0], pos[1])
            self.glasses.append (glass)
        add_glass ((900, 550))
        add_glass ((1200, 550))
        add_glass ((1050, 800))
        add_glass ((1350, 800))
        add_glass ((900, 1050))
        add_glass ((1200, 1050))
        # Cherries.
        self.plates = [ ]
        self.cherries = [ ]
        def add_plate (pos, color):
            plate = RectangularObstacle ((170, 170), 0)
            plate.pos = pos
            plate.angle = 0
            self.plates.append (plate)
            cpos = ((-42, -42), (-42, 0), (-42, +42), (0, -21), (0, 21),
                    (42, -42), (42, 0), (42, +42))
            ccol = [ color ] + 7 * [ None ]
            random.shuffle (ccol)
            for p, c in zip (cpos, ccol):
                cherry = RoundObstacle (20, 0)
                cherry.pos = (pos[0] + p[0], pos[1] + p[1])
                cherry.color = c
                self.cherries.append (cherry)
        for py in (250, 600, 1000, 1400, 1750):
            add_plate ((200, py), False)
            add_plate ((3000 - 200, py), True)
        # Gifts.
        self.gifts = [ ]
        def add_gift (pos, color):
            gift = RectangularObstacle ((150, 50), 0)
            gift.pos = pos
            gift.color = color
            gift.state = False
            self.gifts.append (gift)
        def add_gifts (pos):
            add_gift ((pos[0] - 176 / 2, pos[1] - 72), False)
            add_gift ((pos[0] + 176 / 2, pos[1] - 72), True)
        add_gifts ((600, 0))
        add_gifts ((1200, 0))
        add_gifts ((1800, 0))
        add_gifts ((2400, 0))
        # Add everything to obstacles.
        self.obstacles.append (cake)
        self.obstacles += self.candles
        self.obstacles += self.glasses
        self.obstacles += self.plates
        self.obstacles += self.cherries
        self.obstacles += self.gifts

